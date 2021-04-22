#include "Graphics.h"
#include "MathExt.h"
#include <iostream>
#include "Sort.h"
#include "StringTools.h"
#include "BitmapFont.h"

#pragma region DRAWING_FUNCTIONS

Color Graphics::activeColor = { 0, 0, 0, 255 };
Font* Graphics::activeFont = nullptr;
Image* Graphics::activeImage = nullptr;
unsigned char Graphics::compositeRule = Graphics::COMPOSITE_SRC_OVER;
unsigned char Graphics::blendMode = Graphics::BLEND_NORMAL;
bool Graphics::fillRule = Graphics::FILL_EVEN_ODD;
bool Graphics::antiAliasing = false;

unsigned char Graphics::defaultFontValue = Graphics::NORMAL_FONT;
Font* Graphics::defaultFont = nullptr;
Font* Graphics::defaultFontLarge = nullptr;

void Graphics::init()
{
	Graphics::defaultFont = new BitmapFont("./Resources/DefaultFont.fnt");
	Graphics::defaultFontLarge = new BitmapFont("./Resources/DefaultFontLarge.fnt");
}

void Graphics::dispose()
{
	if(defaultFont!=nullptr)
		delete defaultFont;
	
	if(defaultFontLarge!=nullptr)
		delete defaultFontLarge;
	
	defaultFont = nullptr;
	defaultFontLarge = nullptr;
}

void Graphics::setImage(Image* img)
{
	activeImage = img;
}

Image* Graphics::getImage()
{
	return activeImage;
}

void Graphics::clearImage(Image* surf)
{
	if (surf == nullptr)
	{
		if (activeImage != nullptr)
		{
			activeImage->setAllPixels(activeColor);
		}
	}
	else
	{
		surf->setAllPixels(activeColor);
	}
}

void Graphics::drawPixel(double x, double y, Color c, Image* surf)
{
	//convert into four separate pixels
	Vec2f p1 = Vec2f(floor(x), floor(y));
	Vec2f p2 = Vec2f(ceil(x), ceil(y));

	float xAlpha = 1.0f - (x-p1.x);
	float yAlpha = 1.0f - (y-p1.y);

	Color c1, c2, c3, c4;
	c1 = c;
	c1.alpha = (unsigned char) (c1.alpha * xAlpha * yAlpha);

	c2 = c;
	c2.alpha = (unsigned char) (c2.alpha * (1-xAlpha) * yAlpha);
	
	c3 = c;
	c3.alpha = (unsigned char) (c3.alpha * (1-xAlpha) * (1-yAlpha));
	
	c4 = c;
	c4.alpha = (unsigned char) (c4.alpha * xAlpha * (1-yAlpha));
	
	drawPixel( (int)p1.x, (int)p1.y, c1, surf);
	drawPixel( (int)p2.x, (int)p1.y, c2, surf);
	drawPixel( (int)p2.x, (int)p2.y, c3, surf);
	drawPixel( (int)p1.x, (int)p2.y, c4, surf);
}

void Graphics::drawPixel(int x, int y, Color c, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if(compositeRule == NO_COMPOSITE)
	{
		otherImg->setPixel(x, y, c);
		return;
	}

	float Fa = 0;
	float Fb = 0;

	int red = 0;
	int green = 0;
	int blue = 0;
	int alpha = 0;

	//other stuff to
	float tAlpha1 = (float)c.alpha/255;
	Color otherColor = otherImg->getPixel(x,y);
	float tAlpha2 = (float)otherColor.alpha/255;

	if (otherImg != nullptr)
	{
		switch(compositeRule)
		{
			case COMPOSITE_CLEAR:
				Fa=0;
				Fb=0;
				break;
			case COMPOSITE_COPY:
				Fa = 1;
				Fb = 0;
				break;
			case COMPOSITE_DEST:
				Fa = 0;
				Fb = 1;
				break;
			case COMPOSITE_SRC_OVER:
				Fa = 1;
				Fb = 1-tAlpha1;
				break;
			case COMPOSITE_DEST_OVER:
				Fa = 1-tAlpha2;
				Fb = 1;
				break;
			case COMPOSITE_SRC_IN:
				Fa = tAlpha2;
				Fb = 0;
				break;
			case COMPOSITE_DEST_IN:
				Fa = 0;
				Fb = tAlpha1;
				break;
			case COMPOSITE_SRC_OUT:
				Fa = 1-tAlpha2;
				Fb = 0;
				break;
			case COMPOSITE_DEST_OUT:
				Fa = 0;
				Fb = 1-tAlpha1;
				break;
			case COMPOSITE_SRC_ATOP:
				Fa = tAlpha2;
				Fb = 1-tAlpha1;
				break;
			case COMPOSITE_DEST_ATOP:
				Fa = 1-tAlpha2;
				Fb = tAlpha1;
				break;
			case COMPOSITE_XOR:
				Fa = 1-tAlpha2;
				Fb = 1-tAlpha1;
				break;
			case COMPOSITE_LIGHTER:
				Fa = 1;
				Fb = 1;
				break;
			default:
				Fa = 0;
				Fb = 0;
				break;
		}

		red = (tAlpha1 * c.red * Fa) + (tAlpha2 * otherColor.red * Fb);
		green = (tAlpha1 * c.green * Fa) + (tAlpha2 * otherColor.green * Fb);
		blue = (tAlpha1 * c.blue * Fa) + (tAlpha2 * otherColor.blue * Fb);
		alpha = (c.alpha * Fa) + (otherColor.alpha * Fb);

		unsigned char redB = (unsigned char)MathExt::min(red, 255);
		unsigned char greenB = (unsigned char)MathExt::min(green, 255);
		unsigned char blueB = (unsigned char)MathExt::min(blue, 255);
		unsigned char alphaB = (unsigned char)MathExt::min(alpha, 255);

		otherImg->setPixel(x,y,{redB,greenB,blueB,alphaB});
	}
}

Color Graphics::blend(Color src, Color dest)
{
	if(compositeRule == NO_COMPOSITE)
	{
		return src;
	}

	float Fa = 0;
	float Fb = 0;

	int red = 0;
	int green = 0;
	int blue = 0;
	int alpha = 0;

	//other stuff to
	float tAlpha1 = (float)src.alpha/255;
	float tAlpha2 = (float)dest.alpha/255;

	switch(compositeRule)
	{
		case COMPOSITE_CLEAR:
			Fa=0;
			Fb=0;
			break;
		case COMPOSITE_COPY:
			Fa = 1;
			Fb = 0;
			break;
		case COMPOSITE_DEST:
			Fa = 0;
			Fb = 1;
			break;
		case COMPOSITE_SRC_OVER:
			Fa = 1;
			Fb = 1-tAlpha1;
			break;
		case COMPOSITE_DEST_OVER:
			Fa = 1-tAlpha2;
			Fb = 1;
			break;
		case COMPOSITE_SRC_IN:
			Fa = tAlpha2;
			Fb = 0;
			break;
		case COMPOSITE_DEST_IN:
			Fa = 0;
			Fb = tAlpha1;
			break;
		case COMPOSITE_SRC_OUT:
			Fa = 1-tAlpha2;
			Fb = 0;
			break;
		case COMPOSITE_DEST_OUT:
			Fa = 0;
			Fb = 1-tAlpha1;
			break;
		case COMPOSITE_SRC_ATOP:
			Fa = tAlpha2;
			Fb = 1-tAlpha1;
			break;
		case COMPOSITE_DEST_ATOP:
			Fa = 1-tAlpha2;
			Fb = tAlpha1;
			break;
		case COMPOSITE_XOR:
			Fa = 1-tAlpha2;
			Fb = 1-tAlpha1;
			break;
		case COMPOSITE_LIGHTER:
			Fa = 1;
			Fb = 1;
			break;
		default:
			Fa = 0;
			Fb = 0;
			break;
	}

	red = (tAlpha1 * src.red * Fa) + (tAlpha2 * dest.red * Fb);
	green = (tAlpha1 * src.green * Fa) + (tAlpha2 * dest.green * Fb);
	blue = (tAlpha1 * src.blue * Fa) + (tAlpha2 * dest.blue * Fb);
	alpha = (src.alpha * Fa) + (dest.alpha * Fb);

	unsigned char redB = (unsigned char)MathExt::min(red, 255);
	unsigned char greenB = (unsigned char)MathExt::min(green, 255);
	unsigned char blueB = (unsigned char)MathExt::min(blue, 255);
	unsigned char alphaB = (unsigned char)MathExt::min(alpha, 255);

	return {redB,greenB,blueB,alphaB};

}

#pragma region SSE_AND_AVX_BLENDS

