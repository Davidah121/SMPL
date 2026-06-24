#include "SimpleJobQueue.h"
#include "System.h"
#include <thread>

namespace smpl
{
    SmartJobQueue::SmartJobQueue(size_t minThreads, size_t maxThreads)
    {
        running = true;
        analysisThread = std::thread(&SmartJobQueue::analysisThreadRun, this);

		if(minThreads <= 0 )
			minThreads = 1;
		jobThreads = std::vector<std::thread>(minThreads);

		createdTask = std::vector<FiberTask*>();
		postponedJobs = SimpleHashSet<JobTask, RapidHash<JobTask>, CompareJobTask>(); //no task in progress at the start.
		jobsBeingRun = SimpleHashSet<size_t>();

        maxThreadsAllowed = maxThreads; 
        for(size_t i=0; i<minThreads; i++)
        {
            jobThreads[threadID] = std::thread(&SmartJobQueue::threadRun, this, threadID, false);
            threadID++;
        }
    }
    
    SmartJobQueue::~SmartJobQueue()
    {
        running = false;
        cv.notify_all();
        analysisThread.join();
		for(int i=0; i<jobThreads.size(); i++)
		{
			jobThreads[i].join();
		}
		
		for(FiberTask* tPointer : createdTask)
		{
			if(tPointer != nullptr)
				delete tPointer;
		}

		createdTask.clear();
		availableFiberTask.clear();
		postponedJobs.clear();
		jobsBeingRun.clear();
		jobThreads.clear();
		jobs.clear();
        jobsDoneSinceCheck = 0;
        threadID = 0;
        threadLoad = 0;
        jobID = 0;
    }

    size_t SmartJobQueue::addJob(const std::function<void()>& j, double priority)
    {
		PQData<JobInfo> info = PQData<JobInfo>{priority, JobInfo(jobID++, j)};
		size_t tempID = info.data.getID();
        jobQueueMutex.lock();
		jobs.add(std::move(info));
        jobQueueMutex.unlock();

		knownJobCount++;
        cv.notify_one();
        return tempID;
    }
    
	size_t SmartJobQueue::addJobs(const std::vector<std::pair<std::function<void()>, double>>& jobList)
	{
		if(jobList.empty())
			return -1;

		jobQueueMutex.lock();
		size_t startID = jobID;
		for(size_t i=0; i<jobList.size(); i++)
		{
			jobs.add({jobList[i].second, JobInfo(jobID++, jobList[i].first)});
		}
		jobQueueMutex.unlock();

		knownJobCount+=jobList.size();
		// if(jobList.size() >= jobThreads.size())
		// 	cv.notify_all();
		// else
		// {
		// 	for(size_t i=0; i<jobList.size(); i++)
		// 	{
		// 		cv.notify_one();
		// 	}
		// }
		cv.notify_one();
		return startID;
	}

	void SmartJobQueue::parallelize(const std::function<void(size_t, size_t, size_t)>& func, size_t start, size_t end, size_t incr, size_t threadsWanted)
	{
		if(threadsWanted < 1)
			threadsWanted = jobThreads.size();
		if(threadsWanted == 1)
		{
			func(start, end, incr);
			return;
		}

		std::vector<std::function<void()>> jobsToAdd;
		size_t max = end;
		size_t indexIncr = (end-start)/threadsWanted;
		indexIncr -= indexIncr % incr;

		if(indexIncr < incr)
		{
			func(start, end, incr); //Change this behavior. Should reduce the total number of threads instead of making it all single threaded.
			return;
		}
		
		size_t tStart = start;
		size_t tEnd = start+indexIncr;
		for(size_t i=0; i<threadsWanted-1; i++)
		{
			addJob([func, tStart, tEnd, incr]() ->void{
				func(tStart, tEnd, incr);
			}, 0);
			
			tStart += indexIncr;
			tEnd += indexIncr;
		}

		tEnd = end;
		
		addJob([func, tStart, tEnd, incr]() ->void{
			func(tStart, tEnd, incr);
		}, 0);
		waitForAllJobs(); //should actually use a different approach
	}

