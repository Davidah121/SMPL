#include <catch2/catch_amalgamated.hpp>
#include "SmartMemory.h"

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
		SmartMemory<int> m = SmartMemory(n, false, true, true);
		SmartMemory<int> m2 = m;

		m.~SmartMemory();
		REQUIRE(m2.getPointer() == nullptr);
	}

	SECTION("Test copy and main deletion of a single element")
	{
		int counter = 0;
		SmartMemory<testClass> m = SmartMemory(new testClass(&counter), false, false, true);
		SmartMemory<testClass> m2 = m;

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
		SmartMemory<testClass> mems = SmartMemory( new testClass[4]{testClass(&counters[0]), testClass(&counters[1]), testClass(&counters[2]), testClass(&counters[3])}, true, false, true );
		SmartMemory<testClass> mems2 = mems;

		REQUIRE( (counters[0] == 0 && counters[1] == 0 && counters[2] == 0 && counters[3] == 0) );
		//force destructor of copy
		mems2.~SmartMemory();
		REQUIRE( (counters[0] == 0 && counters[1] == 0 && counters[2] == 0 && counters[3] == 0) );
		
		//force destructor of main
		mems.~SmartMemory();
		REQUIRE( (counters[0] == 1 && counters[1] == 1 && counters[2] == 1 && counters[3] == 1) );
	}
}