#if (OPTI>=1)
__m128i Graphics::blend(__m128i src, __m128i dest)
{
	if(compositeRule == NO_COMPOSITE)
	{
		return src;
	}

	__m128 Fa;
	__m128 Fb;

	Color* c1 = (Color*)&src;
	Color* c2 = (Color*)&dest;

	__m128 srcRed, srcGreen, srcBlue, srcAlpha;
	__m128 destRed, destGreen, destBlue, destAlpha;
	
	srcRed = _mm_set_ps(
		((float)c1[3].red),
		((float)c1[2].red),
		((float)c1[1].red),
		((float)c1[0].red)
	);

	srcGreen = _mm_set_ps(
		((float)c1[3].green),
		((float)c1[2].green),
		((float)c1[1].green),
		((float)c1[0].green)
	);

	srcBlue = _mm_set_ps(
		((float)c1[3].blue),
		((float)c1[2].blue),
		((float)c1[1].blue),
		((float)c1[0].blue)
	);

	srcAlpha = _mm_set_ps(
		((float)c1[3].alpha)/255,
		((float)c1[2].alpha)/255,
		((float)c1[1].alpha)/255,
		((float)c1[0].alpha)/255
	);

	destRed = _mm_set_ps(
		((float)c2[3].red),
		((float)c2[2].red),
		((float)c2[1].red),
		((float)c2[0].red)
	);

	destGreen = _mm_set_ps(
		((float)c2[3].green),
		((float)c2[2].green),
		((float)c2[1].green),
		((float)c2[0].green)
	);

	destBlue = _mm_set_ps(
		((float)c2[3].blue),
		((float)c2[2].blue),
		((float)c2[1].blue),
		((float)c2[0].blue)
	);

	destAlpha = _mm_set_ps(
		((float)c2[3].alpha)/255,
		((float)c2[2].alpha)/255,
		((float)c2[1].alpha)/255,
		((float)c2[0].alpha)/255
	);

	//other stuff to

	switch(compositeRule)
	{
		case COMPOSITE_CLEAR:
			Fa = _mm_set1_ps(0);
			Fb = _mm_set1_ps(0);
			break;
		case COMPOSITE_COPY:
			Fa = _mm_set1_ps(1);
			Fb = _mm_set1_ps(0);
			break;
		case COMPOSITE_DEST:
			Fa = _mm_set1_ps(0);
			Fb = _mm_set1_ps(1);
			break;
		case COMPOSITE_SRC_OVER:
			Fa = _mm_set1_ps(1);
			Fb = _mm_sub_ps(_mm_set1_ps(1), srcAlpha);
			break;
		case COMPOSITE_DEST_OVER:
			Fa = _mm_sub_ps(_mm_set1_ps(1), destAlpha);
			Fb = _mm_set1_ps(1);
			break;
		case COMPOSITE_SRC_IN:
			Fa = destAlpha;
			Fb = _mm_set1_ps(0);
			break;
		case COMPOSITE_DEST_IN:
			Fa = _mm_set1_ps(0);
			Fb = srcAlpha;
			break;
		case COMPOSITE_SRC_OUT:
			Fa = _mm_sub_ps(_mm_set1_ps(1), destAlpha);
			Fb = _mm_set1_ps(0);
			break;
		case COMPOSITE_DEST_OUT:
			Fa = _mm_set1_ps(0);
			Fb = _mm_sub_ps(_mm_set1_ps(1), srcAlpha);
			break;
		case COMPOSITE_SRC_ATOP:
			Fa = destAlpha;
			Fb = _mm_sub_ps(_mm_set1_ps(1), srcAlpha);
			break;
		case COMPOSITE_DEST_ATOP:
			Fa = _mm_sub_ps(_mm_set1_ps(1), destAlpha);
			Fb = srcAlpha;
			break;
		case COMPOSITE_XOR:
			Fa = _mm_sub_ps(_mm_set1_ps(1), destAlpha);
			Fb = _mm_sub_ps(_mm_set1_ps(1), srcAlpha);
			break;
		case COMPOSITE_LIGHTER:
			Fa = _mm_set1_ps(1);
			Fb = _mm_set1_ps(1);
			break;
		default:
			Fa = _mm_set1_ps(0);
			Fb = _mm_set1_ps(0);
			break;
	}

	__m128 nRed, nGreen, nBlue, nAlpha;

	__m128 nSrcAlpha = _mm_mul_ps(Fa, srcAlpha);
	__m128 nDestAlpha = _mm_mul_ps(Fb, destAlpha);
	
	nRed = _mm_add_ps( _mm_mul_ps(srcRed, nSrcAlpha), _mm_mul_ps(destRed, nDestAlpha) );
	nGreen = _mm_add_ps( _mm_mul_ps(srcGreen, nSrcAlpha), _mm_mul_ps(destGreen, nDestAlpha) );
	nBlue = _mm_add_ps( _mm_mul_ps(srcBlue, nSrcAlpha), _mm_mul_ps(destBlue, nDestAlpha) );
	nAlpha = _mm_mul_ps(_mm_add_ps( nSrcAlpha, nDestAlpha ), _mm_set1_ps(255));

	__m128i finalRGB;

	//note only if composite lighter is used
	nRed = _mm_min_ps( nRed, _mm_set1_ps(255));
	nGreen = _mm_min_ps( nGreen, _mm_set1_ps(255));
	nBlue = _mm_min_ps( nBlue, _mm_set1_ps(255));
	nAlpha = _mm_min_ps( nAlpha, _mm_set1_ps(255));

	float* r = (float*)&nRed;
	float* g = (float*)&nGreen;
	float* b = (float*)&nBlue;
	float* a = (float*)&nAlpha;
	
	finalRGB = _mm_set_epi8(
		(unsigned char)a[3], (unsigned char)b[3], (unsigned char)g[3], (unsigned char)r[3], 
		(unsigned char)a[2], (unsigned char)b[2], (unsigned char)g[2], (unsigned char)r[2],
		(unsigned char)a[1], (unsigned char)b[1], (unsigned char)g[1], (unsigned char)r[1],
		(unsigned char)a[0], (unsigned char)b[0], (unsigned char)g[0], (unsigned char)r[0]
	);

	return finalRGB;

}
#endif

#if (OPTI>=2)
__m256i Graphics::blend(__m256i src, __m256i dest)
{
	if(compositeRule == NO_COMPOSITE)
	{
		return src;
	}

	__m256 Fa;
	__m256 Fb;

	Color* c1 = (Color*)&src;
	Color* c2 = (Color*)&dest;

	__m256 srcRed, srcGreen, srcBlue, srcAlpha;
	__m256 destRed, destGreen, destBlue, destAlpha;
	
	srcRed = _mm256_set_ps(
		((float)c1[7].red),
		((float)c1[6].red),
		((float)c1[5].red),
		((float)c1[4].red),
		((float)c1[3].red),
		((float)c1[2].red),
		((float)c1[1].red),
		((float)c1[0].red)
	);

	srcGreen = _mm256_set_ps(
		((float)c1[7].green),
		((float)c1[6].green),
		((float)c1[5].green),
		((float)c1[4].green),
		((float)c1[3].green),
		((float)c1[2].green),
		((float)c1[1].green),
		((float)c1[0].green)
	);

	srcBlue = _mm256_set_ps(
		((float)c1[7].blue),
		((float)c1[6].blue),
		((float)c1[5].blue),
		((float)c1[4].blue),
		((float)c1[3].blue),
		((float)c1[2].blue),
		((float)c1[1].blue),
		((float)c1[0].blue)
	);

	srcAlpha = _mm256_set_ps(
		((float)c1[7].alpha),
		((float)c1[6].alpha),
		((float)c1[5].alpha),
		((float)c1[4].alpha),
		((float)c1[3].alpha),
		((float)c1[2].alpha),
		((float)c1[1].alpha),
		((float)c1[0].alpha)
	);

	destRed = _mm256_set_ps(
		((float)c2[7].red),
		((float)c2[6].red),
		((float)c2[5].red),
		((float)c2[4].red),
		((float)c2[3].red),
		((float)c2[2].red),
		((float)c2[1].red),
		((float)c2[0].red)
	);

	destGreen = _mm256_set_ps(
		((float)c2[7].green),
		((float)c2[6].green),
		((float)c2[5].green),
		((float)c2[4].green),
		((float)c2[3].green),
		((float)c2[2].green),
		((float)c2[1].green),
		((float)c2[0].green)
	);

	destBlue = _mm256_set_ps(
		((float)c2[7].blue),
		((float)c2[6].blue),
		((float)c2[5].blue),
		((float)c2[4].blue),
		((float)c2[3].blue),
		((float)c2[2].blue),
		((float)c2[1].blue),
		((float)c2[0].blue)
	);

	destAlpha = _mm256_set_ps(
		((float)c2[7].alpha),
		((float)c2[6].alpha),
		((float)c2[5].alpha),
		((float)c2[4].alpha),
		((float)c2[3].alpha),
		((float)c2[2].alpha),
		((float)c2[1].alpha),
		((float)c2[0].alpha)
	);
	
	srcAlpha = _mm256_div_ps( srcAlpha, _mm256_set1_ps(255));
	destAlpha = _mm256_div_ps( destAlpha, _mm256_set1_ps(255));

	//other stuff to

	switch(compositeRule)
	{
		case COMPOSITE_CLEAR:
			Fa = _mm256_set1_ps(0);
			Fb = _mm256_set1_ps(0);
			break;
		case COMPOSITE_COPY:
			Fa = _mm256_set1_ps(1);
			Fb = _mm256_set1_ps(0);
			break;
		case COMPOSITE_DEST:
			Fa = _mm256_set1_ps(0);
			Fb = _mm256_set1_ps(1);
			break;
		case COMPOSITE_SRC_OVER:
			Fa = _mm256_set1_ps(1);
			Fb = _mm256_sub_ps(_mm256_set1_ps(1), srcAlpha);
			break;
		case COMPOSITE_DEST_OVER:
			Fa = _mm256_sub_ps(_mm256_set1_ps(1), destAlpha);
			Fb = _mm256_set1_ps(1);
			break;
		case COMPOSITE_SRC_IN:
			Fa = destAlpha;
			Fb = _mm256_set1_ps(0);
			break;
		case COMPOSITE_DEST_IN:
			Fa = _mm256_set1_ps(0);
			Fb = srcAlpha;
			break;
		case COMPOSITE_SRC_OUT:
			Fa = _mm256_sub_ps(_mm256_set1_ps(1), destAlpha);
			Fb = _mm256_set1_ps(0);
			break;
		case COMPOSITE_DEST_OUT:
			Fa = _mm256_set1_ps(0);
			Fb = _mm256_sub_ps(_mm256_set1_ps(1), srcAlpha);
			break;
		case COMPOSITE_SRC_ATOP:
			Fa = destAlpha;
			Fb = _mm256_sub_ps(_mm256_set1_ps(1), srcAlpha);
			break;
		case COMPOSITE_DEST_ATOP:
			Fa = _mm256_sub_ps(_mm256_set1_ps(1), destAlpha);
			Fb = srcAlpha;
			break;
		case COMPOSITE_XOR:
			Fa = _mm256_sub_ps(_mm256_set1_ps(1), destAlpha);
			Fb = _mm256_sub_ps(_mm256_set1_ps(1), srcAlpha);
			break;
		case COMPOSITE_LIGHTER:
			Fa = _mm256_set1_ps(1);
			Fb = _mm256_set1_ps(1);
			break;
		default:
			Fa = _mm256_set1_ps(0);
			Fb = _mm256_set1_ps(0);
			break;
	}

	__m256 nRed, nGreen, nBlue, nAlpha;

	__m256 nSrcAlpha = _mm256_mul_ps(Fa, srcAlpha);
	__m256 nDestAlpha = _mm256_mul_ps(Fb, destAlpha);
	
	nRed = _mm256_add_ps( _mm256_mul_ps(srcRed, nSrcAlpha), _mm256_mul_ps(destRed, nDestAlpha) );
	nGreen = _mm256_add_ps( _mm256_mul_ps(srcGreen, nSrcAlpha), _mm256_mul_ps(destGreen, nDestAlpha) );
	nBlue = _mm256_add_ps( _mm256_mul_ps(srcBlue, nSrcAlpha), _mm256_mul_ps(destBlue, nDestAlpha) );
	nAlpha = _mm256_mul_ps(_mm256_add_ps( nSrcAlpha, nDestAlpha ), _mm256_set1_ps(255));

	__m256i finalRGB;

	//note only if composite lighter is used
	nRed = _mm256_min_ps( nRed, _mm256_set1_ps(255));
	nGreen = _mm256_min_ps( nGreen, _mm256_set1_ps(255));
	nBlue = _mm256_min_ps( nBlue, _mm256_set1_ps(255));
	nAlpha = _mm256_min_ps( nAlpha, _mm256_set1_ps(255));

	float* r = (float*)&nRed;
	float* g = (float*)&nGreen;
	float* b = (float*)&nBlue;
	float* a = (float*)&nAlpha;
	
	finalRGB = _mm256_set_epi8(
		(unsigned char)a[7], (unsigned char)b[7], (unsigned char)g[7], (unsigned char)r[7], 
		(unsigned char)a[6], (unsigned char)b[6], (unsigned char)g[6], (unsigned char)r[6],
		(unsigned char)a[5], (unsigned char)b[5], (unsigned char)g[5], (unsigned char)r[5],
		(unsigned char)a[4], (unsigned char)b[4], (unsigned char)g[4], (unsigned char)r[4],
		(unsigned char)a[3], (unsigned char)b[3], (unsigned char)g[3], (unsigned char)r[3], 
		(unsigned char)a[2], (unsigned char)b[2], (unsigned char)g[2], (unsigned char)r[2],
		(unsigned char)a[1], (unsigned char)b[1], (unsigned char)g[1], (unsigned char)r[1],
		(unsigned char)a[0], (unsigned char)b[0], (unsigned char)g[0], (unsigned char)r[0]
	);

	return finalRGB;

}

