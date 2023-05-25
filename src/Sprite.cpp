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
		loops = o.loops;

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
	}

	Image* Sprite::getImage(size_t index)
	{
		if (index < images.size())
		{
			return images[index];
		}
		return nullptr;
	}

	int Sprite::getDelayTime()
	{
		return delayTimeForFrame;
	}

	void Sprite::setDelayTime(int milliSecondsDelay)
	{
		delayTimeForFrame = milliSecondsDelay;
	}

	size_t Sprite::getSize()
	{
		return images.size();
	}

	void Sprite::addImage(Image* p)
	{
		images.push_back(p);
	}

	void Sprite::removeImage(size_t index)
	{
		if (index < images.size())
		{
			std::vector<Image*> newImages = std::vector<Image*>();

			for (size_t i = 0; i < images.size(); i++)
			{
				if (i != index)
				{
					newImages.push_back(images[i]);
				}
				else
					delete images[i];
			}

			images = newImages;
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
			this->delayTimeForFrame = extraData[1];
		}

		for (int i = 0; i < amountOfImages; i++)
		{
			if(extraData.size() == (size_t)amountOfImages+1)
				addImage(imgs[i]);
			else
				addImage(imgs[i]);
		}

	}

	bool Sprite::saveAGIF(File f)
	{
		return Image::saveAGIF(f, images.data(), images.size(), delayTimeForFrame, loops, 256, true, false);
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