#include "InternalGraphicsHeader.h"

namespace glib
{
    // void Graphics::drawLine(int x1, int y1, int x2, int y2, Image* surf)
	// {
	// 	int currentComposite = compositeRule;
	// 	//Will pointers for modifying the data
	// 	Image* otherImg;
	// 	if (surf == nullptr)
	// 		return;
	// 	else
	// 		otherImg = surf;

	// 	if (otherImg != nullptr)
	// 	{
	// 		if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
	// 		{
	// 			return;
	// 		}
	// 		//determine if it is on the line
	// 		//Using general equation
	// 		//Ax + By = C
	// 		//In order to avoid holes, swap between solving for x or y depending
	// 		//on which has a greater rate of change. AntiAliasing can be added easily
	// 		//in this case.
	// 		int P1 = y1-y2;
	// 		int P2 = x2-x1;
	// 		int con = x1*y2 - x2*y1;

	// 		if(P1!=0 && P2!=0)
	// 		{
	// 			if(MathExt::abs(P1) >= MathExt::abs(P2))
	// 			{
	// 				//VERTICAL BASED
	// 				int minY, maxY;
	// 				bool dir = (P1>0 || P2>0);

	// 				int tempWidth = otherImg->getWidth()-1;
	// 				int tempHeight = otherImg->getHeight()-1;

	// 				int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
	// 				int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

	// 				int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
	// 				int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

	// 				double offsetVal = (dir) ? -0.5 : 0.5;

	// 				if(y1 <= y2)
	// 				{
	// 					minY = MathExt::clamp(y1, minYBound, maxYBound);
	// 					maxY = MathExt::clamp(y2, minYBound, maxYBound);
	// 				}
	// 				else
	// 				{
	// 					minY = MathExt::clamp(y2, minYBound, maxYBound);
	// 					maxY = MathExt::clamp(y1, minYBound, maxYBound);
	// 				}

	// 				if(currentComposite == NO_COMPOSITE)
	// 				{
						
	// 					for(int i=minY; i<=maxY; i++)
	// 					{
	// 						//solve with respect to x
	// 						double val = -(con+P2*(i+offsetVal)) / (double)P1;
	// 						int actualX = MathExt::round(val);
							
	// 						if(actualX >= minXBound && actualX <= maxXBound)
	// 							otherImg->setPixel(actualX, i, Graphics::activeColor); //change later
	// 					}
	// 				}
	// 				else
	// 				{
	// 					for(int i=minY; i<=maxY; i++)
	// 					{
	// 						//solve with respect to x
	// 						double val = -(con+P2*(i+offsetVal)) / (double)P1;
	// 						int actualX = MathExt::round(val);
	// 						if(actualX >= minXBound && actualX <= maxXBound)
	// 							otherImg->drawPixel(actualX, i, Graphics::activeColor); //change later
	// 					}
	// 				}
	// 			}
	// 			else
	// 			{
	// 				//HORIZONTAL BASED
	// 				int minX, maxX;
	// 				bool dir = (P1>0);
					
	// 				int tempWidth = otherImg->getWidth()-1;
	// 				int tempHeight = otherImg->getHeight()-1;

	// 				int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
	// 				int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

	// 				int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
	// 				int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

	// 				double offsetVal = (dir) ? -0.5 : 0.5;

	// 				if(x1 <= x2)
	// 				{
	// 					minX = MathExt::clamp(x1, minXBound, maxXBound);
	// 					maxX = MathExt::clamp(x2, minXBound, maxXBound);
	// 				}
	// 				else
	// 				{
	// 					minX = MathExt::clamp(x2, minXBound, maxXBound);
	// 					maxX = MathExt::clamp(x1, minXBound, maxXBound);
	// 				}

	// 				if(currentComposite == NO_COMPOSITE)
	// 				{
	// 					for(int i=minX; i<=maxX; i++)
	// 					{
	// 						//solve with respect to y
	// 						double val = -(con+P1*(i+offsetVal)) / (double)P2;
	// 						int actualY = MathExt::round(val);