    void SmartJobQueue::removeJob(size_t ID)
    {
		jobQueueMutex.lock();
		bool deletedSomething = jobs.erase(JobInfo(ID, nullptr));
		if(deletedSomething)
			knownJobCount--;
		jobQueueMutex.unlock();
    }

    bool SmartJobQueue::jobDone(size_t ID)
    {
		bool found = false;
		std::lock_guard<std::mutex> lock(jobQueueMutex);

		//check if its in the list of suspended jobs
		if(postponedJobs.find(ID) != postponedJobs.end())
			return false;

		//check if its in the list of inprogress jobs
		if(jobsBeingRun.find(ID) != jobsBeingRun.end())
			return false;

		//check if its in the list of jobs not queued up
		if(jobs.find({ID, nullptr}))
			return false;

		return true;
    }
    
    bool SmartJobQueue::allJobsDone()
    {
		jobQueueMutex.lock();
		bool finishedEverything = (jobs.size() == 0) && postponedJobs.empty() && jobsBeingRun.empty();
		jobQueueMutex.unlock();
		return finishedEverything;
    }
    
    void SmartJobQueue::removeAllJobs()
    {
        jobQueueMutex.lock();
		knownJobCount -= jobs.size(); //Not necessarily gonna go to 0. May have existing Task that have been started that were suspended. Can't get rid of those yet.
        jobs.clear();
        jobQueueMutex.unlock();
    }
    
    bool SmartJobQueue::getRunning()
    {
        return running;
    }

    void SmartJobQueue::waitForAllJobs()
    {
        while(!allJobsDone())
        {
			//Otherwise, wait.
			std::unique_lock<std::mutex> temporaryLock(jobWaitHaltMutex);
			jobWaitCV.wait_for(temporaryLock, std::chrono::milliseconds(1));
        }
    }

    void SmartJobQueue::threadRun(size_t ID, bool temporary)
    {
		bool taskPriority = false; //FALSE = job queue first. TRUE = Existing jobs first

        while(getRunning())
        {
            //try to get a job from the queue
			size_t jobID = -1;
			FiberTask* taskPtr = nullptr;
            
			//try to get a job from the queue
			if(knownJobCount > 0)
			{
				if(!jobQueueMutex.try_lock())
				{
					//Wait to be notified that the jobQueueMutex is free. Will almost certainly grab something out of the jobQueue
					std::unique_lock<std::mutex> temporaryLock(haltMutex);
					cv.wait_for(temporaryLock, std::chrono::milliseconds(1));
					jobQueueMutex.lock();
				}
			}
			else
			{
				//Wait to be notified of a job
				if(temporary)
				{
					std::unique_lock<std::mutex> temporaryLock(haltMutex);
					auto waitStatus = cv.wait_for(temporaryLock, std::chrono::milliseconds(100));
					if(waitStatus == std::cv_status::timeout) //100 milliseconds of nothing. Thread probably not needed anymore. Discard it
                        break;
				}
				else
				{
					std::unique_lock<std::mutex> temporaryLock(haltMutex);
					cv.wait_for(temporaryLock, std::chrono::milliseconds(1)); //may result in more fighting over the mutex but the mutex itself still results in a sleep if failed
					// cv.wait(temporaryLock);
				
				}
				jobQueueMutex.lock();
			}
			
			auto taskIterator = postponedJobs.end();
			
			//check for jobs in the in progress list. These should be done with the same priority as the others
			if(taskPriority == true)
				taskIterator = postponedJobs.begin();
			

			if(jobs.size() > 0 && taskIterator == postponedJobs.end())
			{
				//get job and remove from queue. Mark as busy
				JobInfo node;
				jobs.get(node);
				
				//create task from this node
				jobID = node.getID();
				taskPtr = getFreeFiberTask(node);
				jobs.pop();
			}

			if(taskPriority == false && taskIterator == postponedJobs.end() && jobID == -1)
				taskIterator = postponedJobs.begin();

			if(taskIterator != postponedJobs.end())
			{
				//if you got task iterator and didn't have to create one
				jobID = taskIterator->getID();
				taskPtr = &taskIterator->getTask();
				postponedJobs.erase(taskIterator); //remove the task from the list as we are working on it. (Can't have multiple threads trying to work on it at once)
			}

			if(jobID != -1)
			{
				jobsBeingRun.insert(jobID);
				knownJobCount--;
			}
			
			jobQueueMutex.unlock();
			if(knownJobCount > 0)
				cv.notify_one(); //notify anything that is waiting that its okay to grab the mutex

			//execute job if we got one
			if(jobID != SIZE_MAX && taskPtr != nullptr)
			{
				taskPtr->run();

				//check if the job is done.
				jobQueueMutex.lock();
				if(taskPtr->getStatus() == FiberTask::STATUS_DONE)
					availableFiberTask.push_back(taskPtr);
				else
				{
					postponedJobs.insert(JobTask(jobID, taskPtr)); //add it into the jobs in progress
					knownJobCount++;; //Since we added it back in, you know for a fact that their is work left to do. You just may not get this task again. Matters more with a very small job queue.
					// cv.notify_all(); //It is possible that we wish to awaken all threads to catch any strays that slip through unnoticed. This is mainly for temporary threads
				}
				jobsBeingRun.erase(jobID);
				jobQueueMutex.unlock();
				
				cv.notify_one();
				
				if(knownJobCount == 0)
					jobWaitCV.notify_one();
			}
			
			//ensure fairness by allowing swaping priority of task execution (existing or new task first)
			taskPriority = !taskPriority;
		}
    }

