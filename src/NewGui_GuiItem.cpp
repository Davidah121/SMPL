#include "NewGui.h"

namespace smpl
{
    GuiItem::GuiItem()
    {

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
        if(getVisible())
            layoutUpdate(offX, offY, maximumWidth, maximumHeight);
        else
            resetPosition();
    }
    
    void GuiItem::doUpdate(SmartMemory<GuiManager> manager)
    {
        fixPosition();
        if(getVisible())
            update(manager);
    }

    void GuiItem::doPreUpdate()
    {
        if(getVisible())
            preUpdate();
    }

    
    void GuiItem::doPreRender(SmartMemory<GuiManager> manager)
    {
        if(!getVisible())
            return;
        
        doPreRenderOperations(manager);
        determineChangeInOverlap(manager);
        updateManagerRenderCounter(manager);
    }

    void GuiItem::doRender(SmartMemory<GuiManager> manager)
    {
        if(!getVisible())
            return;
        
        if(getShouldReRender())
        {
            GRect newDrawnArea = {0, 0, 65535, 65535};
            if(manager.getPointer() != nullptr)
            {
                newDrawnArea = manager.getRawPointer()->getNewDrawnArea();
            }
            
            smpl::GraphicsInterface::setClippingRect(Box2D(newDrawnArea.left, newDrawnArea.top, newDrawnArea.right, newDrawnArea.bottom));
            render(manager);
            shouldReRender = false;
        }

    }

    void GuiItem::determineChangeFromLastTime()
    {
        //check if old render rect is the same as the current one
        if(lastKnownRenderRect.left != trueX)
            setShouldRender();
        if(lastKnownRenderRect.right != trueX+width)
            setShouldRender();
        if(lastKnownRenderRect.top != trueY)
            setShouldRender();
        if(lastKnownRenderRect.bottom != trueY+height)
            setShouldRender();
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

    void GuiItem::determineChangeInOverlap(SmartMemory<GuiManager> manager)
    {
        if(!getVisible())
            return;

        //Get all area known to be redrawing
        GRect newlyDrawnArea = manager.getRawPointer()->getNewDrawnArea();
        if(newlyDrawnArea.left == newlyDrawnArea.right || newlyDrawnArea.top == newlyDrawnArea.bottom) //invalid box.
            return;
        
        //check for overlap. If so, this item should probably be re-rendered
        if(lastKnownRenderRect.left <= newlyDrawnArea.right && lastKnownRenderRect.right >= newlyDrawnArea.left)
            if(lastKnownRenderRect.top <= newlyDrawnArea.bottom && lastKnownRenderRect.bottom >= newlyDrawnArea.top)
                setShouldRender();
        
        if(newlyDrawnArea.left <= lastKnownRenderRect.right && newlyDrawnArea.right >= lastKnownRenderRect.left)
            if(newlyDrawnArea.top <= lastKnownRenderRect.bottom && newlyDrawnArea.bottom >= lastKnownRenderRect.top)
                setShouldRender();
        
        determineChangeInOverlapForChildren(manager);
    }

    void GuiItem::determineChangeInOverlapForChildren(SmartMemory<GuiManager> manager)
    {
        if(this->type == TYPE_LAYOUT)
        {
            for(SmartMemory<GuiItem>& mem : ((GuiLayout*)this)->children)
            {
                if(mem.getPointer() != nullptr)
                    mem.getRawPointer()->determineChangeInOverlap(manager);
            }
        }
    }

    void GuiItem::doPreRenderOperations(SmartMemory<GuiManager> manager)
    {
        if(!getVisible())
            return;
        
        fixPosition();
        GRect currRect = {trueX, trueY, trueX+width, trueY+height, 0};

        determineChangeFromLastTime();
        if(manager.getPointer() != nullptr)
        {
            //get this objects depth
            uint32_t myDepth = manager.getRawPointer()->getNextDepthValue();
            currRect.depth = myDepth;

            if(manager.getRawPointer()->getMustRedraw())
                setShouldRender();

            if(getShouldReRender())
                manager.getRawPointer()->addNewDrawnArea(currRect);
        }

        doPreRenderOperationsForChildren(manager);

        lastKnownRenderRect = currRect;
    }

    void GuiItem::updateManagerRenderCounter(SmartMemory<GuiManager> manager)
    {
        if(getShouldReRender())
        {
            if(manager.getPointer() != nullptr)
            {
                manager.getRawPointer()->updateRenderCounter();
                updateManagerRenderCounterForChildren(manager);
            }
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
        shouldReRender = true;
    }

    bool GuiItem::getShouldReRender()
    {
        return shouldReRender && getVisible();
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
        if(parent.getPointer() != nullptr)
        {
            parent.getRawPointer()->onChildRemoved( SmartMemory<GuiItem>::createNoDelete(this) );
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

    bool GuiItem::getFocused(SmartMemory<GuiManager> manager)
    {
        if(manager.getPointer() != nullptr)
        {
            return manager.getRawPointer()->getObjectInFocus( SmartMemory<GuiItem>::createNoDelete(this) );
        }
        return false;
    }

    void GuiItem::setFocused(SmartMemory<GuiManager> manager, bool f)
    {
        if(manager.getPointer() != nullptr)
        {
            if(f)
            {
                manager.getRawPointer()->setObjectInFocus( SmartMemory<GuiItem>::createNoDelete(this) );
            }
            else
            {
                if(getFocused(manager))
                {
                    manager.getRawPointer()->setObjectInFocus(nullptr);
                }
            }
        }
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
        return visible;
    }
    void GuiItem::setVisible(bool v)
    {
        if(v == true && visible == false)
        {
            setShouldRender();
        }
        visible = v;
    }
    
    std::string GuiItem::getNameID()
    {
        return nameID;
    }

    void GuiItem::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        HashPair<std::string, std::string>* pair = attribs.get("id");
        if(pair != nullptr)
            nameID = pair->data;
        attribs.remove(pair);

        pair = attribs.get("visible");
        if(pair != nullptr)
            setVisible(pair->data == "true");
        attribs.remove(pair);
    }
}