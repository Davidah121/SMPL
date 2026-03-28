#include <catch_amalgamated.hpp>
#include <cstddef>
#include "SimpleHashTable.h"

TEST_CASE("Testing of the SimpleHashTable class", "[SimpleHashMap]")
{
	
	SECTION("Test insertion")
	{
		smpl::SimpleHashMap<size_t, size_t> map;
		map.insert({32, 0});
		map.insert({31, 33});

		REQUIRE(map.size() == 2);
	}

	SECTION("Test find")
	{
		smpl::SimpleHashMap<size_t, size_t> map;
		map.insert({32, 0});
		map.insert({31, 33});

		auto it1 = map.find(32);
		auto it2 = map.find(31);
		auto it3 = map.find(30);

		REQUIRE(it1 != map.end());
		REQUIRE(it1->first == 32);
		REQUIRE(it1->second == 0);

		REQUIRE(it2 != map.end());
		REQUIRE(it2->first == 31);
		REQUIRE(it2->second == 33);

		REQUIRE(it3 == map.end());
	}

	SECTION("Test duplicated insertion")
	{
		smpl::SimpleHashMap<size_t, size_t> map;
		map.insert({32, 0});

		auto it1 = map.find(32);

		REQUIRE(it1 != map.end());
		REQUIRE(it1->first == 32);
		REQUIRE(it1->second == 0);

		it1 = map.insert({32, 33});
		REQUIRE(it1 != map.end());
		REQUIRE(it1->first == 32);
		REQUIRE(it1->second == 0);
	}

	SECTION("Test delete")
	{
		smpl::SimpleHashMap<size_t, size_t> map;
		map.insert({32, 0});
		map.insert({31, 33});
		map.insert({30, 90});

		map.erase(32);
		auto it1 = map.find(32);
		auto it2 = map.find(31);
		auto it3 = map.find(30);

		REQUIRE(it1 == map.end());
		REQUIRE(it2 != map.end());
		REQUIRE(it3 != map.end());

		map.erase(30);
		it1 = map.find(32);
		it2 = map.find(31);
		it3 = map.find(30);

		REQUIRE(it1 == map.end());
		REQUIRE(it2 != map.end());
		REQUIRE(it3 == map.end());

		
		smpl::SimpleHashMap<size_t, size_t, std::hash<size_t>> map2 = {{0, 0}, {1, 0}, {smpl::SimpleHashMap<size_t, size_t>::MINIMUM_BUCKETS, 0}};
		//this causes the 3rd value to attempt to go to the same location as the first value. This is important because it gets offset by 2 locations as we are using the identity hash.
		//upon deletion of the first element, we should still be able to access every element.
		auto it1_2 = map2.find(0);
		auto it2_2 = map2.find(1);
		auto it3_2 = map2.find(smpl::SimpleHashMap<size_t, size_t>::MINIMUM_BUCKETS);

		REQUIRE(it1_2 != map2.end());
		REQUIRE(it2_2 != map2.end());
		REQUIRE(it3_2 != map2.end());
		
		map2.erase(0);
		it1_2 = map2.find(0);
		it2_2 = map2.find(1);
		it3_2 = map2.find(smpl::SimpleHashMap<size_t, size_t>::MINIMUM_BUCKETS);
		REQUIRE(it1_2 == map2.end());
		REQUIRE(it2_2 != map2.end());
		REQUIRE(it3_2 != map2.end());
	}

	SECTION("Test copy")
	{
		smpl::SimpleHashMap<size_t, size_t> map;
		map.insert({32, 0});
		map.insert({31, 33});

		smpl::SimpleHashMap<size_t, size_t> map2 = map;

		REQUIRE(map.size() == 2);
		REQUIRE(map2.size() == 2);

		map.clear();

		REQUIRE(map.size() == 0);
		REQUIRE(map2.size() == 2);
	}

	SECTION("Test operator[]")
	{
		smpl::SimpleHashMap<size_t, size_t> map;
		map[32] = 1;
		map[33] = 2;
		REQUIRE(map.size() == 2);
	}
	
	SECTION("Test large insert")
	{
		smpl::SimpleHashMap<size_t, size_t> map;
		for(int i=0; i<10000; i++)
		{
			map[i] = i;
		}

		REQUIRE(map.size() == 10000);
	}
}

