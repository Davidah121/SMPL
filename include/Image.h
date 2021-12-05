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

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

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
		 * @param clamp 
		 * 		Whether to clamp to the edges of the image if the x or y value is outside of the allowed bounds.
		 * @return Color 
		 * 		If unsuccessful, the default Color is return with the value RGBA(0,0,0,0)
		 */
		Color getPixel(int x, int y, bool clamp = false);

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

		/**
		 * @brief Loads an image from a file.
		 * 		Valid files are .bmp, .jpg, .jpeg, .png, and .gif
		 * 		Can load animations for .png and .gif.
		 * 
		 * @param filename 
		 * 		The name of file to load.
		 * @param amountOfImages 
		 * 		A pointer to an int that will store the amount of images loaded.
		 * @param extraData 
		 * 		A pointer to a vector<int> that will store additional data about the image.
		 * 		For .png and .gif, it stores the delay time between each image in the animation.
		 * @return Image** 
		 * 		Returns an array of Image*. The size of the array is specified by the amountOfImages argument.
		 * 		Should be deleted when no longer needed. The individual Image* should also be deleted when no longer needed.
		 */
		static Image** loadImage(std::string filename, int* amountOfImages, std::vector<int>* extraData = nullptr);

		/**
		 * @brief Loads an image from a file.
		 * 		Valid files are .bmp, .jpg, .jpeg, .png, and .gif
		 * 		Can load animations for .png and .gif.
		 * 
		 * @param filename 
		 * 		The name of file to load.
		 * @param amountOfImages 
		 * 		A pointer to an int that will store the amount of images loaded.
		 * @param extraData 
		 * 		A pointer to a vector<int> that will store additional data about the image.
		 * 		For .png and .gif, it stores the delay time between each image in the animation.
		 * @return Image** 
		 * 		Returns an array of Image*. The size of the array is specified by the amountOfImages argument.
		 * 		Should be deleted when no longer needed. The individual Image* should also be deleted when no longer needed.
		 */
		static Image** loadImage(std::wstring filename, int* amountOfImages, std::vector<int>* extraData = nullptr);

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
		 */
		void saveJPG(File file);

	private:
		int width = 0;
		int height = 0;
		ColorPalette p = ColorPalette();
		Color* pixels = nullptr;

		static Image** loadBMP(std::vector<unsigned char> fileData, int* amountOfImages, std::vector<int>* extraData = nullptr);
		static Image** loadGIF(std::vector<unsigned char> fileData, int* amountOfImages, std::vector<int>* extraData = nullptr);
		static Image** loadPNG(std::vector<unsigned char> fileData, int* amountOfImages, std::vector<int>* extraData = nullptr);
		static Image** loadJPG(std::vector<unsigned char> fileData, int* amountOfImages, std::vector<int>* extraData = nullptr);
	};

} //NAMESPACE glib END