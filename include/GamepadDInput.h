#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include<Windows.h>
#include<vector>

#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace glib
{

	struct DeviceStruct
	{
		LPCDIDEVICEINSTANCE deviceInfo;
		LPDIRECTINPUTDEVICE8 gamepadDevice;
	};


	class GamepadDInput
	{
	public:
		const static int A_BUTTON = 1;
		const static int B_BUTTON = 2;
		const static int X_BUTTON = 0;
		const static int Y_BUTTON = 3;

		const static int DPAD_UP_BUTTON = -4;
		const static int DPAD_DOWN_BUTTON = -5;
		const static int DPAD_LEFT_BUTTON = -6;
		const static int DPAD_RIGHT_BUTTON = -7;

		const static int LB_BUTTON = 5;
		const static int RB_BUTTON = 6;
		const static int LEFT_STICK_BUTTON = 11;
		const static int RIGHT_STICK_BUTTON = 12;

		const static int LEFT_TRIGGER = 7;
		const static int RIGHT_TRIGGER = 8;

		const static int START_BUTTON = 9;
		const static int BACK_BUTTON = 10;

		const static int LEFT_X_AXIS = 0;
		const static int LEFT_Y_AXIS = 1;
		const static int RIGHT_X_AXIS = 2;
		const static int RIGHT_Y_AXIS = 3;

		const static int EXTRA_AXIS1 = 6;
		const static int EXTRA_AXIS2 = 7;

		const static int SLIDER1 = 8;
		const static int SLIDER2 = 9;

		const static int AXIS_DIVIDE_VALUE = 32767;
		const static int TRIGGER_DIVIDE_VALUE = 255;

		static int DEAD_ZONE;

		GamepadDInput();
		~GamepadDInput();

		//If windowHandle is NULL, it defaults to the desktop window
		//Both foreground and Exclusive will be false in that case.
		static bool init(HWND windowHandle, bool Foreground, bool Exclusive);
		static void dispose();

		static int getSize();

		static GUID getGUID(int id);
		static LPCDIDEVICEINSTANCE getDeviceInfo(int id);
		static DIJOYSTATE getJoystickValues(int id);

		static void update();
		static bool isConnected(int id);
		static bool getButtonPressed(int id, int button);
		static bool getButtonDown(int id, int button);
		static bool getButtonUp(int id, int button);
		static float getAxisValue(int id, int axis);

		static void refindDevices();

	private:
		static BOOL CALLBACK EnumerateDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID uselessJunk);
		static BOOL IsXInputDevice(const GUID* pGuidProductFromDirectInput);

		static void findDevices();

		static std::vector< DeviceStruct > devices;
		static std::vector< DIJOYSTATE > previousState;
		static std::vector< DIJOYSTATE > currentState;
		static std::vector< bool > connected;

		static bool exclusive;
		static bool foreground;

		static HWND windowHandle;

		static IDirectInput8* inface;
	};

}