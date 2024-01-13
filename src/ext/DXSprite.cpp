#ifdef USE_DIRECTX

	#include "ext/DXSprite.h"

	namespace glib
	{
			
		const RootClass DXSprite::myClass = RootClass("DXSprite", {"Object"});

		DXSprite::DXSprite()
		{
		}


		DXSprite::~DXSprite()
		{
			dispose();
		}

		DXSprite::DXSprite(const DXSprite& o)
		{
			images = o.images;
			delayTimeForFrame = o.delayTimeForFrame;
		}

		void DXSprite::operator=(const DXSprite& o)
		{
			images = o.images;
			delayTimeForFrame = o.delayTimeForFrame;
		}

		void DXSprite::dispose()
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

		DXTexture* DXSprite::getTexture(int index)
		{
			if (images.size() > index && index >= 0)
			{
				return images[index];
			}
			return nullptr;
		}

		int DXSprite::getDelayTime(int index)
		{
			if (images.size() > index && index >= 0)
			{
				return delayTimeForFrame[index];
			}
			return -1;
		}

		void DXSprite::setDelayTime(int index, int microSecondsDelay)
		{
			if (images.size() > index && index >= 0)
			{
				delayTimeForFrame[index] = microSecondsDelay;
			}
		}

		int DXSprite::getSize()
		{
			return images.size();
		}

		void DXSprite::addTexture(DXTexture* p, int microSecondsDelay)
		{
			images.push_back(p);
			delayTimeForFrame.push_back(microSecondsDelay);
		}

		void DXSprite::removeImage(int index)
		{
			if (index >= 0 && index < images.size())
			{
				std::vector<DXTexture*> newImages = std::vector<DXTexture*>();
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

		void DXSprite::loadImage(File file)
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
				DXTexture* k = new DXTexture(imgs[i], true);

				if(extraData.size() == amountOfImages+1)
					addTexture(k, extraData[i+1]);
				else
					addTexture(k);
			}
		}

		bool DXSprite::shouldLoop()
		{
			return loops;
		}

		void DXSprite::setShouldLoop(bool v)
		{
			loops = v;
		}

	} //NAMESPACE glib END

#endif