#pragma once
#include <vector>
#include "Object.h"

struct Color
{
	unsigned char red = 0;
	unsigned char green = 0;
	unsigned char blue = 0;
	unsigned char alpha = 0;
};

class ColorPalette : public Object
{
public:
	ColorPalette();
	~ColorPalette();
	ColorPalette(const ColorPalette& other);

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	void addNewColor(Color c);
	Color getColor(int index);
	Color* getColorRef(int index);
	Color getClosestColor(Color c);
	int getClosestColorIndex(Color c);
	int getSize();
	std::vector<Color> getPalette();
	std::vector<Color>* getPaletteRef();
	void clearPalette();

	static ColorPalette createColorPalette(int reds, int greens, int blues);
	static ColorPalette generateOptimalPalette(Color* colorArray, int size, int colors);

private:
	std::vector<Color> palette = std::vector<Color>();
};

