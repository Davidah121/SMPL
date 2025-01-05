#include <catch2/catch_amalgamated.hpp>
#include "SmartMemory.h"
#include "System.h"

TEST_CASE("Testing of the SmartMemory class", "[SmartMemory]")
{
	class testClass
	{
		public:
			testClass(int* editThis)
			{
				editThisPoint = editThis;
			}
			~testClass()
			{
				if(editThisPoint != nullptr)
				{
					*editThisPoint += 1;
				}
				editThisPoint = nullptr;
			}
		private:
			int* editThisPoint = nullptr;
	};

	SECTION("Test main deletion of an element")
	{
		int* n = new int[1];
		smpl::SmartMemory<int> m = smpl::SmartMemory(n, 1, true, true);
		smpl::SmartMemory<int> m2 = m;

		m.~SmartMemory();
		REQUIRE(m2.getPointer() == nullptr);
	}

	SECTION("Test copy and main deletion of a single element")
	{
		int counter = 0;
		smpl::SmartMemory<testClass> m = smpl::SmartMemory(new testClass(&counter), 1, false, true);
		smpl::SmartMemory<testClass> m2 = m;

		REQUIRE(counter == 0);
		//force destructor of copy
		m2.~SmartMemory();
		REQUIRE(counter == 0);

		//force destructor of main
		m.~SmartMemory();
		REQUIRE(counter == 1);
	}

	SECTION("Test copy and main deletion of an array of elements")
	{
		std::vector<int> counters = std::vector<int>(4);
		smpl::SmartMemory<testClass> mems = smpl::SmartMemory( new testClass[4]{testClass(&counters[0]), testClass(&counters[1]), testClass(&counters[2]), testClass(&counters[3])}, 4, false, true );
		smpl::SmartMemory<testClass> mems2 = mems;

		REQUIRE( (counters[0] == 0 && counters[1] == 0 && counters[2] == 0 && counters[3] == 0) );
		//force destructor of copy
		mems2.~SmartMemory();
		REQUIRE( (counters[0] == 0 && counters[1] == 0 && counters[2] == 0 && counters[3] == 0) );
		
		//force destructor of main
		mems.~SmartMemory();
		REQUIRE( (counters[0] == 1 && counters[1] == 1 && counters[2] == 1 && counters[3] == 1) );
	}

	SECTION("Test Locking Smart Memory")
	{
		int value = 0;
		smpl::SmartMemory<testClass> mainMemory = smpl::SmartMemory<testClass>::createDeleteRights(new testClass(&value), 1);
		smpl::SmartMemory<testClass> copyMemory = mainMemory; //a copy with no delete rights

		//create a thread and make a LockingSmartMemory object from the main one.
		std::thread t = std::thread([&mainMemory]() ->void{
			smpl::LockingSmartMemory<testClass> lsm = mainMemory.getLockingPointer();
			//sleep for like 5 millisecond
			smpl::System::sleep(5);
		});

		//sleep until the thread starts. 2 milliseconds should be more than enough
		smpl::System::sleep(2);
		mainMemory.~SmartMemory();
		REQUIRE(value == 0); //value changes when testClass is properly destroyed. should be 0 because it is locked

		t.join();
		REQUIRE(value == 1); //value changes when testClass is properly destroyed. should be 1 because it is not locked and was requested to be deleted.

		//copy memory is holding on to deleted memory by now.
		REQUIRE(copyMemory.isValid() == false);
	}
}