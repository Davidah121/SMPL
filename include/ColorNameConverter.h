#pragma once
#include <iostream>
#include <unordered_map>
#include "ColorPalette.h"

namespace smpl
{

	class ColorNameConverter
	{
	public:

		/**
		 * @brief Converts a name to its represented color.
		 * 		Ex: black -> RGB(0,0,0)
		 * 		Ex: red -> RGB(255,0,0)
		 * @param n
		 * 		The name of the color
		 * @return Color
		 * 		Returns the correct color with alpha = 255 if successful. Otherwise, the default color RGBA(0,0,0,0) is returned.
		 */
		static Color NameToColor(std::string n);
		static Color NameToColor(std::wstring n);
		
		/**
		 * @brief Converts a color to its represented name
		 * 		Ex: RGB(0,0,0) -> black
		 * 		Ex: RGB(255,0,0) -> red
		 * 		Alpha is omitted in the search
		 * @param c
		 * 		The color to convert to a name
		 * @return std::string
		 * 		Returns a non empty string if the color could be found in the list of valid colors and names.
		 */
		static std::string ColorToName(Color c);
	private:
		static const std::unordered_map<std::string, Color> colorNamePair;
	};

}  //NAMESPACE smpl END