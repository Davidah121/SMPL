#pragma once
#include "FiberTask.h"
#include "PriorityQueue.h"
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "Concurrency.h"
#include "Queue.h"
#include <cstdint>
#include <thread>
#include <vector>
#include <functional>
#include "SimpleHashTable.h"

namespace smpl
{
	

    class JobInfo
    {
	public:
		JobInfo(){} //Ideally don't use this

		JobInfo(size_t ID, const std::function<void()>& func)
		{
			this->id = ID;
			this->func = func;
		}
		~JobInfo() = default;
		JobInfo(const JobInfo&) = default;
		JobInfo& operator=(const JobInfo&) = default;
		JobInfo(JobInfo&&) = default;
		JobInfo& operator=(JobInfo&&) = default;

		size_t getID() const
		{
			return id;
		}
		const std::function<void()>& getFunction() const
		{
			return func;
		}
		friend bool operator==(const size_t& id, const JobInfo& jbInfo)
		{
			return id == jbInfo.id;
		}
		bool operator==(const JobInfo& other) const
        {
            return id == other.id;
        }
		bool operator==(const size_t& other) const
        {
            return id == other;
        }
	private:
        size_t id = SIZE_MAX;
        std::function<void()> func = nullptr;
    };

	class JobTask
	{
	public:
		JobTask(size_t id, FiberTask* task)
		{
			this->id = id;
			this->task = task;
		}
		~JobTask() = default;
		JobTask(JobTask&&) = default;
		JobTask& operator=(JobTask&&) = default;

		size_t getID() const
		{
			return id;
		}
		FiberTask& getTask() const
		{
			return *task;
		}
		bool operator==(const JobTask& other) const
        {
            return id == other.id;
        }
	private:
        size_t id;
		FiberTask* task;
	};

	struct CompareJobTask
	{
		using is_transparent = void;
		bool operator()(const JobTask& v, const JobTask& v2) const
		{
			return v.getID() == v2.getID();
		}
		
		bool operator()(const size_t& v, const JobTask& v2) const
		{
			return v == v2.getID();
		}
		
		bool operator()(const JobTask& v, const size_t& v2) const
		{
			return v.getID() == v2;
		}
	};

	template<>
	struct RapidHash<JobTask> {
		using is_transparent = void;
		size_t operator()(const JobTask& p) const noexcept
		{
			return RapidHash<size_t>{}(p.getID());
		}
		size_t operator()(const size_t& p) const noexcept
		{
			return RapidHash<size_t>{}(p);
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
        size_t addJob(const std::function<void()>& j);

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
         *      All threads must not be busy working on anything as well and no jobs may be postponed for later execution.
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

		/**
		 * @brief Gets a free FiberTask or creates one if there is not one available.
		 *		Note that these will get freed when the job queue gets destroyed.
		 * 
		 * @return FiberTask* 
		 */
		FiberTask* getFreeFiberTask(const JobInfo& jbInfo);
        
        HybridSpinLock jobQueueMutex;
        bool running = false;
        size_t jobID = 0;

        Queue<JobInfo> jobs; //queue of jobs that you want to be run but are not yet started.

		std::vector<FiberTask*> createdTask; //need to delete these at the end.
		std::vector<FiberTask*> availableFiberTask; //Tasks that are done that could be reused by another function

		SimpleHashSet<JobTask, RapidHash<JobTask>, CompareJobTask> postponedJobs; //All tasks that are suspended. THIS SHOULD BE A QUEUE. FIX THIS LATER
		SimpleHashSet<size_t> jobsBeingRun; //A set of all jobs that are currently being run by a thread.

        std::vector<std::thread> jobThreads; //All available threads that can run task

        //Used for efficient halting and interrupts
        std::condition_variable cv;
        // std::mutex haltMutex;
    };

    class DLL_OPTION SmartJobQueue
    {
    public:
		static const size_t NO_LIMIT = SIZE_MAX;
        /**
         * @brief Construct a new Smart Job Queue object
         *      Jobs are added to a queue to be executed by individual threads.
         *      There is no order to when a job is finished. The jobs are only guaranteed to start in the order presented.
         *      The jobs start in order of priority (higher priority starts first)
         * 
         *      These are job threads themselves and are separate from the main thread.
		 *			Note that a separate analysis thread is created which will occasionally check the job queue's efficiency to determine if new threads must be
		 *			added or deleted with respect to maxThreads. This removes the burden from the main calling threads.
         * @param minThreads
         *      These are the total number of threads that will always be allocated.
		 *			Must be at least 1. If minThreads < 1, minThreads = 1
         * @param maxThreads
         *      If the maxThreads = -1, no maximum limit.
		 *		If maxThreads <= minThreads, no additional threads will be created.
         */
        SmartJobQueue(size_t minThreads, size_t maxThreads);

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
        size_t addJob(const std::function<void()>& j, double priority);

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
         *      All threads must not be busy working on anything as well and no jobs may be postponed for later execution.
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
        
		/**
		 * @brief Gets a free FiberTask or creates one if there is not one available.
		 *		Note that these will get freed when the job queue gets destroyed.
		 * 
		 * @return FiberTask* 
		 */
		FiberTask* getFreeFiberTask(const JobInfo& jbInfo);


        HybridSpinLock jobQueueMutex;
        bool running = false;
        size_t jobID = 0;
        size_t threadID = 0;
        PriorityQueue<JobInfo> jobs; //queue of jobs to be run that haven't started

		std::vector<FiberTask*> createdTask; //need to delete these at the end.
		std::vector<FiberTask*> availableFiberTask; //Tasks that are done that could be reused by another function

		
		SimpleHashSet<JobTask, RapidHash<JobTask>, CompareJobTask> postponedJobs; //All tasks that are suspended. THIS SHOULD BE A QUEUE. FIX THIS LATER
		SimpleHashSet<size_t> jobsBeingRun; //A set of all jobs that are currently being run by a thread.

		std::vector<std::thread> jobThreads; //The main list of threads. All new threads are detached since they will be destroyed when no jobs left

        std::thread analysisThread;
        uint64_t jobsDoneSinceCheck = 0;
        uint64_t threadLoad = 0;
        uint16_t maxThreadsAllowed = -1;
        
        //Used for efficient halting and interrupts
        std::condition_variable cv;
        std::mutex haltMutex;
    };
}