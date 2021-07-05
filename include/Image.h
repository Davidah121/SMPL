#pragma once
#include "ColorPalette.h"
#include <string>
#include "MathExt.h"
#include "Opti.h"

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
	static const Class myClass;

	int getWidth();
	int getHeight();
	Color* getPixels();

	Color getPixel(int x, int y, bool clamp = false);
	void setPixel(int x, int y, Color c);
	void setAllPixels(Color c);
	
	void copyImage(Image* v);

	void setPalette(ColorPalette p);
	ColorPalette& getPalette();

	void enforcePalette();

	static Image** loadImage(std::string filename, int* amountOfImages);
	static Image** loadImage(std::wstring filename, int* amountOfImages);

	//Wrapper around the graphics class for images
	void clearImage();
	void drawRect(int x, int y, int x2, int y2, bool outline);
	void drawLine(int x, int y, int x2, int y2);
	void drawCircle(int x, int y, int radius, bool outline);
	void drawPolygon(Vec2f* points, int size);

	void drawImage(Image* img, int x, int y);
	void drawSprite(Image* img, int x, int y);
	void drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH);

	void drawText(std::string str, int x, int y);
	void drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool allowLineBreak);
	
	void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool outline);
	void drawTexturedTriangle(Vec4f p1, Vec4f p2, Vec4f p3, Image* texture);

	void drawPixel(int x, int y, Color c);
	void drawPixel(double x, double y, Color c);

	void saveBMP(std::string filename, unsigned char alphaThreshold = 255, bool greyscale = false);
	void saveGIF(std::string filename, int paletteSize = 256, bool dither = false, bool saveAlpha = true, unsigned char alphaThreshold = 127, bool greyscale = false);
	void savePNG(std::string filename, bool saveAlpha = true, bool greyscale = false, bool strongCompression = false);
	void saveJPG(std::string filename);

	void saveBMP(std::wstring filename, unsigned char alphaThreshold = 255, bool greyscale = false);
	void saveGIF(std::wstring filename, int paletteSize = 256, bool dither = false, bool saveAlpha = true, unsigned char alphaThreshold = 127, bool greyscale = false);
	void savePNG(std::wstring filename, bool saveAlpha = true, bool greyscale = false, bool strongCompression = false);
	void saveJPG(std::wstring filename);
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