	// 						if(actualY >= minYBound && actualY <= maxYBound)
	// 							otherImg->setPixel(i, actualY, Graphics::activeColor); //change later
	// 					}
	// 				}
	// 				else
	// 				{
	// 					for(int i=minX; i<=maxX; i++)
	// 					{
	// 						//solve with respect to y
	// 						double val = -(con+P1*(i+offsetVal)) / (double)P2;
	// 						int actualY = MathExt::round(val);
	// 						if(actualY >= minYBound && actualY <= maxYBound)
	// 							otherImg->drawPixel(i, actualY, Graphics::activeColor); //change later
	// 					}

	// 				}
	// 			}
	// 		}
	// 		else if(P2==0)
	// 		{
	// 			//VERTICAL LINE
	// 			int minY, maxY;

	// 			int tempWidth = otherImg->getWidth()-1;
	// 			int tempHeight = otherImg->getHeight()-1;

	// 			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
	// 			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

	// 			int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
	// 			int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());
				
	// 			if(y1 <= y2)
	// 			{
	// 				minY = MathExt::clamp(y1, minYBound, maxYBound);
	// 				maxY = MathExt::clamp(y2, minYBound, maxYBound);
	// 			}
	// 			else
	// 			{
	// 				minY = MathExt::clamp(y2, minYBound, maxYBound);
	// 				maxY = MathExt::clamp(y1, minYBound, maxYBound);
	// 			}

	// 			if(x1 < minXBound || x1 > maxXBound)
	// 			{
	// 				return;
	// 			}

	// 			//fix later. Currently will always draw 1 pixel at either the maxYBound or maxYBound as long as x1 is between the x bounds

	// 			if(currentComposite == NO_COMPOSITE)
	// 			{
	// 				for(int i=minY; i<=maxY; i++)
	// 				{
	// 					otherImg->setPixel(x1, i, Graphics::activeColor);
	// 				}
	// 			}
	// 			else
	// 			{
	// 				for(int i=minY; i<=maxY; i++)
	// 				{
	// 					otherImg->drawPixel(x1, i, Graphics::activeColor);
	// 				}
	// 			}
	// 		}
	// 		else if(P1==0)
	// 		{
	// 			//HORIZONTAL LINE
	// 			int minX, maxX;

	// 			int tempWidth = otherImg->getWidth()-1;
	// 			int tempHeight = otherImg->getHeight()-1;

	// 			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
	// 			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

	// 			int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
	// 			int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

	// 			if(x1 <= x2)
	// 			{
	// 				minX = MathExt::clamp(x1, minXBound, maxXBound);
	// 				maxX = MathExt::clamp(x2, minXBound, maxXBound);
	// 			}
	// 			else
	// 			{
	// 				minX = MathExt::clamp(x2, minXBound, maxXBound);
	// 				maxX = MathExt::clamp(x1, minXBound, maxXBound);
	// 			}

	// 			if(y1 < minYBound || y1 > maxYBound)
	// 			{
	// 				return;
	// 			}

	// 			#if(OPTI>=2)
	// 				int avxWidth = (1+maxX-minX) >> 3;
	// 				int remainder = (1+maxX - minX) - (avxWidth<<3);

	// 				Color* startColor = otherImg->getPixels() + minX + (y1*otherImg->getWidth());
	// 				__m256i* startAVX = (__m256i*)startColor;
	// 				__m256i* endAVX = startAVX + avxWidth;
					

	// 				__m256i avxColor = _mm256_set1_epi32( *((int*)&Graphics::activeColor) );
					
	// 				if(currentComposite == NO_COMPOSITE)
	// 				{
	// 					while(startAVX < endAVX)
	// 					{
	// 						_mm256_storeu_si256(startAVX, avxColor);
	// 						startAVX++;
	// 					}

