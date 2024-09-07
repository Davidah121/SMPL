#include "Sprite.h"

namespace smpl
{
		
	const RootClass Sprite::globalClass = CREATE_ROOT_CLASS(Sprite, &Object::globalClass);
    const RootClass* Sprite::getClass()
	{
		return &Sprite::globalClass;
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
		copy(o);
	}

	void Sprite::operator=(const Sprite& o)
	{
		copy(o);
	}

	void Sprite::copy(const Sprite& o)
	{
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
		if(index < delayTimeForFrame.size())
			return delayTimeForFrame[index];
		return 100;
	}

	void Sprite::setDelayTime(size_t index, int milliSecondsDelay)
	{
		delayTimeForFrame[index] = milliSecondsDelay;
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

	void Sprite::loadImage(File file, bool clear)
	{
		if(clear)
			dispose();

		int amountOfImages = 0;

		std::vector<int> extraData;
		Image** imgs = Image::loadImage(file, &amountOfImages, &extraData);
		
		if(imgs == nullptr)
			return;
		
		if(extraData.size()>=1)
		{
			this->loops = extraData[0] == 1;
		}

		for (int i = 0; i < amountOfImages; i++)
		{
			addImage(imgs[i]);
			if(extraData.size() == (size_t)amountOfImages+1)
				delayTimeForFrame.push_back(extraData[i]);
			else
				delayTimeForFrame.push_back(100);
		}
		if(imgs != nullptr)
			delete[] imgs;
	}

	
	void Sprite::loadSpriteSheet(File file, int width, int height, int xStride, int yStride, int count, bool clear)
	{
		if(clear)
			dispose();

		int amountOfImages = 0;
		Image** imgs = Image::loadImage(file, &amountOfImages);
		
		if(imgs == nullptr)
			return;
		
		for (int i = 0; i < amountOfImages; i++)
		{
			Color* origPixels = imgs[i]->getPixels();
			int origWidth = imgs[i]->getWidth();
			int origHeight = imgs[i]->getHeight();
			for(int y=0; y<imgs[i]->getHeight(); y+=height + yStride)
			{
				for(int x=0; x<imgs[i]->getWidth(); x+=width + xStride)
				{
					Image* extractedImg = new Image(width, height);
					Color* extractedImgPixels = extractedImg->getPixels();

					int maxWidthAllowed = ((origWidth - (x+width)) >= 0) ? width : (origWidth - (x+width));
					int maxHeightAllowed = ((origHeight - (y + height)) >= 0) ? height : (origHeight - (y+height));
					
					for(int ty=0; ty<maxHeightAllowed; ty++)
					{
						for(int tx=0; tx<maxWidthAllowed; tx++)
						{
							extractedImgPixels[ty*width + tx] = origPixels[ty*origWidth + tx];
						}
					}
					images.push_back(extractedImg);
					delayTimeForFrame.push_back(0);

					if(count > 0 && images.size() >= count)
						break;
				}

				if(count > 0 && images.size() >= count)
					break;
			}

			if(count > 0 && images.size() >= count)
				break;
		}
		
		if(imgs != nullptr)
		{
			for(int i=0; i<amountOfImages; i++)
				delete imgs[i];
			delete[] imgs;
		}
	}

	bool Sprite::saveAGIF(File f, int paletteSize, bool dither, bool saveAlpha, unsigned char alphaThreshold)
	{
		return Image::saveAGIF(f, images.data(), images.size(), delayTimeForFrame.data(), loops, paletteSize, dither, saveAlpha, alphaThreshold);
	}

	bool Sprite::saveAPNG(File f, bool saveAlpha, bool greyscale, bool strongCompression)
	{
		return Image::saveAPNG(f, images.data(), images.size(), delayTimeForFrame.data(), loops, saveAlpha, greyscale, strongCompression);
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