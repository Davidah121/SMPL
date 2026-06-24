#include <catch_amalgamated.hpp>
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
			],
			"array2": [
				"pair1": "data1",
				"pair2": "data2",
				"pair3": "1"
			],
			"object3": {
				"pairO1": "data01",
				"pair02": "dataO2",
				"pair3": "1"
			}
		}
	)R0N0";

	const std::string rawJSON3 = R"R0N0(
		[
			{
				"index1": "data1\nLinebreak",
				"index2": "data2"
			}
		]
	)R0N0";

	SECTION("Test loading an json from a file or series of bytes")
	{
		smpl::SimpleJSON json = smpl::SimpleJSON((unsigned char*)rawJSON.data(), rawJSON.size());
		//check for the right stuff
		//root node must be an JSON Object and not a JSON Array
		REQUIRE(json.getRootNode().getType() == smpl::JObject::TYPE);
		REQUIRE(json.getRootNode().getObject().size() == 6);
		REQUIRE(json.getRootNode()["index1"].getType() == smpl::JPair::TYPE);
		REQUIRE(json.getRootNode()["index1"].getPair().getValue() == "data");
		REQUIRE(json.getRootNode()["index2"].getType() == smpl::JPair::TYPE);
		REQUIRE(json.getRootNode()["index2"].getPair().getValue() == "data2");

		
		REQUIRE(json.getRootNode()["object"].getType() == smpl::JObject::TYPE);
		REQUIRE(json.getRootNode()["object"]["objVar1"].getType() == smpl::JPair::TYPE);
		REQUIRE(json.getRootNode()["object"]["objVar1"].getPair().getValue() == "data");
		REQUIRE(json.getRootNode()["object"]["objVar2"].getType() == smpl::JPair::TYPE);
		REQUIRE(json.getRootNode()["object"]["objVar2"].getPair().getValue() == "1");

		REQUIRE(json.getRootNode()["array"].getType() == smpl::JArray::TYPE);
		REQUIRE(json.getRootNode()["array"].getArray().size() == 3);
		REQUIRE(json.getRootNode()["array"][0].getType() == smpl::JPair::TYPE);
		REQUIRE(json.getRootNode()["array"][0].getName() == "v1");
		REQUIRE(json.getRootNode()["array"][0].getPair().getValue() == "");
		
		REQUIRE(json.getRootNode()["array"][1].getType() == smpl::JPair::TYPE);
		REQUIRE(json.getRootNode()["array"][1].getName() == "v2");
		REQUIRE(json.getRootNode()["array"][1].getPair().getValue() == "");
		
		REQUIRE(json.getRootNode()["array"][2].getType() == smpl::JPair::TYPE);
		REQUIRE(json.getRootNode()["array"][2].getName() == "v3");
		REQUIRE(json.getRootNode()["array"][2].getPair().getValue() == "");

		//could continue but you get the idea
	}

	SECTION("Test for reading in escaped linebreaks in value part of JPair nodes")
	{
		smpl::SimpleJSON json = smpl::SimpleJSON((unsigned char*)rawJSON3.data(), rawJSON3.size());
		REQUIRE(json.getRootNode().getType() == smpl::JArray::TYPE);
		REQUIRE(json.getRootNode().getArray().size() == 1);
		REQUIRE(json.getRootNode().getArray()[0].getType() == smpl::JObject::TYPE);
		
		REQUIRE(json.getRootNode().getArray()[0].getObject().size() == 2);
		REQUIRE(json.getRootNode().getArray()[0].getObject()["index1"].getType() == smpl::JPair::TYPE);
		REQUIRE(json.getRootNode().getArray()[0].getObject()["index2"].getType() == smpl::JPair::TYPE);
		
		
		REQUIRE(json.getRootNode().getArray()[0].getObject()["index1"].getPair().getValue() == "data1\nLinebreak");
		REQUIRE(json.getRootNode().getArray()[0].getObject()["index2"].getPair().getValue() == "data2");
	}

	SECTION("Test for linebreaks in value part of JPair nodes")
	{
		smpl::JPair testPair = smpl::JPair("index1", "data1\nLinebreak");
		REQUIRE(testPair.getValue() == "data1\nLinebreak");
		REQUIRE(testPair.getString(true) == "\"index1\": \"data1\\nLinebreak\"");
	}
}