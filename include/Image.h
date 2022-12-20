#pragma once
#include "ColorPalette.h"
#include "File.h"
#include "MathExt.h"
#include "Opti.h"

namespace glib
{

	class Image : public Object
	{
	public:
		const static int NONE = 0;
		const static int CLAMP = 1;
		const static int REPEAT = 2;

		const static int NEAREST = 0;
		const static int BILINEAR = 1;
		const static int BICUBIC = 2;

		/**
		 * @brief Construct a new Image object.
		 * 		It stores pixel data using the Color Struct which maps to a 32 bit integer.
		 * 		It is stored as a single array. An invalid image has a width of 0 or height of 0.
		 */
		Image();

		/**
		 * @brief Construct a new Image object.
		 * 		It stores pixel data using the Color Struct which maps to a 32 bit integer.
		 * 		It is stored as a single array. An invalid image has a width of 0 or height of 0.
		 * 
		 * @param width 
		 * 		The width of the image. Must be greater than 0 to be valid.
		 * @param height 
		 * 		The height of the image. Must be greater than 0 to be valid.
		 */
		Image(int width, int height);

		/**
		 * @brief Construct a new Image object from another Image object.
		 * 
		 * @param other 
		 * 		The other Image object to copy.
		 */
		Image(const Image& other);

		/**
		 * @brief Copies a Image object.
		 * 
		 * @param other 
		 * 		The other Image object to copy.
		 */
		void operator=(const Image& other);

		/**
		 * @brief Destroy the Image object
		 * 		Deletes the array of pixels and the palette for the image.
		 */
		~Image();

		/**
		 * @brief Destroys the Image object
		 * 		Deletes the array of pixels and the palette for the image.
		 * 
		 */
		void dispose();

		//Object and Class Stuff
		static const Class globalClass;

		/**
		 * @brief Sets the Sampling Method used to get pixels that are not aligned.
		 * 
		 * @param m 
		 */
		void setSamplingMethod(int m);

		/**
		 * @brief Get the Sampling Method used to get pixels that are not aligned.
		 * 
		 * @return int 
		 */
		int getSamplingMethod();

		/**
		 * @brief Get the Width of the image.
		 * 
		 * @return int 
		 */
		int getWidth();

		/**
		 * @brief Get the Height of the image.
		 * 
		 * @return int 
		 */
		int getHeight();

		/**
		 * @brief Get the Pixels of the image.
		 * 		Returns a reference to the internal array used for pixels for modification or fast access.
		 * 
		 * @return Color* 
		 */
		Color* getPixels();

		/**
		 * @brief Gets a pixel in the image.
		 * 
		 * @param x 
		 * 		X position of the pixel.
		 * @param y 
		 * 		Y position of the pixel.
		 * @param edgeBehavior 
		 * 		The behavior to take if the location is beyond the edges of the image.
		 * @return Color 
		 * 		If unsuccessful, the default Color is return with the value RGBA(0,0,0,0)
		 */
		Color getPixel(int x, int y, int edgeBehavior = NONE);

		/**
		 * @brief Geta pixel in the image.
		 * 		Uses the image's filter method to blend pixels for the final output.
		 * 
		 * @param x 
		 * @param y 
		 * @param edgeBehavior 
		 * 		The behavior to take if the location is beyond the edges of the image.
		 * @return Color 
		 * 		If unsuccessful, the default Color is return with the value RGBA(0,0,0,0)
		 */
		Color getPixel(double x, double y, int edgeBehavior = NONE);

		/**
		 * @brief Set the Pixel at the specified location
		 * 
		 * @param x 
		 * 		X position of the pixel.
		 * @param y 
		 * 		Y position of the pixel.
		 * @param c 
		 * 		The color to set the pixel to.
		 */
		void setPixel(int x, int y, Color c);

		/**
		 * @brief Sets all pixels to the specified color
		 * 
		 * @param c 
		 * 		The specified color.
		 */
		void setAllPixels(Color c);
		
		/**
		 * @brief Copies another Image object to this.
		 * 
		 * @param v 
		 * 		The Image to copy.
		 */
		void copyImage(Image* v);

		/**
		 * @brief Sets the Palette for the image.
		 * 
		 * @param p 
		 * 		The color palette.
		 */
		void setPalette(ColorPalette p);

		/**
		 * @brief Gets the Color Palette for the image.
		 * 		May return an empty palette if it has not been set.
		 * @return ColorPalette& 
		 */
		ColorPalette& getPalette();

