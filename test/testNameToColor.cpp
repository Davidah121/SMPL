#include <catch2/catch_amalgamated.hpp>
#include "ColorNameConverter.h"

TEST_CASE("Test the ColorNameConverter class" "[ColorNameConverter]")
{
	glib::Color Red = glib::ColorNameConverter::NameToColor("Red");
	glib::Color rEd = glib::ColorNameConverter::NameToColor("rEd");
	glib::Color red = {255, 0, 0, 255};
	REQUIRE(Red == rEd);
	REQUIRE(Red == red);

	glib::Color invalid = glib::ColorNameConverter::NameToColor("invalidColor1");
	glib::Color invalidC = {0, 0, 0, 0};
	REQUIRE(invalid == invalidC);

	std::string redName = glib::ColorNameConverter::ColorToName(red);
	std::string noneName = glib::ColorNameConverter::ColorToName(invalidC);
	std::string actuallyInvalid = glib::ColorNameConverter::ColorToName({1, 2, 3, 4});
	REQUIRE(redName == "red");
	REQUIRE(noneName == "none");
	REQUIRE(actuallyInvalid == "");
}