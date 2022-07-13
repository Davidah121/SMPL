#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_SPRITE_CLASS

	const Class GuiSprite::globalClass = Class("GuiSprite", {&GuiInstance::globalClass});

	GuiSprite::GuiSprite()
	{
		setClass(globalClass);
		boundingBox = Box2D(0,0,0,0);
	}

	GuiSprite::GuiSprite(File f)
	{
		setClass(globalClass);
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
		img = graphicsInterface->createSprite(f);
		if(img->getSize() > 0)
			boundingBox = Box2D(x, y, x+img->getImage(0)->getWidth(), y+img->getImage(0)->getHeight());
		else
			boundingBox = Box2D(0,0,0,0);
	}

	GuiSprite::~GuiSprite()
	{
		if(img != nullptr)
			delete img;
		img = nullptr;
	}

	void GuiSprite::update()
	{
		int lastIndex = index;
		if(img == nullptr)
			return;
		
		if(index < img->getSize())
		{
			if(lastUpdateTime == 0)
			{
				lastUpdateTime = System::getCurrentTimeMicro();
			}
			else if(System::getCurrentTimeMicro() - lastUpdateTime >= img->getDelayTime(index))
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

			boundingBox = Box2D(x, y, x+img->getImage(index)->getWidth(), y+img->getImage(index)->getHeight());
		}
		else
		{
			boundingBox = Box2D(0,0,0,0);
		}
	}

	void GuiSprite::render()
	{
		if(img == nullptr)
			return;
		
		if(img->getImage(index)!=nullptr)
		{
			GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();

			graphicsInterface->setColor(imgColor);

			int tempWidth = (width > 0) ? width : img->getImage(index)->getWidth();
			int tempHeight = (height > 0) ? height : img->getImage(index)->getHeight();
			
			double nXScale = (tempWidth * xScale) / img->getImage(index)->getWidth();
			double nYScale = (tempHeight * yScale) / img->getImage(index)->getHeight();
			
			if(nXScale == 1 && nYScale == 1)
			{
				graphicsInterface->drawSprite(img->getImage(index), renderX, renderY);
			}
			else
			{
				int x1 = renderX;
				int x2 = renderX + img->getImage(index)->getWidth() * nXScale;

				int y1 = renderY;
				int y2 = renderY + img->getImage(index)->getHeight() * nYScale;

				graphicsInterface->drawSprite(img->getImage(index), x1, y1, x2, y2);
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
		if(index < img->getSize())
		{
			boundingBox = Box2D(x, y, x+img->getImage(index)->getWidth(), y+img->getImage(index)->getHeight());
		}
		else
		{
			boundingBox = Box2D(0,0,0,0);
		}
	}


	void GuiSprite::loadDataFromXML(std::unordered_map<std::string, std::string>& attribs, GuiGraphicsInterface* inter)
	{
		GuiInstance::loadDataFromXML(attribs, inter);
		std::vector<std::string> possibleNames = { "src", "width", "height", "xscale", "yscale", "color"};

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.find(possibleNames[i]);
			if(it != attribs.end())
			{
				if(it->first == "src")
				{
					img = inter->createSprite(it->second);
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
				boundingBox = Box2D(x, y, x+img->getImage(index)->getWidth(), y+img->getImage(index)->getHeight());
		}
	}

	void GuiSprite::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiSprite", GuiSprite::loadFunction);
	}

	GuiInstance* GuiSprite::loadFunction(std::unordered_map<std::string, std::string>& attributes, GuiGraphicsInterface* inter)
	{
		GuiSprite* ins = new GuiSprite();
		ins->loadDataFromXML(attributes, inter);

		return ins;
	}

	#pragma endregion

} //NAMESPACE glib END