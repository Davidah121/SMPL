#include "InternalGraphicsHeader.h"

namespace glib
{
    void Graphics::drawCircle(int x, int y, int radius, bool outline, Image* surf)
	{
		int currentComposite = compositeRule;

		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			int absRad = MathExt::abs(radius);

			int tempWidth = otherImg->getWidth();
			int tempHeight = otherImg->getHeight();

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x-absRad, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+absRad, minXBound, maxXBound);

			int minY = MathExt::clamp(y-absRad+1, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+absRad-1, minYBound, maxYBound);
			
			int tX = minX;
			int tY = minY;
			double radSqr = MathExt::sqr(absRad);
			
			if (outline == false)
			{
				#if(OPTI>=2)

					Color* startPoint = otherImg->getPixels() + (minY * tempWidth);
					Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

					__m256i* ssePoint;
					
					__m256i sseColor = _mm256_set1_epi32( *((int*)&activeColor) );
					if(currentComposite == NO_COMPOSITE)
					{
						while (startPoint < endPoint)
						{
							double startX,endX;

							if(tY>y)
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x;
							}
							else
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x;
							}

							int x1 = MathExt::clamp( (int)MathExt::round(startX), minX, maxX);
							int x2 = MathExt::clamp( (int)MathExt::round(endX-1), minX, maxX);

							int addAmount = x1;
							int addAmount2 = tempWidth-x2;

							if(x1==x2)
							{
								startPoint += addAmount;
								*startPoint = activeColor;
								startPoint += addAmount2;
								tY++;
								continue;
							}

							int wid = ((x2-x1) >> 3);
							int remainder = (x2-x1) - (wid<<3);

							startPoint += addAmount;
							ssePoint = (__m256i*)startPoint;

							for(int i=0; i<wid; i++)
							{
								_mm256_storeu_si256(ssePoint, sseColor);
								ssePoint++;
							}

							startPoint += wid<<3;

							for(int i=0; i<remainder; i++)
							{
								*startPoint = activeColor;
								startPoint++;
							}

							tY++;
							startPoint += addAmount2;
						}
					}
					else
					{
						while (startPoint < endPoint)
						{
							double startX,endX;

							if(tY>y)
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x;
							}
							else
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x;
							}

							int x1 = MathExt::clamp( (int)MathExt::round(startX), minX, maxX);
							int x2 = MathExt::clamp( (int)MathExt::round(endX-1), minX, maxX);

							int addAmount = x1;
							int addAmount2 = tempWidth-x2;

							if(x1==x2)
							{
								startPoint += addAmount;
								*startPoint = blend(activeColor, *startPoint);
								startPoint += addAmount2;
								tY++;
								continue;
							}

							int wid = ((x2-x1) >> 3);
							int remainder = (x2-x1) - (wid<<3);

							startPoint += addAmount;
							ssePoint = (__m256i*)startPoint;

							for(int i=0; i<wid; i++)
							{
								__m256i currentColor = _mm256_loadu_si256(ssePoint);
								__m256i blendC = blend(sseColor, currentColor);
								_mm256_storeu_si256(ssePoint, blendC);
								ssePoint++;
							}

							startPoint += wid<<3;

							for(int i=0; i<remainder; i++)
							{
								*startPoint = blend(activeColor, *startPoint);
								startPoint++;
							}

							tY++;
							startPoint += addAmount2;
						}
					}

				#elif(OPTI>=1)

					Color* startPoint = otherImg->getPixels() + (minY * tempWidth);
					Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

					__m128i* ssePoint;
					
					__m128i sseColor = _mm_set1_epi32( *((int*)&activeColor) );
					if(currentComposite == NO_COMPOSITE)
					{
						while (startPoint < endPoint)
						{
							double startX,endX;

							if(tY>y)
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x;
							}
							else
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x;
							}

							int x1 = MathExt::clamp( (int)MathExt::round(startX), minX, maxX);
							int x2 = MathExt::clamp( (int)MathExt::round(endX-1), minX, maxX);

							int addAmount = x1;
							int addAmount2 = tempWidth-x2;

							if(x1==x2)
							{
								startPoint += addAmount;
								*startPoint = activeColor;
								startPoint += addAmount2;
								tY++;
								continue;
							}

							int wid = ((x2-x1) >> 2);
							int remainder = (x2-x1) - (wid<<2);

							startPoint += addAmount;
							ssePoint = (__m128i*)startPoint;

							for(int i=0; i<wid; i++)
							{
								_mm_storeu_si128(ssePoint, sseColor);
								ssePoint++;
							}

							startPoint += wid<<2;

							for(int i=0; i<remainder; i++)
							{
								*startPoint = activeColor;
								startPoint++;
							}

							tY++;
							startPoint += addAmount2;
						}
					}
					else
					{
						while (startPoint < endPoint)
						{
							double startX,endX;

							if(tY>y)
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x;
							}
							else
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x;
							}

							int x1 = MathExt::clamp( (int)MathExt::round(startX), minX, maxX);
							int x2 = MathExt::clamp( (int)MathExt::round(endX-1), minX, maxX);

							int addAmount = x1;
							int addAmount2 = tempWidth-x2;

							if(x1==x2)
							{
								startPoint += addAmount;
								*startPoint = blend(activeColor, *startPoint);
								startPoint += addAmount2;
								tY++;
								continue;
							}

							int wid = ((x2-x1) >> 2);
							int remainder = (x2-x1) - (wid<<2);

							startPoint += addAmount;
							ssePoint = (__m128i*)startPoint;

							for(int i=0; i<wid; i++)
							{
								__m128i currentColor = _mm_loadu_si128(ssePoint);
								__m128i blendC = blend(sseColor, currentColor);
								_mm_storeu_si128(ssePoint, blendC);
								ssePoint++;
							}

							startPoint += wid<<2;

							for(int i=0; i<remainder; i++)
							{
								*startPoint = blend(activeColor, *startPoint);
								startPoint++;
							}

							tY++;
							startPoint += addAmount2;
						}
					}

				#else
				
					Color* startPoint = otherImg->getPixels() + (minY * tempWidth);
					Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

					if(currentComposite == NO_COMPOSITE)
					{
						while (startPoint < endPoint)
						{
							double startX,endX;

							if(tY>y)
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x;
							}
							else
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x;
							}

							int x1 = MathExt::clamp( (int)MathExt::round(startX), minX, maxX);
							int x2 = MathExt::clamp( (int)MathExt::round(endX-1), minX, maxX);

							int addAmount = x1;
							int addAmount2 = tempWidth-x2;

							startPoint += addAmount;
							for(int i=x1; i<=x2; i++)
							{
								*startPoint = activeColor;
								startPoint++;
							}

							tY++;
							startPoint += addAmount-1;
						}
					}
					else
					{
						while (startPoint < endPoint)
						{
							double startX,endX;

							if(tY>y)
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x;
							}
							else
							{
								startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x + 1;
								endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x;
							}

							int x1 = MathExt::clamp( (int)MathExt::round(startX), minX, maxX);
							int x2 = MathExt::clamp( (int)MathExt::round(endX-1), minX, maxX);

							int addAmount = x1;
							int addAmount2 = tempWidth-x2;

							startPoint += addAmount;
							for(int i=x1; i<=x2; i++)
							{
								*startPoint = blend(activeColor, *startPoint);
								startPoint++;
							}

							tY++;
							startPoint += addAmount2-1;
						}
					}

				#endif
			}
			else
			{
				int oldX1, oldX2;
				
				oldX1 = x;
				oldX2 = x;

				for(int tY = minY; tY<=maxY+1; tY++)
				{
					double startX, endX;
					if(tY>y)
					{
						startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x + 1;
						endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y+0.5) ) + x;
					}
					else
					{
						startX = -MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x + 1;
						endX = MathExt::sqrt( radSqr - MathExt::sqr(tY-y-0.5) ) + x;
					}

					int x1 = MathExt::clamp( (int)MathExt::round(startX), minX, maxX);
					int x2 = MathExt::clamp( (int)MathExt::round(endX-1), minX, maxX);

					if(tY == minY)
					{
						if(tY == y-absRad+1)
						{
							drawLine(x1, tY, x2, tY, surf);
						}
						else
						{
							drawPixel(x1, tY, activeColor, surf);
							drawPixel(x2, tY, activeColor, surf);
						}
					}
					else if(tY == maxY+1)
					{
						if(tY == y+absRad)
						{
							drawLine(oldX1, tY-1, oldX2, tY-1, surf);
						}
					}
					else
					{
						//draw line from x1 to oldX1
						drawLine(x1, tY, oldX1, tY-1, surf);
						//draw line from x2 to oldX2
						drawLine(x2, tY, oldX2, tY-1, surf);
					}
					
					oldX1 = x1;
					oldX2 = x2;
				}
			}
		}
	}
    
} //NAMESPACE glib END