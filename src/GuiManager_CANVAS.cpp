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
		setPriority(LOWER_PRIORITY);
	}

	GuiCanvas::GuiCanvas(int width, int height)
	{
		setClass(globalClass);
		// myImage = Image(width, height);
		boundingBox = Box2D(x, y, x+width, y+height);
		includeChildrenInBounds = false;
		setPriority(LOWER_PRIORITY);
	}

	GuiCanvas::~GuiCanvas()
	{

	}

	void GuiCanvas::update()
	{
		// SimpleGraphics::setColor(clearColor);
		// myImage.clearImage();
		// boundingBox = Box2D(x, y, x+myImage.getWidth(), y+myImage.getHeight());
		// setShouldRedraw(true);
	}

	void GuiCanvas::render()
	{
		// surf->drawImage(&myImage, renderX, renderY);
	}

	void GuiCanvas::setClearColor(Color c)
	{
		clearColor = c;
	}

	Color GuiCanvas::getClearColor()
	{
		return clearColor;
	}

	void GuiCanvas::setInstanceCanvas(GuiInstance* ins)
	{
		// ins->setCanvas(&myImage);
		// ins->setRenderOffset(&x, &y);
		// setPriority( GuiInstance::CANVAS_PRIORITY_VALUE );

		// for(GuiInstance* o : ins->getChildren())
		// {
		// 	setInstanceCanvas(o);
		// }
	}

	
	void GuiCanvas::solveBoundingBox()
	{
		//Due to the properties of the canvas, The children do not affect the bounding box size. It's size is only the image.
		if(myImage != nullptr)
			boundingBox = Box2D(x, y, x+myImage->getWidth(), myImage->getHeight());
	}

	#pragma endregion

} //NAMESPACE glib END