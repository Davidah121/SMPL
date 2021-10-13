#include "InternalGuiManagerHeader.h"

namespace glib
{

#pragma region GUI_INSTANCE_CLASS

	const Class GuiInstance::myClass = Class("GuiInstance", {&Object::myClass});
	const Class* GuiInstance::getClass()
	{
		return &GuiInstance::myClass;
	}

	GuiInstance::GuiInstance()
	{
		shouldCallA = false;
		shouldCallV = false;
	}

	GuiInstance::GuiInstance(const GuiInstance& other)
	{
		copy(other);
	}

	void GuiInstance::operator=(const GuiInstance& other)
	{
		copy(other);
	}

	void GuiInstance::copy(const GuiInstance& other)
	{
		baseX = other.baseX;
		baseY = other.baseY;

		x = other.x;
		y = other.y;

		renderX = other.renderX;
		renderY = other.renderY;

		priority = other.priority;

		visible = other.visible;
		active = other.active;
		focus = other.focus;
		alwaysFocus = other.alwaysFocus;

		offX = other.offX;
		offY = other.offY;

		renderOffX = other.renderOffX;
		renderOffY = other.renderOffY;
		
		shouldCallA = other.shouldCallA;
		shouldCallV = other.shouldCallV;
		shouldCallA2 = other.shouldCallA2;
		shouldCallV2 = other.shouldCallV2;

		shouldCallF = other.shouldCallF;

		//children = other.children;
		
		if(other.parent!=nullptr)
		{
			other.parent->addChild(this);
		}
		
		for(GuiInstance* child : children)
		{
			child->parent = nullptr;
			addChild(child);
		}

		manager = other.manager;
		canvas = other.canvas;
		
		onFocusFunction = other.onFocusFunction;
		onActivateFunction = other.onActivateFunction;
		onVisibleFunction = other.onVisibleFunction;

		onDeActivateFunction = other.onDeActivateFunction;
		onInvisibleFunction = other.onInvisibleFunction;

		onChangedFunction = other.onChangedFunction;
	}

	GuiInstance::~GuiInstance()
	{
		if(parent!=nullptr)
		{
			parent->removeChild(this);
		}
		if(manager!=nullptr)
		{
			manager->deleteElement(this);
		}

		for(GuiInstance* ins : children)
		{
			ins->parent = nullptr;
			ins->setOffset(nullptr, nullptr);
		}

		parent = nullptr;
		children.clear();
		manager = nullptr;

		renderOffX = nullptr;
		renderOffY = nullptr;

		offX = nullptr;
		offY = nullptr;
	}

	void GuiInstance::update()
	{
	}

	void GuiInstance::render(Image* surf)
	{
	}

	void GuiInstance::addChild(GuiInstance* ins)
	{
		if(ins->parent!=nullptr)
		{
			ins->parent->removeChild(ins);
		}
		
		ins->parent = this;
		ins->updatePriority();
		ins->setOffset( &x, &y );
		children.push_back(ins);

		if(manager!=nullptr && manager!=ins->manager)
		{
			//add to manager
			manager->addElement(ins);
		}
	}

	void GuiInstance::removeChild(GuiInstance* ins)
	{
		std::vector<GuiInstance*> m = std::vector<GuiInstance*>();
		for(int i=0; i<children.size(); i++)
		{
			if(children[i] != ins)
			{
				m.push_back(children[i]);
			}
		}

		children = m;

		ins->parent = nullptr;
	}

	std::vector<GuiInstance*> GuiInstance::getChildren()
	{
		return children;
	}

	void GuiInstance::setVisible(bool is)
	{
		visible = is;
		if (visible)
		{
			shouldCallV = true;
			shouldCallV2 = false;
		}
		else
		{
			shouldCallV2 = true;
			shouldCallV = false;
		}
	}

	bool GuiInstance::getVisible()
	{
		bool finalVisibility = (parent!=nullptr) ? (visible && parent->getVisible()) : visible;
		return finalVisibility;
	}

	void GuiInstance::setActive(bool is)
	{
		active = is;
		if (active)
		{
			shouldCallA = true;
			shouldCallA2 = false;
		}
		else
		{
			shouldCallA2 = true;
			shouldCallA = false;
		}
	}

	bool GuiInstance::getActive()
	{
		bool finalActivity = (parent!=nullptr) ? (active && parent->getActive()) : active;
		return finalActivity;
	}

	void GuiInstance::setFocus(bool is)
	{
		if(!alwaysFocus)
		{
			if(!focus && is)
			{
				shouldCallF = true;
			}
			focus = is;
		}
		else
		{
			focus = true;
		}
	}

