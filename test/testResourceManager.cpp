#include <catch2/catch_amalgamated.hpp>
#include "ResourceManager.h"

//Relies on SmartMemory so if that has been properly unit tested, this becomes redundant
//Only need to test adding and deletion.
TEST_CASE("Testing of the ResourceManager class", "[ResourceManager]")
{
	ResourceManager<int> man = ResourceManager<int>();

	SECTION("Test adding and deletion of a resource")
	{
		man.addResource(new int[1], "Key1", false);
		REQUIRE(man.getResource("Key1").getPointer() != nullptr);
		man.deleteResource("Key1");
		REQUIRE(man.getResource("Key1").getPointer() == nullptr);
	}
}