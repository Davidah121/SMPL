#include "InternalGraphicsHeader.h"

namespace glib
{
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

			int tempWidth = surf->getWidth();
			int tempHeight = surf->getHeight();

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());
			
			int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());
			
			
			int minX = MathExt::min(x1,x2);
			int maxX = MathExt::max(x1,x2);
			
			int minY = MathExt::min(y1,y2);
			int maxY = MathExt::max(y1,y2);
			
			if(maxX < minXBound || minX > maxXBound)
			{
				return;
			}

			minX = MathExt::clamp(minX, minXBound, maxXBound);
			maxX = MathExt::clamp(maxX, minXBound, maxXBound);

			if(maxY < minYBound || minY > maxYBound)
			{
				return;
			}

			minY = MathExt::clamp(minY, minYBound, maxYBound);
			maxY = MathExt::clamp(maxY, minYBound, maxYBound);
		
			if(dy == 0 && dx != 0)
			{
				//horizontal line
				
				Color* startPoint = surf->getPixels() + minX + (surf->getWidth()*y1);
				Color* endPoint = surf->getPixels() + maxX + (surf->getWidth()*y1);
				
				#if (OPTI >= 2)
					int avxWidth = (1+maxX-minX) >> 3;
					int remainder = (1+maxX-minX) - (avxWidth << 3);

					__m256i* avxStart = (__m256i*)startPoint;
					__m256i* avxEnd = avxStart + avxWidth;
					__m256i avxColor = _mm256_set1_epi32( *((int*)&activeColor) );

					while(avxStart < avxEnd)
					{
						__m256i srcColor = _mm256_loadu_si256(avxStart);
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

				#elif (OPTI >= 1)

					int sseWidth = (1+maxX-minX) >> 2;
					int remainder = (1+maxX-minX) - (sseWidth << 2);

					__m128i* sseStart = (__m128i*)startPoint;
					__m128i* sseEnd = sseStart + sseWidth;
					__m128i sseColor = _mm_set1_epi32( *((int*)&activeColor) );

					while(sseStart < sseEnd)
					{
						__m128i srcColor = _mm_loadu_si128(sseStart);
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