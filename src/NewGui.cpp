#include "NewGui.h"
namespace glib
{
    Color loadColor(std::string data)
    {
        //check if first character is '{'
        if(!data.empty())
        {
            if(data.front() == '{')
            {
                std::vector<std::string> split = StringTools::splitString(data.substr(1, data.size()-2), ',');
                if(split.size() == 3)
                {
                    //parse as rgb
                    try
                    {
                        Color c;
                        c.red = StringTools::toInt(split[0]);
                        c.green = StringTools::toInt(split[1]);
                        c.blue = StringTools::toInt(split[2]);
                        c.alpha = 255;
                        return c;
                    }
                    catch(...)
                    {
                        //problem parsing text. Ignore for now but log errors later
                    }
                    
                }
                else if(split.size() == 4)
                {
                    //parse as rgba
                    try
                    {
                        Color c;
                        c.red = StringTools::toInt(split[0]);
                        c.green = StringTools::toInt(split[1]);
                        c.blue = StringTools::toInt(split[2]);
                        c.alpha = StringTools::toInt(split[3]);
                        return c;
                    }
                    catch(...)
                    {
                        //problem parsing text. Ignore for now but log errors later
                    }
                }
            }
            else
            {
                std::vector<std::string> split = StringTools::splitString(data, ',');
                if(split.size() == 1)
                {
                    //parse as color name
                    return ColorNameConverter::NameToColor(split[0]);
                }
                else if(split.size() == 2)
                {
                    //parse as color name and alpha
                    try
                    {
                        Color c = ColorNameConverter::NameToColor(split[0]);
                        c.alpha = StringTools::toInt(split[1]);
                        return c;
                    }
                    catch(...)
                    {
                        //problem parsing text. Ignore for now but log errors later
                    }
                }
            }
        }
        return {0, 0, 0, 0};
    }

    GuiEmpty::GuiEmpty() : GuiItem()
    {

    }
    GuiEmpty::~GuiEmpty()
    {

    }
    
