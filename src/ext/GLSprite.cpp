#include "ext/GLSprite.h"

#ifdef USE_OPENGL
	namespace smpl
	{
		GLSprite::GLSprite()
		{
		}

		GLSprite::~GLSprite()
		{
		}

		void GLSprite::hardCopy(const GLSprite& o)
		{
			delayTimeForFrame = o.delayTimeForFrame;
			loops = o.loops;
			
			//hard copy
			for(size_t i=0; i<o.images.size(); i++)
			{
				LockingSmartMemory<GLTexture> lockedPointer = o.images[i].getLockingPointer();
				if(lockedPointer == nullptr)
				{
					images.push_back(nullptr);
				}
				else
				{
					GLTexture* nImg = new GLTexture();
					nImg->copyTexture(*lockedPointer.getRawPointer());
					images.push_back( nImg );
				}
			}
			// images = o.images;
		}

		GLSprite::GLSprite(const Sprite& o)
		{
			for(size_t i=0; i<o.getSize(); i++)
			{
				LockingSmartMemory<Image> img = o.getImage(i).getLockingPointer();
				images.push_back( new GLTexture(img.getPointer()) );
				delayTimeForFrame.push_back( o.getDelayTime(i) );
			}
		}

		void GLSprite::clear()
		{
			images.clear();
			delayTimeForFrame.clear();
		}

		SmartMemory<GLTexture> GLSprite::getTexture(size_t index)
		{
			if (images.size() > index)
			{
				return images[index];
			}
			return nullptr;
		}

		int GLSprite::getDelayTime(size_t index)
		{
			if(index < delayTimeForFrame.size())
				return delayTimeForFrame[index];
			return 100;
		}

		void GLSprite::setDelayTime(size_t index, int milliSecondsDelay)
		{
			delayTimeForFrame[index] = milliSecondsDelay;
		}

		size_t GLSprite::getSize()
		{
			return images.size();
		}

		void GLSprite::addTexture(const SmartMemory<GLTexture>& p)
		{
			images.push_back(p);
		}

		void GLSprite::removeImage(size_t index)
		{
			if (index < images.size())
			{
				std::vector<SmartMemory<GLTexture>> newImages = std::vector<SmartMemory<GLTexture>>();
				std::vector<int> nDelay = std::vector<int>();

				for (size_t i = 0; i < images.size(); i++)
				{
					if (i != index)
					{
						newImages.push_back(images[i]);
						nDelay.push_back(delayTimeForFrame[i]);
					}
				}

				images = newImages;
				delayTimeForFrame = nDelay;
			}
		}

		bool GLSprite::loadImage(const File& file, bool shouldClear, bool includeAlpha, GLTextureParams params)
		{
			if(shouldClear)
				clear();
			
			Sprite s;
			bool fine = s.loadImage(file);
			for (size_t i=0; i<s.getSize(); i++)
			{
				SmartMemory<GLTexture> k = new GLTexture(s.getImage(i).getRawPointer(), includeAlpha, params);
				addTexture(k);
				delayTimeForFrame.push_back(s.getDelayTime(i));
			}
			return fine;
		}

		
		bool GLSprite::loadSpriteSheet(const File& file, int width, int height, int xStride, int yStride, int count, bool shouldClear, bool includeAlpha, GLTextureParams params)
		{
			if(shouldClear)
				clear();
			
			Sprite s;
			bool fine = s.loadSpriteSheet(file, width, height, xStride, yStride, count);
			for (size_t i=0; i<s.getSize(); i++)
			{
				GLTexture* k = new GLTexture(s.getImage(i).getRawPointer(), includeAlpha, params);
				addTexture(k);
				delayTimeForFrame.push_back(s.getDelayTime(i));
			}
			return fine;
		}

		bool GLSprite::shouldLoop()
		{
			return loops;
		}

		void GLSprite::setShouldLoop(bool v)
		{
			loops = v;
		}

	} //NAMESPACE smpl END
	
#endif