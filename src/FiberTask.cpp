#include "FiberTask.h"

namespace smpl
{
	//This will be called upon first use so Fibers are completely ignored if you don't wish to use them.
	thread_local FiberTask FiberTask::rootTask = FiberTask::createRootTask();
	thread_local FiberTask* FiberTask::currentTask = nullptr;

	//ROOT TASK. CAN NOT BE CREATED OUTSIDE INIT
	FiberTask::FiberTask()
	{
		taskStatus = STATUS_ROOT;
	}

	FiberTask::FiberTask(const std::function<void()>& runningFunc)
	{
		isRootTask = false;
		f = runningFunc;
		if(f != nullptr)
		{
			//pass in the stack size, a STATIC function or something visible from global space to make it easier and pass in the object pointer (not used)
			fiberHandle = tiny_fiber::CreateFiber(STACK_SIZE, FiberTask::taskRunner, this);
			taskStatus = STATUS_IDLE;
		}
		else
			taskStatus = STATUS_INVALID;
	}
	FiberTask::~FiberTask()
	{
		if(taskStatus != STATUS_INVALID)
			tiny_fiber::DeleteFiber(fiberHandle);
		f = nullptr;
		taskStatus = STATUS_INVALID;
	}

	FiberTask FiberTask::createRootTask()
	{
		FiberTask retTask = FiberTask();
		retTask.fiberHandle = tiny_fiber::CreateFiberFromThread();
		retTask.isRootTask = true;
		return retTask;
	}


	void FiberTask::changeTask(const std::function<void()>& runningFunc)
	{
		//IFF the task is done, you may replace the running function. This allows a fiber to reuse its stack.
		if(taskStatus == STATUS_DONE)
		{
			f = runningFunc;
			taskStatus = STATUS_IDLE;
		}
		else
		{
			throw std::runtime_error("TASK HAS NOT FINISHED YET changeTask() was called.");
		}
	}
	void FiberTask::run()
	{
		//start the task if it hasn't been started and is not invalid
		if(taskStatus == STATUS_ROOT)
		{
			throw std::runtime_error("TASK IS THE ROOT TASK. IT CAN NEVER BE RUN.");
		}
		if(taskStatus == STATUS_INVALID)
		{
			throw std::runtime_error("TASK IS IN AN INVALID STATE AND CAN NOT BE RUN.");
		}
		if(taskStatus == STATUS_RUNNING)
		{
			throw std::runtime_error("TASK IS IN A RUNNING STATE AND CAN NOT BE RUN AGAIN TILL IT IS SUSPENDED OR DONE.");
		}


		if(FiberTask::currentTask == nullptr)
			FiberTask::currentTask = &FiberTask::rootTask; //This is where the root task is actually initialized which converts the current thread to a fiber.
		resume();
	}

	void FiberTask::yield()
	{
		//shouldn't call this from outside the current thread running the task.
		//Will context switch and an improper context switch will break things.
		//Can force it to never allow this

		taskStatus = STATUS_SUSPENDED;

		#ifndef __WIN32
		//save floating point status
		std::fegetenv(&floatingPointControl);
		#endif

		yieldCounter++;

		//context switch out
		tiny_fiber::SwitchFiber(fiberHandle, parentTask->fiberHandle);

		#ifndef __WIN32
		//set floating point status to old one (rounding mode and x87 stuff)
		std::fesetenv(&floatingPointControl);
		#endif
	}

	size_t FiberTask::getYieldCounter()
	{
		return yieldCounter;
	}

	int FiberTask::getStatus()
	{
		return taskStatus;
	}

	void FiberTask::resume()
	{
		//save current task pointer and set this as the current task being run on this thread.
		//(may be nullptr)
		parentTask = FiberTask::currentTask;
		currentTask = this;

		#ifndef __WIN32
		//save old floating point status
		std::fegetenv(&floatingPointControl);

		//set floating point status (rounding mode and x87 stuff).
		//May Fail. Skip if its the first run so it inherits the floating point status
		if(taskStatus != STATUS_IDLE)
			std::fesetenv(&floatingPointControl);

		#endif
		//set status to running
		taskStatus = STATUS_RUNNING;

		//context switch in
		tiny_fiber::SwitchFiber(parentTask->fiberHandle, fiberHandle);
		
		//restore the previous task (may be nullptr)
		currentTask = parentTask;

		#ifndef __WIN32
		//restore floating point status
		std::fesetenv(&floatingPointControl);
		#endif
	}
	void FiberTask::internalRun()
	{
		f();
		taskStatus = STATUS_DONE;
	}

	void FiberTask::endTask()
	{
		//context switch out
		tiny_fiber::SwitchFiber(fiberHandle, parentTask->fiberHandle);
	}


	void FiberTask::taskRunner(void* taskPtr)
	{
		//infinite loop that allows the program to reuse existing stacks
		FiberTask* task = (FiberTask*)taskPtr;
		while(true)
		{
			task->internalRun();

			//this will context switch out of the task run code back to caller.
			task->endTask(); 

			//If you recall this function, you'll start here and start over.
			//Note that the assumption is that Task::currentTask is consistent across every call of this taskRunner function for a particular task
		}
	}
		

	#pragma region ThisFiberTask


	void ThisFiberTask::yield()
	{
		if(FiberTask::currentTask != nullptr && FiberTask::currentTask != &FiberTask::rootTask)
		{
			FiberTask::currentTask->yield();
		}
		//do nothing otherwise. Should never yield the root task as there is nothing to yield to.
	}

	size_t ThisFiberTask::getYieldCounter()
	{
		if(FiberTask::currentTask != nullptr && FiberTask::currentTask != &FiberTask::rootTask)
		{
			return FiberTask::currentTask->getYieldCounter();
		}
		return 0;
	}

	FiberTask* ThisFiberTask::getCurrentTask()
	{
		return FiberTask::currentTask;
	}

	#pragma endregion ThisFiberTask



	//Task Scheduler Stuff - Per Thread scheduling
	thread_local FiberTaskScheduler FiberTaskScheduler::singleton = FiberTaskScheduler();

	FiberTaskScheduler::FiberTaskScheduler(){}
	FiberTaskScheduler::~FiberTaskScheduler(){}
	void FiberTaskScheduler::run()
	{
		FiberTaskScheduler::singleton.internalRun();
	}

	void FiberTaskScheduler::addTask(FiberTask* taskPtr)
	{
		FiberTaskScheduler::singleton.internalAddTask(taskPtr);
	}


	void FiberTaskScheduler::internalRun()
	{
		while(!taskPtrs.empty())
		{
			//grab task off queue
			FiberTask* task = taskPtrs.front();
			taskPtrs.pop_front();
			if(task == nullptr)
				continue;
			if(task->getStatus() == FiberTask::STATUS_DONE)
				continue;

			task->run();

			//check if the task yielded
			if(task->getStatus() == FiberTask::STATUS_SUSPENDED)
			{
				internalAddTask(task); //round robin scheduling. Can make this better by scheduling based on priority (yield should have less priority than sleep)
			}
		}
	}

	void FiberTaskScheduler::internalAddTask(FiberTask* taskPtr)
	{
		taskPtrs.push_back(taskPtr);
	}
	void FiberTaskScheduler::internalPopTask()
	{
		taskPtrs.pop_front();
	}
}