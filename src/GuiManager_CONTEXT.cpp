#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CONTEXT_MENU_CLASS

	const Class GuiContextMenu::myClass = Class("GuiContextMenu", {&GuiInstance::myClass});
	const Class* GuiContextMenu::getClass()
	{
		return &GuiContextMenu::myClass;
	}

	GuiContextMenu::GuiContextMenu()
	{
		setPriority( GuiInstance::CONTEXT_MENU_PRIORITY_VALUE );
		addChild( &listMenu );

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

		if(!listMenu.pointIsInList(mouseX, mouseY))
		{
			if(!getVisible())
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

			if(showOnRightClick)
			{
				if(Input::getMouseUp(Input::RIGHT_MOUSE_BUTTON))
				{
					showMenu(mouseX, mouseY);
				}
			}
		}
	}

	void GuiContextMenu::render(Image* surf)
	{

	}
	
	GuiList* GuiContextMenu::getListElement()
	{
		return &listMenu;
	}

	void GuiContextMenu::showMenu(int x, int y)
	{
		setVisible(true);
		listMenu.setActive(true);
		listMenu.setBaseX(x);
		listMenu.setBaseY(y);
	}

	void GuiContextMenu::hideMenu()
	{
		setVisible(false);
		listMenu.setActive(false);
	}

	void GuiContextMenu::setShowOnRightClick(bool v)
	{
		showOnRightClick = v;
	}

	bool GuiContextMenu::getShowOnRightClick()
	{
		return showOnRightClick;
	}
	
	#pragma endregion

} //NAMESPACE glib END