    void GuiEmpty::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        
    }

    void GuiEmpty::update()
    {

    }

    void GuiEmpty::render()
    {

    }

    void GuiEmpty::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        //just call parent load data function. Empty element with no traits
        GuiItem::loadDataFromXML(attribs, manager);
    }

    SmartMemory<GuiItem> GuiEmpty::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiEmpty* ins = new GuiEmpty();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }

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
        if(this->type == TYPE_LAYOUT)
        {
            ((GuiLayout*)this)->preUpdate();
        }
        else
        {
            fixPosition();
        }
    }

    void GuiItem::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        x = offX;
        y = offY;
    }

    void GuiItem::update(SmartMemory<GuiManager> manager)
    {
        
    }

    void GuiItem::render()
    {

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

    GuiContent::GuiContent() : GuiItem()
    {
        type = TYPE_CONTENT;
    }
    GuiContent::~GuiContent()
    {

    }
    void GuiContent::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiItem::loadDataFromXML(attribs, manager);
    }

    GuiLayout::GuiLayout() : GuiItem()
    {
        type = TYPE_LAYOUT;
    }
    GuiLayout::~GuiLayout()
    {

    }

    void GuiLayout::preUpdate()
    {
        fixPosition();

        for(SmartMemory<GuiItem> child : children)
        {
            GuiItem* rawP = child.getPointer();
            if(rawP != nullptr)
                rawP->preUpdate();
        }
    }
    
    void GuiLayout::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        //not needed. Must be overridden by any extended class.
    }

    void GuiLayout::update(SmartMemory<GuiManager> manager)
    {
        //just call update on the children
        for(SmartMemory<GuiItem> c : children)
        {
            GuiItem* p = c.getPointer();
            if(p != nullptr)
                p->update(manager);
        }
    }

    void GuiLayout::baseRender()
    {
        int tx = getTrueX();
        int ty = getTrueY();
        int tx2 = getTrueX() + width - 1;
        int ty2 = getTrueY() + height - 1;

        //render border first. Then background
        glib::GraphicsInterface::setColor(borderColor);
        if(border.left > 0)
        {
            //left border
            glib::GraphicsInterface::drawRect(tx, ty, tx+(border.left-1), ty2, false);
        }
        if(border.right > 0)
        {
            //right border
            glib::GraphicsInterface::drawRect(tx2-(border.right-1), ty, tx2, ty2, false);
        }
        if(border.top > 0)
        {
            //top border
            glib::GraphicsInterface::drawRect(tx, ty, tx2, ty+(border.top-1), false);
        }
        if(border.bottom > 0)
        {
            //bottom border
            glib::GraphicsInterface::drawRect(tx, ty2-(border.bottom-1), tx2, ty2, false);
        }

        //readjust x,y,x2,y2 to remove border
        tx += border.left;
        ty += border.top;
        tx2 -= border.right;
        ty2 -= border.bottom;
        
        glib::GraphicsInterface::setColor(backgroundColor);
        glib::GraphicsInterface::drawRect(tx, ty, tx2, ty2, false);
    }

    void GuiLayout::render()
    {
        baseRender();

        for(SmartMemory<GuiItem> child : children)
        {
            GuiItem* rawP = child.getPointer();
            if(rawP != nullptr)
                rawP->render();
        }
    }

    void GuiLayout::addChild(SmartMemory<GuiItem> c)
    {
        GuiItem* rawPointer = c.getPointer();

        if(rawPointer != this && rawPointer != nullptr)
        {
            rawPointer->setParent( SmartMemory<GuiItem>::createNoDelete(this) );
            children.push_back(c);
        }
    }

    void GuiLayout::removeChild(SmartMemory<GuiItem> c)
    {
        std::vector<SmartMemory<GuiItem>> nChildren;
        GuiItem* rawChildPointer = c.getPointer();

        for(int i=0; i<children.size(); i++)
        {
            GuiItem* testPointer = children[i].getPointer();

            if(testPointer != nullptr && testPointer != rawChildPointer)
            {
                nChildren.push_back(c);
            }
        }

        if(rawChildPointer != nullptr)
        {
            rawChildPointer->setParent(nullptr);
        }

        if(nChildren.size() != children.size())
            children = nChildren;
    }

    void GuiLayout::setLeftAlign()
    {
        uint16_t temp = flags & 0b1100111111;
        temp |= GuiItem::FLAG_AUTO_RIGHT_MARGIN;
        flags = temp;
    }
    void GuiLayout::setTopAlign()
    {
        uint16_t temp = flags & 0b0011111111;
        temp |= GuiItem::FLAG_AUTO_TOP_MARGIN;
        flags = temp;
    }
    void GuiLayout::setCenterAlign(bool vertical)
    {
        if(!vertical)
        {
            uint16_t temp = flags & 0b1100111111;
            temp |= GuiItem::FLAG_AUTO_RIGHT_MARGIN;
            temp |= GuiItem::FLAG_AUTO_LEFT_MARGIN;
            flags = temp;
        }
        else
        {
            uint16_t temp = flags & 0b0011111111;
            temp |= GuiItem::FLAG_AUTO_TOP_MARGIN;
            temp |= GuiItem::FLAG_AUTO_BOTTOM_MARGIN;
            flags = temp;
        }
    }
    void GuiLayout::setBottomAlign()
    {
        uint16_t temp = flags & 0b0011111111;
        temp |= GuiItem::FLAG_AUTO_BOTTOM_MARGIN;
        flags = temp;
    }
    void GuiLayout::setRightAlign()
    {
        uint16_t temp = flags & 0b1100111111;
        temp |= GuiItem::FLAG_AUTO_LEFT_MARGIN;
        flags = temp;
    }

    void GuiLayout::setMinWidthPercentage(bool t)
    {
        uint16_t temp = flags & 0b1111111110;
        temp |= (t) ? FLAG_MIN_WIDTH_PERCENTAGE : 0;
        flags = temp;
    }
    void GuiLayout::setMaxWidthPercentage(bool t)
    {
        uint16_t temp = flags & 0b1111111011;
        temp |= (t) ? FLAG_MAX_WIDTH_PERCENTAGE : 0;
        flags = temp;
    }
    void GuiLayout::setMinHeightPercentage(bool t)
    {
        uint16_t temp = flags & 0b1111111101;
        temp |= (t) ? FLAG_MIN_HEIGHT_PERCENTAGE : 0;
        flags = temp;
    }
    void GuiLayout::setMaxHeightPercentage(bool t)
    {
        uint16_t temp = flags & 0b1111110111;
        temp |= (t) ? FLAG_MAX_HEIGHT_PERCENTAGE : 0;
        flags = temp;
    }
    void GuiLayout::setXPercentage(bool t)
    {
        uint16_t temp = flags & 0b1111101111;
        temp |= (t) ? FLAG_X_PERCENTAGE : 0;
        flags = temp;
    }
    void GuiLayout::setYPercentage(bool t)
    {
        uint16_t temp = flags & 0b1111011111;
        temp |= (t) ? FLAG_Y_PERCENTAGE : 0;
        flags = temp;
    }
    void GuiLayout::setAbsolutePosition(bool t)
    {
        uint16_t temp = flags & 0b1110111111;
        temp |= (t) ? FLAG_ABSOLUTE_POSITION : 0;
        flags = temp;
    }

    void GuiLayout::setMargin(GRect r)
    {
        margin = r;
    }
    void GuiLayout::setPadding(GRect r)
    {
        padding = r;
    }
    void GuiLayout::setBorder(GRect r)
    {
        border = r;
    }

    void GuiLayout::setBorderColor(glib::Color c)
    {
        borderColor = c;
    }
    void GuiLayout::setBackgroundColor(glib::Color c)
    {
        backgroundColor = c;
    }

    uint16_t GuiLayout::getFlags()
    {
        return flags;
    }
    GRect GuiLayout::getMargin()
    {
        return margin;
    }
    GRect GuiLayout::getPadding()
    {
        return padding;
    }
    GRect GuiLayout::getBorder()
    {
        return border;
    }
    glib::Color GuiLayout::getBorderColor()
    {
        return borderColor;
    }
    glib::Color GuiLayout::getBackgroundColor()
    {
        return backgroundColor;
    }

    void GuiLayout::setMinWidth(uint16_t v)
    {
        minWidth = v;
    }
    void GuiLayout::setMaxWidth(uint16_t v)
    {
        maxWidth = v;
    }
    void GuiLayout::setMinHeight(uint16_t v)
    {
        minHeight = v;
    }
    void GuiLayout::setMaxHeight(uint16_t v)
    {
        maxHeight = v;
    }


    void GuiLayout::setColSpan(uint16_t c)
    {
        colSpan = __max(c, 1); //must be at least 1
    }

    void GuiLayout::setRowSpan(uint16_t r)
    {
        rowSpan = __max(r, 1); //must be at least 1
    }

    uint16_t GuiLayout::getColSpan()
    {
        return colSpan;
    }
    uint16_t GuiLayout::getRowSpan()
    {
        return rowSpan;
    }

    void GuiLayout::loadRectStuff(GRect& input, std::string data, bool autoAllowed)
    {
        std::vector<std::string> split = StringTools::splitString(data, ',');
        if(split.size() == 4)
        {
            //parse and look for % or auto. % is not supported so skip it.
            if(split[0] == "auto" && autoAllowed)
            {
                //only margin can do this
                setLeftAlign();
            }
            else
            {
                if(loadValueStuff(input.left, data))
                    setXPercentage(true); //Only margin can do this so far
            }
            if(split[1] == "auto" && autoAllowed)
            {
                //only margin can do this
                setTopAlign();
            }
            else
            {
                if(loadValueStuff(input.top, data))
                    setYPercentage(true); //Only margin can do this so far
            }
            if(split[2] == "auto" && autoAllowed)
            {
                //only margin can do this
                setRightAlign();
            }
            else
            {
                loadValueStuff(input.right, data);
            }
            if(split[3] == "auto" && autoAllowed)
            {
                //only margin can do this
                setBottomAlign();
            }
            else
            {
                loadValueStuff(input.bottom, data);
            }
        }
    }

    void GuiLayout::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiItem::loadDataFromXML(attribs, manager);

        auto pair = attribs.get("margin");
        if(pair != nullptr)
        {
            loadRectStuff(margin, pair->data, true);
        }
        attribs.remove(pair);

        pair = attribs.get("padding");
        if(pair != nullptr)
        {
            loadRectStuff(padding, pair->data, false);
        }
        attribs.remove(pair);

        pair = attribs.get("border");
        if(pair != nullptr)
        {
            loadRectStuff(border, pair->data, false);
        }
        attribs.remove(pair);

        pair = attribs.get("max-width");
        if(pair != nullptr)
        {
            if(loadValueStuff(maxWidth, pair->data))
                setMaxWidthPercentage(true);
        }
        attribs.remove(pair);

        pair = attribs.get("min-width");
        if(pair != nullptr)
        {
            if(loadValueStuff(minWidth, pair->data))
                setMinWidthPercentage(true);
        }
        attribs.remove(pair);

        pair = attribs.get("max-height");
        if(pair != nullptr)
        {
            if(loadValueStuff(maxHeight, pair->data))
                setMaxHeightPercentage(true);
        }
        attribs.remove(pair);

        pair = attribs.get("min-height");
        if(pair != nullptr)
        {
            if(loadValueStuff(minHeight, pair->data))
                setMinHeightPercentage(true);
        }
        attribs.remove(pair);

        pair = attribs.get("background-color");
        if(pair != nullptr)
        {
            backgroundColor = loadColor(pair->data);
        }
        attribs.remove(pair);

        pair = attribs.get("border-color");
        if(pair != nullptr)
        {
            borderColor = loadColor(pair->data);
        }
        attribs.remove(pair);

        pair = attribs.get("col-span");
        if(pair != nullptr)
        {
            if(loadValueStuff(colSpan, pair->data))
                colSpan = 1;
            colSpan = __max(colSpan, 1);
        }
        attribs.remove(pair);

        pair = attribs.get("row-span");
        if(pair != nullptr)
        {
            if(loadValueStuff(rowSpan, pair->data))
                rowSpan = 1;
            rowSpan = __max(rowSpan, 1);
        }
        attribs.remove(pair);

        pair = attribs.get("absolute");
        if(pair != nullptr)
        {
            setAbsolutePosition(true);
        }
        attribs.remove(pair);
    }

    #pragma region GUI_LAYOUT_FIXED
    GuiLayoutFixed::GuiLayoutFixed() : GuiLayout()
    {

    }

    GuiLayoutFixed::~GuiLayoutFixed()
    {

    }

    void GuiLayoutFixed::processLayout(int offX, int offY, int maximumWidth, int maximumHeight)
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
            
            
            actualMaxW = maximumWidth - x;
            actualMaxH = maximumHeight - y;
            
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
        int maxContentWidth = actualMaxW - nOffX - x - padding.right - border.right;
        int maxContentHeight = actualMaxH - nOffY - y - padding.bottom - border.bottom;

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
                ((GuiContent*)child)->layoutUpdate(nOffX, nOffY, maxChildWidth, maxChildHeight);
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
                ((GuiLayout*)child)->layoutUpdate(nOffX, nOffY, maxChildWidth, maxChildHeight);
                
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

    #pragma endregion

    #pragma region GUI_LAYOUT_LIST

    GuiLayoutList::GuiLayoutList(bool direction, bool wrap) : GuiLayout()
    {
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
            
            
            actualMaxW = maximumWidth - x;
            actualMaxH = maximumHeight - y;
            
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
        int maxContentWidth = actualMaxW - nOffX - x - padding.right - border.right;
        int maxContentHeight = actualMaxH - nOffY - y - padding.bottom - border.bottom;

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
                ((GuiContent*)child)->layoutUpdate(nOffX, nOffY, maxChildWidth, maxChildHeight);
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
                ((GuiLayout*)child)->layoutUpdate(nOffX, nOffY, maxChildWidth, maxChildHeight);
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
                    if(y2 > maxContentHeight)
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
                    if(x2 > maxContentWidth)
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
            
            maxChildWidth = actualMaxW - nOffX - x - padding.right - border.right;
            maxChildHeight = actualMaxH - nOffY - y - padding.bottom - border.bottom;
            skipped = false;
            childIndex++;
        }

        
        //post processing of children objects. This is alignment done on the last row if it has not been done yet
        if(direction == DIRECTION_HORIZONTAL)
        {
            int totalChildWidth = maxXOffsetSeen;
            int totalChildHeight = maxYOffsetSeen;
            if(indexOfLastWrap != children.size()-1)
            {
                int autoMarginWidth = 0;
                int autoMarginHeight = 0;
                if(horizontalMarginAutoCount != 0)
                    autoMarginWidth = (width - totalChildWidth) / horizontalMarginAutoCount;
                if(verticalMarginAutoCount != 0)
                    autoMarginHeight = (height - totalChildHeight) / verticalMarginAutoCount;

                fixAlignmentAndLayout(autoMarginWidth, autoMarginHeight, indexOfLastWrap, childIndex);
                
                horizontalMarginAutoCount = 0;
                verticalMarginAutoCount = 0;
            }
        }
        else
        {
            int totalChildWidth = maxXOffsetSeen - padding.right - border.right;
            int totalChildHeight = maxYOffsetSeen - padding.bottom - border.bottom;
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
                    int childHeight = child->height;

                    //Factor in margin. (IFF not auto)
                    if(!(childFlags & FLAG_AUTO_RIGHT_MARGIN))
                        childWidth += childMargin.right;
                    if(!(childFlags & FLAG_AUTO_BOTTOM_MARGIN))
                        childHeight += childMargin.bottom;

                    int autoMarginWidth = totalChildWidth - childWidth;
                    int autoMarginHeight = totalChildHeight - childHeight;

                    //if both are auto, center it. If right is auto, ignore as it is already left aligned.
                    if(childFlags & FLAG_AUTO_LEFT_MARGIN && childFlags & FLAG_AUTO_RIGHT_MARGIN)
                        autoMarginWidth /= 2;
                    else if(childFlags & FLAG_AUTO_RIGHT_MARGIN)
                        autoMarginWidth = 0;
                    else if(!(childFlags & FLAG_AUTO_LEFT_MARGIN) && !(childFlags & FLAG_AUTO_RIGHT_MARGIN))
                        autoMarginWidth = 0; //No auto margin
                    
                    //Same here, but if bottom is auto, ignore as it is already top aligned.
                    if(childFlags & FLAG_AUTO_TOP_MARGIN && childFlags & FLAG_AUTO_BOTTOM_MARGIN)
                        autoMarginHeight /= 2;
                    else if(childFlags & FLAG_AUTO_BOTTOM_MARGIN)
                        autoMarginHeight = 0;
                    else if(!(childFlags & FLAG_AUTO_TOP_MARGIN) && !(childFlags & FLAG_AUTO_BOTTOM_MARGIN))
                        autoMarginHeight = 0; //No auto margin
                    
                    //Add the adjustments. May add 0
                    child->x += autoMarginWidth;
                    child->y += autoMarginHeight;
                }
                childIndex++;
            }
        }
        
        //Add in padding and border to the right and bottom edges
        width += padding.right + border.right;
        height += padding.bottom + border.bottom;

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
                        child->x += autoMarginWidth;
                    
                    if(childFlags & FLAG_AUTO_RIGHT_MARGIN)
                        remainingX = autoMarginWidth;
                    else
                        remainingX = 0;
                }
                
                if(autoMarginHeight != 0)
                {
                    if(childFlags & FLAG_AUTO_TOP_MARGIN)
                        child->y += autoMarginHeight;
                    
                    if(childFlags & FLAG_AUTO_BOTTOM_MARGIN)
                        remainingY = autoMarginHeight;
                    else
                        remainingY = 0;
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
            if(pair->data == "horizontal")
                setDirection(DIRECTION_HORIZONTAL);
            else
                setDirection(DIRECTION_VERTICAL);
        }
        attribs.remove(pair);
        
        pair = attribs.get("wrap");
        if(pair != nullptr)
        {
            if(pair->data == "true")
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

    #pragma endregion

    #pragma region GUI_LAYOUT_TABLE

    GuiLayoutTable::GuiLayoutTable(uint16_t numColumns) : GuiLayout()
    {
        columns = numColumns;
    }

    GuiLayoutTable::~GuiLayoutTable()
    {

    }

    void GuiLayoutTable::setShowInnerGrid(bool v)
    {
        showInnerGrid = v;
    }

    void GuiLayoutTable::setNumberOfColumns(uint16_t c)
    {
        columns = c;
    }

    void GuiLayoutTable::setInnerTablePadding(uint16_t p)
    {
        innerTablePadding = p;
    }

    void GuiLayoutTable::render()
    {
        int tx = getTrueX();
        int ty = getTrueY();
        int tx2 = getTrueX() + width - 1;
        int ty2 = getTrueY() + height - 1;

        
        //render border first. Then background
        glib::GraphicsInterface::setColor(borderColor);
        if(border.left > 0)
        {
            //left border
            glib::GraphicsInterface::drawRect(tx, ty, tx+(border.left-1), ty2, false);
        }
        if(border.right > 0)
        {
            //right border
            glib::GraphicsInterface::drawRect(tx2-(border.right-1), ty, tx2, ty2, false);
        }
        if(border.top > 0)
        {
            //top border
            glib::GraphicsInterface::drawRect(tx, ty, tx2, ty+(border.top-1), false);
        }
        if(border.bottom > 0)
        {
            //bottom border
            glib::GraphicsInterface::drawRect(tx, ty2-(border.bottom-1), tx2, ty2, false);
        }

        //readjust x,y,x2,y2 to remove border
        tx += border.left;
        ty += border.top;
        tx2 -= border.right;
        ty2 -= border.bottom;
        
        glib::GraphicsInterface::setColor(backgroundColor);
        glib::GraphicsInterface::drawRect(tx, ty, tx2, ty2, false);

        if(showInnerGrid)
        {
            glib::GraphicsInterface::setColor(borderColor);

            //get start point
            int tempX = tx + padding.left;
            int tempY = ty + padding.top;

            for(int r=0; r<tableCellSkip.size(); r++)
            {
                tempX = tx + padding.left;
                for(int c=0; c<tableCellSkip[r].size(); c++)
                {
                    //Do not draw vertical line here if the cell is apart of a column span
                    if(!(tableCellSkip[r][c] & COLSPAN))
                    {
                        //draw vertical line to the left.
                        glib::GraphicsInterface::drawLine(tempX, tempY, tempX, tempY + tableHeightPerRow[r]);
                    }

                    //Do not draw horizontal line here if the cel is apart of a row span
                    if(!(tableCellSkip[r][c] & ROWSPAN))
                    {
                        //draw horizontal line above.
                        glib::GraphicsInterface::drawLine(tempX, tempY, tempX + tableWidthPerCol[c], tempY);
                    }
                    
                    tempX += tableWidthPerCol[c];
                }

                //draw vertical line to the left.
                glib::GraphicsInterface::drawLine(tempX, tempY, tempX, tempY + tableHeightPerRow[r]);

                tempY += tableHeightPerRow[r];
            }

            //draw last horizontal line at the bottom
            tempX = tx + padding.left;
            int endX = tx2 - padding.right;
            glib::GraphicsInterface::drawLine(tempX, tempY, endX, tempY);
        }

        for(SmartMemory<GuiItem> child : children)
        {
            GuiItem* p = child.getPointer();
            if(p != nullptr)
                p->render();
        }
    }

    void GuiLayoutTable::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
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
            
            
            actualMaxW = maximumWidth - x;
            actualMaxH = maximumHeight - y;
            
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
        //Note that flexing still occurs but each row and column are sized up equally to maintain their relative sizes to each other.
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
        int maxContentWidth = actualMaxW - nOffX - x - padding.right - border.right;
        int maxContentHeight = actualMaxH - nOffY - y - padding.bottom - border.bottom;

        //The maximum allowed width and height for a child of this layout.
        //Changes per child
        int maxChildWidth = maxContentWidth;
        int maxChildHeight = maxContentHeight;

        //Needed to determine which cells to skip over due to spanning
        tableCellSkip = std::vector< std::vector<uint8_t>>();

        //Width per column. Each column must be the same width down on each row at the end. Record max width per element on each column
        std::vector<int> widthPerCol = std::vector<int>(columns);

        //Each row must be the same height across itself. Record max height on each row per element.
        std::vector<int> heightPerRow = std::vector<int>(1);
        
        //Note that auto margin still works like normal but since it is done per cell, we don't have to worry about counting anything.
        
        
        //calculate child maximum width and height
        int rowIndex = 0;
        int colIndex = 0;
        
        int childIndex = 0;

        //add in inner table padding
        nOffX += innerTablePadding;
        nOffY += innerTablePadding;
        
        //add the first cell skip to avoid issues. It must always be false. Should default to false but could memset if necessary
        //add the entire row cause why not
        tableCellSkip.push_back( std::vector<uint8_t>(columns) );

        while(childIndex < children.size())
        {
            SmartMemory<GuiItem> _child = children[childIndex];
            GuiItem* child = _child.getPointer();
            if(child == nullptr)
            {
                childIndex++;
                continue;
            }

            if(tableCellSkip[rowIndex][colIndex])
            {
                nOffX += widthPerCol[colIndex];
                colIndex++;
                if(colIndex >= columns)
                {
                    rowIndex++;
                    colIndex = 0;

                    if(rowIndex >= heightPerRow.size())
                        heightPerRow.push_back(0);
                    if(rowIndex >= tableCellSkip.size())
                        tableCellSkip.push_back( std::vector<uint8_t>(columns) );
                    
                    
                    nOffX = padding.left + border.left + innerTablePadding;
                    nOffY = height + innerTablePadding;
                }
                continue;
            }
            
            //Move everything back to 0,0 to be consistent.
            child->resetPosition();
            
            int x2=0;
            int y2=0;

            int childColSpan = 1;
            int childRowSpan = 1;

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
                ((GuiContent*)child)->layoutUpdate(nOffX, nOffY, maxChildWidth, maxChildHeight);
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
                ((GuiLayout*)child)->layoutUpdate(nOffX, nOffY, maxChildWidth, maxChildHeight);
                x2 = child->x + child->width; //x + width
                y2 = child->y + child->height; //y + height

                //Factor in margin. (IFF not auto)
                if(!(childFlags & FLAG_AUTO_RIGHT_MARGIN))
                    x2 += childMargin.right;
                if(!(childFlags & FLAG_AUTO_BOTTOM_MARGIN))
                    y2 += childMargin.bottom;
                
                //Set child col and row span
                childColSpan = ((GuiLayout*)child)->getColSpan();
                childRowSpan = ((GuiLayout*)child)->getRowSpan();
            }

            if(x2 > child->x)
                x2 += 2*innerTablePadding;
            if(y2 > child->y)
                y2 += 2*innerTablePadding;
            
            //Some flexing.
            if(x2 > width)
            {
                width = x2 - innerTablePadding;
            }
            if(y2 > height)
            {
                height = y2 - innerTablePadding;
            }

            //No wrapping but the children may have that option so maximum width must be included still.
            //This may cause things to extend out of the allowed width and height.

            //No direction. Always horizontal
            nOffX = x2;

            //record max width and height for column and row
            //These factor in the margins. If the margins are auto, they are considered to be 0.
            int actualChildWidth = x2 - child->x;
            int actualChildHeight = y2 - child->y;

            if(childColSpan == 1)
            {
                widthPerCol[colIndex] = __max(widthPerCol[colIndex], actualChildWidth);

                //col span = 1 so don't skip next cell.
            }
            else
            {
                //total of widthPerCol from colIndex to colIndex+childColSpan-1
                int totalWidthOverCol = 0;
                int spanAllowed = __min(childColSpan, columns-colIndex);
                for(int i=0; i<spanAllowed; i++)
                {
                    totalWidthOverCol += widthPerCol[colIndex+i];
                }

                int totalWidthAdjusted = __max(totalWidthOverCol, actualChildWidth);

                //average difference over each col and add them to those cols
                int adjustmentAmount = (totalWidthAdjusted - totalWidthOverCol) / spanAllowed;
                for(int i=0; i<spanAllowed; i++)
                {
                    widthPerCol[colIndex+i] += adjustmentAmount;
                }

                //col span > 1 so skip next cells.
                for(int i=1; i<spanAllowed; i++)
                    tableCellSkip[rowIndex][colIndex+i] |= COLSPAN;
            }
            
            if(childRowSpan == 1)
            {
                heightPerRow[rowIndex] = __max(heightPerRow[rowIndex], actualChildHeight);
                
                //row span = 1 so do not skip on cell below.
            }
            else
            {
                //add necessary rows and average the height over them.
                int addAmount = (rowIndex + childRowSpan) - heightPerRow.size();
                for(int i=0; i<addAmount; i++)
                {
                    heightPerRow.push_back(0);
                }
                int avgHeight = actualChildHeight / childRowSpan;
                for(int i=0; i<=addAmount; i++)
                {
                    heightPerRow[rowIndex+i] = __max(heightPerRow[rowIndex+i], avgHeight);
                }

                //row span > 1 so skip on cells below. Must add rows below
                int colAddAmount = __min(childColSpan, columns-colIndex);
                for(int i=0; i<addAmount; i++)
                {
                    tableCellSkip.push_back( std::vector<uint8_t>(columns) );
                    for(int j=0; j<colAddAmount; j++)
                    {
                        tableCellSkip.back()[colIndex+j] |= ROWSPAN;
                    }
                }
                
            }
            
            //increment and move to a new row if necessary.
            childIndex++;
            colIndex++;
            if(colIndex >= columns)
            {
                rowIndex++;
                colIndex = 0;

                if(rowIndex >= heightPerRow.size())
                    heightPerRow.push_back(0);
                if(rowIndex >= tableCellSkip.size())
                    tableCellSkip.push_back( std::vector<uint8_t>(columns) );
                
                nOffX = padding.left + border.left + innerTablePadding;
                nOffY = y2;
            }

            maxChildWidth = actualMaxW - nOffX - x - padding.right - border.right;
            maxChildHeight = actualMaxH - nOffY - y - padding.bottom - border.bottom;
        }
        
        //post processing of children objects. This is alignment done on each cell after setting new width and height for each cell.
        childIndex = 0;
        colIndex = 0;
        rowIndex = 0;

        //Also need to adjust x and y offsets for each object with respect to the increase in size of a column and row.
        //This is cumulative where x resets when a moving to a new row.
        int xInc = 0;
        tableHeightPerRow = heightPerRow;
        tableWidthPerCol = widthPerCol;

        while(childIndex < children.size())
        {
            SmartMemory<GuiItem> _child = children[childIndex];
            GuiItem* child = _child.getPointer();
            if(child == nullptr)
            {
                childIndex++;
                continue;
            }

            if(tableCellSkip[rowIndex][colIndex])
            {
                colIndex++;
                if(colIndex >= columns)
                {
                    xInc = 0;
                    colIndex = 0;
                    rowIndex++;
                }
                continue;
            }

            //do xInc stuff
            child->x += xInc;

            int actualChildW = child->width;
            int actualChildH = child->height;
            int childColSpan = 1;
            int childRowSpan = 1;

            if(child->getType() == TYPE_LAYOUT)
            {
                uint16_t childFlags = ((GuiLayout*)child)->getFlags();
                GRect childMargin = ((GuiLayout*)child)->getMargin();

                //Factor in margin. (IFF not auto)
                if(!(childFlags & FLAG_AUTO_RIGHT_MARGIN))
                    actualChildW += childMargin.right;
                if(!(childFlags & FLAG_AUTO_BOTTOM_MARGIN))
                    actualChildH += childMargin.bottom;
                
                childColSpan = ((GuiLayout*)child)->getColSpan();
                childRowSpan = ((GuiLayout*)child)->getRowSpan();
            }

            int totalWidthOverCols = 0;
            int maxChildColSpan = __min(widthPerCol.size() - colIndex, childColSpan);
            int maxChildRowSpan = __min(heightPerRow.size() - rowIndex, childRowSpan);
            
            for(int i=0; i<maxChildColSpan; i++)
            {
                totalWidthOverCols += widthPerCol[colIndex+i];
            }

            int totalHeightOverRows = 0;
            for(int i=0; i<maxChildRowSpan; i++)
            {
                totalHeightOverRows += heightPerRow[rowIndex+i];
            }
            
            xInc += __max(totalWidthOverCols - actualChildW, 0) - 2*innerTablePadding; //Should never be negative anyway.
            
            if(child->getType() == TYPE_LAYOUT)
            {
                uint16_t childFlags = ((GuiLayout*)child)->getFlags();

                int autoMarginWidth = totalWidthOverCols - actualChildW - 2*innerTablePadding;
                int autoMarginHeight = totalHeightOverRows - actualChildH - 2*innerTablePadding;

                //if both are auto, center it. If right is auto, ignore as it is already left aligned.
                if(childFlags & FLAG_AUTO_LEFT_MARGIN && childFlags & FLAG_AUTO_RIGHT_MARGIN)
                    autoMarginWidth /= 2;
                else if(childFlags & FLAG_AUTO_RIGHT_MARGIN)
                    autoMarginWidth = 0;
                else if(!(childFlags & FLAG_AUTO_LEFT_MARGIN) && !(childFlags & FLAG_AUTO_RIGHT_MARGIN))
                    autoMarginWidth = 0; //No auto margin
                
                //Same here, but if bottom is auto, ignore as it is already top aligned.
                if(childFlags & FLAG_AUTO_TOP_MARGIN && childFlags & FLAG_AUTO_BOTTOM_MARGIN)
                    autoMarginHeight /= 2;
                else if(childFlags & FLAG_AUTO_BOTTOM_MARGIN)
                    autoMarginHeight = 0;
                else if(!(childFlags & FLAG_AUTO_TOP_MARGIN) && !(childFlags & FLAG_AUTO_BOTTOM_MARGIN))
                    autoMarginHeight = 0; //No auto margin
                
                //Add the adjustments. May add 0
                child->x += autoMarginWidth;
                child->y += autoMarginHeight;
            }

            childIndex++;
            colIndex++;
            if(colIndex >= columns)
            {
                xInc = 0;
                colIndex = 0;
                rowIndex++;
            }
        }

        //set width = the total width of all columns + padding.left + border.left
        int actualWidth = padding.left + border.left;
        for(int i=0; i<widthPerCol.size(); i++)
        {
            actualWidth += widthPerCol[i];
        }
        
        //Add in padding and border to the right and bottom edges
        width = actualWidth + padding.right + border.right;
        height += padding.bottom + border.bottom;

        //May have exceeded the maximum allowed width and height.
        // //insure that width and height have not exceeded the min and max
        // width = __min(width, actualMaxW);
        // height = __min(height, actualMaxH);
    }

    void GuiLayoutTable::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiLayout::loadDataFromXML(attribs, manager);
        auto pair = attribs.get("inner-padding");
        if(pair != nullptr)
        {
            if(loadValueStuff(innerTablePadding, pair->data))
                innerTablePadding = 0;
        }
        attribs.remove(pair);
        
        pair = attribs.get("num-columns");
        if(pair != nullptr)
        {
            if(loadValueStuff(columns, pair->data))
                columns = 1;
        }
        attribs.remove(pair);

        pair = attribs.get("show-inner-grid");
        if(pair != nullptr)
        {
            showInnerGrid = (pair->data == "true");
        }
        attribs.remove(pair);
    }

    SmartMemory<GuiItem> GuiLayoutTable::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiLayoutTable* ins = new GuiLayoutTable(1);
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }

    #pragma endregion
}