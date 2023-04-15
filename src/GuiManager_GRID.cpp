#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_GRID_CLASS

	const Class GuiGrid::globalClass = Class("GuiGrid", {&GuiInstance::globalClass});

	GuiGrid::GuiGrid(int x, int y) : GuiInstance()
	{
		setClass(globalClass);
		setBaseX(x);
		setBaseY(y);
		includeChildrenInBounds = false;
		setPriority(HIGHER_PRIORITY);
	}

	GuiGrid::GuiGrid(const GuiGrid& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiGrid::operator=(const GuiGrid& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiGrid::copy(const GuiGrid& other)
	{
		setClass(globalClass);
		gridXSpacing = other.gridXSpacing;
		gridYSpacing = other.gridYSpacing;
		rowSize = other.rowSize;
		colSize = other.colSize;
		rowMajorOrder = other.rowMajorOrder;
		backgroundColor = other.backgroundColor;
		outlineColor = other.outlineColor;
		setPriority(HIGHER_PRIORITY);
	}

	GuiGrid::~GuiGrid()
	{
		for(size_t i=0; i<locations.size(); i++)
		{
			if(locations[i]!=nullptr)
				delete locations[i];
		}
		locations.clear();
	}

	void GuiGrid::update()
	{
		Box2D oldBounds = boundingBox;
		boundingBox = GuiInstance::getInvalidBox();
		std::vector<GuiInstance*> children = getChildren();
		
		int maxWidth = 0;
		int maxHeight = 0;

		int minWidthRequired = 0;
		int minHeightRequired = 0;

		for(size_t i=0; i<children.size(); i++)
		{
			GuiInstance* child = children[i];
			Box2D box = child->getBoundingBox();

			minWidthRequired = MathExt::max((int)box.getWidth(), minWidthRequired);
			minHeightRequired = MathExt::max((int)box.getHeight(), minHeightRequired);
		}

		if(!rowMajorOrder)
		{
			for(size_t i=0; i<children.size(); i++)
			{
				int c = i%colSize;
				int r = i/colSize;
				
				locations[i]->x = x + (minWidthRequired+gridXSpacing) * c;
				locations[i]->y = y + (minHeightRequired+gridYSpacing) * r;
			}
			
			maxWidth = (minWidthRequired+gridXSpacing) * MathExt::min((int)children.size(), colSize);
			maxHeight = (minHeightRequired+gridYSpacing) * ((children.size())/colSize);
		}
		else
		{
			for(size_t i=0; i<children.size(); i++)
			{
				int r = i%rowSize;
				int c = i/rowSize;
				
				locations[i]->x = x + (minWidthRequired+gridXSpacing) * c;
				locations[i]->y = y + (minHeightRequired+gridYSpacing) * r;
			}
			
			maxWidth = (minWidthRequired+gridXSpacing) * MathExt::min((int)children.size(), rowSize);
			maxHeight = (minHeightRequired+gridYSpacing) * ((children.size())/rowSize);
		}
		
		if(oldBounds.getLeftBound() == x && oldBounds.getRightBound() == x+maxWidth)
		{
			if(oldBounds.getTopBound() == y && oldBounds.getBottomBound() == y+maxHeight)
			{
				//Don't do anything. It may still need to be redrawn for other reasons.
			}
			else
			{
				setShouldRedraw(true);
			}
		}
		else
		{
			setShouldRedraw(true);
		}
		boundingBox = Box2D(x, y, x+maxWidth, y+maxHeight);
	}

	void GuiGrid::render()
	{
		int width = (int)MathExt::round(boundingBox.getWidth());
		int height = (int)MathExt::round(boundingBox.getHeight());

		GraphicsInterface::setColor(backgroundColor);
		GraphicsInterface::drawRect(x, y, x+width, y+height, false);

		GraphicsInterface::setColor(outlineColor);
		GraphicsInterface::drawRect(x, y, x+width, y+height, true);
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
			GuiInstance::addChild(ins);
			ins->setOffset( &p->x, &p->y );
		}
	}
	
	void GuiGrid::removeElement(GuiInstance* ins)
	{
		if(ins!=nullptr)
		{
			std::vector<Point*> nPositions;
			std::vector<GuiInstance*> children = getChildren();
			for(size_t i=0; i<children.size(); i++)
			{
				if(children[i] == ins)
				{
					GuiInstance::removeChild(ins);
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

	void GuiGrid::addChild(GuiInstance* ins)
	{
		addElement(ins);
	}

	void GuiGrid::removeChild(GuiInstance* ins)
	{
		removeElement(ins);
	}

	bool GuiGrid::pointIsInGrid(int x, int y)
	{
		int width = (int)MathExt::round(boundingBox.getWidth());
		int height = (int)MathExt::round(boundingBox.getHeight());
		
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
		setShouldRedraw(true);
	}

	void GuiGrid::setOutlineColor(Color c)
	{
		outlineColor = c;
		setShouldRedraw(true);
	}

	void GuiGrid::solveBoundingBox()
	{
		//Call the update function since it only solves for the new bounding box. Also repositions all of the objects below it.
		update();
	}

	void GuiGrid::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs)
	{
		GuiInstance::loadDataFromXML(attribs);
		std::vector<std::string> possibleNames = { "horizontalspacing", "verticalspacing", "rowmajor", "maxrows", "maxcolumns", "backgroundcolor", "outlinecolor"};

		for(size_t i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.get(possibleNames[i]);
			if(it != nullptr)
			{
				if(it->key == "horizontalspacing")
				{
					gridXSpacing = std::abs(StringTools::toInt(it->data));
				}
				else if(it->key == "verticalspacing")
				{
					gridYSpacing = std::abs(StringTools::toInt(it->data));
				}
				else if(it->key == "maxrows")
				{
					rowSize = std::abs(StringTools::toInt(it->data));
				}
				else if(it->key == "maxcolumns")
				{
					colSize = std::abs(StringTools::toInt(it->data));
				}
				else if(it->key == "rowmajor")
				{
					rowMajorOrder = StringTools::equalsIgnoreCase<char>(it->data, "true");
				}
				else if(it->key == "backgroundcolor")
				{
					//define as color name or rgba
					backgroundColor = ColorNameConverter::NameToColor(it->data);
				}
				else if(it->key == "outlinecolor")
				{
					//define as color name or rgba
					outlineColor = ColorNameConverter::NameToColor(it->data);
				}
				
				attribs.remove(it);
			}
		}
	}

	void GuiGrid::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiGrid", GuiGrid::loadFunction);
	}

	GuiInstance* GuiGrid::loadFunction(SimpleHashMap<std::string, std::string>& attributes)
	{
		GuiGrid* ins = new GuiGrid(0,0);
		ins->loadDataFromXML(attributes);
		
		return ins;
	}
	
	#pragma endregion

} //NAMESPACE glib END