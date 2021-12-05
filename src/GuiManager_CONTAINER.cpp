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
	
	void GuiContainer::loadDataFromXML(std::unordered_map<std::wstring, std::wstring>& attributes)
	{
		GuiInstance::loadDataFromXML(attributes);
	}

	GuiInstance* GuiContainer::loadFunction(std::unordered_map<std::wstring, std::wstring>& attributes)
	{
		GuiContainer* ins = new GuiContainer();
		ins->loadDataFromXML(attributes);

		return ins;
	}

	void GuiContainer::registerLoadFunction()
	{
		GuiManager::registerLoadFunction(L"Container", GuiContainer::loadFunction);
	}

	#pragma endregion

} //NAMESPACE glib END