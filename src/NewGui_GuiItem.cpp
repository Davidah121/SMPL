#include "NewGui.h"
#include "Input.h"

namespace smpl
{
    GuiItem::GuiItem()
    {
        setFocusable(false);
    }
    
    GuiItem::~GuiItem()
    {

    }

    void GuiItem::fixPosition()
    {
        if(parent.getPointer() != nullptr)
        {
            trueX = x + parent.getRawPointer()->trueX;
            trueY = y + parent.getRawPointer()->trueY;
        }
        else
        {
            trueX = x;
            trueY = y;
        }
    }

    void GuiItem::resetPosition()
    {
        x = 0;
        y = 0;
        trueX = 0;
        trueY = 0;
        width = 0;
        height = 0;
        // currentLayoutMaxWidth = 0;
        // currentLayoutMaxHeight = 0;
    }

    void GuiItem::preUpdate()
    {
        fixPosition();
        if(this->type == TYPE_LAYOUT)
        {
            ((GuiLayout*)this)->preUpdate();
        }
    }

    void GuiItem::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        x = offX;
        y = offY;
    }

    
    void GuiItem::doLayoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        currentLayoutMaxWidth = maximumWidth;
        currentLayoutMaxHeight = maximumHeight;

        if(getVisible())
            layoutUpdate(offX, offY, maximumWidth, maximumHeight);
        else
            resetPosition();
    }
    
    void GuiItem::doUpdate(SmartMemory<GuiManager> manager)
    {
        int mouseX = 0;
        int mouseY = 0;
        GuiManager* rawGPointer = manager.getPointer();

        fixPosition();
        if(getVisible())
        {
            if(rawGPointer != nullptr && isFocusable)
            {
                //do focused stuff
                if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
                {
                    mouseX = rawGPointer->getMouseX();
                    mouseY = rawGPointer->getMouseY();
                    if(isColliding(mouseX, mouseY))
                    {
                        setFocused(manager, true);
                    }
                    else
                    {
                        setFocused(manager, false);
                    }
                }
                if(oldFocus != getFocused(manager))
                {
                    onFocusChanged(manager, !oldFocus);
                    oldFocus = !oldFocus;
                }
            }
            update(manager);
        }
    }

    void GuiItem::doPreUpdate()
    {
        if(getVisible())
            preUpdate();
    }

    
    void GuiItem::doPreRender(SmartMemory<GuiManager> manager)
    {
        // if(!getVisible())
        //     return;

        doPreRenderOperations(manager);
        updateManagerRenderCounter(manager);

        clearShouldRender();
    }

    void GuiItem::doRender(SmartMemory<GuiManager> manager)
    {
        if(!getVisible())
            return;
        
        render(manager);
    }
    
    void GuiItem::setFinishedRendering()
    {
        shouldReRender = false;
        if(this->type == TYPE_LAYOUT)
        {
            for(SmartMemory<GuiItem>& mem : ((GuiLayout*)this)->children)
            {
                if(mem.getPointer() != nullptr)
                    mem.getRawPointer()->setFinishedRendering();
            }
        }
    }
    
    void GuiItem::clearShouldRender()
    {
        shouldReRender = false;
        if(this->type == TYPE_LAYOUT)
        {
            for(SmartMemory<GuiItem>& mem : ((GuiLayout*)this)->children)
            {
                if(mem.getPointer() != nullptr)
                    mem.getRawPointer()->clearShouldRender();
            }
        }
    }

    void GuiItem::doPreRenderOperationsForChildren(SmartMemory<GuiManager> manager)
    {
        if(this->type == TYPE_LAYOUT)
        {
            for(SmartMemory<GuiItem>& mem : ((GuiLayout*)this)->children)
            {
                if(mem.getPointer() != nullptr)
                    mem.getRawPointer()->doPreRenderOperations(manager);
            }

            //add removed rectangles
            for(GRect& r : ((GuiLayout*)this)->removedChildrenRect)
            {
                manager.getRawPointer()->addNewDrawnArea(r);
            }

            //clear removed rectangles
            ((GuiLayout*)this)->removedChildrenRect.clear();
        }
    }

    void GuiItem::doPreRenderOperations(SmartMemory<GuiManager> manager)
    {
        // if(!getVisible())
        //     return;
        
        GRect currRect = {0,0,0,0};
        if(getVisible())
        {
            fixPosition();
            currRect = {trueX, trueY, trueX+width, trueY+height};
            if(shouldReRender == false)
                shouldReRender = lastKnownRenderRect != currRect;
        }

        LockingSmartMemory<GuiManager> lockedManager = manager.getLockingPointer();
        if(lockedManager != nullptr)
        {
            if(lockedManager->getMustRedraw())
                setShouldRender();

            if(shouldReRender)
            {
                lockedManager->addOldDrawnArea(lastKnownRenderRect);
                if(getVisible())
                    lockedManager->addNewDrawnArea(currRect);
            }
        }

        // if(getVisible())
        doPreRenderOperationsForChildren(manager);
        lastKnownRenderRect = currRect;
    }

    void GuiItem::updateManagerRenderCounter(SmartMemory<GuiManager> manager)
    {
        LockingSmartMemory<GuiManager> lockedManager = manager.getLockingPointer();
        if(getVisible() && lockedManager != nullptr)
        {
            if(shouldReRender)
                lockedManager->updateRenderCounter();
            
            updateManagerRenderCounterForChildren(manager);
        }
    }

    void GuiItem::updateManagerRenderCounterForChildren(SmartMemory<GuiManager> manager)
    {
        if(this->type == TYPE_LAYOUT)
        {
            for(SmartMemory<GuiItem>& mem : ((GuiLayout*)this)->children)
            {
                if(mem.getPointer() != nullptr)
                    mem.getRawPointer()->updateManagerRenderCounter(manager);
            }
        }
    }

    void GuiItem::setShouldRender()
    {
        // StringTools::println("PICK ME");
        shouldReRender = true;
    }
    
    GRect GuiItem::getPreviousRenderRect()
    {
        return lastKnownRenderRect;
    }

    bool GuiItem::isColliding(int px, int py)
    {
        fixPosition();
        if(px >= trueX && px <= trueX + width)
            if(py >= trueY && py <= trueY + height)
                return true;
        return false;
    }

    int GuiItem::getType()
    {
        return type;
    }

    void GuiItem::setParent(SmartMemory<GuiItem> p)
    {
        LockingSmartMemory<GuiItem> lockedParent = p.getLockingPointer();
        if(lockedParent != nullptr)
        {
            lockedParent->onChildRemoved( SmartMemory<GuiItem>::createNoDelete(this) );
        }
        parent = p;
    }

    SmartMemory<GuiItem> GuiItem::getParent()
    {
        return parent;
    }

    void GuiItem::onChildRemoved(SmartMemory<GuiItem> removedChild)
    {
        //nothing
    }

    void GuiItem::onFocusChanged(SmartMemory<GuiManager> manager, bool changedTo)
    {
        //nothing
    }

    bool GuiItem::getFocused(SmartMemory<GuiManager> manager)
    {
        LockingSmartMemory<GuiManager> lockedManager = manager.getLockingPointer();
        if(lockedManager != nullptr)
        {
            return lockedManager->getObjectInFocus( SmartMemory<GuiItem>::createNoDelete(this) );
        }
        return false;
    }

    void GuiItem::setFocused(SmartMemory<GuiManager> manager, bool f)
    {
        LockingSmartMemory<GuiManager> lockedManager = manager.getLockingPointer();
        if(lockedManager != nullptr)
        {
            if(f)
            {
                lockedManager->setObjectInFocus( SmartMemory<GuiItem>::createNoDelete(this) );
            }
            else
            {
                if(getFocused(manager))
                {
                    lockedManager->setObjectInFocus(nullptr);
                }
            }
        }
    }
    
    bool GuiItem::getFocusable()
    {
        return isFocusable;
    }
    void GuiItem::setFocusable(bool b)
    {
        isFocusable = b;
    }

    int GuiItem::getTrueX()
    {
        return trueX;
    }

    int GuiItem::getTrueY()
    {
        return trueY;
    }

    bool GuiItem::getVisible()
    {
        bool parentVisible = true;
        auto parentPointer = parent.getLockingPointer();
        if(parentPointer != nullptr)
            parentVisible = parentPointer->getVisible();
        return visible && parentVisible;
    }
    void GuiItem::setVisible(bool v)
    {
        if(v != visible)
        {
            setShouldRender();
        }
        visible = v;
    }
    
    std::string GuiItem::getNameID()
    {
        return nameID;
    }
    
    uint16_t GuiItem::getMaximumWidthAllowedByLayout()
    {
        return currentLayoutMaxWidth;
    }

    uint16_t GuiItem::getMaximumHeightAllowedByLayout()
    {
        return currentLayoutMaxHeight;
    }

    void GuiItem::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        std::pair<std::string, std::string>* pair = attribs.get("id");
        if(pair != nullptr)
            nameID = pair->second;
        attribs.remove(pair);

        pair = attribs.get("visible");
        if(pair != nullptr)
            setVisible(pair->second == "true");
        attribs.remove(pair);
    }
}