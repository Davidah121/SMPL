#include "ext/GLSprite.h"

#ifdef USE_OPENGL
	namespace smpl
	{
		GLSprite::GLSprite()
		{
		}

		GLSprite::~GLSprite()
		{
			dispose();
		}

		GLSprite::GLSprite(const GLSprite& o)
		{
			copy(o);
		}

		void GLSprite::operator=(const GLSprite& o)
		{
			copy(o);
		}

		void GLSprite::copy(const GLSprite& o)
		{
			delayTimeForFrame = o.delayTimeForFrame;
			images = o.images;
		}

		GLSprite::GLSprite(Sprite& o)
		{
			for(size_t i=0; i<o.getSize(); i++)
			{
				images.push_back( new GLTexture(o.getImage(i)) );
				delayTimeForFrame.push_back( o.getDelayTime(i) );
			}
		}

		void GLSprite::dispose()
		{
			for(size_t i=0; i<images.size(); i++)
			{
				if(images[i]!=nullptr)
				{
					delete images[i];
				}
			}

			images.clear();
			delayTimeForFrame.clear();
		}

		GLTexture* GLSprite::getTexture(size_t index)
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

		void GLSprite::addTexture(GLTexture* p)
		{
			images.push_back(p);
		}

		void GLSprite::removeImage(size_t index)
		{
			if (index < images.size())
			{
				std::vector<GLTexture*> newImages = std::vector<GLTexture*>();
				std::vector<int> nDelay = std::vector<int>();

				for (size_t i = 0; i < images.size(); i++)
				{
					if (i != index)
					{
						newImages.push_back(images[i]);
						nDelay.push_back(delayTimeForFrame[i]);
					}
					else
						delete images[i];
				}

				images = newImages;
				delayTimeForFrame = nDelay;
			}
		}

		void GLSprite::loadImage(File file, bool clear)
		{
			if(clear)
				dispose();
			
			Sprite s;
			s.loadImage(file);
			for (size_t i=0; i<s.getSize(); i++)
			{
				GLTexture* k = new GLTexture(s.getImage(i), true);
				addTexture(k);
				delayTimeForFrame.push_back(s.getDelayTime(i));
			}
		}

		
		void GLSprite::loadSpriteSheet(File file, int width, int height, int xStride, int yStride, int count, bool clear)
		{
			if(clear)
				dispose();
			
			Sprite s;
			s.loadSpriteSheet(file, width, height, xStride, yStride, count);
			for (size_t i=0; i<s.getSize(); i++)
			{
				GLTexture* k = new GLTexture(s.getImage(i), true);
				addTexture(k);
				delayTimeForFrame.push_back(s.getDelayTime(i));
			}
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