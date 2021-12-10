#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_SPRITE_CLASS

	const Class GuiSprite::myClass = Class("GuiSprite", {&GuiInstance::myClass});
	const Class* GuiSprite::getClass()
	{
		return &GuiSprite::myClass;
	}

	GuiSprite::GuiSprite()
	{
		boundingBox = Box2D(0,0,0,0);
	}

	GuiSprite::~GuiSprite()
	{
		
	}

	void GuiSprite::update()
	{
		if(index < img.getSize())
		{
			if(lastUpdateTime == 0)
			{
				lastUpdateTime = System::getCurrentTimeMicro();
			}
			else if(System::getCurrentTimeMicro() - lastUpdateTime >= img.getDelayTime(index))
			{
				lastUpdateTime = System::getCurrentTimeMicro();

				index++;

				if(index >= img.getSize())
				{
					if(img.shouldLoop())
					{
						index = 0;
					}
				}
			}

			boundingBox = Box2D(x, y, x+img.getImage(index)->getWidth(), y+img.getImage(index)->getHeight());
		}
		else
		{
			boundingBox = Box2D(0,0,0,0);
		}
	}

	void GuiSprite::render(Image* surf)
	{
		if(surf!=nullptr)
		{
			if(img.getImage(index)!=nullptr)
			{
				SimpleGraphics::setColor(imgColor);

				int tempWidth = (width > 0) ? width : img.getImage(index)->getWidth();
				int tempHeight = (height > 0) ? height : img.getImage(index)->getHeight();
				
				double nXScale = (tempWidth * xScale) / img.getImage(index)->getWidth();
				double nYScale = (tempHeight * yScale) / img.getImage(index)->getHeight();
				
				if(nXScale == 1 && nYScale == 1)
				{
					SimpleGraphics::drawSprite(img.getImage(index), renderX, renderY, surf);
				}
				else
				{
					int x1 = renderX;
					int x2 = renderX + img.getImage(index)->getWidth() * nXScale;

					int y1 = renderY;
					int y2 = renderY + img.getImage(index)->getHeight() * nYScale;

					SimpleGraphics::drawSprite(img.getImage(index), x1, y1, x2, y2, surf);
				}
			}
		}
	}

	void GuiSprite::setSprite(Sprite img)
	{
		this->img = img;
	}

	Sprite& GuiSprite::getSprite()
	{
		return img;
	}

	void GuiSprite::setXScale(double v)
	{
		xScale = v;
	}
	void GuiSprite::setYScale(double v)
	{
		yScale = v;
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
	}

	void GuiSprite::setHeight(int height)
	{
		this->height = height;
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
	}

	Color GuiSprite::getColor()
	{
		return imgColor;
	}

	void GuiSprite::reset()
	{
		index = 0;
		lastUpdateTime = 0;
	}


	void GuiSprite::loadDataFromXML(std::unordered_map<std::wstring, std::wstring>& attribs)
	{
		GuiInstance::loadDataFromXML(attribs);
		std::vector<std::wstring> possibleNames = { L"src", L"width", L"height", L"xscale", L"yscale", L"color"};

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.find(possibleNames[i]);
			if(it != attribs.end())
			{
				if(it->first == L"src")
				{
					img.loadImage(it->second);
				}
				else if(it->first == L"width")
				{
					width = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == L"height")
				{
					height = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == L"xscale")
				{
					xScale = StringTools::toFloat(it->second);
				}
				else if(it->first == L"yscale")
				{
					yScale = StringTools::toFloat(it->second);
				}
				else if(it->first == L"color")
				{
					imgColor = ColorNameConverter::NameToColor(it->second);
				}

				attribs.erase(possibleNames[i]);
			}
		}

	}

	void GuiSprite::registerLoadFunction()
	{
		GuiManager::registerLoadFunction(L"GuiSprite", GuiSprite::loadFunction);
	}

	GuiInstance* GuiSprite::loadFunction(std::unordered_map<std::wstring, std::wstring>& attributes)
	{
		GuiSprite* ins = new GuiSprite();
		ins->loadDataFromXML(attributes);

		return ins;
	}

	#pragma endregion

} //NAMESPACE glib END