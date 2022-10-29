#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CONTAINER_CLASS

	const Class GuiContainer::globalClass = Class("GuiContainer", {&GuiInstance::globalClass});

	GuiContainer::GuiContainer()
	{
		setClass(globalClass);
		includeChildrenInBounds = true;
		setPriority(HIGHER_PRIORITY);
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

	void GuiContainer::solveBoundingBox()
	{
		//Do nothing as the grouping of all of the children's boxes is handled by the instance itself
		boundingBox = Box2D(0x7FFFFFFF, 0x7FFFFFFF, 0, 0);
	}
	
	void GuiContainer::loadDataFromXML(std::unordered_map<std::string, std::string>& attributes, GuiGraphicsInterface* inter)
	{
		GuiInstance::loadDataFromXML(attributes, inter);
	}

	GuiInstance* GuiContainer::loadFunction(std::unordered_map<std::string, std::string>& attributes, GuiGraphicsInterface* inter)
	{
		GuiContainer* ins = new GuiContainer();
		ins->loadDataFromXML(attributes, inter);

		return ins;
	}

	void GuiContainer::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiContainer", GuiContainer::loadFunction);
	}

	#pragma endregion

} //NAMESPACE glib END