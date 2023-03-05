#include "ext/MouseSimulator.h"
#include "Input.h"

namespace glib
{
    void MouseSimulator::emulateMousePress(int key, int x, int y)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			INPUT i;
			ZeroMemory(&i, sizeof(INPUT));

			i.type = INPUT_MOUSE;
			
			switch (key)
			{
			case Input::LEFT_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				break;
			case Input::RIGHT_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				break;
			case Input::MIDDLE_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
				break;
			case Input::X1_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_XDOWN;
				i.mi.mouseData = XBUTTON1;
				break;
			case Input::X2_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_XDOWN;
				i.mi.mouseData = XBUTTON2;
				break;
			default:
				break;
			}

			if(x > 0)
				i.mi.dx = x;
			if(y > 0)
				i.mi.dy = y;
			if(x > 0 || y > 0)
				i.mi.dwFlags |= MOUSE_MOVE_ABSOLUTE;

			SendInput(1, &i, sizeof(INPUT));

		#endif
	}

	void MouseSimulator::emulateMouseRelease(int key, int x, int y)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			INPUT i;
			ZeroMemory(&i, sizeof(INPUT));

			i.type = INPUT_MOUSE;

			switch (key)
			{
			case Input::LEFT_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				break;
			case Input::RIGHT_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
				break;
			case Input::MIDDLE_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
				break;
			case Input::X1_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_XUP;
				i.mi.mouseData = XBUTTON1;
				break;
			case Input::X2_MOUSE_BUTTON:
				i.mi.dwFlags = MOUSEEVENTF_XUP;
				i.mi.mouseData = XBUTTON2;
				break;
			default:
				break;
			}
			
			if(x > 0)
				i.mi.dx = x;
			if(y > 0)
				i.mi.dy = y;
			if(x > 0 || y > 0)
				i.mi.dwFlags |= MOUSE_MOVE_ABSOLUTE;

			SendInput(1, &i, sizeof(INPUT));
		
		#endif
	}

	void MouseSimulator::emulateMouseWheel(int wheel, int amount)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			INPUT i;
			ZeroMemory(&i, sizeof(INPUT));

			i.type = INPUT_MOUSE;

			switch (wheel)
			{
			case MOUSE_WHEEL_UP:
				i.mi.mouseData = -WHEEL_DELTA * amount;
				i.mi.dwFlags = MOUSEEVENTF_WHEEL;
				break;
			case MOUSE_WHEEL_DOWN:
				i.mi.mouseData = WHEEL_DELTA * amount;
				i.mi.dwFlags = MOUSEEVENTF_WHEEL;
				break;
			case MOUSE_WHEEL_LEFT:
				i.mi.mouseData = -WHEEL_DELTA * amount;
				i.mi.dwFlags = MOUSEEVENTF_HWHEEL;
				break;
			case MOUSE_WHEEL_RIGHT:
				i.mi.mouseData = WHEEL_DELTA * amount;
				i.mi.dwFlags = MOUSEEVENTF_HWHEEL;
				break;
			default:
				break;
			}

			SendInput(1, &i, sizeof(INPUT));

		#endif
	}

	void MouseSimulator::setMousePosition(int x, int y)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32
		SetCursorPos(x, y);
		#endif
	}
}