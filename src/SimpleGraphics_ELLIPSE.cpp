#include "InternalGraphicsHeader.h"

namespace smpl
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

			int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x-absXRad, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+absXRad, minXBound, maxXBound);

			int minY = MathExt::clamp(y-absYRad, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+absYRad, minYBound, maxYBound);

			double xrSqr = MathExt::sqr(absXRad-1);
			double xrSqrOuter = MathExt::sqr(absXRad);
			
			double yrSqr = MathExt::sqr(absYRad-1);
			double yrSqrOuter = MathExt::sqr(absYRad);
			
			if(absXRad == 0 || absYRad == 0)
				return;
			
			Color* surfPixels = surf->getPixels();
			//x^2/xRad^2 + y^2/yRad^2 = 1

			if(!antiAliasing)
			{
				for(int tY=minY; tY<maxY; tY++)
				{
					int startX, endX;
					//add offsets back in later
					float B = 1.0 - (float)MathExt::sqr(y-tY-0.5) / yrSqrOuter;
					float v = absXRad * MathExt::sqrt(B);
					
					startX = (int)MathExt::round(x-v);
					endX = (int)MathExt::round(x+v);

					startX = MathExt::clamp(startX, minX, maxX);
					endX = MathExt::clamp(endX, minX, maxX);
					
					for(int tX=startX; tX<endX; tX++)
					{
						surfPixels[tX + tY*surf->getWidth()] = blend(activeColor, surfPixels[tX + tY*surf->getWidth()]);
					}
				}
			}
			else
			{
				for(int tY=minY; tY<maxY; tY++)
				{
					int x1, x2, x3, x4;
					float B1, B2, v1, v2;
					
					//add offsets back in later
					B2 = 1.0 - (float)MathExt::sqr(y-tY-0.5) / yrSqrOuter;
					v2 = absXRad * MathExt::sqrt(B2);

					B1 = 1.0 - (float)MathExt::sqr(y-tY-0.5) / yrSqr;
					if(B1 >= 0)
						v1 = (absXRad-1) * MathExt::sqrt(B1);

					x1 = (int)MathExt::round(x-v2);
					x2 = (int)MathExt::round(x-v1);
					x3 = (int)MathExt::round(x+v1);
					x4 = (int)MathExt::round(x+v2);

					x1 = MathExt::clamp( x1, minX, maxX);
					x2 = MathExt::clamp( x2, minX, maxX);
					x3 = MathExt::clamp( x3, minX, maxX);
					x4 = MathExt::clamp( x4, minX, maxX);

					if(B1 < 0)
					{
						for(int tX=x1; tX<x4; tX++)
						{
							Color c = activeColor;

							Vec2f p = Vec2f(tX+0.5 - x, tY+0.5 - y);
							double dis = p.getLength();
							Vec2f pNorm = p / dis;
							double A = MathExt::sqr(pNorm.x)/xrSqrOuter;
							double B = MathExt::sqr(pNorm.y)/yrSqrOuter;
							double maxDisAllowed = MathExt::sqrt( 1.0 / (A+B));

							double lerpVal = maxDisAllowed - dis;

							if(lerpVal <= 0)
								continue;
							if(lerpVal < 1)
								c.alpha = (unsigned char)(lerpVal*c.alpha);
							
							surfPixels[tX + tY*surf->getWidth()] = blend(activeColor, surfPixels[tX + tY*surf->getWidth()]);
						}
						continue;
					}
					
					
					for(int tX=x1; tX<x2; tX++)
					{
						Color c = activeColor;

						Vec2f p = Vec2f(tX+0.5 - x, tY+0.5 - y);
						double dis = p.getLength();
						Vec2f pNorm = p / dis;
						double A = MathExt::sqr(pNorm.x)/xrSqrOuter;
						double B = MathExt::sqr(pNorm.y)/yrSqrOuter;
						double maxDisAllowed = MathExt::sqrt( 1.0 / (A+B));

						double lerpVal = maxDisAllowed - dis;

						if(lerpVal <= 0)
							continue;
						if(lerpVal < 1)
							c.alpha = (unsigned char)(lerpVal*c.alpha);
						
						surfPixels[tX + tY*surf->getWidth()] = blend(activeColor, surfPixels[tX + tY*surf->getWidth()]);
					}

					//middle
					for(int tX=x2; tX<x3; tX++)
					{
						surfPixels[tX + tY*surf->getWidth()] = blend(activeColor, surfPixels[tX + tY*surf->getWidth()]);
					}

					//right side
					for(int tX=x3; tX<x4; tX++)
					{
						Color c = activeColor;

						Vec2f p = Vec2f(tX+0.5 - x, tY+0.5 - y);
						double dis = p.getLength();
						Vec2f pNorm = p / dis;
						double A = MathExt::sqr(pNorm.x)/xrSqrOuter;
						double B = MathExt::sqr(pNorm.y)/yrSqrOuter;
						double maxDisAllowed = MathExt::sqrt( 1.0 / (A+B));

						double lerpVal = maxDisAllowed - dis;

						if(lerpVal <= 0)
							continue;
						if(lerpVal < 1)
							c.alpha = (unsigned char)(lerpVal*c.alpha);
						
						surfPixels[tX + tY*surf->getWidth()] = blend(activeColor, surfPixels[tX + tY*surf->getWidth()]);
					}
				}
			}
		}
	}

} //NAMESPACE glib END