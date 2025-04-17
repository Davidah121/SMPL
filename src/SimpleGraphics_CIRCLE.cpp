#include "InternalGraphicsHeader.h"

namespace smpl
{
	#if (OPTI == 0)

    void SimpleGraphics::drawCircle(int x, int y, int radius, bool outline, Image* surf)
	{
		if(outline)
		{
			drawCircle(x, y, radius, 1, surf);
			return;
		}

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

			int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x-absRad, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+absRad, minXBound, maxXBound);

			int minY = MathExt::clamp(y-absRad, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+absRad, minYBound, maxYBound);
			
			// int tX = minX;
			// int tY = minY;
			double radSqr = MathExt::sqr(absRad-1);
			double radSqr2 = MathExt::sqr(absRad);
			
			int approxArea = (maxX-minX)*(maxY-minY);
			
			double A = 1;
			double B = -2*x;

			Color* srcPixels = otherImg->getPixels();

			if(!antiAliasing)
			{
				LARGE_ENOUGH_CLAUSE(approxArea)
				#pragma omp parallel for
				for(int tY=minY; tY<maxY; tY++)
				{
					double C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;
					std::vector<double> xRangeInCircle = MathExt::solveQuadraticReal(A, B, C-radSqr2);

					if(xRangeInCircle.size()<=0)
					{
						continue;
					}

					int x1 = (int)MathExt::round(xRangeInCircle[1]);
					int x2 = (int)MathExt::round(xRangeInCircle[0]);
					
					x1 = MathExt::clamp( x1, minX, maxX);
					x2 = MathExt::clamp( x2, minX, maxX);

					for(int i=x1; i<x2; i++)
					{
						srcPixels[i + tY*tempWidth] = blend(activeColor, srcPixels[i + tY*tempWidth]);
					}
				}
				RESET_LARGE_ENOUGH_CLAUSE()
			}
			else
			{
				LARGE_ENOUGH_CLAUSE(approxArea)
				#pragma omp parallel for
				for(int tY=minY; tY<maxY; tY++)
				{
					//create polynomial to describe distance
					double C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;

					std::vector<double> xRangeInnerCircle = MathExt::solveQuadraticReal(A, B, C-radSqr);
					std::vector<double> xRangeOuterCircle = MathExt::solveQuadraticReal(A, B, C-radSqr2);
					
					int x1 = (int)MathExt::round(xRangeOuterCircle[1]);
					int x2 = (int)MathExt::round(xRangeOuterCircle[0]);
					x1 = MathExt::clamp( x1, minX, maxX);
					x2 = MathExt::clamp( x2, minX, maxX);

					int x3 = 0;
					int x4 = 0;
					if(xRangeInnerCircle.size() >= 2)
					{
						x3 = (int)MathExt::round(xRangeInnerCircle[1]);
						x4 = (int)MathExt::round(xRangeInnerCircle[0]);
						x3 = MathExt::clamp( x3, minX, maxX);
						x4 = MathExt::clamp( x4, minX, maxX);
					}
					else
					{
						for(int tX = x1; tX <x2; tX++)
						{
							Color newBlendColor = activeColor;

							int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
							double maxAddDis = radSqr2 - radSqr;
							double lerpVal = (double)(radSqr2 - dis) / maxAddDis;

							if(lerpVal < 0)
								continue;
							if(lerpVal < 1)
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
							
							srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
						}
						continue;
					}
					
					//draw left side
					for(int tX = x1; tX < x3; tX++)
					{
						Color newBlendColor = activeColor;

						int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
						double maxAddDis = radSqr2 - radSqr;
						double lerpVal = (double)(radSqr2 - dis) / maxAddDis;

						if(lerpVal < 0)
							continue;
						if(lerpVal < 1)
							newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
						
						srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
					}

					//draw right side
					for(int tX = x4; tX < x2; tX++)
					{
						Color newBlendColor = activeColor;

						int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
						double maxAddDis = radSqr2 - radSqr;
						double lerpVal = (double)(radSqr2 - dis) / maxAddDis;

						if(lerpVal < 0)
							continue;
						if(lerpVal < 1)
							newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
						
						srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
					}
					
					for(int i=x3; i<x4; i++)
					{
						srcPixels[i + tY*tempWidth] = blend(activeColor, srcPixels[i + tY*tempWidth]);
					}
				}
				RESET_LARGE_ENOUGH_CLAUSE()
			}
		}
	}
	
	void SimpleGraphics::drawCircle(int x, int y, int radius, int maxDistanceFromEdge, Image* surf)
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

			int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x-absRad, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+absRad, minXBound, maxXBound);

			int minY = MathExt::clamp(y-absRad, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+absRad, minYBound, maxYBound);
			
			// int tX = minX;
			// int tY = minY;
			double outerRadSqrEdge = MathExt::sqr(absRad);
			double outerRadSqr = MathExt::sqr(absRad-1);
			double innerRadSqrEdge = MathExt::sqr(absRad-maxDistanceFromEdge);
			double innerRadSqr = MathExt::sqr(absRad-maxDistanceFromEdge-1);

			double minOutDis = MathExt::sqr(absRad-1);
			double maxOutDis = MathExt::sqr(absRad);
			double minInDis = MathExt::sqr(absRad-maxDistanceFromEdge-1);
			double maxInDis = MathExt::sqr(absRad-maxDistanceFromEdge);
			
			int approxArea = (maxX-minX)*(maxY-minY);
			
			double A = 1;
			double B = -2*x;
			Color* srcPixels = otherImg->getPixels();

			if(!antiAliasing)
			{
				LARGE_ENOUGH_CLAUSE(approxArea)
				#pragma omp parallel for
				for(int tY=minY; tY<maxY; tY++)
				{
					//create polynomial to describe distance
					double C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;

					std::vector<double> xRangeOuterEdgeCircle = MathExt::solveQuadraticReal(A, B, C-outerRadSqrEdge);
					std::vector<double> xRangeInnerEdgeCircle = MathExt::solveQuadraticReal(A, B, C-innerRadSqrEdge);
					std::vector<double> xRangeOuterCircle = MathExt::solveQuadraticReal(A, B, C-outerRadSqr);
					std::vector<double> xRangeInnerCircle = MathExt::solveQuadraticReal(A, B, C-innerRadSqr);
					
					for(double& v : xRangeOuterEdgeCircle)
						v = MathExt::clamp<double>(MathExt::round(v), minX, maxX);
					for(double& v : xRangeInnerEdgeCircle)
						v = MathExt::clamp<double>(MathExt::round(v), minX, maxX);
					for(double& v : xRangeOuterCircle)
						v = MathExt::clamp<double>(MathExt::round(v), minX, maxX);
					for(double& v : xRangeInnerCircle)
						v = MathExt::clamp<double>(MathExt::round(v), minX, maxX);
					
					//rules:
					//OuterEdge = very outside edge. Will be antialiased if enabled
					//InnerEdge = inner edge separating the inner circle. Will be antialiased if enabled
					//OuterCircle = Stuff that would normally be fully filled in with no antialiasing
					//InnerCircle = Stuff that would normally be Ignored.
					//Space between outer edge and outer circle is antialiased for sure
					//Space between outer circle and innerEdge is antialiased for sure
					//Everything else is filled fully. Nothing that would be in the inner circle is filled
					
					if(xRangeOuterCircle.size() < 2)
					{
						//very edge. Interpolate all values
						for(int tX = xRangeOuterEdgeCircle[1]; tX < xRangeOuterEdgeCircle[0]; tX++)
						{
							srcPixels[tX + tY*tempWidth] = blend(activeColor, srcPixels[tX + tY*tempWidth]);
						}
					}
					else if(xRangeInnerEdgeCircle.size() < 2)
					{
						//before the inner circle exists. Same as drawing normal circle
						for(int i=xRangeOuterEdgeCircle[1]; i<xRangeOuterEdgeCircle[0]; i++)
						{
							srcPixels[i + tY*tempWidth] = blend(activeColor, srcPixels[i + tY*tempWidth]);
						}
					}
					else
					{
						//fill between the outer and inner edge.
						//before the inner circle exists.
						//draw left side
						for(int i=xRangeOuterEdgeCircle[1]; i<xRangeInnerEdgeCircle[1]; i++)
						{
							srcPixels[i + tY*tempWidth] = blend(activeColor, srcPixels[i + tY*tempWidth]);
						}
						
						if(xRangeInnerCircle.size() < 2)
						{
							for(int tX = xRangeInnerEdgeCircle[1]; tX < xRangeInnerEdgeCircle[0]; tX++)
							{
								srcPixels[tX + tY*tempWidth] = blend(activeColor, srcPixels[tX + tY*tempWidth]);
							}
						}
						else
						{
							for(int tX = xRangeInnerEdgeCircle[1]; tX < xRangeInnerCircle[1]; tX++)
							{
								srcPixels[tX + tY*tempWidth] = blend(activeColor, srcPixels[tX + tY*tempWidth]);
							}

							//draw right side
							for(int tX = xRangeInnerCircle[0]; tX < xRangeInnerEdgeCircle[0]; tX++)
							{
								srcPixels[tX + tY*tempWidth] = blend(activeColor, srcPixels[tX + tY*tempWidth]);
							}
						}

						//draw right side
						for(int tX = xRangeInnerEdgeCircle[0]; tX < xRangeOuterEdgeCircle[0]; tX++)
						{
							srcPixels[tX + tY*tempWidth] = blend(activeColor, srcPixels[tX + tY*tempWidth]);
						}
					}
					
				}
				RESET_LARGE_ENOUGH_CLAUSE()
			}
			else
			{
				LARGE_ENOUGH_CLAUSE(approxArea)
				#pragma omp parallel for
				for(int tY=minY; tY<maxY; tY++)
				{
					//create polynomial to describe distance
					double C = MathExt::sqr(y-tY-0.5) + MathExt::sqr(x)+1;

					std::vector<double> xRangeOuterEdgeCircle = MathExt::solveQuadraticReal(A, B, C-outerRadSqrEdge);
					std::vector<double> xRangeInnerEdgeCircle = MathExt::solveQuadraticReal(A, B, C-innerRadSqrEdge);
					std::vector<double> xRangeOuterCircle = MathExt::solveQuadraticReal(A, B, C-outerRadSqr);
					std::vector<double> xRangeInnerCircle = MathExt::solveQuadraticReal(A, B, C-innerRadSqr);
					
					for(double& v : xRangeOuterEdgeCircle)
						v = MathExt::clamp<double>(MathExt::round(v), minX, maxX);
					for(double& v : xRangeInnerEdgeCircle)
						v = MathExt::clamp<double>(MathExt::round(v), minX, maxX);
					for(double& v : xRangeOuterCircle)
						v = MathExt::clamp<double>(MathExt::round(v), minX, maxX);
					for(double& v : xRangeInnerCircle)
						v = MathExt::clamp<double>(MathExt::round(v), minX, maxX);
					
					//rules:
					//OuterEdge = very outside edge. Will be antialiased if enabled
					//InnerEdge = inner edge separating the inner circle. Will be antialiased if enabled
					//OuterCircle = Stuff that would normally be fully filled in with no antialiasing
					//InnerCircle = Stuff that would normally be Ignored.
					//Space between outer edge and outer circle is antialiased for sure
					//Space between outer circle and innerEdge is antialiased for sure
					//Everything else is filled fully. Nothing that would be in the inner circle is filled
					
					if(xRangeOuterCircle.size() < 2)
					{
						//very edge. Interpolate all values
						for(int tX = xRangeOuterEdgeCircle[1]; tX < xRangeOuterEdgeCircle[0]; tX++)
						{
							Color newBlendColor = activeColor;

							int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
							double maxAddDis = outerRadSqrEdge - outerRadSqr;
							double lerpVal = (double)(outerRadSqrEdge - dis) / maxAddDis;

							if(lerpVal < 0)
								continue;
							if(lerpVal < 1)
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
							
							srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
						}
					}
					else if(xRangeInnerEdgeCircle.size() < 2)
					{
						//before the inner circle exists. Same as drawing normal circle
						//draw left side
						for(int tX = xRangeOuterEdgeCircle[1]; tX < xRangeOuterCircle[1]; tX++)
						{
							Color newBlendColor = activeColor;

							int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
							double maxAddDis = outerRadSqrEdge - outerRadSqr;
							double lerpVal = (double)(outerRadSqrEdge - dis) / maxAddDis;

							if(lerpVal < 0)
								continue;
							if(lerpVal < 1)
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
							
							srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
						}

						//draw right side
						for(int tX = xRangeOuterCircle[0]; tX < xRangeOuterEdgeCircle[0]; tX++)
						{
							Color newBlendColor = activeColor;

							int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
							double maxAddDis = outerRadSqrEdge - outerRadSqr;
							double lerpVal = (double)(outerRadSqrEdge - dis) / maxAddDis;

							if(lerpVal < 0)
								continue;
							if(lerpVal < 1)
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
							
							srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
						}
						
						for(int i=xRangeOuterCircle[1]; i<xRangeOuterCircle[0]; i++)
						{
							srcPixels[i + tY*tempWidth] = blend(activeColor, srcPixels[i + tY*tempWidth]);
						}
					}
					else
					{
						//fill between the outer and inner edge.
						//before the inner circle exists. Same as drawing normal circle
						//draw left side
						for(int tX = xRangeOuterEdgeCircle[1]; tX < xRangeOuterCircle[1]; tX++)
						{
							Color newBlendColor = activeColor;

							int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
							double maxAddDis = outerRadSqrEdge - outerRadSqr;
							double lerpVal = (double)(outerRadSqrEdge-dis) / maxAddDis;

							if(lerpVal < 0)
								continue;
							if(lerpVal < 1)
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
							
							srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
						}
						for(int i=xRangeOuterCircle[1]; i<xRangeInnerEdgeCircle[1]; i++)
						{
							srcPixels[i + tY*tempWidth] = blend(activeColor, srcPixels[i + tY*tempWidth]);
						}
						
						if(xRangeInnerCircle.size() < 2)
						{
							for(int tX = xRangeInnerEdgeCircle[1]; tX < xRangeInnerEdgeCircle[0]; tX++)
							{
								Color newBlendColor = activeColor;

								int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
								double maxAddDis = innerRadSqrEdge - innerRadSqr;
								double lerpVal = (double)(dis - innerRadSqr) / maxAddDis;

								if(lerpVal < 0)
									continue;
								if(lerpVal < 1)
									newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
								
								srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
							}
						}
						else
						{
							for(int tX = xRangeInnerEdgeCircle[1]; tX < xRangeInnerCircle[1]; tX++)
							{
								Color newBlendColor = activeColor;
		
								int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
								double maxAddDis = innerRadSqrEdge - innerRadSqr;
								double lerpVal = (double)(dis - innerRadSqr) / maxAddDis;
		
								if(lerpVal < 0)
									continue;
								if(lerpVal < 1)
									newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
								
								srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
							}

							//draw right side
							for(int tX = xRangeInnerCircle[0]; tX < xRangeInnerEdgeCircle[0]; tX++)
							{
								Color newBlendColor = activeColor;

								int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
								double maxAddDis = innerRadSqrEdge - innerRadSqr;
								double lerpVal = (double)(dis - innerRadSqr) / maxAddDis;

								if(lerpVal < 0)
									continue;
								if(lerpVal < 1)
									newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
								
								srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
							}
						}

						for(int i=xRangeInnerEdgeCircle[0]; i<xRangeOuterCircle[0]; i++)
						{
							srcPixels[i + tY*tempWidth] = blend(activeColor, srcPixels[i + tY*tempWidth]);
						}
						
						//draw right side
						for(int tX = xRangeOuterCircle[0]; tX < xRangeOuterEdgeCircle[0]; tX++)
						{
							Color newBlendColor = activeColor;

							int dis = MathExt::sqr(tY+0.5 - y) + MathExt::sqr(tX+0.5 - x);
							double maxAddDis = outerRadSqrEdge - outerRadSqr;
							double lerpVal = (double)(outerRadSqrEdge - dis) / maxAddDis;

							if(lerpVal < 0)
								continue;
							if(lerpVal < 1)
								newBlendColor.alpha = (unsigned char)(lerpVal*newBlendColor.alpha);
							
							srcPixels[tX + tY*tempWidth] = blend(newBlendColor, srcPixels[tX + tY*tempWidth]);
						}
					}
					
				}
				RESET_LARGE_ENOUGH_CLAUSE()
			}
		}
	}
    #endif

} //NAMESPACE glib END