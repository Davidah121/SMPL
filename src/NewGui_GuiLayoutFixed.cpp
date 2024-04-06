#include "NewGui.h"

namespace smpl
{
    GuiLayoutFixed::GuiLayoutFixed() : GuiLayout()
    {

    }

    GuiLayoutFixed::~GuiLayoutFixed()
    {

    }

    void GuiLayoutFixed::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        resetPosition();
        //margin = distance from outside content to the edge of this objects border
        
        int actualMaxW, actualMaxH;
        bool processFixed = (flags & FLAG_ABSOLUTE_POSITION) || parent.getPointer() == nullptr;
        
        if(!processFixed)
        {
            x = offX;
            y = offY;

            if(!(flags & FLAG_AUTO_LEFT_MARGIN))
                x += margin.left;
            
            if(!(flags & FLAG_AUTO_TOP_MARGIN))
                y += margin.top;
            
            
            actualMaxW = maximumWidth - margin.left;
            actualMaxH = maximumHeight - margin.top;
            
            if(!(flags & FLAG_AUTO_RIGHT_MARGIN))
                actualMaxW -= margin.right;

            if(!(flags & FLAG_AUTO_BOTTOM_MARGIN))
                actualMaxH -= margin.bottom;
        }
        else
        {
            x = margin.left;
            y = margin.top;
            //assume x and y already set
            if(flags & FLAG_X_PERCENTAGE)
                x = maximumWidth * (double)margin.left/PERCENTAGE_DIVIDE;

            if(flags & FLAG_Y_PERCENTAGE)
                y = maximumHeight * (double)margin.top/PERCENTAGE_DIVIDE;
            
            //margin not included
            actualMaxW = maximumWidth - x;
            actualMaxH = maximumHeight - y;
        }

        //set min width and min height
        if(flags & FLAG_MIN_WIDTH_PERCENTAGE)
            width = __max(actualMaxW * (double)minWidth/PERCENTAGE_DIVIDE, width);
        else
            width = __max(minWidth, width);
        
        if(flags & FLAG_MIN_HEIGHT_PERCENTAGE)
            height = __max(actualMaxH * (double)minHeight/PERCENTAGE_DIVIDE, height);
        else
            height = __max(minHeight, height);

        if(children.size() == 0)
        {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
            return;
        }

        //padding = distance from border to content inside border. Also add in border
        int nOffX = padding.left + border.left;
        int nOffY = padding.top + border.top;

        //The maximum width and height for all content this layout contains.
        int maxContentWidth = actualMaxW - nOffX - padding.right - border.right;
        int maxContentHeight = actualMaxH - nOffY - padding.bottom - border.bottom;

        //The maximum allowed width and height for a child of this layout.
        //Changes per child
        int maxChildWidth = maxContentWidth;
        int maxChildHeight = maxContentHeight;

        int maxXOffsetSeen = 0;
        int maxYOffsetSeen = 0;
        
        
        //calculate child maximum width and height
        bool skipped = false;
        int childIndex = 0;
        int indexOfLastWrap = 0;

        int horizontalMarginAutoCount = 0;
        int verticalMarginAutoCount = 0;
        
        while(childIndex < children.size())
        {
            SmartMemory<GuiItem> _child = children[childIndex];
            GuiItem* child = _child.getPointer();
            if(child == nullptr)
            {
                childIndex++;
                continue;
            }
            
            //Move everything back to 0,0 to be consistent.
            child->resetPosition();
            
            int x2=0;
            int y2=0;
            if(child->getType() == TYPE_EMPTY)
            {
                child->x = nOffX;
                child->y = nOffY;
                x2 = nOffX;
                y2 = nOffY;
            }
            else if(child->getType() == TYPE_CONTENT)
            {
                //just need to get the finalbox. It has no margin, padding, etc.
                ((GuiContent*)child)->doLayoutUpdate(nOffX, nOffY, maxChildWidth, maxChildHeight);
                child->x = nOffX;
                child->y = nOffY;

                x2 = child->x + child->width; //x + width
                y2 = child->y + child->height; //y + height
            }
            else if(child->getType() == TYPE_LAYOUT)
            {
                uint16_t childFlags = ((GuiLayout*)child)->getFlags();
                GRect childMargin = ((GuiLayout*)child)->getMargin();

                //Need to call preprocess on the child
                ((GuiLayout*)child)->setAbsolutePosition(true);
                ((GuiLayout*)child)->doLayoutUpdate(nOffX, nOffY, maxChildWidth, maxChildHeight);
                
                //No need to do any additional processing. Should add the offsets in though
                child->x += nOffX;
                child->y += nOffY;

                x2 = child->x + child->width; //x + width
                y2 = child->y + child->height; //y + height
            }

            if(x2 > width)
            {
                width = x2;
            }
            if(y2 > height)
            {
                height = y2;
            }

            childIndex++;
        }
        
        //Add in padding and border to the right and bottom edges
        width += padding.right + border.right;
        height += padding.bottom + border.bottom;
    }

    void GuiLayoutFixed::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiLayout::loadDataFromXML(attribs, manager);
    }

    SmartMemory<GuiItem> GuiLayoutFixed::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiLayoutFixed* ins = new GuiLayoutFixed();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}