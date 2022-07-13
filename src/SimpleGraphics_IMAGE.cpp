#include "InternalGraphicsHeader.h"

namespace glib
{
	void SimpleGraphics::drawImage(Image* img, int x, int y, Image* surf)
	{
		int currentComposite = compositeRule;
		if(surf == nullptr)
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

		Color* surfPixels = surf->getPixels();
		Color* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions

		if(currentComposite == NO_COMPOSITE)
		{
			int imgY = startImgY;
			for(int tY=minY; tY<=maxY; tY++)
			{
				int imgX = startImgX; //distance from x to minX
				for(int tX=minX; tX<=maxX; tX++)
				{
					surfPixels[tX + tY*tempWidth] = drawImgPixels[imgX + imgY*img->getWidth()];
					imgX++;
				}
				imgY++;
			}
		}
		else
		{
			int imgY = startImgY;
			for(int tY=minY; tY<=maxY; tY++)
			{
				int imgX = startImgX; //distance from x to minX
				for(int tX=minX; tX<=maxX; tX++)
				{
					drawPixel(tX, tY, drawImgPixels[imgX + imgY*img->getWidth()], surf);
					imgX++;
				}
				imgY++;
			}
		}
	}

	void SimpleGraphics::drawSprite(Image* img, int x, int y, Image* surf)
	{
		int currentComposite = compositeRule;
		if(surf == nullptr)
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

		Color* surfPixels = surf->getPixels();
		Color* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions
		Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);

