#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_SPRITE_CLASS

	const Class GuiSprite::globalClass = Class("GuiSprite", {&GuiInstance::globalClass});

	GuiSprite::GuiSprite() : GuiInstance()
	{
		setClass(globalClass);
		boundingBox = GuiInstance::getInvalidBox();
	}

	GuiSprite::GuiSprite(File f) : GuiInstance()
	{
		setClass(globalClass);
		img = GuiGraphicsInterface::createSprite(f);
		if(img->getSize() > 0)
			boundingBox = Box2D(x, y, x+img->getImage(0).getWidth(), y+img->getImage(0).getHeight());
		else
			boundingBox = GuiInstance::getInvalidBox();
	}
	
	GuiSprite::GuiSprite(const GuiSprite& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiSprite::operator=(const GuiSprite& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiSprite::copy(const GuiSprite& other)
	{
		lastUpdateTime = other.lastUpdateTime;
		index = other.index;
		xScale = other.xScale;
		yScale = other.yScale;
		width = other.width;
		height = other.height;
		imgColor = other.imgColor;

		img = new GuiSpriteInterface( *other.img );
	}

	GuiSprite::~GuiSprite()
	{
		if(img != nullptr)
			delete img;
		img = nullptr;
	}

	void GuiSprite::update()
	{
		size_t lastIndex = index;
		if(img == nullptr)
			return;
		
		if(index < img->getSize())
		{
			if(lastUpdateTime == 0)
			{
				lastUpdateTime = System::getCurrentTimeMicro();
			}
			else if(System::getCurrentTimeMicro() - lastUpdateTime >= (size_t)img->getDelayTime(index))
			{
				lastUpdateTime = System::getCurrentTimeMicro();
				
				index++;
				if(index >= img->getSize())
				{
					if(img->shouldLoop())
					{
						index = 0;
					}
				}

				if(index != lastIndex)
					setShouldRedraw(true);
			}

			boundingBox = Box2D(x, y, x+img->getImage(index).getWidth(), y+img->getImage(index).getHeight());
		}
		else
		{
			boundingBox = GuiInstance::getInvalidBox();
		}
	}

	void GuiSprite::render()
	{
		if(img == nullptr)
			return;
		
		GuiImageInterface tempImg = img->getImage(index);
		if(tempImg.getType() != GuiGraphicsInterface::TYPE_INVALID)
		{
			GuiGraphicsInterface::setColor(imgColor);

			int tempWidth = (width > 0) ? width : tempImg.getWidth();
			int tempHeight = (height > 0) ? height : tempImg.getHeight();
			
			double nXScale = (tempWidth * xScale) / tempImg.getWidth();
			double nYScale = (tempHeight * yScale) / tempImg.getHeight();
			
			if(nXScale == 1 && nYScale == 1)
			{
				GuiGraphicsInterface::drawSprite(&tempImg, x, y);
			}
			else
			{
				int x1 = x;
				int x2 = (int)MathExt::round(x + tempImg.getWidth() * nXScale);

				int y1 = y;
				int y2 = (int)MathExt::round(y + tempImg.getHeight() * nYScale);

				GuiGraphicsInterface::drawSprite(&tempImg, x1, y1, x2, y2);
			}
		}
	
	}

	GuiSpriteInterface* GuiSprite::getSprite()
	{
		return img;
	}

	void GuiSprite::setXScale(double v)
	{
		xScale = v;
		setShouldRedraw(true);
	}

	void GuiSprite::setYScale(double v)
	{
		yScale = v;
		setShouldRedraw(true);
	}

	double GuiSprite::getXScale()
	{
		return xScale;
	}
	double GuiSprite::getYScale()
	{
		return yScale;
	}

	void GuiSprite::setWidth(int width)
	{
		this->width = width;
		setShouldRedraw(true);
	}

	void GuiSprite::setHeight(int height)
	{
		this->height = height;
		setShouldRedraw(true);
	}

	int GuiSprite::getWidth()
	{
		return width;
	}

	int GuiSprite::getHeight()
	{
		return height;
	}

	void GuiSprite::setColor(Color c)
	{
		imgColor = c;
		setShouldRedraw(true);
	}

	Color GuiSprite::getColor()
	{
		return imgColor;
	}

	void GuiSprite::reset()
	{
		index = 0;
		lastUpdateTime = 0;
		setShouldRedraw(true);
	}

	void GuiSprite::solveBoundingBox()
	{
		if(img != nullptr)
		{
			if(index < img->getSize())
			{
				GuiImageInterface temp = img->getImage(index);
				boundingBox = Box2D(x, y, x+temp.getWidth(), y+temp.getHeight());
			}
			else
			{
				boundingBox = GuiInstance::getInvalidBox();
			}
		}
		else
		{
			boundingBox = GuiInstance::getInvalidBox();
		}
	}


	void GuiSprite::loadDataFromXML(std::unordered_map<std::string, std::string>& attribs)
	{
		GuiInstance::loadDataFromXML(attribs);
		std::vector<std::string> possibleNames = { "src", "width", "height", "xscale", "yscale", "color"};

		for(size_t i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.find(possibleNames[i]);
			if(it != attribs.end())
			{
				if(it->first == "src")
				{
					img = GuiGraphicsInterface::createSprite(it->second);
				}
				else if(it->first == "width")
				{
					width = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == "height")
				{
					height = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == "xscale")
				{
					xScale = StringTools::toFloat(it->second);
				}
				else if(it->first == "yscale")
				{
					yScale = StringTools::toFloat(it->second);
				}
				else if(it->first == "color")
				{
					imgColor = ColorNameConverter::NameToColor(it->second);
				}

				attribs.erase(possibleNames[i]);
			}
		}

		if(img != nullptr)
		{
			if(img->getSize() > 0)
				boundingBox = Box2D(x, y, x+img->getImage(0).getWidth(), y+img->getImage(0).getHeight());
		}
	}

	void GuiSprite::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiSprite", GuiSprite::loadFunction);
	}

	GuiInstance* GuiSprite::loadFunction(std::unordered_map<std::string, std::string>& attributes)
	{
		GuiSprite* ins = new GuiSprite();
		ins->loadDataFromXML(attributes);

		return ins;
	}

	#pragma endregion

} //NAMESPACE glib END