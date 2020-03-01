#pragma once
#include "ColorPalette.h"
#include <string>

class Image : public Object
{
public:
	Image();
	Image(int width, int height);
	Image(const Image& other);
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

	void drawSprite(Image* img, int x, int y);
	void drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH);

	void drawText(std::string str, int x, int y);

private:
	int width = 0;
	int height = 0;
	ColorPalette p = ColorPalette();
	Color* pixels = nullptr;

	static Image** loadBMP(std::vector<unsigned char> fileData, int* amountOfImages);
	static Image** loadGIF(std::vector<unsigned char> fileData, int* amountOfImages);
	static Image** loadPNG(std::vector<unsigned char> fileData, int* amountOfImages);
	static Color PNGFiltering(std::vector<unsigned char> &byteData, int index, int filterType, Image* img, int x, int y, int bytesToRead);

	static Image** loadJPG(std::vector<unsigned char> fileData, int* amountOfImages);
	
};

