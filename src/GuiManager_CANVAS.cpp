#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CANVAS_CLASS

	const Class GuiCanvas::globalClass = Class("GuiCanvas", {&GuiInstance::globalClass});

	GuiCanvas::GuiCanvas()
	{
		setClass(globalClass);
		boundingBox = GuiInstance::getInvalidBox();
		includeChildrenInBounds = false;
		setPriority(HIGH_LOW_PRIORITY);
	}

	GuiCanvas::GuiCanvas(int width, int height)
	{
		setClass(globalClass);
		if(this->getManager() != nullptr)
		{
			myImage = GuiGraphicsInterface::createSurface(width, height);
		}
		boundingBox = Box2D(x, y, x+width, y+height);
		includeChildrenInBounds = false;
		setPriority(HIGH_LOW_PRIORITY);
	}

	GuiCanvas::~GuiCanvas()
	{
		if(myImage != nullptr)
			delete myImage;

		myImage = nullptr;
	}

	void GuiCanvas::update()
	{
		if(this->getManager() != nullptr)
		{
			if(myImage == nullptr)
			{
				//removed delayed creation since the graphics interface is static now and always accessible.
				return;
			}

			auto oldSurface = GuiGraphicsInterface::getBoundSurface();
			GuiGraphicsInterface::setBoundSurface(myImage);
			GuiGraphicsInterface::setColor(clearColor);
			GuiGraphicsInterface::clear();
			GuiGraphicsInterface::setBoundSurface(oldSurface);

			setShouldRedraw(true);
		
		}
	}

	void GuiCanvas::render()
	{
		if(this->getManager() != nullptr)
		{
			GuiGraphicsInterface::setColor(Color{255,255,255,255});
			GuiGraphicsInterface::drawSurface(myImage, x, y);
			GuiGraphicsInterface::setColor(Color{0,0,0,255});
			GuiGraphicsInterface::drawRect(x, y, x+boundingBox.getWidth(), y+boundingBox.getHeight(), true);
		}
	}

	void GuiCanvas::setClearColor(Color c)
	{
		clearColor = c;
	}

	Color GuiCanvas::getClearColor()
	{
		return clearColor;
	}

	void GuiCanvas::addChild(GuiInstance* ins)
	{
		GuiInstance::addChild(ins);
		setInstanceCanvas(ins, true);
	}

	void GuiCanvas::removeChild(GuiInstance* ins)
	{
		GuiInstance::removeChild(ins);
		removeInstanceCanvas(ins);
	}

	void GuiCanvas::setInstanceCanvas(GuiInstance* ins, bool setOffset)
	{
		ins->setCanvas(myImage);
		if(setOffset)
			ins->setRenderOffset(&x, &y);
			
		for(GuiInstance* o : ins->getChildren())
		{
			setInstanceCanvas(o, false);
		}
	}

	void GuiCanvas::removeInstanceCanvas(GuiInstance* ins)
	{
		if(ins->getCanvas() != nullptr && ins->getCanvas() == nullptr)
		{
			ins->setCanvas(nullptr);
			ins->setRenderOffset(nullptr, nullptr);

			for(GuiInstance* o : ins->getChildren())
			{
				removeInstanceCanvas(o);
			}
		}
	}
	
	void GuiCanvas::solveBoundingBox()
	{
		//Due to the properties of the canvas, The children do not affect the bounding box size. It's size is only the image.
		if(myImage != nullptr)
			boundingBox = Box2D(x, y, x+myImage->getWidth(), myImage->getHeight());
	}

	#pragma endregion

} //NAMESPACE glib END