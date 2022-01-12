#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_LIST_CLASS

	const Class GuiList::myClass = Class("GuiList", {&GuiInstance::myClass});
	const Class* GuiList::getClass()
	{
		return &GuiList::myClass;
	}

	GuiList::GuiList(int x, int y, bool isVerticalList)
	{
		baseX = x;
		baseY = y;
		isVertical = isVerticalList;
	}

	GuiList::~GuiList()
	{
		for(int i=0; i<locations.size(); i++)
		{
			if(locations[i]!=nullptr)
				delete locations[i];
		}
	}

	void GuiList::update()
	{
		std::vector<GuiInstance*> children = getChildren();
		int height = 0;
		int width = 0;

		if(isVertical)
		{
			for(int i=0; i<children.size(); i++)
			{
				locations[i]->x = x;
				locations[i]->y = y + height;

				GuiInstance* c = children[i];

				int disToObjectEdge = c->getBoundingBox().getRightBound() - x;

				width = max(disToObjectEdge, width);
				height += elementSpacing + c->getBoundingBox().getHeight();
			}
		}
		else
		{
			for(int i=0; i<children.size(); i++)
			{
				locations[i]->x = x + width;
				locations[i]->y = y;

				GuiInstance* c = children[i];
				
				int disToObjectEdge = c->getBoundingBox().getBottomBound() - y;

				width += elementSpacing + c->getBoundingBox().getWidth();
				height = max(disToObjectEdge, height);
			}
		}

		if(boundingBox.getLeftBound() == x && boundingBox.getRightBound() == x+width)
		{
			if(boundingBox.getTopBound() == y && boundingBox.getBottomBound() == y+height)
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

		boundingBox.setLeftBound(x);
		boundingBox.setRightBound(x+width);
		boundingBox.setTopBound(y);
		boundingBox.setBottomBound(y+height);

	}

	void GuiList::render(Image* surf)
	{
		int width = boundingBox.getWidth();
		int height = boundingBox.getHeight();
		
		SimpleGraphics::setColor(backgroundColor);
		surf->drawRect(renderX, renderY, renderX+width, renderY+height, false);
		
		SimpleGraphics::setColor(outlineColor);
		surf->drawRect(renderX, renderY, renderX+width, renderY+height, true);
	}

	void GuiList::setElementSpacing(int value)
	{
		elementSpacing = value;	
	}
	
	bool GuiList::pointIsInList(int x, int y)
	{
		if(x >= this->x && x <= this->x + (boundingBox.getRightBound()-boundingBox.getLeftBound()) )
		{
			if(y >= this->y && y <= this->y + (boundingBox.getBottomBound()-boundingBox.getTopBound()))
			{
				return true;
			}
		}
		return false;
	}

	void GuiList::addElement(GuiInstance* ins)
	{
		if(ins!=nullptr)
		{
			Point* p = new Point();

			locations.push_back( p );
			GuiInstance::addChild(ins);
			ins->setOffset( &p->x, &p->y );
		}
	}

	void GuiList::removeElement(GuiInstance* ins)
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
	
	void GuiList::addChild(GuiInstance* ins)
	{
		addElement(ins);
	}

	void GuiList::removeChild(GuiInstance* ins)
	{
		removeElement(ins);
	}

	void GuiList::setBackgroundColor(Color c)
	{
		backgroundColor = c;
		setShouldRedraw(true);
	}

	void GuiList::setOutlineColor(Color c)
	{
		outlineColor = c;
		setShouldRedraw(true);
	}

	void GuiList::setIsVerticalList(bool v)
	{
		isVertical = v;
		setShouldRedraw(true);
	}

	bool GuiList::getIsVerticalList()
	{
		return isVertical;
	}

	void GuiList::loadDataFromXML(std::unordered_map<std::wstring, std::wstring>& attribs)
	{
		GuiInstance::loadDataFromXML(attribs);
		std::vector<std::wstring> possibleNames = { L"spacing", L"isvertical", L"backgroundcolor", L"outlinecolor"};

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.find(possibleNames[i]);
			if(it != attribs.end())
			{
				if(it->first == L"spacing")
				{
					elementSpacing = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == L"isvertical")
				{
					isVertical = StringTools::equalsIgnoreCase<wchar_t>(it->second, L"true");
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

	void GuiList::registerLoadFunction()
	{
		GuiManager::registerLoadFunction(L"GuiList", GuiList::loadFunction);
	}

	GuiInstance* GuiList::loadFunction(std::unordered_map<std::wstring, std::wstring>& attributes)
	{
		GuiList* ins = new GuiList(0,0);
		ins->loadDataFromXML(attributes);
		
		return ins;
	}
	
	#pragma endregion

} //NAMESPACE glib END