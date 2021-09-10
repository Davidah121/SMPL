#include "InternalGraphicsHeader.h"

namespace glib
{
	//Works properly now
	void Graphics::drawImage(Image* img, int x, int y, Image* surf)
	{
		int currentComposite = compositeRule;

		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr && img!=nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			if(img->getWidth()<=0 || img->getHeight()<=0)
			{
				return;
			}
			int tempWidth = otherImg->getWidth();
			int tempHeight = otherImg->getHeight();

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(tempWidth, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+img->getWidth(), minXBound, maxXBound);

			int minY = MathExt::clamp(y, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+img->getHeight(), minYBound, maxYBound);

			Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
			Color* endPoint = otherImg->getPixels() + maxX + ((maxY-1) * tempWidth);

			int drawImgWidth = maxX - minX;
			int drawImgAddX = img->getWidth() - drawImgWidth;
			int otherImgAddX = tempWidth - drawImgWidth;

			Color* drawImgStart = img->getPixels();

			int tX = 0;

			#if(OPTI>=2)

				int avxWidth = (maxX-minX) >> 3;
				int remainder = (maxX-minX) - (avxWidth<<3);

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							_mm256_storeu_si256(avxPoint, avxColor);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							*startPoint = *drawImgStart;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							__m256i currentColor = _mm256_loadu_si256(avxPoint);
							__m256i blendC = blend(avxColor, currentColor);
							_mm256_storeu_si256(avxPoint, blendC);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							*startPoint = blend(*drawImgStart, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#elif(OPTI>=1)

				int sseWidth = (maxX-minX) >> 2;
				int remainder = (maxX-minX) - (sseWidth<<2);

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							_mm_storeu_si128(ssePoint, sseColor);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							*startPoint = *drawImgStart;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							__m128i currentColor = _mm_loadu_si128(ssePoint);
							__m128i blendC = blend(sseColor, currentColor);
							_mm_storeu_si128(ssePoint, blendC);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							*startPoint = blend(*drawImgStart, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#else

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							*startPoint = *drawImgStart;

							startPoint++;
							drawImgStart++;
						}
					
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							*startPoint = blend(*drawImgStart, *startPoint);

							startPoint++;
							drawImgStart++;
						}
					
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}

