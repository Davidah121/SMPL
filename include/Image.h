#pragma once
#include "BuildOptions.h"
#include "ColorPalette.h"
#include "File.h"
#include "MathExt.h"
#include "Opti.h"

namespace smpl
{

	class DLL_OPTION Image;
	class DLL_OPTION HiResImage;
	
	class DLL_OPTION Image : public Object
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
		 * 		It is stored as a single array of colors. An invalid image has a width of 0 or height of 0.
		 * 
		 * 		Creates an invalid image.
		 */
		Image();

		/**
		 * @brief Construct a new Image object.
		 * 		It stores pixel data using the Color Struct which maps to a 32 bit integer.
		 * 		It is stored as a single array of colors. An invalid image has a width of 0 or height of 0.
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

		// Image(Image&& other);

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

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

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
		 * 		If the image has higher bit depth pixels, these pixels are the quantized versions of the higher bit depth pixels.
		 * 		If modified, the higher bit depth will not be modified and must be modified manually.
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
		 * @brief Copies a HiResImage object to this.
		 * 		Quantizes the colors.
		 * 
		 * @param v 
		 */
		void copyImage(HiResImage* v);

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
		void saveGIF(File file, int paletteSize = 256, bool dither = false, bool saveAlpha = true, unsigned char alphaThreshold = 127);

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

		/**
		 * @brief Attempts to save an animated gif. It uses the same options as the normal GIF save function but
		 * 		it allows each image to have its own palette. This will be the lazy approach for now meaning that
		 * 		it will just replace each image as a whole regardless of how much changed and each will have a local palette.
		 * 		
		 * 		All images in the set must have the same size. If they do not, the function will fail.
		 * 		The default delayTimePerFrame is 100 (0.1 seconds) so the array delayTimePerFrame does not have to be defined and can be null.
		 * 		For a gif
		 * 
		 * @param file 
		 * @param images 
		 * @param size 
		 * @param delayTimePerFrame 
		 * @param loops 
		 * @param paletteSize 
		 * @param dither 
		 * @param saveAlpha 
		 * @param alphaThreshold 
		 * @param greyscale 
		 */
		static bool saveAGIF(File file, Image** images, int size, int* delayTimePerFrame, bool loops, int paletteSize = 256, bool dither = false, bool saveAlpha = true, unsigned char alphaThreshold = 127);

		/**
		 * @brief Attempts to save an animated png. It uses the same options as the normal PNG save function. This will be the lazy
		 * 		approach for now meaning that it will just replace each image as a whole.
		 * 
		 * 		All images in the set must have the same size. If they do not, the function will fail.
		 * 		The default delayTimePerFrame is 100 (0.1 seconds) so the array delayTimePerFrame does not have to be defined and can be null.
		 * 
		 * @param file 
		 * @param images 
		 * @param size 
		 * @param delayTimePerFrame 
		 * @param loops 
		 * @param saveAlpha 
		 * @param greyscale 
		 * @param strongCompression 
		 * @return true 
		 * @return false 
		 */
		static bool saveAPNG(File file, Image** images, int size, int* delayTimePerFrame, bool loops, bool saveAlpha = true, bool greyscale = false, bool strongCompression = false);
		
	private:
		int width = 0;
		int height = 0;
		int samplingMethod = NEAREST;
		ColorPalette p = ColorPalette();

