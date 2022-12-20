#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "SimpleGraphics.h"
#include <iostream>
#include "StringTools.h"

#define min(a,b) ((a<b) ? a:b)

namespace glib
{

	const Class Image::globalClass = Class("Image", {&Object::globalClass});

	Image::Image()
	{
		setClass(globalClass);
		width = 0;
		height = 0;
		pixels = nullptr;
	}

	Image::Image(int width, int height)
	{
		setClass(globalClass);
		this->width = width;
		this->height = height;
		pixels = new Color[width * height];
		memset(pixels, 0, width*height*sizeof(Color));
	}

	Image::Image(const Image& other)
	{
		dispose();

		setClass(globalClass);
		this->width = other.width;
		this->height = other.height;

		if(other.pixels != nullptr)
		{
			pixels = new Color[width * height];
			memcpy(pixels, other.pixels, width * height * sizeof(Color));
		}

		p = other.p;
	}

	void Image::operator=(const Image& other)
	{
		dispose();
		this->width = other.width;
		this->height = other.height;
		
		setClass(globalClass);
		if(other.pixels != nullptr)
		{
			pixels = new Color[width * height];
			memcpy(pixels, other.pixels, width * height * sizeof(Color));
		}

		p = other.p;
	}

	Image::~Image()
	{
		dispose();
	}

	void Image::dispose()
	{
		if(pixels!=nullptr)
			delete[] pixels;
		
		pixels = nullptr;
		p.~ColorPalette();
	}
	
	void Image::setSamplingMethod(int m)
	{
		samplingMethod = m;
	}

	int Image::getSamplingMethod()
	{
		return samplingMethod;
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

	Color Image::getPixel(int x, int y, int edgeBehavior)
	{
		if(pixels == nullptr)
			return Color();

		if(edgeBehavior == NONE)
		{
			if (x < 0 || x >= width)
				return Color();
			if (y < 0 || y >= height)
				return Color();
			return pixels[y * width + x];
		}
		else if(edgeBehavior == CLAMP)
		{
			int tX, tY;
			tX = MathExt::clamp(x, 0, width-1);
			tY = MathExt::clamp(y, 0, height-1);
			
			return pixels[tY * width + tX];
		}
		else if(edgeBehavior == REPEAT)
		{
			int tX, tY;
			tX = x % width;
			tY = y % height;
			
			return pixels[tY * width + tX];
		}
		
		return Color();
	}

	Color Image::getPixel(double x, double y, int edgeBehavior)
	{
		if(samplingMethod == NEAREST)
		{
			int nX = (int)round(x);
			int nY = (int)round(y);
			
			return getPixel(nX, nY, edgeBehavior);
		}
		else if(samplingMethod == BILINEAR)
		{
			//average 4 pixels
			int nX = (int)floor(x);
			int nY = (int)floor(y);
			int nX2 = (int)ceil(x);
			int nY2 = (int)ceil(y);
			double xFrac = MathExt::frac(x);
			double yFrac = MathExt::frac(y);
			
			if(nX < 0 || nX >= width)
				return Color();
			if(nX2 < 0 || nX2 >= width)
				return Color();
			if(nY < 0 || nY >= height)
				return Color();
			if(nY2 < 0 || nY2 >= height)
				return Color();
			
			Color p1 = getPixel(nX, nY, edgeBehavior);
			Color p2 = getPixel(nX2, nY, edgeBehavior);
			Color p3 = getPixel(nX, nY2, edgeBehavior);
			Color p4 = getPixel(nX2, nY2, edgeBehavior);
			
			Color blend1 = SimpleGraphics::lerp(p1, p2, xFrac);
			Color blend2 = SimpleGraphics::lerp(p3, p4, xFrac);
			
			return SimpleGraphics::lerp(blend1, blend2, yFrac);
		}
		else if(samplingMethod == BICUBIC)
		{
			//Same as a 3D bezier patch. (Bicubic Patch)
			//Need 16 points
			double yFrac = MathExt::frac(y);
			double xFrac = MathExt::frac(x);

			int yPoints[4];
			yPoints[1] = (int)MathExt::floor(y);
			yPoints[0] = yPoints[1]-1;
			yPoints[2] = (int)MathExt::ceil(y);
			yPoints[3] = yPoints[2]+1;
			int xPoints[4];
			xPoints[1] = (int)MathExt::floor(x);
			xPoints[0] = xPoints[1]-1;
			xPoints[2] = (int)MathExt::ceil(x);
			xPoints[3] = xPoints[2]+1;
			
			Vec4f arr[16];
			for(int j=0; j<16; j++)
			{
				int xV = j%4;
				int yV = j/4;
				Color c = getPixel(xPoints[xV], yPoints[yV], edgeBehavior);
				arr[j] = Vec4f(c.red, c.green, c.blue, c.alpha);
			}

			Vec4f polys[4];
			for(int j=0; j<4; j++)
			{
				Vec4f a, b, c, d;
				a = (arr[j*4 + 0]*-0.5) + (arr[j*4 + 1]*1.5) + (arr[j*4 + 2]*-1.5) + (arr[j*4 + 3]*0.5);
				b = (arr[j*4 + 0]) + (arr[j*4 + 1]*-2.5) + (arr[j*4 + 2]*2) + (arr[j*4 + 3]*-0.5);
				c = (arr[j*4 + 0]*-0.5) + (arr[j*4 + 2]*0.5);
				d = arr[j*4 + 1];

				polys[j] = ((a*xFrac + b)*xFrac + c)*xFrac + d;
			}
			
			Vec4f a, b, c, d;
			a = (polys[0]*-0.5) + (polys[1]*1.5) + (polys[2]*-1.5) + (polys[3]*0.5);
			b = (polys[0]) + (polys[1]*-2.5) + (polys[2]*2) + (polys[3]*-0.5);
			c = (polys[0]*-0.5) + (polys[2]*0.5);
			d = polys[1];

			Vec4f finalC = ((a*yFrac + b)*yFrac + c)*yFrac + d;

			return {
				(unsigned char)MathExt::clamp(finalC.x, 0.0, 255.0),
				(unsigned char)MathExt::clamp(finalC.y, 0.0, 255.0),
				(unsigned char)MathExt::clamp(finalC.z, 0.0, 255.0),
				(unsigned char)MathExt::clamp(finalC.w, 0.0, 255.0)
			};
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
			int leftOver = (width*height) - (size<<3);

			__m256i* endPixsSSE = startPixsSSE + size;
			__m256i value = _mm256_set1_epi32(m);

			while (startPixsSSE < endPixsSSE)
			{
				_mm256_storeu_si256(startPixsSSE, value);
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
			int leftOver = (width*height) - (size<<2);

			__m128i* endPixsSSE = startPixsSSE + size;
			__m128i value = _mm_set1_epi32(m);

			while (startPixsSSE < endPixsSSE)
			{
				_mm_storeu_si128(startPixsSSE, value);
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
		dispose();
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
		SimpleGraphics::clearImage(this);
	}

	void Image::drawRect(int x, int y, int x2, int y2, bool outline)
	{
		SimpleGraphics::drawRect(x, y, x2, y2, outline, this);
	}

	void Image::drawLine(int x, int y, int x2, int y2)
	{
		SimpleGraphics::drawLine(x, y, x2, y2, this);
	}

	void Image::drawCircle(int x, int y, int radius, bool outline)
	{
		SimpleGraphics::drawCircle(x, y, radius, outline, this);
	}

	void Image::drawPolygon(Vec2f* points, int size)
	{
		SimpleGraphics::drawPolygon(points, size, this);
	}

	void Image::drawImage(Image* img, int x, int y)
	{
		SimpleGraphics::drawImage(img, x, y, this);
	}

	void Image::drawSprite(Image* img, int x, int y)
	{
		SimpleGraphics::drawSprite(img, x, y, this);
	}

	void Image::drawSprite(Image* img, int x, int y, int x2, int y2)
	{
		SimpleGraphics::drawSprite(img, x, y, x2, y2, this);
	}

	void Image::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH)
	{
		SimpleGraphics::drawSpritePart(img, x, y, imgX, imgY, imgW, imgH, this);
	}

	void Image::drawText(std::string str, int x, int y)
	{
		SimpleGraphics::drawText(str, x, y, this);
	}

	void Image::drawText(std::wstring str, int x, int y)
	{
		SimpleGraphics::drawText(str, x, y, this);
	}

	void Image::drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool allowLineBreak)
	{
		SimpleGraphics::drawTextLimits(str, x, y, maxWidth, maxHeight, allowLineBreak, this);
	}

	void Image::drawTextLimits(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool allowLineBreak)
	{
		SimpleGraphics::drawTextLimits(str, x, y, maxWidth, maxHeight, allowLineBreak, this);
	}

	void Image::drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor)
	{
		SimpleGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor, this);
	}

