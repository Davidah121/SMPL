#include "InternalGuiManagerHeader.h"

namespace smpl
{
	#pragma region GUI_CONTEXT_MENU_HELPER_CLASS
	const Class GuiContextMenuHelper::globalClass = Class("GuiContextMenuHelper", {&GuiInstance::globalClass});
	GuiContextMenuHelper GuiContextMenuHelper::singletonHelper;

	GuiContextMenuHelper::GuiContextMenuHelper() : GuiInstance()
	{
		setClass(globalClass);
		setVisible(true);
		includeChildrenInBounds = true;
	}

	GuiContextMenuHelper::~GuiContextMenuHelper()
	{
		removeContextMenu();
	}

	void GuiContextMenuHelper::update()
	{
		//Must always be at 0,0
		setBaseX(0);
		setBaseY(0);
	}

	void GuiContextMenuHelper::render()
	{

	}

	GuiContextMenuHelper* GuiContextMenuHelper::getContextMenuHelper(GuiManager* manager)
	{
		if(manager == nullptr)
			return nullptr;
		
		//set the singletonHelper's guiManager if it does not have one
		if(singletonHelper.getManager() == nullptr)
		{
			manager->addElement(&singletonHelper);
			return &singletonHelper;
		}
		if(singletonHelper.getManager() == manager)
			return &singletonHelper;
		
		return nullptr;
	}

	void GuiContextMenuHelper::setContextMenu(GuiContextMenu* o)
	{
		//remove the previous context menu
		removeContextMenu();
		
		heldContextMenu = o;
		this->addChild(heldContextMenu->getListElement());
		setVisible(true);
	}

	void GuiContextMenuHelper::removeContextMenu(GuiContextMenu* o)
	{
		if(heldContextMenu == o)
		{
			this->removeChild(heldContextMenu->getListElement());
			heldContextMenu = nullptr;
			setVisible(false);
		}
	}

	void GuiContextMenuHelper::removeContextMenu()
	{
		if(heldContextMenu != nullptr)
		{
			this->removeChild(heldContextMenu->getListElement());
		}
		heldContextMenu = nullptr;
		setVisible(false);
	}
	
	bool GuiContextMenuHelper::isMenuShowing()
	{
		return heldContextMenu != nullptr;
	}

	void GuiContextMenuHelper::solveBoundingBox()
	{
		//The size is the size of the children
		boundingBox = GuiInstance::getInvalidBox();
	}

	#pragma endregion

	#pragma region GUI_CONTEXT_MENU_CLASS

	const Class GuiContextMenu::globalClass = Class("GuiContextMenu", {&GuiInstance::globalClass});

	GuiContextMenu::GuiContextMenu() : GuiInstance()
	{
		setClass(globalClass);
		listMenu.setBackgroundColor(Color{255,255,255,255});
		includeChildrenInBounds = true;
		listMenu.setVisible(false);
	}

	GuiContextMenu::GuiContextMenu(const GuiContextMenu& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiContextMenu::operator=(const GuiContextMenu& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiContextMenu::copy(const GuiContextMenu& other)
	{
		setClass(globalClass);
		listMenu = other.listMenu;
		includeChildrenInBounds = true;
		showOnRightClick = other.showOnRightClick;
		listMenu.setVisible(false);
	}
	
	GuiContextMenu::~GuiContextMenu()
	{
		GuiContextMenuHelper* helper = GuiContextMenuHelper::getContextMenuHelper( this->getManager() );
		if(helper != nullptr)
			helper->removeContextMenu(this);
	}

	void GuiContextMenu::update()
	{
		int mouseX, mouseY;

		if(getManager()!=nullptr)
		{
			mouseX = getManager()->getMouseX();
			mouseY = getManager()->getMouseY();
		}

		if(listMenu.getVisible())
		{
			if(!listMenu.pointIsInList(mouseX, mouseY) && canHideOnClick)
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
			if(Input::getMouseUp(Input::RIGHT_MOUSE_BUTTON))
			{
				if(getManager() != nullptr)
				{
					if(!getManager()->getFocus())
						return; //Can't open menu unless in focus.
				}

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

					if(shouldOpen)
					{
						showMenu(mouseX, mouseY);
					}
				}
			}
		}
		// setShouldRedraw(false);
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
		listMenu.setVisible(true);
		listMenu.setActive(true);

		listMenu.setBaseX(x);
		listMenu.setBaseY(y);

		GuiContextMenuHelper* helper = GuiContextMenuHelper::getContextMenuHelper( this->getManager() );
		if(helper != nullptr)
			helper->setContextMenu(this);
	}

	void GuiContextMenu::hideMenu()
	{
		listMenu.setVisible(false);
		listMenu.setActive(false);
		GuiContextMenuHelper* helper = GuiContextMenuHelper::getContextMenuHelper( this->getManager() );
		if(helper != nullptr)
			helper->removeContextMenu(this);
	}

	bool GuiContextMenu::isMenuShowing()
	{
		return listMenu.getVisible();
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

	void GuiContextMenu::setCanHideOnClick(bool v)
	{
		canHideOnClick = v;
	}
	
	bool GuiContextMenu::getCanHideOnClick()
	{
		return canHideOnClick;
	}

	void GuiContextMenu::solveBoundingBox()
	{
		//The size is the size of the children
		boundingBox = GuiInstance::getInvalidBox();
	}

	void GuiContextMenu::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs)
	{
		GuiInstance::loadDataFromXML(attribs);
		listMenu.loadDataFromXML(attribs);
	}

	void GuiContextMenu::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiContextMenu", GuiContextMenu::loadFunction);
	}

	GuiInstance* GuiContextMenu::loadFunction(SimpleHashMap<std::string, std::string>& attributes)
	{
		GuiContextMenu* ins = new GuiContextMenu();
		ins->loadDataFromXML(attributes);
		
		return ins;
	}
	
	#pragma endregion

} //NAMESPACE smpl END