#include "SIMD_Template.h"
#include "SimpleGraphics.h"
#include "SIMD.h"
#include "Algorithms.h"
#include "StringTools.h"
#include "Vec2f.h"
#include <vector>

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
		this->keyPointValues = std::vector<float>(keyPoints.size());
		this->keyPointColors = std::vector<Color>(keyPoints.size());

		std::vector<GradientKeyPoint> tempPoints = keyPoints;
		Sort::mergeSort(tempPoints.data(), tempPoints.size());
		for(size_t i=0; i<keyPoints.size(); i++)
		{
			keyPointValues[i] = tempPoints[i].keyPoint;
			keyPointColors[i] = tempPoints[i].color;
		}
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

		size_t startIndex=keyPointValues.size()-1, endIndex=keyPointValues.size()-1;
		for(size_t i=0; i<keyPointValues.size(); i++)
		{
			if(scalarBlend < keyPointValues[i])
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
			float newBlendingValue = (scalarBlend - keyPointValues[startIndex]) / (keyPointValues[endIndex] - keyPointValues[startIndex]);
			actualOutputColor = SimpleGraphics::lerp(keyPointColors[startIndex], keyPointColors[endIndex], newBlendingValue);
		}
		else
		{
			actualOutputColor = keyPointColors[startIndex];
		}

		*locationOfOutput = SimpleGraphics::blend(actualOutputColor, *locationOfOutput);
	}

	// brute force approach - For each point, project onto the line, find the 2 points its between, lerp between the colors of those 2 points
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

		//32 floats > AVX-512 and you are already past the point where you should be utilizing a GPU instead + this is not the library for that.
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
				
				startKeyPoints[j] = keyPointValues.back();
				startColors[j] = keyPointColors.back();
				endKeyPoints[j] = keyPointValues.back();
				endColors[j] = keyPointColors.back();
				for(size_t i=0; i<keyPointValues.size(); i++)
				{
					if(tempArr[j] < keyPointValues[i])
					{
						startKeyPoints[j] = keyPointValues[i-1];
						startColors[j] = keyPointColors[i-1];
						endKeyPoints[j] = keyPointValues[i];
						endColors[j] = keyPointColors[i];
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

	//Smarter somewhat - Instead of something like "For each x, find the 2 end points to blend between", its "For a group of x, find the first end point".
	//First original approach required solving each x value separately where this one solves multiple at once but it does require going over all blend points.
	//In practice, you'd expect less blend points than x values in one group. There is no exit early so its Ω(xB / SIMD_SIZE) where B = the total blend points.
	//The original approach was O(xB) and θ(x) since it was possible that the gradient always exits early on a particular scanline.
	//This approach also side steps the requirement to project onto the line since the relationship between the projected distances are still linear we just need to convert those keypoints.
	
	// void LinearGradientFillFunction::fillBetween(int x1, int x2, int y, Image* surf)
	// {
	//     if(surf == nullptr)
	//         throw std::runtime_error("nullptr exception in LinearGradientFillFunction::drawColor");

	// 	//given a Y value, determine the x values where you swap between control points

	// 	//Using the start and end points, create a line and at each key point, create a bisecting line and intersect that against the horizontal line at Y.
	// 	//This should be done without SSE
	// 	std::vector<float> xSwapPoints;
		
	// 	//get the bisector line. Needs to be moved to each key point but by default, its at the middle which is 0.5
	// 	Line scanLine = Line(x1, y, x2, y);
	// 	Line biscectorAtZero = gradientLine.getPerpendicularBisector();
	// 	biscectorAtZero = biscectorAtZero - biscectorAtZero.getPoint1();

	// 	for(int i=0; i<keyPointValues.size(); i++)
	// 	{
	// 		Vec2f intersectionPoint;
	// 		Line adjustedLineAtKeyPoint = biscectorAtZero + gradientLine.getToPoint()*keyPointValues[i];
	// 		bool didIntersect = adjustedLineAtKeyPoint.getIntersection(scanLine, intersectionPoint);
	// 		if(didIntersect)
	// 		{
	// 			xSwapPoints.push_back(intersectionPoint.x);
	// 		}
	// 	}

	// 	//AVX-512 compatible. Not Compatible with any larger size SIMD function though you can easily extend for that.
	// 	const float startXValues[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	// 	int iValuesArr[16];

	// 	Color* surfacePixels = surf->getPixels() + x1 + (surf->getWidth()*y);
	// 	SIMD_FP32 xValues = SIMD_FP32::load(startXValues);
	// 	xValues += x1;

	// 	uint64_t simdBound = SIMD_U32::getSIMDBound(x2-x1);
	// 	for(int i=0; i<simdBound; i+=SIMD_U32::SIZE)
	// 	{
	// 		SIMD_32 iValues = 0;
	// 		for(int i=0; i<xSwapPoints.size(); i++)
	// 		{
	// 			SIMD_FP32 blendFactor = xValues > xSwapPoints[i];
	// 			iValues = iValues.blend(i, *(SIMD_32*)&blendFactor); //if statement equivalent.
	// 			//Safe to type pun like this. SIMD_32 and SIMD_FP deal with the exact same underlying data but the function calls are separated by type for convenience. Some of the functions to cast are just type puns
	// 		}
	// 		SIMD_32 nextIValues = iValues+1;
	// 		//need to ensure that we don't go too far forward also so need to do a max
	// 		nextIValues = nextIValues.min(xSwapPoints.size()-1);
			
	// 		SIMD_FP32 keyPointsSIMD = SIMD_FP32::gather(xSwapPoints.data(), iValues.values);
	// 		SIMD_FP32 keyPoints2SIMD = SIMD_FP32::gather(xSwapPoints.data(), nextIValues.values);

	// 		SIMD_FP32 blendValue = (xValues - keyPointsSIMD) / (keyPoints2SIMD - keyPointsSIMD);

	// 		SIMD_U32 keyPoints1ColorSIMD = SIMD_U32::gather((uint32_t*)keyPointColors.data(), iValues.values);
	// 		SIMD_U32 keyPoints2ColorSIMD = SIMD_U32::gather((uint32_t*)keyPointColors.data(), nextIValues.values);

	// 		SIMD_U32 finalColors = SimpleGraphics::lerp(keyPoints1ColorSIMD.values, keyPoints2ColorSIMD.values, blendValue.values);

	// 		SIMD_U32 outputColor = SimpleGraphics::blend(finalColors.values, SIMD_U32::load((uint32_t*)surfacePixels).values);
	// 		outputColor.store((uint32_t*)surfacePixels);
			
	// 		//THESE MUST BE THE SAME SIZE. IF THEY ARE DIFFERENT FOR SOME UNGODLY REASON, USE SOMETHING ELSE
	// 		surfacePixels += SIMD_U32::SIZE;
	// 		xValues += SIMD_FP32::SIZE;
	// 	}

	// 	for(int x=simdBound; x < x2; x++)
	// 	{
	// 		drawColor(x, y, surf); //lazy. fix later
	// 	}
	// }

	RadialGradientFillFunction::RadialGradientFillFunction(Vec2f pos, const std::vector<GradientKeyPoint>& keyPoints)
	{
		if(keyPoints.size() < 2)
			throw std::runtime_error("nullptr exception in RadialGradientFillFunction::RadialGradientFillFunction. Must have 2 keypoints");
		
		this->pos = pos;
		this->keyPointValues = std::vector<float>(keyPoints.size());
		this->keyPointColors = std::vector<Color>(keyPoints.size());

		std::vector<GradientKeyPoint> tempPoints = keyPoints;
		Sort::mergeSort(tempPoints.data(), tempPoints.size());
		
		size_t offset = 0;
		if(tempPoints.front().keyPoint != 0)
		{
			//duplicate first point
			keyPointValues[0] = 0;
			keyPointColors[0] = tempPoints.front().color;
			keyPointValues.push_back(0);
			keyPointColors.push_back({0, 0, 0, 0});
			offset=1;
		}

		for(size_t i=0; i<tempPoints.size(); i++)
		{
			keyPointValues[i+offset] = tempPoints[i].keyPoint;
			keyPointColors[i+offset] = tempPoints[i].color;
		}
	}
	RadialGradientFillFunction::~RadialGradientFillFunction()
	{

	}

	void RadialGradientFillFunction::drawColor(int x, int y, Image* surf)
	{
		if(surf == nullptr)
			throw std::runtime_error("nullptr exception in RadialGradientFillFunction::drawColor");
		Color* locationOfOutput = &surf->getPixels()[x + y*surf->getWidth()];

		//measure the distance of (x,y) from the center of gradient. Select the 2 colors to blend between
		float distance = (Vec2f(x, y) - pos).getLength();

		int startIndex = keyPointValues.size();
		int endIndex = keyPointValues.size();
		//should binary search but I'm lazy
		for(size_t i=1; i<keyPointValues.size(); i++)
		{
			if(distance < keyPointValues[i])
			{
				startIndex = i-1;
				endIndex = i;
				break;
			}
		}

		if(startIndex == keyPointValues.size())
		{
			*locationOfOutput = SimpleGraphics::blend(keyPointColors.back(), *locationOfOutput);
			return;
		}

		float lerpValue = (distance - keyPointValues[startIndex]) / (keyPointValues[endIndex] - keyPointValues[startIndex]);
		Color fillColor = SimpleGraphics::lerp(keyPointColors[startIndex], keyPointColors[endIndex], lerpValue);
		*locationOfOutput = SimpleGraphics::blend(fillColor, *locationOfOutput);
	}
	// void RadialGradientFillFunction::fillBetween(int x1, int x2, int y, Image* surf)
	// {
	// 	for(int i=x1; i < x2; i++)
	// 	{
	// 		drawColor(i, y, surf);
	// 	}
	//     // SimpleGraphics::fillBetween(fillColor, x1, x2, y, surf);
	// }
	
	void RadialGradientFillFunction::fillBetween(int x1, int x2, int y, Image* surf)
	{
		if(surf == nullptr)
			throw std::runtime_error("nullptr exception in RadialGradientFillFunction::fillBetween");

		const float tempXArr[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
		SIMD_FP32 xValues = SIMD_FP32::load(tempXArr) + 0.5;
		SIMD_FP32 yValues = y+0.5;

		SIMD_FP32 circleX = pos.x;
		SIMD_FP32 circleY = pos.y;
		
		Color* surfacePixels = surf->getPixels() + x1 + (surf->getWidth()*y);
		Color* maskedPixels = SimpleGraphics::getMask().getPixels() + x1 + (SimpleGraphics::getMask().getWidth()*y);

		for(int i=0; i<SIMD_FP32::getSIMDBound(x2-x1); i+=SIMD_FP32::SIZE)
		{
			SIMD_FP32 xDiff = (xValues-circleX);
			SIMD_FP32 yDiff = (yValues-circleY);
			SIMD_FP32 distance = SEML::sqrt((xDiff*xDiff + yDiff*yDiff).values);

			//find the indicies needed
			SIMD_U32 endIndicies = keyPointValues.size();
			for(int j=0; j<keyPointValues.size(); j++)
			{
				SIMD_FP32 kpv = keyPointValues[j];
				SIMD_FP32 compareValue = distance >= kpv;
				endIndicies = endIndicies.blend(j+1, *(SIMD_U32*)&compareValue);
			}
			
			//clamp to valid index values
			endIndicies = endIndicies.min(keyPointValues.size()-1);
			SIMD_U32 startIndicies = endIndicies-1;

			//color lerp and all that
			SIMD_FP32 keyPointsSIMD = SIMD_FP32::gather(keyPointValues.data(), startIndicies.values);
			SIMD_FP32 keyPoints2SIMD = SIMD_FP32::gather(keyPointValues.data(), endIndicies.values);
			
			//for when distance exceeds the last keypoint. Clamps it back to valid location
			distance = distance.min(keyPoints2SIMD);
			
			SIMD_FP32 blendValue = (distance - keyPointsSIMD) / (keyPoints2SIMD - keyPointsSIMD);

			SIMD_U32 keyPoints1ColorSIMD = SIMD_U32::gather((uint32_t*)keyPointColors.data(), startIndicies.values);
			SIMD_U32 keyPoints2ColorSIMD = SIMD_U32::gather((uint32_t*)keyPointColors.data(), endIndicies.values);

			SIMD_U32 finalColors = SimpleGraphics::lerp(keyPoints1ColorSIMD.values, keyPoints2ColorSIMD.values, blendValue.values);
			SIMD_U32 maskedColors = SIMD_U32::load((uint32_t*)maskedPixels);

			finalColors = SimpleGraphics::maskPixel(finalColors.values, maskedColors.values);
			SIMD_U32 outputColor = SimpleGraphics::blend(finalColors.values, SIMD_U32::load((uint32_t*)surfacePixels).values);
			outputColor.store((uint32_t*)surfacePixels);

			maskedPixels += SIMD_U32::SIZE;
			surfacePixels += SIMD_U32::SIZE;
			xValues += SIMD_FP32::SIZE;
		}
	}
}