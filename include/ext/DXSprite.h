#pragma once

#ifdef USE_DIRECTX

	#include "Image.h"
	#include "ext/DXTexture.h"
	#include "Object.h"

	namespace glib
	{

		class DXSprite : public Object
		{
		public:
			/**
			 * @brief Construct a new DXSprite object
			 * 		A sprite in this case is a collection of images.
			 * 		They can be used for animation and each image can have a unique delay time between the next image.
			 * 		Can be used as a wrapper for the Image Loading functions.
			 */
			DXSprite();

			/**
			 * @brief Construct a new DXSprite object from another DXSprite object
			 * 
			 * @param o 
			 */
			DXSprite(const DXSprite& o);

			/**
			 * @brief Copies a DXSprite object
			 * 
			 * @param o 
			 */
			void operator=(const DXSprite& o);

			/**
			 * @brief Destroy the DXSprite object
			 * 
			 */
			~DXSprite();

			//Object and Class Stuff
			static const Class globalClass;

			/**
			 * @brief Gets an texture from the sprite at the specified index.
			 * 
			 * @param index 
			 * @return DXTexture* 
			 */
			DXTexture* getTexture(int index);

			/**
			 * @brief Gets the Delay Time for the image.
			 * 		Specifies how long before showing the next image in microseconds.
			 * 
			 * @param index 
			 * @return int 
			 */
			int getDelayTime(int index);

			/**
			 * @brief Gets the amount of images in the sprite.
			 * 
			 * @return int 
			 */
			int getSize();

			/**
			 * @brief Adds a new image to the sprite with a delay value.
			 * 
			 * @param p 
			 * @param microSecondsDelay 
			 * 		Default value is 0
			 */
			void addTexture(DXTexture* p, int microSecondsDelay = 0);

			/**
			 * @brief Set the Delay Time for an image in the sprite specified by the index.
			 * 
			 * @param index 
			 * @param microSecondsDelay 
			 */
			void setDelayTime(int index, int microSecondsDelay);

			/**
			 * @brief Removes an image from the sprite.
			 * 
			 * @param index 
			 */
			void removeImage(int index);
			
			/**
			 * @brief Loads an image from a file. If the file contains multiple images, they will be loaded as well.
			 * 
			 * @param file 
			 */
			void loadImage(File file);

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
			std::vector<DXTexture*> images = std::vector<DXTexture*>();
			std::vector<int> delayTimeForFrame = std::vector<int>();
		};

	} //NAMESPACE glib END

#endif