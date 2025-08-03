#pragma once

#ifdef USE_OPENGL

	#include "Image.h"
	#include "ext/GLTexture.h"
	#include "SimpleSerialization.h"
	#include "Sprite.h"

	namespace smpl
	{

		class GLSprite : public SerializedObject
		{
		public:
			/**
			 * @brief Construct a new GLSprite object
			 * 		A sprite in this case is a collection of images.
			 * 		They can be used for animation and each image can have a unique delay time between the next image.
			 * 		Can be used as a wrapper for the Image Loading functions.
			 */
			GLSprite();

			/**
			 * @brief Construct a new GLSprite object from a Sprite.
			 * 		Useful to prevent a new load and convert a software based sprite to
			 * 		a graphics accelerated one.
			 * 
			 * @param o 
			 */
			GLSprite(Sprite& o);

			/**
			 * @brief Construct a new GLSprite object from another GLSprite object
			 * 		Does not re-create the textures but instead just copies them.
			 * 			Equivalent to a shallow copy.
			 * 
			 * @param o 
			 */
			GLSprite(const GLSprite& o);

			/**
			 * @brief Copies a GLSprite object
			 * 
			 * @param o 
			 */
			void operator=(const GLSprite& o);

			/**
			 * @brief Copies a GLSprite object
			 * 		Hard Copy
			 * 
			 * @param o 
			 */
			void copy(const GLSprite& o);

			/**
			 * @brief Destroy the GLSprite object
			 * 
			 */
			~GLSprite();

			/**
			 * @brief Gets an texture from the sprite at the specified index.
			 * 
			 * @param index 
			 * @return GLTexture* 
			 */
			GLTexture* getTexture(size_t index);

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
			void addTexture(GLTexture* p);

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
			 * @brief Removes an image from the sprite.
			 * 
			 * @param index 
			 */
			void removeImage(size_t index);
			
			/**
			 * @brief Loads an image from a file. If the file contains multiple images, they will be loaded as well.
			 * 
			 * @param file 
			 */
			void loadImage(File file, bool clear=false);
			
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
			void loadSpriteSheet(File file, int width, int height, int xStride, int yStride, int count, bool clear=false);
			

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
			std::vector<GLTexture*> images = std::vector<GLTexture*>();
			std::vector<int> delayTimeForFrame = std::vector<int>();

		SERIALIZE_CLASS(loops, images, delayTimeForFrame)
		};

	} //NAMESPACE smpl END
	
#endif