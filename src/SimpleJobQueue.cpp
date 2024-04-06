#include "SimpleJobQueue.h"
#include "System.h"

namespace smpl
{
    SimpleJobQueue::SimpleJobQueue(int threads)
    {
        running = true;
        jobThreads = std::vector<std::thread*>(threads);
        
        for(int i=0; i<jobThreads.size(); i++)
        {
            jobThreads[i] = new std::thread(&SimpleJobQueue::threadRun, this, i);
        }
    }
    
    SimpleJobQueue::~SimpleJobQueue()
    {
        jobQueueMutex.lock();
        running = false;
        jobQueueMutex.unlock();

        for(int i=0; i<jobThreads.size(); i++)
        {
            jobThreads[i]->join();
            delete jobThreads[i];
        }

        jobThreads.clear();
    }

    SmartMemory<SLinkNode<std::function<void()>>> SimpleJobQueue::addJob(std::function<void()> j)
    {
        importantMutex.lock();
        SmartMemory<SLinkNode<std::function<void()>>> node = jobs.add(j);
        importantMutex.unlock();
        cv.notify_one();
        return node;
    }
    

    void SimpleJobQueue::removeJob(SmartMemory<SLinkNode<std::function<void()>>> n)
    {
        importantMutex.lock();
        jobs.erase(n);
        importantMutex.unlock();
    }

    
    void SimpleJobQueue::removeAllJobs()
    {
        importantMutex.lock();
        jobs.clear();
        importantMutex.unlock();
    }
    
    bool SimpleJobQueue::getRunning()
    {
        jobQueueMutex.lock();
        bool res = running;
        jobQueueMutex.unlock();
        return res;
    }

    void SimpleJobQueue::clearAllJobs()
    {
        while(true)
        {
            importantMutex.lock();
            //check if there is something in the job queue.
            bool jobQueueEmpty = jobs.empty();
            importantMutex.unlock();

            if(jobQueueEmpty)
                break;
            
            //Otherwise, wait.
            System::sleep(1, 0, false);
        }
    }

    void SimpleJobQueue::threadRun(int id)
    {
        bool knownToHaveMoreWork = false;
        std::mutex waitMutex;

        while(true)
        {
            if(!getRunning())
                break;
            
            if(!knownToHaveMoreWork)
            {
                std::unique_lock<std::mutex> ulock(waitMutex);
                cv.wait_for(ulock, std::chrono::milliseconds(1));
            }
            
            //try to get a job from the queue
            std::function<void()> execJob = nullptr;
            importantMutex.lock();
            if(!jobs.empty())
            {
                //get job and remove from queue. Mark as busy
                SmartMemory<SLinkNode<std::function<void()>>> node = jobs.get();
                if(node.getPointer() != nullptr)
                {
                    execJob = node.getRawPointer()->value;
                }
                jobs.pop();
            }

            knownToHaveMoreWork = !jobs.empty();
            importantMutex.unlock();

            //execute job if we got one
            if(execJob != nullptr)
            {
                execJob();
            }
        }
    }
    
    bool SimpleJobQueue::hasJobs()
    {
        jobQueueMutex.lock();
        bool v = jobs.empty();
        jobQueueMutex.unlock();
        return !v;
    }
}