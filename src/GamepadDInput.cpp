#include "GamepadDInput.h"

#ifdef __unix__

#else
	#include <wbemidl.h>
	#include <oleauto.h>
	#include <iostream>

	#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
	#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

	#pragma warning(disable : 4996)

	namespace glib
	{

		std::vector< DeviceStruct > GamepadDInput::devices = std::vector< DeviceStruct >();
		bool GamepadDInput::exclusive = true;
		bool GamepadDInput::foreground = true;
		IDirectInput8* GamepadDInput::inface = nullptr;
		HWND GamepadDInput::windowHandle;


		std::vector< DIJOYSTATE > GamepadDInput::previousState = std::vector< DIJOYSTATE >();
		std::vector< DIJOYSTATE > GamepadDInput::currentState = std::vector< DIJOYSTATE >();
		std::vector< bool > GamepadDInput::connected = std::vector< bool >();

		//-----------------------------------------------------------------------------
		// Enum each PNP device using WMI and check each device ID to see if it contains 
		// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
		// Unfortunately this information can not be found by just using DirectInput 
		//-----------------------------------------------------------------------------
		BOOL GamepadDInput::IsXInputDevice(const GUID* pGuidProductFromDirectInput)
		{
			IWbemLocator* pIWbemLocator = NULL;
			IEnumWbemClassObject* pEnumDevices = NULL;
			IWbemClassObject* pDevices[20] = { 0 };
			IWbemServices* pIWbemServices = NULL;
			BSTR                    bstrNamespace = NULL;
			BSTR                    bstrDeviceID = NULL;
			BSTR                    bstrClassName = NULL;
			DWORD                   uReturned = 0;
			bool                    bIsXinputDevice = false;
			UINT                    iDevice = 0;
			VARIANT                 var;
			HRESULT                 hr;

			// CoInit if needed
			hr = CoInitialize(NULL);
			bool bCleanupCOM = SUCCEEDED(hr);

			// Create WMI
			hr = CoCreateInstance(__uuidof(WbemLocator),
				NULL,
				CLSCTX_INPROC_SERVER,
				__uuidof(IWbemLocator),
				(LPVOID*)& pIWbemLocator);
			if (FAILED(hr) || pIWbemLocator == NULL)
				goto LCleanup;

			bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2"); if (bstrNamespace == NULL) goto LCleanup;
			bstrClassName = SysAllocString(L"Win32_PNPEntity");   if (bstrClassName == NULL) goto LCleanup;
			bstrDeviceID = SysAllocString(L"DeviceID");          if (bstrDeviceID == NULL)  goto LCleanup;

			// Connect to WMI 
			hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L,
				0L, NULL, NULL, &pIWbemServices);
			if (FAILED(hr) || pIWbemServices == NULL)
				goto LCleanup;

			// Switch security level to IMPERSONATE. 
			CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
				RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

			hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
			if (FAILED(hr) || pEnumDevices == NULL)
				goto LCleanup;

			// Loop over all devices
			for (;; )
			{
				// Get 20 at a time
				hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
				if (FAILED(hr))
					goto LCleanup;
				if (uReturned == 0)
					break;

				for (iDevice = 0; iDevice < uReturned; iDevice++)
				{
					// For each device, get its device ID
					hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
					if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
					{
						// Check if the device ID contains "IG_".  If it does, then it's an XInput device
							// This information can not be found from DirectInput 
						if (wcsstr(var.bstrVal, L"IG_"))
						{
							// If it does, then get the VID/PID from var.bstrVal
							DWORD dwPid = 0, dwVid = 0;
							WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
							if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1)
								dwVid = 0;
							WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
							if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1)
								dwPid = 0;

							// Compare the VID/PID to the DInput device
							DWORD dwVidPid = MAKELONG(dwVid, dwPid);
							if (dwVidPid == pGuidProductFromDirectInput->Data1)
							{
								bIsXinputDevice = true;
								goto LCleanup;
							}
						}
					}
					SAFE_RELEASE(pDevices[iDevice]);
				}
			}

		LCleanup:
			if (bstrNamespace)
				SysFreeString(bstrNamespace);
			if (bstrDeviceID)
				SysFreeString(bstrDeviceID);
			if (bstrClassName)
				SysFreeString(bstrClassName);
			for (iDevice = 0; iDevice < 20; iDevice++)
				SAFE_RELEASE(pDevices[iDevice]);
			SAFE_RELEASE(pEnumDevices);
			SAFE_RELEASE(pIWbemLocator);
			SAFE_RELEASE(pIWbemServices);

			if (bCleanupCOM)
				CoUninitialize();

			return bIsXinputDevice;
		}


		void GamepadDInput::update()
		{
			for (int i = 0; i < devices.size(); i++)
			{
				previousState[i] = currentState[i];
				currentState[i] = getJoystickValues(i);
			}
		}

		bool GamepadDInput::isConnected(int id)
		{
			if (id < devices.size())
			{
				return connected[id];
			}
			return false;
		}

		bool GamepadDInput::getButtonPressed(int id, int button)
		{
			if (id < devices.size())
			{
				if (button > 0)
				{
					if (previousState[id].rgbButtons[button] == false)
					{
						return currentState[id].rgbButtons[button];
					}
				}
				else
				{
					int oldDir = previousState[id].rgdwPOV[0] / 4500;
					int dir = currentState[id].rgdwPOV[0] / 4500;

					if (button == DPAD_UP_BUTTON)
					{
						if (dir == 0 || dir == 1 || dir == 7)
						{
							if (oldDir != 0 && oldDir != 1 && oldDir != 7)
							{
								return true;
							}
						}
					}
					else if (button == DPAD_RIGHT_BUTTON)
					{
						if (dir == 1 || dir == 2 || dir == 3)
						{
							if (oldDir != 1 && oldDir != 2 && oldDir != 3)
							{
								return true;
							}
						}
					}
					else if (button == DPAD_LEFT_BUTTON)
					{
						if (dir == 5 || dir == 6 || dir == 7)
						{
							if (oldDir != 5 && oldDir != 6 && oldDir != 7)
							{
								return true;
							}
						}
					}
					else if (button == DPAD_DOWN_BUTTON)
					{
						if (dir == 3 || dir == 4 || dir == 5)
						{
							if (oldDir != 3 && oldDir != 4 && oldDir != 5)
							{
								return true;
							}
						}
					}

					return false;
				}
				return false;
			}
			return false;
		}

		bool GamepadDInput::getButtonDown(int id, int button)
		{
			if (id < devices.size())
			{
				if (button > 0)
				{
					return currentState[id].rgbButtons[button];
				}
				else
				{
					int dir = currentState[id].rgdwPOV[0] / 4500;

					switch (dir)
					{
					case 0:
						//dpad up
						if (button == DPAD_UP_BUTTON)
							return true;
						return false;
					case 1:
						//dpad up right
						if (button == DPAD_UP_BUTTON || button == DPAD_RIGHT_BUTTON)
							return true;
						return false;
					case 2:
						//dpad right
						if (button == DPAD_RIGHT_BUTTON)
							return true;
						return false;
					case 3:
						//dpad down right
						if (button == DPAD_DOWN_BUTTON || button == DPAD_RIGHT_BUTTON)
							return true;
						return false;
					case 4:
						//dpad down
						if (button == DPAD_DOWN_BUTTON)
							return true;
						return false;
					case 5:
						//dpad down left
						if (button == DPAD_DOWN_BUTTON || button == DPAD_LEFT_BUTTON)
							return true;
						return false;
					case 6:
						//dpad left
						if (button == DPAD_LEFT_BUTTON)
							return true;
						return false;
					case 7:
						//dpad up left
						if (button == DPAD_UP_BUTTON || button == DPAD_LEFT_BUTTON)
							return true;
						return false;
					default:
						return false;
					}
				}
			}
			return false;
		}

		bool GamepadDInput::getButtonUp(int id, int button)
		{
			if (id < devices.size())
			{
				if (button > 0)
				{
					if (previousState[id].rgbButtons[button] == true)
					{
						return currentState[id].rgbButtons[button] == false;
					}
				}
				else
				{
					int oldDir = previousState[id].rgdwPOV[0] / 4500;
					int dir = currentState[id].rgdwPOV[0] / 4500;

					if (button == DPAD_UP_BUTTON)
					{
						if (dir != 0 && dir != 1 && dir != 7)
						{
							if (oldDir == 0 || oldDir == 1 || oldDir == 7)
							{
								return true;
							}
						}
					}
					else if (button == DPAD_RIGHT_BUTTON)
					{
						if (dir != 1 && dir != 2 && dir != 3)
						{
							if (oldDir == 1 || oldDir == 2 || oldDir == 3)
							{
								return true;
							}
						}
					}
					else if (button == DPAD_LEFT_BUTTON)
					{
						if (dir != 5 && dir != 6 && dir != 7)
						{
							if (oldDir == 5 || oldDir == 6 || oldDir == 7)
							{
								return true;
							}
						}
					}
					else if (button == DPAD_DOWN_BUTTON)
					{
						if (dir != 3 && dir != 4 && dir != 5)
						{
							if (oldDir == 3 || oldDir == 4 || oldDir == 5)
							{
								return true;
							}
						}
					}

					return false;
				}
				return false;
			}
			return false;
		}

		float GamepadDInput::getAxisValue(int id, int axis)
		{
			if (id < devices.size())
			{
				switch (axis)
				{
				case GamepadDInput::LEFT_X_AXIS:
					return (float)currentState[id].lX / GamepadDInput::AXIS_DIVIDE_VALUE;
				case GamepadDInput::LEFT_Y_AXIS:
					return (float)currentState[id].lY / GamepadDInput::AXIS_DIVIDE_VALUE;
				case GamepadDInput::RIGHT_X_AXIS:
					return (float)currentState[id].lRx / GamepadDInput::AXIS_DIVIDE_VALUE;
				case GamepadDInput::RIGHT_Y_AXIS:
					return (float)currentState[id].lRy / GamepadDInput::AXIS_DIVIDE_VALUE;
				case GamepadDInput::EXTRA_AXIS1:
					return (float)currentState[id].lZ / GamepadDInput::AXIS_DIVIDE_VALUE;
				case GamepadDInput::EXTRA_AXIS2:
					return (float)currentState[id].lRz / GamepadDInput::AXIS_DIVIDE_VALUE;
				case GamepadDInput::SLIDER1:
					return (float)currentState[id].rglSlider[0] / GamepadDInput::AXIS_DIVIDE_VALUE;
				case GamepadDInput::SLIDER2:
					return (float)currentState[id].rglSlider[1] / GamepadDInput::AXIS_DIVIDE_VALUE;
				default:
					return 0.0f;
				}
			}
			return 0.0f;
		}

		BOOL CALLBACK GamepadDInput::EnumerateDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID uselessJunk)
		{
			//check if it is an xinput device later

			if (!IsXInputDevice(&lpddi->guidProduct))
			{
				LPDIRECTINPUTDEVICE8 testDevice;
				inface->CreateDevice(lpddi->guidInstance, &testDevice, NULL);

				testDevice->SetDataFormat(&c_dfDIJoystick);
				DWORD fore = ((foreground) ? DISCL_FOREGROUND : DISCL_BACKGROUND);
				DWORD excl = ((exclusive) ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE);

				testDevice->SetCooperativeLevel(windowHandle, fore | excl);

				testDevice->Acquire();

				DeviceStruct newDevice = { lpddi, testDevice };

				devices.push_back(newDevice);
				currentState.push_back(DIJOYSTATE());
				previousState.push_back(DIJOYSTATE());
				connected.push_back(true);
			}

			return DIENUM_CONTINUE;
		}

		void GamepadDInput::findDevices()
		{
			inface->EnumDevices(DI8DEVCLASS_GAMECTRL, GamepadDInput::EnumerateDeviceCallback, 0, DIEDFL_ATTACHEDONLY);
		}

		bool GamepadDInput::init(HWND windowHandle, bool Foreground = true, bool Exclusive = true)
		{
			if (windowHandle != NULL)
			{
				GamepadDInput::exclusive = Exclusive;
				GamepadDInput::foreground = Foreground;
				GamepadDInput::windowHandle = windowHandle;
			}
			else
			{
				GamepadDInput::exclusive = false;
				GamepadDInput::foreground = false;
				GamepadDInput::windowHandle = GetDesktopWindow();
			}

			HINSTANCE hins = GetModuleHandle(NULL);

			HRESULT err = DirectInput8Create(hins, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)& inface, NULL);

			if (err != 0)
			{
				return false;
			}
			
			findDevices();

			return true;
		}

		void GamepadDInput::refindDevices()
		{
			for (int i = 0; i < devices.size(); i++)
			{
				devices[i].gamepadDevice->Unacquire();
				devices[i].gamepadDevice->Release();
			}

			currentState.clear();
			previousState.clear();
			connected.clear();
			devices.clear();

			findDevices();
		}

		void GamepadDInput::dispose()
		{
			for (int i = 0; i < devices.size(); i++)
			{
				devices[i].gamepadDevice->Unacquire();
				devices[i].gamepadDevice->Release();
			}

			currentState.clear();
			previousState.clear();
			connected.clear();
			devices.clear();

			inface->Release();
		}

		int GamepadDInput::getSize()
		{
			return devices.size();
		}

		GUID GamepadDInput::getGUID(int id)
		{
			if (id < devices.size())
			{
				return devices[id].deviceInfo->guidInstance;
			}
			return GUID();
		}

		LPCDIDEVICEINSTANCE GamepadDInput::getDeviceInfo(int id)
		{
			if (id < devices.size())
			{
				return devices[id].deviceInfo;
			}
			return LPCDIDEVICEINSTANCE();
		}

		DIJOYSTATE GamepadDInput::getJoystickValues(int id)
		{
			if (id < devices.size())
			{
				DIJOYSTATE state;
				HRESULT err = devices[id].gamepadDevice->GetDeviceState(sizeof(DIJOYSTATE), &state);

				if (FAILED(err))
				{
					//possibly disconnected
					//try reconnect once
					devices[id].gamepadDevice->Acquire();

					err = devices[id].gamepadDevice->GetDeviceState(sizeof(DIJOYSTATE), &state);
					if (FAILED(err))
					{
						connected[id] = false;
					}
					else
					{
						connected[id] = true;
					}
				}
				else
				{
					connected[id] = true;
				}

				return state;
			}

			return DIJOYSTATE();
		}

	} //NAMESPACE glib END
#endif