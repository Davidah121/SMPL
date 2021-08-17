#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CONTAINER_CLASS

	const Class GuiContainer::myClass = Class("GuiContainer", {&GuiInstance::myClass});
	const Class* GuiContainer::getClass()
	{
		return &GuiContainer::myClass;
	}

	GuiContainer::GuiContainer()
	{
		
	}

	GuiContainer::~GuiContainer()
	{
		
	}

	void GuiContainer::update()
	{
		
	}

	void GuiContainer::render(Image* surf)
	{
		
	}

	#pragma endregion

} //NAMESPACE glib END