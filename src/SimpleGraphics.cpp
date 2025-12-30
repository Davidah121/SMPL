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
		result.red = (unsigned char)MathExt::clamp<float>(v.x*255, 0.0, 255.0);
		result.green = (unsigned char)MathExt::clamp<float>(v.y*255, 0.0, 255.0);
		result.blue = (unsigned char)MathExt::clamp<float>(v.z*255, 0.0, 255.0);
		result.alpha = (unsigned char)MathExt::clamp<float>(v.w*255, 0.0, 255.0);
		return result;
	}
	
	Color SimpleGraphics::convertColor4fToColor(Color4f c)
	{
		Color result;
		//cheating a bit. just take the upper 8 bits
		result.red = (unsigned char)(c.red.a>>8);
		result.green = (unsigned char)(c.green.a>>8);
		result.blue = (unsigned char)(c.blue.a>>8);
		result.alpha = (unsigned char)(c.alpha.a>>8);
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

	
	void SimpleGraphics::fillBetween(Color c, int x1, int x2, int y, Image* surf)
	{
		SimpleGraphics::fillBetween(c.toUInt(), x1, x2, y, surf);
	}
	
	void SimpleGraphics::fillBetween(SIMD_U32 c, int x1, int x2, int y, Image* surf)
	{
		Color arr[8]; //Needed for sse/avx. Slower
		c.store((unsigned int*)arr);
		Color nonSSEColor = arr[0];

		Color* srcPixels = surf->getPixels();
		int len = x2-x1;
		int simdBound = SIMD_U32::getSIMDBound(len);
		unsigned int* startOfSrcPixelFillSpot = (unsigned int*)&srcPixels[x1 + y*surf->getWidth()];
		unsigned int* endOfSrcPixelFillSpot = (unsigned int*)&srcPixels[x2 + y*surf->getWidth()];
		
		for(int k=0; k<simdBound; k+=SIMD_U32::SIZE)
		{
			SIMD_U32 destColor = SIMD_U32::load(startOfSrcPixelFillSpot);
			SIMD_U32 blendedColor = blend(c.values, destColor.values);
			blendedColor.store(startOfSrcPixelFillSpot);
			startOfSrcPixelFillSpot += SIMD_U32::SIZE;
		}
		while(startOfSrcPixelFillSpot < endOfSrcPixelFillSpot)
		{
			*((Color*)startOfSrcPixelFillSpot) = blend(nonSSEColor, *((Color*)startOfSrcPixelFillSpot));
			startOfSrcPixelFillSpot++;
		}
	}

	
	void SimpleGraphics::fillBetweenAntiAliased(Color c, double x1, double x2, int y, Image* surf)
	{
		SimpleGraphics::fillBetweenAntiAliased(c.toUInt(), x1, x2, y, surf);
	}
	void SimpleGraphics::fillBetweenAntiAliased(SIMD_U32 c, double x1, double x2, int y, Image* surf)
	{
		const float MIN_ALPHA_VALUE = 1.0/255.0;

		Color arr[8]; //Needed for sse/avx. Slower
		c.store((unsigned int*)arr);
		Color nonSSEColor = arr[0];

		//bounds solving
		int reducedX1 = MathExt::floor(x1);
		float fracX1 = 1-(x1 - reducedX1);
		int reducedX2 = MathExt::ceil(x2);
		float fracX2 = 1-(reducedX2 - x2);
		

		if(fracX1 > MIN_ALPHA_VALUE)
		{
			reducedX1++;
		}

		Color* srcPixels = surf->getPixels();
		int len = reducedX2-reducedX1;
		int simdBound = SIMD_U32::getSIMDBound(len);
		unsigned int* startOfSrcPixelFillSpot = (unsigned int*)&srcPixels[reducedX1 + y*surf->getWidth()];
		unsigned int* endOfSrcPixelFillSpot = (unsigned int*)&srcPixels[reducedX2 + y*surf->getWidth()];
		
		for(int k=0; k<simdBound; k+=SIMD_U32::SIZE)
		{
			SIMD_U32 destColor = SIMD_U32::load(startOfSrcPixelFillSpot);
			SIMD_U32 blendedColor = blend(c.values, destColor.values);
			blendedColor.store(startOfSrcPixelFillSpot);
			startOfSrcPixelFillSpot += SIMD_U32::SIZE;
		}
		while(startOfSrcPixelFillSpot < endOfSrcPixelFillSpot)
		{
			*((Color*)startOfSrcPixelFillSpot) = blend(nonSSEColor, *((Color*)startOfSrcPixelFillSpot));
			startOfSrcPixelFillSpot++;
		}

		//draw at start pos and end pos with the fractional amount of color.
		if(fracX1 > MIN_ALPHA_VALUE)
		{
			//draw fractional pixel at reducedX1-1
			nonSSEColor.alpha = fracX1*255;
			drawPixel(reducedX1-1, y, nonSSEColor, surf);
		}
		if(fracX2 > MIN_ALPHA_VALUE)
		{
			//draw fractional pixel at reducedX2
			nonSSEColor.alpha = fracX2*255;
			drawPixel(reducedX2, y, nonSSEColor, surf);
		}
		
	}

	
	uint16_t quickDiv255(uint16_t a)
	{
		uint32_t b = (uint32_t)a*0x8081;
		return b>>23;
	}
	
	#ifdef __SSE4_2__
	__m128i quickDiv255(__m128i A)
	{
		const __m128i intMagicNumber = _mm_set1_epi32(0x8081);
		__m128i lowAsInt = _mm_cvtepu16_epi32(A);
		lowAsInt = _mm_mullo_epi32(lowAsInt, intMagicNumber);
		lowAsInt = _mm_srli_epi32(lowAsInt, 23);

		__m128i highAsInt = _mm_cvtepu16_epi32(_mm_srli_si128(A, 8));
		highAsInt = _mm_mullo_epi32(highAsInt, intMagicNumber);
		highAsInt = _mm_srli_epi32(highAsInt, 23);
		
		return _mm_packus_epi32(lowAsInt, highAsInt);
	}
	#endif
	
	
	#ifdef __AVX2__
	__m256i quickDiv255(__m256i A)
	{
		const __m256i intMagicNumber = _mm256_set1_epi32(0x8081);
		__m256i lowShortsAsInt = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(A, 0));
		__m256i highShortsAsInt = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(A, 1));

		lowShortsAsInt = _mm256_mullo_epi32(lowShortsAsInt, intMagicNumber);
		lowShortsAsInt = _mm256_srli_epi32(lowShortsAsInt, 23);

		highShortsAsInt = _mm256_mullo_epi32(highShortsAsInt, intMagicNumber);
		highShortsAsInt = _mm256_srli_epi32(highShortsAsInt, 23);

		__m256i packedOutput = _mm256_packus_epi32(lowShortsAsInt, highShortsAsInt);
		return _mm256_permute4x64_epi64(packedOutput, 0b11011000);
	}
	#endif

	Color SimpleGraphics::blend(Color src, Color dest)
	{
		if(compositeRule == NO_COMPOSITE)
		{
			return src;
		}

		Color result;
		uint16_t srcA = src.alpha;
		uint16_t destA = dest.alpha;
		uint16_t Fa = 0;
		uint16_t Fb = 0;

		switch(compositeRule)
		{
			case SimpleGraphics::COMPOSITE_CLEAR:
				break;
			case SimpleGraphics::COMPOSITE_COPY:
				Fa = 255;
				break;
			case SimpleGraphics::COMPOSITE_DEST:
				Fb = 255;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OVER:
				Fa = 255;
				Fb = 255-srcA;
				break;
			case SimpleGraphics::COMPOSITE_DEST_OVER:
				Fa = 255-destA;
				Fb = 255;
				break;
			case SimpleGraphics::COMPOSITE_SRC_IN:
				Fa = destA;
				break;
			case SimpleGraphics::COMPOSITE_DEST_IN:
				Fb = srcA;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OUT:
				Fa = 255-destA;
				break;
			case SimpleGraphics::COMPOSITE_DEST_OUT:
				Fb = 255-srcA;
				break;
			case SimpleGraphics::COMPOSITE_SRC_ATOP:
				Fa = destA;
				Fb = 255-srcA;
				break;
			case SimpleGraphics::COMPOSITE_DEST_ATOP:
				Fa = 255-destA;
				Fb = srcA;
				break;
			case SimpleGraphics::COMPOSITE_XOR:
				Fa = 255-destA;
				Fb = 255-srcA;
				break;
			case SimpleGraphics::COMPOSITE_LIGHTER:
				Fa = 255;
				Fb = 255;
				break;
			default:
				break;
		}

		Color srcAlphaMult = {
			(unsigned char)quickDiv255((uint16_t)(src.red*src.alpha)),
			(unsigned char)quickDiv255((uint16_t)(src.green*src.alpha)),
			(unsigned char)quickDiv255((uint16_t)(src.blue*src.alpha)),
			src.alpha
		};
		
		Color destAlphaMult = {
			(unsigned char)quickDiv255((uint16_t)(dest.red*dest.alpha)),
			(unsigned char)quickDiv255((uint16_t)(dest.green*dest.alpha)),
			(unsigned char)quickDiv255((uint16_t)(dest.blue*dest.alpha)),
			dest.alpha
		};

		result.red = (uint8_t)quickDiv255(MathExt::saturatedAdd((uint16_t)(Fa*srcAlphaMult.red), (uint16_t)(Fb*destAlphaMult.red)));
		result.green = (uint8_t)quickDiv255(MathExt::saturatedAdd((uint16_t)(Fa*srcAlphaMult.green), (uint16_t)(Fb*destAlphaMult.green)));
		result.blue = (uint8_t)quickDiv255(MathExt::saturatedAdd((uint16_t)(Fa*srcAlphaMult.blue), (uint16_t)(Fb*destAlphaMult.blue)));
		result.alpha = (uint8_t)quickDiv255(MathExt::saturatedAdd((uint16_t)(Fa*srcA), (uint16_t)(Fb*destA)));
		return result;
	}

	Vec4f SimpleGraphics::blend(Vec4f src, Vec4f dest)
	{
		if(compositeRule == NO_COMPOSITE)
		{
			return src;
		}

		float Fa = 0;
		float Fb = 0;
		switch(compositeRule)
		{
			case SimpleGraphics::COMPOSITE_CLEAR:
				break;
			case SimpleGraphics::COMPOSITE_COPY:
				Fa = 1;
				break;
			case SimpleGraphics::COMPOSITE_DEST:
				Fb = 1;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OVER:
				Fa = 1;
				Fb = 1-src.w;
				break;
			case SimpleGraphics::COMPOSITE_DEST_OVER:
				Fa = 1-dest.w;
				Fb = 1;
				break;
			case SimpleGraphics::COMPOSITE_SRC_IN:
				Fa = dest.w;
				break;
			case SimpleGraphics::COMPOSITE_DEST_IN:
				Fb = src.w;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OUT:
				Fa = 1-dest.w;
				break;
			case SimpleGraphics::COMPOSITE_DEST_OUT:
				Fb = 1-src.w;
				break;
			case SimpleGraphics::COMPOSITE_SRC_ATOP:
				Fa = dest.w;
				Fb = 1-src.w;
				break;
			case SimpleGraphics::COMPOSITE_DEST_ATOP:
				Fa = 1-dest.w;
				Fb = src.w;
				break;
			case SimpleGraphics::COMPOSITE_XOR:
				Fa = 1-dest.w;
				Fb = 1-src.w;
				break;
			case SimpleGraphics::COMPOSITE_LIGHTER:
				Fa = 1;
				Fb = 1;
				break;
			default:
				break;
		}

		Vec4f retValue;
		float tempSrcAlpha = src.w;
		float tempDestAlpha = dest.w;
		
		src.w = 1;
		dest.w = 1;
		retValue = tempSrcAlpha*src*Fa + tempDestAlpha*dest*Fb;
		return retValue;
	}

	Color4f SimpleGraphics::blend(Color4f src, Color4f dest)
	{
		if(compositeRule == NO_COMPOSITE)
		{
			return src;
		}

		float Fa = 0;
		float Fb = 0;
		switch(compositeRule)
		{
			case SimpleGraphics::COMPOSITE_CLEAR:
				break;
			case SimpleGraphics::COMPOSITE_COPY:
				Fa = 1;
				break;
			case SimpleGraphics::COMPOSITE_DEST:
				Fb = 1;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OVER:
				Fa = 1;
				Fb = 1.0f-src.alpha;
				break;
			case SimpleGraphics::COMPOSITE_DEST_OVER:
				Fa = 1.0f-dest.alpha;
				Fb = 1;
				break;
			case SimpleGraphics::COMPOSITE_SRC_IN:
				Fa = dest.alpha;
				break;
			case SimpleGraphics::COMPOSITE_DEST_IN:
				Fb = src.alpha;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OUT:
				Fa = 1.0f-dest.alpha;
				break;
			case SimpleGraphics::COMPOSITE_DEST_OUT:
				Fb = 1.0f-src.alpha;
				break;
			case SimpleGraphics::COMPOSITE_SRC_ATOP:
				Fa = dest.alpha;
				Fb = 1.0f-src.alpha;
				break;
			case SimpleGraphics::COMPOSITE_DEST_ATOP:
				Fa = 1.0f-dest.alpha;
				Fb = src.alpha;
				break;
			case SimpleGraphics::COMPOSITE_XOR:
				Fa = 1.0f-dest.alpha;
				Fb = 1.0f-src.alpha;
				break;
			case SimpleGraphics::COMPOSITE_LIGHTER:
				Fa = 1;
				Fb = 1;
				break;
			default:
				break;
		}

		Color4f retValue;
		float tempSrcAlpha = src.alpha;
		float tempDestAlpha = dest.alpha;
		
		src.alpha = 1.0f;
		dest.alpha = 1.0f;
		retValue = tempSrcAlpha*Fa*src + tempDestAlpha*Fb*dest;
		return retValue;
	}

	
	uint32_t SimpleGraphics::blend(uint32_t src, uint32_t multiplier)
	{
		return blend(*((Color*)&src), *((Color*)&multiplier)).toUInt();
	}

	#pragma region SSE_AND_AVX_BLENDS

	#ifdef __SSE4_2__
	void compositeRule16Bit(unsigned char composite, __m128i srcAlpha, __m128i destAlpha, __m128i& Fa, __m128i& Fb)
	{
		const __m128i v255 = _mm_set1_epi16(255);
		Fa = _mm_set1_epi16(0);
		Fb = _mm_set1_epi16(0);

		switch(composite)
		{
			case SimpleGraphics::COMPOSITE_CLEAR:
				break;
			case SimpleGraphics::COMPOSITE_COPY:
				Fa = v255;
				break;
			case SimpleGraphics::COMPOSITE_DEST:
				Fb = v255;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OVER:
				Fa = v255;
				Fb = _mm_sub_epi16(v255, srcAlpha);
				break;
			case SimpleGraphics::COMPOSITE_DEST_OVER:
				Fa = _mm_sub_epi16(v255, destAlpha);
				Fb = v255;
				break;
			case SimpleGraphics::COMPOSITE_SRC_IN:
				Fa = destAlpha;
				break;
			case SimpleGraphics::COMPOSITE_DEST_IN:
				Fb = srcAlpha;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OUT:
				Fa = _mm_sub_epi16(v255, destAlpha);
				break;
			case SimpleGraphics::COMPOSITE_DEST_OUT:
				Fb = _mm_sub_epi16(v255, srcAlpha);
				break;
			case SimpleGraphics::COMPOSITE_SRC_ATOP:
				Fa = destAlpha;
				Fb = _mm_sub_epi16(v255, srcAlpha);
				break;
			case SimpleGraphics::COMPOSITE_DEST_ATOP:
				Fa = _mm_sub_epi16(v255, destAlpha);
				Fb = srcAlpha;
				break;
			case SimpleGraphics::COMPOSITE_XOR:
				Fa = _mm_sub_epi16(v255, destAlpha);
				Fb = _mm_sub_epi16(v255, srcAlpha);
				break;
			case SimpleGraphics::COMPOSITE_LIGHTER:
				Fa = v255;
				Fb = v255;
				break;
			default:
				break;
		}
	}

	__m128i SimpleGraphics::blend(__m128i src, __m128i dest)
	{
		const __m128i aa = _mm_set_epi8(15,15,15,15, 11,11,11,11, 7,7,7,7, 3,3,3,3);
		__m128i Fa, Fb;

		//Fa*src.a*src + Fb*dest.a*dest

		//Get Only alpha values. Copy them over
		__m128i srcOnlyAlpha = _mm_shuffle_epi8(src, aa);
		__m128i destOnlyAlpha = _mm_shuffle_epi8(dest, aa);

		//convert colors to premultiply alpha
		__m128i lowSrc16 = _mm_cvtepu8_epi16(src); //only 2 colors now.
		__m128i lowDest16 = _mm_cvtepu8_epi16(dest); //only 2 colors now.

		__m128i lowSrcAlpha16 = _mm_cvtepu8_epi16(srcOnlyAlpha);
		__m128i lowDestAlpha16 = _mm_cvtepu8_epi16(destOnlyAlpha);

		__m128i lowSrcPreMult = quickDiv255( _mm_mullo_epi16(lowSrc16, lowSrcAlpha16)); //premultiplied alpha. Approximation by the way.
		__m128i lowDestPreMult = quickDiv255( _mm_mullo_epi16(lowDest16, lowDestAlpha16)); //premultiplied alpha. Approximation by the way.

		//need the alpha values to not be modified. Get them from the old values
		lowSrcPreMult = _mm_blend_epi16(lowSrcPreMult, lowSrc16, 0b10001000);
		lowDestPreMult = _mm_blend_epi16(lowDestPreMult, lowDest16, 0b10001000);
		
		//now do the Fa*src + Fb*dest
		compositeRule16Bit(compositeRule, lowSrcAlpha16, lowDestAlpha16, Fa, Fb);
		__m128i outputLow = quickDiv255(_mm_adds_epi16(_mm_mullo_epi16(lowSrcPreMult, Fa), _mm_mullo_epi16(lowDestPreMult, Fb)));

		//now do high
		__m128i highSrc16 = _mm_cvtepu8_epi16(_mm_srli_si128(src, 8)); //only 2 colors now.
		__m128i highDest16 = _mm_cvtepu8_epi16(_mm_srli_si128(dest, 8)); //only 2 colors now.

		__m128i highSrcAlpha16 = _mm_cvtepu8_epi16(_mm_srli_si128(srcOnlyAlpha, 8));
		__m128i highDestAlpha16 = _mm_cvtepu8_epi16(_mm_srli_si128(destOnlyAlpha, 8));

		__m128i highSrcPreMult = quickDiv255( _mm_mullo_epi16(highSrc16, highSrcAlpha16)); //premultiplied alpha. Approximation by the way.
		__m128i highDestPreMult = quickDiv255( _mm_mullo_epi16(highDest16, highDestAlpha16)); //premultiplied alpha. Approximation by the way.

		//need the alpha values to not be modified. Get them from the old values
		highSrcPreMult = _mm_blend_epi16(highSrcPreMult, highSrc16, 0b10001000);
		highDestPreMult = _mm_blend_epi16(highDestPreMult, highDest16, 0b10001000);
		
		//now do the Fa*src + Fb*dest
		compositeRule16Bit(compositeRule, highSrcAlpha16, highDestAlpha16, Fa, Fb);
		__m128i outputHigh = quickDiv255(_mm_adds_epi16(_mm_mullo_epi16(highSrcPreMult, Fa), _mm_mullo_epi16(highDestPreMult, Fb)));

		return _mm_packus_epi16(outputLow, outputHigh);
	}
	#endif

	#ifdef __AVX2__
	void compositeRule16Bit(unsigned char composite, __m256i srcAlpha, __m256i destAlpha, __m256i& Fa, __m256i& Fb)
	{
		const __m256i v255 = _mm256_set1_epi16(255);
		Fa = _mm256_set1_epi16(0);
		Fb = _mm256_set1_epi16(0);

		switch(composite)
		{
			case SimpleGraphics::COMPOSITE_CLEAR:
				break;
			case SimpleGraphics::COMPOSITE_COPY:
				Fa = v255;
				break;
			case SimpleGraphics::COMPOSITE_DEST:
				Fb = v255;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OVER:
				Fa = v255;
				Fb = _mm256_sub_epi16(v255, srcAlpha);
				break;
			case SimpleGraphics::COMPOSITE_DEST_OVER:
				Fa = _mm256_sub_epi16(v255, destAlpha);
				Fb = v255;
				break;
			case SimpleGraphics::COMPOSITE_SRC_IN:
				Fa = destAlpha;
				break;
			case SimpleGraphics::COMPOSITE_DEST_IN:
				Fb = srcAlpha;
				break;
			case SimpleGraphics::COMPOSITE_SRC_OUT:
				Fa = _mm256_sub_epi16(v255, destAlpha);
				break;
			case SimpleGraphics::COMPOSITE_DEST_OUT:
				Fb = _mm256_sub_epi16(v255, srcAlpha);
				break;
			case SimpleGraphics::COMPOSITE_SRC_ATOP:
				Fa = destAlpha;
				Fb = _mm256_sub_epi16(v255, srcAlpha);
				break;
			case SimpleGraphics::COMPOSITE_DEST_ATOP:
				Fa = _mm256_sub_epi16(v255, destAlpha);
				Fb = srcAlpha;
				break;
			case SimpleGraphics::COMPOSITE_XOR:
				Fa = _mm256_sub_epi16(v255, destAlpha);
				Fb = _mm256_sub_epi16(v255, srcAlpha);
				break;
			case SimpleGraphics::COMPOSITE_LIGHTER:
				Fa = v255;
				Fb = v255;
				break;
			default:
				break;
		}
	}

	__m256i SimpleGraphics::blend(__m256i src, __m256i dest)
	{
		const __m256i aa = _mm256_set_epi8(31,31,31,31, 27,27,27,27, 23,23,23,23, 19,19,19,19, 15,15,15,15, 11,11,11,11, 7,7,7,7, 3,3,3,3);
		__m256i Fa, Fb;

		//Fa*src.a*src + Fb*dest.a*dest

		//Get Only alpha values. Copy them over
		__m256i srcOnlyAlpha = _mm256_shuffle_epi8(src, aa);
		__m256i destOnlyAlpha = _mm256_shuffle_epi8(dest, aa);

		__m128i m128Src = _mm256_extracti128_si256(src, 0);
		__m128i m128Dest = _mm256_extracti128_si256(dest, 0);
		__m128i m128SrcAlpha = _mm256_extracti128_si256(srcOnlyAlpha, 0);
		__m128i m128DestAlpha = _mm256_extracti128_si256(destOnlyAlpha, 0);
		
		//convert colors to premultiply alpha
		
		__m256i lowSrc16 = _mm256_cvtepu8_epi16(m128Src); //only 4/8 colors now.
		__m256i lowDest16 = _mm256_cvtepu8_epi16(m128Dest); //only 4/8 colors now.

		__m256i lowSrcAlpha16 = _mm256_cvtepu8_epi16(m128SrcAlpha);
		__m256i lowDestAlpha16 = _mm256_cvtepu8_epi16(m128DestAlpha);

		__m256i lowSrcPreMult = quickDiv255( _mm256_mullo_epi16(lowSrc16, lowSrcAlpha16)); //premultiplied alpha. Approximation by the way.
		__m256i lowDestPreMult = quickDiv255( _mm256_mullo_epi16(lowDest16, lowDestAlpha16)); //premultiplied alpha. Approximation by the way.

		//need the alpha values to not be modified. Get them from the old values
		lowSrcPreMult = _mm256_blend_epi16(lowSrcPreMult, lowSrc16, 0b10001000);
		lowDestPreMult = _mm256_blend_epi16(lowDestPreMult, lowDest16, 0b10001000);
		
		//now do the Fa*src + Fb*dest
		compositeRule16Bit(compositeRule, lowSrcAlpha16, lowDestAlpha16, Fa, Fb);
		__m256i outputLow = quickDiv255(_mm256_adds_epi16(_mm256_mullo_epi16(lowSrcPreMult, Fa), _mm256_mullo_epi16(lowDestPreMult, Fb)));

		//now do high
		
		m128Src = _mm256_extracti128_si256(src, 1);
		m128Dest = _mm256_extracti128_si256(dest, 1);
		m128SrcAlpha = _mm256_extracti128_si256(srcOnlyAlpha, 1);
		m128DestAlpha = _mm256_extracti128_si256(destOnlyAlpha, 1);

		__m256i highSrc16 = _mm256_cvtepu8_epi16(m128Src); //only 4/8 colors now.
		__m256i highDest16 = _mm256_cvtepu8_epi16(m128Dest); //only 4/8 colors now.

		__m256i highSrcAlpha16 = _mm256_cvtepu8_epi16(m128SrcAlpha);
		__m256i highDestAlpha16 = _mm256_cvtepu8_epi16(m128DestAlpha);

		__m256i highSrcPreMult = quickDiv255( _mm256_mullo_epi16(highSrc16, highSrcAlpha16)); //premultiplied alpha. Approximation by the way.
		__m256i highDestPreMult = quickDiv255( _mm256_mullo_epi16(highDest16, highDestAlpha16)); //premultiplied alpha. Approximation by the way.

		//need the alpha values to not be modified. Get them from the old values
		highSrcPreMult = _mm256_blend_epi16(highSrcPreMult, highSrc16, 0b10001000);
		highDestPreMult = _mm256_blend_epi16(highDestPreMult, highDest16, 0b10001000);
		
		//now do the Fa*src + Fb*dest
		compositeRule16Bit(compositeRule, highSrcAlpha16, highDestAlpha16, Fa, Fb);
		__m256i outputHigh = quickDiv255(_mm256_adds_epi16(_mm256_mullo_epi16(highSrcPreMult, Fa), _mm256_mullo_epi16(highDestPreMult, Fb)));

		__m256i packedOutput = _mm256_packus_epi16(outputLow, outputHigh);
		// return packedOutput;
		return _mm256_permute4x64_epi64(packedOutput, 0b11011000); //0 - 2 - 1 - 3
	}

	#endif

	#pragma endregion

	Color SimpleGraphics::lerp(Color src, Color dest, double lerpVal)
	{
		uint16_t blendV = MathExt::round(lerpVal*255);
		uint16_t r = (255-blendV)*src.red + blendV*dest.red;
		uint16_t g = (255-blendV)*src.green + blendV*dest.green;
		uint16_t b = (255-blendV)*src.blue + blendV*dest.blue;
		uint16_t a = (255-blendV)*src.alpha + blendV*dest.alpha;
		
		return Color{(unsigned char)quickDiv255(r), 
					 (unsigned char)quickDiv255(g), 
					 (unsigned char)quickDiv255(b), 
					 (unsigned char)quickDiv255(a)};
	}
	
	Color4f SimpleGraphics::lerp(Color4f src, Color4f dest, double lerpVal)
	{
		return src*(1.0-lerpVal) + dest*(lerpVal);
	}

	Vec4f SimpleGraphics::lerp(Vec4f src, Vec4f dest, double lerpVal)
	{
		return src*(1.0-lerpVal) + dest*(lerpVal);
	}
	
	uint32_t SimpleGraphics::lerp(uint32_t src, uint32_t dest, double lerpVal)
	{
		return lerp(*((Color*)&src), *((Color*)&dest), lerpVal).toUInt();
	}


	#ifdef __SSE4_2__
	__m128i SimpleGraphics::lerp(__m128i src, __m128i dest, __m128 lerpVal)
	{
		SIMD_SSE<float> blendV = lerpVal;
		SIMD_SSE<float> A1 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(src));
		SIMD_SSE<float> B1 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(dest));
		
		SIMD_SSE<float> A2 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(src, 4)));
		SIMD_SSE<float> B2 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(dest, 4)));
		
		SIMD_SSE<float> A3 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(src, 8)));
		SIMD_SSE<float> B3 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(dest, 8)));
		
		SIMD_SSE<float> A4 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(src, 12)));
		SIMD_SSE<float> B4 = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_srli_si128(dest, 12)));

		SIMD_SSE<float> res1 = A1*(SIMD_SSE<float>(1.0f)-blendV) + B1*blendV;
		SIMD_SSE<float> res2 = A2*(SIMD_SSE<float>(1.0f)-blendV) + B2*blendV;
		SIMD_SSE<float> res3 = A3*(SIMD_SSE<float>(1.0f)-blendV) + B3*blendV;
		SIMD_SSE<float> res4 = A4*(SIMD_SSE<float>(1.0f)-blendV) + B4*blendV;
		
		__m128i output1 = SEML::floatToInt32(res1.values);
		__m128i output2 = SEML::floatToInt32(res2.values);
		__m128i output3 = SEML::floatToInt32(res3.values);
		__m128i output4 = SEML::floatToInt32(res4.values);

		__m128i pack16Low = _mm_packus_epi32(output1, output2);
		__m128i pack16High = _mm_packus_epi32(output3, output4);

		return _mm_packs_epi16(pack16Low, pack16High);
	}
	#endif

	#ifdef __AVX2__
	__m256i SimpleGraphics::lerp(__m256i src, __m256i dest, __m256 lerpVal)
	{
		SIMD_AVX<float> blendV = lerpVal;
		__m128i A1_Extracted128 = _mm256_extracti128_si256(src, 0);
		__m128i A2_Extracted128 = _mm256_extracti128_si256(src, 1);
		__m128i B1_Extracted128 = _mm256_extracti128_si256(dest, 0);
		__m128i B2_Extracted128 = _mm256_extracti128_si256(dest, 1);
		
		SIMD_AVX<float> A1 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(A1_Extracted128));
		SIMD_AVX<float> B1 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(B1_Extracted128));
		
		SIMD_AVX<float> A2 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(_mm_srli_si128(A1_Extracted128, 8)));
		SIMD_AVX<float> B2 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(_mm_srli_si128(B1_Extracted128, 8)));
		
		SIMD_AVX<float> A3 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(A2_Extracted128));
		SIMD_AVX<float> B3 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(B2_Extracted128));

		SIMD_AVX<float> A4 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(_mm_srli_si128(A2_Extracted128, 8)));
		SIMD_AVX<float> B4 = _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(_mm_srli_si128(B2_Extracted128, 8)));

		SIMD_AVX<float> res1 = A1*(SIMD_AVX<float>(1.0f)-blendV) + B1*blendV;
		SIMD_AVX<float> res2 = A2*(SIMD_AVX<float>(1.0f)-blendV) + B2*blendV;
		SIMD_AVX<float> res3 = A3*(SIMD_AVX<float>(1.0f)-blendV) + B3*blendV;
		SIMD_AVX<float> res4 = A4*(SIMD_AVX<float>(1.0f)-blendV) + B4*blendV;
		
		__m256i output1 = SEML::floatToInt32(res1.values);
		__m256i output2 = SEML::floatToInt32(res2.values);
		__m256i output3 = SEML::floatToInt32(res3.values);
		__m256i output4 = SEML::floatToInt32(res4.values);

		__m256i pack16Low = _mm256_packs_epi32(output1, output2);
		__m256i pack16High = _mm256_packs_epi32(output3, output4);

		return _mm256_packs_epi16(pack16Low, pack16High);
	}
	#endif
	
	Color SimpleGraphics::multColor(Color src, Color multiplier)
	{
		return Color{
			(unsigned char)(((uint16_t)src.red * multiplier.red)>>8),
			(unsigned char)(((uint16_t)src.green * multiplier.green)>>8),
			(unsigned char)(((uint16_t)src.blue * multiplier.blue)>>8),
			(unsigned char)(((uint16_t)src.alpha * multiplier.alpha)>>8)
		};
	}
	uint32_t SimpleGraphics::multColor(uint32_t src, uint32_t multiplier)
	{
		return multColor(*((Color*)&src), *((Color*)&multiplier)).toUInt();
	}

	Color4f SimpleGraphics::multColor(Color4f src, Color4f multiplier)
	{
		return Color4f{
			src.red * multiplier.red,
			src.green * multiplier.green,
			src.blue * multiplier.blue,
			src.alpha * multiplier.alpha
		};
	}
	Vec4f SimpleGraphics::multColor(Vec4f src, Vec4f multiplier)
	{
		return Vec4f(
			src.x * multiplier.x,
			src.y * multiplier.y,
			src.z * multiplier.z,
			src.w * multiplier.w
		);
	}
	
	#ifdef __SSE4_2__
	__m128i SimpleGraphics::multColor(__m128i src, __m128i multiplier)
	{
		__m128i lowSrc = _mm_cvtepu8_epi16(src);
		__m128i lowMult = _mm_cvtepu8_epi16(multiplier);
		__m128i resultLow = _mm_srli_epi16(_mm_mullo_epi16(lowSrc, lowMult), 8);
		
		__m128i highSrc = _mm_cvtepu8_epi16(_mm_srli_si128(src, 8));
		__m128i highMult = _mm_cvtepu8_epi16(_mm_srli_si128(multiplier, 8));
		__m128i resultHigh = _mm_srli_epi16(_mm_mullo_epi16(highSrc, highMult), 8);
		
		return _mm_packus_epi16(resultLow, resultHigh);
	}
	#endif

	#ifdef __AVX2__
	__m256i SimpleGraphics::multColor(__m256i src, __m256i multiplier)
	{
		__m256i lowSrc = _mm256_cvtepu8_epi16(_mm256_extractf128_si256(src, 0));
		__m256i lowMult = _mm256_cvtepu8_epi16(_mm256_extractf128_si256(multiplier, 0));
		__m256i resultLow = _mm256_srli_epi16(_mm256_mullo_epi16(lowSrc, lowMult), 8);

		__m256i highSrc = _mm256_cvtepu8_epi16(_mm256_extractf128_si256(src, 1));
		__m256i highMult = _mm256_cvtepu8_epi16(_mm256_extractf128_si256(multiplier, 1));
		__m256i resultHigh = _mm256_srli_epi16(_mm256_mullo_epi16(highSrc, highMult), 8);
		
		__m256i packedOutput = _mm256_packus_epi16(resultLow, resultHigh);
		return _mm256_permute4x64_epi64(packedOutput, 0b11011000);
	}
	#endif

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