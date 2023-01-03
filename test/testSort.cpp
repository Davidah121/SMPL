#include <catch2/catch_amalgamated.hpp>
#include <Sort.h>

TEST_CASE("Testing of Sorting and Shuffle Algorithms", "[Sort]")
{
	std::vector<int> data = {4, 0, 3, 2, 9, 8, 7, 1, 5, 6};
	
	SECTION( "INSERTION SORT TEST" )
	{
		std::vector<int> testData = data;
		glib::Sort::insertionSort(testData.data(), testData.size());
		bool valid = true;
		for(int i=0; i<testData.size(); i++)
		{
			if(i != testData[i])
			{
				valid = false;
				break;
			}
		}
		REQUIRE(testData.size() == data.size());
		REQUIRE(valid == true);
	}
	SECTION( "MERGE SORT TEST" )
	{
		std::vector<int> testData = data;
		glib::Sort::mergeSort(testData.data(), testData.size());
		bool valid = true;
		for(int i=0; i<testData.size(); i++)
		{
			if(i != testData[i])
			{
				valid = false;
				break;
			}
		}
		REQUIRE(testData.size() == data.size());
		REQUIRE(valid == true);
	}
	SECTION( "QUICK SORT TEST" )
	{
		std::vector<int> testData = data;
		glib::Sort::quickSort(testData.data(), testData.size());
		bool valid = true;
		for(int i=0; i<testData.size(); i++)
		{
			if(i != testData[i])
			{
				valid = false;
				break;
			}
		}
		REQUIRE(testData.size() == data.size());
		REQUIRE(valid == true);
	}
	SECTION( "SHUFFLE TEST" )
	{
		std::vector<int> testData = data;
		std::vector<bool> appeared = std::vector<bool>(data.size());

		glib::Sort::shuffle(testData.data(), testData.size());
		bool valid = true;
		//All points must appear. All unique and up to 10 so simple test without frequency checking.
		for(int i=0; i<appeared.size(); i++)
		{
			appeared[ testData[i] ] = true;
		}
		for(int i=0; i<appeared.size(); i++)
		{
			if(appeared[i] != true)
			{
				valid = false;
				break;
			}
		}
		
		REQUIRE(appeared.size() == data.size());
		REQUIRE(valid == true);
	}
}
