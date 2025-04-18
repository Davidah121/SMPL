#include "InternalGraphicsHeader.h"

namespace smpl
{
	//exception to the rule. No SIMD speedup available. (I'm not writting it for this)
	void SimpleGraphics::drawImage(HiResImage* img, int x, int y, HiResImage* surf)
	{
		if(surf == nullptr)
			return;
		if(img == nullptr)
			return;
		if(surf->getWidth()<=0 || surf->getHeight()<=0)
		{
			return;
		}
		if(img->getWidth()<=0 || img->getHeight()<=0)
		{
			return;
		}

		int tempWidth = surf->getWidth();
		int tempHeight = surf->getHeight();

		int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
		int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

		int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
		int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

		int minX = MathExt::clamp(x, minXBound, maxXBound);
		int minY = MathExt::clamp(y, minYBound, maxYBound);

		int startImgY = MathExt::max(minY - y, 0);
		int startImgX = MathExt::max(minX - x, 0);

		int imgDrawWidth = img->getWidth() - startImgX;
		int imgDrawHeight = img->getHeight() - startImgY;
		
		int maxX = MathExt::clamp(minX+(imgDrawWidth), minXBound, maxXBound);
		int maxY = MathExt::clamp(minY+(imgDrawHeight), minYBound, maxYBound);
		
		if(x+tempWidth < minX || y+tempHeight < minY)
			return; //Outside of the bounds that can be rendered
		if(y > maxY || x > maxX)
			return; //Outside of the bounds that can be rendered

		if(startImgX >= img->getWidth() || startImgY >= img->getHeight())
			return; //Not valid bounds for the image we are drawing.
		
		int approxArea = (maxX-minX)*(maxY-minY);
		Color4f* surfPixels = surf->getPixels();
		Color4f* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions

		int imgY = startImgY;
		LARGE_ENOUGH_CLAUSE(approxArea)
		#pragma omp parallel for
		for(int tY=minY; tY<maxY; tY++)
		{
			int imgX = startImgX; //distance from x to minX
			for(int tX=minX; tX<maxX; tX++)
			{
				drawPixel(tX, tY, drawImgPixels[imgX + imgY*img->getWidth()], surf);
				imgX++;
			}
			imgY++;
		}
		RESET_LARGE_ENOUGH_CLAUSE()
	}

	#if (OPTI == 0)

	void SimpleGraphics::drawImage(Image* img, int x, int y, Image* surf)
	{
		if(surf == nullptr)
			return;
		if(img == nullptr)
			return;
		if(surf->getWidth()<=0 || surf->getHeight()<=0)
		{
			return;
		}
		if(img->getWidth()<=0 || img->getHeight()<=0)
		{
			return;
		}

		int tempWidth = surf->getWidth();
		int tempHeight = surf->getHeight();

		int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
		int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

		int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
		int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

		int minX = MathExt::clamp(x, minXBound, maxXBound);
		int minY = MathExt::clamp(y, minYBound, maxYBound);

		int startImgY = MathExt::max(minY - y, 0);
		int startImgX = MathExt::max(minX - x, 0);

		int imgDrawWidth = img->getWidth() - startImgX;
		int imgDrawHeight = img->getHeight() - startImgY;
		
		int maxX = MathExt::clamp(minX+(imgDrawWidth), minXBound, maxXBound);
		int maxY = MathExt::clamp(minY+(imgDrawHeight), minYBound, maxYBound);
		
		if(x+tempWidth < minX || y+tempHeight < minY)
			return; //Outside of the bounds that can be rendered
		if(y > maxY || x > maxX)
			return; //Outside of the bounds that can be rendered

		if(startImgX >= img->getWidth() || startImgY >= img->getHeight())
			return; //Not valid bounds for the image we are drawing.
		
		int approxArea = (maxX-minX)*(maxY-minY);
		Color* surfPixels = surf->getPixels();
		Color* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions
		LARGE_ENOUGH_CLAUSE(approxArea)
		#pragma omp parallel for
		for(int tY=minY; tY<maxY; tY++)
		{
			int v = startImgY + (tY-minY);
			int u = startImgX; //distance from x to minX
			for(int tX=minX; tX<maxX; tX++)
			{
				drawPixel(tX, tY, drawImgPixels[u + v*img->getWidth()], surf);
				u++;
			}
		}
		RESET_LARGE_ENOUGH_CLAUSE()
	}

