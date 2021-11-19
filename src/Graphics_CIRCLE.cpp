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
			double radSqr = MathExt::sqr(absRad-1);
			double radSqr2 = MathExt::sqr(absRad);
			
			if (outline == false)
			{
				
				double A = 1;
				double B = -2*x;
				double C = 0;

				#if(OPTI>=2)

					Color* startPoint = otherImg->getPixels() + (minY * tempWidth);
					Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

					__m256i* ssePoint;
					
					__m256i sseColor = _mm256_set1_epi32( *((int*)&activeColor) );

					if(!antiAliasing)
					{
						while (startPoint < endPoint)
						{
							if(tY < y)
								C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;
							else
								C = MathExt::sqr(y-tY+0.5) + MathExt::sqr(x)+1;

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);

							if(xRangeInCircle.size()<=0)
							{
								tY++;
								startPoint += tempWidth;
								continue;
							}

							int x1 = MathExt::round(xRangeInCircle[1]);
							int x2 = MathExt::round(xRangeInCircle[0]);
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

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

							for(int i=0; i<=remainder; i++)
							{
								*startPoint = blend(activeColor, *startPoint);
								startPoint++;
							}

							tY++;
							startPoint += addAmount2-1;
						}

					}
					else
					{
						while (startPoint < endPoint)
						{
							double C = MathExt::sqr(y-tY) + MathExt::sqr(x);

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);
							std::vector<double> xRangeOnEdgeCircle = MathExt::solveQuadraticReal(A, B, C-radSqr2);

							if(xRangeInCircle.size()<=0)
							{
								tY++;
								startPoint += tempWidth;
								continue;
							}
							
							//Fill outer xRange. Does not use sse instructions as it is likely that
							//it won't fit in the registers properly. (Less than 8 color values)
							int startX, endX, startX2, endX2;
							
							startX = MathExt::floor(xRangeOnEdgeCircle[1]);
							endX = MathExt::ceil(xRangeInCircle[1]);

							startX2 = MathExt::floor(xRangeInCircle[0])+1;
							endX2 = MathExt::ceil(xRangeOnEdgeCircle[0]);
							
							startX = MathExt::clamp( startX, minX, maxX);
							endX = MathExt::clamp( endX, minX, maxX);
							
							startX2 = MathExt::clamp( startX2, minX, maxX);
							endX2 = MathExt::clamp( endX2, minX, maxX);

							//draw left side
							for(int tX = startX; tX < endX; tX++)
							{
								int dis = MathExt::sqr(tY - y) + MathExt::sqr(tX - x);
								double maxAddDis = radSqr2 - radSqr;
								double lerpVal = (double)(radSqr2 - dis) / maxAddDis;

								if(lerpVal < 0)
									continue;
								
								Color newBlendColor = activeColor;
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);

								startPoint[tX] = blend(newBlendColor, startPoint[tX]);
							}

							//draw right side
							for(int tX = startX2; tX <= endX2; tX++)
							{
								int dis = MathExt::sqr(tY - y) + MathExt::sqr(tX - x);
								double maxAddDis = radSqr2 - radSqr;
								double lerpVal = (double)(radSqr2 - dis) / maxAddDis;
								
								if(lerpVal < 0)
									continue;
								
								Color newBlendColor = activeColor;
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);

								startPoint[tX] = blend(newBlendColor, startPoint[tX]);
							}

							//Fill inner x ranges
							int x1 = MathExt::ceil(xRangeInCircle[1]);
							int x2 = MathExt::floor(xRangeInCircle[0]);
							
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

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

							for(int i=0; i<=remainder; i++)
							{
								*startPoint = blend(activeColor, *startPoint);
								startPoint++;
							}

							tY++;
							startPoint += addAmount2-1;
						}
					}

					
				

				#elif(OPTI>=1)

					Color* startPoint = otherImg->getPixels() + (minY * tempWidth);
					Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

					__m128i* ssePoint;
					
					__m128i sseColor = _mm_set1_epi32( *((int*)&activeColor) );
					if(!antiAliasing)
					{
						while (startPoint < endPoint)
						{
							if(tY < y)
								C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;
							else
								C = MathExt::sqr(y-tY+0.5) + MathExt::sqr(x)+1;

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);

							if(xRangeInCircle.size()<=0)
							{
								tY++;
								startPoint += tempWidth;
								continue;
							}

							int x1 = MathExt::round(xRangeInCircle[1]);
							int x2 = MathExt::round(xRangeInCircle[0]);
							
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

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

							for(int i=0; i<=remainder; i++)
							{
								*startPoint = blend(activeColor, *startPoint);;
								startPoint++;
							}

							tY++;
							startPoint += addAmount2-1;
						}
					}
					else
					{
						while (startPoint < endPoint)
						{
							double C = MathExt::sqr(y-tY) + MathExt::sqr(x);

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);
							std::vector<double> xRangeOnEdgeCircle = MathExt::solveQuadraticReal(A, B, C-radSqr2);

							if(xRangeInCircle.size()<=0)
							{
								tY++;
								startPoint += tempWidth;
								continue;
							}
							
							//Fill outer xRange. Does not use sse instructions as it is likely that
							//it won't fit in the registers properly. (Less than 8 color values)
							int startX, endX, startX2, endX2;
							
							startX = MathExt::floor(xRangeOnEdgeCircle[1]);
							endX2 = MathExt::ceil(xRangeOnEdgeCircle[0]);

							endX = MathExt::ceil(xRangeInCircle[1]);
							startX2 = MathExt::floor(xRangeInCircle[0])+1;

							
							startX = MathExt::clamp( startX, minX, maxX);
							endX = MathExt::clamp( endX, minX, maxX);
							
							startX2 = MathExt::clamp( startX2, minX, maxX);
							endX2 = MathExt::clamp( endX2, minX, maxX);

							//draw left side
							for(int tX = startX; tX < endX; tX++)
							{
								int dis = MathExt::sqr(tY - y) + MathExt::sqr(tX - x);
								double maxAddDis = radSqr2 - radSqr;
								double lerpVal = (double)(radSqr2 - dis) / maxAddDis;

								if(lerpVal < 0)
									continue;
								
								Color newBlendColor = activeColor;
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);

								startPoint[tX] = blend(newBlendColor, startPoint[tX]);
							}

							//draw right side
							for(int tX = startX2; tX <= endX2; tX++)
							{
								int dis = MathExt::sqr(tY - y) + MathExt::sqr(tX - x);
								double maxAddDis = radSqr2 - radSqr;
								double lerpVal = (double)(radSqr2 - dis) / maxAddDis;
								
								if(lerpVal < 0)
									continue;
								
								Color newBlendColor = activeColor;
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);

								startPoint[tX] = blend(newBlendColor, startPoint[tX]);
							}

							//Fill inner x ranges
							int x1 = MathExt::ceil(xRangeInCircle[1]);
							int x2 = MathExt::floor(xRangeInCircle[0]);
							
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

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

							for(int i=0; i<=remainder; i++)
							{
								*startPoint = blend(activeColor, *startPoint);
								startPoint++;
							}

							tY++;
							startPoint += addAmount2-1;
						}
					}

				#else
				
					Color* startPoint = otherImg->getPixels() + (minY * tempWidth);
					Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

					if(!antiAliasing)
					{
						while (startPoint < endPoint)
						{
							double C = 0;
							if(tY < y)
								C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;
							else
								C = MathExt::sqr(y-tY+0.5) + MathExt::sqr(x)+1;

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);

							if(xRangeInCircle.size()<=0)
							{
								tY++;
								startPoint += tempWidth;
								continue;
							}

							int x1 = MathExt::round(xRangeInCircle[1]);
							int x2 = MathExt::round(xRangeInCircle[0]);
							
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

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
					else
					{
						while (startPoint < endPoint)
						{
							//create polynomial to describe distance
							double C = MathExt::sqr(y-tY) + MathExt::sqr(x);

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);
							std::vector<double> xRangeOnEdgeCircle = MathExt::solveQuadraticReal(A, B, C-radSqr2);


							//fill inner xRange
							if(xRangeInCircle.size()<=0)
							{
								tY++;
								startPoint += tempWidth;
								continue;
							}

							//fill outer xRange
							int startX, endX, startX2, endX2;

							startX = MathExt::floor(xRangeOnEdgeCircle[1]);
							endX = MathExt::ceil(xRangeInCircle[1]);

							startX2 = MathExt::floor(xRangeInCircle[0])+1;
							endX2 = MathExt::ceil(xRangeOnEdgeCircle[0]);
							
							startX = MathExt::clamp( startX, minX, maxX);
							endX = MathExt::clamp( endX, minX, maxX);
							
							startX2 = MathExt::clamp( startX2, minX, maxX);
							endX2 = MathExt::clamp( endX2, minX, maxX);
							
							//draw left side
							for(int tX = startX; tX < endX; tX++)
							{
								int dis = MathExt::sqr(tY - y) + MathExt::sqr(tX - x);
								double maxAddDis = radSqr2 - radSqr;
								double lerpVal = (double)(radSqr2 - dis) / maxAddDis;

								if(lerpVal < 0)
									continue;
								
								Color newBlendColor = activeColor;
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);

								startPoint[tX] = blend(newBlendColor, startPoint[tX]);
							}

							//draw right side
							for(int tX = startX2; tX <= endX2; tX++)
							{
								int dis = MathExt::sqr(tY - y) + MathExt::sqr(tX - x);
								double maxAddDis = radSqr2 - radSqr;
								double lerpVal = (double)(radSqr2 - dis) / maxAddDis;
								
								if(lerpVal < 0)
									continue;
								
								Color newBlendColor = activeColor;
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
								
								startPoint[tX] = blend(newBlendColor, startPoint[tX]);
							}

							int x1 = MathExt::ceil(xRangeInCircle[1]);
							int x2 = MathExt::floor(xRangeInCircle[0]);
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

							int addAmount = x1;
							int addAmount2 = tempWidth-x2;

							startPoint += addAmount;
							for(int i=x1; i<=x2; i++)
							{
								*startPoint = blend(activeColor, *startPoint);
								startPoint++;
							}

							tY++;
							startPoint += addAmount2-1; //sets startPoint to the beginning of the next scanline
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
    
	void Graphics::testDrawCircle(int x, int y, int radius, bool outline, Image* surf)
	{
		int currentComposite = compositeRule;

		if (surf != nullptr)
		{
			if(surf->getWidth()<=0 || surf->getHeight()<=0)
			{
				return;
			}
			int absRad = MathExt::abs(radius);

			int tempWidth = surf->getWidth();
			int tempHeight = surf->getHeight();

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x-absRad, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+absRad, minXBound, maxXBound);

			int minY = MathExt::clamp(y-absRad+1, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+absRad-1, minYBound, maxYBound);
			
			double radSqr = MathExt::sqr(absRad-1);
			double radSqr2 = MathExt::sqr(absRad);

			#define OLD
			#ifndef OLD

				for(int tY = minY; tY<=maxY; tY++)
				{
					for(int tX = minX; tX<=maxX; tX++)
					{
						//measure distance to center x,y
						int dis = MathExt::sqr(tY - y) + MathExt::sqr(tX - x);
						
						if(dis > radSqr2)
						{
							//not a pixel in circle
							continue;
						}
						if(dis <= radSqr)
						{
							//fully in circle
							drawPixel(tX, tY, activeColor, surf);
						}
						else if(dis < radSqr2)
						{
							//partially in circle
							//the closer to radSqr2 the more transparent
							//the closer to radSqr the more opaque
							//double maxAddDis = radSqr2 - radSqr;
							//double alpha = 1.0*lerpVal + 0.0*(1.0-lerpVal) = 1.0*lerpVal;
							//double lerpVal = (radSqr2 - dis)/maxAddDis;

							double maxAddDis = radSqr2 - radSqr;
							double lerpVal = (double)(radSqr2 - dis) / maxAddDis;
							
							Color newBlendColor = activeColor;
							newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);

							drawPixel(tX, tY, newBlendColor, surf);
						}
						
					}
				}

			#else
				double A = 1;
				double B = -2*x;

				if(antiAliasing)
				{
					for(int tY = minY; tY<=maxY; tY++)
					{
						//create polynomial to describe distance
						double C = MathExt::sqr(y-tY) + MathExt::sqr(x);

						std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);
						std::vector<double> xRangeOnEdgeCircle = MathExt::solveQuadraticReal(A, B, C-radSqr2);


						//fill inner xRange
						if(xRangeInCircle.size() > 0)
						{
							int startX, endX;

							if(xRangeInCircle[0] < xRangeInCircle[1])
							{
								startX = MathExt::ceil(xRangeInCircle[0]);
								endX = MathExt::floor(xRangeInCircle[1]);
							}
							else
							{
								startX = MathExt::ceil(xRangeInCircle[1]);
								endX = MathExt::floor(xRangeInCircle[0]);
							}

							for(int tX = startX; tX <= endX; tX++)
							{
								drawPixel(tX, tY, activeColor, surf);
							}
						}
						else
						{
							//error
							continue;
						}

						//fill outer xRange
						int startX, endX, startX2, endX2;

						if(xRangeOnEdgeCircle[0] < xRangeOnEdgeCircle[1])
						{
							startX = MathExt::floor(xRangeOnEdgeCircle[0]);
							endX2 = MathExt::ceil(xRangeOnEdgeCircle[1]);
						}
						else
						{
							startX = MathExt::floor(xRangeOnEdgeCircle[1]);
							endX2 = MathExt::ceil(xRangeOnEdgeCircle[0]);
						}

						if(xRangeInCircle[0] < xRangeInCircle[1])
						{
							endX = MathExt::ceil(xRangeInCircle[0]);
							startX2 = MathExt::floor(xRangeInCircle[1])+1;
						}
						else
						{
							endX = MathExt::ceil(xRangeInCircle[1]);
							startX2 = MathExt::floor(xRangeInCircle[0])+1;
						}

						//draw left side
						for(int tX = startX; tX < endX; tX++)
						{
							int dis = MathExt::sqr(tY - y) + MathExt::sqr(tX - x);
							double maxAddDis = radSqr2 - radSqr;
							double lerpVal = (double)(radSqr2 - dis) / maxAddDis;

							if(lerpVal < 0)
								continue;
							
							Color newBlendColor = activeColor;
							newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);

							drawPixel(tX, tY, newBlendColor, surf);
						}

						//draw right side
						for(int tX = startX2; tX <= endX2; tX++)
						{
							int dis = MathExt::sqr(tY - y) + MathExt::sqr(tX - x);
							double maxAddDis = radSqr2 - radSqr;
							double lerpVal = (double)(radSqr2 - dis) / maxAddDis;
							
							if(lerpVal < 0)
								continue;
							
							Color newBlendColor = activeColor;
							newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);

							drawPixel(tX, tY, newBlendColor, surf);
						}
					
					}
				}
				else
				{
					for(int tY = minY; tY<=maxY; tY++)
					{
						//create polynomial to describe distance
						double C = 0;
						if(tY < y)
							C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;
						else
							C = MathExt::sqr(y-tY+0.5) + MathExt::sqr(x)+1;

						std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);
						

						//fill inner xRange
						if(xRangeInCircle.size() > 0)
						{
							int startX, endX;

							if(xRangeInCircle[0] < xRangeInCircle[1])
							{
								startX = MathExt::round(xRangeInCircle[0]);
								endX = MathExt::round(xRangeInCircle[1]);
							}
							else
							{
								startX = MathExt::round(xRangeInCircle[1]);
								endX = MathExt::round(xRangeInCircle[0]);
							}

							for(int tX = startX; tX <= endX; tX++)
							{
								drawPixel(tX, tY, activeColor, surf);
							}
						}
					}
				}

			#endif
			

		}
	}

} //NAMESPACE glib END