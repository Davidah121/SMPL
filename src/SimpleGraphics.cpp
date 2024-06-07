#include "InternalGraphicsHeader.h"

namespace smpl
{

	#pragma region DRAWING_FUNCTIONS

	Color SimpleGraphics::activeColor = { 0, 0, 0, 255 };
	Font* SimpleGraphics::activeFont = nullptr;
	unsigned char SimpleGraphics::compositeRule = SimpleGraphics::COMPOSITE_SRC_OVER;
	unsigned char SimpleGraphics::blendMode = SimpleGraphics::BLEND_NORMAL;
	bool SimpleGraphics::fillRule = SimpleGraphics::FILL_EVEN_ODD;
	bool SimpleGraphics::antiAliasing = false;

	unsigned char SimpleGraphics::defaultFontValue = SimpleGraphics::NORMAL_FONT;
	Font* SimpleGraphics::defaultFont = nullptr;
	Font* SimpleGraphics::defaultFontMedium = nullptr;
	Font* SimpleGraphics::defaultFontLarge = nullptr;

	Box2D SimpleGraphics::clippingRect = Box2D(0, 0, 0xFFFF, 0xFFFF);

	SimpleGraphics SimpleGraphics::g = SimpleGraphics();

	SimpleGraphics::SimpleGraphics()
	{

	}

	SimpleGraphics::~SimpleGraphics()
	{
		dispose();
	}

	void SimpleGraphics::init()
	{
		if(defaultFont==nullptr)
			SimpleGraphics::defaultFont = new BitmapFont("./Resources/DefaultFont.fnt");

		if(defaultFontMedium==nullptr)
			SimpleGraphics::defaultFontMedium = new BitmapFont("./Resources/DefaultFontMedium.fnt");

		if(defaultFontLarge==nullptr)
			SimpleGraphics::defaultFontLarge = new BitmapFont("./Resources/DefaultFontLarge.fnt");
	}

	void SimpleGraphics::dispose()
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

	void SimpleGraphics::clearImage(Image* surf)
	{
		if (surf != nullptr)
		{
			surf->setAllPixels(activeColor);
		}
	}

	Vec4f SimpleGraphics::convertColorToVec4f(Color c)
	{
		Vec4f result;
		result.x = (double)c.red / 255.0;
		result.y = (double)c.green / 255.0;
		result.z = (double)c.blue / 255.0;
		result.w = (double)c.alpha / 255.0;
		return result;
	}
	
	Color4f SimpleGraphics::convertColorToColor4f(Color c)
	{
		Color4f result;
		result.red = (double)c.red / 255.0;
		result.green = (double)c.green / 255.0;
		result.blue = (double)c.blue / 255.0;
		result.alpha = (double)c.alpha / 255.0;
		return result;
	}

	Color SimpleGraphics::convertVec4fToColor(Vec4f v)
	{
		Color result;
		result.red = (unsigned char)MathExt::clamp(v.x*255, 0.0, 255.0);
		result.green = (unsigned char)MathExt::clamp(v.y*255, 0.0, 255.0);
		result.blue = (unsigned char)MathExt::clamp(v.z*255, 0.0, 255.0);
		result.alpha = (unsigned char)MathExt::clamp(v.w*255, 0.0, 255.0);
		return result;
	}
	
	Color SimpleGraphics::convertColor4fToColor(Color4f c)
	{
		Color result;
		result.red = (unsigned char)((float)c.red*255);
		result.green = (unsigned char)((float)c.green*255);
		result.blue = (unsigned char)((float)c.blue*255);
		result.alpha = (unsigned char)((float)c.alpha*255);
		return result;
	}

	void SimpleGraphics::drawPixel(double x, double y, Color c, Image* surf)
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
	
	void SimpleGraphics::drawPixel(double x, double y, Color4f c, HiResImage* surf)
	{
		//convert into four separate pixels
		Vec2f p1 = Vec2f(floor(x), floor(y));
		Vec2f p2 = Vec2f(ceil(x), ceil(y));

		double xAlpha = 1.0f - (x-p1.x);
		double yAlpha = 1.0f - (y-p1.y);

		Color4f c1, c2, c3, c4;
		c1 = c;
		c1.alpha = (c1.alpha * xAlpha * yAlpha);

		c2 = c;
		c2.alpha = (c2.alpha * (1-xAlpha) * yAlpha);
		
		c3 = c;
		c3.alpha = (c3.alpha * (1-xAlpha) * (1-yAlpha));
		
		c4 = c;
		c4.alpha = (c4.alpha * xAlpha * (1-yAlpha));
		
		drawPixel( (int)p1.x, (int)p1.y, c1, surf);
		drawPixel( (int)p2.x, (int)p1.y, c2, surf);
		drawPixel( (int)p2.x, (int)p2.y, c3, surf);
		drawPixel( (int)p1.x, (int)p2.y, c4, surf);
	}

	void SimpleGraphics::drawPixel(int x, int y, Color c, Image* surf)
	{
		Image* otherImg = surf;

		if (otherImg == nullptr)
			return;

		if(compositeRule == NO_COMPOSITE)
		{
			otherImg->setPixel(x, y, c);
		}
		else
		{
			Color newColor = blend(c, otherImg->getPixel(x,y));
			otherImg->setPixel(x, y, newColor);
		}
	}

	void SimpleGraphics::drawPixel(int x, int y, Color4f c, HiResImage* surf)
	{
		if (surf == nullptr)
			return;

		if(compositeRule == NO_COMPOSITE)
		{
			surf->setPixel(x, y, c);
		}
		else
		{
			Color4f newColor = blend(c, surf->getPixel(x,y));
			surf->setPixel(x, y, newColor);
		}
	}
	

