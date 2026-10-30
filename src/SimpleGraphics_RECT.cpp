#include "Concurrency.h"
#include "InternalGraphicsHeader.h"
#include "SimpleJobQueue.h"
#include "StringTools.h"

namespace smpl
{
    //works properly now
    void SimpleGraphics::drawRect(int x, int y, int width, int height, bool outline, Image* surf)
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
            
            int x2 = x+width;
            int y2 = y+height;
            
            int tempWidth = otherImg->getWidth();
            int tempHeight = otherImg->getHeight();

            int minXBound = max(0, (int)clippingRect.getLeftBound());
            int minYBound = max(0, (int)clippingRect.getTopBound());

            int maxXBound = min(tempWidth, (int)clippingRect.getRightBound());
            int maxYBound = min(tempHeight, (int)clippingRect.getBottomBound());

            int minX = min(x, x2);
            int maxX = max(x, x2);
            if(minX > maxXBound || maxX < minXBound)
                return;

            int minY = min(y, y2);
            int maxY = max(y, y2);
            if(minY > maxYBound || maxY < minYBound)
                return;
            
            minX = MathExt::clamp(minX, minXBound, maxXBound);
            maxX = MathExt::clamp(maxX, minXBound, maxXBound);
            minY = MathExt::clamp(minY, minYBound, maxYBound);
            maxY = MathExt::clamp(maxY, minYBound, maxYBound);
            
            if(minX==maxX && minY==maxY)
                return;
            
            
            int approxArea = (maxX-minX) * (maxY-minY);
            if(outline == false)
            {
                SIMD_U32 activeColorAsSIMD = activeColor.toUInt();
                int stopPoint = minX + SIMD_U32::getSIMDBound((maxX-minX));
                
				// SJQ_OMP_Replacement::parallelize([minX, otherImg, activeColorAsSIMD, maxX](size_t tStart, size_t tEnd, size_t incr)->void{
				// 	for(int tY = tStart; tY < tEnd; tY+=incr)
				// 	{
				// 		//should've been doing this
				// 		SimpleGraphics::fillBetween(activeColorAsSIMD, minX, maxX, tY, otherImg);
				// 	}
				// }, minY, maxY, 1, 24);
				
				SET_OMP_THREADS(OMP_DESIRED_THREADS(maxY-minY, OMP_HEAVY_TASK));
                #pragma omp parallel for
                for(int tY = minY; tY < maxY; tY++)
                {
					//should've been doing this
					SimpleGraphics::fillBetween(activeColorAsSIMD, minX, maxX, tY, otherImg);
                }
                RESET_LARGE_ENOUGH_CLAUSE()
            }
            else
            {
                //horizontal lines
                drawLine(x, y, x2, y, otherImg);
                drawLine(x, y2, x2, y2, otherImg);

                //vertical lines
                drawLine(x, y, x, y2, otherImg);
                drawLine(x2, y, x2, y2, otherImg);
            }
        }
    }
    
} //NAMESPACE glib END