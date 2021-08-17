#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CANVAS_CLASS

	const Class GuiCanvas::myClass = Class("GuiCanvas", {&GuiInstance::myClass});
	const Class* GuiCanvas::getClass()
	{
		return &GuiCanvas::myClass;
	}

	GuiCanvas::GuiCanvas()
	{
	}

	GuiCanvas::GuiCanvas(int width, int height)
	{
		myImage = Image(width, height);
	}

	GuiCanvas::~GuiCanvas()
	{

	}

	void GuiCanvas::update()
	{

		Graphics::setColor(clearColor);
		myImage.clearImage();
	}

	void GuiCanvas::render(Image* surf)
	{
		surf->drawImage(&myImage, renderX, renderY);
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
		ins->setCanvas(&myImage);
		ins->setRenderOffset(&x, &y);
		setPriority( GuiInstance::CANVAS_PRIORITY_VALUE );

		for(GuiInstance* o : ins->getChildren())
		{
			setInstanceCanvas(o);
		}
	}

	#pragma endregion

} //NAMESPACE glib END