	bool GuiInstance::getFocus()
	{
		return focus;
	}

	void GuiInstance::setAlwaysFocus(bool is)
	{
		alwaysFocus = is;
		setFocus( getFocus() ); //Will set to true if alwaysFocus is true.
	}

	bool GuiInstance::getAlwaysFocus()
	{
		return alwaysFocus;
	}

	int GuiInstance::getOffsetX()
	{
		if(offX!=nullptr)
			return *offX;
		return 0;
	}

	int GuiInstance::getOffsetY()
	{
		if(offY!=nullptr)
			return *offY;
		return 0;
	}

	int GuiInstance::getRenderOffsetX()
	{
		if(renderOffX!=nullptr)
			return *renderOffX;
		return 0;
	}

	int GuiInstance::getRenderOffsetY()
	{
		if(renderOffY!=nullptr)
			return *renderOffY;
		return 0;
	}

	void GuiInstance::setBaseX(int x)
	{
		baseX = x;
	}

	int GuiInstance::getBaseX()
	{
		return baseX;
	}

	void GuiInstance::setBaseY(int y)
	{
		baseY = y;
	}

	int GuiInstance::getBaseY()
	{
		return baseY;
	}

	int GuiInstance::getX()
	{
		return x;
	}

	int GuiInstance::getY()
	{
		return y;
	}

	void GuiInstance::setOnActivateFunction(std::function<void(GuiInstance*)> func)
	{
		onActivateFunction = func;
	}

	void GuiInstance::setOnVisibleFunction(std::function<void(GuiInstance*)> func)
	{
		onVisibleFunction = func;
	}

	void GuiInstance::setOnDeActivateFunction(std::function<void(GuiInstance*)> func)
	{
		onDeActivateFunction = func;
	}

	void GuiInstance::setOnInvisibleFunction(std::function<void(GuiInstance*)> func)
	{
		onInvisibleFunction = func;
	}

	void GuiInstance::setOnChangedFunction(std::function<void(GuiInstance*)> func)
	{
		onChangedFunction = func;
	}

	void GuiInstance::setOnFocusFunction(std::function<void(GuiInstance*)> func)
	{
		onFocusFunction = func;
	}

	void GuiInstance::setOffset(int* offX, int* offY)
	{
		this->offX = offX;
		this->offY = offY;
	}

	void GuiInstance::setRenderOffset(int* offX, int* offY)
	{
		this->renderOffX = offX;
		this->renderOffY = offY;
	}

	const void GuiInstance::baseRender()
	{
		renderX = baseX + getOffsetX() - getRenderOffsetX();
		renderY = baseY + getOffsetY() - getRenderOffsetY();
	}

	const void GuiInstance::baseUpdate()
	{
		x = getOffsetX() + baseX;
		y = getOffsetY() + baseY;

		if (shouldCallA)
			if(onActivateFunction!=nullptr)
				onActivateFunction(this);

		if (shouldCallV)
			if(onVisibleFunction!=nullptr)
				onVisibleFunction(this);
		
		if (shouldCallA2)
			if(onDeActivateFunction!=nullptr)
				onDeActivateFunction(this);

		if (shouldCallV2)
			if(onInvisibleFunction!=nullptr)
				onInvisibleFunction(this);

		if(shouldCallF)
			if(onFocusFunction!=nullptr)
				onFocusFunction(this);
		
		shouldCallA = false;
		shouldCallV = false;

		shouldCallA2 = false;
		shouldCallV2 = false;

		shouldCallF = false;
	}

	int GuiInstance::getPriority()
	{
		return priority;
	}

	void GuiInstance::setPriority(int value)
	{
		if(value!=CANVAS_PRIORITY_VALUE)
		{
			priority = MathExt::clamp(value, GuiInstance::MIN_PRIORITY_VALUE, GuiInstance::MAX_PRIORITY_VALUE);
		}
		else
		{
			priority = MathExt::abs(GuiInstance::CANVAS_PRIORITY_VALUE);
		}
		
		for(GuiInstance* child : children)
		{
			child->updatePriority();
		}
	}

	void GuiInstance::updatePriority()
	{
		if(parent!=nullptr)
			priority = max(priority, parent->priority+1);
		
		for(GuiInstance* insChildren : children)
		{
			insChildren->updatePriority();
		}
	}

	void GuiInstance::setManager(GuiManager* m)
	{
		manager = m;
	}

	GuiManager* GuiInstance::getManager()
	{
		return manager;
	}

	void GuiInstance::setCanvas(Image* m)
	{
		canvas = m;
	}

	Image* GuiInstance::getCanvas()
	{
		return canvas;
	}

	#pragma endregion
    
} //NAMESPACE glib END