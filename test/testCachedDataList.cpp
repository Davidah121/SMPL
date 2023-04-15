#include <catch2/catch_amalgamated.hpp>
#include "CachedDataList.h"

TEST_CASE("Test CachedDataList for data pointers." "[CachedDataList]")
{
	class TestClass
	{
		public:
			TestClass(int id)
			{
				this->id = id;
			}
			~TestClass()
			{

			}

			int getID()
			{
				return id;
			}
		private:
			int id = -1;
	};

	SECTION("Add data")
	{
		CachedDataList<TestClass> cdl = CachedDataList<TestClass>(4);

		cdl.addData(0, new TestClass(0), true);
		REQUIRE(cdl.getData(0) != nullptr);
		REQUIRE(cdl.getData(0)->getID() == 0);
		
		cdl.addData(1, new TestClass(1), true);
		REQUIRE(cdl.getData(1) != nullptr);
		REQUIRE(cdl.getData(1)->getID() == 1);

		cdl.addData(0, new TestClass(2), true);
		REQUIRE(cdl.getData(0) != nullptr);
		REQUIRE(cdl.getData(0)->getID() == 2);
	}

	SECTION("Remove data")
	{
		CachedDataList<TestClass> cdl = CachedDataList<TestClass>(4);

		cdl.addData(0, new TestClass(0), true);
		cdl.addData(1, new TestClass(1), true);
		cdl.addData(2, new TestClass(2), true);

		cdl.removeData(0);

		REQUIRE(cdl.getData(0) == nullptr);
		REQUIRE(cdl.getData(1) != nullptr);
		REQUIRE(cdl.getData(1)->getID() == 1);
		REQUIRE(cdl.getData(2) != nullptr);
		REQUIRE(cdl.getData(2)->getID() == 2);
	}

	SECTION("Run out of cache")
	{
		CachedDataList<TestClass> cdl = CachedDataList<TestClass>(4);

		cdl.addData(1, new TestClass(1), true);
		cdl.addData(0, new TestClass(0), true);
		cdl.addData(2, new TestClass(2), true);
		cdl.addData(3, new TestClass(3), true);
		cdl.addData(4, new TestClass(4), true);

		REQUIRE(cdl.getData(1) == nullptr);
		REQUIRE(cdl.getData(0) != nullptr);
		REQUIRE(cdl.getData(0)->getID() == 0);
		REQUIRE(cdl.getData(2) != nullptr);
		REQUIRE(cdl.getData(2)->getID() == 2);
		REQUIRE(cdl.getData(3) != nullptr);
		REQUIRE(cdl.getData(3)->getID() == 3);
		REQUIRE(cdl.getData(4) != nullptr);
		REQUIRE(cdl.getData(4)->getID() == 4);
	}
}