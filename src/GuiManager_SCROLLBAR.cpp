#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_SCROLL_BAR

	const Class GuiScrollBar::myClass = Class("GuiScrollBar", {&GuiScrollBar::myClass});
	const Class* GuiScrollBar::getClass()
	{
		return &GuiScrollBar::myClass;
	}

	GuiScrollBar::GuiScrollBar(int startX, int startY, int endX, int endY) : GuiInstance()
	{
		setBaseX(x);
		setBaseY(y);
		
		//must satisfy the min width and height
		this->startX = startX;
		this->startY = startY;
		this->endX = endX;
		this->endY = endY;
		
		onSlideFunction = nullptr;

		int width = this->endX-this->startX;
		int height = this->endY-this->startY;

		int buttonWidth = max(width, minWidth);
		int buttonHeight = max(height, minHeight);


		//location of button center == (startX+endX)/2
		//startButX = startX + ??
		//startButX2 = startX + ?? + buttonWidth

		int butX = startX;
		int butY = startY;

		if(isHorizontal)
		{
			if(buttonHeight != height)
			{
				butY -= buttonHeight/2;
			}
		}
		else
		{
			if(buttonWidth != width)
			{
				butX -= buttonWidth/2;
			}
		}

		buttonElement = GuiRectangleButton(butX, butY, buttonWidth, buttonHeight);

		std::function<void(GuiInstance*)> f = [this](GuiInstance* a) -> void {this->holdButtonFunction(a);};
		std::function<void(GuiInstance*)> f2 = [this](GuiInstance* a) -> void { this->setFocus(true);};
		std::function<void(GuiInstance*)> f3 = [this](GuiInstance* a) -> void { this->setFocus(false);};

		buttonElement.setOnClickHoldFunction( f );
		buttonElement.setOnActivateFunction( f2 );
		buttonElement.setOnDeActivateFunction( f3 );
		addChild( &buttonElement );

		std::function<void(GuiInstance*)> scrollDecrease = [this](GuiInstance* a) -> void{
			this->decreaseScroll();
		};

		std::function<void(GuiInstance*)> scrollIncrease = [this](GuiInstance* a) -> void{
			this->increaseScroll();
		};
		
		if(isHorizontal)
		{
			decreaseButtonElement = GuiRectangleButton(this->startX-buttonHeight, butY, buttonHeight, buttonHeight);
			increaseButtonElement = GuiRectangleButton(this->endX, butY, buttonHeight, buttonHeight);
		}
		else
		{
			decreaseButtonElement = GuiRectangleButton(butX, this->startY-buttonWidth, buttonWidth, buttonWidth);
			increaseButtonElement = GuiRectangleButton(butX, this->endY, buttonWidth, buttonWidth);
		}

		decreaseButtonElement.setOnClickFunction(scrollDecrease);
		increaseButtonElement.setOnClickFunction(scrollIncrease);
		
		addChild( &decreaseButtonElement );
		addChild( &increaseButtonElement );
	}

	GuiScrollBar::GuiScrollBar(const GuiScrollBar& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiScrollBar::operator=(const GuiScrollBar& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiScrollBar::copy(const GuiScrollBar& other)
	{
		startX = other.startX;
		endX = other.endX;
		startY = other.startY;
		endY = other.endY;

		steps = other.steps;
		currStep = other.currStep;

		minWidth = other.minWidth;
		minHeight = other.minHeight;

		isHorizontal = other.isHorizontal;
		showScrollButtons = other.showScrollButtons;

		//holdButtonFunction variables
		onSlideFunction = other.onSlideFunction;
		preMouseX = other.preMouseX;
		preMouseY = other.preMouseY;
		preButtonX = other.preButtonX;
		preButtonY = other.preButtonY;

		backgroundColor = other.backgroundColor;
		outlineColor = other.outlineColor;

		buttonElement = other.buttonElement;

		decreaseButtonElement = other.decreaseButtonElement;
		increaseButtonElement = other.increaseButtonElement;

		//Recreate functions for the button elements to refer to this object and not the other
		std::function<void(GuiInstance*)> f = [this](GuiInstance* a) -> void {this->holdButtonFunction(a);};
		std::function<void(GuiInstance*)> f2 = [this](GuiInstance* a) -> void { this->setFocus(true);};
		std::function<void(GuiInstance*)> f3 = [this](GuiInstance* a) -> void { this->setFocus(false);};

		buttonElement.setOnClickHoldFunction( f );
		buttonElement.setOnActivateFunction( f2 );
		buttonElement.setOnDeActivateFunction( f3 );

		std::function<void(GuiInstance*)> scrollDecrease = [this](GuiInstance* a) -> void{
			this->decreaseScroll();
		};

		std::function<void(GuiInstance*)> scrollIncrease = [this](GuiInstance* a) -> void{
			this->increaseScroll();
		};

		decreaseButtonElement.setOnClickFunction(scrollDecrease);
		increaseButtonElement.setOnClickFunction(scrollIncrease);

		addChild(&buttonElement);
		addChild(&decreaseButtonElement);
		addChild(&increaseButtonElement);
	}

	GuiScrollBar::~GuiScrollBar()
	{
	}

	void GuiScrollBar::decreaseScroll()
	{
		setCurrentStep( currStep - 1 );
	}

	void GuiScrollBar::increaseScroll()
	{
		setCurrentStep( currStep + 1 );

	}

	void GuiScrollBar::update()
	{
		if(buttonElement.getFocus() || increaseButtonElement.getFocus() || decreaseButtonElement.getFocus())
		{
			if(!getFocus())
				setFocus(true);
		}
		else
		{
			if(getFocus())
				setFocus(false);
		}

		if(getFocus())
		{
			if(isHorizontal)
			{
				if(Input::getKeyPressed(VK_LEFT) || Input::getMouseScrollVertical() > 0 || Input::getMouseScrollHorizontal() < 0)
				{
					decreaseScroll();
				}
				else if (Input::getKeyPressed(VK_RIGHT) || Input::getMouseScrollVertical() < 0 || Input::getMouseScrollHorizontal() > 0)
				{
					increaseScroll();
				}
			}
			else
			{
				if(Input::getKeyPressed(VK_UP) || Input::getMouseScrollVertical() > 0)
				{
					decreaseScroll();
				}
				else if (Input::getKeyPressed(VK_DOWN) || Input::getMouseScrollVertical() < 0)
				{
					increaseScroll();
				}
			}
		}
	}

	void GuiScrollBar::holdButtonFunction(GuiInstance* ins)
	{
		//ins is the rectangle button. Can be disregarded.
		int mouseX = Input::getMouseX();
		int mouseY = Input::getMouseY();
		
		if(getManager()!=nullptr)
		{
			mouseX -= getManager()->getWindowX();
			mouseY -= getManager()->getWindowY();
		}

		if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
		{
			preMouseX = mouseX;
			preMouseY = mouseY;
			preButtonX = buttonElement.getBaseX();
			preButtonY = buttonElement.getBaseY();
		}

		mouseX -= preMouseX;
		mouseY -= preMouseY;

		int nSX = startX + x;
		int nSY = startY + y;
		int nEX = endX + x;
		int nEY = endY + y;

		int oldStep = currStep;

		if(!isHorizontal)
		{
			//adjust so that the end of the button does not pass the bounds
			int butHeight = buttonElement.getHeight();

			//determine button position
			int butY = MathExt::clamp(preButtonY + mouseY, nSY, nEY-butHeight);
			buttonElement.setBaseY( butY );

			//determine current step
			double percentage = (double)(butY - nSY) / (nEY - nSY - butHeight);
			double t = percentage*steps;

			currStep = MathExt::clamp((int)MathExt::round(t), 0, steps);
		}
		else
		{
			//adjust so that the end of the button does not pass the bounds
			int butWidth = buttonElement.getWidth();

			//determine button position
			int butX = MathExt::clamp(preButtonX + mouseX, nSX, nEX-butWidth);
			buttonElement.setBaseX( butX );

			//determine current step
			double percentage = (double)(butX - nSX) / (nEX - nSX - butWidth);
			double t = percentage*steps;

			currStep = MathExt::clamp((int)MathExt::round(t), 0, steps);
		}

		if(oldStep!=currStep)
		{
			if(onSlideFunction!=nullptr)
			{
				onSlideFunction(this);
			}
		}
	}

	void GuiScrollBar::render(Image* surf)
	{
		if(surf!=nullptr)
		{
			//draw the background bar
			Graphics::setColor(backgroundColor);
			surf->drawRect(renderX+startX, renderY+startY, renderX+endX, renderY+endY, false);

			Graphics::setColor(outlineColor);
			surf->drawRect(renderX+startX, renderY+startY, renderX+endX, renderY+endY, true);
		}
	}

	void GuiScrollBar::setHorizontalBar(bool v)
	{
		isHorizontal = v;

		int width = this->endX-this->startX;
		int height = this->endY-this->startY;

		int buttonWidth = max(width, minWidth);
		int buttonHeight = max(height, minHeight);

		if(isHorizontal)
		{
			int nY = startY - ((buttonHeight/2) - (height/2));
			buttonElement.setBaseY(nY);

			decreaseButtonElement.setBaseX(this->startX-buttonHeight);
			decreaseButtonElement.setBaseY(this->startY-buttonHeight/2);
			decreaseButtonElement.setWidth(buttonHeight);
			decreaseButtonElement.setHeight(buttonHeight);
			
			increaseButtonElement.setBaseX(this->endX);
			increaseButtonElement.setBaseY(this->startY-buttonHeight/2);
			increaseButtonElement.setWidth(buttonHeight);
			increaseButtonElement.setHeight(buttonHeight);
		}
		else
		{
			int nX = startX - ((buttonWidth/2) - (width/2));
			buttonElement.setBaseX(nX);

			decreaseButtonElement.setBaseX(nX);
			decreaseButtonElement.setBaseY(this->startY-buttonWidth);
			decreaseButtonElement.setWidth(buttonWidth);
			decreaseButtonElement.setHeight(buttonWidth);
			
			increaseButtonElement.setBaseX(nX);
			increaseButtonElement.setBaseY(this->endY);
			increaseButtonElement.setWidth(buttonWidth);
			increaseButtonElement.setHeight(buttonWidth);
		}
	}

	bool GuiScrollBar::getHorizontalBar()
	{
		return isHorizontal;
	}

	void GuiScrollBar::setShowScrollButtons(bool v)
	{
		decreaseButtonElement.setVisible(v);
		increaseButtonElement.setVisible(v);
		showScrollButtons = v;
	}

	bool GuiScrollBar::getShowScrollButtons()
	{
		return showScrollButtons;
	}

	void GuiScrollBar::setOnSlideFunction(std::function<void(GuiInstance*)> func)
	{
		onSlideFunction = func;
	}

	void GuiScrollBar::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}

	Color GuiScrollBar::getBackgroundColor()
	{
		return backgroundColor;
	}

	void GuiScrollBar::setOutlineColor(Color c)
	{
		outlineColor = c;
	}

	Color GuiScrollBar::getOutlineColor()
	{
		return outlineColor;
	}

	void GuiScrollBar::setStartX(int x)
	{
		startX = x;
	}

	int GuiScrollBar::getStartX()
	{
		return startX;
	}

	void GuiScrollBar::setStartY(int y)
	{
		startY = y;
	}

	int GuiScrollBar::getStartY()
	{
		return startY;
	}

	void GuiScrollBar::setEndX(int x)
	{
		endX = x;
	}

	int GuiScrollBar::getEndX()
	{
		return endX;
	}

	void GuiScrollBar::setEndY(int y)
	{
		endY = y;
	}

	int GuiScrollBar::getEndY()
	{
		return endY;
	}

	void GuiScrollBar::setMinWidth(int w)
	{
		minWidth = w;
	}

	int GuiScrollBar::getMinWidth()
	{
		return minWidth;
	}

	void GuiScrollBar::setMinHeight(int h)
	{
		minHeight = h;
	}

	int GuiScrollBar::getMinHeight()
	{
		return minHeight;
	}

	void GuiScrollBar::setSteps(int s)
	{
		steps = max(s, 0);
		setCurrentStep( MathExt::clamp(currStep, 0, steps) );

		if(isHorizontal)
		{
			int width = (endX-startX);
			
			width = (int)MathExt::round((double)width/(steps+1));

			buttonElement.setWidth( max(width, minWidth) );
		}
		else
		{
			int height = (endY-startY);

			height = (int)MathExt::round((double)height/(steps+1));

			buttonElement.setHeight( max(height, minHeight) );
		}
	}

	int GuiScrollBar::getSteps()
	{
		return steps;
	}

	void GuiScrollBar::setCurrentStep(int s)
	{
		int oldStep = currStep;
		currStep = MathExt::clamp(s, 0, steps);

		if(isHorizontal)
		{
			//adjust so that the end of the button does not pass the bounds
			int butWidth = buttonElement.getWidth();

			//determine percentage
			double percentage = (double)currStep/steps;
			double t = percentage*(endX-startX-butWidth) + startX;

			int xPos = MathExt::clamp((int)MathExt::round(t), startX, endX-butWidth);

			buttonElement.setBaseX( xPos );
		}
		else
		{
			//adjust so that the end of the button does not pass the bounds
			int butHeight = buttonElement.getHeight();

			//determine percentage
			double percentage = (double)currStep/steps;
			double t = percentage*(endY-startY-butHeight) + startY;

			int yPos = MathExt::clamp((int)MathExt::round(t), startY, endY-butHeight);

			buttonElement.setBaseY( yPos );
		}

		if(oldStep!=currStep)
		{
			if(onSlideFunction!=nullptr)
			{
				onSlideFunction(this);
			}
		}
	}

	int GuiScrollBar::getCurrentStep()
	{
		return currStep;
	}

	GuiRectangleButton* GuiScrollBar::getButtonElement()
	{
		return &buttonElement;
	}

	GuiRectangleButton* GuiScrollBar::getDecreaseButtonElement()
	{
		return &decreaseButtonElement;
	}

	GuiRectangleButton* GuiScrollBar::getIncreaseButtonElement()
	{
		return &increaseButtonElement;
	}

	#pragma endregion

} //NAMESPACE glib END