	// 					//fill remainder
	// 					startColor += avxWidth<<3;
	// 					for(int i=0; i<remainder; i++)
	// 					{
	// 						*startColor = Graphics::activeColor;
	// 						startColor++;
	// 					}
	// 				}
	// 				else
	// 				{
	// 					while(startAVX < endAVX)
	// 					{
	// 						__m256i currentColor = _mm256_loadu_si256(startAVX);
	// 						__m256i blendC = blend(avxColor, currentColor);
	// 						_mm256_storeu_si256(startAVX, blendC);
	// 						startAVX++;
	// 					}

	// 					//fill remainder
	// 					startColor += avxWidth<<3;
	// 					for(int i=0; i<remainder; i++)
	// 					{
	// 						*startColor = Graphics::activeColor;
	// 						startColor++;
	// 					}
	// 				}

	// 			#elif(OPTI>=1)
					
	// 				int sseWidth = (1+maxX-minX) >> 2;
	// 				int remainder = (1+maxX - minX) - (sseWidth<<2);

	// 				Color* startColor = otherImg->getPixels() + minX + (y1*otherImg->getWidth());
	// 				__m128i* startSSE = (__m128i*)startColor;
	// 				__m128i* endSSE = startSSE + sseWidth;


	// 				__m128i sseColor = _mm_set1_epi32( *((int*)&Graphics::activeColor) );
					
	// 				if(currentComposite == NO_COMPOSITE)
	// 				{
	// 					while(startSSE < endSSE)
	// 					{
	// 						_mm_storeu_si128(startSSE, sseColor);
	// 						startSSE++;
	// 					}

	// 					//fill remainder
	// 					startColor += sseWidth<<2;
	// 					for(int i=0; i<remainder; i++)
	// 					{
	// 						*startColor = Graphics::activeColor;
	// 						startColor++;
	// 					}
	// 				}
	// 				else
	// 				{
	// 					while(startSSE < endSSE)
	// 					{
	// 						__m128i currentColor = _mm_loadu_si128(startSSE);
	// 						__m128i blendC = blend(sseColor, currentColor);
	// 						_mm_storeu_si128(startSSE, blendC);
	// 						startSSE++;
	// 					}

	// 					//fill remainder
	// 					startColor += sseWidth<<2;
	// 					for(int i=0; i<remainder; i++)
	// 					{
	// 						*startColor = blend(Graphics::activeColor, *startColor);
	// 						startColor++;
	// 					}
	// 				}

	// 			#else

	// 				Color* startColor = otherImg->getPixels() + minX + (y1*otherImg->getWidth());
	// 				Color* endColor = otherImg->getPixels() + maxX + (y1*otherImg->getWidth());

	// 				if(currentComposite == NO_COMPOSITE)
	// 				{
	// 					while(startColor <= endColor)
	// 					{
	// 						*startColor = Graphics::activeColor;
	// 						startColor++;
	// 					}
	// 				}
	// 				else
	// 				{
	// 					while(startColor <= endColor)
	// 					{
	// 						*startColor = blend(Graphics::activeColor, *startColor);
	// 						startColor++;
	// 					}
	// 				}

	// 			#endif
				
	// 		}

	// 	}
	// }

