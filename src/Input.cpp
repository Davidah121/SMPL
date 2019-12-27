#include "Input.h"
#include <Windows.h>

bool* Input::preKeyState = new bool[255];
bool* Input::keyState = new bool[255];

bool* Input::preMouseState = new bool[3];
bool* Input::mouseState = new bool[3];

bool Input::mouseMoved = false;
bool Input::keyChanged = false;
bool Input::mouseClicked = false;

int Input::mouseX = -1;
int Input::mouseY = -1;

void Input::pollInput()
{
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

	for (int i = 0; i < 255; i++)
	{
		bool keyValue = (GetKeyState(i) >> 15 & 0x01) == 1;

		preKeyState[i] = keyState[i];
		keyState[i] = keyValue;

		if (keyState[i] != preKeyState[i])
		{
			keyChanged = true;
		}
	}
	preMouseState[0] = mouseState[0];
	mouseState[0] = (GetKeyState(VK_LBUTTON) >> 15 & 0x01) == 1;
	preMouseState[1] = mouseState[1];
	mouseState[1] = (GetKeyState(VK_MBUTTON) >> 15 & 0x01) == 1;
	preMouseState[2] = mouseState[2];
	mouseState[2] = (GetKeyState(VK_RBUTTON) >> 15 & 0x01) == 1;

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

bool Input::getKeyChanged()
{
	return keyChanged;
}

bool Input::getMouseMoved()
{
	return mouseMoved;
}
