#include "InternalGraphicsHeader.h"

namespace smpl
{

    #pragma region IMAGE_GRADIENTS

		void SimpleGraphics::drawLinearGradient(Color c1, Color c2, Vec2f startPoint, Vec2f endPoint, Image* surf)
		{
			if(!surf)
				return;

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(surf->getWidth()-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(surf->getHeight()-1, (int)clippingRect.getBottomBound());
			
			//vector projection
			//Normalize dirVec to reduce needed work per pixel
			Vec2f dirVec = endPoint - startPoint;
			double maxDis = dirVec.getLength();
			dirVec = dirVec.normalize();
			for(int y=minYBound; y<maxYBound; y++)
			{
				Vec2f currPos = Vec2f(0, y);
				for(int x=minXBound; x<maxXBound; x++)
				{
					currPos.x = x;
					double perc = currPos.dot(dirVec) / maxDis;
					Color nColor = SimpleGraphics::lerp(c1, c2, perc);
					SimpleGraphics::drawPixel(x, y, nColor, surf);
				}
			}

			//Can be potentially faster if calc3 partial derivatives are used.
		}

		void SimpleGraphics::drawLinearGradient(std::vector<Color> blendColors, std::vector<Vec2f> blendPoints, Image* surf)
		{
			if(!surf)
				return;

			if(blendColors.size() != blendPoints.size())
				return;
			
			if(blendColors.size() < 2)
				return;
			
			for(size_t i=0; i<blendColors.size()-1; i++)
			{
				SimpleGraphics::drawLinearGradient(blendColors[i], blendColors[i+1], blendPoints[i], blendPoints[i+1], surf);
			}
		}

		void SimpleGraphics::drawRadialGradient(Color c1, Color c2, Vec2f midPoint, double radius, Image* surf)
		{
			if(!surf)
				return;
			double rAbs = MathExt::abs(radius);
			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(surf->getWidth()-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(surf->getHeight()-1, (int)clippingRect.getBottomBound());
			
			//distance
			for(int y=minYBound; y<maxYBound; y++)
			{
				Vec2f currPos = Vec2f(0, y);
				for(int x=minXBound; x<maxXBound; x++)
				{
					currPos.x = x;
					double perc = MathExt::distanceTo(midPoint, currPos) / rAbs;
					Color nColor = SimpleGraphics::lerp(c1, c2, perc);
					SimpleGraphics::drawPixel(x, y, nColor, surf);
				}
			}
		}
		
	#pragma endregion

} //NAMESPACE glib END