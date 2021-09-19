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
		}
	}

	void GuiSprite::render(Image* surf)
	{
		if(surf!=nullptr)
		{
			if(img.getImage(index)!=nullptr)
			{
				Graphics::setColor(imgColor);
				int x1 = renderX;
				int x2 = renderX + img.getImage(index)->getWidth() * xScale;

				int y1 = renderY;
				int y2 = renderY + img.getImage(index)->getHeight() * yScale;
				
				Graphics::drawTexturedTriangle(Vec4f(x1, y1, 0, 0), Vec4f(x2, y1, 1, 0), Vec4f(x2, y2, 1, 1), img.getImage(index), surf);
				Graphics::drawTexturedTriangle(Vec4f(x1, y1, 0, 0), Vec4f(x2, y2, 1, 1), Vec4f(x1, y2, 0, 1), img.getImage(index), surf);
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

	#pragma endregion

} //NAMESPACE glib END