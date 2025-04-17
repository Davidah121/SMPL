#include "NewGui.h"

namespace smpl
{
    GuiLayout::GuiLayout() : GuiItem()
    {
        type = TYPE_LAYOUT;
    }
    GuiLayout::~GuiLayout()
    {

    }

    void GuiLayout::preUpdate()
    {
        for(SmartMemory<GuiItem> child : children)
        {
            GuiItem* rawP = child.getPointer();
            if(rawP != nullptr)
                rawP->doPreUpdate();
        }
    }

    void GuiLayout::update(SmartMemory<GuiManager> manager)
    {
        //just call update on the children
        for(SmartMemory<GuiItem> c : children)
        {
            GuiItem* p = c.getPointer();
            if(p != nullptr)
                p->doUpdate(manager);
        }
    }

    void GuiLayout::baseRender()
    {
        int tx = getTrueX();
        int ty = getTrueY();
        int tx2 = getTrueX() + width;
        int ty2 = getTrueY() + height;
        if(tx2 < tx || ty2 < ty)
            return;

        //render border first. Then background
        smpl::GraphicsInterface::setColor(borderColor);
        if(border.left > 0)
        {
            //left border
            smpl::GraphicsInterface::drawRect(tx, ty, (border.left), height, false);
        }
        if(border.right > 0)
        {
            //right border
            smpl::GraphicsInterface::drawRect(tx2-(border.right), ty, border.right, height, false);
        }
        if(border.top > 0)
        {
            //top border
            smpl::GraphicsInterface::drawRect(tx, ty, width, (border.top), false);
        }
        if(border.bottom > 0)
        {
            //bottom border
            smpl::GraphicsInterface::drawRect(tx, ty2-(border.bottom), width, border.bottom, false);
        }

        //readjust x,y,x2,y2 to remove border
        tx += border.left;
        ty += border.top;
        tx2 -= border.right;
        ty2 -= border.bottom;
        
        smpl::GraphicsInterface::setColor(backgroundColor);
        smpl::GraphicsInterface::drawRect(tx, ty, (tx2-tx), (ty2-ty), false);
    }

    void GuiLayout::render(SmartMemory<GuiManager> manager)
    {
        baseRender();
        for(SmartMemory<GuiItem> child : children)
        {
            GuiItem* rawP = child.getPointer();
            if(rawP != nullptr)
                rawP->doRender(manager);
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
        bool wasFound = false;

        for(int i=0; i<children.size(); i++)
        {
            GuiItem* testPointer = children[i].getPointer();

            if(testPointer != nullptr && testPointer != rawChildPointer)
            {
                if(testPointer == rawChildPointer)
                    wasFound = true;
                nChildren.push_back(c);
            }
        }

        if(rawChildPointer != nullptr && wasFound)
        {
            removedChildrenRect.push_back( rawChildPointer->getPreviousRenderRect() );
            rawChildPointer->setParent(nullptr);
        }

        if(nChildren.size() != children.size())
        {
            children = nChildren;
        }
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
        setShouldRender();
        margin = r;
    }
    void GuiLayout::setPadding(GRect r)
    {
        setShouldRender();
        padding = r;
    }
    void GuiLayout::setBorder(GRect r)
    {
        setShouldRender();
        border = r;
    }

    void GuiLayout::setBorderColor(smpl::Color c)
    {
        setShouldRender();
        borderColor = c;
    }
    void GuiLayout::setBackgroundColor(smpl::Color c)
    {
        setShouldRender();
        backgroundColor = c;
    }

    void GuiLayout::setFlags(uint16_t f)
    {
        setShouldRender();
        flags = f;
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
    smpl::Color GuiLayout::getBorderColor()
    {
        return borderColor;
    }
    smpl::Color GuiLayout::getBackgroundColor()
    {
        return backgroundColor;
    }

    uint16_t GuiLayout::getContentWidth()
    {
        return contentWidth;
    }
    uint16_t GuiLayout::getContentHeight()
    {
        return contentHeight;
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
        if(data.empty())
            return;
        if(data.front() == '{')
            data = data.substr(1);
        if(!data.empty() && data.back() == '}')
            data.pop_back();
        
        std::vector<std::string> split = StringTools::splitString(data, ',');
        for(std::string& arg : split)
        {
            arg = StringTools::removeWhitespace(arg, true, true);
        }
        
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
                if(loadValueFromAttrib(input.left, split[0]))
                    setXPercentage(true); //Only margin can do this so far
            }

            if(split[1] == "auto" && autoAllowed)
            {
                //only margin can do this
                setTopAlign();
            }
            else
            {
                if(loadValueFromAttrib(input.top, split[1]))
                    setYPercentage(true); //Only margin can do this so far
            }

            if(split[2] == "auto" && autoAllowed)
            {
                //only margin can do this
                if(split[0] != "auto")
                    setRightAlign();
                else
                    setCenterAlign(false);
            }
            else
            {
                loadValueFromAttrib(input.right, split[2]);
            }

            if(split[3] == "auto" && autoAllowed)
            {
                //only margin can do this
                if(split[1] != "auto")
                    setBottomAlign();
                else
                    setCenterAlign(true);
            }
            else
            {
                loadValueFromAttrib(input.bottom, split[3]);
            }
        }
    }

    void GuiLayout::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiItem::loadDataFromXML(attribs, manager);

        auto pair = attribs.get("margin");
        if(pair != nullptr)
        {
            loadRectStuff(margin, pair->second, true);
        }
        attribs.remove(pair);

        pair = attribs.get("padding");
        if(pair != nullptr)
        {
            loadRectStuff(padding, pair->second, false);
        }
        attribs.remove(pair);

        pair = attribs.get("border");
        if(pair != nullptr)
        {
            loadRectStuff(border, pair->second, false);
        }
        attribs.remove(pair);

        pair = attribs.get("max-width");
        if(pair != nullptr)
        {
            if(loadValueFromAttrib(maxWidth, pair->second))
                setMaxWidthPercentage(true);
        }
        attribs.remove(pair);

        pair = attribs.get("min-width");
        if(pair != nullptr)
        {
            if(loadValueFromAttrib(minWidth, pair->second))
                setMinWidthPercentage(true);
        }
        attribs.remove(pair);

        pair = attribs.get("max-height");
        if(pair != nullptr)
        {
            if(loadValueFromAttrib(maxHeight, pair->second))
                setMaxHeightPercentage(true);
        }
        attribs.remove(pair);

        pair = attribs.get("min-height");
        if(pair != nullptr)
        {
            if(loadValueFromAttrib(minHeight, pair->second))
                setMinHeightPercentage(true);
        }
        attribs.remove(pair);

        pair = attribs.get("background-color");
        if(pair != nullptr)
        {
            backgroundColor = loadColor(pair->second);
        }
        attribs.remove(pair);

        pair = attribs.get("border-color");
        if(pair != nullptr)
        {
            borderColor = loadColor(pair->second);
        }
        attribs.remove(pair);

        pair = attribs.get("col-span");
        if(pair != nullptr)
        {
            if(loadValueFromAttrib(colSpan, pair->second))
                colSpan = 1;
            colSpan = __max(colSpan, 1);
        }
        attribs.remove(pair);

        pair = attribs.get("row-span");
        if(pair != nullptr)
        {
            if(loadValueFromAttrib(rowSpan, pair->second))
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
}