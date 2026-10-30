#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "Image.h"
#include "SimpleSerialization.h"
#include "SmartMemory.h"

namespace smpl
{

	class DLL_OPTION Sprite : public SerializedObject
	{
	public:
		/**
		 * @brief Construct a new Sprite object
		 * 		A sprite in this case is a collection of images.
		 * 		They can be used for animation and each image can have a unique delay time between the next image.
		 * 		Can be used as a wrapper for the Image Loading functions.
		 *		
		 *		No sprite by default claims 100% ownership over the images contained and they can be copied over to other sprites in a shallow way
		 *			The pointer itself is copied.
		 *		If hard copies are directly needed, a hard copy function is provided.
		 */
		Sprite();

		Sprite(const Sprite& o) = default;
		Sprite& operator=(const Sprite& o) = default;

		Sprite(Sprite&& o) = default;
		Sprite& operator=(Sprite&& o) = default;

		/**
		 * @brief Copies a sprite object.
		 * 		Hard copy
		 * 
		 * @param o 
		 */
		void hardCopy(const Sprite& o);

		/**
		 * @brief Destroy the Sprite object
		 * 
		 */
		~Sprite();

		/**
		 * @brief Gets an image from the sprite at the specified index.
		 * 		May return a nullptr if the image is no longer valid for some reason but this would be due to improper programming
		 *		and not by design. It is typically safe to assume that as long as index is valid, the pointer is valid too.
		 *			For extra safety, obtain a locking pointer of that image and pass that raw pointer around as needed.
		 *
		 * @param index 
		 * @return SmartMemory<Image> 
		 */
		SmartMemory<Image> getImage(size_t index) const;

		/**
		 * @brief Gets the Delay Time for the image.
		 * 		Specifies how long before showing the next image in milliseconds.
		 * 
		 * @param index 
		 * @return size_t 
		 */
		int getDelayTime(size_t index) const;

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
		size_t getSize() const;

		/**
		 * @brief Adds a new image to the sprite with a default delay time of 0.
		 *		These SmartMemory objects may be of any type. Not strictly a shared_pointer.
		 * 
		 * @param p
		 */
		void addImage(const SmartMemory<Image>& p);

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
		bool loadImage(const File& file, bool clear = true);

		/**
		 * @brief Loads an image that contains a sprite sheet/atlas. These images contain smaller images separated
		 * 		from each other by empty space typically and is common for 2D animations.
		 * 
		 * @param file 
		 * @param width 
		 * 		The width of each image in the sprite sheet. Assumed that all have equal width.
		 * @param height 
		 * 		The height of each image in the sprite sheet. Assumed that all have equal height.
		 * @param xStride 
		 * 		The additional horizontal space between each column in the sprite sheet.
		 * @param yStride 
		 * 		The additional vertical space between each row in the sprite sheet.
		 * @param count
		 * 		This specifies how many images are in the sprite sheet.
		 * 		Set to a negative number to just process each spot as a sprite.
		 * 			Note that this can result in completely empty images. 
		 * @param clear 
		 * 		Clears the sprite when called
		 * 		Default is true.
		 */
		bool loadSpriteSheet(const File& file, int width, int height, int xStride, int yStride, int count, bool clear = true);
		
		/**
		 * @brief Saves the sprite as a APNG.
		 * 		Whether it loops and delay time per frame are expected to be defined.
		 * 
		 * @param file 
		 * @param saveAlpha
		 * @param greyscale
		 * @param strongCompression
		 */
		bool saveAPNG(const File& file, bool saveAlpha = true, bool greyscale = false, bool strongCompression = false) const;

		/**
		 * @brief Saves the sprite as a Animated GIF.
		 * 		Whether it loops and delay time per frame are expected to be defined.
		 * 
		 * @param file 
		 */
		bool saveAGIF(const File& file, int paletteSize = 256, bool dither = false, bool saveAlpha = true, unsigned char alphaThreshold = 127) const;

		/**
		 * @brief Deletes the images from the sprite. Any images added to the sprite, including those that were loaded,
		 * 		will all be considered invalid unless it is owned by another sprite or at some other location.
		 *		All smart memory created from the class functions are deleted on last reference (shared_pointers).
		 *
		 *		Any smart memory coming from another location added via addImage() may be different.
		 * 
		 */
		void clear();

		/**
		 * @brief Returns if the animation for the sprite should loop.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool shouldLoop() const;

		/**
		 * @brief Sets whether the sprite should loop.
		 * 
		 * @param v
		 */
		void setShouldLoop(bool v);

	private:
		bool loops = true;
		std::vector<SmartMemory<Image>> images = std::vector<SmartMemory<Image>>();
		std::vector<int> delayTimeForFrame = std::vector<int>();

		SERIALIZE_CLASS()
	};

} //NAMESPACE glib END