#include "ext/GLSprite.h"

#ifdef USE_OPENGL
	namespace glib
	{
			
		const RootClass GLSprite::globalClass = RootClass("GLSprite", {"Object"});

		GLSprite::GLSprite()
		{
			setClass(GLSprite::globalClass);
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
			setClass(GLSprite::globalClass);
			delayTimeForFrame = o.delayTimeForFrame;
			images = o.images;
		}

		GLSprite::GLSprite(Sprite& o)
		{
			setClass(GLSprite::globalClass);
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

		void GLSprite::loadImage(File file)
		{
			dispose();
			int amountOfImages = 0;

			std::vector<int> extraData;
			Image** imgs = Image::loadImage(file, &amountOfImages, &extraData);
			
			if(extraData.size()>=1)
			{
				this->loops = extraData[0] == 1;
			}

			for (int i = 0; i < amountOfImages; i++)
			{
				GLTexture* k = new GLTexture(imgs[i], true);
				addTexture(k);
				if(extraData.size() == (size_t)amountOfImages+1)
					delayTimeForFrame.push_back(extraData[i]);
				else
					delayTimeForFrame.push_back(100);
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

	} //NAMESPACE glib END
	
#endif