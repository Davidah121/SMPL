#include <catch2/catch_amalgamated.hpp>
#include "SimpleXml.h"

//Relies on SmartMemory so if that has been properly unit tested, this becomes redundant
//Only need to test adding and deletion.
TEST_CASE("Testing of the SimpleXml class", "[SimpleXml]")
{
	const std::string rawXML = R"R0N0(
		<!DOCTYPE html>
		<html>
			<head>
				<title>Title 123</title>
			</head>
			<body>
				<!--<h1>Title Header</h1> -->
				<p class="testClass" id="42P">This is text</p>
			</body>
		</html>
	)R0N0";
	SECTION("Test loading an xml from a file or series of bytes")
	{
		glib::SimpleXml xml = glib::SimpleXml();
		xml.loadFromBytes((unsigned char*)rawXML.data(), rawXML.size());
		//check for the right stuff

		REQUIRE(xml.getNodes().size() == 2);
		REQUIRE(xml.getNodes()[0]->getTitle() == "!DOCTYPE");
		REQUIRE(xml.getNodes()[0]->getRawAttributes().getSize() > 0);
		REQUIRE(xml.getNodes()[0]->getAttribute("html") != nullptr);
		REQUIRE(xml.getNodes()[1]->getTitle() == "html");

		REQUIRE(xml.getNodes()[1]->getChildNodes().size() == 2);
		
		REQUIRE(xml.getNodes()[1]->getChildNodes()[0]->getTitle() == "head");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1]->getTitle() == "body");

		
		REQUIRE(xml.getNodes()[1]->getChildNodes()[0]->getChildNodes().size() == 1);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[0]->getChildNodes()[0]->getTitle() == "title");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[0]->getChildNodes()[0]->getValue() == "Title 123");

		REQUIRE(xml.getNodes()[1]->getChildNodes()[1]->getChildNodes().size() == 1);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1]->getChildNodes()[0]->getTitle() == "p");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1]->getChildNodes()[0]->getValue() == "This is text");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1]->getChildNodes()[0]->getAttribute("class") != nullptr);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1]->getChildNodes()[0]->getAttribute("id") != nullptr);
	}

	SECTION("Test searching for node")
	{
		glib::SimpleXml xml = glib::SimpleXml();
		xml.loadFromBytes((unsigned char*)rawXML.data(), rawXML.size());

		std::vector<std::string> searches = {"html"};
		std::vector<glib::XmlNode*> result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 1);

		searches = {"html", "body"};
		result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 1);


		searches = {"html", "body", "p"};
		result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 1);
		
		
		searches = {"html", "body", "q"};
		result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 0);
		
		
		searches = {"body", "body", "p"};
		result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 0);
	}
}