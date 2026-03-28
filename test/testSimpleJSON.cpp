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
			[
				"pair1": "data1",
				"pair2": "data2",
				"pair3": "1"
			],
			{
				"pairO1": "data01",
				"pair02": "dataO2",
				"pair3": "1"
			}
		}
	)R0N0";

	const std::string rawJSON2 = R"R0N0(
		[
			{
				"index1": "data1",
				"index2": "data2"
			}
		]
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
		REQUIRE(((smpl::JObject*)json.getRootNode()) != nullptr);
		REQUIRE(((smpl::JObject*)json.getRootNode())->getNodes().size() == 6);
		REQUIRE(((smpl::JObject*)json.getRootNode())->getNodes()[0]->getType() == smpl::JPair::TYPE);
		REQUIRE(((smpl::JObject*)json.getRootNode())->getNodes()[1]->getType() == smpl::JPair::TYPE);

		REQUIRE(((smpl::JPair*)((smpl::JObject*)json.getRootNode())->getNodes()[0])->getName() == "index1");
		REQUIRE(((smpl::JPair*)((smpl::JObject*)json.getRootNode())->getNodes()[0])->getValue() == "data");
		
		REQUIRE(((smpl::JPair*)((smpl::JObject*)json.getRootNode())->getNodes()[1])->getName() == "index2");
		REQUIRE(((smpl::JPair*)((smpl::JObject*)json.getRootNode())->getNodes()[1])->getValue() == "data2");

		REQUIRE(((smpl::JObject*)json.getRootNode())->getNodes()[2]->getType() == smpl::JObject::TYPE);
		REQUIRE(((smpl::JObject*)((smpl::JObject*)json.getRootNode())->getNodes()[2])->getName() == "object");
		REQUIRE(((smpl::JObject*)((smpl::JObject*)json.getRootNode())->getNodes()[2])->getNodes().size() == 2);
		REQUIRE(((smpl::JPair*)((smpl::JObject*)((smpl::JObject*)json.getRootNode())->getNodes()[2])->getNodes()[1])->getValue() == "1");
		
		REQUIRE(((smpl::JObject*)json.getRootNode())->getNodes()[3]->getType() == smpl::JArray::TYPE);
		REQUIRE(((smpl::JArray*)((smpl::JObject*)json.getRootNode())->getNodes()[3])->getName() == "array");
		
		REQUIRE(((smpl::JArray*)((smpl::JObject*)json.getRootNode())->getNodes()[3])->getNodes()[0]->getType() == smpl::JPair::TYPE);
		REQUIRE(((smpl::JArray*)((smpl::JObject*)json.getRootNode())->getNodes()[3])->getNodes()[1]->getType() == smpl::JPair::TYPE);
		REQUIRE(((smpl::JArray*)((smpl::JObject*)json.getRootNode())->getNodes()[3])->getNodes()[2]->getType() == smpl::JPair::TYPE);
		
		REQUIRE(((smpl::JArray*)((smpl::JObject*)json.getRootNode())->getNodes()[3])->getNodes()[0]->getName() == "v1");
		REQUIRE(((smpl::JArray*)((smpl::JObject*)json.getRootNode())->getNodes()[3])->getNodes()[1]->getName() == "v2");
		REQUIRE(((smpl::JArray*)((smpl::JObject*)json.getRootNode())->getNodes()[3])->getNodes()[2]->getName() == "v3");
		
	}

	SECTION("Test searching for node")
	{
		smpl::SimpleJSON json = smpl::SimpleJSON((unsigned char*)rawJSON.data(), rawJSON.size());
		REQUIRE(json.getRootNode()->getType() == smpl::JObject::TYPE);

		std::vector<smpl::JNode*> result = json.getNodesPattern({"index1"});
		REQUIRE(result.size() == 1);

		result = json.getNodesPattern({"object"});
		REQUIRE(result.size() == 1);

		result = json.getNodesPattern({"object", "objVar1"});
		REQUIRE(result.size() == 1);

		result = json.getNodesPattern({"array", "v4"});
		REQUIRE(result.size() == 0);
		
		result = json.getNodesPattern({""});
		REQUIRE(result.size() == 2);
		
		result = json.getNodesPattern({"", "pairO1"});
		REQUIRE(result.size() == 1);
		
		result = json.getNodesPattern({"", "pair3"});
		REQUIRE(result.size() == 2);
	}

	SECTION("2nd Test for searching for JSON nodes")
	{
		smpl::SimpleJSON json = smpl::SimpleJSON((unsigned char*)rawJSON2.data(), rawJSON2.size());
		REQUIRE(json.getRootNode()->getType() == smpl::JArray::TYPE);

		std::vector<smpl::JNode*> result = json.getNodesPattern({""});
		REQUIRE(result.size() == 1);

		result = json.getNodesPattern({"", "index1"});
		REQUIRE(result.size() == 1);
		
		result = json.getNodesPattern({"", "index2"});
		REQUIRE(result.size() == 1);
	}
	
	SECTION("Test for reading in escaped linebreaks in value part of JPair nodes")
	{
		smpl::SimpleJSON json = smpl::SimpleJSON((unsigned char*)rawJSON3.data(), rawJSON3.size());
		REQUIRE(json.getRootNode()->getType() == smpl::JArray::TYPE);
		REQUIRE(json[0]["index1"].getType() == smpl::JPair::TYPE);
		REQUIRE(json[0]["index2"].getType() == smpl::JPair::TYPE);
		
		REQUIRE(((smpl::JPair&)json[0]["index1"]).getValue() == "data1\nLinebreak");
		REQUIRE(((smpl::JPair&)json[0]["index2"]).getValue() == "data2");
	}

	SECTION("Test for linebreaks in value part of JPair nodes")
	{
		smpl::JPair testPair = smpl::JPair("index1", "data1\nLinebreak");
		REQUIRE(testPair.getValue() == "data1\nLinebreak");
		REQUIRE(testPair.getString(true) == "\"index1\": \"data1\\nLinebreak\"");
	}
}