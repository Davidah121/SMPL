#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_RECTANGLE_BUTTON

	const Class GuiRectangleButton::myClass = Class("GuiRectangleButton", {&GuiInstance::myClass});
	const Class* GuiRectangleButton::getClass()
	{
		return &GuiRectangleButton::myClass;
	}

	GuiRectangleButton::GuiRectangleButton(int x, int y, int width, int height) : GuiInstance()
	{
		setBaseX(x);
		setBaseY(y);
		this->width = width;
		this->height = height;
		onClickFunction = nullptr;
	}

	GuiRectangleButton::GuiRectangleButton(const GuiRectangleButton& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiRectangleButton::operator=(const GuiRectangleButton& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiRectangleButton::copy(const GuiRectangleButton& other)
	{
		onClickFunction = other.onClickFunction;
		onClickHoldFunction = other.onClickHoldFunction;

		width = other.width;
		height = other.height;

		hover = other.hover;

		backgroundColor = other.backgroundColor;
		hoverColor = other.hoverColor;
		focusBackgroundColor = other.focusBackgroundColor;

		outlineColor = other.outlineColor;
		focusOutlineColor = other.focusOutlineColor;
	}

	GuiRectangleButton::~GuiRectangleButton()
	{
	}

	void GuiRectangleButton::update()
	{
		int mouseX = Input::getMouseX();
		int mouseY = Input::getMouseY();
		
		if(getManager()!=nullptr)
		{
			mouseX -= getManager()->getWindowX();
			mouseY -= getManager()->getWindowY();
		}

		
		if (mouseX >= x && mouseX <= x + width)
		{
			if (mouseY >= y && mouseY <= y + height)
			{
				hover = true;

				if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
				{
					setFocus(true);
					if (onClickFunction != nullptr)
						onClickFunction(this);
				}
			}
			else
			{
				hover = false;
			}
		}
		else
		{
			hover = false;
		}

		if(Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
		{
			if(hover && getFocus())
			{
				if(onClickReleaseFunction != nullptr)
					onClickReleaseFunction(this);
			}
			setFocus(false);
		}

		if(getFocus())
		{
			if (onClickHoldFunction != nullptr)
				onClickHoldFunction(this);
		}

		
		boundingBox = Box2D(x, y, x+width, y+height);
	}

	void GuiRectangleButton::render(Image* surf)
	{
		if(surf!=nullptr)
		{
			//draw a rectangle
			if(!getFocus())
			{
				if(!hover)
					Graphics::setColor(backgroundColor);
				else
					Graphics::setColor(hoverColor);
			}
			else
			{
				Graphics::setColor(focusBackgroundColor);
			}
			
			surf->drawRect(renderX, renderY, renderX + width, renderY + height, false);

			if (getFocus() == false)
				Graphics::setColor(outlineColor);
			else
				Graphics::setColor(focusOutlineColor);

			surf->drawRect(renderX, renderY, renderX + width, renderY + height, true);
		}
	}

	void GuiRectangleButton::setOnClickFunction(std::function<void(GuiInstance*)> func)
	{
		onClickFunction = func;
	}

	void GuiRectangleButton::setOnClickHoldFunction(std::function<void(GuiInstance*)> func)
	{
		onClickHoldFunction = func;
	}

	void GuiRectangleButton::setOnClickReleaseFunction(std::function<void(GuiInstance*)> func)
	{
		onClickReleaseFunction = func;
	}

	void GuiRectangleButton::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}

	void GuiRectangleButton::setOutlineColor(Color c)
	{
		outlineColor = c;
	}

	void GuiRectangleButton::setFocusOutlineColor(Color c)
	{
		focusOutlineColor = c;
	}

	void GuiRectangleButton::setFocusBackgroundColor(Color c)
	{
		focusBackgroundColor = c;
	}

	void GuiRectangleButton::setHoverColor(Color c)
	{
		hoverColor = c;
	}

	void GuiRectangleButton::setWidth(int v)
	{
		width = v;
	}
	void GuiRectangleButton::setHeight(int v)
	{
		height = v;
	}
	int GuiRectangleButton::getWidth()
	{
		return width;
	}
	int GuiRectangleButton::getHeight()
	{
		return height;
	}

	#pragma endregion

} //NAMESPACE glib END