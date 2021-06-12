#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "Graphics.h"
#include <iostream>
#include "StringTools.h"

#define min(a,b) ((a<b) ? a:b)

const Class Image::myClass = Class("Image", {&Object::myClass});
const Class* Image::getClass()
{
	return &Image::myClass;
}

Image::Image()
{
	width = 0;
	height = 0;
	pixels = nullptr;
}

Image::Image(int width, int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width * height];
	memset(pixels, 0, width*height*sizeof(Color));
}

Image::Image(const Image& other)
{
	this->~Image();

	this->width = other.width;
	this->height = other.height;
	pixels = new Color[width * height];

	memcpy(pixels, other.pixels, width * height * sizeof(Color));

	p = other.p;
}

void Image::operator=(const Image& other)
{
	this->~Image();
	this->width = other.width;
	this->height = other.height;
	pixels = new Color[width * height];

	memcpy(pixels, other.pixels, width * height * sizeof(Color));

	p = other.p;
}

Image::~Image()
{
	if(pixels!=nullptr)
		delete[] pixels;

	p.~ColorPalette();
}

int Image::getWidth()
{
	return width;
}

int Image::getHeight()
{
	return height;
}

Color* Image::getPixels()
{
	return pixels;
}

Color Image::getPixel(int x, int y, bool clamp)
{
	if(!clamp)
	{
		if (x >= 0 && x < width)
		{
			if (y >= 0 && y < height)
			{
				return pixels[y * width + x];
			}
		}
	}
	else
	{
		int tX, tY;
		tX = MathExt::clamp(x, 0, width-1);
		tY = MathExt::clamp(y, 0, height-1);
		
		return pixels[tY * width + tX];
	}
	
	return Color();
}

void Image::setPixel(int x, int y, Color c)
{
	if (x >= 0 && x < width)
	{
		if (y >= 0 && y < height)
		{
			pixels[y * width + x] = c;
		}
	}
}

void Image::setAllPixels(Color c)
{
	#if (OPTI >= 2)
		int m = *((int*)&c);
		__m256i* startPixsSSE = (__m256i*)pixels;

		int size = (width*height) >> 3;
		int leftOver = (width*height) % 8;

		__m256i* endPixsSSE = startPixsSSE + size;
		__m256i value = _mm256_set1_epi32(m);

		while (startPixsSSE < endPixsSSE)
		{
			_mm256_store_si256(startPixsSSE, value);
			startPixsSSE++;
		}
		
		Color* leftOverPixsEnd = pixels + (width*height);
		Color* leftOverPixs = leftOverPixsEnd - leftOver;
		
		while(leftOverPixs < leftOverPixsEnd)
		{
			*leftOverPixs = c;
			leftOverPixs++;
		}
		
	#elif (OPTI >= 1)

		int m = *((int*)&c);
		__m128i* startPixsSSE = (__m128i*)pixels;

		int size = (width*height) >> 2;
		int leftOver = (width*height) % 4;

		__m128i* endPixsSSE = startPixsSSE + size;
		__m128i value = _mm_set1_epi32(m);

		while (startPixsSSE < endPixsSSE)
		{
			_mm_store_si128(startPixsSSE, value);
			startPixsSSE++;
		}
		
		Color* leftOverPixsEnd = pixels + (width*height);
		Color* leftOverPixs = leftOverPixsEnd - leftOver;
		
		while(leftOverPixs < leftOverPixsEnd)
		{
			*leftOverPixs = c;
			leftOverPixs++;
		}
		
	#else

		Color* start = pixels;
		Color* end = pixels + (width * height);
		while (start < end)
		{
			*start = c;
			start++;
		}

	#endif
}

void Image::copyImage(Image* v)
{
	this->~Image();
	this->width = v->width;
	this->height = v->height;
	pixels = new Color[width * height];

	memcpy(pixels, v->pixels, width * height * sizeof(Color));

	p = v->p;
}

void Image::setPalette(ColorPalette p)
{
	this->p = p;
}

ColorPalette& Image::getPalette()
{
	return p;
}

