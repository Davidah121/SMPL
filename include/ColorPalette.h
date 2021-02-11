#pragma once
#include <vector>
#include "Object.h"
#include "KDTree.h"

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
	void operator=(const ColorPalette& other);

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	void addNewColor(Color c);
	Color getColor(int index);
	Color* getColorRef(int index);
	int getColorIndex(Color c);
	
	Color getClosestColor(Color c);
	int getClosestColorIndex(Color c);
	int getSize();
	std::vector<Color>& getPalette();
	void setPalette(std::vector<Color> p);

	void clearPalette();
	void setPaletteMode(bool uniqueOnly);

	static ColorPalette createColorPalette(int reds, int greens, int blues);

	static const unsigned char MEAN_CUT = 0;
	static const unsigned char MEDIAN_CUT = 1;
	static const unsigned char K_MEANS = 2;

	static ColorPalette generateOptimalPalette(Color* colorArray, int size, int colors, unsigned char type, bool convertToLab = false, bool uniqueOnly = false);

	void reBalance();

private:
	void copy(const ColorPalette& other);

	static std::vector<Color> meanCut(std::vector<Color> colorArray, int colors);
	static std::vector<Color> medianCut(std::vector<Color> colorArray, int colors);
	static std::vector<Color> kMeans(std::vector<Color> colorArray, int colors, int maxIterations);
	
	std::vector<Color> paletteArr = std::vector<Color>();
	KDTree<unsigned char>* paletteTree = new KDTree<unsigned char>(3);
	bool uniquePalette = true;
};

