#include "NewGui.h"
#include "Input.h"

namespace smpl
{
    GuiScrollBar::GuiScrollBar() : GuiLayoutFixed()
    {
        GuiLayoutFixed::addChild( SmartMemory<GuiItem>::createNoDelete(&horizontalScrollButton, false) );
        GuiLayoutFixed::addChild( SmartMemory<GuiItem>::createNoDelete(&verticalScrollButton, false) );

        verticalScrollButton.setMinWidth(8);
        verticalScrollButton.setMinHeight(32);
        verticalScrollButton.setBackgroundColor({0, 0, 0, 128});
        verticalScrollButton.setHoverColor({64,64,64,128});
        verticalScrollButton.setPressedColor({128,128,128,128});
        verticalScrollButton.setBorderColor({0, 0, 255, 128});
        
        horizontalScrollButton.setMinWidth(32);
        horizontalScrollButton.setMinHeight(8);
        horizontalScrollButton.setBackgroundColor({0, 0, 0, 128});
        horizontalScrollButton.setHoverColor({64,64,64,128});
        horizontalScrollButton.setPressedColor({128,128,128,128});
        horizontalScrollButton.setBorderColor({0, 0, 255, 255});
    }
    GuiScrollBar::~GuiScrollBar()
    {

    }

    void GuiScrollBar::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        //position the vertical scroll bar and the horizontal scroll bar
        GRect vbutMargins = verticalScrollButton.getMargin();
        vbutMargins.left = maximumWidth - verticalScrollButton.width;
        verticalScrollButton.setMargin(vbutMargins);
        
        GRect hbutMargins = horizontalScrollButton.getMargin();
        hbutMargins.top = maximumHeight - horizontalScrollButton.height;
        horizontalScrollButton.setMargin(hbutMargins);
        
        GuiLayoutFixed::layoutUpdate(offX, offY, maximumWidth, maximumHeight);
    }
    
    void GuiScrollBar::addChild(SmartMemory<GuiItem> child)
    {
        //can only have 3 children.
        //2 of those children are for the buttons for the scroll bar.
        //More efficient than GuiLayoutList::removeChild() since we are removing all.
        
        while(children.size() > 2)
        {
            GuiItem* rawP = children.back().getPointer();
            if(rawP != nullptr)
            {
                rawP->setParent(nullptr);
            }
            children.pop_back();
        }
    
        //does checking for nullptr or self inside this function.
        GuiLayoutFixed::addChild(child);
    }

    void GuiScrollBar::update(SmartMemory<GuiManager> manager)
    {
        //Adjust offset x and y. Adjust bounds in x and y
        //need 2 scroll bars probably. Could just add another scroll bar but meh
        int currMaxX = getTrueX() + width;
        int currMaxY = getTrueY() + height;
        int scrollYValue = Input::getMouseScrollVertical();
        int scrollXValue = Input::getMouseScrollHorizontal();
        // if(Input::getKeyDown(Input::KEY_LEFT))
        // {
        //     if(scrollXValue == 0)
        //         scrollXValue = 1;
        // }
        // if(Input::getKeyDown(Input::KEY_RIGHT))
        // {
        //     if(scrollXValue == 0)
        //         scrollXValue = -1;
        // }

        // if(Input::getKeyDown(Input::KEY_UP))
        // {
        //     if(scrollYValue == 0)
        //         scrollYValue = 1;
        // }
        // if(Input::getKeyDown(Input::KEY_DOWN))
        // {
        //     if(scrollYValue == 0)
        //         scrollYValue = -1;
        // }

        if(padding.left < 0)
        {
            //can go back up
            if(scrollXValue>0)
                padding.left += scrollXValue;
        }
        else
        {
            padding.left = 0;
        }

        if(currMaxX > getMaximumWidthAllowedByLayout())
        {
            int v = getMaximumHeightAllowedByLayout() - (currMaxY - padding.top);
            if(scrollXValue>0)
                padding.left += scrollXValue;
            
            if(padding.left <= v)
                padding.left = v;
        }

        if(padding.top < 0)
        {
            //can go back up
            if(scrollYValue>0)
                padding.top += scrollYValue;
        }
        else
        {
            padding.top = 0;
        }

        if(currMaxY > getMaximumHeightAllowedByLayout())
        {
            //can go down
            int v = getMaximumHeightAllowedByLayout() - (currMaxY - padding.top);
            if(scrollYValue<0)
                padding.top += scrollYValue;
            
            if(padding.top <= v)
                padding.top = v;
        }

        //mouse controls of the scroll bar
        int mouseX = 0;
        int mouseY = 0;
        
        if(manager.getPointer() != nullptr)
        {
            mouseX = manager.getRawPointer()->getMouseX();
            mouseY = manager.getRawPointer()->getMouseY();
            
            //show buttons if in focus
            if(getFocused(manager.getRawPointer()))
            {
                verticalScrollButton.setVisible(true);
                horizontalScrollButton.setVisible(true);
            }
            else
            {
                verticalScrollButton.setVisible(false);
                horizontalScrollButton.setVisible(false);
            }
        }
        
        //resize scroll bar buttons and position scroll bar buttons
        if(width)
        
        //update the children
        GuiLayoutFixed::update(manager);
    }
    
    void GuiScrollBar::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiLayoutFixed::loadDataFromXML(attribs, manager);
        // scrollButton.loadDataFromXML(attribs, manager);
    }

    SmartMemory<GuiItem> GuiScrollBar::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiScrollBar* ins = new GuiScrollBar();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }

}