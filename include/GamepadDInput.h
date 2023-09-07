#pragma once

#ifndef NO_GAME_INPUT

	#ifdef __unix__

	#else

		#ifndef WIN32_LEAN_AND_MEAN
			#define WIN32_LEAN_AND_MEAN
		#endif

		#ifndef NOMINMAX
			#define NOMINMAX
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

				//If windowHandle is NULL, it defaults to the desktop window
				//Both foreground and Exclusive will be false in that case.

				/**
				 * @brief Initializes the program for DirectInput.
				 * 		This library is exclusive to windows and requires the Windows SDK.
				 * 			Headers used are Windows.h, dinput.h
				 * 			Libraries used are dinput8.lib and dxguid.h
				 * 				They are linked using #pragma comment() but may need to be linked manually.
				 * 
				 * 		Note that XInput devices will not appear and should be accessed from GamepadXInput.
				 * 		Note that the constants are based on a Logitech F310 in DirectInput Mode
				 * 		Must be disposed at the end of the program.
				 * @param windowHandle
				 * 		A valid HWND that will receive the input.
				 * 		If the value is NULL or 0, the desktop window is used.
				 * @param Foreground
				 * 		Chooses whether to allow input if the window is in the background.
				 * 		Will be false if the desktop window is used.
				 * @param Exclusive
				 * 		Chooses whether to have exclusive rights over input.
				 * 		Will be false if the desktop window is used.
				 */
				static bool init(HWND windowHandle, bool Foreground, bool Exclusive);

				/**
				 * @brief Disposes all memory used and releases the devices.
				 * 		Must be called when the program is done with DirectInput.
				 */
				static void dispose();

				/**
				 * @brief Returns the amount of devices that can be used.
				 * @return int
				 */
				static int getSize();

				/**
				 * @brief Returns the GUID of the specified device.
				 * @param id
				 * 		This refers to the index in the internal list of devices and not the GUID.
				 * @return GUID
				 */
				static GUID getGUID(int id);

				/**
				 * @brief Gets a structure containing information about the device.
				 * @param id
				 * 		The index of the device in the list.
				 * @return LPCDIDEVICEINSTANCE
				 * 		If successful, returns a valid structure. Otherwise, returns an empty structure.
				 */
				static LPCDIDEVICEINSTANCE getDeviceInfo(int id);

				/**
				 * @brief Gets a structure containing joystick information for the device.
				 * 		This contains the currently pressed buttons, joystick values, and dpad buttons.
				 * 		Can be used directly if needed.
				 * 
				 * 		A reconnection is attempted if failed the first time. If the second attempt fails,
				 * 		the device is considered disconnected.
				 * @param id
				 * 		The index of the device in the list.
				 * @return DIJOYSTATE
				 * 		If successful, returns a valid structure. Otherwise, returns an empty structure.
				 */
				static DIJOYSTATE getJoystickValues(int id);

				/**
				 * @brief Obtains updated information for all of the connected joysticks.
				 * 		This is the prefered method to obtain information for joysticks.
				 */
				static void update();

				/**
				 * @brief Returns if the specified joystick is connected.
				 * 		update() should be called before.
				 * @param id
				 * 		The index of the device in the list.
				 * @return bool
				 */
				static bool isConnected(int id);

				/**
				 * @brief Returns if the specified button of the joystick has just been pressed.
				 * 		update() should be called before.
				 * 		Last until the next update is called.
				 * @param id
				 * 		The index of the device in the list.
				 * @param button
				 * 		The id of the button.
				 * @return bool
				 */
				static bool getButtonPressed(int id, int button);

				/**
				 * @brief Returns if the specified button of the joystick is being held down.
				 * 		update() should be called before.
				 * @param id
				 * 		The index of the device in the list.
				 * @param button
				 * 		The id of the button.
				 * @return bool
				 */
				static bool getButtonDown(int id, int button);

				/**
				 * @brief Returns if the specified button of the joystick has just been released.
				 * 		update() should be called before.
				 * 		Last until the next update is called.
				 * @param id
				 * 		The index of the device in the list.
				 * @param button
				 * 		The id of the button.
				 * @return bool
				 */
				static bool getButtonUp(int id, int button);

				/**
				 * @brief Returns the value of the specified axis on the joystick.
				 * 		update() should be called before.
				 * @param id
				 * 		The index of the device in the list.
				 * @param axis
				 * 		The id of the axis.
				 * @return float
				 */
				static float getAxisValue(int id, int axis);

				/**
				 * @brief Determines which devices are connected.
				 * 		Use this to find new devices and remove devices that have been
				 * 		disconnected.
				 */
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
	#endif
	
#endif