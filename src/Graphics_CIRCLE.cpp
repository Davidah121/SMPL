#include "InternalGraphicsHeader.h"

namespace glib
{
    void Graphics::drawCircle(int x, int y, int radius, bool outline, Image* surf)
	{
		int currentComposite = compositeRule;

		Image* otherImg;
		if (surf == nullptr)
			otherImg = activeImage;
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

							if(xDisToCenter==0)
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

							if(xDisToCenter==0)
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
				int preX1 = -1;
				int preX2 = -1;

				int nextX1 = -1;
				int nextX2 = -1;
				
				while (tY <= maxY)
				{
					double startX,endX;

					if(tY>y)
					{
						startX = -MathExt::sqrt( radSqr - MathExt::sqr((tY+1)-y+0.5) ) + x + 1;
						endX = MathExt::sqrt( radSqr - MathExt::sqr((tY+1)-y+0.5) ) + x;

						nextX1 = MathExt::clamp( (int)MathExt::round(startX), minX, maxX);
						nextX2 = MathExt::clamp( (int)MathExt::round(endX-1), minX, maxX);

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

					//fill from preX1 to x1 and x2 to preX2
					int nStart, nEnd, nStart2, nEnd2;

					if(tY != minY && tY != maxY)
					{
						if(tY <= y)
						{
							int leftToRightDis = MathExt::abs(x1-preX1)-1;
							leftToRightDis = MathExt::max(leftToRightDis, 0);

							nStart = x1;
							nEnd = x1+leftToRightDis;
							
							drawLine(nStart,tY,nEnd,tY,surf);

							leftToRightDis = MathExt::abs(x2-preX2)-1;
							leftToRightDis = MathExt::max(leftToRightDis, 0);

							nStart = (x2-leftToRightDis);
							nEnd = x2;

							drawLine(nStart,tY,nEnd,tY,surf);
						}
						else
						{
							int leftToRightDis = MathExt::abs(x1-nextX1)-1;
							leftToRightDis = MathExt::max(leftToRightDis, 0);

							nStart = x1;
							nEnd = x1+leftToRightDis;

							drawLine(nStart,tY,nEnd,tY,surf);

							leftToRightDis = MathExt::abs(x2-nextX2)-1;
							leftToRightDis = MathExt::max(leftToRightDis, 0);

							nStart = (x2-leftToRightDis);
							nEnd = x2;

							drawLine(nStart,tY,nEnd,tY,surf);
						}
					}
					else
					{
						drawLine(x1,tY,x2,tY,surf);
					}

					preX1 = x1;
					preX2 = x2;
					tY++;
				}
				
			}
		}
	}
    
} //NAMESPACE glib END