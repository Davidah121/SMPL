#include "InternalGuiManagerHeader.h"

namespace smpl
{
    
	#pragma region GUI_CUSTOM_OBJECT_CLASS

	const Class GuiCustomObject::globalClass = Class("GuiCustomObject", {&GuiInstance::globalClass});

	GuiCustomObject::GuiCustomObject()
	{
		setClass(globalClass);
		includeChildrenInBounds = true;
		boundingBox = GuiInstance::getInvalidBox();
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
			renderFunc();
		}
	}

	void GuiCustomObject::setUpdateFunction(std::function<void()> func)
	{
		updateFunc = func;
	}

	void GuiCustomObject::setRenderFunction(std::function<void()> func)
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

} //NAMESPACE smpl END