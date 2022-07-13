#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CONTEXT_MENU_CLASS

	const Class GuiContextMenu::globalClass = Class("GuiContextMenu", {&GuiInstance::globalClass});

	GuiContextMenu::GuiContextMenu()
	{
		setClass(globalClass);
		GuiInstance::addChild(&listMenu);
		listMenu.setBackgroundColor(Color{255,255,255,255});
		includeChildrenInBounds = true;
		setVisible(false);
	}

	GuiContextMenu::~GuiContextMenu()
	{

	}

	void GuiContextMenu::update()
	{
		int mouseX = Input::getMouseX();
		int mouseY = Input::getMouseY();
		
		if(getManager()!=nullptr)
		{
			mouseX -= getManager()->getWindowX();
			mouseY -= getManager()->getWindowY();
		}

		if(getVisible())
		{
			if(!listMenu.pointIsInList(mouseX, mouseY))
			{
				if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
				{
					hideMenu();
				}
				if(Input::getMousePressed(Input::RIGHT_MOUSE_BUTTON))
				{
					hideMenu();
				}
				if(Input::getMousePressed(Input::MIDDLE_MOUSE_BUTTON))
				{
					hideMenu();
				}
			}
		}
		else
		{
			if(showOnRightClick)
			{
				GuiInstance* parentIns = this->getParent();
				bool shouldOpen = true;
				if(parentIns != nullptr)
				{
					Box2D box = parentIns->getBoundingBox();
					Point2D point = Point2D(Vec2f(mouseX, mouseY));
					shouldOpen = CollisionMaster::getCollision(&box, &point, true);
				}

				if(Input::getMouseUp(Input::RIGHT_MOUSE_BUTTON) && shouldOpen)
				{
					showMenu(mouseX, mouseY);
				}
			}
		}
		setShouldRedraw(false);
	}

	void GuiContextMenu::render()
	{

	}
	
	GuiList* GuiContextMenu::getListElement()
	{
		return &listMenu;
	}

	void GuiContextMenu::showMenu(int x, int y)
	{
		setVisible(true);
		int offX = x - this->x;
		int offY = y - this->y;
		listMenu.setActive(true);
		listMenu.setBaseX(offX);
		listMenu.setBaseY(offY);
	}

	void GuiContextMenu::hideMenu()
	{
		setVisible(false);
		listMenu.setActive(false);
	}

	void GuiContextMenu::addChild(GuiInstance* ins)
	{
		listMenu.addElement(ins);
	}

	void GuiContextMenu::removeChild(GuiInstance* ins)
	{
		listMenu.removeElement(ins);
	}

	void GuiContextMenu::setShowOnRightClick(bool v)
	{
		showOnRightClick = v;
	}

	bool GuiContextMenu::getShowOnRightClick()
	{
		return showOnRightClick;
	}

	void GuiContextMenu::solveBoundingBox()
	{
		//The size is the size of the children
	}

	void GuiContextMenu::loadDataFromXML(std::unordered_map<std::string, std::string>& attribs, GuiGraphicsInterface* inter)
	{
		GuiInstance::loadDataFromXML(attribs, inter);
		listMenu.loadDataFromXML(attribs, inter);
	}

	void GuiContextMenu::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiContextMenu", GuiContextMenu::loadFunction);
	}

	GuiInstance* GuiContextMenu::loadFunction(std::unordered_map<std::string, std::string>& attributes, GuiGraphicsInterface* inter)
	{
		GuiContextMenu* ins = new GuiContextMenu();
		ins->loadDataFromXML(attributes, inter);
		
		return ins;
	}
	
	#pragma endregion

} //NAMESPACE glib END