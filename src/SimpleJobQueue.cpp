#include "SimpleJobQueue.h"
#include "System.h"

namespace glib
{
    SimpleJobQueue::SimpleJobQueue(int threads)
    {
        running = true;
        jobThreads = std::vector<std::thread*>(threads);
        busy = std::vector<bool>(threads);
        
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
        jobQueueMutex.lock();
        SmartMemory<SLinkNode<std::function<void()>>> node = jobs.add(j);
        jobQueueMutex.unlock();
        return node;
    }
    

    void SimpleJobQueue::removeJob(SmartMemory<SLinkNode<std::function<void()>>> n)
    {
        jobQueueMutex.lock();
        jobs.erase(n);
        jobQueueMutex.unlock();
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
            jobQueueMutex.lock();
            
            //check if all threads are not busy
            bool noneBusy = true;
            for(int i=0; i<jobThreads.size(); i++)
            {
                if(busy[i])
                {
                    noneBusy = false;
                    break;
                }
            }
            
            //check if there is something in the job queue.
            bool jobQueueEmpty = jobs.size() == 0;
            jobQueueMutex.unlock();

            if(jobQueueEmpty && noneBusy)
                break;
            
            //Otherwise, wait. Could sleep for 1 millisecond but this is okay.
            std::this_thread::yield();
            System::sleep(1);
        }
    }

    void SimpleJobQueue::threadRun(int id)
    {
        while(true)
        {
            if(!getRunning())
                break;
            
            //try to get a job from the queue
            std::function<void()> execJob = nullptr;
            jobQueueMutex.lock();
            if(!jobs.empty())
            {
                //get job and remove from queue. Mark as busy
                SmartMemory<SLinkNode<std::function<void()>>> node = jobs.get();
                if(node.getPointer() != nullptr)
                {
                    execJob = node.getRawPointer()->value;
                }
                jobs.pop();
                busy[id] = true;
            }
            jobQueueMutex.unlock();

            //execute job if we got one
            if(execJob != nullptr)
                execJob();
            
            //mark as not busy anymore
            jobQueueMutex.lock();
            busy[id] = false;
            jobQueueMutex.unlock();
            
            //wait for a bit
            std::this_thread::yield();
            System::sleep(1);
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