#include "Sprite.h"

const Class Sprite::myClass = Class("Sprite", {&Object::myClass});
const Class* Sprite::getClass()
{
	return &Sprite::myClass;
}

Sprite::Sprite()
{
}


Sprite::~Sprite()
{
	dispose();
}

Sprite::Sprite(const Sprite& o)
{
	ownership = false;
	images = o.images;
}

void Sprite::operator=(const Sprite& o)
{
	ownership = false;
	images = o.images;
}

void Sprite::dispose()
{
	if(ownership)
	{
		for(int i=0; i<images.size(); i++)
		{
			if(images[i]!=nullptr)
			{
				delete images[i];
			}
		}
	}

	images.clear();
}

Image* Sprite::getImage(int index)
{
	if (images.size() > index && index >= 0)
	{
		return images[index];
	}
	return nullptr;
}

int Sprite::getSize()
{
	return images.size();
}

void Sprite::addImage(Image* p)
{
	images.push_back(p);
}

void Sprite::removeImage(int index)
{
	if (index >= 0 && index < images.size())
	{
		std::vector<Image*> newImages = std::vector<Image*>();

		for (int i = 0; i < images.size(); i++)
		{
			if (i != index)
				newImages.push_back(images[i]);
			else
				delete images[i];
		}

		images = newImages;
	}
}

void Sprite::loadImage(std::string filename)
{
	dispose();
	ownership = true;
	int amountOfImages = 0;

	Image** imgs = Image::loadImage(filename, &amountOfImages);

	for (int i = 0; i < amountOfImages; i++)
	{
		addImage(imgs[i]);
	}
}

void Sprite::loadImage(std::wstring filename)
{
	dispose();
	ownership = true;
	int amountOfImages = 0;

	Image** imgs = Image::loadImage(filename, &amountOfImages);

	for (int i = 0; i < amountOfImages; i++)
	{
		addImage(imgs[i]);
	}
}