TEST_CASE("Testing of the SimpleHashTable class", "[SimpleHashSet]")
{
	
	SECTION("Test insertion")
	{
		smpl::SimpleHashSet<size_t> map;
		map.insert(32);
		map.insert(31);

		REQUIRE(map.size() == 2);
	}

	SECTION("Test find")
	{
		smpl::SimpleHashSet<size_t> map;
		map.insert(32);
		map.insert(31);

		auto it1 = map.find(32);
		auto it2 = map.find(31);
		auto it3 = map.find(30);

		REQUIRE(it1 != map.end());
		REQUIRE(*it1 == 32);

		REQUIRE(it2 != map.end());
		REQUIRE(*it2 == 31);

		REQUIRE(it3 == map.end());
	}

	SECTION("Test delete")
	{
		smpl::SimpleHashSet<size_t> map;
		map.insert(32);
		map.insert(31);
		map.insert(30);

		map.erase(32);
		auto it1 = map.find(32);
		auto it2 = map.find(31);
		auto it3 = map.find(30);

		REQUIRE(it1 == map.end());
		REQUIRE(it2 != map.end());
		REQUIRE(it3 != map.end());

		map.erase(30);
		it1 = map.find(32);
		it2 = map.find(31);
		it3 = map.find(30);

		REQUIRE(it1 == map.end());
		REQUIRE(it2 != map.end());
		REQUIRE(it3 == map.end());
		
		smpl::SimpleHashSet<size_t, std::hash<size_t>> map2 = {0, 1, smpl::SimpleHashSet<size_t>::MINIMUM_BUCKETS};
		//this causes the 3rd value to attempt to go to the same location as the first value. This is important because it gets offset by 2 locations as we are using the identity hash.
		//upon deletion of the first element, we should still be able to access every element.
		auto it1_2 = map2.find(0);
		auto it2_2 = map2.find(1);
		auto it3_2 = map2.find(smpl::SimpleHashSet<size_t>::MINIMUM_BUCKETS);

		REQUIRE(it1_2 != map2.end());
		REQUIRE(it2_2 != map2.end());
		REQUIRE(it3_2 != map2.end());
		
		map2.erase(0);
		it1_2 = map2.find(0);
		it2_2 = map2.find(1);
		it3_2 = map2.find(smpl::SimpleHashSet<size_t>::MINIMUM_BUCKETS);
		REQUIRE(it1_2 == map2.end());
		REQUIRE(it2_2 != map2.end());
		REQUIRE(it3_2 != map2.end());
	}

	SECTION("Test copy")
	{
		smpl::SimpleHashSet<size_t> map;
		map.insert(32);
		map.insert(31);

		smpl::SimpleHashSet<size_t> map2 = map;

		REQUIRE(map.size() == 2);
		REQUIRE(map2.size() == 2);

		map.clear();

		REQUIRE(map.size() == 0);
		REQUIRE(map2.size() == 2);
	}
	
	SECTION("Test large insert")
	{
		smpl::SimpleHashSet<size_t> map;
		for(int i=0; i<10000; i++)
		{
			map.insert(i);
		}

		REQUIRE(map.size() == 10000);
	}
}

TEST_CASE("Testing of the SimpleHashTable class using MULTI flag", "[SimpleHashMultiMap]")
{
	
	SECTION("Test insertion")
	{
		smpl::SimpleHashMultiMap<size_t, size_t> map;
		map.insert({32, 0});
		map.insert({31, 33});

		REQUIRE(map.size() == 2);
		
		map.insert({31, 34});

		REQUIRE(map.size() == 3);

		auto it = map.begin();
		REQUIRE(it->first == 32);
		REQUIRE(it->second == 0);
		
		++it;
		REQUIRE(it->first == 31);
		REQUIRE(it->second == 33);
		
		++it;
		REQUIRE(it->first == 31);
		REQUIRE(it->second == 34);
	}

	SECTION("Test find")
	{
		smpl::SimpleHashMultiMap<size_t, size_t> map;
		map.insert({32, 0});
		map.insert({31, 33});
		map.insert({31, 34});

		auto it1 = map.find(32);
		auto it2 = map.find(31);
		auto it3 = map.find(30);

		REQUIRE(it1 != map.end());
		REQUIRE(it1->first == 32);
		REQUIRE(it1->second == 0);

		REQUIRE(it2 != map.end());
		REQUIRE(it2->first == 31);
		REQUIRE(it2->second == 33);

		++it2;
		REQUIRE(it2 != map.end());
		REQUIRE(it2->first == 31);
		REQUIRE(it2->second == 34);

		++it2;
		REQUIRE(it2 == map.end());
		REQUIRE(it3 == map.end());
	}

	SECTION("Test delete")
	{
		smpl::SimpleHashMultiMap<size_t, size_t> map;
		map.insert({32, 0});
		map.insert({31, 33});
		map.insert({31, 34});
		map.insert({30, 90});
		map.insert({30, 91});
		map.insert({30, 92});

		map.erase(32);
		auto it1 = map.find(32);
		auto it2 = map.find(31);
		auto it3 = map.find(30);

		REQUIRE(it1 == map.end());
		REQUIRE(it2 != map.end());
		REQUIRE(it3 != map.end());
		REQUIRE(map.size() == 5);

		map.erase(31);
		it1 = map.find(32);
		it2 = map.find(31);
		it3 = map.find(30);

		REQUIRE(it1 == map.end());
		REQUIRE(it2 == map.end());
		REQUIRE(it3 != map.end());
		REQUIRE(map.size() == 3);

		map.erase(++it3);
		
		it3 = map.find(30);
		REQUIRE(map.size() == 2);
		REQUIRE(it3->first == 30);
		REQUIRE((it3->second == 90 || it3->second == 92));
		++it3;
		REQUIRE(it3->first == 30);
		REQUIRE((it3->second == 90 || it3->second == 92));
		REQUIRE(map.size() == 2);
	}

	SECTION("Test copy")
	{
		smpl::SimpleHashMultiMap<size_t, size_t> map;
		map.insert({32, 0});
		map.insert({31, 33});
		map.insert({31, 34});

		smpl::SimpleHashMultiMap<size_t, size_t> map2 = map;

		REQUIRE(map.size() == 3);
		REQUIRE(map2.size() == 3);

		map.clear();

		REQUIRE(map.size() == 0);
		REQUIRE(map2.size() == 3);
	}

	SECTION("Test operator[]")
	{
		smpl::SimpleHashMultiMap<size_t, size_t> map;
		map[32] = 1;
		map[33] = 2;
		REQUIRE(map.size() == 2);
	}
	
	SECTION("Test large insert")
	{
		smpl::SimpleHashMultiMap<size_t, size_t> map;
		for(int i=0; i<10000; i++)
		{
			map[i%256] = i;
		}

		REQUIRE(map.size() == 10000);
	}
}