	void SimpleGraphics::drawSprite(Image* img, int x, int y, Image* surf)
	{
		if(surf == nullptr)
			return;
		if(img == nullptr)
			return;
		if(surf->getWidth()<=0 || surf->getHeight()<=0)
		{
			return;
		}
		if(img->getWidth()<=0 || img->getHeight()<=0)
		{
			return;
		}

		int tempWidth = surf->getWidth();
		int tempHeight = surf->getHeight();

		int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
		int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

		int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
		int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

		int minX = MathExt::clamp(x, minXBound, maxXBound);
		int minY = MathExt::clamp(y, minYBound, maxYBound);

		int startImgY = MathExt::max(minY - y, 0);
		int startImgX = MathExt::max(minX - x, 0);

		int imgDrawWidth = img->getWidth() - startImgX;
		int imgDrawHeight = img->getHeight() - startImgY;
		
		int maxX = MathExt::clamp(minX+(imgDrawWidth), minXBound, maxXBound);
		int maxY = MathExt::clamp(minY+(imgDrawHeight), minYBound, maxYBound);
		
		if(x+tempWidth < minX || y+tempHeight < minY)
			return; //Outside of the bounds that can be rendered
		if(y > maxY || x > maxX)
			return; //Outside of the bounds that can be rendered

		if(startImgX >= img->getWidth() || startImgY >= img->getHeight())
			return; //Not valid bounds for the image we are drawing.
		
		int approxArea = (maxX-minX)*(maxY-minY);

		Color* surfPixels = surf->getPixels();
		Color* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions
		Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);

