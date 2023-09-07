#include "NewGui.h"
#include "Input.h"

namespace glib
{
    GuiButton::GuiButton() : GuiLayoutList()
    {

    }
    
    GuiButton::~GuiButton()
    {

    }
    
    void GuiButton::setOnClickFunction(std::function<void()> onClick)
    {
        onClickFunc = onClick;
    }

    void GuiButton::setOnMouseInFunction(std::function<void()> onMouseIn)
    {
        onMouseInFunc = onMouseIn;
    }

    void GuiButton::setOnMouseOutFunction(std::function<void()> onMouseOut)
    {
        onMouseOutFunc = onMouseOut;
    }

    void GuiButton::addChild(SmartMemory<GuiItem> child)
    {
        //can only have one child.
        //More efficient than GuiLayoutList::removeChild() since we are removing all.
        while(children.size() > 0)
        {
            GuiItem* rawP = children.back().getPointer();
            if(rawP != nullptr)
            {
                rawP->setParent(nullptr);
            }
            children.pop_back();
        }
    
        //does checking for nullptr or self inside this function.
        GuiLayoutList::addChild(child);
    }

    void GuiButton::update(SmartMemory<GuiManager> manager)
    {
        //get mouseX and mouseY
        int mouseX = 0;
        int mouseY = 0;
        if(manager.getPointer() != nullptr)
        {
            mouseX = manager.getRawPointer()->getMouseX();
            mouseY = manager.getRawPointer()->getMouseY();
        }

        if(isColliding(mouseX, mouseY))
        {
            if(hovering == false)
            {
                if(onMouseInFunc != nullptr)
                    onMouseInFunc();
            }
            hovering = true;
        }
        else
        {
            if(hovering == true)
            {
                if(onMouseOutFunc != nullptr)
                    onMouseOutFunc();
            }
            hovering = false;
        }
        
        if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON) && hovering == true)
            depressed = true;
        
        if(Input::getMouseUp(Input::LEFT_MOUSE_BUTTON) && hovering == true && depressed == true)
        {
            if(onClickFunc != nullptr)
                onClickFunc();
        }
        
        //update the children
        GuiLayout::update(manager);
    }

    void GuiButton::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiLayoutList::loadDataFromXML(attribs, manager);
    }

    SmartMemory<GuiItem> GuiButton::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiButton* ins = new GuiButton();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}