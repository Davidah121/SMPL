#include "Input.h"
#include "SimpleWindow.h"

namespace glib
{

	bool Input::preKeyState[256];
	bool Input::keyState[256];

	bool Input::preMouseState[3];
	bool Input::mouseState[3];

	bool Input::mouseMoved = false;
	bool Input::keyChanged = false;
	bool Input::mouseClicked = false;

	int Input::mouseX = -1;
	int Input::mouseY = -1;

	int Input::lastKeyPressed = -1;
	int Input::lastKeyDown = -1;
	int Input::lastKeyUp = -1;

	int Input::verticalScrollValue = 0;
	int Input::horizontalScrollValue = 0;

	int Input::nVerticalScrollValue = 0;
	int Input::nHorizontalScrollValue = 0;

	void Input::pollInput()
	{
		//get mouse wheel from current active window
		SimpleWindow::setMouseVWheelValuePointer(&nVerticalScrollValue);
		SimpleWindow::setMouseHWheelValuePointer(&nHorizontalScrollValue);

		verticalScrollValue = nVerticalScrollValue;
		horizontalScrollValue = nHorizontalScrollValue;

		nVerticalScrollValue = 0;
		nHorizontalScrollValue = 0;

		lastKeyPressed = -1;
		lastKeyUp = -1;
		mouseMoved = false;
		keyChanged = false;
		mouseClicked = false;

		POINT p;
		GetCursorPos(&p);

		if (p.x != mouseX)
		{
			mouseMoved = true;
		}
		if (p.y != mouseY)
		{
			mouseMoved = true;
		}

		mouseX = (int)p.x;
		mouseY = (int)p.y;

		for (int i = 0; i < 256; i++)
		{
			bool keyValue = (GetAsyncKeyState(i) >> 15 & 0x01) == 1;

			preKeyState[i] = keyState[i];
			keyState[i] = keyValue;

			if (keyState[i] != preKeyState[i])
			{
				keyChanged = true;

				if(preKeyState[i] == false)
				{
					lastKeyPressed = i;
					lastKeyDown = i;
				}
				if(preKeyState[i] == true)
				{
					lastKeyUp = i;

					if(i==lastKeyDown)
						lastKeyDown = -1;
				}
			}
		}
		preMouseState[0] = mouseState[0];
		mouseState[0] = (GetAsyncKeyState(VK_LBUTTON) >> 15 & 0x01) == 1;
		preMouseState[1] = mouseState[1];
		mouseState[1] = (GetAsyncKeyState(VK_MBUTTON) >> 15 & 0x01) == 1;
		preMouseState[2] = mouseState[2];
		mouseState[2] = (GetAsyncKeyState(VK_RBUTTON) >> 15 & 0x01) == 1;

		if (preMouseState[0] != mouseState[0]
			|| preMouseState[1] != mouseState[1]
			|| preMouseState[2] != mouseState[2])
		{
			mouseClicked = true;
		}
	}

	bool Input::getKeyPressed(unsigned char k)
	{
		if (preKeyState[k] == false && keyState[k] == true)
			return true;
		return false;
	}

	bool Input::getKeyDown(unsigned char k)
	{
		return keyState[k];
	}

	bool Input::getKeyUp(unsigned char k)
	{
		if (preKeyState[k] == true && keyState[k] == false)
			return true;
		return false;
	}

	bool Input::getMousePressed(int v)
	{
		if (preMouseState[v] == false && mouseState[v] == true)
			return true;
		return false;
	}

	bool Input::getMouseDown(int v)
	{
		return mouseState[v];
	}

	bool Input::getMouseUp(int v)
	{
		if (preMouseState[v] == true && mouseState[v] == false)
			return true;
		return false;
	}

	int Input::getMouseX()
	{
		return mouseX;
	}

	int Input::getMouseY()
	{
		return mouseY;
	}

	int Input::getMouseScrollVertical()
	{
		return verticalScrollValue;
	}

	int Input::getMouseScrollHorizontal()
	{
		return horizontalScrollValue;
	}

	bool Input::getKeyChanged()
	{
		return keyChanged;
	}

	bool Input::getMouseMoved()
	{
		return mouseMoved;
	}

	int Input::getLastKeyPressed()
	{
		return lastKeyPressed;
	}

	int Input::getLastKeyDown()
	{
		return lastKeyDown;
	}

	int Input::getLastKeyReleased()
	{
		return lastKeyUp;
	}

} //NAMESPACE glib END