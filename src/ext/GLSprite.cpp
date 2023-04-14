#include "ext/GLSprite.h"

#ifdef USE_OPENGL
	namespace glib
	{
			
		const Class GLSprite::globalClass = Class("GLSprite", {&Object::globalClass});

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

		size_t GLSprite::getDelayTime(size_t index)
		{
			if (images.size() > index)
			{
				return delayTimeForFrame[index];
			}
			return 0;
		}

		void GLSprite::setDelayTime(size_t index, int microSecondsDelay)
		{
			if (images.size() > index)
			{
				delayTimeForFrame[index] = microSecondsDelay;
			}
		}

		size_t GLSprite::getSize()
		{
			return images.size();
		}

		void GLSprite::addTexture(GLTexture* p, int microSecondsDelay)
		{
			images.push_back(p);
			delayTimeForFrame.push_back(microSecondsDelay);
		}

		void GLSprite::removeImage(size_t index)
		{
			if (index < images.size())
			{
				std::vector<GLTexture*> newImages = std::vector<GLTexture*>();
				std::vector<int> newImageDelay = std::vector<int>();

				for (size_t i = 0; i < images.size(); i++)
				{
					if (i != index)
					{
						newImages.push_back(images[i]);
						newImageDelay.push_back(delayTimeForFrame[i]);
					}
					else
						delete images[i];
				}

				images = newImages;
				delayTimeForFrame = newImageDelay;
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

				if(extraData.size() == amountOfImages+1)
					addTexture(k, extraData[i+1]);
				else
					addTexture(k);
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