#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_LIST_CLASS

	const Class GuiList::myClass = Class("GuiList", {&GuiInstance::myClass});
	const Class* GuiList::getClass()
	{
		return &GuiList::myClass;
	}

	GuiList::GuiList(int x, int y)
	{
		baseX = x;
		baseY = y;
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
		for(int i=0; i<children.size(); i++)
		{
			locations[i]->x = x;
			locations[i]->y = y + i*elementSpacing;
		}
	}

	void GuiList::render(Image* surf)
	{
		int height = locations.size()*elementSpacing;

		SimpleGraphics::setColor(backgroundColor);
		surf->drawRect(renderX, renderY, renderX+width, renderY+height, false);
		
		SimpleGraphics::setColor(outlineColor);
		surf->drawRect(renderX, renderY, renderX+width, renderY+height, true);
	}

	void GuiList::setElementSpacing(int value)
	{
		elementSpacing = value;	
	}

	void GuiList::addElement(GuiInstance* ins)
	{
		if(ins!=nullptr)
		{
			Point* p = new Point();

			locations.push_back( p );
			addChild(ins);
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
	
	bool GuiList::pointIsInList(int x, int y)
	{
		if(x >= this->x && x <= this->x + width)
		{
			if(y >= this->y && y <= this->y + locations.size()*elementSpacing)
			{
				return true;
			}
		}
		return false;
	}

	void GuiList::setExpectedWidth(int w)
	{
		width = w;
	}

	void GuiList::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}

	void GuiList::setOutlineColor(Color c)
	{
		outlineColor = c;
	}
	
	#pragma endregion

} //NAMESPACE glib END