			#endif
		}
	}

	void Graphics::drawSprite(Image* img, int x, int y, Image* surf)
	{
		int currentComposite = compositeRule;
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr && img!=nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			if(img->getWidth()<=0 || img->getHeight()<=0)
			{
				return;
			}
			int tempWidth = otherImg->getWidth();
			int tempHeight = otherImg->getHeight();

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

			int minX = MathExt::clamp(x, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+img->getWidth(), minXBound, maxXBound);

			int minY = MathExt::clamp(y, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+img->getHeight(), minYBound, maxYBound);

			Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
			Color* endPoint = otherImg->getPixels() + maxX + ((maxY-1) * tempWidth);
			
			int drawImgWidth = maxX - minX;
			int drawImgAddX = img->getWidth() - drawImgWidth;
			int otherImgAddX = tempWidth - drawImgWidth;

			Color* drawImgStart = img->getPixels();

			int tX = 0;
			Vec4f colorMult = Vec4f((double)Graphics::activeColor.red / 255.0, (double)Graphics::activeColor.green / 255.0, (double)Graphics::activeColor.blue / 255.0, (double)Graphics::activeColor.alpha / 255.0);

			#if(OPTI>=2)
				
				int avxWidth = (maxX-minX) >> 3;
				int remainder = (maxX-minX) - (avxWidth<<3);

				__m256 colorMultRed = _mm256_set1_ps( ((float)Graphics::activeColor.red) / 255.0 );
				__m256 colorMultGreen = _mm256_set1_ps( ((float)Graphics::activeColor.green) / 255.0 );
				__m256 colorMultBlue = _mm256_set1_ps( ((float)Graphics::activeColor.blue) / 255.0 );
				__m256 colorMultAlpha = _mm256_set1_ps( ((float)Graphics::activeColor.alpha) / 255.0 );

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							
							Color* colors = (Color*)&avxColor;

							__m256 reds = _mm256_set_ps(
								(float)colors[7].red, (float)colors[6].red, (float)colors[5].red, (float)colors[4].red,
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m256 greens = _mm256_set_ps(
								(float)colors[7].green, (float)colors[6].green, (float)colors[5].green, (float)colors[4].green,
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m256 blues = _mm256_set_ps(
								(float)colors[7].blue, (float)colors[6].blue, (float)colors[5].blue, (float)colors[4].blue,
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m256 alphas = _mm256_set_ps(
								(float)colors[7].alpha, (float)colors[6].alpha, (float)colors[5].alpha, (float)colors[4].alpha,
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm256_mul_ps(reds, colorMultRed);
							greens = _mm256_mul_ps(greens, colorMultGreen);
							blues = _mm256_mul_ps(blues, colorMultBlue);
							alphas = _mm256_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m256i finalColor = _mm256_set_epi8(
								(unsigned char)aFloats[7], (unsigned char)bFloats[7], (unsigned char)gFloats[7], (unsigned char)rFloats[7],
								(unsigned char)aFloats[6], (unsigned char)bFloats[6], (unsigned char)gFloats[6], (unsigned char)rFloats[6],
								(unsigned char)aFloats[5], (unsigned char)bFloats[5], (unsigned char)gFloats[5], (unsigned char)rFloats[5],
								(unsigned char)aFloats[4], (unsigned char)bFloats[4], (unsigned char)gFloats[4], (unsigned char)rFloats[4],
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);


							_mm256_storeu_si256(avxPoint, avxColor);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							__m256i currentColor = _mm256_loadu_si256(avxPoint);
							
							Color* colors = (Color*)&avxColor;

							__m256 reds = _mm256_set_ps(
								(float)colors[7].red, (float)colors[6].red, (float)colors[5].red, (float)colors[4].red,
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m256 greens = _mm256_set_ps(
								(float)colors[7].green, (float)colors[6].green, (float)colors[5].green, (float)colors[4].green,
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m256 blues = _mm256_set_ps(
								(float)colors[7].blue, (float)colors[6].blue, (float)colors[5].blue, (float)colors[4].blue,
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m256 alphas = _mm256_set_ps(
								(float)colors[7].alpha, (float)colors[6].alpha, (float)colors[5].alpha, (float)colors[4].alpha,
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm256_mul_ps(reds, colorMultRed);
							greens = _mm256_mul_ps(greens, colorMultGreen);
							blues = _mm256_mul_ps(blues, colorMultBlue);
							alphas = _mm256_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m256i finalColor = _mm256_set_epi8(
								(unsigned char)aFloats[7], (unsigned char)bFloats[7], (unsigned char)gFloats[7], (unsigned char)rFloats[7],
								(unsigned char)aFloats[6], (unsigned char)bFloats[6], (unsigned char)gFloats[6], (unsigned char)rFloats[6],
								(unsigned char)aFloats[5], (unsigned char)bFloats[5], (unsigned char)gFloats[5], (unsigned char)rFloats[5],
								(unsigned char)aFloats[4], (unsigned char)bFloats[4], (unsigned char)gFloats[4], (unsigned char)rFloats[4],
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);

							__m256i blendC = blend(finalColor, currentColor);
							_mm256_storeu_si256(avxPoint, blendC);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#elif(OPTI>=1)

				int sseWidth = (maxX-minX) >> 2;
				int remainder = (maxX-minX) - (sseWidth<<2);

				__m128 colorMultRed = _mm_set1_ps( ((float)Graphics::activeColor.red) / 255.0 );
				__m128 colorMultGreen = _mm_set1_ps( ((float)Graphics::activeColor.green) / 255.0 );
				__m128 colorMultBlue = _mm_set1_ps( ((float)Graphics::activeColor.blue) / 255.0 );
				__m128 colorMultAlpha = _mm_set1_ps( ((float)Graphics::activeColor.alpha) / 255.0 );

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							
							Color* colors = (Color*)&sseColor;

							__m128 reds = _mm_set_ps(
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m128 greens = _mm_set_ps(
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m128 blues = _mm_set_ps(
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m128 alphas = _mm_set_ps(
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm_mul_ps(reds, colorMultRed);
							greens = _mm_mul_ps(greens, colorMultGreen);
							blues = _mm_mul_ps(blues, colorMultBlue);
							alphas = _mm_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m128i finalColor = _mm_set_epi8(
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);


							_mm_storeu_si128(ssePoint, sseColor);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							__m128i currentColor = _mm_loadu_si128(ssePoint);
							
							Color* colors = (Color*)&sseColor;

							__m128 reds = _mm_set_ps(
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m128 greens = _mm_set_ps(
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m128 blues = _mm_set_ps(
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m128 alphas = _mm_set_ps(
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm_mul_ps(reds, colorMultRed);
							greens = _mm_mul_ps(greens, colorMultGreen);
							blues = _mm_mul_ps(blues, colorMultBlue);
							alphas = _mm_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m128i finalColor = _mm_set_epi8(
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);

							__m128i blendC = blend(finalColor, currentColor);
							_mm_storeu_si128(ssePoint, blendC);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#else
				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;

							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);

							startPoint++;
							drawImgStart++;
						}
					
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}

			#endif
		}
	}

	void Graphics::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf)
	{
		int currentComposite = compositeRule;
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr && img!=nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			if(img->getWidth()<=0 || img->getHeight()<=0)
			{
				return;
			}
			int tempWidth = otherImg->getWidth();
			int tempHeight = otherImg->getHeight();

			int minXBound = MathExt::max(0, (int)clippingRect.getLeftBound());
			int minYBound = MathExt::max(0, (int)clippingRect.getTopBound());

			int maxXBound = MathExt::min(tempWidth-1, (int)clippingRect.getRightBound());
			int maxYBound = MathExt::min(tempHeight-1, (int)clippingRect.getBottomBound());

			int clampImgW = MathExt::clamp(imgW, 0, img->getWidth()-1);
			int clampImgH = MathExt::clamp(imgH, 0, img->getHeight()-1);
			
			int minX = MathExt::clamp(x, minXBound, maxXBound);
			int maxX = MathExt::clamp(x+clampImgW, minXBound, maxXBound);

			int minY = MathExt::clamp(y, minYBound, maxYBound);
			int maxY = MathExt::clamp(y+clampImgH, minYBound, maxYBound);

			Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
			Color* endPoint = otherImg->getPixels() + maxX + ((maxY-1) * tempWidth);
			
			int drawImgWidth = maxX - minX;
			int otherImgAddX = tempWidth - drawImgWidth;

			Color* drawImgStart = img->getPixels() + imgX + (imgY*img->getWidth());
			int drawImgAddX = img->getWidth() - drawImgWidth;

			int tX = 0;
			Vec4f colorMult = Vec4f((double)Graphics::activeColor.red / 255.0, (double)Graphics::activeColor.green / 255.0, (double)Graphics::activeColor.blue / 255.0, (double)Graphics::activeColor.alpha / 255.0);

			#if(OPTI>=2)
				
				int avxWidth = (maxX-minX) >> 3;
				int remainder = (maxX-minX) - (avxWidth<<3);

				__m256 colorMultRed = _mm256_set1_ps( ((float)Graphics::activeColor.red) / 255.0 );
				__m256 colorMultGreen = _mm256_set1_ps( ((float)Graphics::activeColor.green) / 255.0 );
				__m256 colorMultBlue = _mm256_set1_ps( ((float)Graphics::activeColor.blue) / 255.0 );
				__m256 colorMultAlpha = _mm256_set1_ps( ((float)Graphics::activeColor.alpha) / 255.0 );

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							
							Color* colors = (Color*)&avxColor;

							__m256 reds = _mm256_set_ps(
								(float)colors[7].red, (float)colors[6].red, (float)colors[5].red, (float)colors[4].red,
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m256 greens = _mm256_set_ps(
								(float)colors[7].green, (float)colors[6].green, (float)colors[5].green, (float)colors[4].green,
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m256 blues = _mm256_set_ps(
								(float)colors[7].blue, (float)colors[6].blue, (float)colors[5].blue, (float)colors[4].blue,
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m256 alphas = _mm256_set_ps(
								(float)colors[7].alpha, (float)colors[6].alpha, (float)colors[5].alpha, (float)colors[4].alpha,
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm256_mul_ps(reds, colorMultRed);
							greens = _mm256_mul_ps(greens, colorMultGreen);
							blues = _mm256_mul_ps(blues, colorMultBlue);
							alphas = _mm256_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m256i finalColor = _mm256_set_epi8(
								(unsigned char)aFloats[7], (unsigned char)bFloats[7], (unsigned char)gFloats[7], (unsigned char)rFloats[7],
								(unsigned char)aFloats[6], (unsigned char)bFloats[6], (unsigned char)gFloats[6], (unsigned char)rFloats[6],
								(unsigned char)aFloats[5], (unsigned char)bFloats[5], (unsigned char)gFloats[5], (unsigned char)rFloats[5],
								(unsigned char)aFloats[4], (unsigned char)bFloats[4], (unsigned char)gFloats[4], (unsigned char)rFloats[4],
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);


							_mm256_storeu_si256(avxPoint, avxColor);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m256i* avxPoint = (__m256i*)startPoint;
						__m256i* imgPoint = (__m256i*)drawImgStart;

						for(int i=0; i<avxWidth; i++)
						{
							__m256i avxColor = _mm256_loadu_si256(imgPoint);
							__m256i currentColor = _mm256_loadu_si256(avxPoint);
							
							Color* colors = (Color*)&avxColor;

							__m256 reds = _mm256_set_ps(
								(float)colors[7].red, (float)colors[6].red, (float)colors[5].red, (float)colors[4].red,
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m256 greens = _mm256_set_ps(
								(float)colors[7].green, (float)colors[6].green, (float)colors[5].green, (float)colors[4].green,
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m256 blues = _mm256_set_ps(
								(float)colors[7].blue, (float)colors[6].blue, (float)colors[5].blue, (float)colors[4].blue,
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m256 alphas = _mm256_set_ps(
								(float)colors[7].alpha, (float)colors[6].alpha, (float)colors[5].alpha, (float)colors[4].alpha,
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm256_mul_ps(reds, colorMultRed);
							greens = _mm256_mul_ps(greens, colorMultGreen);
							blues = _mm256_mul_ps(blues, colorMultBlue);
							alphas = _mm256_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m256i finalColor = _mm256_set_epi8(
								(unsigned char)aFloats[7], (unsigned char)bFloats[7], (unsigned char)gFloats[7], (unsigned char)rFloats[7],
								(unsigned char)aFloats[6], (unsigned char)bFloats[6], (unsigned char)gFloats[6], (unsigned char)rFloats[6],
								(unsigned char)aFloats[5], (unsigned char)bFloats[5], (unsigned char)gFloats[5], (unsigned char)rFloats[5],
								(unsigned char)aFloats[4], (unsigned char)bFloats[4], (unsigned char)gFloats[4], (unsigned char)rFloats[4],
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);

							__m256i blendC = blend(finalColor, currentColor);
							_mm256_storeu_si256(avxPoint, blendC);
							avxPoint++;
							imgPoint++;
						}

						startPoint += avxWidth<<3;
						drawImgStart += avxWidth<<3;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#elif(OPTI>=1)

				int sseWidth = (maxX-minX) >> 2;
				int remainder = (maxX-minX) - (sseWidth<<2);

				__m128 colorMultRed = _mm_set1_ps( ((float)Graphics::activeColor.red) / 255.0 );
				__m128 colorMultGreen = _mm_set1_ps( ((float)Graphics::activeColor.green) / 255.0 );
				__m128 colorMultBlue = _mm_set1_ps( ((float)Graphics::activeColor.blue) / 255.0 );
				__m128 colorMultAlpha = _mm_set1_ps( ((float)Graphics::activeColor.alpha) / 255.0 );

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							
							Color* colors = (Color*)&sseColor;

							__m128 reds = _mm_set_ps(
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m128 greens = _mm_set_ps(
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m128 blues = _mm_set_ps(
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m128 alphas = _mm_set_ps(
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm_mul_ps(reds, colorMultRed);
							greens = _mm_mul_ps(greens, colorMultGreen);
							blues = _mm_mul_ps(blues, colorMultBlue);
							alphas = _mm_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m128i finalColor = _mm_set_epi8(
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);


							_mm_storeu_si128(ssePoint, sseColor);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;
							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						__m128i* ssePoint = (__m128i*)startPoint;
						__m128i* imgPoint = (__m128i*)drawImgStart;

						for(int i=0; i<sseWidth; i++)
						{
							__m128i sseColor = _mm_loadu_si128(imgPoint);
							__m128i currentColor = _mm_loadu_si128(ssePoint);
							
							Color* colors = (Color*)&sseColor;

							__m128 reds = _mm_set_ps(
								(float)colors[3].red, (float)colors[2].red, (float)colors[1].red, (float)colors[0].red
							);
							__m128 greens = _mm_set_ps(
								(float)colors[3].green, (float)colors[2].green, (float)colors[1].green, (float)colors[0].green
							);
							__m128 blues = _mm_set_ps(
								(float)colors[3].blue, (float)colors[2].blue, (float)colors[1].blue, (float)colors[0].blue
							);
							__m128 alphas = _mm_set_ps(
								(float)colors[3].alpha, (float)colors[2].alpha, (float)colors[1].alpha, (float)colors[0].alpha
							);

							reds = _mm_mul_ps(reds, colorMultRed);
							greens = _mm_mul_ps(greens, colorMultGreen);
							blues = _mm_mul_ps(blues, colorMultBlue);
							alphas = _mm_mul_ps(alphas, colorMultAlpha);

							float* rFloats = (float*)&reds;
							float* gFloats = (float*)&greens;
							float* bFloats = (float*)&blues;
							float* aFloats = (float*)&alphas;

							__m128i finalColor = _mm_set_epi8(
								(unsigned char)aFloats[3], (unsigned char)bFloats[3], (unsigned char)gFloats[3], (unsigned char)rFloats[3],
								(unsigned char)aFloats[2], (unsigned char)bFloats[2], (unsigned char)gFloats[2], (unsigned char)rFloats[2],
								(unsigned char)aFloats[1], (unsigned char)bFloats[1], (unsigned char)gFloats[1], (unsigned char)rFloats[1],
								(unsigned char)aFloats[0], (unsigned char)bFloats[0], (unsigned char)gFloats[0], (unsigned char)rFloats[0]
							);

							__m128i blendC = blend(finalColor, currentColor);
							_mm_storeu_si128(ssePoint, blendC);
							ssePoint++;
							imgPoint++;
						}

						startPoint += sseWidth<<2;
						drawImgStart += sseWidth<<2;

						//fill remainder
						for(int i=0; i<remainder; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);
							startPoint++;
							drawImgStart++;
						}
						
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					}
				}

			#else
				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = drawC;

							startPoint++;
							drawImgStart++;
						}

						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<drawImgWidth; i++)
						{
							Color drawC = { (unsigned char) (drawImgStart->red * colorMult.x),
											(unsigned char) (drawImgStart->green * colorMult.y),
											(unsigned char) (drawImgStart->blue * colorMult.z), 
											(unsigned char) (drawImgStart->alpha * colorMult.w) };
							*startPoint = blend(drawC, *startPoint);

							startPoint++;
							drawImgStart++;
						}
					
						startPoint += otherImgAddX;
						drawImgStart += drawImgAddX;
					
					}
				}

			#endif
		}
	}

} //NAMESPACE glib END