		LARGE_ENOUGH_CLAUSE(approxArea)
		#pragma omp parallel for
		for(int tY=minY; tY<maxY; tY++)
		{
			int v = startImgY + (tY-minY);
			int u = startImgX; //distance from x to minX
			for(int tX=minX; tX<maxX; tX++)
			{
				Color c = drawImgPixels[u + v*img->getWidth()];
				c.red = (unsigned char)MathExt::clamp<float>(c.red * colorMult.x, 0.0, 255.0);
				c.green = (unsigned char)MathExt::clamp<float>(c.green * colorMult.y, 0.0, 255.0);
				c.blue = (unsigned char)MathExt::clamp<float>(c.blue * colorMult.z, 0.0, 255.0);
				c.alpha = (unsigned char)MathExt::clamp<float>(c.alpha * colorMult.w, 0.0, 255.0);
				drawPixel(tX, tY, c, surf);
				u++;
			}
		}
		RESET_LARGE_ENOUGH_CLAUSE()
	}

	void SimpleGraphics::drawSprite(Image* img, int x1, int y1, int width, int height, Image* surf)
	{
		if(surf == nullptr)
			return;
		if(img == nullptr)
			return;
		if(surf->getWidth()<=0 || surf->getHeight()<=0)
		{
			return;
		}
		if(img->getWidth()<=0 || img->getHeight()<=0)
		{
			return;
		}

		int x2 = x1+width;
		int y2 = y1+height;

		if(x1 < x2)
		{
			int temp = x2;
			x2 = x1;
			x1 = temp;
		}

		if(y1 < y2)
		{
			int temp = y2;
			y2 = y1;
			y1 = temp;
		}

		int tempWidth = surf->getWidth();
		int tempHeight = surf->getHeight();

		int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
		int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

		int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
		int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

		int minX = MathExt::clamp(x1, minXBound, maxXBound);
		int minY = MathExt::clamp(y1, minYBound, maxYBound);

		int maxX = MathExt::clamp(x2, minXBound, maxXBound);
		int maxY = MathExt::clamp(y2, minYBound, maxYBound);

		int startImgY = MathExt::max(minY - y1, 0);
		int startImgX = MathExt::max(minX - x1, 0);

		int imgDrawWidth = img->getWidth();
		int imgDrawHeight = img->getHeight();
		
		
		if(y1 >= maxY || x1 >= maxX || x2 <= minX || y2 <= minY)
			return; //Outside of the bounds that can be rendered
		
		int approxArea = (maxX-minX)*(maxY-minY);
		Color* surfPixels = surf->getPixels();
		Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);
		
		LARGE_ENOUGH_CLAUSE(approxArea)
		#pragma omp parallel for
		for(int tY=minY; tY<maxY; tY++)
		{
			int imgY = startImgY + (tY-minY);
			double v = imgDrawHeight * (imgY / (y2-y1));
			int imgX = startImgX;
			for(int tX=minX; tX<maxX; tX++)
			{
				double u = imgDrawWidth * (imgX / (x2-x1));
				Color c = img->getPixel(u, v, Image::CLAMP);
				c.red = (unsigned char)MathExt::clamp<float>(c.red * colorMult.x, 0.0, 255.0);
				c.green = (unsigned char)MathExt::clamp<float>(c.green * colorMult.y, 0.0, 255.0);
				c.blue = (unsigned char)MathExt::clamp<float>(c.blue * colorMult.z, 0.0, 255.0);
				c.alpha = (unsigned char)MathExt::clamp<float>(c.alpha * colorMult.w, 0.0, 255.0);
				drawPixel(tX, tY, c, surf);

				imgX++;
			}
		}
		RESET_LARGE_ENOUGH_CLAUSE()
	}

	void SimpleGraphics::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf)
	{
		if(surf == nullptr)
			return;
		if(img == nullptr)
			return;
		if(surf->getWidth()<=0 || surf->getHeight()<=0)
			return;
		if(img->getWidth()<=0 || img->getHeight()<=0)
			return;

		int tempWidth = surf->getWidth();
		int tempHeight = surf->getHeight();

		int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
		int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

		int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
		int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

		int minX = MathExt::clamp(x, minXBound, maxXBound);
		int minY = MathExt::clamp(y, minYBound, maxYBound);

		int startImgY = imgY + MathExt::max(minY - y, 0);
		int startImgX = imgX + MathExt::max(minX - x, 0);
		int nImgW = imgW - (startImgX - imgX);
		int nImgH = imgH - (startImgY - imgY);

		nImgW = MathExt::clamp(nImgW, 0, img->getWidth());
		nImgH = MathExt::clamp(nImgH, 0, img->getHeight());

		
		int maxX = MathExt::clamp(minX+(nImgW), minXBound, maxXBound);
		int maxY = MathExt::clamp(minY+(nImgH), minYBound, maxYBound);

		if(x+imgW < minX || y+imgH < minY)
			return; //Outside of the bounds that can be rendered
		if(y > maxY || x > maxX)
			return; //Outside of the bounds that can be rendered
		
		if(startImgX >= img->getWidth() || startImgY >= img->getHeight())
			return; //Not valid bounds for the image we are drawing.
		
		int approxArea = (maxX-minX)*(maxY-minY);
		Color* surfPixels = surf->getPixels();
		Color* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions
		Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);

		LARGE_ENOUGH_CLAUSE(approxArea)
		#pragma omp parallel for
		for(int tY=minY; tY<maxY; tY++)
		{
			int v = startImgY + (tY-minY);
			int u = startImgX; //distance from x to minX
			for(int tX=minX; tX<maxX; tX++)
			{
				Color c = drawImgPixels[u + v*img->getWidth()];
				c.red = (unsigned char)MathExt::clamp<float>(c.red * colorMult.x, 0.0, 255.0);
				c.green = (unsigned char)MathExt::clamp<float>(c.green * colorMult.y, 0.0, 255.0);
				c.blue = (unsigned char)MathExt::clamp<float>(c.blue * colorMult.z, 0.0, 255.0);
				c.alpha = (unsigned char)MathExt::clamp<float>(c.alpha * colorMult.w, 0.0, 255.0);
				drawPixel(tX, tY, c, surf);
				u++;
			}
		}
		RESET_LARGE_ENOUGH_CLAUSE()
	}
	#endif

} //NAMESPACE glib END