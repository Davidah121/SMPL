#include "InternalGraphicsHeader.h"

namespace smpl
{
	
	#if (OPTI > 0)
    void SimpleGraphics::drawCircle(int x, int y, int radius, bool outline, Image* surf)
	{
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
			
			// int tX = minX;
			// int tY = minY;
			double radSqr = MathExt::sqr(absRad-1);
			double radSqr2 = MathExt::sqr(absRad);

			GRAPHICS_SIMD_DATATYPE activeColorAsSIMD = COLOR_TO_SIMD(activeColor);
			
			if (outline == false)
			{
				double A = 1;
				double B = -2*x;

				Color* srcPixels = otherImg->getPixels();

				if(!antiAliasing)
				{
					if(compositeRule == NO_COMPOSITE)
					{
						#pragma omp parallel for
						for(int tY=minY; tY<=maxY; tY++)
						{
							double C = 0;
							if(tY < y)
								C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;
							else
								C = MathExt::sqr(y-tY+0.5) + MathExt::sqr(x)+1;

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);

							if(xRangeInCircle.size()<=0)
							{
								continue;
							}

							int x1 = (int)MathExt::round(xRangeInCircle[1]);
							int x2 = (int)MathExt::round(xRangeInCircle[0]);
							
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

							int len = x2-x1;
							int simdBound = GET_GRAPHICS_SIMD_BOUND(len);
							int i=x1;
							for(int k=0; k<simdBound; k+=GRAPHICS_INC_AMOUNT)
							{
								GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&srcPixels[i + tY*tempWidth], activeColorAsSIMD);
								i+=GRAPHICS_INC_AMOUNT;
							}
							while(i <= x2)
							{
								srcPixels[i + tY*tempWidth] = activeColor;
								i++;
							}
						}
					}
					else
					{
						#pragma omp parallel for
						for(int tY=minY; tY<=maxY; tY++)
						{
							double C = 0;
							if(tY < y)
								C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;
							else
								C = MathExt::sqr(y-tY+0.5) + MathExt::sqr(x)+1;

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);

							if(xRangeInCircle.size()<=0)
							{
								continue;
							}

							int x1 = (int)MathExt::round(xRangeInCircle[1]);
							int x2 = (int)MathExt::round(xRangeInCircle[0]);
							
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

							int len = x2-x1;
							int simdBound = GET_GRAPHICS_SIMD_BOUND(len);
							int i=x1;
							for(int k=0; k<simdBound; k+=GRAPHICS_INC_AMOUNT)
							{
								GRAPHICS_SIMD_DATATYPE destColor = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&srcPixels[i + tY*tempWidth]);
								GRAPHICS_SIMD_DATATYPE blendedColor = blend(activeColorAsSIMD, destColor);
								GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&srcPixels[i + tY*tempWidth], blendedColor);
								i+=GRAPHICS_INC_AMOUNT;
							}
							while(i <= x2)
							{
								srcPixels[i + tY*tempWidth] = blend(activeColor, srcPixels[i + tY*tempWidth]);
								i++;
							}
						}
					}
				}
				else
				{
					if(compositeRule == NO_COMPOSITE)
					{
						#pragma omp parallel for
						for(int tY=minY; tY<=maxY; tY++)
						{
							//create polynomial to describe distance
							double C = MathExt::sqr(y-tY) + MathExt::sqr(x);

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);
							std::vector<double> xRangeOnEdgeCircle = MathExt::solveQuadraticReal(A, B, C-radSqr2);

							//fill inner xRange
							if(xRangeInCircle.size()<=0)
							{
								continue;
							}

							//fill outer xRange
							int startX, endX, startX2, endX2;

							startX = (int)MathExt::floor(xRangeOnEdgeCircle[1]);
							endX = (int)MathExt::ceil(xRangeInCircle[1]);

							startX2 = (int)MathExt::floor(xRangeInCircle[0])+1;
							endX2 = (int)MathExt::ceil(xRangeOnEdgeCircle[0]);
							
							startX = (int)MathExt::clamp( startX, minX, maxX);
							endX = (int)MathExt::clamp( endX, minX, maxX);
							
							startX2 = (int)MathExt::clamp( startX2, minX, maxX);
							endX2 = (int)MathExt::clamp( endX2, minX, maxX);
							
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

								srcPixels[tX + tY*tempWidth] = newBlendColor;
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
								
								srcPixels[tX + tY*tempWidth] = newBlendColor;
							}

							int x1 = (int)MathExt::ceil(xRangeInCircle[1]);
							int x2 = (int)MathExt::floor(xRangeInCircle[0]);
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

							int len = x2-x1;
							int simdBound = GET_GRAPHICS_SIMD_BOUND(len);
							int i=x1;
							for(int k=0; k<simdBound; k+=GRAPHICS_INC_AMOUNT)
							{
								GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&srcPixels[i + tY*tempWidth], activeColorAsSIMD);
								i+=GRAPHICS_INC_AMOUNT;
							}
							while(i <= x2)
							{
								srcPixels[i + tY*tempWidth] = activeColor;
								i++;
							}
						}
					}
					else
					{
						#pragma omp parallel for
						for(int tY=minY; tY<=maxY; tY++)
						{
							//create polynomial to describe distance
							double C = MathExt::sqr(y-tY) + MathExt::sqr(x);

							std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);
							std::vector<double> xRangeOnEdgeCircle = MathExt::solveQuadraticReal(A, B, C-radSqr2);

							//fill inner xRange
							if(xRangeInCircle.size()<=0)
							{
								continue;
							}

							//fill outer xRange
							int startX, endX, startX2, endX2;

							startX = (int)MathExt::floor(xRangeOnEdgeCircle[1]);
							endX = (int)MathExt::ceil(xRangeInCircle[1]);

							startX2 = (int)MathExt::floor(xRangeInCircle[0])+1;
							endX2 = (int)MathExt::ceil(xRangeOnEdgeCircle[0]);
							
							startX = (int)MathExt::clamp( startX, minX, maxX);
							endX = (int)MathExt::clamp( endX, minX, maxX);
							
							startX2 = (int)MathExt::clamp( startX2, minX, maxX);
							endX2 = (int)MathExt::clamp( endX2, minX, maxX);
							
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

								srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
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
								
								srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
							}

							int x1 = (int)MathExt::ceil(xRangeInCircle[1]);
							int x2 = (int)MathExt::floor(xRangeInCircle[0]);
							x1 = MathExt::clamp( x1, minX, maxX);
							x2 = MathExt::clamp( x2, minX, maxX);

							int len = x2-x1;
							int simdBound = GET_GRAPHICS_SIMD_BOUND(len);
							int i=x1;
							for(int k=0; k<simdBound; k+=GRAPHICS_INC_AMOUNT)
							{
								GRAPHICS_SIMD_DATATYPE destColor = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)&srcPixels[i + tY*tempWidth]);
								GRAPHICS_SIMD_DATATYPE blendedColor = blend(activeColorAsSIMD, destColor);
								GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)&srcPixels[i + tY*tempWidth], blendedColor);
								i+=GRAPHICS_INC_AMOUNT;
							}
							while(i <= x2)
							{
								srcPixels[i + tY*tempWidth] = blend(activeColor, srcPixels[i + tY*tempWidth]);
								i++;
							}
						}
					}
				}
			}
			else
			{
				int oldX1, oldX2;
				
				oldX1 = x;
				oldX2 = x;

				#pragma omp parallel for
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
	#endif

} //NAMESPACE glib END