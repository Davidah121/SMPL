#include "InternalGraphicsHeader.h"

namespace glib
{

	#pragma region DRAWING_FUNCTIONS

	Color Graphics::activeColor = { 0, 0, 0, 255 };
	Font* Graphics::activeFont = nullptr;
	unsigned char Graphics::compositeRule = Graphics::COMPOSITE_SRC_OVER;
	unsigned char Graphics::blendMode = Graphics::BLEND_NORMAL;
	bool Graphics::fillRule = Graphics::FILL_EVEN_ODD;
	bool Graphics::antiAliasing = false;

	unsigned char Graphics::defaultFontValue = Graphics::NORMAL_FONT;
	Font* Graphics::defaultFont = nullptr;
	Font* Graphics::defaultFontMedium = nullptr;
	Font* Graphics::defaultFontLarge = nullptr;

	Box2D Graphics::clippingRect = Box2D(0, 0, 0xFFFF, 0xFFFF);

	void Graphics::init()
	{
		if(defaultFont==nullptr)
			Graphics::defaultFont = new BitmapFont("./Resources/DefaultFont.fnt");

		if(defaultFontMedium==nullptr)
			Graphics::defaultFontMedium = new BitmapFont("./Resources/DefaultFontMedium.fnt");

		if(defaultFontLarge==nullptr)
			Graphics::defaultFontLarge = new BitmapFont("./Resources/DefaultFontLarge.fnt");
	}

	void Graphics::dispose()
	{
		if(defaultFont!=nullptr)
			delete defaultFont;

		if(defaultFontMedium!=nullptr)
			delete defaultFontMedium;

		if(defaultFontLarge!=nullptr)
			delete defaultFontLarge;
		
		defaultFont = nullptr;
		defaultFontMedium = nullptr;
		defaultFontLarge = nullptr;
	}

	void Graphics::clearImage(Image* surf)
	{
		if (surf != nullptr)
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
		Image* otherImg = surf;

		if (otherImg == nullptr)
			return;

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

	void Graphics::setClippingRect(Box2D b)
	{
		clippingRect = b;
	}

	Box2D Graphics::getClippingRect()
	{
		return clippingRect;
	}

	void Graphics::resetClippingPlane()
	{
		clippingRect.setLeftBound(0);
		clippingRect.setRightBound(0xFFFF);
		clippingRect.setTopBound(0);
		clippingRect.setBottomBound(0xFFFF);
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
		if(activeFont!=nullptr)
			return activeFont;
		else
		{
			return Graphics::getDefaultFont(defaultFontValue);
		}
	}

	void Graphics::setDefaultFont(unsigned char byte)
	{
		defaultFontValue = MathExt::clamp(byte, (unsigned char)0, (unsigned char)2);
	}

	Font* Graphics::getDefaultFont(unsigned char byte)
	{
		switch (byte)
		{
		case NORMAL_FONT:
			return defaultFont;
			break;
		case MEDIUM_FONT:
			return defaultFontMedium;
			break;
		case LARGE_FONT:
			return defaultFontLarge;
			break;
		default:
			break;
		}
		return defaultFont;
	}

	void Graphics::setFillRule(bool v)
	{
		Graphics::fillRule = v;
	}

	bool Graphics::getFillRule()
	{
		return fillRule;
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

	void Graphics::setAntiAliasing(bool v)
	{
		antiAliasing = v;
	}

	bool Graphics::getAntiAliasing()
	{
		return antiAliasing;
	}

	#pragma endregion

	

} //NAMESPACE glib END