		/**
		 * @brief Enforces the Color Palette for the image. 
		 * 		This will replace each color with the closest color in the palette.
		 */
		void enforcePalette();

		//Wrapper around the graphics class for images
		/**
		 * @brief Calls the SimpleGraphics class function clearImage
		 * 
		 */
		void clearImage();

		/**
		 * @brief Calls the SimpleGraphics class function drawRect
		 * 
		 * @param x 
		 * @param y 
		 * @param x2 
		 * @param y2 
		 * @param outline 
		 */
		void drawRect(int x, int y, int x2, int y2, bool outline);

		/**
		 * @brief Calls the SimpleGraphics class function drawLine
		 * 
		 * @param x 
		 * @param y 
		 * @param x2 
		 * @param y2 
		 */
		void drawLine(int x, int y, int x2, int y2);

		/**
		 * @brief Calls the SimpleGraphics class function drawCircle
		 * 
		 * @param x 
		 * @param y 
		 * @param radius 
		 * @param outline 
		 */
		void drawCircle(int x, int y, int radius, bool outline);

		/**
		 * @brief Calls the SimpleGraphics class function drawPolygon
		 * 		May be removed later.
		 * 
		 * @param points 
		 * @param size 
		 */
		void drawPolygon(Vec2f* points, int size);

		/**
		 * @brief Calls the SimpleGraphics class function drawImage
		 * 
		 * @param img 
		 * @param x 
		 * @param y 
		 */
		void drawImage(Image* img, int x, int y);

		/**
		 * @brief Calls the SimpleGraphics class function drawSprite
		 * 
		 * @param img 
		 * @param x 
		 * @param y 
		 */
		void drawSprite(Image* img, int x, int y);

		/**
		 * @brief Calls the SimpleGraphics class function drawSprite
		 * 
		 * @param img 
		 * @param x 
		 * @param y 
		 * @param x2 
		 * @param y2 
		 */
		void drawSprite(Image* img, int x, int y, int x2, int y2);

		/**
		 * @brief Calls the SimpleGraphics class function drawSpritePart
		 * 
		 * @param img 
		 * @param x 
		 * @param y 
		 * @param imgX 
		 * @param imgY 
		 * @param imgW 
		 * @param imgH 
		 */
		void drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH);

		/**
		 * @brief Calls the SimpleGraphics class function drawText
		 * 
		 * @param str 
		 * @param x 
		 * @param y 
		 */
		void drawText(std::string str, int x, int y);
		void drawText(std::wstring str, int x, int y);
		

