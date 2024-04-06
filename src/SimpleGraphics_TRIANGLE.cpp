#include "InternalGraphicsHeader.h"

namespace smpl
{
	// void SimpleGraphics::testDrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Image* surf)
	// {
	// 	int currentComposite = compositeRule;
	// 	Image* otherImg;
	// 	if(surf==nullptr)
	// 		return;
	// 	else
	// 		otherImg = surf;
		
	// 	if(otherImg!=nullptr)
	// 	{
	// 		if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
	// 		{
	// 			return;
	// 		}
	// 		int minY = MathExt::min( {y1,y2,y3} );
	// 		int maxY = MathExt::max( {y1,y2,y3} );
	// 		int minX = MathExt::min( {x1,x2,x3} );
	// 		int maxX = MathExt::max( {x1,x2,x3} );

	// 		Line l1 = Line(x1,y1,x2,y2);
	// 		Line l2 = Line(x2,y2,x3,y3);
	// 		Line l3 = Line(x3,y3,x1,y1);
			
	// 		for(int y=minY; y<maxY; y++)
	// 		{
	// 			double xv1 = l1.solveForX(y+0.5);
	// 			double xv2 = l2.solveForX(y+0.5);
	// 			double xv3 = l3.solveForX(y+0.5);

	// 			std::vector<double> solvedVals = {MathExt::round(xv1), MathExt::round(xv2), MathExt::round(xv3)};

	// 			Sort::insertionSort<double>(solvedVals.data(), 3, [](double a, double b) -> bool{
	// 				return a < b;
	// 			});

	// 			int guessedMin = solvedVals[0];
	// 			int guessedMax = solvedVals[1];
				
	// 			for(int x=guessedMin; x<guessedMax; x++)
	// 			{
	// 				drawPixel(x,y,SimpleGraphics::getColor(), surf);
	// 			}
	// 		}
	// 	}
	// }

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

				#if(OPTI>=2)

					int avxWidth = (endX-startX)>>3;
					int remainder = (endX-startX) - (avxWidth<<3);

					Color* startFill = otherImg->getPixels() + startX + y*otherImg->getWidth();
					Color* endFill = startFill + (endX-startX);

					__m256i* avxFill = (__m256i*)startFill;
					__m256i* avxEnd = avxFill + avxWidth;
					__m256i avxColor = _mm256_set1_epi32( *((int*)&SimpleGraphics::activeColor) );

					//fill from cX1 to cX2
					if(currentComposite == NO_COMPOSITE)
					{
						while(avxFill < avxEnd)
						{
							_mm256_storeu_si256(avxFill, avxColor);
							avxFill++;
						}

						//fill remainder
						startFill += avxWidth<<3;
						for(int i=0; i<remainder; i++)
						{
							*startFill = SimpleGraphics::activeColor;
							startFill++;
						}
					}
					else
					{
						while(avxFill < avxEnd)
						{
							__m256i currentColor = _mm256_loadu_si256(avxFill);
							__m256i blendC = blend(avxColor, currentColor);
							_mm256_storeu_si256(avxFill, blendC);
							avxFill++;
						}

						//fill remainder
						startFill += avxWidth<<3;
						for(int i=0; i<remainder; i++)
						{
							*startFill = blend(SimpleGraphics::activeColor, *startFill);
							startFill++;
						}
					}

				#elif(OPTI>=1)

					int sseWidth = (endX-startX)>>2;
					int remainder = (endX-startX) - (sseWidth<<2);

					Color* startFill = otherImg->getPixels() + startX + y*otherImg->getWidth();
					Color* endFill = startFill + (endX-startX);

					__m128i* sseFill = (__m128i*)startFill;
					__m128i* sseEnd = sseFill + sseWidth;
					__m128i sseColor = _mm_set1_epi32( *((int*)&SimpleGraphics::activeColor) );

					//fill from cX1 to cX2
					if(compositeRule == NO_COMPOSITE)
					{
						while(sseFill < sseEnd)
						{
							_mm_storeu_si128(sseFill, sseColor);
							sseFill++;
						}

						//fill remainder
						startFill += sseWidth<<2;
						for(int i=0; i<remainder; i++)
						{
							*startFill = SimpleGraphics::activeColor;
							startFill++;
						}
					}
					else
					{
						while(sseFill < sseEnd)
						{
							__m128i currentColor = _mm_loadu_si128(sseFill);
							__m128i blendC = blend(sseColor, currentColor);
							_mm_storeu_si128(sseFill, blendC);
							sseFill++;
						}

						//fill remainder
						startFill += sseWidth<<2;
						for(int i=0; i<remainder; i++)
						{
							*startFill = blend(SimpleGraphics::activeColor, *startFill);
							startFill++;
						}
					}

				#else

					Color* startFill = otherImg->getPixels() + startX + y*otherImg->getWidth();
					Color* endFill = startFill + (endX-startX);

