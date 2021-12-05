#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_CIRCLE_BUTTON

	const Class GuiCircleButton::myClass = Class("GuiCircleButton", {&GuiInstance::myClass});
	const Class* GuiCircleButton::getClass()
	{
		return &GuiCircleButton::myClass;
	}

	GuiCircleButton::GuiCircleButton(int x, int y, int radius) : GuiInstance()
	{
		setBaseX(x);
		setBaseY(y);
		this->radius = radius;
		onClickFunction = nullptr;
	}

	GuiCircleButton::GuiCircleButton(const GuiCircleButton& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiCircleButton::operator=(const GuiCircleButton& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiCircleButton::copy(const GuiCircleButton& other)
	{
		onClickFunction = other.onClickFunction;
		onClickHoldFunction = other.onClickHoldFunction;

		radius = other.radius;

		backgroundColor = other.backgroundColor;
		outlineColor = other.outlineColor;
		focusOutlineColor = other.focusOutlineColor;
	}


	GuiCircleButton::~GuiCircleButton()
	{
	}

	void GuiCircleButton::update()
	{
		int mouseX = Input::getMouseX();
		int mouseY = Input::getMouseY();
		
		if(getManager()!=nullptr)
		{
			mouseX -= getManager()->getWindowX();
			mouseY -= getManager()->getWindowY();
		}

		if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
		{
			setFocus(false);
			double radSqr = MathExt::sqr(radius);
			double otherRad = MathExt::sqr((double)mouseX - x) + MathExt::sqr((double)mouseY - y);

			if (otherRad <= radSqr)
			{
				setFocus(true);
				if (onClickFunction != nullptr)
					onClickFunction(this);
			}
		}

		if(Input::getMouseDown(Input::LEFT_MOUSE_BUTTON) && getFocus())
		{
			if (onClickHoldFunction != nullptr)
				onClickHoldFunction(this);
		}
	}

	void GuiCircleButton::render(Image* surf)
	{
		if(surf!=nullptr)
		{
			//draw a circle
			SimpleGraphics::setColor(backgroundColor);
			surf->drawCircle(renderX, renderY, radius, false);

			if (getFocus() == false)
				SimpleGraphics::setColor(outlineColor);
			else
				SimpleGraphics::setColor(focusOutlineColor);

			surf->drawCircle(renderX, renderY, radius, true);
		}
	}

	void GuiCircleButton::setOnClickFunction(std::function<void(GuiInstance*)> func)
	{
		onClickFunction = func;
	}

	void GuiCircleButton::setOnClickHoldFunction(std::function<void(GuiInstance*)> func)
	{
		onClickHoldFunction = func;
	}

	void GuiCircleButton::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}

	void GuiCircleButton::setOutlineColor(Color c)
	{
		outlineColor = c;
	}

	void GuiCircleButton::setFocusOutlineColor(Color c)
	{
		focusOutlineColor = c;
	}

	void GuiCircleButton::setRadius(int v)
	{
		radius = v;
	}
	int GuiCircleButton::getRadius()
	{
		return radius;
	}

	#pragma endregion

} //NAMESPACE glib END