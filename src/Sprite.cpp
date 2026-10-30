#include "Sprite.h"

namespace smpl
{
	
	Sprite::Sprite()
	{
	}

	Sprite::~Sprite()
	{
	}


	void Sprite::hardCopy(const Sprite& o)
	{
		delayTimeForFrame = o.delayTimeForFrame;
		loops = o.loops;

		//hard copy
		for(size_t i=0; i<o.images.size(); i++)
		{
			LockingSmartMemory<Image> lockedPointer = o.images[i].getLockingPointer();
			if(lockedPointer == nullptr)
			{
				images.push_back(nullptr);
			}
			else
			{
				Image* nImg = new Image();
				nImg->copyImage(lockedPointer.getRawPointer());
				images.push_back( nImg );
			}
		}
	}

	void Sprite::clear()
	{
		images.clear();
		delayTimeForFrame.clear();
	}

	SmartMemory<Image> Sprite::getImage(size_t index) const
	{
		if (index < images.size())
			return images[index];
		
		return nullptr;
	}

	int Sprite::getDelayTime(size_t index) const
	{
		if(index < delayTimeForFrame.size())
			return delayTimeForFrame[index];
		return 100;
	}

	void Sprite::setDelayTime(size_t index, int milliSecondsDelay)
	{
		delayTimeForFrame[index] = milliSecondsDelay;
	}

	size_t Sprite::getSize() const
	{
		return images.size();
	}

	void Sprite::addImage(const SmartMemory<Image>& p)
	{
		images.push_back(p);
	}

	void Sprite::removeImage(size_t index)
	{
		if (index < images.size())
		{
			std::vector<SmartMemory<Image>> newImages = std::vector<SmartMemory<Image>>();
			std::vector<int> nDelay = std::vector<int>();

			for (size_t i = 0; i < images.size(); i++)
			{
				if (i != index)
				{
					newImages.push_back(images[i]);
					nDelay.push_back(delayTimeForFrame[i]);
				}
			}

			images = newImages;
			delayTimeForFrame = nDelay;
		}
	}

	bool Sprite::loadImage(const File& file, bool shouldClear)
	{
		if(shouldClear)
			clear();

		int amountOfImages = 0;

		std::vector<int> extraData;
		Image** imgs = Image::loadImage(file, &amountOfImages, &extraData);
		
		if(imgs == nullptr)
			return false;
		
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

		return true;
	}

	
	bool Sprite::loadSpriteSheet(const File& file, int width, int height, int xStride, int yStride, int count, bool shouldClear)
	{
		if(shouldClear)
			clear();

		int amountOfImages = 0;
		Image** imgs = Image::loadImage(file, &amountOfImages);
		
		if(imgs == nullptr)
			return false;
		
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
		
		return true;
	}

	bool Sprite::saveAGIF(const File& f, int paletteSize, bool dither, bool saveAlpha, unsigned char alphaThreshold) const
	{
		return Image::saveAGIF(f, (const Image**)images.data(), images.size(), delayTimeForFrame.data(), loops, paletteSize, dither, saveAlpha, alphaThreshold);
	}

	bool Sprite::saveAPNG(const File& f, bool saveAlpha, bool greyscale, bool strongCompression) const
	{
		return Image::saveAPNG(f, (const Image**)images.data(), images.size(), delayTimeForFrame.data(), loops, saveAlpha, greyscale, strongCompression);
	}

	bool Sprite::shouldLoop() const
	{
		return loops;
	}

	void Sprite::setShouldLoop(bool v)
	{
		loops = v;
	}

} //NAMESPACE glib END