    void SmartJobQueue::analysisThreadRun()
    {
        //A thread that analyzes the jobs completed, jobs created, total threads allocated
        //and creates more threads if necessary.
        double oldJobSize = 0;
        System::sleep(1, 0, false);
        while(getRunning())
        {
            bool createNewThread = false;

            //check 
            jobQueueMutex.lock();

            //readjust job queue priority. Lower priority jobs should be bumped up to prevent starvation.... do this later?
			//TODO: Add job priority adjustments (Note that once a job has been started, priority does not matter. They are all treated the same)

            //No need to remove threads. They are detached so they will be disposed of automatically

            double jobsCreated = (jobs.size() + postponedJobs.size()) - oldJobSize;
            if(jobsCreated < 0)
                jobsCreated = 0;
            
            //jobsDone - jobsCreated < 0? add more threads
            //oldJobSize > currThreads && jobsInProgress >= .75%? add more threads
                //logic. Old jobs waiting is more than what we can do and we are overloaded with our current threads
                //if we are only using 50% or less of our threads, we can probably do these really quickly.
            if(jobsDoneSinceCheck - jobsCreated < 0)
                createNewThread = true;
            if(oldJobSize > jobThreads.size() && threadLoad >= jobThreads.size()*0.75)
                createNewThread = true;
            
            if(createNewThread && jobThreads.size() < maxThreadsAllowed)
            {
				std::thread t = std::thread(&SmartJobQueue::threadRun, this, threadID, true);
				t.detach(); //once detached, you no longer need to worry about it. Destructor will not break the thread. Thread will auto exit upon reaching the end of its function
				threadID++;
            }
            jobsDoneSinceCheck = 0;
            jobQueueMutex.unlock();
            System::sleep(1, 0, false); //ideally, sleep for a while.
        }
    }
    
    bool SmartJobQueue::hasJobs()
    {
        jobQueueMutex.lock();
        bool v = jobs.size() != 0;
        jobQueueMutex.unlock();
        return !v;
    }
	
	FiberTask* SmartJobQueue::getFreeFiberTask(const JobInfo& jbInfo)
	{
		//DO NOT LOCK. THIS SHOULD ONLY BE RUN FROM INSIDE AN EXISTING CRITICAL SECTION
		//attempt to grab an existing available fiber task.
		FiberTask* ptr = nullptr;
		if(!availableFiberTask.empty())
		{
			ptr = availableFiberTask.back();
			availableFiberTask.pop_back();
			ptr->changeTask(jbInfo.getFunction());
		}
		else
		{
			//create new task
			createdTask.push_back(new FiberTask(jbInfo.getFunction()));
			ptr = createdTask.back();
		}

		return ptr;
	}
}