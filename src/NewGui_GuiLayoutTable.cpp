#include "NewGui.h"

namespace glib
{
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

    void GuiLayoutTable::render(SmartMemory<GuiManager> manager)
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
                p->doRender(manager);
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
        int xAdjust = 0;
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
                    xAdjust = 0;
                    colIndex = 0;
                    rowIndex++;
                }
                continue;
            }

            //do xAdjust stuff
            // StringTools::println("x for %s = %d", child->getNameID().c_str(), child->x);
            child->x += xAdjust;
            // StringTools::println("new x for %s = %d", child->getNameID().c_str(), child->x);

            int actualChildW = child->width;
            int actualChildH = child->height;
            int childColSpan = 1;
            int childRowSpan = 1;

            if(child->getType() == TYPE_LAYOUT)
            {
                // StringTools::println("In table fix for %s", child->getNameID().c_str());

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
            int widthUsedInXAdjust = 0;
            int maxChildColSpan = __min(widthPerCol.size() - colIndex, childColSpan);
            int maxChildRowSpan = __min(heightPerRow.size() - rowIndex, childRowSpan);

            for(int i=0; i<maxChildColSpan; i++)
            {
                totalWidthOverCols += widthPerCol[colIndex+i];
            }

            //just need the first col that is part of the span.
            widthUsedInXAdjust = widthPerCol[colIndex];

            int totalHeightOverRows = 0;
            for(int i=0; i<maxChildRowSpan; i++)
            {
                totalHeightOverRows += heightPerRow[rowIndex+i];
            }
            
            xAdjust += __max(widthUsedInXAdjust - actualChildW, 0) - (2*innerTablePadding); //Should never be negative anyway.
            
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
                xAdjust = 0;
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
            if(loadValueFromAttrib(innerTablePadding, pair->data))
                innerTablePadding = 0;
        }
        attribs.remove(pair);
        
        pair = attribs.get("num-columns");
        if(pair != nullptr)
        {
            if(loadValueFromAttrib(columns, pair->data))
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
}