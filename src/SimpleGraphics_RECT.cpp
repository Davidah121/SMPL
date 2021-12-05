#include "InternalGraphicsHeader.h"

namespace glib
{

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

            int minX = MathExt::clamp(MathExt::min(x, x2), minXBound, maxXBound);
            int maxX = MathExt::clamp(MathExt::max(x, x2), minXBound, maxXBound);

            int minY = MathExt::clamp(MathExt::min(y, y2), minYBound, maxYBound);
            int maxY = MathExt::clamp(MathExt::max(y, y2), minYBound, maxYBound);

            if(outline == false)
            {
                #if(OPTI>=2)

                    __m256i* avxPoint;
                    
                    Color* startPoint = (otherImg->getPixels() + minX + (minY * tempWidth));
                    Color* endPoint = (otherImg->getPixels() + maxX + (maxY * tempWidth));
                    
                    //int startOffset = (maxX - minX) % 4;
                    
                    int offWidth = (1+maxX - minX)>>3;
                    int remainder = (1+maxX - minX) - (offWidth<<3);
                    int addAmount = (tempWidth - (maxX-minX)-1);

                    int tX = 0;

                    int actualX = minX;
                    int actualY = minY;

                    __m256i avxColor = _mm256_set1_epi32( *((int*)&activeColor) );
                    
                    if(currentComposite == NO_COMPOSITE)
                    {
                        while (startPoint < endPoint)
                        {
                            avxPoint = (__m256i*)startPoint;
                            for(int i=0; i<offWidth; i++)
                            {
                                _mm256_storeu_si256(avxPoint, avxColor);
                                avxPoint++;
                            }

                            //fill remainder
                            startPoint += offWidth<<3;

                            for(int i=0; i<remainder; i++)
                            {
                                *startPoint = activeColor;
                                startPoint++;
                            }

                            startPoint += addAmount;
                        }
                    }
                    else
                    {
                        while (startPoint < endPoint)
                        {
                            avxPoint = (__m256i*)startPoint;
                            for(int i=0; i<offWidth; i++)
                            {
                                __m256i currentColors = _mm256_loadu_si256(avxPoint);
                                __m256i blendC = blend(avxColor, currentColors);
                                _mm256_storeu_si256(avxPoint, blendC );
                                avxPoint++;
                            }

                            //fill remainder
                            startPoint += offWidth<<3;

                            for(int i=0; i<remainder; i++)
                            {
                                *startPoint = blend(activeColor, *startPoint);
                                startPoint++;
                            }

                            startPoint += addAmount;
                        }
                        
                    }

                #elif (OPTI>=1)

                    __m128i* ssePoint;
                    
                    Color* startPoint = (otherImg->getPixels() + minX + (minY * tempWidth));
                    Color* endPoint = (otherImg->getPixels() + maxX + (maxY * tempWidth));
                    
                    //int startOffset = (maxX - minX) % 4;
                    
                    int offWidth = (1+maxX - minX)>>2;
                    int remainder = (1+maxX - minX) - (offWidth<<2);
                    int addAmount = (tempWidth - (maxX-minX)-1);

                    int tX = 0;

                    int actualX = minX;
                    int actualY = minY;

                    __m128i sseColor = _mm_set1_epi32( *((int*)&activeColor) );
                    
                    if(currentComposite == NO_COMPOSITE)
                    {
                        while (startPoint < endPoint)
                        {
                            ssePoint = (__m128i*)startPoint;
                            for(int i=0; i<offWidth; i++)
                            {
                                _mm_storeu_si128(ssePoint, sseColor);
                                ssePoint++;
                            }

                            //fill remainder
                            startPoint += offWidth<<2;

                            for(int i=0; i<remainder; i++)
                            {
                                *startPoint = activeColor;
                                startPoint++;
                            }

                            startPoint += addAmount;
                        }
                    }
                    else
                    {
                        while (startPoint < endPoint)
                        {
                            ssePoint = (__m128i*)startPoint;
                            for(int i=0; i<offWidth; i++)
                            {
                                __m128i currentColors = _mm_loadu_si128(ssePoint);
                                __m128i blendC = blend(sseColor, currentColors);
                                _mm_storeu_si128(ssePoint, blendC );
                                ssePoint++;
                            }

                            //fill remainder
                            startPoint += offWidth<<2;

                            for(int i=0; i<remainder; i++)
                            {
                                *startPoint = blend(activeColor, *startPoint);
                                startPoint++;
                            }

                            startPoint += addAmount;
                        }
                        
                    }
                    
                #else

                    Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
                    Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);
                    
                    int offWidth = maxX - minX;
                    int addAmount = (tempWidth - offWidth)-1;

                    int tX = 0;

                    int actualX = minX;
                    int actualY = minY;
                    
                    if(currentComposite == NO_COMPOSITE)
                    {
                        while (startPoint < endPoint)
                        {
                            for(int i=0; i<=offWidth; i++)
                            {
                                *startPoint = activeColor;
                                startPoint++;
                            }
                            startPoint += addAmount;
                        }
                    }
                    else
                    {
                        while (startPoint < endPoint)
                        {
                            for(int i=0; i<=offWidth; i++)
                            {
                                *startPoint = blend(activeColor, *startPoint);
                                startPoint++;
                            }
                            startPoint += addAmount;
                        }
                    }

                #endif
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