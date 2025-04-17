#include "NewGui.h"
#include "Input.h"

namespace smpl
{
    GuiScrollBar::GuiScrollBar() : GuiLayoutFixed()
    {
        GuiLayoutFixed::addChild( SmartMemory<GuiItem>::createNoDelete(&horizontalScrollButton, false) );
        GuiLayoutFixed::addChild( SmartMemory<GuiItem>::createNoDelete(&verticalScrollButton, false) );
        GuiLayoutFixed::addChild( SmartMemory<GuiItem>::createNoDelete(&clippingLayout, false) );

        verticalScrollButton.setAbsolutePosition(true);
        verticalScrollButton.setMinWidth(8);
        verticalScrollButton.setMinHeight(32);
        verticalScrollButton.setBackgroundColor({0, 0, 0, 64});
        verticalScrollButton.setHoverColor({64,64,64,128});
        verticalScrollButton.setPressedColor({128,128,128,128});
        verticalScrollButton.setBorderColor({0, 0, 255, 128});
        
        
        horizontalScrollButton.setAbsolutePosition(true);
        horizontalScrollButton.setMinWidth(32);
        horizontalScrollButton.setMinHeight(8);
        horizontalScrollButton.setBackgroundColor({0, 0, 0, 64});
        horizontalScrollButton.setHoverColor({64,64,64,128});
        horizontalScrollButton.setPressedColor({128,128,128,128});
        horizontalScrollButton.setBorderColor({0, 0, 255, 255});

        verticalScrollButton.setOnClickFunction([this](GuiButton* buttonP, int button) ->void{
            if(button == Input::LEFT_MOUSE_BUTTON)
            {
                this->selectedButton = SELECTED_VERTICAL_BAR;
                this->selectedMouseX = Input::getMouseX();
                this->selectedMouseY = Input::getMouseY();
            }
        });
        
        verticalScrollButton.setOnClickReleaseFunction([this](GuiButton* buttonP, int button, bool hovering) ->void{
            if(button == Input::LEFT_MOUSE_BUTTON)
                selectedButton = SELECTED_INVALID;
        });
        
        horizontalScrollButton.setOnClickFunction([this](GuiButton* buttonP, int button) ->void{
            if(button == Input::LEFT_MOUSE_BUTTON)
            {
                this->selectedButton = SELECTED_HORIZONTAL_BAR;
                this->selectedMouseX = Input::getMouseX();
                this->selectedMouseY = Input::getMouseY();
            }
        });
        
        horizontalScrollButton.setOnClickReleaseFunction([this](GuiButton* buttonP, int button, bool hovering) ->void{
            if(button == Input::LEFT_MOUSE_BUTTON)
                selectedButton = SELECTED_INVALID;
        });
    }
    GuiScrollBar::~GuiScrollBar()
    {

    }

    void GuiScrollBar::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        //position the vertical scroll bar and the horizontal scroll bar
        GRect vbutMargins = verticalScrollButton.getMargin();
        vbutMargins.left = __min(maxWidth, maximumWidth) - verticalScrollButton.width;
        vbutMargins.top = -clippingLayout.getPadding().top;
        verticalScrollButton.setMargin(vbutMargins);
        
        GRect hbutMargins = horizontalScrollButton.getMargin();
        hbutMargins.top = __min(maxHeight, maximumHeight) - horizontalScrollButton.height;
        vbutMargins.left = -clippingLayout.getPadding().left;
        horizontalScrollButton.setMargin(hbutMargins);
        
