#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_IMAGE_CLASS

	const Class GuiImage::myClass = Class("GuiImage", {&GuiInstance::myClass});
	const Class* GuiImage::getClass()
	{
		return &GuiImage::myClass;
	}

	GuiImage::GuiImage(Image* img)
	{
		this->img = img;
	}

	GuiImage::~GuiImage()
	{
		
	}

	void GuiImage::update()
	{
		boundingBox = Box2D(x, y, x+img->getWidth(), y+img->getHeight());
	}

	void GuiImage::render(Image* surf)
	{
		if(surf!=nullptr)
		{
			if(img!=nullptr)
			{
				SimpleGraphics::drawImage(img, renderX, renderY, surf);
			}
		}
	}

	void GuiImage::setImage(Image* img)
	{
		this->img = img;
		setShouldRedraw(true);
	}

	Image* GuiImage::getImage()
	{
		return img;
	}

	#pragma endregion

} //NAMESPACE glib END