#endif

#pragma endregion

Color Graphics::lerp(Color src, Color dest, double lerpVal)
{
	Vec4f v1 = Vec4f(src.red, src.green, src.blue, src.alpha);
	Vec4f v2 = Vec4f(dest.red, dest.green, dest.blue, dest.alpha);
	
	Vec4f v3 = v1*(1.0-lerpVal) + v2*(lerpVal);

	return {(unsigned char)MathExt::clamp(v3.x, 0.0, 255.0),
			(unsigned char)MathExt::clamp(v3.y, 0.0, 255.0),
			(unsigned char)MathExt::clamp(v3.z, 0.0, 255.0),
			(unsigned char)MathExt::clamp(v3.w, 0.0, 255.0) };
}

//works properly now
void Graphics::drawRect(int x, int y, int x2, int y2, bool outline, Image* surf)
{
	int currentComposite = compositeRule;
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		int tempWidth = otherImg->getWidth();

		int minX = MathExt::clamp(MathExt::min(x, x2), 0, tempWidth);
		int maxX = MathExt::clamp(MathExt::max(x, x2), 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(MathExt::min(y, y2), 0, tempHeight);
		int maxY = MathExt::clamp(MathExt::max(y, y2), 0, tempHeight);

		if(outline == false)
		{
			#if(OPTI>=2)

				__m256i* avxPoint;
				
				Color* startPoint = (otherImg->getPixels() + minX + (minY * tempWidth));
				Color* endPoint = (otherImg->getPixels() + maxX + ((maxY-1) * tempWidth));
				
				//int startOffset = (maxX - minX) % 4;
				
				int offWidth = (maxX - minX)>>3;
				int remainder = (maxX - minX) - (offWidth<<3);
				int addAmount = (tempWidth - (maxX-minX));

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
				Color* endPoint = (otherImg->getPixels() + maxX + ((maxY-1) * tempWidth));
				
				//int startOffset = (maxX - minX) % 4;
				
				int offWidth = (maxX - minX)/4;
				int remainder = (maxX - minX) - (offWidth*4);
				int addAmount = (tempWidth - (maxX-minX));

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
				Color* endPoint = otherImg->getPixels() + maxX + ((maxY-1) * tempWidth);
				
				int offWidth = maxX - minX;
				int addAmount = tempWidth - offWidth;

				int tX = 0;

				int actualX = minX;
				int actualY = minY;
				
				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						for(int i=0; i<offWidth; i++)
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
						for(int i=0; i<offWidth; i++)
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

void Graphics::drawLine(int x1, int y1, int x2, int y2, Image* surf)
{
	int currentComposite = compositeRule;
	//Will pointers for modifying the data
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		
		//determine if it is on the line
		//Using general equation
		//Ax + By = C
		//In order to avoid holes, swap between solving for x or y depending
		//on which has a greater rate of change. AntiAliasing can be added easily
		//in this case.
		int P1 = y1-y2;
		int P2 = x2-x1;
		int con = x1*y2 - x2*y1;

		if(P1!=0 && P2!=0)
		{
			if(MathExt::abs(P1) >= MathExt::abs(P2))
			{
				//VERTICAL BASED
				int minY, maxY;
				bool dir = (P1>0 || P2>0);
				if(y1 <= y2)
				{
					minY = MathExt::clamp(y1, 0, otherImg->getHeight()-1);
					maxY = MathExt::clamp(y2, 0, otherImg->getHeight()-1);
				}
				else
				{
					minY = MathExt::clamp(y2, 0, otherImg->getHeight()-1);
					maxY = MathExt::clamp(y1, 0, otherImg->getHeight()-1);
				}

				if(currentComposite == NO_COMPOSITE)
				{
					for(int i=minY; i<=maxY; i++)
					{
						//solve with respect to x
						double val = -(con+P2*i) / (double)P1;
						int actualX = (dir) ? MathExt::floor(val+0.5) : MathExt::ceil(val-0.5);
						otherImg->setPixel(actualX, i, Graphics::activeColor);
					}
				}
				else
				{
					for(int i=minY; i<=maxY; i++)
					{
						//solve with respect to x
						double val = -(con+P2*i) / (double)P1;
						int actualX = (dir) ? MathExt::floor(val+0.5) : MathExt::ceil(val-0.5);
						otherImg->drawPixel(actualX, i, Graphics::activeColor);
					}
				}
			}
			else
			{
				//HORIZONTAL BASED
				int minX, maxX;
				bool dir = (P1>0);
				if(x1 <= x2)
				{
					minX = MathExt::clamp(x1, 0, otherImg->getWidth()-1);
					maxX = MathExt::clamp(x2, 0, otherImg->getWidth()-1);
				}
				else
				{
					minX = MathExt::clamp(x2, 0, otherImg->getWidth()-1);
					maxX = MathExt::clamp(x1, 0, otherImg->getWidth()-1);
				}

				if(currentComposite == NO_COMPOSITE)
				{
					for(int i=minX; i<=maxX; i++)
					{
						//solve with respect to y
						double val = -(con+P1*i) / (double)P2;
						int actualY = (dir) ? MathExt::floor(val+0.5) : MathExt::ceil(val-0.5);
						otherImg->setPixel(i, actualY, Graphics::activeColor);
					}
				}
				else
				{
					for(int i=minX; i<=maxX; i++)
					{
						//solve with respect to y
						double val = -(con+P1*i) / (double)P2;
						int actualY = (dir) ? MathExt::floor(val+0.5) : MathExt::ceil(val-0.5);
						otherImg->drawPixel(i, actualY, Graphics::activeColor);
					}

				}
			}
		}
		else if(P2==0)
		{
			//VERTICAL LINE
			int minY, maxY;
			if(y1 <= y2)
			{
				minY = MathExt::clamp(y1, 0, otherImg->getHeight()-1);
				maxY = MathExt::clamp(y2, 0, otherImg->getHeight()-1);
			}
			else
			{
				minY = MathExt::clamp(y2, 0, otherImg->getHeight()-1);
				maxY = MathExt::clamp(y1, 0, otherImg->getHeight()-1);
			}

			if(x1 < 0 || x1 >= otherImg->getWidth())
			{
				return;
			}

			if(currentComposite == NO_COMPOSITE)
			{
				for(int i=minY; i<=maxY; i++)
				{
					otherImg->setPixel(x1, i, Graphics::activeColor);
				}
			}
			else
			{
				for(int i=minY; i<=maxY; i++)
				{
					otherImg->drawPixel(x1, i, Graphics::activeColor);
				}
			}
		}
		else if(P1==0)
		{
			//HORIZONTAL LINE
			int minX, maxX;
			if(x1 <= x2)
			{
				minX = MathExt::clamp(x1, 0, otherImg->getWidth()-1);
				maxX = MathExt::clamp(x2, 0, otherImg->getWidth()-1);
			}
			else
			{
				minX = MathExt::clamp(x2, 0, otherImg->getWidth()-1);
				maxX = MathExt::clamp(x1, 0, otherImg->getWidth()-1);
			}

			if(y1 < 0 || y1 >= otherImg->getHeight())
			{
				return;
			}

			#if(OPTI>=2)

				int avxWidth = (maxX-minX) >> 3;
				int remainder = 8 - (maxX-minX)%8;

				Color* startColor = otherImg->getPixels() + minX + (y1*otherImg->getWidth());
				__m256i* startAVX = (__m256i*)startColor;
				__m256i* endAVX = startAVX + avxWidth;
				
				if(remainder == 8)
				{
					remainder = 0;
				}

				__m256i avxColor = _mm256_set1_epi32( *((int*)&Graphics::activeColor) );
				
				if(currentComposite == NO_COMPOSITE)
				{
					while(startAVX < endAVX)
					{
						_mm256_storeu_si256(startAVX, avxColor);
						startAVX++;
					}

					//fill remainder
					startColor += avxWidth<<3;
					for(int i=0; i<remainder; i++)
					{
						*startColor = Graphics::activeColor;
						startColor++;
					}
				}
				else
				{
					while(startAVX < endAVX)
					{
						__m256i currentColor = _mm256_loadu_si256(startAVX);
						__m256i blendC = blend(avxColor, currentColor);
						_mm256_storeu_si256(startAVX, blendC);
						startAVX++;
					}

					//fill remainder
					startColor += avxWidth<<3;
					for(int i=0; i<remainder; i++)
					{
						*startColor = Graphics::activeColor;
						startColor++;
					}
				}

			#elif(OPTI>=1)
				
				int sseWidth = (maxX-minX) >> 2;
				int remainder = 4 - (maxX-minX)%4;

				Color* startColor = otherImg->getPixels() + minX + (y1*otherImg->getWidth());
				__m128i* startSSE = (__m128i*)startColor;
				__m128i* endSSE = startSSE + sseWidth;
				
				if(remainder == 4)
				{
					remainder = 0;
				}

				__m128i sseColor = _mm_set1_epi32( *((int*)&Graphics::activeColor) );
				
				if(currentComposite == NO_COMPOSITE)
				{
					while(startSSE < endSSE)
					{
						_mm_storeu_si128(startSSE, sseColor);
						startSSE++;
					}

					//fill remainder
					startColor += sseWidth<<2;
					for(int i=0; i<remainder; i++)
					{
						*startColor = Graphics::activeColor;
						startColor++;
					}
				}
				else
				{
					while(startSSE < endSSE)
					{
						__m128i currentColor = _mm_loadu_si128(startSSE);
						__m128i blendC = blend(sseColor, currentColor);
						_mm_storeu_si128(startSSE, blendC);
						startSSE++;
					}

					//fill remainder
					startColor += sseWidth<<2;
					for(int i=0; i<remainder; i++)
					{
						*startColor = blend(Graphics::activeColor, *startColor);
						startColor++;
					}
				}

			#else

				Color* startColor = otherImg->getPixels() + minX + (y1*otherImg->getWidth());
				Color* endColor = otherImg->getPixels() + maxX + (y1*otherImg->getWidth());

				if(currentComposite == NO_COMPOSITE)
				{
					while(startColor <= endColor)
					{
						*startColor = Graphics::activeColor;
						startColor++;
					}
				}
				else
				{
					while(startColor <= endColor)
					{
						*startColor = blend(Graphics::activeColor, *startColor);
						startColor++;
					}
				}

			#endif
			
		}

	}
}

void Graphics::drawCircle(int x, int y, int radius, bool outline, Image* surf)
{
	int currentComposite = compositeRule;

	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		int fakeRad = radius-1;
		int tempWidth = otherImg->getWidth();

		int minX = MathExt::clamp(MathExt::min(x-fakeRad, x+fakeRad), 0, tempWidth);
		int maxX = MathExt::clamp(MathExt::max(x-fakeRad, x+fakeRad), 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(MathExt::min(y-fakeRad, y+fakeRad), 0, tempHeight);
		int maxY = MathExt::clamp(MathExt::max(y-fakeRad, y+fakeRad), 0, tempHeight);
		
		int tX = minX;
		int tY = minY;
		double radSqr = MathExt::sqr(fakeRad);
		
		if (outline == false)
		{
			#if(OPTI>=2)

				Color* startPoint = otherImg->getPixels() + (minY * tempWidth);
				Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

				__m256i* ssePoint;
				
				__m256i sseColor = _mm256_set1_epi32( *((int*)&activeColor) );
				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						double xDisToCenter = MathExt::sqrt( radSqr - MathExt::sqr(tY - y) ) + 0.5;

						int x1 = MathExt::clamp( (int)MathExt::ceil(x-xDisToCenter), 0, tempWidth-1);
						int x2 = MathExt::clamp( (int)MathExt::floor(x+xDisToCenter), 0, tempWidth-1);

						int addAmount = x1;
						int addAmount2 = tempWidth-x2;

						if(xDisToCenter==0)
						{
							startPoint += addAmount;
							*startPoint = activeColor;
							startPoint += addAmount2;
							tY++;
							continue;
						}

						int wid = ((x2-x1) >> 3);
						int remainder = (x2-x1) - (wid<<3);

						startPoint += addAmount;
						ssePoint = (__m256i*)startPoint;

						for(int i=0; i<wid; i++)
						{
							_mm256_storeu_si256(ssePoint, sseColor);
							ssePoint++;
						}

						startPoint += wid<<3;

						for(int i=0; i<remainder; i++)
						{
							*startPoint = activeColor;
							startPoint++;
						}

						tY++;
						startPoint += addAmount2;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						double xDisToCenter = MathExt::sqrt( radSqr - MathExt::sqr(tY - y) ) + 0.5;

						int x1 = MathExt::clamp( (int)MathExt::ceil(x-xDisToCenter), 0, tempWidth-1);
						int x2 = MathExt::clamp( (int)MathExt::floor(x+xDisToCenter), 0, tempWidth-1);

						int addAmount = x1;
						int addAmount2 = tempWidth-x2;

						if(xDisToCenter==0)
						{
							startPoint += addAmount;
							*startPoint = blend(activeColor, *startPoint);
							startPoint += addAmount2;
							tY++;
							continue;
						}

						int wid = ((x2-x1) >> 3);
						int remainder = (x2-x1) - (wid<<3);

						startPoint += addAmount;
						ssePoint = (__m256i*)startPoint;

						for(int i=0; i<wid; i++)
						{
							__m256i currentColor = _mm256_loadu_si256(ssePoint);
							__m256i blendC = blend(sseColor, currentColor);
							_mm256_storeu_si256(ssePoint, blendC);
							ssePoint++;
						}

						startPoint += wid<<3;

						for(int i=0; i<remainder; i++)
						{
							*startPoint = blend(activeColor, *startPoint);
							startPoint++;
						}

						tY++;
						startPoint += addAmount2;
					}
				}

			#elif(OPTI>=1)

				Color* startPoint = otherImg->getPixels() + (minY * tempWidth);
				Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

				__m128i* ssePoint;
				
				__m128i sseColor = _mm_set1_epi32( *((int*)&activeColor) );
				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						double xDisToCenter = MathExt::sqrt( radSqr - MathExt::sqr(tY - y) ) + 0.5;

						int x1 = MathExt::clamp( (int)MathExt::ceil(x-xDisToCenter), 0, tempWidth-1);
						int x2 = MathExt::clamp( (int)MathExt::floor(x+xDisToCenter), 0, tempWidth-1);

						int addAmount = x1;
						int addAmount2 = tempWidth-x2;

						if(xDisToCenter==0)
						{
							startPoint += addAmount;
							*startPoint = activeColor;
							startPoint += addAmount2;
							tY++;
							continue;
						}

						int wid = ((x2-x1) >> 2);
						int remainder = (x2-x1) - (wid<<2);

						startPoint += addAmount;
						ssePoint = (__m128i*)startPoint;

						for(int i=0; i<wid; i++)
						{
							_mm_storeu_si128(ssePoint, sseColor);
							ssePoint++;
						}

						startPoint += wid<<2;

						for(int i=0; i<remainder; i++)
						{
							*startPoint = activeColor;
							startPoint++;
						}

						tY++;
						startPoint += addAmount2;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						double xDisToCenter = MathExt::sqrt( radSqr - MathExt::sqr(tY - y) ) + 0.5;

						int x1 = MathExt::clamp( (int)MathExt::ceil(x-xDisToCenter), 0, tempWidth-1);
						int x2 = MathExt::clamp( (int)MathExt::floor(x+xDisToCenter), 0, tempWidth-1);

						int addAmount = x1;
						int addAmount2 = tempWidth-x2;

						if(xDisToCenter==0)
						{
							startPoint += addAmount;
							*startPoint = blend(activeColor, *startPoint);
							startPoint += addAmount2;
							tY++;
							continue;
						}

						int wid = ((x2-x1) >> 2);
						int remainder = (x2-x1) - (wid<<2);

						startPoint += addAmount;
						ssePoint = (__m128i*)startPoint;

						for(int i=0; i<wid; i++)
						{
							__m128i currentColor = _mm_loadu_si128(ssePoint);
							__m128i blendC = blend(sseColor, currentColor);
							_mm_storeu_si128(ssePoint, blendC);
							ssePoint++;
						}

						startPoint += wid<<2;

						for(int i=0; i<remainder; i++)
						{
							*startPoint = blend(activeColor, *startPoint);
							startPoint++;
						}

						tY++;
						startPoint += addAmount2;
					}
				}

			#else

				Color* startPoint = otherImg->getPixels() + (minY * tempWidth);
				Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

				if(currentComposite == NO_COMPOSITE)
				{
					while (startPoint < endPoint)
					{
						double xDisToCenter = MathExt::sqrt( radSqr - MathExt::sqr(tY - y) ) + 0.5;

						int x1 = MathExt::clamp( (int)MathExt::ceil(x-xDisToCenter), 0, tempWidth-1);
						int x2 = MathExt::clamp( (int)MathExt::floor(x+xDisToCenter), 0, tempWidth-1);

						int addAmount = x1;
						int addAmount2 = tempWidth-x2;

						startPoint += addAmount;
						for(int i=x1; i<=x2; i++)
						{
							*startPoint = activeColor;
							startPoint++;
						}

						tY++;
						startPoint += addAmount-1;
					}
				}
				else
				{
					while (startPoint < endPoint)
					{
						double xDisToCenter = MathExt::sqrt( radSqr - MathExt::sqr(tY - y) ) + 0.5;

						int x1 = MathExt::clamp( (int)MathExt::ceil(x-xDisToCenter), 0, tempWidth-1);
						int x2 = MathExt::clamp( (int)MathExt::floor(x+xDisToCenter), 0, tempWidth-1);

						int addAmount = x1;
						int addAmount2 = tempWidth-x2;

						startPoint += addAmount;
						for(int i=x1; i<=x2; i++)
						{
							*startPoint = blend(activeColor, *startPoint);
							startPoint++;
						}

						tY++;
						startPoint += addAmount2-1;
					}
				}

			#endif

			//draw outline to fill areas not gotten by the normal fill
			int preX1 = -1;
			int preX2 = -1;
			tY = minY;

			while (tY <= maxY)
			{
				double xDisToCenter = MathExt::sqrt( radSqr - MathExt::sqr(tY - y) );

				int x1 = MathExt::clamp( (int)MathExt::round(x-xDisToCenter), 0, tempWidth-1);
				int x2 = MathExt::clamp( (int)MathExt::round(x+xDisToCenter), 0, tempWidth-1);
				
				if(tY != minY)
				{
					//Done for symmetry
					if(tY<y)
					{
						//fill from preX1 to X1
						Graphics::drawLine(preX1, tY-1, x1, tY, otherImg);
						//fill from preX2 to X2
						Graphics::drawLine(preX2, tY-1, x2, tY, otherImg);
					}
					else
					{
						//fill from X1 to preX1
						Graphics::drawLine(x1, tY, preX1, tY-1, otherImg);
						//fill from X2 to preX2
						Graphics::drawLine(x2, tY, preX2, tY-1, otherImg);
					}
				}

				preX1 = x1;
				preX2 = x2;
				tY++;
			}
		}
		else
		{
			int preX1 = -1;
			int preX2 = -1;

			while (tY <= maxY)
			{
				double xDisToCenter = MathExt::sqrt( radSqr - MathExt::sqr(tY - y) );

				int x1 = MathExt::clamp( (int)MathExt::round(x-xDisToCenter), 0, tempWidth-1);
				int x2 = MathExt::clamp( (int)MathExt::round(x+xDisToCenter), 0, tempWidth-1);
				
				if(tY != minY)
				{
					//Done for symmetry
					if(tY<y)
					{
						//fill from preX1 to X1
						Graphics::drawLine(preX1, tY-1, x1, tY, otherImg);
						//fill from preX2 to X2
						Graphics::drawLine(preX2, tY-1, x2, tY, otherImg);
					}
					else
					{
						//fill from X1 to preX1
						Graphics::drawLine(x1, tY, preX1, tY-1, otherImg);
						//fill from X2 to preX2
						Graphics::drawLine(x2, tY, preX2, tY-1, otherImg);
					}
				}

				preX1 = x1;
				preX2 = x2;
				tY++;
			}

		}
	}
}

