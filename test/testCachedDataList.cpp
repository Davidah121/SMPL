#include <catch_amalgamated.hpp>
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
		smpl::CachedDataList<size_t, TestClass> cdl = smpl::CachedDataList<size_t, TestClass>(4);

		cdl.addData(0, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(0)));
		REQUIRE((cdl.getData(0) != nullptr));
		REQUIRE(cdl.getData(0)->getID() == 0);
		
		cdl.addData(1, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(1)));
		REQUIRE((cdl.getData(1) != nullptr));
		REQUIRE(cdl.getData(1)->getID() == 1);

		cdl.addData(0, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(2)));
		REQUIRE((cdl.getData(0) != nullptr));
		REQUIRE(cdl.getData(0)->getID() == 2);
	}

	SECTION("Remove data")
	{
		smpl::CachedDataList<size_t, TestClass> cdl = smpl::CachedDataList<size_t, TestClass>(4);

		cdl.addData(0, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(0)));
		cdl.addData(1, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(1)));
		cdl.addData(2, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(2)));

		cdl.removeData(0);

		REQUIRE((cdl.getData(0) == nullptr));
		REQUIRE((cdl.getData(1) != nullptr));
		REQUIRE(cdl.getData(1)->getID() == 1);
		REQUIRE((cdl.getData(2) != nullptr));
		REQUIRE(cdl.getData(2)->getID() == 2);
	}

	SECTION("Run out of cache")
	{
		smpl::CachedDataList<size_t, TestClass> cdl = smpl::CachedDataList<size_t, TestClass>(4);
		
		cdl.addData(0, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(0)));
		cdl.addData(1, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(1)));
		cdl.addData(2, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(2)));
		cdl.addData(3, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(3)));
		cdl.addData(4, smpl::SmartMemory<TestClass>::createDeleteRights(new TestClass(4)));

		REQUIRE((cdl.getData(0) == nullptr)); //oldest one

		REQUIRE((cdl.getData(1) != nullptr));
		REQUIRE(cdl.getData(1)->getID() == 1);

		REQUIRE((cdl.getData(2) != nullptr));
		REQUIRE(cdl.getData(2)->getID() == 2);

		REQUIRE((cdl.getData(3) != nullptr));
		REQUIRE(cdl.getData(3)->getID() == 3);
		
		REQUIRE((cdl.getData(4) != nullptr));
		REQUIRE(cdl.getData(4)->getID() == 4);
	}
}