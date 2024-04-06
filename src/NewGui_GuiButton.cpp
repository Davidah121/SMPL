#include "NewGui.h"
#include "Input.h"

namespace smpl
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
        if(!hovering && !depressed)
            originalBackgroundColor = getBackgroundColor();
        
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
        {
            depressed = true;
        }
        

        if(depressed)
        {
            Color currColor = getBackgroundColor();
            if(memcmp(&currColor, &depressedColor, sizeof(Color)) != 0)
            {
                setBackgroundColor(depressedColor);
                setShouldRender();
            }
        }
        else if(hovering)
        {
            Color currColor = getBackgroundColor();
            if(memcmp(&currColor, &hoverColor, sizeof(Color)) != 0)
            {
                setBackgroundColor(hoverColor);
                setShouldRender();
            }
        }
        else
        {
            Color currColor = getBackgroundColor();
            if(memcmp(&currColor, &originalBackgroundColor, sizeof(Color)) != 0)
            {
                setBackgroundColor(originalBackgroundColor);
                setShouldRender();
            }
        }

        if(Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
        {
            if(hovering == true && depressed == true)
            {
                if(onClickFunc != nullptr)
                    onClickFunc();
            }
            
            if(depressed == true)
            {
                depressed = false;
                setBackgroundColor(originalBackgroundColor);
                setShouldRender();
            }
        }
        
        //update the children
        GuiLayout::update(manager);
    }

    void GuiButton::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiLayoutList::loadDataFromXML(attribs, manager);

        HashPair<std::string, std::string>* hoverColorInfo = attribs.get("hover-color");
        HashPair<std::string, std::string>* pressedColorInfo = attribs.get("pressed-color");

        if(hoverColorInfo != nullptr)
        {
            hoverColor = loadColor(hoverColorInfo->data);
            attribs.remove(hoverColorInfo);
        }
        if(pressedColorInfo != nullptr)
        {
            depressedColor = loadColor(pressedColorInfo->data);
            attribs.remove(pressedColorInfo);
        }
    }

    SmartMemory<GuiItem> GuiButton::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiButton* ins = new GuiButton();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}