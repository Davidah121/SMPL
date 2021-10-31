#pragma once
#include "Image.h"
#include "Font.h"
#include "Model.h"
#include "Opti.h"
#include "Shape.h"
#include "BezierCurve.h"

namespace glib
{

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

		/**
		 * @brief Initializes the Graphics class for drawing.
		 * 		It loads in a default font for drawing text.
		 * 		Dispose should be called if this function has been called.
		 */
		static void init();

		/**
		 * @brief
		 * 		Disposes of the memory allocated by the Graphics class.
		 */
		static void dispose();

		//Drawing functions

		/**
		 * @brief Clears the specified Image* to the draw color.
		 * @param surf
		 * 		If set to nullptr, no error is thrown.
		 */
		static void clearImage(Image* surf);

		/**
		 * @brief Draws a pixel to the specified image using Porter Duff rules.
		 * @param x
		 * 		The x location of the pixel.
		 * @param y
		 * 		The y location of the pixel.
		 * @param c
		 * 		The color of the pixel.
		 * @param surf
		 * 		If set to nullptr, no error is thrown.
		 */
		static void drawPixel(int x, int y, Color c, Image* surf);

		/**
		 * @brief Draws a pixel to the specified image using Porter Duff rules.
		 * 		This function can draw up to 4 different pixels due to the specified point
		 * 		not directly lining up with the pixel grid.
		 * @param x
		 * 		The x location of the pixel.
		 * @param y
		 * 		The y location of the pixel.
		 * @param c
		 * 		The color of the pixel.
		 * @param surf
		 * 		If set to nullptr, no error is thrown.
		 */
		static void drawPixel(double x, double y, Color c, Image* surf);

		/**
		 * @brief Blends 2 colors using Porter Duff rules.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param src
		 * 		The source color.
		 * @param dest
		 * 		The destination color.
		 * @return Color
		 */
		static Color blend(Color src, Color dest);

		#if (OPTI>=1)
			static __m128i blend(__m128i src, __m128i dest);
		#endif

		#if (OPTI>=2)
			static __m256i blend(__m256i src, __m256i dest);
		#endif

		/**
		 * @brief Linearly blends 2 colors together.
		 * 		At value 0, src is the output color.
		 * 		At value 1, dest is the output color.
		 * @param src
		 * 		The start color.
		 * @param dest
		 * 		The end color.
		 * @param lerpVal
		 * 		The blending value. Should be between [0, 1]
		 * @return Color
		 * 		Returns the blended color.
		 */
		static Color lerp(Color src, Color dest, double lerpVal);

		/**
		 * @brief Draws a rectangle using the active color.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param x
		 * 		The x location of the rectangle.
		 * @param y
		 * 		The y location of the rectangle
		 * @param x2
		 * 		The 2nd x location of the rectangle.
		 * @param y2
		 * 		The 2nd y location of the rectangle.
		 * @param outline
		 * 		Controls whether only the outline of the rectangle should be drawn.
		 * @param surf
		 * 		The image to draw the rectangle onto.
		 */
		static void drawRect(int x, int y, int x2, int y2, bool outline, Image* surf);

		/**
		 * @brief Draws a Line using the active color.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param x
		 * 		The x location of the line.
		 * @param y
		 * 		The y location of the line
		 * @param x2
		 * 		The 2nd x location of the line.
		 * @param y2
		 * 		The 2nd y location of the line.
		 * @param surf
		 * 		The image to draw the line onto.
		 */
		static void drawLine(int x, int y, int x2, int y2, Image* surf);

		//TODO
		static void drawBezierCurve(BezierCurve& b, int subdivisions, bool useArcLength, Image* surf);

		/**
		 * @brief Draws a circle using the active color.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param x
		 * 		The center x location of the circle.
		 * @param y
		 * 		The center y location of the circle
		 * @param radius
		 * 		The radius of the circle.
		 * @param outline
		 * 		Controls whether only the outline of the circle should be drawn.
		 * @param surf
		 * 		The image to draw the circle onto.
		 */
		static void drawCircle(int x, int y, int radius, bool outline, Image* surf);