	Color SimpleGraphics::blend(Color src, Color dest)
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

		red = (int)MathExt::round( (tAlpha1 * src.red * Fa) + (tAlpha2 * dest.red * Fb));
		green = (int)MathExt::round( (tAlpha1 * src.green * Fa) + (tAlpha2 * dest.green * Fb));
		blue = (int)MathExt::round( (tAlpha1 * src.blue * Fa) + (tAlpha2 * dest.blue * Fb));
		alpha = (int)MathExt::round( (src.alpha * Fa) + (dest.alpha * Fb));

		unsigned char redB = (unsigned char)MathExt::min(red, 255);
		unsigned char greenB = (unsigned char)MathExt::min(green, 255);
		unsigned char blueB = (unsigned char)MathExt::min(blue, 255);
		unsigned char alphaB = (unsigned char)MathExt::min(alpha, 255);

		return {redB,greenB,blueB,alphaB};

	}

	Vec4f SimpleGraphics::blend(Vec4f src, Vec4f dest)
	{
		if(compositeRule == NO_COMPOSITE)
		{
			return src;
		}

		double Fa = 0;
		double Fb = 0;

		//other stuff to
		double tAlpha1 = src.w;
		double tAlpha2 = dest.w;

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

		Vec4f retValue;
		retValue.x = (tAlpha1 * src.x * Fa) + (tAlpha2 * dest.x * Fb);
		retValue.y = (tAlpha1 * src.y * Fa) + (tAlpha2 * dest.y * Fb);
		retValue.z = (tAlpha1 * src.z * Fa) + (tAlpha2 * dest.z * Fb);
		retValue.w = (src.w * Fa) + (dest.w * Fb);

		return retValue;
	}

	Color4f SimpleGraphics::blend(Color4f src, Color4f dest)
	{
		if(compositeRule == NO_COMPOSITE)
		{
			return src;
		}

		double Fa = 0;
		double Fb = 0;

		//other stuff to
		double tAlpha1 = src.alpha;
		double tAlpha2 = dest.alpha;

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

		Color4f retValue;
		retValue.red = (tAlpha1 * src.red * Fa) + (tAlpha2 * dest.red * Fb);
		retValue.green = (tAlpha1 * src.green * Fa) + (tAlpha2 * dest.green * Fb);
		retValue.blue = (tAlpha1 * src.blue * Fa) + (tAlpha2 * dest.blue * Fb);
		retValue.alpha = (src.alpha * Fa) + (dest.alpha * Fb);

		return retValue;
	}

	#pragma region SSE_AND_AVX_BLENDS

	#if (OPTI>=1)
	__m128i SimpleGraphics::blend(__m128i src, __m128i dest)
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

		//round the stuff
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
	__m256i SimpleGraphics::blend(__m256i src, __m256i dest)
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

		//round the stuff
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

	Color SimpleGraphics::lerp(Color src, Color dest, double lerpVal)
	{
		Vec4f v1 = Vec4f(src.red, src.green, src.blue, src.alpha);
		Vec4f v2 = Vec4f(dest.red, dest.green, dest.blue, dest.alpha);
		
		Vec4f v3 = v1*(1.0-lerpVal) + v2*(lerpVal);

		return {(unsigned char)MathExt::clamp(v3.x, 0.0, 255.0),
				(unsigned char)MathExt::clamp(v3.y, 0.0, 255.0),
				(unsigned char)MathExt::clamp(v3.z, 0.0, 255.0),
				(unsigned char)MathExt::clamp(v3.w, 0.0, 255.0) };
	}

	Vec4f SimpleGraphics::lerp(Vec4f src, Vec4f dest, double lerpVal)
	{
		return src*(1.0-lerpVal) + dest*(lerpVal);
	}

	void SimpleGraphics::setClippingRect(Box2D b)
	{
		clippingRect = b;
	}

	Box2D SimpleGraphics::getClippingRect()
	{
		return clippingRect;
	}

	void SimpleGraphics::resetClippingPlane()
	{
		clippingRect.setLeftBound(0);
		clippingRect.setRightBound(0xFFFF);
		clippingRect.setTopBound(0);
		clippingRect.setBottomBound(0xFFFF);
	}

	void SimpleGraphics::setColor(Color c)
	{
		activeColor = c;
	}

	Color SimpleGraphics::getColor()
	{
		return activeColor;
	}

	void SimpleGraphics::setFont(Font* f)
	{
		activeFont = f;
	}

	Font* SimpleGraphics::getFont()
	{
		if(activeFont!=nullptr)
			return activeFont;
		else
		{
			return SimpleGraphics::getDefaultFont(defaultFontValue);
		}
	}

	void SimpleGraphics::setDefaultFont(unsigned char byte)
	{
		defaultFontValue = MathExt::clamp(byte, (unsigned char)0, (unsigned char)2);
	}

	Font* SimpleGraphics::getDefaultFont(unsigned char byte)
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

	void SimpleGraphics::setFillRule(bool v)
	{
		SimpleGraphics::fillRule = v;
	}

	bool SimpleGraphics::getFillRule()
	{
		return fillRule;
	}

	void SimpleGraphics::setCompositeRule(unsigned char b)
	{
		compositeRule = b;
	}

	unsigned char SimpleGraphics::getCompositeRule()
	{
		return compositeRule;
	}

	void SimpleGraphics::setBlendMode(unsigned char b)
	{
		blendMode = b;
	}

	unsigned char SimpleGraphics::getBlendMode()
	{
		return blendMode;
	}

	void SimpleGraphics::setAntiAliasing(bool v)
	{
		antiAliasing = v;
	}

	bool SimpleGraphics::getAntiAliasing()
	{
		return antiAliasing;
	}

	#pragma endregion

	

} //NAMESPACE glib END