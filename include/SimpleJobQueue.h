#pragma once
#include "PriorityQueue.h"
#include "BuildOptions.h"
#include "Concurrency.h"
#include "Queue.h"
#include <thread>
#include <vector>
#include <functional>
#include <unordered_map>

namespace smpl
{
    struct JobInfo
    {
        size_t ID = SIZE_MAX;
        std::function<void()> func = nullptr;
        bool operator==(const JobInfo& other)
        {
            return ID == other.ID;
        }
    };

    class DLL_OPTION SimpleJobQueue
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
         *      Returns a ID for the job if it needs to be deleted.
         * 
         * @param j 
         * @return size_t
         */
        size_t addJob(std::function<void()> j);

        /**
         * @brief Attempts to remove a job with the specified ID.
         *      The expected input here is returned from addJob()
         * 
         *      May not remove the job if it has already been executed or is in the process of being executed.
         * 
         * @param it 
         */
        void removeJob(size_t jobID);

        /**
         * @brief Forces the current thread / program to wait until all jobs in the queue are finished.
         * 
         */
        void waitForAllJobs();

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

        /**
         * @brief Determines if the job with the specified ID is done.
         * 
         * @param ID 
         * @return true 
         * @return false 
         */
        bool jobDone(size_t ID);

        /**
         * @brief Determines if all jobs in the queue have been finished.
         *      All threads must not be busy working on anything as well.
         * 
         * @return true 
         * @return false 
         */
        bool allJobsDone();

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
        
        HybridSpinLock jobQueueMutex;
        bool running = false;
        size_t jobID = 0;
        Queue<JobInfo> jobs;
        std::vector<size_t> jobsInProgress;
        std::vector<std::thread*> jobThreads;

        //Used for efficient halting and interrupts
        std::condition_variable cv;
        std::mutex haltMutex;
    };

    class DLL_OPTION SmartJobQueue
    {
    public:
        /**
         * @brief Construct a new Smart Job Queue object
         *      Jobs are added to a queue to be executed by individual threads.
         *      There is no order to when a job is finished. The jobs are only guaranteed to start in the order presented.
         *      The jobs start in order of priority (higher priority starts first)
         * 
         *      A minimum of 2 threads are used.
         *          One to add jobs and such. One to run actual jobs
         * @param minThreads
         *      If the minThreads < 1, it starts with just 1 thread.
         *      Otherwise, these are the total number of threads that will always be allocated.
         * @param maxThreads
         *      If the maxThreads = -1,
         *          It will have no limits to the total number of threads.
         */
        SmartJobQueue(int minThreads, int maxThreads);

        /**
         * @brief Destroy the Smart Job Queue object
         *      Does not wait until all jobs are done. Just waits until the threads are done
         *      with their current job to exit.
         *      If waiting till all jobs are done is required, use clearAllJobs() first.
         * 
         *      The SmartJobQueue Object is invalid after being destroyed.
         */
        ~SmartJobQueue();

        /**
         * @brief Adds a new job to the job queue with some priority. It is any valid function call which
         *      can be a lamda function if other inputs are needed.
         *      Returns a ID for the job if it needs to be deleted.
         * 
         * @param j 
         * @return size_t
         */
        size_t addJob(std::function<void()> j, double priority);

        /**
         * @brief Attempts to remove a job with the specified ID.
         *      The expected input here is returned from addJob()
         * 
         *      May not remove the job if it has already been executed or is in the process of being executed.
         * 
         * @param it 
         */
        void removeJob(size_t jobID);

        /**
         * @brief Forces the current thread / program to wait until all jobs in the queue are finished.
         * 
         */
        void waitForAllJobs();

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

        /**
         * @brief Determines if the job with the specified ID is done.
         * 
         * @param ID 
         * @return true 
         * @return false 
         */
        bool jobDone(size_t ID);

        /**
         * @brief Determines if all jobs in the queue have been finished.
         *      All threads must not be busy working on anything as well.
         * 
         * @return true 
         * @return false 
         */
        bool allJobsDone();

    private:
        /**
         * @brief An internal function for the created threads.
         *      Will pick jobs from the queue to execute and yield control as long as there is nothing to do.
         * 
         */
        void threadRun(size_t ID, bool temporary);

        /**
         * @brief A thread that spawns new threads to run
         * 
         */
        void analysisThreadRun();

        /**
         * @brief An internal function to determine if the threads should keep running
         * 
         * @return true 
         * @return false 
         */
        bool getRunning();
        
        HybridSpinLock jobQueueMutex;
        bool running = false;
        size_t jobID = 0;
        size_t threadID = 0;
        PriorityQueue<JobInfo> jobs;
        std::unordered_map<size_t, size_t> jobsInProgress;
        std::unordered_map<size_t, bool> threadDone;
        std::unordered_map<size_t, std::thread*> jobThreads;
        std::thread analysisThread;
        uint64_t jobsDoneSinceCheck = 0;
        uint64_t threadLoad = 0;
        uint16_t maxThreadsAllowed = -1;
        
        //Used for efficient halting and interrupts
        std::condition_variable cv;
        std::mutex haltMutex;
    };
}