#pragma once
#include <Windows.h>
#include <thread>
#include <vector>
#include <mutex>
#include "Image.h"
#include "GuiManager.h"

class WndWindow : public Object
{
public:
	static const int MOUSE_LEFT = 0;
	static const int MOUSE_MIDDLE = 1;
	static const int MOUSE_RIGHT = 2;

	WndWindow();
	WndWindow(std::wstring title, int width = 320, int height = 240, int x = -1, int y = -1);
	WndWindow(std::string title, int width = 320, int height = 240, int x = -1, int y = -1);
	WndWindow(const WndWindow& other);
	~WndWindow();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void setVisible(bool value);

	void setX(int x);
	void setY(int y);
	void setPosition(int x, int y);

	void setWidth(int width);
	void setHeight(int height);
	void setSize(int width, int height);

	int getMouseX();
	int getMouseY();

	int getWidth();
	int getHeight();

	std::wstring getTitle();

	void repaint();

	bool getValid();
	bool getRunning();

	void close();

	HWND getWindowHandle();

	void setPaintFunction(void(*function)(void));
	void setKeyUpFunction(void(*function)(WPARAM, LPARAM));
	void setKeyDownFunction(void(*function)(WPARAM, LPARAM));

	void setMouseButtonDownFunction(void(*function)(int));
	void setMouseButtonUpFunction(void(*function)(int));
	void setMouseHWheelFunction(void(*function)(int));
	void setMouseWheelFunction(void(*function)(int));
	void setMouseButtonMovedFunction(void(*function)(void));

	void setClosingFunction(void(*function)(void));

	GuiManager* getGuiManager();
	void drawImage(Image* g);

private:

	static std::vector<WndWindow*> windowList;
	static int screenWidth;
	static int screenHeight;
	static WndWindow* getWindowByHandle(HWND value);
	static void removeWindowFromList(WndWindow* value);
	static LRESULT _stdcall wndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam);

	void init(int x, int y, int width, int height, std::wstring title);
	
	void setAllFunctionsToNull();
	
	void initBitmap();

	void setRunning(bool value);
	void setValid(bool value);
	void setFinishInit(bool value);
	void setDonePainting(bool value);

	bool getFinishInit();
	bool getDonePainting();
	
	void run();
	void dispose();
	
	int x = 0;
	int y = 0;
	int width = 320;
	int height = 240;
	std::wstring title = L"";

	unsigned char* wndPixels;

	std::thread* wndThread;
	std::mutex myMutex;

	HWND windowHandle;
	WNDCLASSEXW wndClass;
	HINSTANCE hins;
	HBITMAP bitmap;
	BITMAPINFO bitInfo;
	HDC myHDC;

	GuiManager* gui;

	bool valid = true;
	bool running = true;
	bool donePainting = true;

	bool finishedInit = false;

	void (*paintFunction)(void);
	void (*keyUpFunction)(WPARAM, LPARAM);
	void (*keyDownFunction)(WPARAM, LPARAM);

	void (*mouseDoubleClickFunction)(int);
	void (*mouseButtonDownFunction)(int);
	void (*mouseButtonUpFunction)(int);
	void (*mouseWheelFunction)(int);
	void (*mouseHWheelFunction)(int);
	void (*mouseMovedFunction)(void);

	void (*closingFunction)(void);
};
