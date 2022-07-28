#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CANVAS_CLASS

	const Class GuiCanvas::globalClass = Class("GuiCanvas", {&GuiInstance::globalClass});

	GuiCanvas::GuiCanvas()
	{
		setClass(globalClass);
		boundingBox = Box2D(0, 0, 0, 0);
		includeChildrenInBounds = false;
		setPriority(HIGH_LOW_PRIORITY);
	}

	GuiCanvas::GuiCanvas(int width, int height)
	{
		setClass(globalClass);
		if(this->getManager() != nullptr)
		{
			myImage = this->getManager()->getGraphicsInterface()->createSurface(width, height);
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
			auto graphicsInterface = this->getManager()->getGraphicsInterface();

			if(graphicsInterface != nullptr)
			{
				if(myImage == nullptr)
				{
					//must create image and set the children's canvas
					//delayed creation
					myImage = graphicsInterface->createSurface(boundingBox.getWidth(), boundingBox.getHeight());
					for(GuiInstance* childIns : this->getChildrenRef())
					{
						setInstanceCanvas(childIns, true);
					}
				}

				auto oldSurface = graphicsInterface->getBoundSurface();
				graphicsInterface->setBoundSurface(myImage);
				graphicsInterface->setColor(clearColor);
				graphicsInterface->clear();
				graphicsInterface->setBoundSurface(oldSurface);

				setShouldRedraw(true);
			}
		}
	}

	void GuiCanvas::render()
	{
		if(this->getManager() != nullptr)
		{
			auto graphicsInterface = this->getManager()->getGraphicsInterface();

			if(graphicsInterface != nullptr)
			{
				graphicsInterface->setColor(Color{255,255,255,255});
				graphicsInterface->drawSurface(myImage, x, y);
				graphicsInterface->setColor(Color{0,0,0,255});
				graphicsInterface->drawRect(x, y, x+boundingBox.getWidth(), y+boundingBox.getHeight(), true);
			}
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