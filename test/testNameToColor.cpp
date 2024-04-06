#include <catch2/catch_amalgamated.hpp>
#include "ColorNameConverter.h"

TEST_CASE("Test the ColorNameConverter class" "[ColorNameConverter]")
{
	smpl::Color Red = smpl::ColorNameConverter::NameToColor("Red");
	smpl::Color rEd = smpl::ColorNameConverter::NameToColor("rEd");
	smpl::Color red = {255, 0, 0, 255};
	REQUIRE((Red == rEd));
	REQUIRE((Red == red));

	smpl::Color invalid = smpl::ColorNameConverter::NameToColor("invalidColor1");
	smpl::Color invalidC = {0, 0, 0, 0};
	REQUIRE((invalid == invalidC));

	std::string redName = smpl::ColorNameConverter::ColorToName(red);
	std::string noneName = smpl::ColorNameConverter::ColorToName(invalidC);
	std::string actuallyInvalid = smpl::ColorNameConverter::ColorToName({1, 2, 3, 4});
	REQUIRE((redName == "red"));
	REQUIRE((noneName == "none"));
	REQUIRE((actuallyInvalid == ""));
}