        GuiLayoutFixed::layoutUpdate(offX, offY, maximumWidth, maximumHeight);
    }
    
    void GuiScrollBar::addChild(SmartMemory<GuiItem> child)
    {
        clippingLayout.addChild(child);
    }
    void GuiScrollBar::removeChild(SmartMemory<GuiItem> child)
    {
        clippingLayout.removeChild(child);
    }

    void GuiScrollBar::update(SmartMemory<GuiManager> manager)
    {
        //Adjust offset x and y. Adjust bounds in x and y
        //need 2 scroll bars probably. Could just add another scroll bar but meh
        
        int currMaxX = getTrueX() + width;
        int currMaxY = getTrueY() + height;
        int scrollYValue = Input::getMouseScrollVertical();
        int scrollXValue = Input::getMouseScrollHorizontal();
        
        if(manager.getPointer() != nullptr)
        {
            //show buttons if in focus
            if(getFocused(manager.getRawPointer()))
            {
                verticalScrollButton.setVisible(true);
                horizontalScrollButton.setVisible(true);
                
                if(Input::getKeyDown(Input::KEY_LEFT))
                {
                    if(scrollXValue == 0)
                        scrollXValue = 1;
                }
                if(Input::getKeyDown(Input::KEY_RIGHT))
                {
                    if(scrollXValue == 0)
                        scrollXValue = -1;
                }
        
                if(Input::getKeyDown(Input::KEY_UP))
                {
                    if(scrollYValue == 0)
                        scrollYValue = 1;
                }
                if(Input::getKeyDown(Input::KEY_DOWN))
                {
                    if(scrollYValue == 0)
                        scrollYValue = -1;
                }
            }
            else
            {
                verticalScrollButton.setVisible(false);
                horizontalScrollButton.setVisible(false);
            }
        }

        //mouse controls of the scroll bar
        //note that the positions given are relative to the screen and not the window which won't matter
        int mouseX = Input::getMouseX();
        int mouseY = Input::getMouseY();

        int mouseXDelta = selectedMouseX - mouseX; //change in position
        int mouseYDelta = selectedMouseY - mouseY; //change in position

        if(selectedButton == SELECTED_VERTICAL_BAR) //vertical
            scrollYValue = mouseYDelta;
        else if(selectedButton == SELECTED_HORIZONTAL_BAR)
            scrollXValue = mouseXDelta;
        
        selectedMouseY = mouseY;
        selectedMouseX = mouseX;

        GRect clipPadding = clippingLayout.getPadding();
        int xOverflow = (clippingLayout.getContentWidth() - width + border.right + border.left);
        int yOverflow = (clippingLayout.getContentHeight() - height + border.bottom + border.top);
        int xUnderflow = clipPadding.left;
        int yUnderflow = clipPadding.top;
        

        if(xUnderflow < 0)
        {
            //can go back left
            if(scrollXValue>0)
                clipPadding.left += __min(scrollXValue, abs(xUnderflow));
        }

        if(xOverflow > 0)
        {
            if(scrollXValue<0)
                clipPadding.left -= __min(abs(scrollXValue), xOverflow);
        }

        if(yUnderflow < 0)
        {
            //can go back up
            if(scrollYValue>0)
                clipPadding.top += __min(scrollYValue, abs(yUnderflow));
        }

        if(yOverflow > 0)
        {
            if(scrollYValue<0)
                clipPadding.top -= __min(abs(scrollYValue), yOverflow);
        }
        
        //resize scroll bar buttons and position scroll bar buttons
        int verticalAllowance = yOverflow - yUnderflow;
        int horizontalAllowance = xOverflow - xUnderflow;

        int finalHeightOfScrollBar = maxHeight - __max(verticalAllowance, 0);
        int finalWidthOfScrollBar = maxWidth - __max(horizontalAllowance, 0);
        
        verticalScrollButton.setMinHeight(finalHeightOfScrollBar);
        horizontalScrollButton.setMinWidth(finalWidthOfScrollBar);
        
        verticalScrollButton.setVisible(finalHeightOfScrollBar!=maxHeight);
        horizontalScrollButton.setVisible(finalWidthOfScrollBar!=maxWidth);

        clippingLayout.setPadding(clipPadding);
        
        //update the children
        GuiLayoutFixed::update(manager);
    }
    
    void GuiScrollBar::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiLayoutFixed::loadDataFromXML(attribs, manager);

        clippingLayout.setMargin(margin);
        clippingLayout.setMinWidth(minWidth);
        clippingLayout.setMaxWidth(maxWidth);
        clippingLayout.setMinHeight(minHeight);
        clippingLayout.setMaxHeight(maxHeight);
        clippingLayout.setFlags(flags);
    }

    SmartMemory<GuiItem> GuiScrollBar::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiScrollBar* ins = new GuiScrollBar();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }

}