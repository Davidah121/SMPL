#include "InternalGuiManagerHeader.h"

namespace smpl
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
		boundingBox = GuiInstance::getInvalidBox();
	}
	
	void GuiContainer::loadDataFromXML(SimpleHashMap<std::string, std::string>& attributes)
	{
		GuiInstance::loadDataFromXML(attributes);
	}

	GuiInstance* GuiContainer::loadFunction(SimpleHashMap<std::string, std::string>& attributes)
	{
		GuiContainer* ins = new GuiContainer();
		ins->loadDataFromXML(attributes);

		return ins;
	}

	void GuiContainer::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiContainer", GuiContainer::loadFunction);
	}

	#pragma endregion

} //NAMESPACE smpl END