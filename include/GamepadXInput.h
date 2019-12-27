#pragma once

#pragma comment(lib, "XInput.lib")
#include <Windows.h>
#include <Xinput.h>
#include <iostream>

class GamepadXInput
{
public:

	static void init();
	static void update();
	static bool isConnected(int id);
	static bool getButtonPressed(int id, int button);
	static bool getButtonDown(int id, int button);
	static bool getButtonUp(int id, int button);
	static float getAxisValue(int id, int axis);
	static XINPUT_STATE getState(int id);
	static void dispose();

	static void setVibration(int id, float intensity);
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

