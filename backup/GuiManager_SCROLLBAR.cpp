#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_SCROLL_BAR

	const Class GuiScrollBar::globalClass = Class("GuiScrollBar", {&GuiScrollBar::globalClass});

	GuiScrollBar::GuiScrollBar(int startX, int startY, int endX, int endY) : GuiInstance()
	{
		setClass(globalClass);
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

		int buttonWidth = MathExt::max(width, minWidth);
		int buttonHeight = MathExt::max(height, minHeight);


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

		buttonElement.setBackgroundColor(buttonColor);
		decreaseButtonElement.setBackgroundColor(buttonColor);
		increaseButtonElement.setBackgroundColor(buttonColor);
		
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
		setClass(globalClass);
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
		buttonColor = other.buttonColor;
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

		buttonElement.setBackgroundColor(buttonColor);
		decreaseButtonElement.setBackgroundColor(buttonColor);
		increaseButtonElement.setBackgroundColor(buttonColor);

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
		bool oldFocus = getFocus();

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
				if(Input::getKeyPressed(Input::KEY_LEFT) || Input::getMouseScrollVertical() > 0 || Input::getMouseScrollHorizontal() < 0)
				{
					decreaseScroll();
				}
				else if (Input::getKeyPressed(Input::KEY_RIGHT) || Input::getMouseScrollVertical() < 0 || Input::getMouseScrollHorizontal() > 0)
				{
					increaseScroll();
				}
			}
			else
			{
				if(Input::getKeyPressed(Input::KEY_UP) || Input::getMouseScrollVertical() > 0)
				{
					decreaseScroll();
				}
				else if (Input::getKeyPressed(Input::KEY_DOWN) || Input::getMouseScrollVertical() < 0)
				{
					increaseScroll();
				}
			}
		}

		if(getFocus() != oldFocus)
			setShouldRedraw(true);

		boundingBox = Box2D(startX, startY, endX, endY);
	}

	void GuiScrollBar::holdButtonFunction(GuiInstance* ins)
	{
		//ins is the rectangle button. Can be disregarded.
		int mouseX = 0;
		int mouseY = 0;
		
		if(getManager()!=nullptr)
		{
			mouseX = getManager()->getMouseX();
			mouseY = getManager()->getMouseY();
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
			setShouldRedraw(true);
		}
	}

	void GuiScrollBar::render()
	{
		//draw the background bar
		GraphicsInterface::setColor(backgroundColor);
		GraphicsInterface::drawRect(x+startX, y+startY, x+endX, y+endY, false);

		GraphicsInterface::setColor(outlineColor);
		GraphicsInterface::drawRect(x+startX, y+startY, x+endX, y+endY, true);
		
	}

	void GuiScrollBar::setHorizontalBar(bool v)
	{
		isHorizontal = v;

		int width = this->endX-this->startX;
		int height = this->endY-this->startY;

		int buttonWidth = MathExt::max(width, minWidth);
		int buttonHeight = MathExt::max(height, minHeight);

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
		setShouldRedraw(true);
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
		setShouldRedraw(true);
	}

	Color GuiScrollBar::getBackgroundColor()
	{
		return backgroundColor;
	}

	void GuiScrollBar::setOutlineColor(Color c)
	{
		outlineColor = c;
		setShouldRedraw(true);
	}

	Color GuiScrollBar::getOutlineColor()
	{
		return outlineColor;
	}

	void GuiScrollBar::setStartX(int x)
	{
		startX = x;
		setShouldRedraw(true);
	}

	int GuiScrollBar::getStartX()
	{
		return startX;
	}

	void GuiScrollBar::setStartY(int y)
	{
		startY = y;
		setShouldRedraw(true);
	}

	int GuiScrollBar::getStartY()
	{
		return startY;
	}

	void GuiScrollBar::setEndX(int x)
	{
		endX = x;
		setShouldRedraw(true);
	}

	int GuiScrollBar::getEndX()
	{
		return endX;
	}

	void GuiScrollBar::setEndY(int y)
	{
		endY = y;
		setShouldRedraw(true);
	}

	int GuiScrollBar::getEndY()
	{
		return endY;
	}

	void GuiScrollBar::setMinWidth(int w)
	{
		minWidth = w;
		setShouldRedraw(true);
	}

	int GuiScrollBar::getMinWidth()
	{
		return minWidth;
	}

	void GuiScrollBar::setMinHeight(int h)
	{
		minHeight = h;
		setShouldRedraw(true);
	}

	int GuiScrollBar::getMinHeight()
	{
		return minHeight;
	}

	void GuiScrollBar::setSteps(int s)
	{
		steps = MathExt::max(s, 0);
		setCurrentStep( MathExt::clamp(currStep, 0, steps) );

		if(isHorizontal)
		{
			int width = (endX-startX);
			
			width = (int)MathExt::round((double)width/(steps+1));

			buttonElement.setWidth( MathExt::max(width, minWidth) );
		}
		else
		{
			int height = (endY-startY);

			height = (int)MathExt::round((double)height/(steps+1));

			buttonElement.setHeight( MathExt::max(height, minHeight) );
		}

		setShouldRedraw(true);
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
			setShouldRedraw(true);
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

	void GuiScrollBar::solveBoundingBox()
	{
		//Shouldn't have children at all but it will be allowed.
		boundingBox = Box2D(startX, startY, endX, endY);
	}

	#pragma endregion

} //NAMESPACE glib END