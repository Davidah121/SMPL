#ifdef USE_OPENGL

	#include "ext/GLSprite.h"

	namespace glib
	{
			
		const Class GLSprite::myClass = Class("GLSprite", {&Object::myClass});
		const Class* GLSprite::getClass()
		{
			return &GLSprite::myClass;
		}

		GLSprite::GLSprite()
		{
			setClass(GLSprite::myClass);
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
			setClass(GLSprite::myClass);
			delayTimeForFrame = o.delayTimeForFrame;
			for(int i=0; i<o.images.size(); i++)
			{
				images.push_back( new GLTexture(o.getImage(i)) );
			}
		}

		GLSprite::GLSprite(Sprite& o)
		{
			for(int i=0; i<o.getSize(); i++)
			{
				images.push_back( new GLTexture(o.getImage(i)) );
				delayTimeForFrame.push_back( o.getDelayTime(i) );
			}
		}

		void GLSprite::dispose()
		{
			for(int i=0; i<images.size(); i++)
			{
				if(images[i]!=nullptr)
				{
					delete images[i];
				}
			}

			images.clear();
			delayTimeForFrame.clear();
		}

		GLTexture* GLSprite::getTexture(int index)
		{
			if (images.size() > index && index >= 0)
			{
				return images[index];
			}
			return nullptr;
		}

		int GLSprite::getDelayTime(int index)
		{
			if (images.size() > index && index >= 0)
			{
				return delayTimeForFrame[index];
			}
			return -1;
		}

		void GLSprite::setDelayTime(int index, int microSecondsDelay)
		{
			if (images.size() > index && index >= 0)
			{
				delayTimeForFrame[index] = microSecondsDelay;
			}
		}

		int GLSprite::getSize()
		{
			return images.size();
		}

		void GLSprite::addTexture(GLTexture* p, int microSecondsDelay)
		{
			images.push_back(p);
			delayTimeForFrame.push_back(microSecondsDelay);
		}

		void GLSprite::removeImage(int index)
		{
			if (index >= 0 && index < images.size())
			{
				std::vector<GLTexture*> newImages = std::vector<GLTexture*>();
				std::vector<int> newImageDelay = std::vector<int>();

				for (int i = 0; i < images.size(); i++)
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