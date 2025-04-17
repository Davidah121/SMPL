#include "InternalGraphicsHeader.h"

namespace smpl
{
    #if (OPTI == 0)

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

            int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
            int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

            int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
            int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

            int minX = MathExt::min(x, x2);
            int maxX = MathExt::max(x, x2);
            if(minX > maxXBound || maxX < minXBound)
                return;

            int minY = MathExt::min(y, y2);
            int maxY = MathExt::max(y, y2);
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
                int offWidth = maxX - minX;
                int addAmount = (tempWidth - offWidth)-1;

                int tX = 0;

                int actualX = minX;
                int actualY = minY;
                
                LARGE_ENOUGH_CLAUSE(approxArea)
                #pragma omp parallel for
                for(int tY = minY; tY < maxY; tY++)
                {
                    Color* startPoint = &otherImg->getPixels()[minX + tY*otherImg->getWidth()];
                    Color* endPoint = &otherImg->getPixels()[maxX + tY*otherImg->getWidth()];
                    while(startPoint < endPoint)
                    {
                        *startPoint = blend(activeColor, *startPoint);
                        startPoint++;
                    }
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
    #endif
} //NAMESPACE glib END