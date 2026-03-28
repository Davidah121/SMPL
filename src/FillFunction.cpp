#include "SimpleGraphics.h"
#include "SIMD.h"
#include "Algorithms.h"

namespace smpl
{
    
    
    FillFunction::FillFunction()
    {
        
    }
    FillFunction::~FillFunction()
    {

    }
    
    void FillFunction::drawColor(int x, int y, Image* surf)
    {
        //throw exception
    }
    void FillFunction::fillBetween(int x1, int x2, int y, Image* surf)
    {
        //throw exception
    }

    FlatFillFunction::FlatFillFunction(Color fillColor)
    {
        this->fillColor = fillColor;
    }
    FlatFillFunction::~FlatFillFunction()
    {

    }

    void FlatFillFunction::drawColor(int x, int y, Image* surf)
    {
        if(surf == nullptr)
            throw std::runtime_error("nullptr exception in FlatFillFunction::drawColor");
        Color* locationOfOutput = &surf->getPixels()[x + y*surf->getWidth()];
        *locationOfOutput = SimpleGraphics::blend(fillColor, *locationOfOutput);
    }
    void FlatFillFunction::fillBetween(int x1, int x2, int y, Image* surf)
    {
        SimpleGraphics::fillBetween(fillColor, x1, x2, y, surf);
    }

    LinearGradientFillFunction::LinearGradientFillFunction(Vec2f startPoint, Vec2f endPoint, const std::vector<GradientKeyPoint>& keyPoints)
    {
        this->gradientLine = Line(startPoint, endPoint);
        this->keyPoints = keyPoints;
        Sort::mergeSort(this->keyPoints.data(), this->keyPoints.size());
    }
    LinearGradientFillFunction::~LinearGradientFillFunction()
    {

    }

    void LinearGradientFillFunction::drawColor(int x, int y, Image* surf)
    {
        if(surf == nullptr)
            throw std::runtime_error("nullptr exception in LinearGradientFillFunction::drawColor");
        Color* locationOfOutput = &surf->getPixels()[x + y*surf->getWidth()];

        Color actualOutputColor;
        float scalarBlend = (float)gradientLine.projectOntoLineParametric(Vec2f(x, y));

        size_t startIndex=keyPoints.size()-1, endIndex=keyPoints.size()-1;
        for(size_t i=0; i<keyPoints.size(); i++)
        {
            if(scalarBlend < keyPoints[i].keyPoint)
            {
                startIndex = i-1;
                endIndex = i;
                break;
            }
        }
        if(startIndex == SIZE_MAX)
            startIndex = 0;
        
        //lerp between the colors
        if(startIndex != endIndex)
        {
            float newBlendingValue = (scalarBlend - keyPoints[startIndex].keyPoint) / (keyPoints[endIndex].keyPoint - keyPoints[startIndex].keyPoint);
            actualOutputColor = SimpleGraphics::lerp(keyPoints[startIndex].color, keyPoints[endIndex].color, newBlendingValue);
        }
        else
        {
            actualOutputColor = keyPoints[startIndex].color;
        }

        *locationOfOutput = SimpleGraphics::blend(actualOutputColor, *locationOfOutput);
    }
    void LinearGradientFillFunction::fillBetween(int x1, int x2, int y, Image* surf)
    {
        if(surf == nullptr)
            throw std::runtime_error("nullptr exception in LinearGradientFillFunction::drawColor");
        Color* locationOfOutput = &surf->getPixels()[x1 + y*surf->getWidth()];
        
        Vec2f normalizedToPoint = gradientLine.getToPoint().normalize();
        SIMD_FP32 sseY = (float)(y - gradientLine.getPoint1().y);
        SIMD_FP32 ssePX = (float)gradientLine.getPoint1().x;
        SIMD_FP32 sseNormPX = (float)normalizedToPoint.x;
        SIMD_FP32 sseNormPY = (float)normalizedToPoint.y;
        SIMD_FP32 ssePLen = (float)gradientLine.getToPoint().getLength();
        
        float tempArr[32];
        for(size_t k=0; k<SIMD_FP32::SIZE; k++)
        {
            tempArr[k] = x1 + k;
        }
        SIMD_FP32 xValues = SIMD_FP32::load(tempArr) - ssePX;

        //32 floats > AVX-512 and you are already past the point where you should be utilizing a GPU + this is not the library for that.
        float startKeyPoints[32];
        float endKeyPoints[32];
        Color startColors[32];
        Color endColors[32];

        size_t simdBound = SIMD_FP32::getSIMDBound(x2-x1);
        for(size_t k=0; k<simdBound; k+=SIMD_FP32::SIZE)
        {
            SIMD_FP32 projectedValues = (xValues*sseNormPX + sseY*sseNormPY) / ssePLen;
            
            //reuse tempArr
            projectedValues.store(tempArr);
            for(size_t j=0; j<SIMD_FP32::SIZE; j++)
            {
                
                startKeyPoints[j] = keyPoints.back().keyPoint;
                startColors[j] = keyPoints.back().color;
                endKeyPoints[j] = keyPoints.back().keyPoint;
                endColors[j] = keyPoints.back().color;
                for(size_t i=0; i<keyPoints.size(); i++)
                {
                    if(tempArr[j] < keyPoints[i].keyPoint)
                    {
                        startKeyPoints[j] = keyPoints[i-1].keyPoint;
                        startColors[j] = keyPoints[i-1].color;
                        endKeyPoints[j] = keyPoints[i].keyPoint;
                        endColors[j] = keyPoints[i].color;
                        break;
                    }
                }
            }

            SIMD_FP32 sseStartKeyPoints = SIMD_FP32::load(startKeyPoints);
            SIMD_FP32 sseEndKeyPoints = SIMD_FP32::load(endKeyPoints);
            
            SIMD_U32 sseStartColors = SIMD_U32::load((uint32_t*)startColors);
            SIMD_U32 sseEndColors = SIMD_U32::load((uint32_t*)endColors);

            SIMD_FP32 divValues = sseEndKeyPoints - sseStartKeyPoints;
            
            //for now just do this
            SIMD_FP32 mask = (divValues == 0);
            divValues = divValues.blend(1, mask);

            SIMD_FP32 lerpValue = (projectedValues - sseStartKeyPoints) / divValues;
            SIMD_U32 outputColor = SimpleGraphics::lerp(sseStartColors.values, sseEndColors.values, lerpValue.values);
            
            SIMD_U32 locationOfOutputAsSSE = SIMD_U32::load((uint32_t*)locationOfOutput);
            outputColor = SimpleGraphics::blend(outputColor.values, locationOfOutputAsSSE.values);
            outputColor.store((uint32_t*)locationOfOutput);

            locationOfOutput += SIMD_FP32::SIZE;
            xValues += SIMD_FP32::SIZE;
        }

        //leftovers - LAZY APPROACH
        xValues.store(tempArr);
        for(size_t newX = tempArr[0]; newX < x2; newX++)
        {
            drawColor(newX, y, surf);
        }
    }
}