#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CUSTOM_OBJECT_CLASS

	const Class GuiCustomObject::myClass = Class("GuiCustomObject", {&GuiInstance::myClass});
	const Class* GuiCustomObject::getClass()
	{
		return &GuiCustomObject::myClass;
	}

	GuiCustomObject::GuiCustomObject()
	{
		
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

	#pragma endregion

} //NAMESPACE glib END