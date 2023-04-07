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

		//check if in resources by name. If not, create a new one as SmartMemory
		spr = GuiResourceManager::getResourceManager().getSprite(f.getFullFileName());

		if(spr.getPointer() == nullptr)
		{
			//create it
			GuiResourceManager::getResourceManager().addSprite( GraphicsInterface::createSprite(f), f.getFullFileName(), false );
			spr = GuiResourceManager::getResourceManager().getSprite(f.getFullFileName());
		}
		
		
		SpriteInterface* tempSpr = spr.getPointer();
		if(tempSpr != nullptr)
		{
			if(tempSpr->getSize() > 0)
				boundingBox = Box2D(x, y, x+tempSpr->getImage(0).getWidth(), y+tempSpr->getImage(0).getHeight());
			else
				boundingBox = GuiInstance::getInvalidBox();
		}
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

		//Should be okay due to smart pointers. As long as it does not have delete rights. It is okay. 
		spr = other.spr;
	}

	GuiSprite::~GuiSprite()
	{
		//No longer needed since SmartMemory is used.
	}

	void GuiSprite::update()
	{
		size_t lastIndex = index;
		SpriteInterface* tempSpr = spr.getPointer();
		if(tempSpr == nullptr)
			return;
		
		if(index < tempSpr->getSize())
		{
			if(lastUpdateTime == 0)
			{
				lastUpdateTime = System::getCurrentTimeMicro();
			}
			else if(System::getCurrentTimeMicro() - lastUpdateTime >= (size_t)tempSpr->getDelayTime(index))
			{
				lastUpdateTime = System::getCurrentTimeMicro();
				
				index++;
				if(index >= tempSpr->getSize())
				{
					if(tempSpr->shouldLoop())
					{
						index = 0;
					}
				}

				if(index != lastIndex)
					setShouldRedraw(true);
			}

			boundingBox = Box2D(x, y, x+tempSpr->getImage(index).getWidth(), y+tempSpr->getImage(index).getHeight());
		}
		else
		{
			boundingBox = GuiInstance::getInvalidBox();
		}
	}

	void GuiSprite::render()
	{
		SpriteInterface* tempSpr = spr.getPointer();
		if(tempSpr == nullptr)
			return;
		
		ImageInterface tempImg = tempSpr->getImage(index);
		if(tempImg.getType() != GraphicsInterface::TYPE_INVALID)
		{
			GraphicsInterface::setColor(imgColor);

			int tempWidth = (width > 0) ? width : tempImg.getWidth();
			int tempHeight = (height > 0) ? height : tempImg.getHeight();
			
			double nXScale = (tempWidth * xScale) / tempImg.getWidth();
			double nYScale = (tempHeight * yScale) / tempImg.getHeight();
			
			if(nXScale == 1 && nYScale == 1)
			{
				GraphicsInterface::drawSprite(&tempImg, x, y);
			}
			else
			{
				int x1 = x;
				int x2 = (int)MathExt::round(x + tempImg.getWidth() * nXScale);

				int y1 = y;
				int y2 = (int)MathExt::round(y + tempImg.getHeight() * nYScale);

				GraphicsInterface::drawSprite(&tempImg, x1, y1, x2, y2);
			}
		}
	
	}

	SpriteInterface* GuiSprite::getSprite()
	{
		SpriteInterface* tempSpr = spr.getPointer();
		return tempSpr;
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
		SpriteInterface* tempSpr = spr.getPointer();
		if(tempSpr != nullptr)
		{
			if(index < tempSpr->getSize())
			{
				ImageInterface temp = tempSpr->getImage(index);
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


	void GuiSprite::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs)
	{
		GuiInstance::loadDataFromXML(attribs);
		std::vector<std::string> possibleNames = { "src", "width", "height", "xscale", "yscale", "color"};

		for(size_t i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.get(possibleNames[i]);
			if(it != nullptr)
			{
				if(it->key == "src")
				{
					//check if in resources by name.
					spr = GuiResourceManager::getResourceManager().getSprite(it->data);
					if(spr.getPointer() == nullptr)
					{
						//create it
						GuiResourceManager::getResourceManager().addSprite( GraphicsInterface::createSprite(it->data), it->data, false );
						spr = GuiResourceManager::getResourceManager().getSprite(it->data);
					}
				}
				else if(it->key == "width")
				{
					width = std::abs(StringTools::toInt(it->data));
				}
				else if(it->key == "height")
				{
					height = std::abs(StringTools::toInt(it->data));
				}
				else if(it->key == "xscale")
				{
					xScale = StringTools::toFloat(it->data);
				}
				else if(it->key == "yscale")
				{
					yScale = StringTools::toFloat(it->data);
				}
				else if(it->key == "color")
				{
					imgColor = ColorNameConverter::NameToColor(it->data);
				}

				attribs.remove(it);
			}
		}

		SpriteInterface* tempSpr = spr.getPointer();

		if(tempSpr->getSprite() != nullptr)
		{
			if(tempSpr->getSize() > 0)
				boundingBox = Box2D(x, y, x+tempSpr->getImage(0).getWidth(), y+tempSpr->getImage(0).getHeight());
		}
	}

	void GuiSprite::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiSprite", GuiSprite::loadFunction);
	}

	GuiInstance* GuiSprite::loadFunction(SimpleHashMap<std::string, std::string>& attributes)
	{
		GuiSprite* ins = new GuiSprite();
		ins->loadDataFromXML(attributes);

		return ins;
	}

	#pragma endregion

} //NAMESPACE glib END