#include "GamepadXInput.h"
#include "MathExt.h"

#ifdef __unix__

#else
	#pragma warning (disable : 4995)

	namespace glib
	{

		bool GamepadXInput::buttonPressed[56] = {};
		bool GamepadXInput::buttonDown[56] = {};
		bool GamepadXInput::buttonUp[56] = {};

		float GamepadXInput::axisValue[24] = {};

		bool GamepadXInput::gamepadConnected[4] = {};

		int GamepadXInput::DEAD_ZONE = 0;

		void GamepadXInput::init()
		{
			XInputEnable(true);
		}

		void GamepadXInput::setButtonDown(int id, int button)
		{
			int correctBut = MathExt::abs(button);

			int offset = id* GamepadXInput::BUTTONS_PER_CONTROLLER;

			if (buttonDown[correctBut + offset] == false)
				buttonPressed[correctBut + offset] = true;
			else
				buttonPressed[correctBut + offset] = false;

			buttonDown[correctBut + offset] = true;
			buttonUp[correctBut + offset] = false;
		}

		void GamepadXInput::setButtonUp(int id, int button)
		{
			int correctBut = abs(button);

			int offset = id* GamepadXInput::BUTTONS_PER_CONTROLLER;

			if (buttonDown[correctBut + offset] == true)
				buttonUp[correctBut + offset] = true;
			else
				buttonUp[correctBut + offset] = false;

			buttonDown[correctBut + offset] = false;
			buttonPressed[correctBut + offset] = false;
		}

		void GamepadXInput::setAxis(int id, int axis, float value)
		{
			int offset = id* GamepadXInput::AXIES_PER_CONTROLLER;
			axisValue[axis + offset] = value;
		}

		void GamepadXInput::update()
		{
			XINPUT_STATE controllerInfo;
			DWORD state;
			int offset = 0;
			bool value = false;
			float convertedValue = 0;

			for (int i = 0; i < 4; i++)
			{
				ZeroMemory(&state, sizeof(DWORD));
				ZeroMemory(&controllerInfo, sizeof(XINPUT_STATE));

				state = XInputGetState(i, &controllerInfo);

				if (state == ERROR_SUCCESS)
				{
					//gamepad is connected so store that for later use.
					//determine if the values have changed (DO LATER)
					gamepadConnected[i] = true;
					
					//for every button, determine if the button is held down then set some values

					//A BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_A)!=0);
					if (value == true)
						setButtonDown(i, GamepadXInput::A_BUTTON);
					else
						setButtonUp(i, GamepadXInput::A_BUTTON);

					//B BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::B_BUTTON);
					else
						setButtonUp(i, GamepadXInput::B_BUTTON);

					//X BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::X_BUTTON);
					else
						setButtonUp(i, GamepadXInput::X_BUTTON);

					//Y BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::Y_BUTTON);
					else
						setButtonUp(i, GamepadXInput::Y_BUTTON);

					//DPAD UP BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::DPAD_UP_BUTTON);
					else
						setButtonUp(i, GamepadXInput::DPAD_UP_BUTTON);

					//DPAD DOWN BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::DPAD_DOWN_BUTTON);
					else
						setButtonUp(i, GamepadXInput::DPAD_DOWN_BUTTON);

					//DPAD RIGHT BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::DPAD_RIGHT_BUTTON);
					else
						setButtonUp(i, GamepadXInput::DPAD_RIGHT_BUTTON);

					//DPAD LEFT BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::DPAD_LEFT_BUTTON);
					else
						setButtonUp(i, GamepadXInput::DPAD_LEFT_BUTTON);

					//LEFT STICK BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::LEFT_STICK_BUTTON);
					else
						setButtonUp(i, GamepadXInput::LEFT_STICK_BUTTON);

					//RIGHT STICK BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::RIGHT_STICK_BUTTON);
					else
						setButtonUp(i, GamepadXInput::RIGHT_STICK_BUTTON);

					//LEFT SHOULDER BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::LB_BUTTON);
					else
						setButtonUp(i, GamepadXInput::LB_BUTTON);

					//RIGHT SHOULDER BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::RB_BUTTON);
					else
						setButtonUp(i, GamepadXInput::RB_BUTTON);

					//START BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::START_BUTTON);
					else
						setButtonUp(i, GamepadXInput::START_BUTTON);

					//BACK BUTTON
					value = ((controllerInfo.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0);
					if (value == true)
						setButtonDown(i, GamepadXInput::BACK_BUTTON);
					else
						setButtonUp(i, GamepadXInput::BACK_BUTTON);


					//Now time for the axis values
					//convert the stick values to be between -1.0 and 1.0
					//convert the trigger values to be between 0.0 and 1.0
					
					if (MathExt::abs(controllerInfo.Gamepad.sThumbLX) > DEAD_ZONE)
					{
						if (controllerInfo.Gamepad.sThumbLX > 0.0f)
						{
							convertedValue = (float)(controllerInfo.Gamepad.sThumbLX-DEAD_ZONE) / (AXIS_DIVIDE_VALUE - DEAD_ZONE);
							convertedValue = MathExt::min(convertedValue, 1.0f);
						}
						else
						{
							convertedValue = (float)(controllerInfo.Gamepad.sThumbLX + DEAD_ZONE) / (AXIS_DIVIDE_VALUE - DEAD_ZONE);
							convertedValue = MathExt::max(convertedValue, -1.0f);
						}

						//std::cout << convertedValue << std::endl;
						setAxis(i, LEFT_X_AXIS, convertedValue);
					}
					else
					{
						setAxis(i, LEFT_X_AXIS, 0.0f);
					}

					if (MathExt::abs(controllerInfo.Gamepad.sThumbLY) > DEAD_ZONE)
					{
						if (controllerInfo.Gamepad.sThumbLY > 0.0f)
						{
							convertedValue = (float)(controllerInfo.Gamepad.sThumbLY - DEAD_ZONE) / (AXIS_DIVIDE_VALUE - DEAD_ZONE);
							convertedValue = MathExt::min(convertedValue, 1.0f);
						}
						else
						{
							convertedValue = (float)(controllerInfo.Gamepad.sThumbLY + DEAD_ZONE) / (AXIS_DIVIDE_VALUE - DEAD_ZONE);
							convertedValue = MathExt::max(convertedValue, -1.0f);
						}

						setAxis(i, LEFT_Y_AXIS, convertedValue);
					}
					else
					{
						setAxis(i, LEFT_Y_AXIS, 0.0f);
					}

					if (MathExt::abs(controllerInfo.Gamepad.sThumbRX) > DEAD_ZONE)
					{
						if (controllerInfo.Gamepad.sThumbRX > 0.0f)
						{
							convertedValue = (float)(controllerInfo.Gamepad.sThumbRX - DEAD_ZONE) / (AXIS_DIVIDE_VALUE - DEAD_ZONE);
							convertedValue = MathExt::min(convertedValue, 1.0f);
						}
						else
						{
							convertedValue = (float)(controllerInfo.Gamepad.sThumbRX + DEAD_ZONE) / (AXIS_DIVIDE_VALUE - DEAD_ZONE);
							convertedValue = MathExt::max(convertedValue, -1.0f);
						}

						setAxis(i, RIGHT_X_AXIS, convertedValue);
					}
					else
					{
						setAxis(i, RIGHT_X_AXIS, 0.0f);
					}

					if (MathExt::abs(controllerInfo.Gamepad.sThumbRY) > DEAD_ZONE)
					{
						if (controllerInfo.Gamepad.sThumbRY > 0.0f)
						{
							convertedValue = (float)(controllerInfo.Gamepad.sThumbRY - DEAD_ZONE) / (AXIS_DIVIDE_VALUE - DEAD_ZONE);
							convertedValue = MathExt::min(convertedValue, 1.0f);
						}
						else
						{
							convertedValue = (float)(controllerInfo.Gamepad.sThumbRY + DEAD_ZONE) / (AXIS_DIVIDE_VALUE - DEAD_ZONE);
							convertedValue = MathExt::max(convertedValue, -1.0f);
						}

						setAxis(i, RIGHT_Y_AXIS, convertedValue);
					}
					else
					{
						setAxis(i, RIGHT_Y_AXIS, 0.0f);
					}

					convertedValue = (float)controllerInfo.Gamepad.bLeftTrigger / TRIGGER_DIVIDE_VALUE;
					setAxis(i, LEFT_TRIGGER, convertedValue);

					convertedValue = (float)controllerInfo.Gamepad.bRightTrigger / TRIGGER_DIVIDE_VALUE;
					setAxis(i, RIGHT_TRIGGER, convertedValue);
				}
				else
				{
					gamepadConnected[i] = false;
				}
			}
		}

		bool GamepadXInput::isConnected(int id)
		{
			return GamepadXInput::gamepadConnected[id];
		}

		bool GamepadXInput::getButtonPressed(int id, int button)
		{
			int correctBut = MathExt::abs(button);
			int offset = id* GamepadXInput::BUTTONS_PER_CONTROLLER;

			return GamepadXInput::buttonPressed[correctBut + offset];
		}

		bool GamepadXInput::getButtonDown(int id, int button)
		{
			int correctBut = MathExt::abs(button);
			int offset = id* GamepadXInput::BUTTONS_PER_CONTROLLER;

			return GamepadXInput::buttonDown[correctBut + offset];
		}

		bool GamepadXInput::getButtonUp(int id, int button)
		{
			int correctBut = MathExt::abs(button);
			int offset = id* GamepadXInput::BUTTONS_PER_CONTROLLER;

			return GamepadXInput::buttonUp[correctBut + offset];
		}

		float GamepadXInput::getAxisValue(int id, int axis)
		{
			int offset = id* GamepadXInput::AXIES_PER_CONTROLLER;

			return GamepadXInput::axisValue[axis + offset];
		}

		XINPUT_STATE GamepadXInput::getState(int id)
		{
			XINPUT_STATE controllerInfo;
			DWORD state;
			ZeroMemory(&controllerInfo, sizeof(XINPUT_STATE));
			ZeroMemory(&state, sizeof(DWORD));
			state = XInputGetState(id, &controllerInfo);

			return controllerInfo;
		}

		void GamepadXInput::setVibration(int id, float intensity)
		{
			XINPUT_VIBRATION vib;
			vib.wLeftMotorSpeed = (unsigned short)MathExt::clamp((int)(65535*intensity), 0, 65535);
			vib.wRightMotorSpeed = (unsigned short)MathExt::clamp((int)(65535*intensity), 0, 65535);

			XInputSetState(id, &vib);
		}

		void GamepadXInput::setVibration(int id, float leftIntensity, float rightIntensity)
		{
			XINPUT_VIBRATION vib;
			
			vib.wLeftMotorSpeed = (unsigned short)MathExt::clamp((int)(65535*leftIntensity), 0, 65535);
			vib.wRightMotorSpeed = (unsigned short)MathExt::clamp((int)(65535*rightIntensity), 0, 65535);

			XInputSetState(id, &vib);
		}

		void GamepadXInput::dispose()
		{
			XInputEnable(FALSE);
		}

	} //NAMESPACE glib END
#endif