		Color* pixels = nullptr;
	};

	//almost identical. Just load functions and storage
	class DLL_OPTION HiResImage : public Object
	{
	public:
		const static int NONE = 0;
		const static int CLAMP = 1;
		const static int REPEAT = 2;

		const static int NEAREST = 0;
		const static int BILINEAR = 1;
		const static int BICUBIC = 2;

		/**
		 * @brief Construct a new HiResImage object.
		 * 		It stores pixel data using the Color Struct which maps to a 64 bit long.
		 * 		It is stored as a single array of colors. An invalid image has a width of 0 or height of 0.
		 * 
		 * 		Creates an invalid image.
		 */
		HiResImage();

		/**
		 * @brief Construct a new HiResImage object.
		 * 		It stores pixel data using the Color Struct which maps to a 64 bit long.
		 * 		It is stored as a single array of colors. An invalid image has a width of 0 or height of 0.
		 * 
		 * @param width 
		 * 		The width of the image. Must be greater than 0 to be valid.
		 * @param height 
		 * 		The height of the image. Must be greater than 0 to be valid.
		 */
		HiResImage(int width, int height);

		/**
		 * @brief Construct a new HiResImage object from another HiResImage object.
		 * 
		 * @param other 
		 * 		The other HiResImage object to copy.
		 */
		HiResImage(const HiResImage& other);

		/**
		 * @brief Copies a HiResImage object.
		 * 
		 * @param other 
		 * 		The other HiResImage object to copy.
		 */
		void operator=(const HiResImage& other);

		// HiResImage(HiResImage&& other);

		/**
		 * @brief Destroy the HiResImage object
		 * 		Deletes the array of pixels and the palette for the image.
		 */
		~HiResImage();

		/**
		 * @brief Destroys the HiResImage object
		 * 		Deletes the array of pixels and the palette for the image.
		 * 
		 */
		void dispose();

		//Object and RootClass Stuff
		static const RootClass globalClass;
		virtual const RootClass* getClass();

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
		 * @return Color4f* 
		 */
		Color4f* getPixels();

		/**
		 * @brief Gets a pixel in the image.
		 * 
		 * @param x 
		 * 		X position of the pixel.
		 * @param y 
		 * 		Y position of the pixel.
		 * @param edgeBehavior 
		 * 		The behavior to take if the location is beyond the edges of the image.
		 * @return Vec4f 
		 * 		If unsuccessful, the default Color4f is return with the value RGBA(0,0,0,0)
		 */
		Color4f getPixel(int x, int y, int edgeBehavior = NONE);

		/**
		 * @brief Geta pixel in the image.
		 * 		Uses the image's filter method to blend pixels for the final output.
		 * 
		 * @param x 
		 * @param y 
		 * @param edgeBehavior 
		 * 		The behavior to take if the location is beyond the edges of the image.
		 * @return Color4f 
		 * 		If unsuccessful, the default Color4f is return with the value RGBA(0,0,0,0)
		 */
		Color4f getPixel(double x, double y, int edgeBehavior = NONE);

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
		void setPixel(int x, int y, Color4f c);

		/**
		 * @brief Sets all pixels to the specified color
		 * 
		 * @param c 
		 * 		The specified color.
		 */
		void setAllPixels(Color4f c);
		
		/**
		 * @brief Copies another HiResImage object to this.
		 * 
		 * @param v 
		 * 		The HiResImage to copy.
		 */
		void copyImage(HiResImage* v);

		
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
		 * @param file 
		 * 		The file to load.
		 * @param amountOfImages 
		 * 		A pointer to an int that will store the amount of images loaded.
		 * @param extraData 
		 * 		A pointer to a vector<int> that will store additional data about the image.
		 * 		For .png and .gif, it stores the delay time between each image in the animation.
		 * @return HiResImage** 
		 * 		Returns an array of HiResImage*. The size of the array is specified by the amountOfImages argument.
		 * 		Should be deleted when no longer needed. The individual HiResImage* should also be deleted when no longer needed.
		 */
		static HiResImage** loadImage(File file, int* amountOfImages, std::vector<int>* extraData = nullptr);

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
		void saveGIF(File file, int paletteSize = 256, bool dither = false, bool saveAlpha = true, unsigned char alphaThreshold = 127);

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

		/**
		 * @brief Attempts to save an animated gif. It uses the same options as the normal GIF save function but
		 * 		it allows each image to have its own palette. This will be the lazy approach for now meaning that
		 * 		it will just replace each image as a whole regardless of how much changed and each will have a local palette.
		 * 		
		 * 		All images in the set must have the same size. If they do not, the function will fail.
		 * 		The default delayTimePerFrame is 100 (0.1 seconds) so the array delayTimePerFrame does not have to be defined and can be null.
		 * 		For a gif
		 * 
		 * @param file 
		 * @param images 
		 * @param size 
		 * @param delayTimePerFrame 
		 * @param loops 
		 * @param paletteSize 
		 * @param dither 
		 * @param saveAlpha 
		 * @param alphaThreshold 
		 * @param greyscale 
		 */
		static bool saveAGIF(File file, HiResImage** images, int size, int* delayTimePerFrame, bool loops, int paletteSize = 256, bool dither = false, bool saveAlpha = true, unsigned char alphaThreshold = 127);

		/**
		 * @brief Attempts to save an animated png. It uses the same options as the normal PNG save function. This will be the lazy
		 * 		approach for now meaning that it will just replace each image as a whole.
		 * 
		 * 		All images in the set must have the same size. If they do not, the function will fail.
		 * 		The default delayTimePerFrame is 100 (0.1 seconds) so the array delayTimePerFrame does not have to be defined and can be null.
		 * 
		 * @param file 
		 * @param images 
		 * @param size 
		 * @param delayTimePerFrame 
		 * @param loops 
		 * @param saveAlpha 
		 * @param greyscale 
		 * @param strongCompression 
		 * @return true 
		 * @return false 
		 */
		static bool saveAPNG(File file, HiResImage** images, int size, int* delayTimePerFrame, bool loops, bool saveAlpha = true, bool greyscale = false, bool strongCompression = false);
		
	private:
		int width = 0;
		int height = 0;
		int samplingMethod = NEAREST;

		Color4f* pixels = nullptr;
		ColorPalette p;

		static HiResImage** loadBMP(std::vector<unsigned char> fileData, int* amountOfImages);
		static HiResImage** loadGIF(std::vector<unsigned char> fileData, int* amountOfImages, std::vector<int>* extraData = nullptr);
		static HiResImage** loadPNG(std::vector<unsigned char> fileData, int* amountOfImages, std::vector<int>* extraData = nullptr);
		static HiResImage** loadJPG(std::vector<unsigned char> fileData, int* amountOfImages);
	};

} //NAMESPACE smpl END