#pragma once

#include <Windows.h>

#include <chrono>
#include <functional>
#include <string>
#include "Image.h"


class System
{
public:
	System();
	~System();

	static const int LEFT_MOUSE_BUTTON = 0;
	static const int RIGHT_MOUSE_BUTTON = 1;
	static const int MIDDLE_MOUSE_BUTTON = 2;
	static const int MOUSE_WHEEL_UP = 3;
	static const int MOUSE_WHEEL_DOWN = 4;
	static const int MOUSE_WHEEL_LEFT = 5;
	static const int MOUSE_WHEEL_RIGHT = 6;
	static const int X1_MOUSE_BUTTON = 7;
	static const int X2_MOUSE_BUTTON = 8;

	static size_t getCurrentTimeMillis();
	static size_t getCurrentTimeMicro();
	static size_t getCurrentTimeNano();

	static void sleep(int millis, int micros=0);

	static void delayRun(void (*function)(), int millis, int micros = 0);
	static void delayRun(std::function<void()> function, int millis, int micros = 0);

	static unsigned int getNumberOfThreads();

	static void dispose();

	static const long NANOSECOND_SEC = 1000000000L;
	static const long MICROSECOND_SEC = 1000000L;
	static const long MILLISECOND_SEC = 1000L;

	static void emulateKeypress(int key);
	static void emulateKeyRelease(int key);

	static void emulateMousePress(int button);
	static void emulateMouseRelease(int button);

	static void emulateMouseWheel(int wheel, int amount);

	static void setMousePosition(int x, int y);
	static int getMouseX();
	static int getMouseY();
	
	static bool getKeyDown(int key);
	static bool getKeyToggled(int key);//Use the Windows VK constants
	static bool getMouseDown(int value);

	static int getDesktopWidth();
	static int getDesktopHeight();

	static int getAmountOfMonitors();

	static void saveScreenShot(HWND window, std::string filename);
	static void saveScreenShot(HWND window, std::wstring filename);
	static Image* getScreenShot(HWND window);

	static void saveScreenShotDesktop(std::string filename);
	static void saveScreenShotDesktop(std::wstring filename);
	static Image* getScreenShotDesktop();

	static void paintImageToDesktop(Image* img, int startX = 0, int startY = 0);
	static void paintImageToWindow(HWND window, Image* img, int startX = 0, int startY = 0);

	static unsigned long getProcessID(std::wstring processName);
	static HWND getProcessWindow(std::wstring windowName);
	
private:
	static unsigned int numberOfThreads;
};