					//fill from cX1 to cX2
					if(compositeRule == NO_COMPOSITE)
					{
						while(startFill < endFill)
						{
							*startFill = activeColor;
							startFill++;
						}
					}
					else
					{
						while(startFill < endFill)
						{
							Color destColor = *startFill;
							Color finalColor = blend(activeColor, destColor);
							*startFill = finalColor;

							startFill++;
						}
					}

				#endif
			}
			
		}
	}

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

				#if(OPTI>=2)

					int avxWidth = (dis)>>3;
					int remainder = (dis) - (avxWidth<<3);

					__m256i* avxFill = (__m256i*)startFill;
					__m256i* avxEnd = avxFill + avxWidth;
					__m256i avxColor = _mm256_set1_epi32( *((int*)&SimpleGraphics::activeColor) );

					__m256 startWeights1 = _mm256_set1_ps((float)s1);
					__m256 startWeights2 = _mm256_set1_ps((float)s2);
					__m256 startWeights3 = _mm256_set1_ps((float)s3);

					__m256 endWeights1 = _mm256_set1_ps((float)e1);
					__m256 endWeights2 = _mm256_set1_ps((float)e2);
					__m256 endWeights3 = _mm256_set1_ps((float)e3);

					__m256 texCoordsU1 = _mm256_set1_ps( p1.z );
					__m256 texCoordsU2 = _mm256_set1_ps( p2.z );
					__m256 texCoordsU3 = _mm256_set1_ps( p3.z );

					__m256 texCoordsV1 = _mm256_set1_ps( p1.w );
					__m256 texCoordsV2 = _mm256_set1_ps( p2.w );
					__m256 texCoordsV3 = _mm256_set1_ps( p3.w );

					while(avxFill < avxEnd)
					{
						__m256 blendValues = _mm256_set_ps(
							((float)(x+7))/dis,
							((float)(x+6))/dis,
							((float)(x+5))/dis,
							((float)(x+4))/dis,
							((float)(x+3))/dis,
							((float)(x+2))/dis,
							((float)(x+1))/dis,
							((float)x)/dis
						);

						__m256 blendValuesInv = _mm256_sub_ps( _mm256_set1_ps(1.0f), blendValues);
						__m256 uValues, vValues, w1Values, w2Values, w3Values, uInt, vInt;
						
						w1Values = _mm256_add_ps(_mm256_mul_ps( startWeights1, blendValuesInv), _mm256_mul_ps( endWeights1, blendValues));
						w2Values = _mm256_add_ps(_mm256_mul_ps( startWeights2, blendValuesInv), _mm256_mul_ps( endWeights2, blendValues));
						w3Values = _mm256_add_ps(_mm256_mul_ps( startWeights3, blendValuesInv), _mm256_mul_ps( endWeights3, blendValues));

						uValues = _mm256_add_ps(_mm256_mul_ps( texCoordsU1, w1Values), _mm256_mul_ps( texCoordsU2, w2Values));
						uValues = _mm256_add_ps(uValues, _mm256_mul_ps( texCoordsU3, w3Values));

						vValues = _mm256_add_ps(_mm256_mul_ps( texCoordsV1, w1Values), _mm256_mul_ps( texCoordsV2, w2Values));
						vValues = _mm256_add_ps(vValues, _mm256_mul_ps( texCoordsV3, w3Values));

						uValues = _mm256_mul_ps( uValues, _mm256_set1_ps( texture->getWidth()) );
						vValues = _mm256_mul_ps( vValues, _mm256_set1_ps( texture->getHeight()) );
						
						uValues = _mm256_round_ps( uValues, _MM_ROUND_NEAREST );
						vValues = _mm256_round_ps( vValues, _MM_ROUND_NEAREST );
						
						
						uInt = _mm256_cvtps_epi32(uValues);
						vInt = _mm256_cvtps_epi32(vValues);
						
						int* xCoords = (int*)&uInt;
						int* yCoords = (int*)&vInt;
						
						Color c1 = texture->getPixel(xCoords[0], yCoords[0], true);
						Color c2 = texture->getPixel(xCoords[1], yCoords[1], true);
						Color c3 = texture->getPixel(xCoords[2], yCoords[2], true);
						Color c4 = texture->getPixel(xCoords[3], yCoords[3], true);
						Color c5 = texture->getPixel(xCoords[4], yCoords[4], true);
						Color c6 = texture->getPixel(xCoords[5], yCoords[5], true);
						Color c7 = texture->getPixel(xCoords[6], yCoords[6], true);
						Color c8 = texture->getPixel(xCoords[7], yCoords[7], true);
						
						__m256i finalColors = _mm256_set_epi32(
							*(int*)&c8,
							*(int*)&c7,
							*(int*)&c6,
							*(int*)&c5,
							*(int*)&c4,
							*(int*)&c3,
							*(int*)&c2,
							*(int*)&c1
						);

						if(currentComposite == NO_COMPOSITE)
						{
							_mm256_storeu_si256(avxFill, finalColors);
						}
						else
						{
							__m256i currentColor = _mm256_loadu_si256(avxFill);
							__m256i blendC = blend(finalColors, currentColor);
							_mm256_storeu_si256(avxFill, blendC);
						}
						
						avxFill++;
						x+=8;
					}

					startFill += avxWidth<<3;

					//fill remainder
					for(int i=0; i<remainder; i++)
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
				
				#elif(OPTI>=1)

					int sseWidth = (dis)>>2;
					int remainder = (dis) - (sseWidth<<2);

					__m128i* sseFill = (__m128i*)startFill;
					__m128i* sseEnd = sseFill + sseWidth;
					__m128i sseColor = _mm_set1_epi32( *((int*)&SimpleGraphics::activeColor) );

					__m128 startWeights1 = _mm_set1_ps((float)s1);
					__m128 startWeights2 = _mm_set1_ps((float)s2);
					__m128 startWeights3 = _mm_set1_ps((float)s3);

					__m128 endWeights1 = _mm_set1_ps((float)e1);
					__m128 endWeights2 = _mm_set1_ps((float)e2);
					__m128 endWeights3 = _mm_set1_ps((float)e3);

					__m128 texCoordsU1 = _mm_set1_ps( p1.z );
					__m128 texCoordsU2 = _mm_set1_ps( p2.z );
					__m128 texCoordsU3 = _mm_set1_ps( p3.z );

					__m128 texCoordsV1 = _mm_set1_ps( p1.w );
					__m128 texCoordsV2 = _mm_set1_ps( p2.w );
					__m128 texCoordsV3 = _mm_set1_ps( p3.w );


					while(sseFill < sseEnd)
					{
						__m128 blendValues = _mm_set_ps(
							((float)(x+3))/dis,
							((float)(x+2))/dis,
							((float)(x+1))/dis,
							((float)x)/dis
						);

						__m128 blendValuesInv = _mm_sub_ps( _mm_set1_ps(1.0f), blendValues);
						__m128 uValues, vValues, w1Values, w2Values, w3Values, uInt, vInt;
						
						w1Values = _mm_add_ps(_mm_mul_ps( startWeights1, blendValuesInv), _mm_mul_ps( endWeights1, blendValues));
						w2Values = _mm_add_ps(_mm_mul_ps( startWeights2, blendValuesInv), _mm_mul_ps( endWeights2, blendValues));
						w3Values = _mm_add_ps(_mm_mul_ps( startWeights3, blendValuesInv), _mm_mul_ps( endWeights3, blendValues));

						uValues = _mm_add_ps(_mm_mul_ps( texCoordsU1, w1Values), _mm_mul_ps( texCoordsU2, w2Values));
						uValues = _mm_add_ps(uValues, _mm_mul_ps( texCoordsU3, w3Values));

						vValues = _mm_add_ps(_mm_mul_ps( texCoordsV1, w1Values), _mm_mul_ps( texCoordsV2, w2Values));
						vValues = _mm_add_ps(vValues, _mm_mul_ps( texCoordsV3, w3Values));

						uValues = _mm_mul_ps( uValues, _mm_set1_ps( texture->getWidth()) );
						vValues = _mm_mul_ps( vValues, _mm_set1_ps( texture->getHeight()) );

						uValues = _mm_round_ps( uValues, _MM_ROUND_NEAREST );
						vValues = _mm_round_ps( vValues, _MM_ROUND_NEAREST );
						
						
						uInt = _mm_cvtps_epi32(uValues);
						vInt = _mm_cvtps_epi32(vValues);
						
						int* xCoords = (int*)&uInt;
						int* yCoords = (int*)&vInt;
						
						Color c1 = texture->getPixel(xCoords[0], yCoords[0], true);
						Color c2 = texture->getPixel(xCoords[1], yCoords[1], true);
						Color c3 = texture->getPixel(xCoords[2], yCoords[2], true);
						Color c4 = texture->getPixel(xCoords[3], yCoords[3], true);
						
						__m128i finalColors = _mm_set_epi32(
							*(int*)&c4,
							*(int*)&c3,
							*(int*)&c2,
							*(int*)&c1
						);

						if(currentComposite == NO_COMPOSITE)
						{
							_mm_storeu_si128(sseFill, finalColors);
						}
						else
						{
							__m128i currentColor = _mm_loadu_si128(sseFill);
							__m128i blendC = blend(finalColors, currentColor);
							_mm_storeu_si128(sseFill, blendC);
						}
						
						sseFill++;
						x+=4;
					}

					startFill += sseWidth<<2;

					//fill remainder
					for(int i=0; i<remainder; i++)
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
				
				#else

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

				#endif
				
			}

		}
	}

} //NAMESPACE glib END