		if(currentComposite == NO_COMPOSITE)
		{
			int v = startImgY;
			for(int tY=minY; tY<=maxY; tY++)
			{
				int u = startImgX; //distance from x to minX
				for(int tX=minX; tX<=maxX; tX++)
				{
					Color c = drawImgPixels[u + v*img->getWidth()];
					c.red = (unsigned char)MathExt::clamp(c.red * colorMult.x, 0.0, 255.0);
					c.green = (unsigned char)MathExt::clamp(c.green * colorMult.y, 0.0, 255.0);
					c.blue = (unsigned char)MathExt::clamp(c.blue * colorMult.z, 0.0, 255.0);
					c.alpha = (unsigned char)MathExt::clamp(c.alpha * colorMult.w, 0.0, 255.0);
					surfPixels[tX + tY*tempWidth] = c;
					u++;
				}
				v++;
			}
		}
		else
		{
			int imgY = startImgY;
			for(int tY=minY; tY<=maxY; tY++)
			{
				int imgX = startImgX; //distance from x to minX
				for(int tX=minX; tX<=maxX; tX++)
				{
					Color c = drawImgPixels[imgX + imgY*img->getWidth()];
					c.red = (unsigned char)MathExt::clamp(c.red * colorMult.x, 0.0, 255.0);
					c.green = (unsigned char)MathExt::clamp(c.green * colorMult.y, 0.0, 255.0);
					c.blue = (unsigned char)MathExt::clamp(c.blue * colorMult.z, 0.0, 255.0);
					c.alpha = (unsigned char)MathExt::clamp(c.alpha * colorMult.w, 0.0, 255.0);
					drawPixel(tX, tY, c, surf);
					imgX++;
				}
				imgY++;
			}
		}
	}

	void SimpleGraphics::drawSprite(Image* img, int x1, int y1, int x2, int y2, Image* surf)
	{
		int currentComposite = compositeRule;
		if(surf == nullptr)
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
			int imgY = startImgY;
			for(int tY=minY; tY<=maxY; tY++)
			{
				double v = imgDrawHeight * (imgY / (y2-y1));
				int imgX = startImgX;
				for(int tX=minX; tX<=maxX; tX++)
				{
					double u = imgDrawWidth * (imgX / (x2-x1));
					Color c = img->getPixel(u, v, Image::CLAMP);
					c.red = (unsigned char)MathExt::clamp(c.red * colorMult.x, 0.0, 255.0);
					c.green = (unsigned char)MathExt::clamp(c.green * colorMult.y, 0.0, 255.0);
					c.blue = (unsigned char)MathExt::clamp(c.blue * colorMult.z, 0.0, 255.0);
					c.alpha = (unsigned char)MathExt::clamp(c.alpha * colorMult.w, 0.0, 255.0);
					surfPixels[tX + tY*tempWidth] = c;

					imgX++;
				}
				imgY++;
			}
		}
		else
		{
			int imgY = startImgY;
			for(int tY=minY; tY<=maxY; tY++)
			{
				double v = imgDrawHeight * (imgY / (y2-y1));
				int imgX = startImgX;
				for(int tX=minX; tX<=maxX; tX++)
				{
					double u = imgDrawWidth * (imgX / (x2-x1));
					Color c = img->getPixel(u, v, Image::CLAMP);
					c.red = (unsigned char)MathExt::clamp(c.red * colorMult.x, 0.0, 255.0);
					c.green = (unsigned char)MathExt::clamp(c.green * colorMult.y, 0.0, 255.0);
					c.blue = (unsigned char)MathExt::clamp(c.blue * colorMult.z, 0.0, 255.0);
					c.alpha = (unsigned char)MathExt::clamp(c.alpha * colorMult.w, 0.0, 255.0);
					drawPixel(tX, tY, c, surf);

					imgX++;
				}
				imgY++;
			}
		}
	}

	void SimpleGraphics::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf)
	{
		int currentComposite = compositeRule;
		if(surf == nullptr)
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
		
		
		int maxX = MathExt::clamp(minX+(nImgW-1), minXBound, maxXBound);
		int maxY = MathExt::clamp(minY+(nImgH-1), minYBound, maxYBound);

		if(x+imgW < minX || y+imgH < minY)
			return; //Outside of the bounds that can be rendered
		if(y > maxY || x > maxX)
			return; //Outside of the bounds that can be rendered
		
		Color* surfPixels = surf->getPixels();
		Color* drawImgPixels = img->getPixels(); //Not using any pixel filtering and no edge conditions
		Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);

		if(currentComposite == NO_COMPOSITE)
		{
			int v = startImgY;
			for(int tY=minY; tY<=maxY; tY++)
			{
				int u = startImgX; //distance from x to minX
				for(int tX=minX; tX<=maxX; tX++)
				{
					Color c = drawImgPixels[u + v*img->getWidth()];
					c.red = (unsigned char)MathExt::clamp(c.red * colorMult.x, 0.0, 255.0);
					c.green = (unsigned char)MathExt::clamp(c.green * colorMult.y, 0.0, 255.0);
					c.blue = (unsigned char)MathExt::clamp(c.blue * colorMult.z, 0.0, 255.0);
					c.alpha = (unsigned char)MathExt::clamp(c.alpha * colorMult.w, 0.0, 255.0);
					surfPixels[tX + tY*tempWidth] = c;
					u++;
				}
				v++;
			}
		}
		else
		{
			int v = startImgY;
			for(int tY=minY; tY<=maxY; tY++)
			{
				int u = startImgX; //distance from x to minX
				for(int tX=minX; tX<=maxX; tX++)
				{
					Color c = drawImgPixels[u + v*img->getWidth()];
					c.red = (unsigned char)MathExt::clamp(c.red * colorMult.x, 0.0, 255.0);
					c.green = (unsigned char)MathExt::clamp(c.green * colorMult.y, 0.0, 255.0);
					c.blue = (unsigned char)MathExt::clamp(c.blue * colorMult.z, 0.0, 255.0);
					c.alpha = (unsigned char)MathExt::clamp(c.alpha * colorMult.w, 0.0, 255.0);
					drawPixel(tX, tY, c, surf);
					u++;
				}
				v++;
			}
		}
	}
	//Works properly now
	void SimpleGraphics::drawImage_OLD(Image* img, int x, int y, Image* surf)
	{
		int currentComposite = compositeRule;

		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr && img!=nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			if(img->getWidth()<=0 || img->getHeight()<=0)
			{
				return;
			}
			int tempWidth = otherImg->getWidth();
			int tempHeight = otherImg->getHeight();

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+(img->getWidth()-1), minXBound, maxXBound);

			int minY = MathExt::clamp(y, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+(img->getHeight()-1), minYBound, maxYBound);

			Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
			Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

			int drawImgWidth = (maxX - minX) + 1;
			int drawImgAddX = img->getWidth() - drawImgWidth;
			int otherImgAddX = tempWidth - drawImgWidth;

			int imgStartX = MathExt::max(minXBound - x, 0);
			int imgStartY = MathExt::max(minYBound - y, 0);

			Color* drawImgStart = img->getPixels() + (int)(imgStartX + img->getWidth()*imgStartY);

			if(imgStartX >= img->getWidth())
				return;
			if(imgStartY >= img->getHeight())
				return;

			int tX = 0;

			#if(OPTI>=2)

				int avxWidth = (1+maxX-minX) >> 3;
				int remainder = (1+maxX-minX) - (avxWidth<<3);

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							_mm256_storeu_si256(avxPoint, avxColor);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							*startPoint = *drawImgStart;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							__m256i currentColor = _mm256_loadu_si256(avxPoint);
							__m256i blendC = blend(avxColor, currentColor);
							_mm256_storeu_si256(avxPoint, blendC);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							*startPoint = blend(*drawImgStart, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#elif(OPTI>=1)

				int sseWidth = (1+maxX-minX) >> 2;
				int remainder = (1+maxX-minX) - (sseWidth<<2);

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							_mm_storeu_si128(ssePoint, sseColor);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							*startPoint = *drawImgStart;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							__m128i currentColor = _mm_loadu_si128(ssePoint);
							__m128i blendC = blend(sseColor, currentColor);
							_mm_storeu_si128(ssePoint, blendC);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							*startPoint = blend(*drawImgStart, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#else

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							*startPoint = *drawImgStart;

							startPoint++;
							drawImgStart++;
						}
					
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							*startPoint = blend(*drawImgStart, *startPoint);

							startPoint++;
							drawImgStart++;
						}
					
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}

			#endif
		}
	}

	void SimpleGraphics::drawSprite_OLD(Image* img, int x, int y, Image* surf)
	{
		int currentComposite = compositeRule;
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr && img!=nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			if(img->getWidth()<=0 || img->getHeight()<=0)
			{
				return;
			}
			int tempWidth = otherImg->getWidth();
			int tempHeight = otherImg->getHeight();

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+(img->getWidth()-1), minXBound, maxXBound);

			int minY = MathExt::clamp(y, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+(img->getHeight()-1), minYBound, maxYBound);

			Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
			Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);
			
			int drawImgWidth = (maxX - minX) + 1;
			int drawImgAddX = img->getWidth() - drawImgWidth;
			int otherImgAddX = tempWidth - drawImgWidth;

			int imgStartX = MathExt::max(minXBound - x, 0);
			int imgStartY = MathExt::max(minYBound - y, 0);

			Color* drawImgStart = img->getPixels() + (int)(imgStartX + img->getWidth()*imgStartY);

			if(imgStartX >= img->getWidth())
				return;
			if(imgStartY >= img->getHeight())
				return;

			int tX = 0;
			Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);

			#if(OPTI>=2)
				
				int avxWidth = (1+maxX-minX) >> 3;
				int remainder = (1+maxX-minX) - (avxWidth<<3);

				__m256 colorMultRed = _mm256_set1_ps( ((float)SimpleGraphics::activeColor.red) / 255.0 );
				__m256 colorMultGreen = _mm256_set1_ps( ((float)SimpleGraphics::activeColor.green) / 255.0 );
				__m256 colorMultBlue = _mm256_set1_ps( ((float)SimpleGraphics::activeColor.blue) / 255.0 );
				__m256 colorMultAlpha = _mm256_set1_ps( ((float)SimpleGraphics::activeColor.alpha) / 255.0 );

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							
							Color* colors = (Color*)&avxColor;

							__m256 reds = _mm256_set_ps(
								(float)colors[7].red, (float)colors[6].red, (float)colors[5].red, (float)colors[4].red,
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m256 greens = _mm256_set_ps(
								(float)colors[7].green, (float)colors[6].green, (float)colors[5].green, (float)colors[4].green,
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m256 blues = _mm256_set_ps(
								(float)colors[7].blue, (float)colors[6].blue, (float)colors[5].blue, (float)colors[4].blue,
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m256 alphas = _mm256_set_ps(
								(float)colors[7].alpha, (float)colors[6].alpha, (float)colors[5].alpha, (float)colors[4].alpha,
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm256_mul_ps(reds, colorMultRed);
							greens = _mm256_mul_ps(greens, colorMultGreen);
							blues = _mm256_mul_ps(blues, colorMultBlue);
							alphas = _mm256_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m256i finalColor = _mm256_set_epi8(
								(unsigned char)aFloats[7], (unsigned char)bFloats[7], (unsigned char)gFloats[7], (unsigned char)rFloats[7],
								(unsigned char)aFloats[6], (unsigned char)bFloats[6], (unsigned char)gFloats[6], (unsigned char)rFloats[6],
								(unsigned char)aFloats[5], (unsigned char)bFloats[5], (unsigned char)gFloats[5], (unsigned char)rFloats[5],
								(unsigned char)aFloats[4], (unsigned char)bFloats[4], (unsigned char)gFloats[4], (unsigned char)rFloats[4],
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);


							_mm256_storeu_si256(avxPoint, finalColor);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							__m256i currentColor = _mm256_loadu_si256(avxPoint);
							
							Color* colors = (Color*)&avxColor;

							__m256 reds = _mm256_set_ps(
								(float)colors[7].red, (float)colors[6].red, (float)colors[5].red, (float)colors[4].red,
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m256 greens = _mm256_set_ps(
								(float)colors[7].green, (float)colors[6].green, (float)colors[5].green, (float)colors[4].green,
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m256 blues = _mm256_set_ps(
								(float)colors[7].blue, (float)colors[6].blue, (float)colors[5].blue, (float)colors[4].blue,
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m256 alphas = _mm256_set_ps(
								(float)colors[7].alpha, (float)colors[6].alpha, (float)colors[5].alpha, (float)colors[4].alpha,
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm256_mul_ps(reds, colorMultRed);
							greens = _mm256_mul_ps(greens, colorMultGreen);
							blues = _mm256_mul_ps(blues, colorMultBlue);
							alphas = _mm256_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m256i finalColor = _mm256_set_epi8(
								(unsigned char)aFloats[7], (unsigned char)bFloats[7], (unsigned char)gFloats[7], (unsigned char)rFloats[7],
								(unsigned char)aFloats[6], (unsigned char)bFloats[6], (unsigned char)gFloats[6], (unsigned char)rFloats[6],
								(unsigned char)aFloats[5], (unsigned char)bFloats[5], (unsigned char)gFloats[5], (unsigned char)rFloats[5],
								(unsigned char)aFloats[4], (unsigned char)bFloats[4], (unsigned char)gFloats[4], (unsigned char)rFloats[4],
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);

							__m256i blendC = blend(finalColor, currentColor);
							_mm256_storeu_si256(avxPoint, blendC);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#elif(OPTI>=1)

				int sseWidth = (1+maxX-minX) >> 2;
				int remainder = (1+maxX-minX) - (sseWidth<<2);

				__m128 colorMultRed = _mm_set1_ps( ((float)SimpleGraphics::activeColor.red) / 255.0 );
				__m128 colorMultGreen = _mm_set1_ps( ((float)SimpleGraphics::activeColor.green) / 255.0 );
				__m128 colorMultBlue = _mm_set1_ps( ((float)SimpleGraphics::activeColor.blue) / 255.0 );
				__m128 colorMultAlpha = _mm_set1_ps( ((float)SimpleGraphics::activeColor.alpha) / 255.0 );

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							
							Color* colors = (Color*)&sseColor;

							__m128 reds = _mm_set_ps(
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m128 greens = _mm_set_ps(
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m128 blues = _mm_set_ps(
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m128 alphas = _mm_set_ps(
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm_mul_ps(reds, colorMultRed);
							greens = _mm_mul_ps(greens, colorMultGreen);
							blues = _mm_mul_ps(blues, colorMultBlue);
							alphas = _mm_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m128i finalColor = _mm_set_epi8(
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);


							_mm_storeu_si128(ssePoint, finalColor);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							__m128i currentColor = _mm_loadu_si128(ssePoint);
							
							Color* colors = (Color*)&sseColor;

							__m128 reds = _mm_set_ps(
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m128 greens = _mm_set_ps(
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m128 blues = _mm_set_ps(
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m128 alphas = _mm_set_ps(
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm_mul_ps(reds, colorMultRed);
							greens = _mm_mul_ps(greens, colorMultGreen);
							blues = _mm_mul_ps(blues, colorMultBlue);
							alphas = _mm_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m128i finalColor = _mm_set_epi8(
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);

							__m128i blendC = blend(finalColor, currentColor);
							_mm_storeu_si128(ssePoint, blendC);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#else
				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;

							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);

							startPoint++;
							drawImgStart++;
						}
					
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}

			#endif
		}
	}
	
	void SimpleGraphics::drawSprite_OLD(Image* img, int x1, int y1, int x2, int y2, Image* surf)
	{
		if(img == nullptr || surf == nullptr)
		{
			return;
		}
	
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

		int minX = MathExt::clamp(x1, minXBound, maxXBound);
		int maxX = MathExt::clamp(x2, minXBound, maxXBound);

		int minY = MathExt::clamp(y1, minYBound, maxYBound);
		int maxY = MathExt::clamp(y2, minYBound, maxYBound);

		Color* startPoint = surf->getPixels() + minX + (minY * tempWidth);
		Color* endPoint = surf->getPixels() + maxX + (maxY * tempWidth);
		
		int drawImgWidth = (maxX - minX);
		int drawImgHeight = (maxY - minY);

		int otherImgAddX = tempWidth - drawImgWidth;

		Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);

		int textureWidth = img->getWidth();
		int textureHeight = img->getHeight();

		int imgStartX = MathExt::max(minXBound - x1, 0);
		int imgStartY = MathExt::max(minYBound - y1, 0);

		if(imgStartX >= textureWidth)
			return;
		if(imgStartY >= textureHeight)
			return;
			
		int tY = 0;

		while(startPoint < endPoint)
		{
			double v = textureHeight * ((double)tY/(y2-y1));
			v += imgStartY;

			if(v >= textureHeight)
				return;
			
			for(int i=0; i<=drawImgWidth; i++)
			{
				double u = textureWidth * ((double)i/(x2-x1));
				u += imgStartX;

				Color c = img->getPixel(u, v, Image::CLAMP);

				Vec4f adjustedColor = Vec4f( c.red * colorMult.x, c.green * colorMult.y, c.blue * colorMult.z, c.alpha * colorMult.w);
				c = {(unsigned char)adjustedColor.x, (unsigned char)adjustedColor.y, (unsigned char)adjustedColor.z, (unsigned char)adjustedColor.w };

				*startPoint = c;

				startPoint++;
			}

			tY++;
			startPoint += otherImgAddX;
		}
	}

	//TODO - Fix culling stuff
	void SimpleGraphics::drawSpritePart_OLD(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf)
	{
		int currentComposite = compositeRule;
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr && img!=nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			if(img->getWidth()<=0 || img->getHeight()<=0)
			{
				return;
			}
			int tempWidth = otherImg->getWidth();
			int tempHeight = otherImg->getHeight();

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

			int clampImgW = MathExt::clamp(imgW, 0, img->getWidth()-1);
			int clampImgH = MathExt::clamp(imgH, 0, img->getHeight()-1);
			
			int minX = MathExt::clamp(x, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+clampImgW, minXBound, maxXBound);

			int minY = MathExt::clamp(y, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+clampImgH, minYBound, maxYBound);

			Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
			Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);
			
			int drawImgWidth = (maxX - minX) + 1;
			int otherImgAddX = tempWidth - drawImgWidth;

			int imgStartX = imgX + MathExt::max(minXBound - x, 0);
			int imgStartY = imgY + MathExt::max(minYBound - y, 0);

			if(imgStartX >= img->getWidth())
				return;
			if(imgStartY >= img->getHeight())
				return;

			Color* drawImgStart = img->getPixels() + imgStartX + (imgStartY*img->getWidth());
			int drawImgAddX = img->getWidth() - drawImgWidth;

			int tX = 0;
			Vec4f colorMult = Vec4f((double)SimpleGraphics::activeColor.red / 255.0, (double)SimpleGraphics::activeColor.green / 255.0, (double)SimpleGraphics::activeColor.blue / 255.0, (double)SimpleGraphics::activeColor.alpha / 255.0);

			#if(OPTI>=2)
				
				int avxWidth = (1+maxX-minX) >> 3;
				int remainder = (1+maxX-minX) - (avxWidth<<3);

				__m256 colorMultRed = _mm256_set1_ps( ((float)SimpleGraphics::activeColor.red) / 255.0 );
				__m256 colorMultGreen = _mm256_set1_ps( ((float)SimpleGraphics::activeColor.green) / 255.0 );
				__m256 colorMultBlue = _mm256_set1_ps( ((float)SimpleGraphics::activeColor.blue) / 255.0 );
				__m256 colorMultAlpha = _mm256_set1_ps( ((float)SimpleGraphics::activeColor.alpha) / 255.0 );

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							
							Color* colors = (Color*)&avxColor;

							__m256 reds = _mm256_set_ps(
								(float)colors[7].red, (float)colors[6].red, (float)colors[5].red, (float)colors[4].red,
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m256 greens = _mm256_set_ps(
								(float)colors[7].green, (float)colors[6].green, (float)colors[5].green, (float)colors[4].green,
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m256 blues = _mm256_set_ps(
								(float)colors[7].blue, (float)colors[6].blue, (float)colors[5].blue, (float)colors[4].blue,
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m256 alphas = _mm256_set_ps(
								(float)colors[7].alpha, (float)colors[6].alpha, (float)colors[5].alpha, (float)colors[4].alpha,
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm256_mul_ps(reds, colorMultRed);
							greens = _mm256_mul_ps(greens, colorMultGreen);
							blues = _mm256_mul_ps(blues, colorMultBlue);
							alphas = _mm256_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m256i finalColor = _mm256_set_epi8(
								(unsigned char)aFloats[7], (unsigned char)bFloats[7], (unsigned char)gFloats[7], (unsigned char)rFloats[7],
								(unsigned char)aFloats[6], (unsigned char)bFloats[6], (unsigned char)gFloats[6], (unsigned char)rFloats[6],
								(unsigned char)aFloats[5], (unsigned char)bFloats[5], (unsigned char)gFloats[5], (unsigned char)rFloats[5],
								(unsigned char)aFloats[4], (unsigned char)bFloats[4], (unsigned char)gFloats[4], (unsigned char)rFloats[4],
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);


							_mm256_storeu_si256(avxPoint, avxColor);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							__m256i currentColor = _mm256_loadu_si256(avxPoint);
							
							Color* colors = (Color*)&avxColor;

							__m256 reds = _mm256_set_ps(
								(float)colors[7].red, (float)colors[6].red, (float)colors[5].red, (float)colors[4].red,
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m256 greens = _mm256_set_ps(
								(float)colors[7].green, (float)colors[6].green, (float)colors[5].green, (float)colors[4].green,
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m256 blues = _mm256_set_ps(
								(float)colors[7].blue, (float)colors[6].blue, (float)colors[5].blue, (float)colors[4].blue,
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m256 alphas = _mm256_set_ps(
								(float)colors[7].alpha, (float)colors[6].alpha, (float)colors[5].alpha, (float)colors[4].alpha,
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm256_mul_ps(reds, colorMultRed);
							greens = _mm256_mul_ps(greens, colorMultGreen);
							blues = _mm256_mul_ps(blues, colorMultBlue);
							alphas = _mm256_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m256i finalColor = _mm256_set_epi8(
								(unsigned char)aFloats[7], (unsigned char)bFloats[7], (unsigned char)gFloats[7], (unsigned char)rFloats[7],
								(unsigned char)aFloats[6], (unsigned char)bFloats[6], (unsigned char)gFloats[6], (unsigned char)rFloats[6],
								(unsigned char)aFloats[5], (unsigned char)bFloats[5], (unsigned char)gFloats[5], (unsigned char)rFloats[5],
								(unsigned char)aFloats[4], (unsigned char)bFloats[4], (unsigned char)gFloats[4], (unsigned char)rFloats[4],
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);

							__m256i blendC = blend(finalColor, currentColor);
							_mm256_storeu_si256(avxPoint, blendC);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#elif(OPTI>=1)

				int sseWidth = (1+maxX-minX) >> 2;
				int remainder = (1+maxX-minX) - (sseWidth<<2);

				__m128 colorMultRed = _mm_set1_ps( ((float)SimpleGraphics::activeColor.red) / 255.0 );
				__m128 colorMultGreen = _mm_set1_ps( ((float)SimpleGraphics::activeColor.green) / 255.0 );
				__m128 colorMultBlue = _mm_set1_ps( ((float)SimpleGraphics::activeColor.blue) / 255.0 );
				__m128 colorMultAlpha = _mm_set1_ps( ((float)SimpleGraphics::activeColor.alpha) / 255.0 );

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							
							Color* colors = (Color*)&sseColor;

							__m128 reds = _mm_set_ps(
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m128 greens = _mm_set_ps(
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m128 blues = _mm_set_ps(
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m128 alphas = _mm_set_ps(
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm_mul_ps(reds, colorMultRed);
							greens = _mm_mul_ps(greens, colorMultGreen);
							blues = _mm_mul_ps(blues, colorMultBlue);
							alphas = _mm_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m128i finalColor = _mm_set_epi8(
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);


							_mm_storeu_si128(ssePoint, sseColor);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							__m128i currentColor = _mm_loadu_si128(ssePoint);
							
							Color* colors = (Color*)&sseColor;

							__m128 reds = _mm_set_ps(
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m128 greens = _mm_set_ps(
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m128 blues = _mm_set_ps(
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m128 alphas = _mm_set_ps(
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm_mul_ps(reds, colorMultRed);
							greens = _mm_mul_ps(greens, colorMultGreen);
							blues = _mm_mul_ps(blues, colorMultBlue);
							alphas = _mm_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m128i finalColor = _mm_set_epi8(
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);

							__m128i blendC = blend(finalColor, currentColor);
							_mm_storeu_si128(ssePoint, blendC);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#else
				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;

							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);

							startPoint++;
							drawImgStart++;
						}
					
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}

			#endif
		}
	}

} //NAMESPACE glib END