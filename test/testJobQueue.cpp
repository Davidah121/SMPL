#include <catch2/catch_amalgamated.hpp>
#include "SimpleJobQueue.h"
#include "System.h"

TEST_CASE("Testing of the SimpleJobQueue class", "[SimpleJobQueue]")
{
	SECTION("Test the creation of a simple job")
	{
		unsigned int data = 1;
		smpl::SimpleJobQueue jobQueue = smpl::SimpleJobQueue(1);
		REQUIRE(data == 1);
		jobQueue.addJob([&data]()->void{
			data = 5;
		});

		while(!jobQueue.allJobsDone())
		{
			smpl::System::sleep(1);
		}
		REQUIRE(data == 5);
	}

	SECTION("Test creation of multiple jobs")
	{
		std::vector<unsigned int> data = std::vector<unsigned int>(5);
		smpl::SimpleJobQueue jobQueue = smpl::SimpleJobQueue(3);
		jobQueue.addJob([&data]()->void{
			data[0] = 32;
			smpl::System::sleep(1);
		});
		jobQueue.addJob([&data]()->void{
			data[1] = 64;
			smpl::System::sleep(1);
		});
		jobQueue.addJob([&data]()->void{
			data[2] = 96;
			smpl::System::sleep(1);
		});
		jobQueue.addJob([&data]()->void{
			data[3] = 128;
			smpl::System::sleep(1);
		});
		jobQueue.addJob([&data]()->void{
			data[4] = 100;
			smpl::System::sleep(1);
		});
		
		while(!jobQueue.allJobsDone())
		{
			smpl::System::sleep(1);
		}

		REQUIRE(data[0] == 32);
		REQUIRE(data[1] == 64);
		REQUIRE(data[2] == 96);
		REQUIRE(data[3] == 128);
		REQUIRE(data[4] == 100);
	}

	SECTION("Test the deletion of jobs in the queue not yet executed.")
	{
		std::vector<unsigned int> data = {1, 2, 3, 4, 5};
		smpl::SimpleJobQueue jobQueue = smpl::SimpleJobQueue(2);
		jobQueue.addJob([&data]()->void{
			data[0] = 32;
			smpl::System::sleep(1);
		});
		jobQueue.addJob([&data]()->void{
			data[1] = 64;
			smpl::System::sleep(1);
		});
		jobQueue.addJob([&data]()->void{
			data[2] = 96;
			smpl::System::sleep(1);
		});
		jobQueue.addJob([&data]()->void{
			data[3] = 128;
			smpl::System::sleep(1);
		});
		size_t jobToDelete = jobQueue.addJob([&data]()->void{
			data[4] = 100;
			smpl::System::sleep(1);
		});

		jobQueue.removeJob(jobToDelete);
		
		while(!jobQueue.allJobsDone())
		{
			smpl::System::sleep(1);
		}
		
		REQUIRE(data[0] == 32);
		REQUIRE(data[1] == 64);
		REQUIRE(data[2] == 96);
		REQUIRE(data[3] == 128);
		REQUIRE(data[4] == 5);
	}
}