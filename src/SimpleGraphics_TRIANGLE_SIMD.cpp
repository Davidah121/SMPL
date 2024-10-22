#include "InternalGraphicsHeader.h"

namespace smpl
{
	#if (OPTI > 0)
	void SimpleGraphics::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool outline, Image* surf)
	{
		int currentComposite = compositeRule;
		Image* otherImg;
		if(surf==nullptr)
			return;
		else
			otherImg = surf;
		
		if(otherImg!=nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}

			if(outline)
			{
				drawLine(x1,y1,x2,y2,surf);
				drawLine(x2,y2,x3,y3,surf);
				drawLine(x3,y3,x1,y1,surf);
				return;
			}

			Line l1 = Line(x1, y1, x2, y2);
			Line l2 = Line(x2, y2, x3, y3);
			Line l3 = Line(x3, y3, x1, y1);
			
			int minY = MathExt::min( {y1,y2,y3} );
			int maxY = MathExt::max( {y1,y2,y3} );
			int minX = MathExt::min( {x1,x2,x3} );
			int maxX = MathExt::max( {x1,x2,x3} );

			minY = MathExt::clamp(minY, (int)SimpleGraphics::getClippingRect().getTopBound(), (int)SimpleGraphics::getClippingRect().getBottomBound());
			maxY = MathExt::clamp(maxY, (int)SimpleGraphics::getClippingRect().getTopBound(), (int)SimpleGraphics::getClippingRect().getBottomBound());
			
			minX = MathExt::clamp(minX, (int)SimpleGraphics::getClippingRect().getLeftBound(), (int)SimpleGraphics::getClippingRect().getRightBound());
			maxX = MathExt::clamp(maxX, (int)SimpleGraphics::getClippingRect().getLeftBound(), (int)SimpleGraphics::getClippingRect().getRightBound());
			
			SIMD_U8 activeColorAsSIMD = COLOR_TO_SIMD(activeColor);
			
			#pragma omp parallel for
			for(int y=minY; y<maxY; y++)
			{
				double xv1 = l1.solveForX(y+0.5);
				double xv2 = l2.solveForX(y+0.5);
				double xv3 = l3.solveForX(y+0.5);

				std::vector<int> solvedVals = {(int)MathExt::round(xv1), (int)MathExt::round(xv2), (int)MathExt::round(xv3)};

				Sort::insertionSort<int>(solvedVals.data(), 3);

				int startX = solvedVals[0];
				int endX = solvedVals[1];

				startX = MathExt::clamp( startX, minX, maxX);
				endX = MathExt::clamp( endX, minX, maxX);

				Color* startFill = otherImg->getPixels() + startX + y*otherImg->getWidth();
				Color* endFill = startFill + (endX-startX);

				//fill from cX1 to cX2
				if(compositeRule == NO_COMPOSITE)
				{
					int stopPoint = SIMD_U8::getSIMDBound((endX-startX));
					for(int i=0; i<stopPoint; i+=SIMD_U8::SIZE)
					{
						activeColorAsSIMD.store((unsigned char*)startFill);
						startFill += SIMD_U8::SIZE;
					}
					while(startFill < endFill)
					{
						*startFill = activeColor;
						startFill++;
					}
				}
				else
				{
					int stopPoint = SIMD_U8::getSIMDBound((endX-startX));
					for(int i=0; i<stopPoint; i+=SIMD_U8::SIZE)
					{
						SIMD_U8 destC = SIMD_U8::load((unsigned char*)startFill);
						SIMD_U8 blendC = blend(activeColorAsSIMD.values, destC.values);
						blendC.store((unsigned char*)startFill);
						startFill += SIMD_U8::SIZE;
					}
					while(startFill < endFill)
					{
						Color destColor = *startFill;
						*startFill = blend(activeColor, destColor);
						startFill++;
					}
				}
			}
			
		}
	}

	//TODO: MAKE SIMD. Same problem as before with DrawSprite. Lerp, etc. are not SIMD
	void SimpleGraphics::drawTexturedTriangle(Vec4f p1, Vec4f p2, Vec4f p3, Image* texture, Image* surf)
	{
		int currentComposite = compositeRule;
		Image* otherImg;
		if(surf==nullptr)
			return;
		else
			otherImg = surf;
		
		if(otherImg!=nullptr && texture!=nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}

			Line l1 = Line(p1.x, p1.y, p2.x, p2.y);
			Line l2 = Line(p2.x, p2.y, p3.x, p3.y);
			Line l3 = Line(p3.x, p3.y, p1.x, p1.y);
			
			int minY = (int)MathExt::min( {p1.y,p2.y,p3.y} );
			int maxY = (int)MathExt::max( {p1.y,p2.y,p3.y} );
			int minX = (int)MathExt::min( {p1.x,p2.x,p3.x} );
			int maxX = (int)MathExt::max( {p1.x,p2.x,p3.x} );

			minY = MathExt::clamp(minY, (int)SimpleGraphics::getClippingRect().getTopBound(), (int)SimpleGraphics::getClippingRect().getBottomBound());
			maxY = MathExt::clamp(maxY, (int)SimpleGraphics::getClippingRect().getTopBound(), (int)SimpleGraphics::getClippingRect().getBottomBound());
			
			minX = MathExt::clamp(minX, (int)SimpleGraphics::getClippingRect().getLeftBound(), (int)SimpleGraphics::getClippingRect().getRightBound());
			maxX = MathExt::clamp(maxX, (int)SimpleGraphics::getClippingRect().getLeftBound(), (int)SimpleGraphics::getClippingRect().getRightBound());
			
			double det = (p2.y-p3.y)*(p1.x-p3.x) + (p3.x-p2.x)*(p1.y-p3.y);

			#pragma omp parallel for
			for(int y=minY; y<maxY; y++)
			{
				double xv1 = l1.solveForX(y+0.5);
				double xv2 = l2.solveForX(y+0.5);
				double xv3 = l3.solveForX(y+0.5);

				std::vector<int> solvedVals = {(int)MathExt::round(xv1), (int)MathExt::round(xv2), (int)MathExt::round(xv3)};

				Sort::insertionSort<int>(solvedVals.data(), 3, [](int a, int b) -> bool{
					return a < b;
				});

				int startX = solvedVals[0];
				int endX = solvedVals[1];

				startX = MathExt::clamp( startX, minX, maxX);
				endX = MathExt::clamp( endX, minX, maxX);

				if(startX > endX)
				{
					//outside the drawing area
					continue;
				}

				if(startX == endX)
				{
					//draw one pixel
					double w1 = ((p2.y-p3.y)*(startX-p3.x) + (p3.x-p2.x)*(y-p3.y)) / det;
					double w2 = ((p3.y-p1.y)*(startX-p3.x) + (p1.x-p3.x)*(y-p3.y)) / det;
					double w3 = 1.0 - w1 - w2;

					double u = p1.z*w1 + p2.z*w2 + p3.z*w3;
					double v = p1.w*w1 + p2.w*w2 + p3.w*w3;
					
					int uInt = (int) MathExt::round(u * (texture->getWidth()));
					int vInt = (int) MathExt::round(v * (texture->getHeight()));

					Color c = texture->getPixel(uInt, vInt, false);

					if(compositeRule==NO_COMPOSITE)
					{
						otherImg->setPixel(startX, y, c);
					}
					else
					{
						Color destColor = otherImg->getPixel(startX, y);
						Color finalColor = blend(c, destColor);

						otherImg->setPixel(startX, y, finalColor);
					}

					continue;
				}

				Color* startFill = otherImg->getPixels() + startX + y*otherImg->getWidth();
				Color* endFill = startFill + (endX-startX);
				
				double s1 = ((p2.y-p3.y)*(startX-p3.x) + (p3.x-p2.x)*(y-p3.y)) / det;
				double s2 = ((p3.y-p1.y)*(startX-p3.x) + (p1.x-p3.x)*(y-p3.y)) / det;
				double s3 = 1.0 - s1 - s2;

				double e1 = ((p2.y-p3.y)*(endX-p3.x) + (p3.x-p2.x)*(y-p3.y)) / det;
				double e2 = ((p3.y-p1.y)*(endX-p3.x) + (p1.x-p3.x)*(y-p3.y)) / det;
				double e3 = 1.0 - e1 - e2;


				int x = 0;
				int dis = (endX - startX);

				while(startFill < endFill)
				{
					double blendValue = (double)x / dis;

					double w1 = MathExt::lerp(s1, e1, blendValue);
					double w2 = MathExt::lerp(s2, e2, blendValue);
					double w3 = MathExt::lerp(s3, e3, blendValue);

					double u = p1.z*w1 + p2.z*w2 + p3.z*w3;
					double v = p1.w*w1 + p2.w*w2 + p3.w*w3;
					
					int uInt = (int) MathExt::round(u * (texture->getWidth()));
					int vInt = (int) MathExt::round(v * (texture->getHeight()));

					Color c = texture->getPixel(uInt, vInt, true);
					
					if(currentComposite == NO_COMPOSITE)
					{
						*startFill = c;
					}
					else
					{
						*startFill = blend(c, *startFill);
					}
					
					startFill++;
					x++;
				}
				
			}

		}
	}
	#endif
} //NAMESPACE glib END