		/**
		 * @brief Calls the SimpleGraphics class function drawTextLimits
		 * 
		 * @param str 
		 * @param x 
		 * @param y 
		 * @param maxWidth 
		 * @param maxHeight 
		 * @param allowLineBreak 
		 */
		void drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool allowLineBreak);
		void drawTextLimits(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool allowLineBreak);
		
		/**
		 * @brief Calls the SimpleGraphics class function drawTextLimitsHighlighted
		 * 
		 * @param str 
		 * @param x 
		 * @param y 
		 * @param maxWidth 
		 * @param maxHeight 
		 * @param useLineBreak 
		 * @param highlightStart 
		 * @param highlightEnd 
		 * @param highlightColor 
		 */
		void drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor);
		void drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor);

		/**
		 * @brief Calls the SimpleGraphics class function drawTriangle
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 * @param x3 
		 * @param y3 
		 * @param outline 
		 */
		void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool outline);

		/**
		 * @brief Calls the SimpleGraphics class function drawTexturedTriangle
		 * 
		 * @param p1 
		 * @param p2 
		 * @param p3 
		 * @param texture 
		 */
		void drawTexturedTriangle(Vec4f p1, Vec4f p2, Vec4f p3, Image* texture);

		/**
		 * @brief Calls the SimpleGraphics class function drawPixel
		 * 
		 * @param x 
		 * @param y 
		 * @param c 
		 */
		void drawPixel(int x, int y, Color c);

		/**
		 * @brief Calls the SimpleGraphics class function drawPixel
		 * 		This method uses doubles for the position.
		 * 
		 * @param x 
		 * @param y 
		 * @param c 
		 */
		void drawPixel(double x, double y, Color c);
		
		/**
		 * @brief Loads an image from a file.
		 * 		Valid files are .bmp, .jpg, .jpeg, .png, and .gif
		 * 		Can load animations for .png and .gif.
		 * 
		 * @param file 
		 * 		The file to load.
		 * @param amountOfImages 
		 * 		A pointer to an int that will store the amount of images loaded.
		 * @param extraData 
		 * 		A pointer to a vector<int> that will store additional data about the image.
		 * 		For .png and .gif, it stores the delay time between each image in the animation.
		 * @return Image** 
		 * 		Returns an array of Image*. The size of the array is specified by the amountOfImages argument.
		 * 		Should be deleted when no longer needed. The individual Image* should also be deleted when no longer needed.
		 */
		static Image** loadImage(File file, int* amountOfImages, std::vector<int>* extraData = nullptr);

		/**
		 * @brief Saves a image as a .bmp file.
		 * 		Note that .bmp can not save alpha values.
		 * @param File 
		 * 		The output file.
		 * @param alphaThreshold 
		 * 		A threshold to use for alpha. Anything below this threshold will be black.
		 * 		Default is 255
		 * @param greyscale 
		 * 		Whether to convert the image to greyscale before saving. This only affects the output file
		 * 		and not the image data in memory.
		 * 		Default is false
		 */
		void saveBMP(File file, unsigned char alphaThreshold = 255, bool greyscale = false);

		/**
		 * @brief Save an image as a .gif file.
		 * 		Can not save animations.
		 * 		Must use a palette of 256 or less colors. Will be converted to a power of 2.
		 * 		Can use transparency but only 1 value of transparency.
		 * @param file 
		 * 		The output file.
		 * @param paletteSize 
		 * 		The size of the palette to use if the image does not have a valid palette.
		 * 		256 is the maximum size.
		 * 		Default is 256.
		 * @param dither 
		 * 		Whether to use FloydSteinburg dithering for saving the image.
		 * 		Default is false.
		 * @param saveAlpha 
		 * 		Whether to save an alpha value. Uses 1 value in the palette.
		 * 		Default is true.
		 * @param alphaThreshold
		 * 		Threshold on whether a color is visible or completely transparent.
		 * 		Default is 127 
		 * @param greyscale 
		 * 		Whether to convert the image to greyscale before saving. This only affects the output file
		 * 		and not the image data in memory.
		 * 		Default is false
		 */
		void saveGIF(File file, int paletteSize = 256, bool dither = false, bool saveAlpha = true, unsigned char alphaThreshold = 127, bool greyscale = false);

		/**
		 * @brief Save an image as a .png file.
		 * 		Can not save animations
		 * 		Can save full alpha transparency.
		 * 
		 * @param file 
		 * 		The output file.
		 * @param saveAlpha 
		 * 		Determines whether to save the alpha values for the image.
		 * 		Default is true.
		 * @param greyscale 
		 * 		Whether to convert the image to greyscale before saving. This only affects the output file
		 * 		and not the image data in memory.
		 * 		Default is false
		 * @param strongCompression 
		 * 		Determines whether to use better but slower compression to save the image. This will use different filters
		 * 		and custom huffman tables in the deflate method.
		 * 		Default is false
		 */
		void savePNG(File file, bool saveAlpha = true, bool greyscale = false, bool strongCompression = false);

		/**
		 * @brief Not implemented. Do not use.
		 * 
		 * @param filename 
		 * 		The output file.
		 * @param quality
		 * 		Picks the quality matrix used.
		 * 		Quality matrix is the default JPEG quality matrix
		 * 		Valid from 1 - 10
		 * @param subsampleMode
		 * 		Mode 0: No sub sampling (4:4:4)
		 * 		Mode 1: Basic sub sampling (4:2:2)
		 * 		Mode 2: Best Compression (4:2:0)
		 */
		void saveJPG(File file, int quality = 8, int subsampleMode = 0);

	private:
		int width = 0;
		int height = 0;
		int samplingMethod = NEAREST;
		ColorPalette p = ColorPalette();
		Color* pixels = nullptr;

		static Image** loadBMP(std::vector<unsigned char> fileData, int* amountOfImages);
		static Image** loadGIF(std::vector<unsigned char> fileData, int* amountOfImages, std::vector<int>* extraData = nullptr);
		static Image** loadPNG(std::vector<unsigned char> fileData, int* amountOfImages, std::vector<int>* extraData = nullptr);
		static Image** loadJPG(std::vector<unsigned char> fileData, int* amountOfImages);
	};

} //NAMESPACE glib END