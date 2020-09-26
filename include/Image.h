#pragma once
#include "ColorPalette.h"
#include <string>
#include "Vec2f.h"

class Image : public Object
{
public:
	Image();
	Image(int width, int height);
	Image(const Image& other);
	void operator=(const Image& other);
	~Image();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	int getWidth();
	int getHeight();
	Color* getPixels();

	Color getPixel(int x, int y);
	void setPixel(int x, int y, Color c);
	void setAllPixels(Color c);
	
	void copyImage(Image* v);

	void setPalette(ColorPalette p);
	ColorPalette getPalette();

	static Image** loadImage(std::string filename, int* amountOfImages);
	static Image** loadImage(std::wstring filename, int* amountOfImages);

	//static Image* orderedDither(Image* img, int size);
	//static Image* floydSteinburgDither(Image* img);
	//static Image* reduceColorPalette(Image* img, Palette palatte);


	//Wrapper around the graphics class for images
	void clearImage();
	void drawRect(int x, int y, int x2, int y2, bool outline);
	void drawLine(int x, int y, int x2, int y2);
	void drawCircle(int x, int y, int radius, bool outline);
	void drawPolygon(Vec2f* points, int size);

	void drawSprite(Image* img, int x, int y);
	void drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH);

	void drawText(std::string str, int x, int y);
	void drawPixel(int x, int y, Color c);

	void saveBMP(std::string filename);
	void saveGIF(std::string filename);
	void savePNG(std::string filename);
	void saveJPG(std::string filename);
private:
	int width = 0;
	int height = 0;
	ColorPalette p = ColorPalette();
	Color* pixels = nullptr;

	static Image** loadBMP(std::vector<unsigned char> fileData, int* amountOfImages);
	static Image** loadGIF(std::vector<unsigned char> fileData, int* amountOfImages);
	static Image** loadPNG(std::vector<unsigned char> fileData, int* amountOfImages);
	static Image** loadJPG(std::vector<unsigned char> fileData, int* amountOfImages);
};

