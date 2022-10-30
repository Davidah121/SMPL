#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_LIST_CLASS

	const Class GuiList::globalClass = Class("GuiList", {&GuiInstance::globalClass});

	GuiList::GuiList(int x, int y, bool isVerticalList) : GuiInstance()
	{
		setClass(globalClass);
		setBaseX(x);
		setBaseY(y);
		isVertical = isVerticalList;
		includeChildrenInBounds = false;
		setPriority(HIGHER_PRIORITY);
	}

	GuiList::GuiList(const GuiList& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiList::operator=(const GuiList& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiList::copy(const GuiList& other)
	{
		setClass(globalClass);
		setBaseX(other.x);
		setBaseY(other.y);
		isVertical = other.isVertical;
		includeChildrenInBounds = false;
		elementSpacing = other.elementSpacing;
		backgroundColor = other.backgroundColor;
		outlineColor = other.outlineColor;
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
		Box2D oldBounds = boundingBox;
		boundingBox = Box2D(x, y, x, y); //Set the bounds to be at the x and y position
		if(!getVisible())
		{
			//Don't need to do anything
			return;
		}

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
				if(c->getVisible())
				{
					c->setRenderOffset(getRenderOffsetPointerX(), getRenderOffsetPointerY());
					Box2D bounds = c->getBoundingBox();

					boundingBox.setLeftBound( MathExt::min(boundingBox.getLeftBound(), (double)x+c->getBaseX()) );
					boundingBox.setRightBound( MathExt::max(boundingBox.getRightBound(), (double)x+c->getBaseX()+bounds.getWidth()) );
					boundingBox.setTopBound( MathExt::min(boundingBox.getTopBound(), (double)y+c->getBaseY()) );
					boundingBox.setBottomBound( MathExt::max(boundingBox.getBottomBound(), (double)y+c->getBaseY()+bounds.getHeight()+height) );
					
					height = boundingBox.getHeight();
					if(i<children.size()-1)
						height += elementSpacing;
				}
			}
		}
		else
		{
			for(int i=0; i<children.size(); i++)
			{
				locations[i]->x = x + width;
				locations[i]->y = y;

				GuiInstance* c = children[i];

				if(c->getVisible())
				{
					c->setRenderOffset(getRenderOffsetPointerX(), getRenderOffsetPointerY());
					Box2D bounds = c->getBoundingBox();
					
					boundingBox.setLeftBound( MathExt::min(boundingBox.getLeftBound(), (double)x+c->getBaseX()) );
					boundingBox.setRightBound( MathExt::max(boundingBox.getRightBound(), (double)x+c->getBaseX()+bounds.getWidth()+width) );
					boundingBox.setTopBound( MathExt::min(boundingBox.getTopBound(), (double)y+c->getBaseY()) );
					boundingBox.setBottomBound( MathExt::max(boundingBox.getBottomBound(), (double)y+c->getBaseY()+bounds.getHeight()) );
					
					width += boundingBox.getWidth();
					if(i<children.size()-1)
						width += elementSpacing;
				}
			}
		}

		if((int)oldBounds.getLeftBound() == x && (int)oldBounds.getRightBound() == x+width)
		{
			if((int)oldBounds.getTopBound() == y && (int)oldBounds.getBottomBound() == y+height)
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
	}

	void GuiList::render()
	{
		int x1 = boundingBox.getLeftBound();
		int y1 = boundingBox.getTopBound();
		
		int x2 = boundingBox.getRightBound();
		int y2 = boundingBox.getBottomBound();

		if(x1>x2 || y1>y2)
			return;
		
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
		
		if(backgroundColor.alpha != 0)
		{
			graphicsInterface->setColor(backgroundColor);
			graphicsInterface->drawRect(x1, y1, x2, y2, false);
		}
		
		if(outlineColor.alpha != 0)
		{
			graphicsInterface->setColor(outlineColor);
			graphicsInterface->drawRect(x1, y1, x2, y2, true);
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