	void Image::drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor)
	{
		SimpleGraphics::drawTextLimitsHighlighted(str, x, y, maxWidth, maxHeight, useLineBreak, highlightStart, highlightEnd, highlightColor, this);
	}

	void Image::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool outline)
	{
		SimpleGraphics::drawTriangle(x1, y1, x2, y2, x3, y3, outline, this);
	}

	void Image::drawTexturedTriangle(Vec4f p1, Vec4f p2, Vec4f p3, Image* texture)
	{
		SimpleGraphics::drawTexturedTriangle(p1, p2, p3, texture, this);
	}

	void Image::drawPixel(int x, int y, Color c)
	{
		SimpleGraphics::drawPixel(x, y, c, this);
	}

	void Image::drawPixel(double x, double y, Color c)
	{
		SimpleGraphics::drawPixel(x, y, c, this);
	}

	#pragma endregion

	Image** Image::loadImage(File filename, int* amountOfImages, std::vector<int>* extraData)
	{
		SimpleFile file(filename, SimpleFile::READ);

		if (file.isOpen())
		{
			std::vector<unsigned char> fileData = file.readFullFileAsBytes();
			file.close();

			std::string filenameExt = filename.getExtension();
			if (filenameExt == ".bmp")
			{
				if (amountOfImages != nullptr)
					* amountOfImages = 1;

				return loadBMP(fileData, amountOfImages);
			}
			else if (filenameExt == ".gif")
			{
				return loadGIF(fileData, amountOfImages, extraData);
			}
			else if (filenameExt == ".png")
			{
				return loadPNG(fileData, amountOfImages, extraData);
			}
			else if (filenameExt == ".jpg" || filenameExt == ".jpeg" || filenameExt == ".jfif")
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
} //NAMESPACE glib END