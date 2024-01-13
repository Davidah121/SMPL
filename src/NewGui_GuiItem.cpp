#include "NewGui.h"

namespace glib
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

    void GuiItem::doRender(SmartMemory<GuiManager> manager)
    {
        doPreRenderOperations(manager);
        
        if(shouldReRender)
        {
            if(manager.getPointer() != nullptr)
            {
                manager.getRawPointer()->updateRenderCounter();
            }
            
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
        //nothing here
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

    void GuiItem::determineChangeInOverlap(SmartMemory<GuiManager> manager, GRect& currRect)
    {
        //Get all areas known to be redrawing
        std::vector<GRect> newDrawingRects = manager.getRawPointer()->getNewDrawingRects();

        for(GRect& nRect : newDrawingRects)
        {
            if(nRect.depth <= currRect.depth)
                continue;
            
            //check for overlap. If so, this item should probably be re-rendered
            if(currRect.left <= nRect.right && currRect.right >= nRect.left)
                if(currRect.top <= nRect.bottom && currRect.bottom >= nRect.top)
                    setShouldRender();
            
            //have to do from the other perspective too
            if(nRect.left <= currRect.right && nRect.right >= currRect.left)
                if(nRect.top <= currRect.bottom && nRect.bottom >= currRect.top)
                    setShouldRender();
            
            //actually want to determine if the overlap polygon changed. This is a little too much work
            //so we cheat a little and take the additional performance cost due to additional draws
        }
    }

    void GuiItem::doPreRenderOperations(SmartMemory<GuiManager> manager)
    {
        fixPosition();
        GRect currRect = {trueX, trueY, trueX+width, trueY+height, 0};

        determineChangeFromLastTime();
        if(manager.getPointer() != nullptr)
        {
            //get this objects depth
            uint32_t myDepth = manager.getRawPointer()->getNextDepthValue();
            currRect.depth = myDepth;

            if(shouldReRender)
                manager.getRawPointer()->addNewDrawnArea(currRect);
        }

        doPreRenderOperationsForChildren(manager);
        determineChangeInOverlap(manager, currRect);

        lastKnownRenderRect = currRect;
    }

    void GuiItem::setShouldRender()
    {
        shouldReRender = true;
    }
    
    GRect GuiItem::getPreviousRenderRect()
    {
        return lastKnownRenderRect;
    }

    bool GuiItem::isColliding(int x, int y)
    {
        if(x >= trueX && x <= trueX + width)
            if(y >= trueY && y <= trueY + height)
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
    }
}