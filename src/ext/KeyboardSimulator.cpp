#include "ext/KeyboardSimulator.h"
#include "StringTools.h"

#ifdef _WIN32

namespace smpl
{
    void KeyboardSimulator::emulateKeyPress(int key)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			INPUT i;
			ZeroMemory(&i, sizeof(INPUT));
			
			i.type = INPUT_KEYBOARD;
			i.ki.wVk = key;
			i.ki.dwFlags = NULL;

			uint32_t err = SendInput(1, &i, sizeof(INPUT));

		#endif
	}

	void KeyboardSimulator::emulateKeyRelease(int key)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			INPUT i;
			ZeroMemory(&i, sizeof(INPUT));

			i.type = INPUT_KEYBOARD;
			i.ki.wVk = key;
			i.ki.dwFlags = KEYEVENTF_KEYUP;

			uint32_t err = SendInput(1, &i, sizeof(INPUT));

		#endif
	}
}

#endif