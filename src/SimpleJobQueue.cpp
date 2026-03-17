#include "SimpleJobQueue.h"
#include "Concurrency.h"
#include "FiberTask.h"
#include "StringTools.h"
#include "System.h"
#include <mutex>

namespace smpl
{
	SimpleJobQueue::SimpleJobQueue(int threads)
	{
		running = true;

		createdTask = std::vector<FiberTask*>();
		jobThreads = std::vector<std::thread>(threads);
		postponedJobs = SimpleHashSet<JobTask, RapidHash<JobTask>, CompareJobTask>(); //no task in progress at the start.
		jobsBeingRun = SimpleHashSet<size_t>();
		
		for(int i=0; i<jobThreads.size(); i++)
		{
			jobThreads[i] = std::thread(&SimpleJobQueue::threadRun, this, i);
		}
	}
	
	SimpleJobQueue::~SimpleJobQueue()
	{
		jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
		running = false;
		jobQueueMutex.unlock();

		cv.notify_all();
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
		jobID = 0;
	}

	size_t SimpleJobQueue::addJob(const std::function<void()>& j)
	{
		jobQueueMutex.lock();
		JobInfo info = JobInfo(jobID++, j);
		jobs.add(info);
		jobQueueMutex.unlock();

		cv.notify_one();
		return info.getID();
	}
	

	void SimpleJobQueue::removeJob(size_t ID)
	{
		jobQueueMutex.lock();
		jobs.erase(JobInfo(ID, nullptr));
		jobQueueMutex.unlock();
	}

	bool SimpleJobQueue::jobDone(size_t ID)
	{
		bool found = false;
		std::lock_guard<HybridSpinLock> lock(jobQueueMutex);

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
	
	bool SimpleJobQueue::allJobsDone()
	{
		jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
		bool finishedEverything = (jobs.size() == 0) && postponedJobs.empty() && jobsBeingRun.empty();
		jobQueueMutex.unlock();
		return finishedEverything;
	}
	
	void SimpleJobQueue::removeAllJobs()
	{
		jobQueueMutex.lock();
		jobs.clear();
		jobQueueMutex.unlock();
	}
	
	bool SimpleJobQueue::getRunning()
	{
		jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
		bool res = running;
		jobQueueMutex.unlock();
		return res;
	}

	void SimpleJobQueue::waitForAllJobs()
	{
		while(!allJobsDone())
		{
			//Otherwise, wait.
			System::sleep(1, 0, false);
		}
	}

	void SimpleJobQueue::threadRun(int id)
	{
		bool knownToHaveMoreWork = false;
		bool taskPriority = false; //FALSE = job queue first. TRUE = Existing jobs first
		while(getRunning())
		{
			size_t jobID = -1;
			FiberTask* taskPtr = nullptr;

			//try to get a job from the queue
			if(!knownToHaveMoreWork)
			{
				std::mutex haltMutex;
				std::unique_lock<std::mutex> temporaryLock(haltMutex);
				cv.wait_for(temporaryLock, std::chrono::milliseconds(1));
			}
			
			jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
			
			auto taskIterator = postponedJobs.end();
			
			//check for jobs in the in progress list. These should be done with the same priority as the others
			if(taskPriority == true)
				taskIterator = postponedJobs.begin();
			

			if(jobs.size() > 0 && taskIterator == postponedJobs.end())
			{
				//get job and remove from queue. Mark as busy
				JobInfo& node = jobs.get();

				//create task from this node
				jobID = node.getID();
				taskPtr = getFreeFiberTask(node);
				jobs.popFront();
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
				jobsBeingRun.insert(jobID);
			
			knownToHaveMoreWork = jobs.size() > 0 || postponedJobs.size() >= jobThreads.size(); //either there is a new job or there is enough existing jobs 
			
			jobQueueMutex.unlock();

			//execute job if we got one
			if(jobID != SIZE_MAX && taskPtr != nullptr)
			{
				taskPtr->run();

				//check if the job is done.
				jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
				if(taskPtr->getStatus() == FiberTask::STATUS_DONE)
					availableFiberTask.push_back(taskPtr);
				else
				{
					postponedJobs.insert(JobTask(jobID, taskPtr)); //add it into the jobs in progress
					knownToHaveMoreWork = true; //Since we added it back in, you know for a fact that their is work left to do. You just may not get this task again. Matters more with a very small job queue.
				}
				jobsBeingRun.erase(jobID);
				jobQueueMutex.unlock();
			}
			
			//ensure fairness by allowing swaping priority of task execution (existing or new task first)
			taskPriority = !taskPriority;
		}
	}
	
	bool SimpleJobQueue::hasJobs()
	{
		jobQueueMutex.lock();
		bool v = jobs.size() != 0;
		jobQueueMutex.unlock();
		return !v;
	}

	
	FiberTask* SimpleJobQueue::getFreeFiberTask(const JobInfo& jbInfo)
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