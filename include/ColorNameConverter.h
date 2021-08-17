#pragma once
#include <iostream>
#include <unordered_map>
#include "ColorPalette.h"

namespace glib
{

	class ColorNameConverter
	{
	public:
		ColorNameConverter();
		~ColorNameConverter();

		static std::string ColorToName(Color c);
		static Color NameToColor(std::string n);
	private:
		static const std::unordered_map<std::string, Color> colorNamePair;
	};

}  //NAMESPACE glib END