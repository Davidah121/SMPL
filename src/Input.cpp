#include "Input.h"
#include "SimpleWindow.h"

namespace glib
{

	bool Input::preKeyState[256];
	bool Input::keyState[256];

	bool Input::preMouseState[3];
	bool Input::mouseState[3];

	std::queue<int> Input::charBuffer;
	bool Input::canClearBuffer = false;

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

	std::mutex Input::inputMutex;
	size_t tempThing = 0;

	void Input::pollInput()
	{
		inputMutex.lock();

		verticalScrollValue = nVerticalScrollValue;
		horizontalScrollValue = nHorizontalScrollValue;

		nVerticalScrollValue = 0;
		nHorizontalScrollValue = 0;

		lastKeyPressed = -1;
		lastKeyUp = -1;
		mouseMoved = false;
		keyChanged = false;
		mouseClicked = false;
		int px, py;

		#ifdef __unix__
		#else
			POINT tp;
			GetCursorPos(&tp);
			px = tp.x;
			py = tp.y;
		#endif

		if (px != mouseX)
		{
			mouseMoved = true;
		}
		if (py != mouseY)
		{
			mouseMoved = true;
		}

		mouseX = px;
		mouseY = py;

		for (int i = 0; i < 256; i++)
		{
			bool keyValue = 0;
			#ifdef __unix__

			#else
				keyValue = (GetAsyncKeyState(i) >> 15 & 0x01) == 1;
			#endif

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
		preMouseState[1] = mouseState[1];
		preMouseState[2] = mouseState[2];

		#ifdef __unix__
		#else
			mouseState[0] = (GetAsyncKeyState(VK_LBUTTON) >> 15 & 0x01) == 1;
			mouseState[1] = (GetAsyncKeyState(VK_MBUTTON) >> 15 & 0x01) == 1;
			mouseState[2] = (GetAsyncKeyState(VK_RBUTTON) >> 15 & 0x01) == 1;
		#endif

		if (preMouseState[0] != mouseState[0]
			|| preMouseState[1] != mouseState[1]
			|| preMouseState[2] != mouseState[2])
		{
			mouseClicked = true;
		}

		if(canClearBuffer)
		{
			charBuffer = std::queue<int>();
			canClearBuffer = false;
		}

		inputMutex.unlock();
	}

	bool Input::getKeyPressed(int k)
	{
		if (preKeyState[k & 0xFF] == false && keyState[k & 0xFF] == true)
			return true;
		return false;
	}

	bool Input::getKeyDown(int k)
	{
		return keyState[k & 0xFF];
	}

	bool Input::getKeyUp(int k)
	{
		if (preKeyState[k & 0xFF] == true && keyState[k & 0xFF] == false)
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
		return mouseState[v & 0xFF];
	}

	bool Input::getMouseUp(int v)
	{
		if (preMouseState[v & 0xFF] == true && mouseState[v & 0xFF] == false)
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

	void Input::clearCharactersTyped()
	{
		inputMutex.lock();
		charBuffer = std::queue<int>();
		inputMutex.unlock();
	}

	std::queue<int> Input::getCharactersTyped()
	{
		inputMutex.lock();
		if(charBuffer.size() > 0)
			canClearBuffer = true;
		std::queue<int> tempBuff = charBuffer;
		inputMutex.unlock();

		return tempBuff;
	}

	void Input::adjustVerticalScroll(int v)
	{
		inputMutex.lock();
		nVerticalScrollValue += v;
		inputMutex.unlock();
	}

	void Input::adjustHorizontalScroll(int v)
	{
		inputMutex.lock();
		nHorizontalScrollValue += v;
		inputMutex.unlock();
	}

	void Input::adjustCurrCharVal(unsigned int v1, unsigned int v2)
	{
		inputMutex.lock();
		int repeatV = v2 & 0xFFFF;
		
		for(int i=0; i<repeatV; i++)
		{
			charBuffer.push(v1);
		}
		inputMutex.unlock();
	}

} //NAMESPACE glib END