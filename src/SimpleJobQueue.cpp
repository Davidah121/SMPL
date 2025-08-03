#include "SimpleJobQueue.h"
#include "System.h"

namespace smpl
{
    SimpleJobQueue::SimpleJobQueue(int threads)
    {
        running = true;
        jobThreads = std::vector<std::thread*>(threads);
        jobsInProgress = std::vector<size_t>(threads);
        
        for(int i=0; i<jobThreads.size(); i++)
        {
            jobsInProgress[i] = SIZE_MAX;
            jobThreads[i] = new std::thread(&SimpleJobQueue::threadRun, this, i);
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
            jobThreads[i]->join();
            delete jobThreads[i];
        }

        jobThreads.clear();
        jobsInProgress.clear();
        jobID = 0;
    }

    size_t SimpleJobQueue::addJob(std::function<void()> j)
    {
        JobInfo info;
        info.func = j;
        jobQueueMutex.lock();
        info.ID = jobID++;
        jobs.add(info);
        jobQueueMutex.unlock();

        cv.notify_all();
        return info.ID;
    }
    

    void SimpleJobQueue::removeJob(size_t ID)
    {
        JobInfo info;
        info.ID = ID;

        jobQueueMutex.lock();
        jobs.erase(info);
        jobQueueMutex.unlock();
    }

    bool SimpleJobQueue::jobDone(size_t ID)
    {
        bool found = false;
        jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);

        //check if its in the list of inprogress functions
        for(int i=0; i<jobsInProgress.size(); i++)
        {
            if(jobsInProgress[i] == ID)
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
        if(jobs.find({ID, nullptr}))
        {
            jobQueueMutex.unlock();
            return false;
        }

        jobQueueMutex.unlock();
        return true;
    }
    
    bool SimpleJobQueue::allJobsDone()
    {
        bool finishedEverything = true;
        jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
        for(int i=0; i<jobsInProgress.size(); i++)
        {
            if(jobsInProgress[i] != SIZE_MAX)
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
        while(getRunning())
        {
            //try to get a job from the queue
            JobInfo node;

            if(!knownToHaveMoreWork)
            {
                std::unique_lock<std::mutex> temporaryLock(haltMutex);
                cv.wait(temporaryLock);
            }
            
            jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
            if(jobs.size() > 0)
            {
                //get job and remove from queue. Mark as busy
                node = jobs.get();
                jobs.popFront();
                jobsInProgress[id] = node.ID;
            }

            knownToHaveMoreWork = jobs.size() > 0;
            jobQueueMutex.unlock();

            //execute job if we got one
            if(node.ID != SIZE_MAX && node.func != nullptr)
            {
                node.func();
            }
            
            jobQueueMutex.lock(HybridSpinLock::MODE_LOWPRIORITY);
            jobsInProgress[id] = SIZE_MAX;
            jobQueueMutex.unlock();
        }
    }
    
    bool SimpleJobQueue::hasJobs()
    {
        jobQueueMutex.lock();
        bool v = jobs.size() != 0;
        jobQueueMutex.unlock();
        return !v;
    }
}