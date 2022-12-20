#include "InternalGraphicsHeader.h"

namespace glib
{
    void SimpleGraphics::drawEllipse(int x, int y, int xRad, int yRad, bool outline, Image* surf)
	{
		// int currentComposite = compositeRule; //Not used

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
			
			int absXRad = MathExt::abs(xRad);
			int absYRad = MathExt::abs(yRad);
			
			int tempWidth = otherImg->getWidth();
			int tempHeight = otherImg->getHeight();
			
			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x-absXRad, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+absXRad, minXBound, maxXBound);

			int minY = MathExt::clamp(y-absYRad, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+absYRad, minYBound, maxYBound);

			double xrSqr = MathExt::sqr(absXRad-1);
			double xrSqrOuter = MathExt::sqr(absXRad+1);
			
			double yrSqr = MathExt::sqr(absYRad-1);
			double yrSqrOuter = MathExt::sqr(absYRad+1);
			
			if(absXRad == 0 || absYRad == 0)
				return;

			double A,B,C, A2,B2,C2;
			double xOffConst, xOffConst2;

			A = 1.0/xrSqr;
			B = -(2.0*x)/xrSqr;
			C = 0.0;
			xOffConst = MathExt::sqr(x)/xrSqr;
			
			A2 = 1.0/xrSqrOuter;
			B2 = -(2.0*x)/xrSqrOuter;
			C2 = 0.0;
			xOffConst2 = MathExt::sqr(x)/xrSqrOuter;
			
			if(outline==false)
			{
				//x^2/xRad^2 + y^2/yRad^2 = 1
				//yRad^2 * x^2  +  xRad^2 * y^2 = xRad^2 * yRad^2

				if(!antiAliasing)
				{
					for(int tY=minY; tY<=maxY; tY++)
					{
						int startX, endX;

						if(tY < y)
							C = MathExt::sqr(y-tY-0.5)/yrSqr + xOffConst - 1;
						else
							C = MathExt::sqr(y-tY+0.5)/yrSqr + xOffConst - 1;

						std::vector<double> xRange = MathExt::solveQuadraticReal(A, B, C);

						if(xRange.size() <= 0)
							continue;
						
						startX = (int)MathExt::round(xRange[1]);
						endX = (int)MathExt::round(xRange[0]);

						startX = MathExt::clamp(startX, minX, maxX);
						endX = MathExt::clamp(endX, minX, maxX);
						
						for(int tX=startX; tX<=endX; tX++)
						{
							drawPixel(tX, tY, activeColor, surf);
						}
					}
				}
				else
				{
					for(int tY=minY; tY<=maxY; tY++)
					{
						int x1, x2, x3, realX3, x4;

						//Does not adjust for pixel center but the results are good regardless.
						C = MathExt::sqr(y-tY)/yrSqr + xOffConst - 1;
						C2 = MathExt::sqr(y-tY)/yrSqrOuter + xOffConst2 - 1;
						
						std::vector<double> xRangeInner = MathExt::solveQuadraticReal(A, B, C);
						std::vector<double> xRangeOuter = MathExt::solveQuadraticReal(A2, B2, C2);

						if(xRangeInner.size() <= 0)
						{
							if(xRangeOuter.size() > 0 && absXRad <= absYRad)
							{
								//all partially transparent
								x1 = (int)MathExt::floor(xRangeOuter[1]);
								x2 = (int)MathExt::ceil(xRangeOuter[0]);

								x1 = MathExt::clamp(x1, minX, maxX);
								x2 = MathExt::clamp(x2, minX, maxX);
								
								//Small offset needed to look better.
								//Note that even adjusting the y position by 0.5 to be in the center of a pixel does not result in pleasing results.
								//Adjusting the distance by half a pixel does.
								double inDis = absYRad-0.5;
								double outDis = inDis+1;
								double maxAddDis = outDis - inDis;
								
								for(int tX=x1; tX<x2; tX++)
								{
									double currDis = MathExt::sqrt(MathExt::sqr(tX-x) + MathExt::sqr(tY-y));
									double lerpVal = 1.0 - ((currDis - inDis)/maxAddDis);

									if(lerpVal <= 0)
										continue;
									if(lerpVal > 1)
										continue;

									Color c = activeColor;
									c.alpha = (unsigned char)(lerpVal*c.alpha);
									
									drawPixel(tX, tY, c, surf);
								}
							}

							continue;
						}
						
						x1 = (int)MathExt::floor(xRangeOuter[1]);
						x2 = (int)MathExt::ceil(xRangeInner[1]);
						x3 = (int)MathExt::floor(xRangeInner[0])+1;
						x4 = (int)MathExt::ceil(xRangeOuter[0]);

						x1 = MathExt::clamp(x1, minX, maxX);
						x2 = MathExt::clamp(x2, minX, maxX);
						realX3 = MathExt::clamp(x3-1, minX, maxX); //Used for the middle section
						x3 = MathExt::clamp(x3, minX, maxX);
						x4 = MathExt::clamp(x4, minX, maxX);
						
						//left side
						Vec2f innerPoint = Vec2f(xRangeInner[1]-x, tY-y);
						double inDis = innerPoint.getLength();
						double outDis = inDis+1;
						double maxAddDis = outDis - inDis;
						
						for(int tX=x1; tX<x2; tX++)
						{
							// drawPixel(tX, tY, {255,255,255,255}, surf);
							double currDis = MathExt::sqrt(MathExt::sqr(tX-x) + MathExt::sqr(tY-y));
							double lerpVal = 1.0 - ((currDis - inDis)/maxAddDis);

							if(lerpVal <= 0)
								continue;
							if(lerpVal >= 1)
								continue;

							Color c = activeColor;
							c.alpha = (unsigned char)(lerpVal*c.alpha);
							
							drawPixel(tX, tY, c, surf);
						}

						//middle
						for(int tX=x2; tX<=realX3; tX++)
						{
							drawPixel(tX, tY, activeColor, surf);
						}

						//right side
						for(int tX=x3; tX<=x4; tX++)
						{
							double currDis = MathExt::sqrt(MathExt::sqr(tX-x) + MathExt::sqr(tY-y));
							double lerpVal = 1.0 - ((currDis - inDis)/maxAddDis);

							if(lerpVal <= 0)
								continue;
							if(lerpVal >= 1)
								continue;

							Color c = activeColor;
							c.alpha = (unsigned char)(lerpVal*c.alpha);
							
							drawPixel(tX, tY, c, surf);
						}
					}
				}
			}
			else
			{

			}
		}
	}

} //NAMESPACE glib END