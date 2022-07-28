#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_LIST_CLASS

	const Class GuiList::globalClass = Class("GuiList", {&GuiInstance::globalClass});

	GuiList::GuiList(int x, int y, bool isVerticalList)
	{
		setClass(globalClass);
		baseX = x;
		baseY = y;
		isVertical = isVerticalList;
		includeChildrenInBounds = false;
		setPriority(HIGHER_PRIORITY);
	}

	GuiList::~GuiList()
	{
		for(int i=0; i<locations.size(); i++)
		{
			if(locations[i]!=nullptr)
				delete locations[i];
		}
		locations.clear();
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
				c->setRenderOffset(getRenderOffsetPointerX(), getRenderOffsetPointerY());
				Box2D bounds = c->getBoundingBox();


				int disToObjectEdge = bounds.getRightBound() - x;

				width = MathExt::max(disToObjectEdge, width);
				height += bounds.getHeight();
				if(i<children.size()-1)
					height += elementSpacing;
			}
		}
		else
		{
			for(int i=0; i<children.size(); i++)
			{
				locations[i]->x = x + width;
				locations[i]->y = y;

				GuiInstance* c = children[i];
				c->setRenderOffset(getRenderOffsetPointerX(), getRenderOffsetPointerY());
				Box2D bounds = c->getBoundingBox();
				
				int disToObjectEdge = bounds.getBottomBound() - y;

				width += bounds.getWidth();
				if(i<children.size()-1)
					width += elementSpacing;

				height = MathExt::max(disToObjectEdge, height);
			}
		}

		if((int)boundingBox.getLeftBound() == x && (int)boundingBox.getRightBound() == x+width)
		{
			if((int)boundingBox.getTopBound() == y && (int)boundingBox.getBottomBound() == y+height)
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

		boundingBox.setLeftBound(x);
		boundingBox.setRightBound(x+width);
		boundingBox.setTopBound(y);
		boundingBox.setBottomBound(y+height);

	}

	void GuiList::render()
	{
		int width = boundingBox.getWidth();
		int height = boundingBox.getHeight();
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
		
		if(backgroundColor.alpha != 0)
		{
			graphicsInterface->setColor(backgroundColor);
			graphicsInterface->drawRect(x, y, x+width, y+height, false);
		}
		
		if(outlineColor.alpha != 0)
		{
			graphicsInterface->setColor(outlineColor);
			graphicsInterface->drawRect(x, y, x+width, y+height, true);
		}
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
			ins->setRenderOffset(getRenderOffsetPointerX(), getRenderOffsetPointerY());
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

	void GuiList::solveBoundingBox()
	{
		//Call the update function since it only solves for the new bounding box. Also repositions all of the objects below it.
		update();
	}

	void GuiList::loadDataFromXML(std::unordered_map<std::string, std::string>& attribs, GuiGraphicsInterface* inter)
	{
		GuiInstance::loadDataFromXML(attribs, inter);
		std::vector<std::string> possibleNames = { "spacing", "isvertical", "backgroundcolor", "outlinecolor"};

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attribs.find(possibleNames[i]);
			if(it != attribs.end())
			{
				if(it->first == "spacing")
				{
					elementSpacing = std::abs(StringTools::toInt(it->second));
				}
				else if(it->first == "isvertical")
				{
					isVertical = StringTools::equalsIgnoreCase<char>(it->second, "true");
				}
				else if(it->first == "backgroundcolor")
				{
					//define as color name or rgba
					backgroundColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(it->first == "outlinecolor")
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
		GuiManager::registerLoadFunction("GuiList", GuiList::loadFunction);
	}

	GuiInstance* GuiList::loadFunction(std::unordered_map<std::string, std::string>& attributes, GuiGraphicsInterface* inter)
	{
		GuiList* ins = new GuiList(0,0);
		ins->loadDataFromXML(attributes, inter);
		
		return ins;
	}
	
	#pragma endregion

} //NAMESPACE glib END