#pragma once
#include"Image.h"
#include "Object.h"

namespace glib
{

	class Sprite : public Object
	{
	public:
		/**
		 * @brief Construct a new Sprite object
		 * 		A sprite in this case is a collection of images.
		 * 		They can be used for animation and each image can have a unique delay time between the next image.
		 * 		Can be used as a wrapper for the Image Loading functions.
		 */
		Sprite();

		/**
		 * @brief Construct a new Sprite object from another Sprite object
		 * 		Hard copy
		 * @param o 
		 */
		Sprite(const Sprite& o);

		/**
		 * @brief Copies a Sprite object
		 * 		Hard copy
		 * @param o 
		 */
		void operator=(const Sprite& o);

		/**
		 * @brief Copies a sprite object.
		 * 		Hard copy
		 * 
		 * @param o 
		 */
		void copy(const Sprite& o);

		/**
		 * @brief Destroy the Sprite object
		 * 
		 */
		~Sprite();

		//Object and Class Stuff
		static const Class globalClass;

		/**
		 * @brief Gets an image from the sprite at the specified index.
		 * 
		 * @param index 
		 * @return Image* 
		 */
		Image* getImage(size_t index);

		/**
		 * @brief Gets the Delay Time for the image.
		 * 		Specifies how long before showing the next image in milliseconds.
		 * 
		 * @param index 
		 * @return size_t 
		 */
		int getDelayTime(size_t index);

		/**
		 * @brief Set the Delay Time in milliseconds.
		 * 		This is how much time to wait between each image to display it.
		 * 
		 * @param millis 
		 */
		void setDelayTime(size_t index, int millis);

		/**
		 * @brief Gets the amount of images in the sprite.
		 * 
		 * @return size_t 
		 */
		size_t getSize();

		/**
		 * @brief Adds a new image to the sprite with a delay value.
		 * 
		 * @param p
		 */
		void addImage(Image* p);

		/**
		 * @brief Removes an image from the sprite.
		 * 
		 * @param index 
		 */
		void removeImage(size_t index);
		
		/**
		 * @brief Loads an image from a file. If the file contains multiple images, they will be loaded as well.
		 * 
		 * @param file 
		 * @param clear
		 * 		Clears the sprite when called
		 * 		Default is true.
		 */
		void loadImage(File file, bool clear = true);

		/**
		 * @brief Saves the sprite as a APNG.
		 * 		Whether it loops and delay time per frame are expected to be defined.
		 * 
		 * @param file 
		 * @param saveAlpha
		 * @param greyscale
		 * @param strongCompression
		 */
		bool saveAPNG(File file, bool saveAlpha = true, bool greyscale = false, bool strongCompression = false);

		/**
		 * @brief Saves the sprite as a Animated GIF.
		 * 		Whether it loops and delay time per frame are expected to be defined.
		 * 
		 * @param file 
		 */
		bool saveAGIF(File file, int paletteSize = 256, bool dither = false, bool saveAlpha = true, unsigned char alphaThreshold = 127);

		/**
		 * @brief Deletes the images from the sprite. Any images added to the sprite, including those that were loaded,
		 * 		will all be considered invalid.
		 * 
		 */
		void dispose();

		/**
		 * @brief Returns if the animation for the sprite should loop.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool shouldLoop();

		/**
		 * @brief Sets whether the sprite should loop.
		 * 
		 * @param v
		 */
		void setShouldLoop(bool v);

	private:
		bool loops = true;
		std::vector<Image*> images = std::vector<Image*>();
		std::vector<int> delayTimeForFrame = std::vector<int>();
	};

} //NAMESPACE glib END