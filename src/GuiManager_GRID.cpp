#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_GRID_CLASS

	const Class GuiGrid::myClass = Class("GuiGrid", {&GuiInstance::myClass});
	const Class* GuiGrid::getClass()
	{
		return &GuiGrid::myClass;
	}

	GuiGrid::GuiGrid(int x, int y)
	{
		baseX = x;
		baseY = y;
	}

	GuiGrid::~GuiGrid()
	{
		for(int i=0; i<locations.size(); i++)
		{
			if(locations[i]!=nullptr)
				delete locations[i];
		}
	}

	void GuiGrid::update()
	{
		std::vector<GuiInstance*> children = getChildren();
		if(rowMajorOrder)
		{
			for(int i=0; i<children.size(); i++)
			{
				int c = i%colSize;
				int r = i/colSize;

				locations[i]->x = x + c*gridXSpacing;
				locations[i]->y = y + r*gridYSpacing;

				if(r >= rowSize)
				{
					children[i]->setVisible(false);
					children[i]->setActive(false);
				}
				else
				{
					children[i]->setVisible(true);
					children[i]->setActive(true);
				}
			}
		}
		else
		{
			for(int i=0; i<children.size(); i++)
			{
				int r = i%rowSize;
				int c = i/rowSize;

				locations[i]->x = x + c*gridXSpacing;
				locations[i]->y = y + r*gridYSpacing;
				if(c >= colSize)
				{
					children[i]->setVisible(false);
					children[i]->setActive(false);
				}
				else
				{
					children[i]->setVisible(true);
					children[i]->setActive(true);
				}
			}
		}
	}

	void GuiGrid::render(Image* surf)
	{
		int width = colSize*gridXSpacing;
		int height = rowSize*gridYSpacing;

		SimpleGraphics::setColor(backgroundColor);
		surf->drawRect(renderX, renderY, renderX+width, renderY+height, false);
		
		SimpleGraphics::setColor(outlineColor);
		surf->drawRect(renderX, renderY, renderX+width, renderY+height, true);
	}

	void GuiGrid::setGridSpacing(int x, int y)
	{
		gridXSpacing = x;
		gridYSpacing = y;
	}

	void GuiGrid::setMaxRows(int row)
	{
		rowSize = row;
	}

	int GuiGrid::getMaxRows()
	{
		return rowSize;
	}

	void GuiGrid::setMaxColumns(int col)
	{
		colSize = col;
	}

	int GuiGrid::getMaxColumns()
	{
		return colSize;
	}

	void GuiGrid::setRowMajorOrder(bool v)
	{
		rowMajorOrder = v;
	}

	void GuiGrid::addElement(GuiInstance* ins)
	{
		if(ins!=nullptr)
		{
			Point* p = new Point();

			locations.push_back( p );
			addChild(ins);
			ins->setOffset( &p->x, &p->y );
		}
	}
	
	void GuiGrid::removeElement(GuiInstance* ins)
	{
		if(ins!=nullptr)
		{
			std::vector<Point*> nPositions;
			std::vector<GuiInstance*> children = getChildren();
			for(int i=0; i<children.size(); i++)
			{
				if(children[i] == ins)
				{
					removeChild(ins);
					if(locations[i]!=nullptr)
						delete locations[i];
					locations[i] = nullptr;
				}
				else
				{
					nPositions.push_back( locations[i] );
				}
			}
			locations = nPositions;
		}
	}

	bool GuiGrid::pointIsInGrid(int x, int y)
	{
		int width = colSize*gridXSpacing;
		int height = rowSize*gridYSpacing;
		
		if(x >= this->x && x <= this->x + width)
		{
			if(y >= this->y && y <= this->y + height)
			{
				return true;
			}
		}
		return false;
	}
	
	void GuiGrid::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}

	void GuiGrid::setOutlineColor(Color c)
	{
		outlineColor = c;
	}
	
	#pragma endregion

} //NAMESPACE glib END