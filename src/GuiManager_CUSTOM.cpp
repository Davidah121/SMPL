#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CUSTOM_OBJECT_CLASS

	const Class GuiCustomObject::globalClass = Class("GuiCustomObject", {&GuiInstance::globalClass});

	GuiCustomObject::GuiCustomObject()
	{
		setClass(globalClass);
		includeChildrenInBounds = true;
	}

	GuiCustomObject::~GuiCustomObject()
	{
		
	}

	void GuiCustomObject::update()
	{
		setShouldRedraw(true);
		if(updateFunc!=nullptr)
			updateFunc();
	}

	void GuiCustomObject::render()
	{
		if(renderFunc!=nullptr)
		{
			GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
			renderFunc(graphicsInterface);
		}
	}

	void GuiCustomObject::setUpdateFunction(std::function<void()> func)
	{
		updateFunc = func;
	}

	void GuiCustomObject::setRenderFunction(std::function<void(GuiGraphicsInterface*)> func)
	{
		renderFunc = func;
	}

	void GuiCustomObject::setBoundingBoxCalcFunction(std::function<void()> func)
	{
		boundCalcFunc = func;
	}
	
	void GuiCustomObject::solveBoundingBox()
	{
		if(boundCalcFunc != nullptr)
			boundCalcFunc();
	}

	#pragma endregion

} //NAMESPACE glib END