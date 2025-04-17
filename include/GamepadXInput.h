#pragma once
#include "BuildOptions.h"

#ifndef NO_GAME_INPUT

	#include <vector>
	#ifdef __unix__

	#else

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef NOMINMAX
		#define NOMINMAX
	#endif

	#include <Windows.h>
	#include <Xinput.h>
	#include <iostream>

	namespace smpl
	{

		class DLL_OPTION GamepadXInput
		{
		public:

			/**
			 * @brief Initializes the program for XInput.
			 * 		Requires the the Windows sdk, Windows.h, and Xinput.h
			 * 		Links with the XInput.lib using a #pragma comment
			 */
			static void init();

			/**
			 * @brief Deinitializes the program for XInput.
			 * 		Should be called when the program no longer needs XInput
			 * 		to free allocated resources.
			 */
			static void dispose();

			/**
			 * @brief Updates all information for all of the connected XInput devices.
			 * 		Should be called before accessing the controller values to receive up to date
			 * 		data.
			 */
			static void update();

			/**
			 * @brief Returns if a controller is connected.
			 * @param id
			 * 		The controller to check. It is the controller's index in the internal list.
			 * @return bool
			 */
			static bool isConnected(int id);

			/**
			 * @brief Returns if a controller specified button has just been pressed.
			 * 		Lasts until the next time update is called.
			 * @param id
			 * 		The controller to check. It is the controller's index in the internal list.
			 * @param button
			 * 		The button to check. The constants should be used to access the correct button.
			 * @return bool
			 */
			static bool getButtonPressed(int id, int button);

			/**
			 * @brief Returns if a controller specified button is held down.
			 * @param id
			 * 		The controller to check. It is the controller's index in the internal list.
			 * @param button
			 * 		The button to check. The constants should be used to access the correct button.
			 * @return bool
			 */
			static bool getButtonDown(int id, int button);

			/**
			 * @brief Returns if a controller specified button has just been released.
			 * 		Lasts until the next time update is called.
			 * @param id
			 * 		The controller to check. It is the controller's index in the internal list.
			 * @param button
			 * 		The button to check. The constants should be used to access the correct button.
			 * @return bool
			 */
			static bool getButtonUp(int id, int button);

			/**
			 * @brief Returns the value of the specified axis for the controller.
			 * @param id
			 * 		The controller to check. It is the controller's index in the internal list.
			 * @param axis
			 * 		The axis to check. The constants should be used to access the correct axis.
			 * @return float
			 * 		A value between [-1, 1]
			 */
			static float getAxisValue(int id, int axis);

			/**
			 * @brief Returns the raw XInput data for the specified controller.
			 * @param id
			 * 		The controller to check. It is the controller's index in the internal list.
			 * @return XINPUT_STATE
			 */
			static XINPUT_STATE getState(int id);

			/**
			 * @brief Sets the vibration for the specified controller if it supports it.
			 * 		Vibration last until it is set back to 0.
			 * @param id
			 * 		The controller to check. It is the controller's index in the internal list.
			 * @param intensity
			 * 		The intensity to vibrate both the left and right motors. Must be a value between [0, 1]
			 */
			static void setVibration(int id, float intensity);

			/**
			 * @brief Sets the vibration for the specified controller if it supports it.
			 * 		Vibration last until it is set back to 0.
			 * @param id
			 * 		The controller to check. It is the controller's index in the internal list.
			 * @param leftIntensity
			 * 		The intensity to vibrate the left motor. Must be a value between [0, 1]
			 * @param rightIntensity
			 * 		The intensity to vibrate the right motor. Must be a value between [0, 1]
			 */
			static void setVibration(int id, float leftIntensity, float rightIntensity);
			
			const static int BUTTONS_PER_CONTROLLER = 14;
			const static int AXIES_PER_CONTROLLER = 6;

			const static int A_BUTTON = 0;
			const static int B_BUTTON = 1;
			const static int X_BUTTON = 2;
			const static int Y_BUTTON = 3;

			const static int DPAD_UP_BUTTON = -4;
			const static int DPAD_DOWN_BUTTON = -5;
			const static int DPAD_LEFT_BUTTON = -6;
			const static int DPAD_RIGHT_BUTTON = -7;

			const static int LB_BUTTON = 8;
			const static int RB_BUTTON = 9;
			const static int LEFT_STICK_BUTTON = 10;
			const static int RIGHT_STICK_BUTTON = 11;

			const static int START_BUTTON = 12;
			const static int BACK_BUTTON = 13;

			const static int LEFT_X_AXIS = 0;
			const static int LEFT_Y_AXIS = 1;
			const static int RIGHT_X_AXIS = 2;
			const static int RIGHT_Y_AXIS = 3;
			const static int LEFT_TRIGGER = 4;
			const static int RIGHT_TRIGGER = 5;

			const static int AXIS_DIVIDE_VALUE = 32767;
			const static int TRIGGER_DIVIDE_VALUE = 255;

			static int DEAD_ZONE;

		private:
			static bool buttonPressed[56];
			static bool buttonDown[56];
			static bool buttonUp[56];

			static float axisValue[24];
			
			static bool gamepadConnected[4];

			static void setButtonDown(int id, int button);
			static void setButtonUp(int id, int button);
			static void setAxis(int id, int axis, float value);
		};

	}  //NAMESPACE smpl END

	#endif

#endif