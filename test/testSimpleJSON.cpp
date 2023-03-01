#include <catch2/catch_amalgamated.hpp>
#include "SimpleJSON.h"

//Relies on SmartMemory so if that has been properly unit tested, this becomes redundant
//Only need to test adding and deletion.
TEST_CASE("Testing of the SimpleJSON class", "[SimpleJSON]")
{
	const std::string rawJSON = R"R0N0(
		{
			"index1": "data",
			"index2": "data2",
			"object": {
				"objVar1": "data",
				"objVar2": 1
			},
			"array": [
				"v1",
				"v2",
				"v3"
			]
		}
	)R0N0";
	SECTION("Test loading an json from a file or series of bytes")
	{
		glib::SimpleJSON json = glib::SimpleJSON();
		json.load((unsigned char*)rawJSON.data(), rawJSON.size());
		//check for the right stuff

		REQUIRE(json.getRootNode() != nullptr);
		REQUIRE(json.getRootNode()->getNodes().size() == 4);
		REQUIRE(json.getRootNode()->getNodes()[0]->getType() == glib::SimpleJSON::TYPE_PAIR);
		REQUIRE(json.getRootNode()->getNodes()[1]->getType() == glib::SimpleJSON::TYPE_PAIR);

		REQUIRE(((glib::JPair*)json.getRootNode()->getNodes()[0])->getName() == "index1");
		REQUIRE(((glib::JPair*)json.getRootNode()->getNodes()[0])->getValue() == "data");
		
		REQUIRE(((glib::JPair*)json.getRootNode()->getNodes()[1])->getName() == "index2");
		REQUIRE(((glib::JPair*)json.getRootNode()->getNodes()[1])->getValue() == "data2");

		REQUIRE(json.getRootNode()->getNodes()[2]->getType() == glib::SimpleJSON::TYPE_OBJECT);
		REQUIRE(((glib::JObject*)json.getRootNode()->getNodes()[2])->getName() == "object");
		REQUIRE(((glib::JObject*)json.getRootNode()->getNodes()[2])->getNodes().size() == 2);
		REQUIRE(((glib::JPair*)((glib::JObject*)json.getRootNode()->getNodes()[2])->getNodes()[1])->getValue() == "1");
		
		REQUIRE(json.getRootNode()->getNodes()[3]->getType() == glib::SimpleJSON::TYPE_ARRAY);
		REQUIRE(((glib::JArray*)json.getRootNode()->getNodes()[3])->getName() == "array");
		
		REQUIRE(((glib::JArray*)json.getRootNode()->getNodes()[3])->getNodes()[0]->getType() == glib::SimpleJSON::TYPE_PAIR);
		REQUIRE(((glib::JArray*)json.getRootNode()->getNodes()[3])->getNodes()[1]->getType() == glib::SimpleJSON::TYPE_PAIR);
		REQUIRE(((glib::JArray*)json.getRootNode()->getNodes()[3])->getNodes()[2]->getType() == glib::SimpleJSON::TYPE_PAIR);
		
		REQUIRE(((glib::JArray*)json.getRootNode()->getNodes()[3])->getNodes()[0]->getName() == "v1");
		REQUIRE(((glib::JArray*)json.getRootNode()->getNodes()[3])->getNodes()[1]->getName() == "v2");
		REQUIRE(((glib::JArray*)json.getRootNode()->getNodes()[3])->getNodes()[2]->getName() == "v3");
		
	}

	SECTION("Test searching for node")
	{
		glib::SimpleJSON json = glib::SimpleJSON();
		json.load((unsigned char*)rawJSON.data(), rawJSON.size());

		std::vector<std::string> searches = {"index1"};
		glib::JNode* result = json.getNode(searches);
		REQUIRE(result != nullptr);

		searches = {"object"};
		result = json.getNode(searches);
		REQUIRE(result != nullptr);

		searches = {"object", "objVar1"};
		result = json.getNode(searches);
		REQUIRE(result != nullptr);

		searches = {"array", "v4"};
		result = json.getNode(searches);
		REQUIRE(result == nullptr);
	}
}