		/**
		 * @brief
		 * 		Test draw function. Should be ignored.
		 * 
		 * 
		 */
		static void testDrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Image* surf);

		/**
		 * @brief Draws a triangle using the active color.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param x
		 * 		The x location of the triangle.
		 * @param y
		 * 		The y location of the triangle
		 * @param x2
		 * 		The 2nd x location of the triangle.
		 * @param y2
		 * 		The 2nd y location of the triangle.
		 * @param x3
		 * 		The 3rd x location of the triangle.
		 * @param y3
		 * 		The 3rd y location of the triangle.
		 * @param outline
		 * 		Controls whether only the outline of the triangle should be drawn.
		 * @param surf
		 * 		The image to draw the triangle onto.
		 */
		static void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool outline, Image* surf);

		/**
		 * @brief Draws a triangle using the specified image. Is not affected by the active color.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * 
		 * 		Uses Vec4f where the x,y specify the point on the triangle and z,w specify texture coordinates
		 * 			between [0, 1]
		 * @param p1
		 * 		The 1st location of the triangle along with it's texture coordinates.
		 * @param p2
		 * 		The 2nd location of the triangle along with it's texture coordinates.
		 * @param p3
		 * 		The 3rd location of the triangle along with it's texture coordinates.
		 * @param texture
		 * 		The texture to map onto the triangle.
		 * @param surf
		 * 		The image to draw the triangle onto.
		 */
		static void drawTexturedTriangle(Vec4f p1, Vec4f p2, Vec4f p3, Image* texture, Image* surf);
		
		/**
		 * @brief Draws an Image. The active color has no influence.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param img
		 * 		The image to draw.
		 * @param x
		 * 		The x location to draw at.
		 * @param y
		 * 		The y location to draw at.
		 * @param surf
		 * 		The image to draw onto.
		 */
		static void drawImage(Image* img, int x, int y, Image* surf);
		
		/**
		 * @brief Draws an Image that is modified by the active drawing color.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param img
		 * 		The image to draw.
		 * @param x
		 * 		The x location to draw at.
		 * @param y
		 * 		The y location to draw at.
		 * @param surf
		 * 		The image to draw onto.
		 */
		static void drawSprite(Image* img, int x, int y, Image* surf);

		/**
		 * @brief Draws a part of an Image that will be modified by the active drawing color.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param img
		 * 		The image to draw.
		 * @param x
		 * 		The x location to draw at.
		 * @param y
		 * 		The y location to draw at.
		 * @param imgX
		 * 		The starting x position of the part of the texture image we are drawing.
		 * @param imgY
		 * 		The starting y position of the part of the texture image we are drawing.
		 * @param imgW
		 * 		The width of the part of the texture image we are drawing.
		 * @param imgH
		 * 		The height of the part of the texture image we are drawing.
		 * @param surf
		 * 		The image to draw onto.
		 */
		static void drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf);

		/**
		 * @brief Draws the specified text using the active font. It is affected by the active drawing color.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param str
		 * 		The text to draw.
		 * @param x
		 * 		The x location to draw at.
		 * @param y
		 * 		The y location to draw at.
		 * @param surf
		 * 		The image to draw onto.
		 */
		static void drawText(std::string str, int x, int y, Image* surf);

		/**
		 * @brief Draws the specified text using the active font. It is affected by the active drawing color.
		 * 		Adds additional limits such as the maximum width and maximum height.
		 * 		If OPTI is defined as 1, SSE instructions are used.
		 * 		If OPTI is defined as 2, AVX instructions are used.
		 * @param str
		 * 		The text to draw.
		 * @param x
		 * 		The x location to draw at.
		 * @param y
		 * 		The y location to draw at.
		 * @param maxWidth
		 * 		The maximum width that is allowed before a line break occurs.
		 * @param maxHeight
		 * 		The maximum height that is allowed.
		 * @param useLineBreaks
		 * 		If set to false, drawing stops if the text hits the maximum width.
		 * @param surf
		 * 		The image to draw onto.
		 */
		static void drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, Image* surf);

		/**
		 * @brief Draws a polygon using the active draw color.
		 * 		Is affected by the current fill rule.
		 * 		May be removed as VectorGraphics accomplishes the same thing but will have more options.
		 * @param points
		 * 		An array of points for the polygon.
		 * @param size
		 * 		The size of the array.
		 * @param surf
		 * 		The image to draw onto.
		 */
		static void drawPolygon(Vec2f* points, int size, Image* surf);
		
		/**
		 * @brief Draws a model using the active draw color or texture.
		 * 		Follows OpenGL drawing. Only 2D shapes are supported.
		 * 		Expects position, and if desired texture, to be defined in the model.
		 * 		Expects position and texture to be Model::TYPE_VEC2.
		 * @param model
		 * 		The model to draw.
		 * @param texture
		 * 		The texture to use if desired.
		 * 		If the model has texture coordinates, this can not be nullptr.
		 * 		Otherwise, it can be nullptr.
		 * @param surf
		 * 		The image to draw onto.
		 */
		static void drawModel(Model* model, Image* texture, Image* surf);
		
		//Getters and Setters

		/**
		 * @brief Sets a rectangle where drawing will be allowed for the following drawing operations.
		 * 		Anything outside of the rectangle is not drawn.
		 * @param b
		 * 		The box to use.
		 */
		static void setClippingRect(Box2D b);

		/**
		 * @brief Gets the clipping rectangle currently used.
		 * 		Anything outside of the rectangle is not drawn.
		 * @return Box2d
		 */
		static Box2D getClippingRect();

		/**
		 * @brief Resets the clipping rectangle.
		 * 		The defaults are:
		 * 			x >= 0 && x<=65535
		 * 			y >= 0 && y<=65535
		 */
		static void resetClippingPlane();

		/**
		 * @brief Sets the currently active drawing color.
		 * @param c
		 */
		static void setColor(Color c);

		/**
		 * @brief Gets the currently active drawing color.
		 * @return Color
		 */
		static Color getColor();

		/**
		 * @brief Sets the currently active Font.
		 * @param f
		 * 		Can be nullptr. If nullptr, one of the default fonts will be used.
		 */
		static void setFont(Font* f);

		/**
		 * @brief Returns the currently active Font.
		 * @return Font*
		 * 		If the active font is nullptr, the currently active default font is returned.
		 */
		static Font* getFont();
		
		/**
		 * @brief Sets the default font to one of the 3 types of default fonts available.
		 * 		These can be modified to the users liking by changing the font files in the resources folder.
		 * @param type
		 * 		Must be one of the following constants.
		 * 			NORMAL_FONT
		 * 			MEDIUM_FONT
		 * 			LARGE_FONT
		 */
		static void setDefaultFont(unsigned char type);

		/**
		 * @brief Returns one of the default font specified.
		 * 		These can be modified to the users liking by changing the font files in the resources folder.
		 * @param type
		 * 		Must be one of the following constants.
		 * 			NORMAL_FONT
		 * 			MEDIUM_FONT
		 * 			LARGE_FONT
		 * @return Font*
		 * 		May return nullptr if the font was not loaded.
		 */
		static Font* getDefaultFont(unsigned char type);

		/**
		 * @brief Sets the Porter Duff rule to use for color and alpha blending.
		 * @param b
		 * 		Must be one of the composite constants.
		 */
		static void setCompositeRule(unsigned char b);

		/**
		 * @brief Returns the Porter Duff rule being used for color and alpha blending.
		 * @return unsigned char
		 */
		static unsigned char getCompositeRule();

		/**
		 * @brief Sets the blend mode to use for color and alpha blending.
		 * 		Currently not implemented.
		 * @param b
		 * 		Must be one of the blend mode constants.
		 */
		static void setBlendMode(unsigned char b);

		/**
		 * @brief Returns the blend mode used for color and alpha blending.
		 * 		Currently not implemented.
		 * @return unsigned char
		 */
		static unsigned char getBlendMode();

		/**
		 * @brief Sets the fill rule to use for drawing polygons.
		 * 		May be removed.
		 * @param b
		 * 		valid options are FILL_EVEN_ODD or FILL_NON_ZERO
		 */
		static void setFillRule(bool b);

		/**
		 * @brief Returns the fill rule used for drawing polygons.
		 * 		May be removed.
		 * @return b
		 */
		static bool getFillRule();
		
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
		
		/**
		 * @brief Returns a new cropped image. Should be deleted when no longer needed.
		 * 		Not currently implemented.
		 */
		static Image* crop(Image* img, int x1, int y1, int x2, int y2);

		/**
		 * @brief Returns a new cropped image. Should be deleted when no longer needed.
		 * 		Not currently implemented.
		 */
		static Image* crop(Image* img, Shape s);

		/**
		 * @brief Replaces the specified color with a new color in the image.
		 * @param img
		 * 		The image to operate on.
		 * @param oldColor
		 * 		The color to replace.
		 * @param newColor
		 * 		The new color to be used.
		 * @param ignoreAlpha
		 * 		Whether to ignore alpha when checking to replace.
		 * 		Note that the alpha will be set to the new colors alpha.
		 */
		static void replaceColor(Image* img, Color oldColor, Color newColor, bool ignoreAlpha = false);

		/**
		 * @brief Removes all colors from the image except the specified color.
		 * @param img
		 * 		The image to operate on.
		 * @param c1
		 * 		The color to keep.
		 */
		static void filterOutColor(Image* img, Color c1);

		/**
		 * @brief Removes all colors from the image unless they fall within the range between
		 * 		the 2 specified colors.
		 * @param img
		 * 		The image to operate on.
		 * @param c1
		 * 		The start of the color range.
		 * @param c2
		 * 		The end of the color range.
		 */
		static void filterOutColorRange(Image* img, Color c1, Color c2);
		
		/**
		 * @brief Converts the pixels in the image to a different color space.
		 * @param img
		 * 		The image to operate on.
		 * @param colorSpace
		 * 		One of the operations defined in ColorSpaceConverter
		 */
		static void convertToColorSpace(Image* img, unsigned char colorSpace);

		/**
		 * @brief Blurs the image using a simple box blur.
		 * @param img
		 * 		The image to operate on.
		 * @param boxSize
		 * 		The size of the box to use for bluring.
		 */
		static void boxBlur(Image* img, int boxSize);

		/**
		 * @brief Blurs the image using a guassian blur.
		 * 		Not implemented.
		 */
		static void gaussianBlur(Image* img, double stdDeviation);

		/**
		 * @brief Applies an uncanny edge filter to the image.
		 * 		Not implemented.
		 */
		static void uncannyEdgeFilter(Image* img);

		/**
		 * @brief
		 * 		Applies a sobel edge filter to the image.
		 * 		Not implemented.
		 */
		static void sobelEdgeFilter(Image* img);
		
		/**
		 * @brief Calculates the gradient of the specified color channel using
		 * 		central differences.
		 * @param img
		 * 		The image to operate on.
		 * @param type
		 * 		Must be one of the following types.
		 * 			RED_CHANNEL
		 * 			GREEN_CHANNEL
		 * 			BLUE_CHANNEL
		 * 			ALPHA_CHANNEL
		 * @return std::vector<std::vector<Vec2f>>
		 * 		Returns a 2D vector of Vec2f where each Vec2f stores the gradient at that point.
		 */
		static std::vector<std::vector<Vec2f>> calculateGradient(Image* img, unsigned char type);
		
		/**
		 * @brief Dithers the image using either Bayer dithering or Floyd Steinburg dithering.
		 * 		Can also just enforce the palette.
		 * 		The image must have a palette set to use this method.
		 * @param img
		 * 		The image to operate on. Must have the palette set.
		 * @param method
		 * 		The dithering method to use. Valid options are:
		 * 			ORDERED_DITHER_BAYER
		 * 			FLOYD_DITHER
		 * 			THRESHOLD
		 * 		If set to THRESHOLD, the image will just have its palette enforced.
		 * 		Default value is THRESHOLD.
		 */
		static void ditherImage(Image* img, unsigned char method = THRESHOLD);

		/**
		 * @brief Scales an image using the specified filter method and returns a new image.
		 * 		That new image should be deleted when no longer needed.
		 * @param img
		 * 		The image to scale.
		 * @param xScale
		 * 		The x scale value.
		 * 		Must be a positive number.
		 * @param yScale
		 * 		The y scale value.
		 * 		Must be a positive number.
		 * @param filterMethod
		 * 		Must be one of the following values:
		 * 			NEAREST_NEIGHBOR_FILTER
		 * 			BILINEAR_FILTER
		 * 			BICUBIC_FILTER
		 * 		The default is NEAREST_NEIGHBOR_FILTER
		 */
		static Image* scaleImage(Image* img, double xScale, double yScale, unsigned char filterMethod = NEAREST_NEIGHBOR_FILTER);

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
		
		static Font* activeFont;
		static Color activeColor;
		static unsigned char compositeRule;
		static unsigned char blendMode;
		static bool fillRule;

		static int ditherMatrixSize;
	};

} //NAMESPACE glib END