#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <chrono>
#include <immintrin.h>
#include <omp.h>
#include "FiberTask.h"

#ifndef __min
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef __max
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef LARGE_ENOUGH_CLAUSE
#ifdef DEBUG
    #define LARGE_ENOUGH_LIMIT 128
#else
    #define LARGE_ENOUGH_LIMIT 0x4000
#endif
#define MAX_THREADS_KNOWN std::thread::hardware_concurrency()
#define RESET_LARGE_ENOUGH_CLAUSE() omp_set_num_threads(MAX_THREADS_KNOWN);
#define LARGE_ENOUGH_CLAUSE(size) omp_set_num_threads(__min(MAX_THREADS_KNOWN, ((double)size / LARGE_ENOUGH_LIMIT)));
#endif

namespace smpl
{
    class HybridSpinLock
    {
	public:
        static const int MAX_ATTEMPTS = 8;
        static const int MODE_STANDARD = 0;
        static const int MODE_LOWPRIORITY = 1;
		HybridSpinLock() = default;
		~HybridSpinLock() = default;

        void lock(int mode = MODE_STANDARD)
        {
            int attempts = 0;
            if(mode == MODE_LOWPRIORITY)
                attempts = MAX_ATTEMPTS;
            
            while(!lockVar.try_lock())
            {
                if(attempts >= MAX_ATTEMPTS)
                {
                    lockVar.lock(); //Note that this does not workout that well for fibers
					locker = std::this_thread::get_id();
					isLocked = true;
                    break;
                }
                else
				{
					attempts++;
					//failing to get lock means that other threads/fibers are using it. Yield to other threads so they may run while you wait.
					//If its a fiber, it may make more sense to yield to the caller which may schedule other fibers.
					ThisFiberTask::yield();
                    std::this_thread::yield();
				}
            }
        }
        void unlock()
        {
			isLocked = false;
            lockVar.unlock();
        }
        bool tryLock()
        {
            return lockVar.try_lock();
        }
	
	private:
		bool isLocked = false;
        std::mutex lockVar;
		std::thread::id locker;
    };

    struct ReadWriterLock
    {
        static const int MAX_ATTEMPTS = 8;
        static const int MODE_STANDARD = 0;
        static const int MODE_LOWPRIORITY = 1;

        static const bool TYPE_READ = false;
        static const bool TYPE_WRITE = true;

        std::atomic_int readerCount = {0};
        bool writerExist = false;

        std::mutex mtx;
        std::condition_variable cv;
        std::thread::id writeID;
        
        void lock(bool type, int mode = MODE_STANDARD)
        {
            int attempts = 0;
            if(mode == MODE_LOWPRIORITY)
                attempts = MAX_ATTEMPTS;
            
            if(type == TYPE_READ)
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this]() {return writerExist == false;});
                readerCount++;
            }
            else
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this]() {return (writerExist == false) && (readerCount == 0);});
                writerExist = true;
                writeID = std::this_thread::get_id();
            }
        }

        bool unlock()
        {
            if(writeID == std::this_thread::get_id())
            {
                std::unique_lock<std::mutex> lock(mtx);
                writeID = std::thread::id();
                writerExist = false;
                cv.notify_all();
                return true;
            }
            else
            {
                std::unique_lock<std::mutex> lock(mtx);
                readerCount--;
                if(readerCount == 0)
                    cv.notify_all();
                return true;
            }
            return false;
        }
    };
}