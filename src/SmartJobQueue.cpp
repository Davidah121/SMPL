#include "SimpleJobQueue.h"
#include "System.h"

namespace smpl
{
    SmartJobQueue::SmartJobQueue(int minThreads, int maxThreads)
    {
        running = true;
        analysisThread = std::thread(&SmartJobQueue::analysisThreadRun, this);
        maxThreadsAllowed = maxThreads; 
        for(size_t i=0; i<minThreads; i++)
        {
            jobsInProgress[threadID] = SIZE_MAX;
            threadDone[threadID] = false;
            jobThreads[threadID] = new std::thread(&SmartJobQueue::threadRun, this, threadID, false);
            threadID++;
        }
    }
    
    SmartJobQueue::~SmartJobQueue()
    {
        jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
        running = false;
        jobQueueMutex.unlock();

        cv.notify_all();
        analysisThread.join();
        for(auto p : jobThreads)
        {
            p.second->join();
            delete p.second;
        }

        jobThreads.clear();
        jobsInProgress.clear();
        threadDone.clear();
        jobsDoneSinceCheck = 0;
        threadID = 0;
        threadLoad = 0;
        jobID = 0;
    }

    size_t SmartJobQueue::addJob(std::function<void()> j, double priority)
    {
        JobInfo info;
        info.func = j;
        jobQueueMutex.lock();
        info.ID = jobID++;
        jobs.add(priority, info);
        jobQueueMutex.unlock();

        cv.notify_all();
        return info.ID;
    }
    

    void SmartJobQueue::removeJob(size_t ID)
    {
        JobInfo info;
        info.ID = ID;

        jobQueueMutex.lock();
        jobs.erase(info);
        jobQueueMutex.unlock();
    }

    bool SmartJobQueue::jobDone(size_t ID)
    {
        bool found = false;
        jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);

        //check if its in the list of inprogress functions
        for(auto o : jobsInProgress)
        {
            if(o.second == ID)
            {
                found = true;
                break;
            }
        }
        if(found)
        {
            jobQueueMutex.unlock();
            return false;
        }
        
        //check if its in the list of remaining functions
        JobInfo temp = {ID, nullptr};
        if(jobs.find(temp))
        {
            jobQueueMutex.unlock();
            return false;
        }

        jobQueueMutex.unlock();
        return true;
    }
    
    bool SmartJobQueue::allJobsDone()
    {
        bool finishedEverything = true;
        jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
        for(auto o : jobsInProgress)
        {
            if(o.second != SIZE_MAX)
            {
                finishedEverything = false;
                break;
            }
        }
        
        if(jobs.size() > 0)
            finishedEverything = false;
        
        jobQueueMutex.unlock();
        return finishedEverything;
    }
    
    void SmartJobQueue::removeAllJobs()
    {
        jobQueueMutex.lock();
        jobs.clear();
        jobQueueMutex.unlock();
    }
    
    bool SmartJobQueue::getRunning()
    {
        jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
        bool res = running;
        jobQueueMutex.unlock();
        return res;
    }

    void SmartJobQueue::waitForAllJobs()
    {
        while(!allJobsDone())
        {
            //Otherwise, wait.
            System::sleep(1, 0, false);
        }
    }

    void SmartJobQueue::threadRun(size_t ID, bool temporary)
    {
        bool knownToHaveMoreWork = false;
        size_t lastTime = System::getCurrentTimeMillis();

        while(getRunning())
        {
            //try to get a job from the queue
            JobInfo node;

            
            if(!knownToHaveMoreWork)
            {
                std::unique_lock<std::mutex> temporaryLock(haltMutex);
                if(temporary)
                {
                    auto waitStatus = cv.wait_for(temporaryLock, std::chrono::milliseconds(100));
                    if(waitStatus == std::cv_status::timeout) //100 milliseconds of nothing. Probably not needed
                        break;
                }
                else
                {
                    cv.wait(temporaryLock);
                }
            }
            
            jobQueueMutex.lock(HybridSpinLock::MODE_STANDARD);
            bool gotStuff = jobs.get(node);
            if(gotStuff)
            {
                jobsInProgress[ID] = node.ID;
                threadLoad++;
            }

            knownToHaveMoreWork = !jobs.empty();
            jobQueueMutex.unlock();

            //execute job if we got one
            if(node.ID != SIZE_MAX && node.func != nullptr)
            {
                node.func();
                lastTime = System::getCurrentTimeMillis();
            }
            
            jobQueueMutex.lock(HybridSpinLock::MODE_STANDARD);
            jobsInProgress[ID] = SIZE_MAX;
            jobsDoneSinceCheck++;
            if(node.ID != SIZE_MAX && node.func != nullptr)
                threadLoad--; //actually got a function to execute
            jobQueueMutex.unlock();
        }

        jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
        threadDone[ID] = true;
        jobQueueMutex.unlock();
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
            jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);

            //readjust job queue priority. Lower priority jobs should be bumped up to prevent starvation

            //also, remove threads that are done if possible
            for(auto it : threadDone)
            {
                if(it.second == true)
                {
                    if(jobThreads[threadID] != nullptr)
                    {
                        jobThreads[threadID]->join();
                        delete jobThreads[threadID];
                    }
                    jobThreads.erase(threadID);
                }
            }

            double jobsCreated = jobs.size() - oldJobSize;
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
                jobsInProgress[threadID] = SIZE_MAX;
                threadDone[threadID] = false;
                jobThreads[threadID] = new std::thread(&SmartJobQueue::threadRun, this, threadID, true);
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
}