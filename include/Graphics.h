#pragma once
#include "Image.h"
#include "BitmapFont.h"

class Graphics
{
public:
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
	
	static void setImage(Image* img);
	static Image* getImage();

	//Drawing functions
	static void clearImage(Image* surf = nullptr);
	static void drawPixel(int x, int y, Color c, Image* surf = nullptr);
	static void drawPixel(double x, double y, Color c, Image* surf = nullptr);

	static Color blend(Color c1, Color c2);
	static Color lerp(Color c1, Color c2, float lerpVal);

	static void drawRect(int x, int y, int x2, int y2, bool outline, Image* surf = nullptr);
	static void drawLine(int x, int y, int x2, int y2, Image* surf = nullptr);
	static void drawCircle(int x, int y, int radius, bool outline, Image* surf = nullptr);

	static void drawImage(Image* img, int x, int y, Image* surf = nullptr);
	
	static void drawSprite(Image* img, int x, int y, Image* surf = nullptr);
	static void drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf = nullptr);

	static void drawText(std::string str, int x, int y, Image* surf = nullptr);

	static void drawPolygon(Vec2f* points, int size, Image* surf = nullptr);

	static void setColor(Color c);
	static Color getColor();

	static void setFont(BitmapFont* f);
	static BitmapFont* getFont();
	
	static void setCompositeRule(unsigned char b);
	static void setBlendMode(unsigned char b);
	static void setFillRule(bool b);
	static void setAntiAliasing(bool b);
	
	//Image Manipulation
	static const unsigned char THRESHOLD = 0;
	static const unsigned char ORDERED_DITHER_BAYER = 1;
	static const unsigned char FLOYD_DITHER = 2;

	static const unsigned char NEAREST_NEIGHBOR_FILTER = 0;
	static const unsigned char BILINEAR_FILTER = 1;
	static const unsigned char BICUBIC_FILTER = 2;

	static void replaceColor(Image* img, Color oldColor, Color newColor, bool ignoreAlpha = false);
	static void ditherImage(Image* img, unsigned char method = 0);
	static Image* scaleImage(Image* img, double xScale, double yScale, unsigned char filterMethod = 0);
private:
	static void floydSteinburgDithering(Image* img);
	static void orderedBayerDithering(Image* img);
	static Matrix generateBayerMatrix(Matrix m, int rowSize);
	
	static Image* activeImage;
	static BitmapFont* activeFont;
	static Color activeColor;
	static unsigned char compositeRule;
	static unsigned char blendMode;
	static bool fillRule;
	static bool antiAliasing;

	static int ditherMatrixSize;
};

