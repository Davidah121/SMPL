#pragma once

#ifdef USE_OPENGL

    #include "StandardTypes.h"
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
			GLSprite(const Sprite& o);

			GLSprite(const GLSprite& o) = default;
			GLSprite& operator=(const GLSprite& o) = default;
			
			GLSprite(GLSprite&& o) = default;
			GLSprite& operator=(GLSprite&& o) = default;

			/**
			 * @brief Copies a GLSprite object
			 * 		Hard Copy
			 * 
			 * @param o 
			 */
			void hardCopy(const GLSprite& o);

			/**
			 * @brief Destroy the GLSprite object
			 * 
			 */
			~GLSprite();

			/**
			 * @brief Gets an texture from the sprite at the specified index.
			 * 
			 * @param index 
			 * @return SmartMemory<GLTexture>
			 */
			SmartMemory<GLTexture> getTexture(size_t index);

			/**
			 * @brief Gets the amount of images in the sprite.
			 * 
			 * @return size_t 
			 */
			size_t getSize();

			/**
			 * @brief Adds a new image to the sprite with a default delay value of 0.
			 * 
			 * @param p 
			 */
			void addTexture(const SmartMemory<GLTexture>& p);

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
			bool loadImage(const File& file, bool clear=true, bool includeAlpha=true, GLTextureParams params = {GLTexture::BEHAVIOR_CLAMP_TO_EDGE, GLTexture::FILTER_LINEAR});
			
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
			bool loadSpriteSheet(const File& file, int width, int height, int xStride, int yStride, int count, bool clear=true, bool includeAlpha=true, GLTextureParams params = {GLTexture::BEHAVIOR_CLAMP_TO_EDGE, GLTexture::FILTER_LINEAR});
			

			/**
			 * @brief Deletes the images from the sprite. Works just like the software Sprite class where all smart pointers are cleared according to how they were created.
			 *		All smart pointers created from class functions are the equivalent of shared_pointers
			 * 
			 */
			void clear();

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
			std::vector<SmartMemory<GLTexture>> images = std::vector<SmartMemory<GLTexture>>();
			std::vector<int> delayTimeForFrame = std::vector<int>();

		SERIALIZE_CLASS(loops, images, delayTimeForFrame)
		};

	} //NAMESPACE smpl END
	
#endif