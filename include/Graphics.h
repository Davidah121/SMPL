#pragma once
#include "Image.h"
#include "Font.h"
#include "Model.h"
#include "Opti.h"
#include "Shape.h"
class Graphics
{
public:
	static const unsigned char NO_COMPOSITE = 255;
	static const unsigned char COMPOSITE_CLEAR = 0;
	static const unsigned char COMPOSITE_COPY = 1;
	static const unsigned char COMPOSITE_DEST = 2;
	static const unsigned char COMPOSITE_SRC_OVER = 3;
	static const unsigned char COMPOSITE_DEST_OVER = 4;
	static const unsigned char COMPOSITE_SRC_IN = 5;
	static const unsigned char COMPOSITE_DEST_IN = 6;
	static const unsigned char COMPOSITE_SRC_OUT = 7;
	static const unsigned char COMPOSITE_DEST_OUT = 8;
	static const unsigned char COMPOSITE_SRC_ATOP = 9;
	static const unsigned char COMPOSITE_DEST_ATOP = 10;
	static const unsigned char COMPOSITE_XOR = 11;
	static const unsigned char COMPOSITE_LIGHTER = 12;

	static const bool FILL_EVEN_ODD = 0;
	static const bool FILL_NON_ZERO = 1;
	
	//todo:
	static const unsigned char BLEND_NORMAL = 0;
	static const unsigned char BLEND_MULT = 1;
	static const unsigned char BLEND_SCREEN = 2;

	//Parameters and initialization:
	static const unsigned char NORMAL_FONT = 0;
	static const unsigned char MEDIUM_FONT = 1;
	static const unsigned char LARGE_FONT = 2;

	static void init();
	static void dispose();

	//Drawing functions
	static void clearImage(Image* surf = nullptr);
	static void drawPixel(int x, int y, Color c, Image* surf = nullptr);
	static void drawPixel(double x, double y, Color c, Image* surf = nullptr);

	static Color blend(Color src, Color dest);

	#if (OPTI>=1)
		static __m128i blend(__m128i src, __m128i dest);
	#endif

	#if (OPTI>=2)
		static __m256i blend(__m256i src, __m256i dest);
	#endif

	static Color lerp(Color src, Color dest, double lerpVal);

	static void drawRect(int x, int y, int x2, int y2, bool outline, Image* surf = nullptr);
	static void drawLine(int x, int y, int x2, int y2, Image* surf = nullptr);
	static void drawCircle(int x, int y, int radius, bool outline, Image* surf = nullptr);

	static void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool outline, Image* surf = nullptr);
	static void drawTexturedTriangle(Vec4f p1, Vec4f p2, Vec4f p3, Image* texture = nullptr, Image* surf = nullptr);
	
	static void drawImage(Image* img, int x, int y, Image* surf = nullptr);
	
	static void drawSprite(Image* img, int x, int y, Image* surf = nullptr);
	static void drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf = nullptr);

	static void drawText(std::string str, int x, int y, Image* surf = nullptr);
	static void drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, Image* surf = nullptr);

	static void drawPolygon(Vec2f* points, int size, Image* surf = nullptr);
	
	static void drawModel(Model* model, Image* texture = nullptr, Image* surf = nullptr);
	
	//Getters and Setters
	static void setClippingRect(Box2D b);
	static Box2D getClippingRect();
	static void resetClippingPlane();

	static void setImage(Image* img);
	static Image* getImage();

	static void setColor(Color c);
	static Color getColor();

	static void setFont(Font* f);
	static Font* getFont();
	
	static void setDefaultFont(unsigned char type);
	static Font* getDefaultFont(unsigned char type);

	static void setCompositeRule(unsigned char b);
	static unsigned char getCompositeRule();

	static void setBlendMode(unsigned char b);
	static unsigned char getBlendMode();

	static void setFillRule(bool b);
	static bool getFillRule();

	static void setAntiAliasing(bool b);
	static bool getAntiAliasing();
	
	//Image Manipulation
	static const unsigned char THRESHOLD = 0;
	static const unsigned char ORDERED_DITHER_BAYER = 1;
	static const unsigned char FLOYD_DITHER = 2;

	static const unsigned char NEAREST_NEIGHBOR_FILTER = 0;
	static const unsigned char BILINEAR_FILTER = 1;
	static const unsigned char BICUBIC_FILTER = 2;

	static const unsigned char RED_CHANNEL = 0;
	static const unsigned char GREEN_CHANNEL = 1;
	static const unsigned char BLUE_CHANNEL = 2;
	static const unsigned char ALPHA_CHANNEL = 3;
	
	static Image* crop(Image* img, int x1, int y1, int x2, int y2);
	static Image* crop(Image* img, Shape s);

	static void replaceColor(Image* img, Color oldColor, Color newColor, bool ignoreAlpha = false);
	static void filterOutColor(Image* img, Color c1);
	static void filterOutColorRange(Image* img, Color c1, Color c2);
	
	static void convertToColorSpace(Image* img, unsigned char colorSpace);

	static void boxBlur(Image* img, int boxSize);
	static void gaussianBlur(Image* img, double stdDeviation);

	static void uncannyEdgeFilter(Image* img);
	static void sobelEdgeFilter(Image* img);
	
	static std::vector<std::vector<Vec2f>> calculateGradient(Image* img, unsigned char type);
	
	static void ditherImage(Image* img, unsigned char method = 0);
	static Image* scaleImage(Image* img, double xScale, double yScale, unsigned char filterMethod = 0);

private:
	static void floydSteinburgDithering(Image* img);
	static void orderedBayerDithering(Image* img);
	static Matrix generateBayerMatrix(Matrix m, int rowSize);

	static Image* scaleNearestNeighbor(Image* img, double xScale, double yScale);
	static Image* scaleBilinear(Image* img, double xScale, double yScale);
	static Image* scaleBicubic(Image* img, double xScale, double yScale);
	
	static unsigned char defaultFontValue;

	static Font* defaultFont;
	static Font* defaultFontMedium;
	static Font* defaultFontLarge;
	
	static Box2D clippingRect;
	
	static Image* activeImage;
	static Font* activeFont;
	static Color activeColor;
	static unsigned char compositeRule;
	static unsigned char blendMode;
	static bool fillRule;
	static bool antiAliasing;

	static int ditherMatrixSize;
};

