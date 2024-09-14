#include "InternalGraphicsHeader.h"

namespace smpl
{
	#if (OPTI > 0)

	void SimpleGraphics::drawImage(Image* img, int x, int y, Image* surf)
	{
		int currentComposite = compositeRule;
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

		int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
		int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

		int minX = MathExt::clamp(x, minXBound, maxXBound);
		int minY = MathExt::clamp(y, minYBound, maxYBound);

		int startImgY = MathExt::max(minY - y, 0);
		int startImgX = MathExt::max(minX - x, 0);

		int imgDrawWidth = img->getWidth() - startImgX;
		int imgDrawHeight = img->getHeight() - startImgY;
		
		int maxX = MathExt::clamp(minX+(imgDrawWidth-1), minXBound, maxXBound);
		int maxY = MathExt::clamp(minY+(imgDrawHeight-1), minYBound, maxYBound);
		
		if(x+tempWidth < minX || y+tempHeight < minY)
			return; //Outside of the bounds that can be rendered
		if(y > maxY || x > maxX)
			return; //Outside of the bounds that can be rendered

		if(startImgX >= img->getWidth() || startImgY >= img->getHeight())
			return; //Not valid bounds for the image we are drawing.
		
		Color* surfPixels = surf->getPixels();
		Color* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions
		
		if(currentComposite == NO_COMPOSITE)
		{
			#pragma omp parallel for
			for(int tY=minY; tY<=maxY; tY++)
			{
				int imgY = startImgY + (tY-minY);
				int imgX = startImgX; //distance from x to minX
				int tX = minX;
				int stopPoint = minX + GET_GRAPHICS_SIMD_BOUND((maxX-minX));
				while(tX < stopPoint)
				{
					GRAPHICS_SIMD_DATATYPE loadV = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&drawImgPixels[imgX + imgY*img->getWidth()]);
					GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&surfPixels[tX + tY*tempWidth], loadV);
					
					imgX += GRAPHICS_INC_AMOUNT;
					tX += GRAPHICS_INC_AMOUNT;
				}
				while(tX <= maxX)
				{
					surfPixels[tX + tY*tempWidth] = drawImgPixels[imgX + imgY*img->getWidth()];
					imgX++;
					tX++;
				}
			}
		}
		else
		{
			#pragma omp parallel for
			for(int tY=minY; tY<=maxY; tY++)
			{
				int imgY = startImgY + (tY-minY);
				int imgX = startImgX; //distance from x to minX
				int tX = minX;
				int stopPoint = minX + GET_GRAPHICS_SIMD_BOUND((maxX-minX));
				while(tX < stopPoint)
				{
					GRAPHICS_SIMD_DATATYPE src = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&drawImgPixels[imgX + imgY*img->getWidth()]);
					GRAPHICS_SIMD_DATATYPE dest = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&surfPixels[tX + tY*tempWidth]);
					GRAPHICS_SIMD_DATATYPE blendV = blend(src, dest);
					GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&surfPixels[tX + tY*tempWidth], blendV);

					imgX += GRAPHICS_INC_AMOUNT;
					tX += GRAPHICS_INC_AMOUNT;
				}
				while(tX <= maxX)
				{
					drawPixel(tX, tY, drawImgPixels[imgX + imgY*img->getWidth()], surf);
					imgX++;
					tX++;
				}
			}
		}
	}

	void SimpleGraphics::drawSprite(Image* img, int x, int y, Image* surf)
	{
		int currentComposite = compositeRule;
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

		int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
		int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

		int minX = MathExt::clamp(x, minXBound, maxXBound);
		int minY = MathExt::clamp(y, minYBound, maxYBound);

		int startImgY = MathExt::max(minY - y, 0);
		int startImgX = MathExt::max(minX - x, 0);

		int imgDrawWidth = img->getWidth() - startImgX;
		int imgDrawHeight = img->getHeight() - startImgY;
		
		int maxX = MathExt::clamp(minX+(imgDrawWidth-1), minXBound, maxXBound);
		int maxY = MathExt::clamp(minY+(imgDrawHeight-1), minYBound, maxYBound);
		
		if(x+tempWidth < minX || y+tempHeight < minY)
			return; //Outside of the bounds that can be rendered
		if(y > maxY || x > maxX)
			return; //Outside of the bounds that can be rendered

		if(startImgX >= img->getWidth() || startImgY >= img->getHeight())
			return; //Not valid bounds for the image we are drawing.
		
		Color* surfPixels = surf->getPixels();
		Color* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions

		GRAPHICS_SIMD_DATATYPE activeColorAsSIMD = COLOR_TO_SIMD(activeColor);
		if(currentComposite == NO_COMPOSITE)
		{
			#pragma omp parallel for
			for(int tY=minY; tY<=maxY; tY++)
			{
				int v = startImgY + (tY-minY);
				int u = startImgX; //distance from x to minX
				int tX = minX;
				int stopPoint = minX + GET_GRAPHICS_SIMD_BOUND((maxX-minX));
				while(tX < stopPoint)
				{
					GRAPHICS_SIMD_DATATYPE src = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&drawImgPixels[u + v*img->getWidth()]);
					GRAPHICS_SIMD_DATATYPE srcMultiplied = multColor(src, activeColorAsSIMD);
					GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&surfPixels[tX + tY*tempWidth], srcMultiplied);
					tX += GRAPHICS_INC_AMOUNT;
					u += GRAPHICS_INC_AMOUNT;
				}
				while(tX <= maxX)
				{
					Color src = drawImgPixels[u + v*img->getWidth()];
					surfPixels[tX + tY*tempWidth] = multColor(src, activeColor);
					tX++;
					u++;
				}
			}
		}
		else
		{
			#pragma omp parallel for
			for(int tY=minY; tY<=maxY; tY++)
			{
				int v = startImgY + (tY-minY);
				int u = startImgX; //distance from x to minX
				int tX = minX;
				int stopPoint = minX + GET_GRAPHICS_SIMD_BOUND((maxX-minX));
				while(tX < stopPoint)
				{
					GRAPHICS_SIMD_DATATYPE src = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&drawImgPixels[u + v*img->getWidth()]);
					GRAPHICS_SIMD_DATATYPE dest = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&surfPixels[tX + tY*tempWidth]);
					GRAPHICS_SIMD_DATATYPE srcMultiplied = multColor(src, activeColorAsSIMD);
					GRAPHICS_SIMD_DATATYPE blended = blend(srcMultiplied, dest);

					GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&surfPixels[tX + tY*tempWidth], blended);
					tX += GRAPHICS_INC_AMOUNT;
					u += GRAPHICS_INC_AMOUNT;
				}
				while(tX <= maxX)
				{
					Color src = drawImgPixels[u + v*img->getWidth()];
					Color dest = surfPixels[tX + tY*tempWidth];
					surfPixels[tX + tY*tempWidth] = blend(multColor(src, activeColor), dest);
					tX++;
					u++;
				}
			}
		}
	}

	//TODO: Rewrite. Needs SIMD. May need to scale the image first then draw that. Simplifies it but adds complexity and additional memory
	//Potential Idea: write an SIMD method to get pixels. Lerp needs to be SIMD. Grabbing a single pixel must be SIMD based.
	void SimpleGraphics::drawSprite(Image* img, int x1, int y1, int x2, int y2, Image* surf)
	{
		int currentComposite = compositeRule;
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

		int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
		int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

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
		
		Color* surfPixels = surf->getPixels();
		Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);

		if(currentComposite == NO_COMPOSITE)
		{
			#pragma omp parallel for
			for(int tY=minY; tY<=maxY; tY++)
			{
				int imgY = startImgY + (tY-minY);
				double v = imgDrawHeight * (imgY / (y2-y1));
				int imgX = startImgX;
				for(int tX=minX; tX<=maxX; tX++)
				{
					double u = imgDrawWidth * (imgX / (x2-x1));
					Color c = img->getPixel(u, v, Image::CLAMP);
					c.red = (unsigned char)MathExt::clamp<float>(c.red * colorMult.x, 0.0, 255.0);
					c.green = (unsigned char)MathExt::clamp<float>(c.green * colorMult.y, 0.0, 255.0);
					c.blue = (unsigned char)MathExt::clamp<float>(c.blue * colorMult.z, 0.0, 255.0);
					c.alpha = (unsigned char)MathExt::clamp<float>(c.alpha * colorMult.w, 0.0, 255.0);
					surfPixels[tX + tY*tempWidth] = c;

					imgX++;
				}
			}
		}
		else
		{
			#pragma omp parallel for
			for(int tY=minY; tY<=maxY; tY++)
			{
				int imgY = startImgY + (tY-minY);
				double v = imgDrawHeight * (imgY / (y2-y1));
				int imgX = startImgX;
				for(int tX=minX; tX<=maxX; tX++)
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
		}
	}

	void SimpleGraphics::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf)
	{
		int currentComposite = compositeRule;
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

		int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
		int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

		int minX = MathExt::clamp(x, minXBound, maxXBound);
		int minY = MathExt::clamp(y, minYBound, maxYBound);

		int startImgY = imgY + MathExt::max(minY - y, 0);
		int startImgX = imgX + MathExt::max(minX - x, 0);
		int nImgW = imgW - (startImgX - imgX);
		int nImgH = imgH - (startImgY - imgY);

		nImgW = MathExt::clamp(nImgW, 0, img->getWidth());
		nImgH = MathExt::clamp(nImgH, 0, img->getHeight());

		
		int maxX = MathExt::clamp(minX+(nImgW-1), minXBound, maxXBound);
		int maxY = MathExt::clamp(minY+(nImgH-1), minYBound, maxYBound);

		if(x+imgW < minX || y+imgH < minY)
			return; //Outside of the bounds that can be rendered
		if(y > maxY || x > maxX)
			return; //Outside of the bounds that can be rendered
		
		if(startImgX >= img->getWidth() || startImgY >= img->getHeight())
			return; //Not valid bounds for the image we are drawing.
		
		Color* surfPixels = surf->getPixels();
		Color* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions
		GRAPHICS_SIMD_DATATYPE activeColorAsSIMD = COLOR_TO_SIMD(activeColor);

		// Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);

		if(currentComposite == NO_COMPOSITE)
		{
			#pragma omp parallel for
			for(int tY=minY; tY<=maxY; tY++)
			{
				int v = startImgY + (tY-minY);
				int u = startImgX; //distance from x to minX
				int tX = minX;
				int stopPoint = minX + GET_GRAPHICS_SIMD_BOUND((maxX-minX));
				while(tX < stopPoint)
				{
					GRAPHICS_SIMD_DATATYPE srcC = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&drawImgPixels[u + v*img->getWidth()]);
					GRAPHICS_SIMD_DATATYPE srcMultiplied = multColor(srcC, activeColorAsSIMD);
					GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&surfPixels[tX + tY*tempWidth], srcMultiplied);
					tX += GRAPHICS_INC_AMOUNT;
					u += GRAPHICS_INC_AMOUNT;
				}

				while(tX <= maxX)
				{
					Color srcC = drawImgPixels[u + v*img->getWidth()];
					Color srcMultiplied = multColor(srcC, activeColor);
					surfPixels[tX + tY*tempWidth] = srcMultiplied;
					tX += 1;
					u += 1;
				}
			}
		}
		else
		{
			#pragma omp parallel for
			for(int tY=minY; tY<=maxY; tY++)
			{
				int v = startImgY + (tY-minY);
				int u = startImgX; //distance from x to minX
				int tX = minX;
				int stopPoint = minX + GET_GRAPHICS_SIMD_BOUND((maxX-minX));
				while(tX < stopPoint)
				{
					GRAPHICS_SIMD_DATATYPE srcC = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&drawImgPixels[u + v*img->getWidth()]);
					GRAPHICS_SIMD_DATATYPE destC = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&surfPixels[tX + tY*tempWidth]);
					GRAPHICS_SIMD_DATATYPE srcMultiplied = multColor(srcC, activeColorAsSIMD);
					GRAPHICS_SIMD_DATATYPE blendedColor = blend(srcMultiplied, destC);
					GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&surfPixels[tX + tY*tempWidth], blendedColor);
					tX += GRAPHICS_INC_AMOUNT;
					u += GRAPHICS_INC_AMOUNT;
				}

				while(tX <= maxX)
				{
					Color srcC = drawImgPixels[u + v*img->getWidth()];
					Color srcMultiplied = multColor(srcC, activeColor);
					Color blendedColor = blend(srcMultiplied, surfPixels[tX + tY*tempWidth]);
					surfPixels[tX + tY*tempWidth] = blendedColor;
					tX += 1;
					u += 1;
				}
			}
		}
	}
	#endif

} //NAMESPACE glib END