	void Graphics::drawLine(int x1, int y1, int x2, int y2, Image* surf)
	{
		int currentComposite = compositeRule;

		if (surf != nullptr)
		{
			if(x1==x2 && y1==y2)
			{
				return;
			}

			double dx = x2-x1;
			double dy = y2-y1;
			double step;
			double x, y;

			if(dy == 0 && dx != 0)
			{
				//horizontal line
				int minX = MathExt::min(x1,x2);
				int maxX = MathExt::max(x1,x2);

				Color* startPoint = surf->getPixels() + minX + (surf->getWidth()*y1);
				Color* endPoint = surf->getPixels() + maxX + (surf->getWidth()*y1);
				
				#if (OPTI >= 2)
					int avxWidth = (1+maxX-minX) >> 3;
					int remainder = (1+maxX-minX) - (avxWidth << 3);

					__m256i* avxStart = (__m256i*)surf->getPixels();
					__m256i* avxEnd = avxStart + avxWidth;
					__m256i avxColor = _mm256_set1_epi32( *((int*)&activeColor) );

					while(avxStart < avxEnd)
					{
						__m256i srcColor = *avxStart;
						__m256i outputColor = blend(avxColor, srcColor);
						_mm256_storeu_si256(avxStart, outputColor);
						avxStart++;
					}

					//fill remainder
					startPoint += avxWidth << 3;
					for(int i=0; i<remainder; i++)
					{
						*startPoint = blend(activeColor, *startPoint);
					}

				#elif (OPTI == 1)

					int sseWidth = (1+maxX-minX) >> 2;
					int remainder = (1+maxX-minX) - (sseWidth << 2);

					__m128i* sseStart = (__m128i*)surf->getPixels();
					__m128i* sseEnd = sseStart + sseWidth;
					__m128i sseColor = _mm_set1_epi32( *((int*)&activeColor) );

					while(sseStart < sseEnd)
					{
						__m128i srcColor = *sseStart;
						__m128i outputColor = blend(sseColor, srcColor);
						_mm_storeu_si128(sseStart, outputColor);
						sseStart++;
					}

					//fill remainder
					startPoint += sseWidth << 2;
					for(int i=0; i<remainder; i++)
					{
						*startPoint = blend(activeColor, *startPoint);
					}
				#else
					while(startPoint <= endPoint)
					{
						*startPoint = blend(activeColor, *startPoint);
						startPoint++;
					}
				#endif
			}
			else if(dx == 0 && dy != 0)
			{
				//vertical line
				int minY = MathExt::min(y1, y2);
				int maxY = MathExt::max(y1, y2);
				for(int i=minY; i<=maxY; i++)
				{
					drawPixel(x1, i, activeColor, surf);
				}
			}
			else
			{

				if(abs(dx) >= abs(dy))
				{
					//increase by x
					step = abs(dx);
					dx /= step;
					dy /= step;

					if(x1 < x2)
					{
						x = x1+0.5;
						y = y1+0.5;
					}
					else
					{
						x = x2+0.5;
						y = y2+0.5;
						dx = -dx;
						dy = -dy;
					}

					for(int i=0; i<=step; i++)
					{
						double fracPart = MathExt::frac(y);
						double midPoint = MathExt::floor(y) + 0.5;
						
						if(fracPart > 0.5)
						{
							double otherPix = MathExt::floor(y+1) + 0.5;

							double errA = y - midPoint;
							double errB = otherPix - y;

							double alpha1 = 1.0 - errA;
							double alpha2 = 1.0 - errB;

							Color c1 = activeColor;
							Color c2 = activeColor;
							c1.alpha = (unsigned char)(c1.alpha*alpha1);
							c2.alpha = (unsigned char)(c2.alpha*alpha2);

							drawPixel((int)x, (int)y, c1, surf);
							drawPixel((int)x, (int)y+1, c2, surf);
						}
						else if(fracPart < 0.5)
						{
							double otherPix = MathExt::floor(y-1) + 0.5;

							double errA = midPoint - y;
							double errB = y - otherPix;

							double alpha1 = 1.0 - errA;
							double alpha2 = 1.0 - errB;

							Color c1 = activeColor;
							Color c2 = activeColor;
							c1.alpha = (unsigned char)(c1.alpha*alpha1);
							c2.alpha = (unsigned char)(c2.alpha*alpha2);
							
							drawPixel((int)x, (int)y, c1, surf);
							drawPixel((int)x, (int)y-1, c2, surf);
						}
						else
						{
							drawPixel((int)x, (int)y, activeColor, surf);
						}
					
						x += dx;
						y += dy;
					}
				}
				else
				{
					//increase by y
					step = abs(dy);
					dx /= step;
					dy /= step;

					if(y1 < y2)
					{
						x = x1+0.5;
						y = y1+0.5;
					}
					else
					{
						x = x2+0.5;
						y = y2+0.5;
						dx = -dx;
						dy = -dy;
					}

					for(int i=0; i<=step; i++)
					{
						double fracPart = MathExt::frac(x);
						double midPoint = MathExt::floor(x) + 0.5;
						
						if(fracPart > 0.5)
						{
							double otherPix = MathExt::floor(x+1) + 0.5;

							double errA = x - midPoint;
							double errB = otherPix - x;

							double alpha1 = 1.0 - errA;
							double alpha2 = 1.0 - errB;

							Color c1 = activeColor;
							Color c2 = activeColor;
							c1.alpha = (unsigned char)(c1.alpha*alpha1);
							c2.alpha = (unsigned char)(c2.alpha*alpha2);

							drawPixel((int)x, (int)y, c1, surf);
							drawPixel((int)x+1, (int)y, c2, surf);
						}
						else if(fracPart < 0.5)
						{
							double otherPix = MathExt::floor(x-1) + 0.5;

							double errA = midPoint - x;
							double errB = x - otherPix;

							double alpha1 = 1.0 - errA;
							double alpha2 = 1.0 - errB;

							Color c1 = activeColor;
							Color c2 = activeColor;
							c1.alpha = (unsigned char)(c1.alpha*alpha1);
							c2.alpha = (unsigned char)(c2.alpha*alpha2);
							
							drawPixel((int)x, (int)y, c1, surf);
							drawPixel((int)x-1, (int)y, c2, surf);
						}
						else
						{
							drawPixel((int)x, (int)y, activeColor, surf);
						}
					
						x += dx;
						y += dy;
					}
				}

			}
		}
	}

