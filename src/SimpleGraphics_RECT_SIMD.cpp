#include "InternalGraphicsHeader.h"

namespace smpl
{

	#if (OPTI > 0)

    //works properly now
    void SimpleGraphics::drawRect(int x, int y, int x2, int y2, bool outline, Image* surf)
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
            int tempWidth = otherImg->getWidth();
            int tempHeight = otherImg->getHeight();

            int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
            int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

            int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
            int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

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
            
            
            if(outline == false)
            {
                GRAPHICS_SIMD_DATATYPE activeColorAsSIMD = COLOR_TO_SIMD(activeColor);
                int stopPoint = minX + GET_GRAPHICS_SIMD_BOUND((maxX-minX));
                if(currentComposite == NO_COMPOSITE)
                {
					#pragma omp parallel for
                    for(int tY = minY; tY <= maxY; tY++)
                    {
                        int tX = minX;
                        Color* startPoint = &otherImg->getPixels()[tX + tY*otherImg->getWidth()];
                        while(tX < stopPoint)
                        {
                            GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)startPoint, activeColorAsSIMD);
                            startPoint += GRAPHICS_INC_AMOUNT;
                            tX += GRAPHICS_INC_AMOUNT;
                        }
                        while(tX <= maxX)
                        {
                            *startPoint = activeColor;
                            startPoint++;
                            tX++;
                        }
                    }
                }
                else
                {
					#pragma omp parallel for
                    for(int tY = minY; tY <= maxY; tY++)
                    {
                        int tX = minX;
                        Color* startPoint = &otherImg->getPixels()[tX + tY*otherImg->getWidth()];
                        while(tX < stopPoint)
                        {
                            GRAPHICS_SIMD_DATATYPE destC = GRAPHICS_SIMD_LOAD((GRAPHICS_SIMD_DATATYPE*)startPoint);
                            GRAPHICS_SIMD_DATATYPE blendC = blend(activeColorAsSIMD, destC);
                            GRAPHICS_SIMD_STORE((GRAPHICS_SIMD_DATATYPE*)startPoint, blendC);
                            startPoint += GRAPHICS_INC_AMOUNT;
                            tX += GRAPHICS_INC_AMOUNT;
                        }
                        while(tX <= maxX)
                        {
                            *startPoint = blend(activeColor, *startPoint);
                            startPoint++;
                            tX++;
                        }
                    }
                }
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