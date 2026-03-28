#pragma once

#include <chrono>
#include <cstddef>
#include <deque>
#include <functional>
#include <cfenv>
#include <iostream>
#include <ratio>
#include <thread>
#include "external/tiny_fiber.h"

namespace smpl
{
	class FiberTaskScheduler;
	class FiberTask;
	class ThisFiberTask;

	//Due to the nature of Windows being extra about context switching, Just use Tiny_Fiber which is very lightweight. You could implement everything by hand too
	//BUT having access to ARM based fibers and MACOS with no extra effort is useful
	//Note that asm code may not compile on different compilers though it should work out 

	//Why Fiber based and not coroutine based?
	//Simply, Coroutines require significant changes to code and requires either C++20 support or even more work.
	//Using Fibers (since they have a stack) allows for nested calls that can still yield and those nested calls do not need to be
	//changed in anyway. Just need to be made aware of the fact that it is running in a fiber (which can be determined through static functions)

	class FiberTask
	{
	public:
		static const unsigned int STACK_SIZE = 0x10000; //~64 mega bytes. A little more than that though
		static const char STATUS_ROOT = -2;
		static const char STATUS_INVALID = -1; //Deleted or Failed Creation
		static const char STATUS_IDLE = 0; //Hasn't been called yet.
		static const char STATUS_RUNNING = 1; //Actively running (may not be the current fiber though)
		static const char STATUS_SUSPENDED = 2; //Specifically Yielded to parent Fiber (Paused)
		static const char STATUS_DONE = 3; //Function has finished running.

		FiberTask(const std::function<void()>& runningFunc);
		~FiberTask();

		//copying is not allowed
		FiberTask(const FiberTask& other) = delete;
		void operator=(const FiberTask& other) = delete;
		
		//move constructors
		FiberTask(FiberTask&& other) = default;
		FiberTask& operator=(FiberTask&& other) = default;
		
		void changeTask(const std::function<void()>& runningFunc);
		void run();

		void yield();
		size_t getYieldCounter();

		template< class Rep, class Period >
		void sleep_for( const std::chrono::duration<Rep, Period>& sleep_duration )
		{
			size_t totalTimeInNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(sleep_duration).count();
			size_t start = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			while(true)
			{
				size_t timeElapsed = std::chrono::high_resolution_clock::now().time_since_epoch().count() - start;
				if(timeElapsed >= totalTimeInNanos)
					break;
				yield();
			}
		}

		template< class Clock, class Duration >
		void sleep_until( const std::chrono::time_point<Clock, Duration>& sleep_time )
		{
			//rework into a sleep for
			auto timeToWait = sleep_time - std::chrono::high_resolution_clock::now();
			sleep_for(timeToWait);
		}
		
		int getStatus();
	private:
		void resume();
		void internalRun();

		void endTask();

		//ROOT TASK. CAN NOT BE CREATED OUTSIDE INIT
		FiberTask();
		static FiberTask createRootTask();
		static void taskRunner(void* taskPtr);
		
		tiny_fiber::FiberHandle fiberHandle;
		FiberTask* parentTask = nullptr;
		size_t yieldCounter = 0;

		std::function<void()> f;
		char taskStatus = STATUS_INVALID;
		bool isRootTask = false;

		#ifndef __WIN32
		//required in linux only. Safe to include here even if you don't use it.
		fenv_t floatingPointControl;
		#endif

		static thread_local FiberTask rootTask;
		static thread_local FiberTask* currentTask;
		friend ThisFiberTask;
	};

	//Use this class to access the currently running task instead of accessing it in a raw manner.
	//IFF you really need the raw Task pointer, get it from here
	class ThisFiberTask
	{
	public:
		static void yield();
		static size_t getYieldCounter();

		
		template< class Rep, class Period >
		static void sleep_for( const std::chrono::duration<Rep, Period>& sleep_duration )
		{
			if(FiberTask::currentTask != nullptr && FiberTask::currentTask != &FiberTask::rootTask)
			{
				FiberTask::currentTask->sleep_for(sleep_duration);
			}
			else
			{
				//This is actually intended
				std::this_thread::sleep_for(sleep_duration);
			}
		}
		
		template< class Clock, class Duration >
		static void sleep_until( const std::chrono::time_point<Clock, Duration>& sleep_time )
		{
			if(FiberTask::currentTask != nullptr && FiberTask::currentTask != &FiberTask::rootTask)
			{
				FiberTask::currentTask->sleep_until(sleep_time);
			}
			else
			{
				//This is actually intended
				std::this_thread::sleep_until(sleep_time);
			}
		}

		static FiberTask* getCurrentTask();
	private:
		friend FiberTask;
	};


	//Task Scheduler Stuff - Per Thread scheduling
	class FiberTaskScheduler
	{
	public:
		~FiberTaskScheduler();
		static void run();
		static void addTask(FiberTask* taskPtr);

	protected:

		void internalRun();

		void internalAddTask(FiberTask* taskPtr);
		void internalPopTask();
	private:
		FiberTaskScheduler();
		std::deque<FiberTask*> taskPtrs;
		static thread_local FiberTaskScheduler singleton;
	};
}