void Image::enforcePalette()
{
	if(p.getSize()>0)
	{
		Color* start = pixels;
		Color* end = pixels + (width * height);
		while (start < end)
		{
			Color oldColor = *start;
			*start = p.getClosestColor(*start);

			(*start).alpha = oldColor.alpha;
			start++;
		}
	}
}

#pragma region GRAPHICS_WRAPPER

void Image::clearImage()
{
	Graphics::clearImage(this);
}

void Image::drawRect(int x, int y, int x2, int y2, bool outline)
{
	Graphics::drawRect(x, y, x2, y2, outline, this);
}

void Image::drawLine(int x, int y, int x2, int y2)
{
	Graphics::drawLine(x, y, x2, y2, this);
}

void Image::drawCircle(int x, int y, int radius, bool outline)
{
	Graphics::drawCircle(x, y, radius, outline, this);
}

void Image::drawPolygon(Vec2f* points, int size)
{
	Graphics::drawPolygon(points, size, this);
}

void Image::drawImage(Image* img, int x, int y)
{
	Graphics::drawImage(img, x, y, this);
}

void Image::drawSprite(Image* img, int x, int y)
{
	Graphics::drawSprite(img, x, y, this);
}

void Image::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH)
{
	Graphics::drawSpritePart(img, x, y, imgX, imgY, imgW, imgH, this);
}

void Image::drawText(std::string str, int x, int y)
{
	Graphics::drawText(str, x, y, this);
}

void Image::drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool allowLineBreak)
{
	Graphics::drawTextLimits(str, x, y, maxWidth, maxHeight, allowLineBreak, this);
}

void Image::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool outline)
{
	Graphics::drawTriangle(x1, y1, x2, y2, x3, y3, outline, this);
}

void Image::drawTexturedTriangle(Vec4f p1, Vec4f p2, Vec4f p3, Image* texture)
{
	Graphics::drawTexturedTriangle(p1, p2, p3, texture, this);
}

void Image::drawPixel(int x, int y, Color c)
{
	Graphics::drawPixel(x, y, c, this);
}

void Image::drawPixel(double x, double y, Color c)
{
	Graphics::drawPixel(x, y, c, this);
}

#pragma endregion

Image** Image::loadImage(std::string filename, int* amountOfImages)
{
	SimpleFile file(filename, SimpleFile::READ);

	if (file.isOpen())
	{
		std::vector<unsigned char> fileData = file.readFullFileAsBytes();

		file.close();

		std::string fileType = filename.substr(filename.size() - 3, 3);
		std::string fileType2 = filename.substr(filename.size() - 4, 4);

		if (fileType == "bmp")
		{
			if (amountOfImages != nullptr)
				*amountOfImages = 1;

			return loadBMP(fileData, amountOfImages);
		}
		else if (fileType == "gif")
		{
			return loadGIF(fileData, amountOfImages);
		}
		else if (fileType == "png")
		{
			return loadPNG(fileData, amountOfImages);
		}
		else if (fileType == "jpg" || fileType2 == "jpeg" || fileType2 == "jfif")
		{
			return loadJPG(fileData, amountOfImages);
		}

		return nullptr;
	}
	else
	{
		if(amountOfImages!=nullptr)
			*amountOfImages = 0;

		return nullptr;
	}
}

Image** Image::loadImage(std::wstring filename, int* amountOfImages)
{
	SimpleFile file(filename, SimpleFile::READ);

	if (file.isOpen())
	{
		std::vector<unsigned char> fileData = file.readFullFileAsBytes();

		file.close();

		std::wstring fileType = filename.substr(filename.size() - 3, 3);
		std::wstring fileType2 = filename.substr(filename.size() - 4, 4);

		if (fileType == L"bmp")
		{
			if (amountOfImages != nullptr)
				* amountOfImages = 1;

			return loadBMP(fileData, amountOfImages);
		}
		else if (fileType == L"gif")
		{
			return loadGIF(fileData, amountOfImages);
		}
		else if (fileType == L"png")
		{
			return loadPNG(fileData, amountOfImages);
		}
		else if (fileType == L"jpg" || fileType2 == L"jpeg")
		{
			return loadJPG(fileData, amountOfImages);
		}
	}
	else
	{
		if (amountOfImages != nullptr)
			* amountOfImages = 0;

		return nullptr;
	}

	return nullptr;
}