	void Graphics::drawBezierCurve(BezierCurve& b, int subdivisions, bool useArcLength, Image* surf)
	{
		if(surf == nullptr)
			return;
		
		if(subdivisions<=0)
			return;
		
		if(b.size() < 2)
			return;

		if(useArcLength)
		{
			std::vector<double> equalLengthT = std::vector<double>();
			equalLengthT.push_back(0);

			double totalArcLength = b.getArcLengthAt(1);
			double arcInc = totalArcLength/subdivisions;

			int u = 1;
			while(u < subdivisions)
			{
				//solve using bisection
				double solveLength = u*arcInc;
				double guessMax = 1;
				double guessMin = 0;

				for(int i=0; i<10; i++)
				{
					double guess = (guessMax + guessMin)/2;
					double fGuess = b.getArcLengthAt(guess);

					if(fGuess == solveLength)
					{
						equalLengthT.push_back(guess);
						break;
					}
					else if(fGuess > solveLength)
					{
						guessMax = guess;
					}
					else
					{
						guessMin = guess;
					}

					if(i==9)
					{
						equalLengthT.push_back(guess);
					}
				}

				u++;
			}

			equalLengthT.push_back(1);
			Vec2f p1 = b.getPoint(0);

			for(int i=1; i<=subdivisions; i++)
			{
				Vec2f p2 = b.getFuctionAt( equalLengthT[i] );
				Graphics::drawLine(MathExt::round(p1.x), MathExt::round(p1.y), MathExt::round(p2.x), MathExt::round(p2.y), surf);
				
				Graphics::drawCircle(MathExt::round(p1.x), MathExt::round(p1.y), 3, true, surf);
				p1 = p2;
			}
		}
		else
		{
			Vec2f p1 = b.getPoint(0);
			double du = 1.0/subdivisions;

			for(int i=1; i<=subdivisions; i++)
			{
				Vec2f p2 = b.getFuctionAt( i*du );
				Graphics::drawLine(MathExt::round(p1.x), MathExt::round(p1.y), MathExt::round(p2.x), MathExt::round(p2.y), surf);
				
				Graphics::drawCircle(MathExt::round(p1.x), MathExt::round(p1.y), 3, true, surf);
				p1 = p2;
			}
		}

	}
    
} //NAMESPACE glib END