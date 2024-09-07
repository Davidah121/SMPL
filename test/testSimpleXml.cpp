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
				<p>this is a <a href="link">link</a> and it is cool.</p>
			</body>
		</html>
	)R0N0";

	const std::string rawXML2 = R"R0N0(
		<TouchMacros>
			<Macro type="touch" x1="960" y1="540" padattribute="a"/>
		</TouchMacros>
	)R0N0";

	const std::string rawHTML = R"R0N0(
		<!DOCTYPE html>
		<html>
			<head>
				<meta stuff="I don't know"/>
			</head>
			<body>
				<form>
					<input id="1" type="text" value="default">
					<input id="2" type="text" value="default"></input>
					<input id="3" type="text" value="default"/>
				</form>
			</body>
		</html>
	)R0N0";

	SECTION("Test loading an xml from a file or series of bytes")
	{
		smpl::SimpleXml xml = smpl::SimpleXml();
		xml.loadFromBytes((unsigned char*)rawXML.data(), rawXML.size());
		//check for the right stuff

		REQUIRE(xml.getNodes().size() == 2);
		REQUIRE(xml.getNodes()[0]->getTitle() == "!doctype");
		REQUIRE(xml.getNodes()[0]->getRawAttributes().size() > 0);
		REQUIRE(xml.getNodes()[0]->getAttribute("html") != nullptr);
		REQUIRE(xml.getNodes()[1]->getTitle() == "html");

		REQUIRE(xml.getNodes()[1]->getChildNodes().size() == 2);
		
		REQUIRE(xml.getNodes()[1]->getChildNodes()[0].node->getTitle() == "head");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getTitle() == "body");

		
		REQUIRE(xml.getNodes()[1]->getChildNodes()[0].node->getChildNodes().size() == 1);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[0].node->getChildNodes()[0].node->getTitle() == "title");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[0].node->getChildNodes()[0].node->getValue() == "Title 123");

		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes().size() == 2);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[0].node->getTitle() == "p");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[0].node->getValue() == "This is text");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[0].node->getAttribute("class") != nullptr);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[0].node->getAttribute("id") != nullptr);
		
		//Test where value and child nodes appear together and make sure that they appear one after the other.
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[1].node->getTitle() == "p");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[1].node->getChildNodes().size() == 3);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[1].node->getChildNodes()[0].type == false);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[1].node->getChildNodes()[0].value == "this is a ");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[1].node->getChildNodes()[1].type == true);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[1].node->getChildNodes()[1].node->getTitle() == "a");
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[1].node->getChildNodes()[2].type == false);
		REQUIRE(xml.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[1].node->getChildNodes()[2].value == " and it is cool.");
		
	}

	SECTION("Test searching for node")
	{
		smpl::SimpleXml xml = smpl::SimpleXml();
		xml.loadFromBytes((unsigned char*)rawXML.data(), rawXML.size());

		std::vector<std::string> searches = {"html"};
		std::vector<smpl::XmlNode*> result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 1);

		searches = {"html", "body"};
		result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 1);


		searches = {"html", "body", "p"};
		result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 2);
		
		
		searches = {"html", "body", "q"};
		result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 0);
		
		
		searches = {"body", "body", "p"};
		result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 0);
	}

	SECTION("Test searching for node 2")
	{
		smpl::SimpleXml xml = smpl::SimpleXml();
		xml.loadFromBytes((unsigned char*)rawXML2.data(), rawXML2.size());

		std::vector<std::string> searches = {"touchmacros", "macro"};
		std::vector<smpl::XmlNode*> result = xml.getNodesPattern(searches);
		REQUIRE(result.size() == 1);

		std::pair<std::string, std::string>* attribs = result[0]->getAttribute("type");
		REQUIRE(attribs != nullptr);
		REQUIRE(attribs->second == "touch");
	}

	SECTION("Test HTML void elements")
	{
		smpl::SimpleXml html = smpl::SimpleXml();
		html.loadFromBytes((unsigned char*)rawHTML.data(), rawHTML.size());

		//Check type and valid xml. Should be html and invalid xml since one tag is not closed
		REQUIRE(html.getType() == smpl::SimpleXml::TYPE_HTML);
		REQUIRE(html.getValidXml() == false);

		//Check Sizes
		REQUIRE(html.getNodes().size() == 2);
		REQUIRE(html.getNodes()[1]->getChildNodes().size() == 2);
		REQUIRE(html.getNodes()[1]->getChildNodes()[0].node->getChildNodes().size() == 1);
		REQUIRE(html.getNodes()[1]->getChildNodes()[1].node->getChildNodes().size() == 1);
		REQUIRE(html.getNodes()[1]->getChildNodes()[1].node->getChildNodes()[0].node->getChildNodes().size() == 3);
		
		//Check search code
		std::vector<std::string> searches = {"html", "body", "form", "input"};
		std::vector<smpl::XmlNode*> result = html.getNodesPattern(searches);
		REQUIRE(result.size() == 3);
	}
}