#include "NewGui.h"

namespace smpl
{
    GuiLayoutList::GuiLayoutList(bool direction, bool wrap) : GuiLayout()
    {
        setFocusable(false);
        this->direction = direction;
        this->wrap = wrap;
    }
    GuiLayoutList::~GuiLayoutList()
    {
        
    }

    void GuiLayoutList::setDirection(bool d)
    {
        direction = d;
    }
    void GuiLayoutList::setWrap(bool w)
    {
        wrap = w;
    }

    void GuiLayoutList::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        resetPosition();

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
            // x = 0;
            // y = 0;
            // width = 0;
            // height = 0;
            return;
        }

        //padding = distance from border to content inside border. Also add in border
        int nOffX = padding.left + border.left;
        int nOffY = padding.top + border.top;

        //These maximum values must actually be set to be considered.
        if(maxHeight > 0)
            actualMaxH = __min(maxHeight, actualMaxH);
        if(maxWidth > 0)
            actualMaxW = __min(maxWidth, actualMaxW);

        //The maximum width and height for all content this layout contains.
        int maxContentWidth = actualMaxW - nOffX - padding.right - border.right;
        int maxContentHeight = actualMaxH - nOffY - padding.bottom - border.bottom;

        // StringTools::println("%d - %d", maxContentWidth, actualMaxW);

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

                //Sum up the number of auto flags for later
                if(childFlags & FLAG_AUTO_LEFT_MARGIN)
                    horizontalMarginAutoCount++;
                if(childFlags & FLAG_AUTO_RIGHT_MARGIN)
                    horizontalMarginAutoCount++;
                
                if(childFlags & FLAG_AUTO_TOP_MARGIN)
                    verticalMarginAutoCount++;
                if(childFlags & FLAG_AUTO_BOTTOM_MARGIN)
                    verticalMarginAutoCount++;

                //Need to call preprocess on the child
                ((GuiLayout*)child)->doLayoutUpdate(nOffX, nOffY, maxChildWidth, maxChildHeight);
                x2 = child->x + child->width; //x + width
                y2 = child->y + child->height; //y + height

                //Factor in margin. (IFF not auto)
                if(!(childFlags & FLAG_AUTO_RIGHT_MARGIN))
                    x2 += childMargin.right;
                if(!(childFlags & FLAG_AUTO_BOTTOM_MARGIN))
                    y2 += childMargin.bottom;
            }
                
            //extend box if necessary and does not violate the maximum width / height
            if(wrap && !skipped)
            {
                //Check if greater than maximum allowed.
                //If so, don't extend size, reset x and y offsets and recalculate.
                //Ignore second violation for this instance and treat like normal.
                if(direction == DIRECTION_VERTICAL)
                {
                    if(y2 > maxContentHeight && maxXOffsetSeen < maxContentWidth)
                    {
                        nOffY = padding.top + border.top;
                        nOffX = maxXOffsetSeen;
                        maxChildHeight = maxContentHeight;
                        skipped = true;
                        
                        int autoMarginWidth = 0;
                        int autoMarginHeight = 0;

                        if(horizontalMarginAutoCount != 0)
                            autoMarginWidth = (maxContentWidth - maxXOffsetSeen) / horizontalMarginAutoCount;
                        if(verticalMarginAutoCount != 0)
                            autoMarginHeight = (maxContentHeight - maxYOffsetSeen) / verticalMarginAutoCount;
                        
                        fixAlignmentAndLayout(autoMarginWidth, autoMarginHeight, indexOfLastWrap, childIndex);

                        horizontalMarginAutoCount = 0;
                        verticalMarginAutoCount = 0;
                        indexOfLastWrap = childIndex;
                        continue;
                    }
                }
                else
                {
                    if(x2 > maxContentWidth && maxYOffsetSeen < maxContentHeight)
                    {
                        nOffX = padding.left + border.left;
                        nOffY = maxYOffsetSeen;
                        maxChildWidth = maxContentWidth;
                        skipped = true;

                        int autoMarginWidth = 0;
                        int autoMarginHeight = 0;
                        if(horizontalMarginAutoCount != 0)
                            autoMarginWidth = (maxContentWidth - maxXOffsetSeen) / horizontalMarginAutoCount;
                        if(verticalMarginAutoCount != 0)
                            autoMarginHeight = (maxContentHeight - maxYOffsetSeen) / verticalMarginAutoCount;

                        fixAlignmentAndLayout(autoMarginWidth, autoMarginHeight, indexOfLastWrap, childIndex);
                        
                        horizontalMarginAutoCount = 0;
                        verticalMarginAutoCount = 0;
                        indexOfLastWrap = childIndex;
                        continue;
                    }
                }
            }

            if(x2 > width)
            {
                width = x2;
            }
            if(y2 > height)
            {
                height = y2;
            }

            if(direction == DIRECTION_VERTICAL)
            {
                nOffY = y2;
            }
            else
            {
                nOffX = x2;
            }

            maxXOffsetSeen = __max(maxXOffsetSeen, x2);
            maxYOffsetSeen = __max(maxYOffsetSeen, y2);
            
            maxChildWidth = actualMaxW - nOffX - padding.right - border.right;
            maxChildHeight = actualMaxH - nOffY - padding.bottom - border.bottom;
            skipped = false;
            childIndex++;
        }

        
        //post processing of children objects. This is alignment done on the last row if it has not been done yet
        if(direction == DIRECTION_HORIZONTAL)
        {
            int totalChildWidth = maxXOffsetSeen;
            int totalChildHeight = MathExt::max(height, maxYOffsetSeen);

            if(indexOfLastWrap != children.size()-1 || children.size() == 1)
            {
                if(horizontalMarginAutoCount != 0)
                {
                    int autoMarginWidthAdjustment = (width - totalChildWidth) / horizontalMarginAutoCount;
                    fixAlignmentAndLayout(autoMarginWidthAdjustment, 0, indexOfLastWrap, childIndex);
                }
            }
            
            childIndex = 0;
            while(childIndex < children.size())
            {
                SmartMemory<GuiItem> _child = children[childIndex];
                GuiItem* child = _child.getPointer();
                if(child == nullptr)
                {
                    childIndex++;
                    continue;
                }
                
                if(child->getType() == TYPE_LAYOUT)
                {
                    uint16_t childFlags = ((GuiLayout*)child)->getFlags();
                    GRect childMargin = ((GuiLayout*)child)->getMargin();

                    //Calculate auto margin width and height.
                    //First calculate true child width and height.
                    int childHeight = child->height;

                    //Factor in margin. (IFF not auto)
                    if(!(childFlags & FLAG_AUTO_BOTTOM_MARGIN))
                        childHeight += childMargin.bottom;

                    int autoMarginHeight = totalChildHeight - childHeight;

                    //if both are auto, center it. If right is auto, ignore as it is already left aligned.
                    if(childFlags & FLAG_AUTO_TOP_MARGIN && childFlags & FLAG_AUTO_BOTTOM_MARGIN)
                        autoMarginHeight /= 2;
                    else if(childFlags & FLAG_AUTO_BOTTOM_MARGIN)
                        autoMarginHeight = 0;
                    else if(!(childFlags & FLAG_AUTO_TOP_MARGIN) && !(childFlags & FLAG_AUTO_BOTTOM_MARGIN))
                        autoMarginHeight = 0; //No auto margin
                    
                    //Add the adjustments. May add 0
                    child->y += autoMarginHeight;
                }
                childIndex++;
            }
        }
        else
        {
            int totalChildWidth = MathExt::max(width, maxXOffsetSeen);
            int totalChildHeight = maxYOffsetSeen;
            if(indexOfLastWrap != children.size()-1 || children.size() == 1)
            {
                if(verticalMarginAutoCount != 0)
                {
                    int autoMarginHeightAdjust = (height - totalChildHeight) / verticalMarginAutoCount;
                    fixAlignmentAndLayout(0, autoMarginHeightAdjust, indexOfLastWrap, childIndex);
                }
            }
            
            childIndex = 0;
            while(childIndex < children.size())
            {
                SmartMemory<GuiItem> _child = children[childIndex];
                GuiItem* child = _child.getPointer();
                if(child == nullptr)
                {
                    childIndex++;
                    continue;
                }
                
                if(child->getType() == TYPE_LAYOUT)
                {
                    uint16_t childFlags = ((GuiLayout*)child)->getFlags();
                    GRect childMargin = ((GuiLayout*)child)->getMargin();

                    //Calculate auto margin width and height.
                    //First calculate true child width and height.
                    int childWidth = child->width;

                    //Factor in margin. (IFF not auto)
                    if(!(childFlags & FLAG_AUTO_RIGHT_MARGIN))
                        childWidth += childMargin.right;

                    int autoMarginWidth = totalChildWidth - childWidth;

                    //if both are auto, center it. If right is auto, ignore as it is already left aligned.
                    if(childFlags & FLAG_AUTO_LEFT_MARGIN && childFlags & FLAG_AUTO_RIGHT_MARGIN)
                        autoMarginWidth /= 2;
                    else if(childFlags & FLAG_AUTO_RIGHT_MARGIN)
                        autoMarginWidth = 0;
                    else if(!(childFlags & FLAG_AUTO_LEFT_MARGIN) && !(childFlags & FLAG_AUTO_RIGHT_MARGIN))
                        autoMarginWidth = 0; //No auto margin
                    
                    //Add the adjustments. May add 0
                    child->x += autoMarginWidth;
                }
                childIndex++;
            }
            
        }
        
        //Add in padding and border to the right and bottom edges
        width += padding.right + border.right;
        height += padding.bottom + border.bottom;
        
        //TODO: FIX THIS. CONTENT SIZE NEEDS TO BE COMPUTED ALONGSIDE WIDTH AND HEIGHT
        //before clamping to the maximum allowed width and height. minimum width and height should be included as a part of content.
        contentWidth = width;
        contentHeight = height;
        
        // StringTools::println("width = %d | height = %d", width, height);
        // //insure that width and height have not exceeded the min and max
        // width = __min(width, actualMaxW);
        // height = __min(height, actualMaxH);
    }

    void GuiLayoutList::fixAlignmentAndLayout(int autoMarginWidth, int autoMarginHeight, int startIndex, int endIndex)
    {
        int remainingX = 0;
        int remainingY = 0;

        if(autoMarginWidth == 0 && autoMarginHeight == 0)
            return; //Unnecessary call.
        
        for(int i=startIndex; i<endIndex; i++)
        {
            SmartMemory<GuiItem> _child = children[i];
            GuiItem* child = _child.getPointer();
            if(child == nullptr)
            {
                continue;
            }

            if(child->getType() == TYPE_EMPTY)
            {
                //ignore
            }
            else if(child->getType() == TYPE_CONTENT)
            {
                //just need to get the finalbox. It has no margin, padding, etc.
                // ((GuiContent*)child)->preprocess();
                child->x += remainingX;
                child->y += remainingY;

                remainingX = 0;
                remainingY = 0;
            }
            else
            {
                uint16_t childFlags = ((GuiLayout*)child)->getFlags();
                child->x += remainingX;
                child->y += remainingY;
                if(autoMarginWidth != 0)
                {
                    if(childFlags & FLAG_AUTO_LEFT_MARGIN)
                    {
                        child->x += autoMarginWidth;
                        remainingX += autoMarginWidth;
                    }
                    
                    if(childFlags & FLAG_AUTO_RIGHT_MARGIN)
                        remainingX += autoMarginWidth;
                    // else
                    //     remainingX = 0;
                }
                
                if(autoMarginHeight != 0)
                {
                    if(childFlags & FLAG_AUTO_TOP_MARGIN)
                    {
                        child->y += autoMarginHeight;
                        remainingY += autoMarginHeight;
                    }
                    
                    if(childFlags & FLAG_AUTO_BOTTOM_MARGIN)
                        remainingY += autoMarginHeight;
                    // else
                    //     remainingY = 0;
                }
            }
        }
    }

    void GuiLayoutList::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiLayout::loadDataFromXML(attribs, manager);
        auto pair = attribs.get("direction");
        if(pair != nullptr)
        {
            if(pair->second == "horizontal")
                setDirection(DIRECTION_HORIZONTAL);
            else
                setDirection(DIRECTION_VERTICAL);
        }
        attribs.remove(pair);
        
        pair = attribs.get("wrap");
        if(pair != nullptr)
        {
            if(pair->second == "true")
                setWrap(true);
            else
                setWrap(false);
        }
        attribs.remove(pair);
    }

    SmartMemory<GuiItem> GuiLayoutList::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiLayoutList* ins = new GuiLayoutList();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}