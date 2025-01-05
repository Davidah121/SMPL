#pragma once
#include "BuildOptions.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <immintrin.h>
#include <omp.h>

namespace smpl
{
    struct HybridSpinLock
    {
        static const int MAX_ATTEMPTS = 16;
        static const int MODE_STANDARD = 0;
        static const int MODE_LOWPRIORITY = 1;
        static const int MODE_AGRESSIVE = 2;
        std::mutex lockVar;
        void lock(int mode = MODE_STANDARD)
        {
            int attempts = 0;
            if(mode == MODE_LOWPRIORITY)
                attempts = MAX_ATTEMPTS;
            
            while(!lockVar.try_lock())
            {
                if(mode != MODE_AGRESSIVE)
                    attempts++;
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
        static const int MODE_AGRESSIVE = 2;

        static const bool TYPE_READ = 0;
        static const bool TYPE_WRITE = 1;
        
        std::atomic_int readCount = {0};
        std::thread::id writeID;
        
        void lock(bool type, int mode = MODE_STANDARD)
        {
            int NOLOCK = 0;
            int WRITELOCK = -1;

            bool FALSEVAL = false;
            int attempts = 0;
            if(mode == MODE_LOWPRIORITY)
                attempts = MAX_ATTEMPTS;
            
            if(type == TYPE_READ)
            {
                while(true)
                {
                    int oldCount = readCount.load();
                    if(oldCount == -1)
                    {
                        //wait till its 0 again
                        while(true)
                        {
                            NOLOCK = 0;
                            if(readCount.compare_exchange_weak(NOLOCK, 1, std::memory_order_acquire))
                            {
                                return;
                            }
                            
                            int oldV = readCount.load();
                            if(oldV > 0)
                                break;
                            
                            if(mode != MODE_AGRESSIVE)
                                attempts++;
                            
                            if(attempts > MAX_ATTEMPTS)
                                std::this_thread::sleep_for(std::chrono::microseconds(1));
                            else
                                std::this_thread::yield();
                        }
                    }
                    else
                    {
                        if(readCount.compare_exchange_weak(oldCount, oldCount+1, std::memory_order_acquire))
                            break;
                    }
                }
            }
            else
            {
                while(true)
                {
                    NOLOCK = 0;
                    if(readCount.compare_exchange_weak(NOLOCK, -1, std::memory_order_acquire))
                        break;
                    if(mode != MODE_AGRESSIVE)
                        attempts++;
                    if(attempts > MAX_ATTEMPTS)
                        std::this_thread::sleep_for(std::chrono::microseconds(1));
                    else
                        std::this_thread::yield();
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
                        return true;
                }
            }
        
            return false;
        }
    };
}