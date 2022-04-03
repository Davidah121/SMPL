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
		
		int maxWidth = 0;
		int maxHeight = 0;

		int minWidthRequired = 0;
		int minHeightRequired = 0;

		for(int i=0; i<children.size(); i++)
		{
			GuiInstance* child = children[i];
			Box2D box = child->getBoundingBox();

			minWidthRequired = max(box.getWidth(), minWidthRequired);
			minHeightRequired = max(box.getHeight(), minHeightRequired);
		}

		if(!rowMajorOrder)
		{
			for(int i=0; i<children.size(); i++)
			{
				int c = i%colSize;
				int r = i/colSize;
				
				locations[i]->x = x + (minWidthRequired+gridXSpacing) * c;
				locations[i]->y = y + (minHeightRequired+gridYSpacing) * r;
			}
			
			maxWidth = (minWidthRequired+gridXSpacing) * min(children.size(), colSize);
			maxHeight = (minHeightRequired+gridYSpacing) * ((children.size())/colSize);
		}
		else
		{
			for(int i=0; i<children.size(); i++)
			{
				int r = i%rowSize;
				int c = i/rowSize;
				
				locations[i]->x = x + (minWidthRequired+gridXSpacing) * c;
				locations[i]->y = y + (minHeightRequired+gridYSpacing) * r;
			}
			
			maxWidth = (minWidthRequired+gridXSpacing) * min(children.size(), rowSize);
			maxHeight = (minHeightRequired+gridYSpacing) * ((children.size())/rowSize);
		}
		
		if(boundingBox.getLeftBound() == x && boundingBox.getRightBound() == x+maxWidth)
		{
			if(boundingBox.getTopBound() == y && boundingBox.getBottomBound() == y+maxHeight)
			{
				setShouldRedraw(false);
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
		int width = boundingBox.getWidth();
		int height = boundingBox.getHeight();
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();

		graphicsInterface->setColor(backgroundColor);
		graphicsInterface->drawRect(renderX, renderY, renderX+width, renderY+height, false);

		graphicsInterface->setColor(outlineColor);
		graphicsInterface->drawRect(renderX, renderY, renderX+width, renderY+height, true);
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
			for(int i=0; i<children.size(); i++)
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
		int width = boundingBox.getWidth();
		int height = boundingBox.getHeight();
		
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

	void GuiGrid::loadDataFromXML(std::unordered_map<std::wstring, std::wstring>& attribs, GuiGraphicsInterface* inter)
	{
		GuiInstance::loadDataFromXML(attribs, inter);
		std::vector<std::wstring> possibleNames = { L"horizontalspacing", L"verticalspacing", L"rowmajor", L"maxrows", L"maxcolumns", L"backgroundcolor", L"outlinecolor"};

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.find(possibleNames[i]);
			if(it != attribs.end())
			{
				if(it->first == L"horizontalspacing")
				{
					gridXSpacing = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == L"verticalspacing")
				{
					gridYSpacing = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == L"maxrows")
				{
					rowSize = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == L"maxcolumns")
				{
					colSize = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == L"rowmajor")
				{
					rowMajorOrder = StringTools::equalsIgnoreCase<wchar_t>(it->second, L"true");
				}
				else if(it->first == L"backgroundcolor")
				{
					//define as color name or rgba
					backgroundColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(it->first == L"outlinecolor")
				{
					//define as color name or rgba
					outlineColor = ColorNameConverter::NameToColor(it->second);
				}
				
				attribs.erase(possibleNames[i]);
			}
		}
	}

	void GuiGrid::registerLoadFunction()
	{
		GuiManager::registerLoadFunction(L"GuiGrid", GuiGrid::loadFunction);
	}

	GuiInstance* GuiGrid::loadFunction(std::unordered_map<std::wstring, std::wstring>& attributes, GuiGraphicsInterface* inter)
	{
		GuiGrid* ins = new GuiGrid(0,0);
		ins->loadDataFromXML(attributes, inter);
		
		return ins;
	}
	
	#pragma endregion

} //NAMESPACE glib END