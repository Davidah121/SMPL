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
	const RootClass HiResImage::globalClass = RootClass("HiResImage", {"Object"});

	void quickCopy(Color4f* src, Color4f* dest, size_t size)
	{
		Color4f* startSrc = src;
		Color4f* startDest = dest;
		Color4f* endSrc = src + size;
		while(startSrc < endSrc)
		{
			*startDest = *startSrc;
			startSrc++;
			startDest++;
		}
	}

	HiResImage::HiResImage()
	{
		setClass(globalClass);
		width = 0;
		height = 0;
		pixels = nullptr;
	}

	HiResImage::HiResImage(int width, int height)
	{
		setClass(globalClass);
		this->width = width;
		this->height = height;
		pixels = new Color4f[width * height];
	}

	HiResImage::HiResImage(const HiResImage& other)
	{
		dispose();

		setClass(globalClass);
		this->width = other.width;
		this->height = other.height;

		if(other.pixels != nullptr)
		{
			pixels = new Color4f[width * height];
			quickCopy(other.pixels, pixels, width*height);
		}

		p = other.p;
	}

	void HiResImage::operator=(const HiResImage& other)
	{
		dispose();

		setClass(globalClass);
		this->width = other.width;
		this->height = other.height;
		
		if(other.pixels != nullptr)
		{
			pixels = new Color4f[width * height];
			quickCopy(other.pixels, pixels, width*height);
		}

		p = other.p;
	}

	HiResImage::~HiResImage()
	{
		dispose();
	}

	void HiResImage::dispose()
	{
		if(pixels!=nullptr)
			delete[] pixels;
		
		pixels = nullptr;
		p.clearPalette();
	}
	
	void HiResImage::setSamplingMethod(int m)
	{
		samplingMethod = m;
	}

	int HiResImage::getSamplingMethod()
	{
		return samplingMethod;
	}

	int HiResImage::getWidth()
	{
		return width;
	}

	int HiResImage::getHeight()
	{
		return height;
	}

	Color4f* HiResImage::getPixels()
	{
		return pixels;
	}

	Color4f HiResImage::getPixel(int x, int y, int edgeBehavior)
	{
		if(pixels == nullptr)
			return Color4f();

		if(edgeBehavior == NONE)
		{
			if (x < 0 || x >= width)
				return Color4f();
			if (y < 0 || y >= height)
				return Color4f();
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
		
		return Color4f();
	}

	Color4f HiResImage::getPixel(double x, double y, int edgeBehavior)
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
				return Color4f();
			if(nX2 < 0 || nX2 >= width)
				return Color4f();
			if(nY < 0 || nY >= height)
				return Color4f();
			if(nY2 < 0 || nY2 >= height)
				return Color4f();
			
			Color4f p1 = getPixel(nX, nY, edgeBehavior);
			Color4f p2 = getPixel(nX2, nY, edgeBehavior);
			Color4f p3 = getPixel(nX, nY2, edgeBehavior);
			Color4f p4 = getPixel(nX2, nY2, edgeBehavior);
			
			Vec4f blend1 = SimpleGraphics::lerp(Vec4f(p1.red, p1.green, p1.blue, p1.alpha), 
												Vec4f(p2.red, p2.green, p2.blue, p2.alpha), 
												xFrac);
			Vec4f blend2 = SimpleGraphics::lerp(Vec4f(p3.red, p3.green, p3.blue, p3.alpha), 
												Vec4f(p4.red, p4.green, p4.blue, p4.alpha), 
												xFrac);
			
			Vec4f finalC = SimpleGraphics::lerp(blend1, blend2, yFrac);
			return Color4f{finalC.x, finalC.y, finalC.z, finalC.w};
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
				Color4f c = getPixel(xPoints[xV], yPoints[yV], edgeBehavior);
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

			return Color4f{finalC.x, finalC.y, finalC.z, finalC.w};
		}
		
		return Color4f();
	}

	void HiResImage::setPixel(int x, int y, Color4f c)
	{
		if (x >= 0 && x < width)
		{
			if (y >= 0 && y < height)
			{
				pixels[y * width + x] = c;
			}
		}
	}

	void HiResImage::setAllPixels(Color4f c)
	{
		Color4f* start = pixels;
		Color4f* end = pixels + (width * height);
		while (start < end)
		{
			*start = c;
			start++;
		}
	}

	void HiResImage::copyImage(HiResImage* v)
	{
		dispose();
		if(v == nullptr)
			return;
		
		this->width = v->width;
		this->height = v->height;
		pixels = new Color4f[width * height];

		quickCopy(v->pixels, pixels, width*height);

		p = v->p;
	}

	void HiResImage::setPalette(ColorPalette p)
	{
		this->p = p;
	}

	ColorPalette& HiResImage::getPalette()
	{
		return p;
	}

	void HiResImage::enforcePalette()
	{
		if(p.getSize()>0)
		{
			Color4f* start = pixels;
			Color4f* end = pixels + (width * height);
			while (start < end)
			{
				Color4f oldColor4f = *start;
				Color testColor = Color{(unsigned char)((float)oldColor4f.red*255), (unsigned char)((float)oldColor4f.green*255), (unsigned char)((float)oldColor4f.blue*255), (unsigned char)((float)oldColor4f.alpha*255)};
				Color closestColor = p.getClosestColor(testColor);
				*start = Color4f{(float)closestColor.red / 255.0, (float)closestColor.green / 255.0, (float)closestColor.blue / 255.0, oldColor4f.alpha};
				start++;
			}
		}
	}

	HiResImage** HiResImage::loadImage(File filename, int* amountOfImages, std::vector<int>* extraData)
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
					*amountOfImages = 1;

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