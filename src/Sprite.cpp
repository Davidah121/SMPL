#include "Sprite.h"

namespace glib
{
		
	const Class Sprite::globalClass = Class("Sprite", {&Object::globalClass});
	
	Sprite::Sprite()
	{
		setClass(globalClass);
	}

	Sprite::~Sprite()
	{
		dispose();
	}

	Sprite::Sprite(const Sprite& o)
	{
		copy(o);
	}

	void Sprite::operator=(const Sprite& o)
	{
		copy(o);
	}

	void Sprite::copy(const Sprite& o)
	{
		setClass(globalClass);
		delayTimeForFrame = o.delayTimeForFrame;

		//hard copy
		for(size_t i=0; i<o.images.size(); i++)
		{
			Image* nImg = new Image();
			nImg->copyImage(o.images[i]);
			images.push_back( nImg );
		}
	}

	void Sprite::dispose()
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

	Image* Sprite::getImage(size_t index)
	{
		if (index < images.size())
		{
			return images[index];
		}
		return nullptr;
	}

	int Sprite::getDelayTime(size_t index)
	{
		if (index < images.size())
		{
			return delayTimeForFrame[index];
		}
		return -1;
	}

	void Sprite::setDelayTime(size_t index, int microSecondsDelay)
	{
		if (index < images.size())
		{
			delayTimeForFrame[index] = microSecondsDelay;
		}
	}

	size_t Sprite::getSize()
	{
		return images.size();
	}

	void Sprite::addImage(Image* p, int microSecondsDelay)
	{
		images.push_back(p);
		delayTimeForFrame.push_back(microSecondsDelay);
	}

	void Sprite::removeImage(size_t index)
	{
		if (index < images.size())
		{
			std::vector<Image*> newImages = std::vector<Image*>();
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

	void Sprite::loadImage(File file)
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
			if(extraData.size() == (size_t)amountOfImages+1)
				addImage(imgs[i], extraData[i+1]);
			else
				addImage(imgs[i]);
		}
	}

	bool Sprite::shouldLoop()
	{
		return loops;
	}

	void Sprite::setShouldLoop(bool v)
	{
		loops = v;
	}

} //NAMESPACE glib END