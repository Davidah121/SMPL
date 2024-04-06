#pragma once
#include "SmartQueue.h"
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <condition_variable>

namespace smpl
{
    class SimpleJobQueue
    {
    public:
        /**
         * @brief Construct a new Simple Job Queue object
         *      Jobs are added to a queue to be executed by individual threads.
         *      There is no order to when a job is finished. The jobs are only guaranteed to start in the order presented.
         *      
         * @param threads 
         *      The number of threads allowed for the job queue.
         *      Can be lower or more than the number of threads a CPU has.
         *      Should not be too large however as performance can suffer.
         */
        SimpleJobQueue(int threads);

        /**
         * @brief Destroy the Simple Job Queue object
         *      Does not wait until all jobs are done. Just waits until the threads are done
         *      with their current job to exit.
         *      If waiting till all jobs are done is required, use clearAllJobs() first.
         * 
         *      The SimpleJobQueue Object is invalid after being destroyed.
         */
        ~SimpleJobQueue();

        /**
         * @brief Adds a new job to the job queue. It is any valid function call which
         *      can be a lamda function if other inputs are needed.
         *      Returns a pointer that can be used to delete the job from the queue if desired.
         *          Should not be used for any other reason as it is not thread safe.
         * 
         * @param j 
         * @return SmartMemory<SLinkNode<std::function<void()>>>
         */
        SmartMemory<SLinkNode<std::function<void()>>> addJob(std::function<void()> j);

        /**
         * @brief Attempts to remove a job with the specified pointer.
         *      Must be a valid pointer for the queue.
         *      The expected input here is returned from addJob()
         * 
         *      May not remove the job if it has already been executed or is in the process of being executed.
         * 
         * @param it 
         */
        void removeJob(SmartMemory<SLinkNode<std::function<void()>>> it);

        /**
         * @brief Forces the current thread / program to wait until all jobs in the queue are finished.
         * 
         */
        void clearAllJobs();

        /**
         * @brief Removes all jobs in the queue.
         *      May not remove a job if it has already or is in the process of being executed.
         * 
         */
        void removeAllJobs();

        /**
         * @brief Returns whether there are jobs in the queue still.
         * 
         * @return true 
         * @return false 
         */
        bool hasJobs();

    private:
        /**
         * @brief An internal function for the created threads.
         *      Will pick jobs from the queue to execute and yield control as long as there is nothing to do.
         * 
         * @param id 
         */
        void threadRun(int id);

        /**
         * @brief An internal function to determine if the threads should keep running
         * 
         * @return true 
         * @return false 
         */
        bool getRunning();
        
        std::mutex jobQueueMutex;
        std::mutex importantMutex;
        bool running = false;
        SmartQueue<std::function<void()>> jobs;
        std::vector<std::thread*> jobThreads;
        std::condition_variable cv;
    };
}