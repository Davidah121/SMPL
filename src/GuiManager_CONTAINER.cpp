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
		setShouldRedraw(false);
	}

	void GuiContainer::render()
	{
		
	}
	
	void GuiContainer::loadDataFromXML(std::unordered_map<std::wstring, std::wstring>& attributes, GuiGraphicsInterface* inter)
	{
		GuiInstance::loadDataFromXML(attributes, inter);
	}

	GuiInstance* GuiContainer::loadFunction(std::unordered_map<std::wstring, std::wstring>& attributes, GuiGraphicsInterface* inter)
	{
		GuiContainer* ins = new GuiContainer();
		ins->loadDataFromXML(attributes, inter);

		return ins;
	}

	void GuiContainer::registerLoadFunction()
	{
		GuiManager::registerLoadFunction(L"GuiContainer", GuiContainer::loadFunction);
	}

	#pragma endregion

} //NAMESPACE glib END