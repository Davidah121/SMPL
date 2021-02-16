#include "Sprite.h"

const Class* Sprite::myClass = new Class("Sprite", {Object::myClass});
const Class* Sprite::getClass()
{
	return Sprite::myClass;
}

Sprite::Sprite()
{
}


Sprite::~Sprite()
{
	dispose();
}

void Sprite::dispose()
{
	for(int i=0; i<images.size(); i++)
	{
		if(images[i]!=nullptr)
		{
			delete images[i];
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
	int amountOfImages = 0;

	Image** imgs = Image::loadImage(filename, &amountOfImages);

	for (int i = 0; i < amountOfImages; i++)
	{
		addImage(imgs[i]);
	}
}