void Graphics::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool outline, Image* surf)
{
	int currentComposite = compositeRule;
	Image* otherImg;
	if(surf==nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;
	
	if(otherImg!=nullptr)
	{
		Line l1 = Line(x1, y1, x2, y2);
		Line l2 = Line(x2, y2, x3, y3);
		Line l3 = Line(x3, y3, x1, y1);
		
		int arr[3] = {x1, x2, x3};
		int minX = MathExt::min( arr, 3 );
		int maxX = MathExt::max( arr, 3 );
		
		arr[0] = y1; arr[1] = y2; arr[2] = y3;
		int minY = MathExt::min( arr, 3 );
		int maxY = MathExt::max( arr, 3 );
		
		Line constLine;
		Line sLine;
		Line eLine;
		Vec2f pivot = Vec2f();
		
		if( l1.getMinY() == minY && l1.getMaxY() == maxY )
		{
			constLine = l1;
			pivot = Vec2f(x3, y3);
			if(y2 < y1)
			{
				sLine = l2;
				eLine = l3;
			}
			else
			{
				sLine = l3;
				eLine = l2;
			}
		}
		else if( l2.getMinY() == minY && l2.getMaxY() == maxY )
		{
			constLine = l2;
			pivot = Vec2f(x1, y1);
			if(y3 < y2)
			{
				sLine = l3;
				eLine = l1;
			}
			else
			{
				sLine = l1;
				eLine = l3;
			}
		}
		else
		{
			constLine = l3;
			pivot = Vec2f(x2, y2);
			if(y1 < y3)
			{
				sLine = l1;
				eLine = l2;
			}
			else
			{
				sLine = l2;
				eLine = l1;
			}
		}

		int startY = MathExt::clamp(minY, 0, otherImg->getHeight()-1);
		int endY = MathExt::clamp(maxY, 0, otherImg->getHeight()-1);

		bool dir1 = (constLine.getToPoint().y < 0) || (constLine.getToPoint().x > 0);
		bool dir2 = (sLine.getToPoint().y < 0) || (sLine.getToPoint().x > 0);
		bool swapPivot = false;

		for(int y=startY; y<endY; y++)
		{
			double cX1 = 0;
			double cX2 = 0;

			if(y<pivot.y)
			{
				cX1 = constLine.solveForX(y);
				cX2 = sLine.solveForX(y);
			}
			else
			{
				if(!swapPivot)
				{
					dir2 = (eLine.getToPoint().y < 0) || (eLine.getToPoint().x > 0);
					swapPivot=true;
				}

				cX1 = constLine.solveForX(y);
				cX2 = eLine.solveForX(y);
			}

			if(cX1>cX2)
			{
				double old = cX2;
				cX2 = cX1;
				cX1 = old;
			}

			int startX = (dir1) ? MathExt::floor(cX1+0.5) : MathExt::ceil(cX1-0.5);
			int endX = (dir2) ? MathExt::floor(cX2+0.5) : MathExt::ceil(cX2-0.5);

			startX = MathExt::clamp( startX, 0, otherImg->getWidth()-1);
			endX = MathExt::clamp( endX, 0, otherImg->getWidth()-1);

			if(startX > endX)
			{
				//outside the drawing area
				continue;
			}

			#if(OPTI>=2)

				int avxWidth = (endX-startX)>>3;
				int remainder = (endX-startX) - (avxWidth<<3);

				Color* startFill = otherImg->getPixels() + startX + y*otherImg->getWidth();
				Color* endFill = startFill + (endX-startX);

				__m256i* avxFill = (__m256i*)startFill;
				__m256i* avxEnd = avxFill + avxWidth;
				__m256i avxColor = _mm256_set1_epi32( *((int*)&Graphics::activeColor) );

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
						*startFill = Graphics::activeColor;
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
						*startFill = blend(Graphics::activeColor, *startFill);
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
				__m128i sseColor = _mm_set1_epi32( *((int*)&Graphics::activeColor) );

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
						*startFill = Graphics::activeColor;
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
						*startFill = blend(Graphics::activeColor, *startFill);
						startFill++;
					}
				}

			#else

				Color* startFill = otherImg->getPixels() + startX + y*otherImg->getWidth();
				Color* endFill = startFill + (endX-startX);

				//fill from cX1 to cX2
				if(compositeRule == NO_COMPOSITE)
				{
					while(startFill <= endFill)
					{
						*startFill = activeColor;
						startFill++;
					}
				}
				else
				{
					while(startFill <= endFill)
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

void Graphics::drawTexturedTriangle(Vec4f p1, Vec4f p2, Vec4f p3, Image* texture, Image* surf)
{
	int currentComposite = compositeRule;
	Image* otherImg;
	if(surf==nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;
	
	if(otherImg!=nullptr && texture!=nullptr)
	{
		
		Line l1 = Line(p1.x, p1.y, p2.x, p2.y);
		Line l2 = Line(p2.x, p2.y, p3.x, p3.y);
		Line l3 = Line(p3.x, p3.y, p1.x, p1.y);


		double arr[3] = {p1.x, p2.x, p3.x};
		double minX = MathExt::min( arr, 3 );
		double maxX = MathExt::max( arr, 3 );
		
		arr[0] = p1.y; arr[1] = p2.y; arr[2] = p3.y;
		double minY = MathExt::min( arr, 3 );
		double maxY = MathExt::max( arr, 3 );
		
		Line constLine;
		Line sLine;
		Line eLine;
		Vec2f pivot = Vec2f();
		if(l1.getMinY() == minY && l1.getMaxY() == maxY)
		{
			constLine = l1;
			pivot = Vec2f(p3.x, p3.y);
			if(p2.y < p1.y)
			{
				sLine = l2;
				eLine = l3;
			}
			else
			{
				sLine = l3;
				eLine = l2;
			}
		}
		else if(l2.getMinY() == minY && l2.getMaxY() == maxY)
		{
			constLine = l2;
			pivot = Vec2f(p1.x, p1.y);
			if(p3.y < p2.y)
			{
				sLine = l3;
				eLine = l1;
			}
			else
			{
				sLine = l1;
				eLine = l3;
			}
		}
		else
		{
			constLine = l3;
			pivot = Vec2f(p2.x, p2.y);
			if(p1.y < p3.y)
			{
				sLine = l1;
				eLine = l2;
			}
			else
			{
				sLine = l2;
				eLine = l1;
			}
		}

		int startY = MathExt::clamp( (int)MathExt::floor(minY), 0, otherImg->getHeight());
		int endY = MathExt::clamp( (int)MathExt::ceil(maxY), 0, otherImg->getHeight());

		double det = (p2.y-p3.y)*(p1.x-p3.x) + (p3.x-p2.x)*(p1.y-p3.y);
		Color* texturePixels = texture->getPixels();

		bool dir1 = (constLine.getToPoint().y < 0) || (constLine.getToPoint().x > 0);
		bool dir2 = (sLine.getToPoint().y < 0) || (sLine.getToPoint().x > 0);
		bool swapPivot = false;

		for(int y=startY; y<=endY; y++)
		{
			double cX1 = 0;
			double cX2 = 0;

			if(y<pivot.y)
			{
				cX1 = constLine.solveForX(y);
				cX2 = sLine.solveForX(y);
			}
			else
			{
				if(!swapPivot)
				{
					dir2 = (eLine.getToPoint().y < 0) || (eLine.getToPoint().x > 0);
					swapPivot=true;
				}

				cX1 = constLine.solveForX(y);
				cX2 = eLine.solveForX(y);
			}

			if(cX1>cX2)
			{
				double old = cX2;
				cX2 = cX1;
				cX1 = old;
			}

			int startX = (dir1) ? MathExt::floor(cX1+0.5) : MathExt::ceil(cX1-0.5);
			int endX = (dir2) ? MathExt::floor(cX2+0.5) : MathExt::ceil(cX2-0.5);

			startX = MathExt::clamp( startX, 0, otherImg->getWidth()-1);
			endX = MathExt::clamp( endX, 0, otherImg->getWidth()-1);

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

				int avxWidth = (dis+1)>>3;
				int remainder = (dis+1) - (avxWidth<<3);

				__m256i* avxFill = (__m256i*)startFill;
				__m256i* avxEnd = avxFill + avxWidth;
				__m256i avxColor = _mm256_set1_epi32( *((int*)&Graphics::activeColor) );

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
					
					Color c1 = texture->getPixel(xCoords[0], yCoords[0], false);
					Color c2 = texture->getPixel(xCoords[1], yCoords[1], false);
					Color c3 = texture->getPixel(xCoords[2], yCoords[2], false);
					Color c4 = texture->getPixel(xCoords[3], yCoords[3], false);
					Color c5 = texture->getPixel(xCoords[4], yCoords[4], false);
					Color c6 = texture->getPixel(xCoords[5], yCoords[5], false);
					Color c7 = texture->getPixel(xCoords[6], yCoords[6], false);
					Color c8 = texture->getPixel(xCoords[7], yCoords[7], false);
					
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

					Color c = texture->getPixel(uInt, vInt, false);
					
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

				int sseWidth = (dis+1)>>2;
				int remainder = (dis+1) - (sseWidth<<2);

				__m128i* sseFill = (__m128i*)startFill;
				__m128i* sseEnd = sseFill + sseWidth;
				__m128i sseColor = _mm_set1_epi32( *((int*)&Graphics::activeColor) );

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

					Color c = texture->getPixel(uInt, vInt, false);
					
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

				while(startFill <= endFill)
				{
					double blendValue = (double)x / dis;

					double w1 = MathExt::lerp(s1, e1, blendValue);
					double w2 = MathExt::lerp(s2, e2, blendValue);
					double w3 = MathExt::lerp(s3, e3, blendValue);

					double u = p1.z*w1 + p2.z*w2 + p3.z*w3;
					double v = p1.w*w1 + p2.w*w2 + p3.w*w3;
					
					int uInt = (int) MathExt::round(u * (texture->getWidth()));
					int vInt = (int) MathExt::round(v * (texture->getHeight()));

					Color c = texture->getPixel(uInt, vInt, false);
					
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

//Works properly now
void Graphics::drawImage(Image* img, int x, int y, Image* surf)
{
	int currentComposite = compositeRule;

	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr && img!=nullptr)
	{
		int tempWidth = otherImg->getWidth();

		int minX = MathExt::clamp(x, 0, tempWidth);
		int maxX = MathExt::clamp(x+img->getWidth(), 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(y, 0, tempHeight);
		int maxY = MathExt::clamp(y+img->getHeight(), 0, tempHeight);

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
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr && img!=nullptr)
	{
		int tempWidth = otherImg->getWidth();

		int minX = MathExt::clamp(x, 0, tempWidth);
		int maxX = MathExt::clamp(x+img->getWidth(), 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(y, 0, tempHeight);
		int maxY = MathExt::clamp(y+img->getHeight(), 0, tempHeight);

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
						*startPoint = blend(*drawImgStart, *startPoint);
						startPoint++;
						drawImgStart++;
					}
					
					startPoint += otherImgAddX;
					drawImgStart += drawImgAddX;
				}
			}

		#else

			Vec4f colorMult = Vec4f((double)Graphics::activeColor.red / 255.0, (double)Graphics::activeColor.green / 255.0, (double)Graphics::activeColor.blue / 255.0, (double)Graphics::activeColor.alpha / 255.0);

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

//Test to see if it works properly later
/*
void Graphics::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		int tempWidth = otherImg->getWidth();

		int minX = MathExt::clamp(x, 0, tempWidth);
		int maxX = MathExt::clamp(x + imgW, 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(y, 0, tempHeight);
		int maxY = MathExt::clamp(y + imgH, 0, tempHeight);

		Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
		Color* endPoint = otherImg->getPixels() + maxX + ((maxY - 1) * tempWidth);

		int offWidth = maxX - minX;
		int addAmount = tempWidth - offWidth;

		Color* otherStartPoint = img->getPixels() + imgX + (imgY*img->getWidth());
		
		int otherAddAmount = img->getWidth() - imgW;

		int tX = 0;
		
		Vec4f colorMult = Vec4f((double)Graphics::activeColor.red / 255.0, (double)Graphics::activeColor.green / 255.0, (double)Graphics::activeColor.blue / 255.0, (double)Graphics::activeColor.alpha / 255.0);

		while (startPoint < endPoint)
		{
			Color drawC = { (unsigned char) (otherStartPoint->red * colorMult.x),
							(unsigned char) (otherStartPoint->green * colorMult.y),
							(unsigned char) (otherStartPoint->blue * colorMult.z), 
							(unsigned char) (otherStartPoint->alpha * colorMult.w) };
			*startPoint = blend(drawC, *startPoint);

			startPoint++;
			otherStartPoint++;
			tX++;

			if (tX >= offWidth)
			{
				tX = 0;
				startPoint += addAmount;
				otherStartPoint += otherAddAmount;
			}
		}
	}
}
*/

void Graphics::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf)
{
	int currentComposite = compositeRule;
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr && img!=nullptr)
	{
		int tempWidth = otherImg->getWidth();

		int clampImgW = MathExt::clamp(imgW, 0, img->getWidth());
		int clampImgH = MathExt::clamp(imgH, 0, img->getHeight());
		
		int minX = MathExt::clamp(x, 0, tempWidth);
		int maxX = MathExt::clamp(x+clampImgW, 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(y, 0, tempHeight);
		int maxY = MathExt::clamp(y+clampImgH, 0, tempHeight);

		Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
		Color* endPoint = otherImg->getPixels() + maxX + ((maxY-1) * tempWidth);
		
		int drawImgWidth = maxX - minX;
		int otherImgAddX = tempWidth - drawImgWidth;

		Color* drawImgStart = img->getPixels() + imgX + (imgY*img->getWidth());
		int drawImgAddX = img->getWidth() - drawImgWidth;

		int tX = 0;

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
						*startPoint = blend(*drawImgStart, *startPoint);
						startPoint++;
						drawImgStart++;
					}
					
					startPoint += otherImgAddX;
					drawImgStart += drawImgAddX;
				}
			}

		#else

			Vec4f colorMult = Vec4f((double)Graphics::activeColor.red / 255.0, (double)Graphics::activeColor.green / 255.0, (double)Graphics::activeColor.blue / 255.0, (double)Graphics::activeColor.alpha / 255.0);

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

void Graphics::drawText(std::string str, int x, int y, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	Font* tFont = activeFont;
	if(tFont == nullptr)
	{
		switch (defaultFontValue)
		{
		case Graphics::NORMAL_FONT:
			tFont = defaultFont;
			break;
		case Graphics::LARGE_FONT:
			tFont = defaultFontLarge;
			break;
		default:
			break;
		}
	}

	if (otherImg != nullptr && tFont != nullptr)
	{
		int currX = x;
		int currY = y;
		for(int i=0; i<str.length(); i++)
		{
			int charIndex = tFont->getCharIndex(str[i]);
			Image* charImg = tFont->getImage(charIndex);
			FontCharInfo fci = tFont->getFontCharInfo(str[i]);
			
			if(charImg == nullptr)
			{
				continue;
			}

			if(str[i] == '\n')
			{
				currX = x;
				currY += fci.height;
				continue;
			}

			if(str[i] == ' ')
			{
				currX += fci.horizAdv;
				continue;
			}

			if(fci.x!=0 || fci.y!=0 || fci.width != charImg->getWidth() || fci.height != charImg->getHeight())
				drawSpritePart(charImg, currX+fci.xOffset, currY+fci.yOffset, fci.x, fci.y, fci.width, fci.height, otherImg);
			else
				drawSprite(charImg, currX+fci.xOffset, currY+fci.yOffset, otherImg);
			
			currX += fci.horizAdv;
		}
	}
}

void Graphics::drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	Font* tFont = activeFont;
	if(tFont == nullptr)
	{
		switch (defaultFontValue)
		{
		case Graphics::NORMAL_FONT:
			tFont = defaultFont;
			break;
		case Graphics::LARGE_FONT:
			tFont = defaultFontLarge;
			break;
		default:
			break;
		}
	}

	if (otherImg != nullptr && tFont != nullptr)
	{
		int currX = x;
		int currY = y;
		int currW = 0;
		int currH = 0;
		
		for(int i=0; i<str.length(); i++)
		{
			int charIndex = tFont->getCharIndex(str[i]);
			Image* charImg = tFont->getImage(charIndex);
			FontCharInfo fci = tFont->getFontCharInfo(str[i]);
			
			if(charImg == nullptr)
			{
				continue;
			}
			
			if(str[i] == '\n')
			{
				currX = x;
				currW = 0;
				currH += fci.height;
				currY += fci.height;

				if(currH >= maxHeight)
				{
					break;
				}
				continue;
			}

			if(currW + fci.width > maxWidth)
			{
				fci.width = maxWidth - currW;
			}
			if(currH + fci.height > maxHeight)
			{
				fci.height = maxHeight - currH;
			}

			if(str[i] != ' ')
			{
				if(fci.x!=0 || fci.y!=0 || fci.width != charImg->getWidth() || fci.height != charImg->getHeight())
					drawSpritePart(charImg, currX+fci.xOffset, currY+fci.yOffset, fci.x, fci.y, fci.width, fci.height, otherImg);
				else
					drawSprite(charImg, currX+fci.xOffset, currY+fci.yOffset, otherImg);
				
				currX += fci.horizAdv;
				currW += fci.horizAdv;
			}
			else
			{
				currX += fci.horizAdv;
				currW += fci.horizAdv;
			}

			if(currW >= maxWidth)
			{
				currW = 0;
				currX = x;
				currH += fci.height;
				currY += fci.height;
			}

			if(currH >= maxHeight)
			{
				break;
			}
		}
	}
}

void Graphics::drawPolygon(Vec2f* points, int size, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		//similar in nature to the vector graphic version
		//always connects the last point to the first

		std::vector<Vec2f> actualPoints = std::vector<Vec2f>();
		for(int i=0; i<size; i++)
		{
			actualPoints.push_back(points[i]);
		}
		actualPoints.push_back(points[0]);
		
		int minY = (int)points[0].y;
		int maxY = (int)points[0].y;

		for(int i=0; i<size; i++)
		{
			if((int)points[i].y < minY)
			{
				minY = (int)points[i].y;
			}
			if((int)points[i].y > maxY)
			{
				maxY = (int)points[i].y;
			}
		}

		minY = MathExt::clamp(minY, 0, otherImg->getHeight()-1);
		maxY = MathExt::clamp(maxY, 0, otherImg->getHeight()-1);

		int yDis = 1+maxY-minY;
		struct criticalPoint
		{
			int xValue;
			bool direction;
		};

		std::vector<criticalPoint>* scanLines = new std::vector<criticalPoint>[yDis];

		for(int i=0; i<size; i++)
		{
			//Ax + By + C = 0
			//A=(y1-y2)
			//B=(x2-x1)
			//C=(x1*y2 - x2*y1)
			double A = (actualPoints[i].y - actualPoints[i+1].y);
			double B = (actualPoints[i+1].x - actualPoints[i].x);
			double C = (actualPoints[i].x*actualPoints[i+1].y - actualPoints[i+1].x*actualPoints[i].y);
			bool posDir = true;

			//AFAIK, if the y is going up in some way, the direction is positive
			//otherwise, it is negative.
			if(A<=0)
			{
				posDir = true;
			}
			else
			{
				posDir = false;
			}
			
			if(A!=0)
			{
				//vertical line or some other valid line
				int thisMinY = MathExt::ceil( MathExt::min( actualPoints[i].y, actualPoints[i+1].y));
				int thisMaxY = MathExt::floor( MathExt::max( actualPoints[i].y, actualPoints[i+1].y));

				thisMinY = MathExt::clamp(thisMinY, minY, maxY);
				thisMaxY = MathExt::clamp(thisMaxY, minY, maxY);

				for(int j=thisMinY; j<=thisMaxY; j++)
				{
					int xValue = MathExt::round(-(B*j + C) / A);
					xValue = MathExt::clamp(xValue, 0, otherImg->getWidth());
					
					scanLines[j-minY].push_back({xValue,posDir});
				}
			}
			else
			{
				//horizontal line or invalid
				//do nothing
			}
		}

		//sort the points on the scanlines
		for(int i=0; i<yDis; i++)
		{
			if(scanLines[i].size()>0)
			{
				Sort::insertionSort<criticalPoint>(scanLines[i].data(), scanLines[i].size(), [](criticalPoint a, criticalPoint b) -> bool {return a.xValue<b.xValue;});
				
				//rule, can not be the same if you are filling to it.
				//different for even-odd and non-zero
				std::vector<criticalPoint> newScanLine = std::vector<criticalPoint>();

				if(fillRule == Graphics::FILL_EVEN_ODD)
				{
					for(int j=1; j<scanLines[i].size(); j+=2)
					{
						if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
						{
							newScanLine.push_back(scanLines[i][j-1]);
							newScanLine.push_back(scanLines[i][j]);
						}
						else
						{
							if(scanLines[i][j].direction != scanLines[i][j-1].direction)
							{
								newScanLine.push_back(scanLines[i][j-1]);
								newScanLine.push_back(scanLines[i][j]);
							}
						}
					}
				}
				else
				{
					newScanLine.push_back(scanLines[i][0]);
					for(int j=1; j<scanLines[i].size(); j++)
					{
						if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
						{
							newScanLine.push_back(scanLines[i][j]);
						}
						else
						{
							if(scanLines[i][j].direction != scanLines[i][j-1].direction)
							{
								newScanLine.push_back(scanLines[i][j]);
							}
						}
					}
				}

				scanLines[i] = newScanLine;
			}
		}

		if(fillRule==Graphics::FILL_EVEN_ODD)
		{
			//even-odd rule. Fill between even to odd indicies
			//not odd to even indicies
			for(int j=0; j<yDis; j++)
			{
				int actualSize = scanLines[j].size();
				if(actualSize%2 == 1)
				{
					actualSize-=2;
				}
				else
				{
					actualSize-=1;
				}
				

				for(int i=0; i<actualSize; i+=2)
				{
					//fill between spots
					int startX = scanLines[j][i].xValue;
					int endX = scanLines[j][i+1].xValue;

					int fillX = startX;

					do
					{
						otherImg->drawPixel(fillX, j+minY, Graphics::activeColor);
						fillX++;
					} while (fillX<=endX);
					
				}
			}
			
		}
		else
		{
			//non-zero rule. Fill when positive only. Not when zero or
			//less.
			for(int j=0; j<yDis; j++)
			{
				int passCounter = 0;
				for(int i=0; i<scanLines[j].size()-1; i++)
				{
					//fill between spots
					int startX = scanLines[j][i].xValue;
					int endX = scanLines[j][i+1].xValue;
					if(scanLines[j][i].direction == true)
					{
						//positive y direction
						passCounter++;
					}
					else
					{
						//negative y direction
						passCounter--;
					}
					
					if(passCounter!=0)
					{
						int fillX = startX;

						do
						{
							otherImg->drawPixel(fillX, j+minY, Graphics::activeColor);
							fillX++;
						} while (fillX<=endX);
						
					}
				}
			}
		}

	}

}

void Graphics::drawModel(Model* model, Image* texture, Image* surf)
{
	Image* otherImg = surf;
	if(otherImg==nullptr)
	{
		otherImg = activeImage;
	}

	if(otherImg!=nullptr)
	{

	}
}

void Graphics::setColor(Color c)
{
	activeColor = c;
}

Color Graphics::getColor()
{
	return activeColor;
}

void Graphics::setFont(Font* f)
{
	activeFont = f;
}

Font* Graphics::getFont()
{
	return activeFont;
}

void Graphics::setDefaultFont(unsigned char byte)
{
	defaultFontValue = MathExt::max(byte, (unsigned char)1);
}

Font* Graphics::getDefaultFont(unsigned char byte)
{
	switch (byte)
	{
	case NORMAL_FONT:
		return defaultFont;
		break;
	case LARGE_FONT:
		return defaultFontLarge;
		break;
	default:
		break;
	}
	return nullptr;
}

void Graphics::setFillRule(bool v)
{
	Graphics::fillRule = v;
}

bool Graphics::getFillRule()
{
	return fillRule;
}

void Graphics::setAntiAliasing(bool v)
{
	Graphics::antiAliasing = v;
}

bool Graphics::getAntiAliasing()
{
	return antiAliasing;
}

void Graphics::setCompositeRule(unsigned char b)
{
	compositeRule = b;
}

unsigned char Graphics::getCompositeRule()
{
	return compositeRule;
}

void Graphics::setBlendMode(unsigned char b)
{
	blendMode = b;
}

unsigned char Graphics::getBlendMode()
{
	return blendMode;
}

#pragma endregion

#pragma region IMAGE_MANIPULATION
int Graphics::ditherMatrixSize = 2;

void Graphics::replaceColor(Image* img, Color oldColor, Color newColor, bool ignoreAlpha)
{
	if(img!=nullptr)
	{
		Color* startPix = img->getPixels();
		Color* endPix = startPix + (img->getWidth() * img->getHeight());
		while(startPix < endPix)
		{
			bool same = true;

			if(startPix->red != oldColor.red)
				same = false;
			if(startPix->green != oldColor.green)
				same = false;
			if(startPix->blue != oldColor.blue)
				same = false;
			
			if(!ignoreAlpha)
			{
				if(startPix->alpha != oldColor.alpha)
					same = false;
			}

			if(!same)
			{
				*startPix = newColor;
			}
			startPix++;
		}
	}
}

void Graphics::ditherImage(Image* img, unsigned char method)
{
	if(img!=nullptr)
	{
		if(img->getPalette().getSize()>0)
		{
			switch (method)
			{
			case ORDERED_DITHER_BAYER:
				orderedBayerDithering(img);
				break;
			case FLOYD_DITHER:
				floydSteinburgDithering(img);
				break;
			default:
				img->enforcePalette();
				break;
			}
		}
	}
}

void Graphics::floydSteinburgDithering(Image* img)
{
	int wid = img->getWidth();
	int hei = img->getHeight();
	Vec3f* error = new Vec3f[wid*hei];
	for(int y=0; y<hei; y++)
	{
		for(int x=0; x<wid; x++)
		{
			Color c = img->getPixel(x,y);
			c.red = (unsigned char)MathExt::clamp((int) ((double)c.red + error[x+y*wid].x), 0, 255);
			c.green = (unsigned char)MathExt::clamp((int) ((double)c.green + error[x+y*wid].y), 0, 255);
			c.blue = (unsigned char)MathExt::clamp((int) ((double)c.blue + error[x+y*wid].z), 0, 255);
			
			Color c2 = img->getPalette().getClosestColor( c );
			c2.alpha = c.alpha;
			
			img->setPixel(x,y,c2);
			Vec3f v1 = Vec3f();
			v1.x = (int)c.red - (int)c2.red;
			v1.y = (int)c.green - (int)c2.green;
			v1.z = (int)c.blue - (int)c2.blue;
			
			if(x!=wid-1)
			{
				error[(x + 1) + y*wid] += v1*(7.0/16.0);

				if(y!=hei-1)
					error[(x + 1) + (y+1)*wid] += v1*(1.0/16.0);
			}

			if(y!=hei-1)
			{
				error[x + (y+1)*wid] += v1*(5.0/16.0);

				if(x!=0)
					error[(x - 1) + (y+1)*wid] += v1*(3.0/16.0);
			}

		}
	}

	delete[] error;
}

void Graphics::orderedBayerDithering(Image* img)
{
	int rows = (int)MathExt::ceil(MathExt::sqrt(img->getPalette().getSize()));
	int size = (int)MathExt::sqr(rows);
	double exp = MathExt::log(size, 2.0);

	Matrix bayerMatrix = generateBayerMatrix(Matrix(), rows);

	bayerMatrix *= 1.0/size;
	double r = 255.0/((double)exp/3.0);

	for(int y=0; y<img->getHeight(); y++)
	{
		for(int x=0; x<img->getWidth(); x++)
		{
			int matX = x%rows;
			int matY = y%rows;

			Color c = img->getPixel(x,y);
			double multVal = bayerMatrix[matY][matX] - 0.5;
			double addVal = r * multVal;

			c.red = (unsigned char)MathExt::clamp((int) ((double)c.red + addVal), 0, 255);
			c.green = (unsigned char)MathExt::clamp((int) ((double)c.green + addVal), 0, 255);
			c.blue = (unsigned char)MathExt::clamp((int) ((double)c.blue + addVal), 0, 255);
			
			Color c2 = img->getPalette().getClosestColor( c );
			
			img->setPixel(x,y,c2);
		}
	}
}

Matrix Graphics::generateBayerMatrix(Matrix mat, int rowSize)
{
	Matrix mat2;
	if(mat.getCols() == 0 || mat.getRows() == 0)
	{
		mat2 = Matrix(2, 2);
		mat2[0][0] = 0;
		mat2[0][1] = 2;
		mat2[1][0] = 3;
		mat2[1][1] = 1;
	}
	else
	{
		mat2 = Matrix(mat.getRows()*2, mat.getCols()*2);

		int inc = mat.getRows();
		for(int y=0; y<mat.getRows(); y++)
		{
			for(int x=0; x<mat.getCols(); x++)
			{
				mat2[y][x] = 4*mat[y][x] + 0;
				mat2[y][x+inc] = 4*mat[y][x] + 2;
				mat2[y+inc][x] = 4*mat[y][x] + 3;
				mat2[y+inc][x+inc] = 4*mat[y][x] + 1;
			}
		}
	}

	if(mat2.getRows() == rowSize)
	{
		return mat2;
	}
	else
	{
		return generateBayerMatrix(mat2, rowSize);
	}
}

Image* Graphics::scaleImage(Image* img, double xScale, double yScale, unsigned char filterMethod)
{
	switch(filterMethod)
	{
		case NEAREST_NEIGHBOR_FILTER:
			return scaleNearestNeighbor(img, xScale, yScale);
			break;
		case BILINEAR_FILTER:
			return scaleBilinear(img, xScale, yScale);
			break;
		case BICUBIC_FILTER:
			return scaleBicubic(img, xScale, yScale);
			break;
		default:
			return scaleBilinear(img, xScale, yScale);
			break;
	}

	return nullptr;
}

Image* Graphics::scaleNearestNeighbor(Image* img, double xScale, double yScale)
{
	if(img!=nullptr)
	{
		int nWidth = img->getWidth() * xScale;
		int nHeight = img->getHeight() * yScale;
		Image* sImg = new Image( nWidth, nHeight );

		//position top left of image so that it has an equal amount of distance from edges
		//divide distance by size of image to find offset

		double xOffset = (double)(nWidth - img->getWidth())/ (2*img->getWidth());
		double yOffset = (double)(nHeight - img->getHeight())/ (2*img->getHeight());

		for(int i=0; i<nHeight; i++)
		{
			double yP = ((double)(i-yOffset) / nHeight) * img->getHeight();
			int nearY = MathExt::clamp(MathExt::round(yP), 0.0, (double)img->getHeight()-1);
			
			for(int i2=0; i2<nWidth; i2++)
			{
				double xP = ((double)(i2-xOffset) / nWidth) * img->getWidth();
				int nearX = MathExt::clamp(MathExt::round(xP), 0.0, (double)img->getWidth()-1);
				
				sImg->setPixel(i2, i, img->getPixel(nearX, nearY));
			}
		}

		return sImg;
	}
	return nullptr;
}

Image* Graphics::scaleBilinear(Image* img, double xScale, double yScale)
{
	if(img!=nullptr)
	{
		int nWidth = img->getWidth() * xScale;
		int nHeight = img->getHeight() * yScale;
		
		//position top left of image so that it has an equal amount of distance from edges
		//divide distance by size of image to find offset

		double xOffset = (double)(nWidth - img->getWidth())/ (2*img->getWidth());
		double yOffset = (double)(nHeight - img->getHeight())/ (2*img->getHeight());

		Image* sImg = new Image( nWidth, nHeight );

		for(int i=0; i<nHeight; i++)
		{
			double yP = ((double)(i-yOffset) / nHeight) * img->getHeight();
			//yP -= yOffset;

			double yFrac = MathExt::frac(yP);

			int y1 = MathExt::clamp(MathExt::floor(yP), 0.0, (double)img->getHeight()-1);
			int y2 = MathExt::clamp(MathExt::ceil(yP), 0.0, (double)img->getHeight()-1);
			
			for(int i2=0; i2<nWidth; i2++)
			{
				double xP = ((double)(i2-xOffset) / nWidth) * img->getWidth();
				//xP -= xOffset;
				double xFrac = MathExt::frac(xP);

				int x1 = MathExt::clamp(MathExt::floor(xP), 0.0, (double)img->getWidth()-1);
				int x2 = MathExt::clamp(MathExt::ceil(xP), 0.0, (double)img->getWidth()-1);
				
				Color c1 = lerp( img->getPixel(x1, y1), img->getPixel(x2, y1), xFrac);
				Color c2 = lerp( img->getPixel(x1, y2), img->getPixel(x2, y2), xFrac);
				Color c3 = lerp( c1, c2, yFrac);

				sImg->setPixel(i2, i, c3);
			}
		}

		return sImg;
	}
	return nullptr;
}

Image* Graphics::scaleBicubic(Image* img, double xScale, double yScale)
{
	if(img!=nullptr)
	{
		int nWidth = img->getWidth() * xScale;
		int nHeight = img->getHeight() * yScale;
		
		//position top left of image so that it has an equal amount of distance from edges
		//divide distance by size of image to find offset

		double xOffset = (double)(nWidth - img->getWidth())/ (2*img->getWidth());
		double yOffset = (double)(nHeight - img->getHeight())/ (2*img->getHeight());


		Image* sImg = new Image( nWidth, nHeight );

		for(int i=0; i<nHeight; i++)
		{
			double yP = ((double)(i-yOffset) / nHeight) * img->getHeight();
			double yFrac = MathExt::frac(yP);

			int yPoints[4];
			yPoints[1] = MathExt::clamp(MathExt::floor(yP), 0.0, (double)img->getHeight()-1);
			yPoints[0] = MathExt::clamp(yPoints[1]-1, 0, img->getHeight()-1);
			yPoints[2] = MathExt::clamp(MathExt::ceil(yP), 0.0, (double)img->getHeight()-1);
			yPoints[3] = MathExt::clamp(yPoints[2]+1, 0, img->getHeight()-1);

			if(yPoints[0]<0)
			{
				yPoints[0] = 0;
			}
			if(yPoints[3]>=img->getHeight())
			{
				yPoints[3] = img->getHeight()-1;
			}
			
			for(int i2=0; i2<nWidth; i2++)
			{
				double xP = ((double)(i2-xOffset) / nWidth) * img->getWidth();

				double xFrac = MathExt::frac(xP);

				int xPoints[4];
				xPoints[1] = MathExt::clamp(MathExt::floor(xP), 0.0, (double)img->getWidth()-1);
				xPoints[0] = MathExt::clamp(xPoints[1]-1, 0, img->getWidth()-1);
				xPoints[2] = MathExt::clamp(MathExt::ceil(xP), 0.0, (double)img->getWidth()-1);
				xPoints[3] = MathExt::clamp(xPoints[2]+1, 0, img->getWidth()-1);

				if(xPoints[0]<0)
				{
					xPoints[0] = 0;
				}
				if(xPoints[3]>=img->getWidth())
				{
					xPoints[3] = img->getWidth()-1;
				}
				
				Vec4f arr[16];
				for(int j=0; j<16; j++)
				{
					int xV = j%4;
					int yV = j/4;
					Color c = img->getPixel(xPoints[xV], yPoints[yV]);
					arr[j] = Vec4f(c.red, c.green, c.blue, c.alpha);
				}

				Vec4f polys[4];
				for(int j=0; j<4; j++)
				{
					Vec4f a, b, c, d;
					a = (arr[j*4 + 0]*-0.5) + (arr[j*4 + 1]*1.5) + (arr[j*4 + 2]*-1.5) + (arr[j*4 + 3]*0.5);
					b = (arr[j*4 + 0]) + (arr[j*4 + 1]*-2.5) + (arr[j*4 + 2]*2) + (arr[j*4 + 3]*-0.5);
					c = (arr[j*4 + 0]*-0.5) + (arr[j*4 + 2]*0.5);
					d = arr[j*4 + 1];

					polys[j] = ((a*xFrac + b)*xFrac + c)*xFrac + d;
				}
				
				Vec4f a, b, c, d;
				a = (polys[0]*-0.5) + (polys[1]*1.5) + (polys[2]*-1.5) + (polys[3]*0.5);
				b = (polys[0]) + (polys[1]*-2.5) + (polys[2]*2) + (polys[3]*-0.5);
				c = (polys[0]*-0.5) + (polys[2]*0.5);
				d = polys[1];

				Vec4f finalC = ((a*yFrac + b)*yFrac + c)*yFrac + d;
				
				unsigned char red = (unsigned char)MathExt::clamp(finalC.x, 0.0, 255.0);
				unsigned char green = (unsigned char)MathExt::clamp(finalC.y, 0.0, 255.0);
				unsigned char blue = (unsigned char)MathExt::clamp(finalC.z, 0.0, 255.0);
				unsigned char alpha = (unsigned char)MathExt::clamp(finalC.w, 0.0, 255.0);
				
				sImg->setPixel(i2, i, {red,green,blue,alpha});
			}
		}

		return sImg;
	}
	return nullptr;
}



#pragma endregion