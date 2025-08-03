#include "NewGui.h"
#include "Input.h"

namespace smpl
{
    GuiButton::GuiButton() : GuiLayoutList()
    {
        setFocusable(false);
    }
    
    GuiButton::~GuiButton()
    {

    }
    
    void GuiButton::setOnClickFunction(std::function<void(GuiButton*, int button)> onClick)
    {
        onClickFunc = onClick;
    }
    void GuiButton::setOnClickReleaseFunction(std::function<void(GuiButton*, int button, bool hovering)> onClickReleased)
    {
        onClickReleasedFunc = onClickReleased;
    }

    void GuiButton::setOnMouseInFunction(std::function<void(GuiButton*)> onMouseIn)
    {
        onMouseInFunc = onMouseIn;
    }

    void GuiButton::setOnMouseOutFunction(std::function<void(GuiButton*)> onMouseOut)
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
                    onMouseInFunc(this);
            }
            hovering = true;
        }
        else
        {
            if(hovering == true)
            {
                if(onMouseOutFunc != nullptr)
                    onMouseOutFunc(this);
            }
            hovering = false;
        }
        
        if(hovering)
        {
            int buttonPressed = -1;
            if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
                buttonPressed = Input::LEFT_MOUSE_BUTTON;
            if(Input::getMousePressed(Input::MIDDLE_MOUSE_BUTTON))
                buttonPressed = Input::MIDDLE_MOUSE_BUTTON;
            if(Input::getMousePressed(Input::RIGHT_MOUSE_BUTTON))
                buttonPressed = Input::RIGHT_MOUSE_BUTTON;
            
            if(buttonPressed != -1)
            {
                depressed = true;
                if(onClickFunc != nullptr)
                    onClickFunc(this, Input::LEFT_MOUSE_BUTTON);
            }
        }
        if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON) && hovering == true)
        {
            depressed = true;
            if(onClickFunc != nullptr)
                onClickFunc(this, Input::LEFT_MOUSE_BUTTON);
        }
        if(Input::getMousePressed(Input::RIGHT_MOUSE_BUTTON) && hovering == true)
        {
            depressed = true;
            if(onClickFunc != nullptr)
                onClickFunc(this, Input::RIGHT_MOUSE_BUTTON);
        }
        if(Input::getMousePressed(Input::MIDDLE_MOUSE_BUTTON) && hovering == true)
        {
            depressed = true;
            if(onClickFunc != nullptr)
                onClickFunc(this, Input::MIDDLE_MOUSE_BUTTON);
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

        
        int buttonReleased = -1;
        if(Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
            buttonReleased = Input::LEFT_MOUSE_BUTTON;
        if(Input::getMouseUp(Input::MIDDLE_MOUSE_BUTTON))
            buttonReleased = Input::MIDDLE_MOUSE_BUTTON;
        if(Input::getMouseUp(Input::RIGHT_MOUSE_BUTTON))
            buttonReleased = Input::RIGHT_MOUSE_BUTTON;

        if(buttonReleased != -1)
        {
            if(depressed == true)
            {
                if(onClickReleasedFunc != nullptr)
                    onClickReleasedFunc(this, buttonReleased, hovering);
            }
            
            if(depressed == true)
            {
                depressed = false;
                setBackgroundColor(originalBackgroundColor);
                setShouldRender();
            }
        }
        
        //update the children
        GuiLayoutList::update(manager);
    }

    void GuiButton::setHoverColor(smpl::Color c)
    {
        hoverColor = c;
    }
    void GuiButton::setPressedColor(smpl::Color c)
    {
        depressedColor = c;
    }

    void GuiButton::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiLayoutList::loadDataFromXML(attribs, manager);

        std::pair<std::string, std::string>* hoverColorInfo = attribs.get("hover-color");
        std::pair<std::string, std::string>* pressedColorInfo = attribs.get("pressed-color");

        if(hoverColorInfo != nullptr)
        {
            hoverColor = loadColor(hoverColorInfo->second);
            attribs.remove(hoverColorInfo);
        }
        if(pressedColorInfo != nullptr)
        {
            depressedColor = loadColor(pressedColorInfo->second);
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