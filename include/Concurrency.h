#pragma once
#include "BuildOptions.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <immintrin.h>
#include <omp.h>

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
    struct HybridSpinLock
    {
        static const int MAX_ATTEMPTS = 8;
        static const int MODE_STANDARD = 0;
        static const int MODE_LOWPRIORITY = 1;
        std::mutex lockVar;
        void lock(int mode = MODE_STANDARD)
        {
            int attempts = 0;
            if(mode == MODE_LOWPRIORITY)
                attempts = MAX_ATTEMPTS;
            
            while(!lockVar.try_lock())
            {
                if(attempts > MAX_ATTEMPTS)
                {
                    lockVar.lock();
                    break;
                }
                else
                    std::this_thread::yield();
            }
        }
        void unlock()
        {
            lockVar.unlock();
        }
        bool tryLock()
        {
            return lockVar.try_lock();
        }
    };

    struct HybridSpinSemaphore
    {
        static const int MAX_ATTEMPTS = 16;
        static const int MODE_STANDARD = 0;
        static const int MODE_LOWPRIORITY = 1;

        static const bool TYPE_READ = 0;
        static const bool TYPE_WRITE = 1;
        
        std::atomic_int readCount = {0};
        std::condition_variable cv;
        std::mutex conditionalMutexThing;
        std::thread::id writeID;

        void lock(bool type, int mode = MODE_STANDARD)
        {
            int NOLOCK = 0;
            int WRITELOCK = -1;
            int attempts = 0;
            if(mode == MODE_LOWPRIORITY)
                attempts = MAX_ATTEMPTS;
            
            if(type == TYPE_READ)
            {
                while(true)
                {
                    int oldCount = readCount.load();
                    if(readCount.compare_exchange_weak(NOLOCK, 1, std::memory_order_acquire))
                    {
                        return;
                    }
                    if(oldCount > 0)
                    {
                        if(readCount.compare_exchange_weak(oldCount, oldCount+1, std::memory_order_acquire))
                            break;
                    }
                    else
                    {
                        if(attempts >= MAX_ATTEMPTS)
                        {
                            std::unique_lock<std::mutex> uniqueLockForCV(conditionalMutexThing);
                            cv.wait_for(uniqueLockForCV, std::chrono::microseconds(1));
                        }
                        else
                            attempts++;
                    }
                }
            }
            else
            {
                while(true)
                {
                    if(readCount.compare_exchange_weak(NOLOCK, -1, std::memory_order_acquire))
                        break;
                    if(attempts >= MAX_ATTEMPTS)
                    {
                        std::unique_lock<std::mutex> uniqueLockForCV(conditionalMutexThing);
                        cv.wait_for(uniqueLockForCV, std::chrono::microseconds(1));
                    }
                    else
                        attempts++;
                }
                writeID = std::this_thread::get_id();
            }
        }

        bool unlock()
        {
            int WRITELOCK = -1;
            int READLOCK = 1;
            bool changed = false;

            if(writeID == std::this_thread::get_id())
            {
                writeID = std::thread::id();
                readCount.exchange(0, std::memory_order_release);
                cv.notify_all();
                return true;
            }
            else
            {
                while(true)
                {
                    int oldCount = readCount.load();
                    if(oldCount == 0)
                        return false;
                    if(readCount.compare_exchange_weak(oldCount, oldCount-1, std::memory_order_release))
                    {
                        cv.notify_one();
                        return true;
                    }
                }
            }
        
            return false;
        }
    };

    // struct HybridSpinSemaphore
    // {
    //     static const int MAX_ATTEMPTS = 16;
    //     static const int MODE_STANDARD = 0;
    //     static const int MODE_LOWPRIORITY = 1;

    //     static const bool TYPE_READ = 0;
    //     static const bool TYPE_WRITE = 1;
        
    //     std::atomic_int readCount = {0};
    //     std::condition_variable cv;
    //     std::mutex conditionalMutexThing;
    //     std::thread::id writeID;
        
    //     void lock(bool type, int mode = MODE_STANDARD)
    //     {
    //         int NOLOCK = 0;
    //         int WRITELOCK = -1;

    //         bool FALSEVAL = false;
    //         int attempts = 0;
    //         if(mode == MODE_LOWPRIORITY)
    //             attempts = MAX_ATTEMPTS;
            
    //         if(type == TYPE_READ)
    //         {
    //             while(true)
    //             {
    //                 int oldCount = readCount.load();
    //                 if(oldCount == -1)
    //                 {
    //                     //wait till its 0 again
    //                     while(true)
    //                     {
    //                         NOLOCK = 0;
    //                         if(readCount.compare_exchange_weak(NOLOCK, 1, std::memory_order_acquire))
    //                         {
    //                             return;
    //                         }
                            
    //                         int oldV = readCount.load();
    //                         if(oldV > 0)
    //                             break;
                            
    //                         if(attempts > MAX_ATTEMPTS)
    //                         {
    //                             std::unique_lock<std::mutex> uniqueLockForCV(conditionalMutexThing);
    //                             cv.wait_for(uniqueLockForCV, std::chrono::microseconds(1));
    //                         }
    //                     }
    //                 }
    //                 else
    //                 {
    //                     if(readCount.compare_exchange_weak(oldCount, oldCount+1, std::memory_order_acquire))
    //                         break;
    //                 }
    //             }
    //         }
    //         else
    //         {
    //             while(true)
    //             {
    //                 NOLOCK = 0;
    //                 if(readCount.compare_exchange_weak(NOLOCK, -1, std::memory_order_acquire))
    //                     break;
    //                 if(attempts > MAX_ATTEMPTS)
    //                 {
    //                     std::unique_lock<std::mutex> uniqueLockForCV(conditionalMutexThing);
    //                     cv.wait_for(uniqueLockForCV, std::chrono::microseconds(1));
    //                 }
    //             }
    //             writeID = std::this_thread::get_id();
    //         }
    //     }

    //     bool unlock()
    //     {
    //         int WRITELOCK = -1;
    //         int READLOCK = 1;
    //         bool changed = false;

    //         if(writeID == std::this_thread::get_id())
    //         {
    //             writeID = std::thread::id();
    //             readCount.exchange(0, std::memory_order_release);
    //             cv.notify_all();
    //             return true;
    //         }
    //         else
    //         {
    //             while(true)
    //             {
    //                 int oldCount = readCount.load();
    //                 if(oldCount == 0)
    //                     return false;
    //                 if(readCount.compare_exchange_weak(oldCount, oldCount-1, std::memory_order_release))
    //                 {
    //                     cv.notify_all();
    //                     return true;
    //                 }
    //             }
    //         }
        
    //         return false;
    //     }
    // };
}