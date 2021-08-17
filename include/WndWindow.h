#pragma once
#include "Image.h"
#include "GuiManager.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <thread>
#include <vector>
#include <mutex>

namespace glib
{

	class WndWindow : public Object
	{
	public:
		static const unsigned char MOUSE_LEFT = 0x00;
		static const unsigned char MOUSE_MIDDLE = 0x01;
		static const unsigned char MOUSE_RIGHT = 0x02;

		static const unsigned char NORMAL_WINDOW = 0b0000;
		static const unsigned char BORDERLESS_WINDOW = 0b0001;

		static const unsigned char STATE_NORMAL = 0x00;
		static const unsigned char STATE_MAXIMIZED = 0x01;
		static const unsigned char STATE_MINIMIZED = 0x02;
		
		static const unsigned char TYPE_FOCUSABLE = 0b0000;
		static const unsigned char TYPE_NONFOCUSABLE = 0b0010;

		static const unsigned char TYPE_THREAD_MANAGED = 0b0000;
		static const unsigned char TYPE_USER_MANAGED = 0b0100;
		

		WndWindow();
		WndWindow(std::wstring title, int width = 320, int height = 240, int x = -1, int y = -1, unsigned char windowType = 0);
		WndWindow(std::string title, int width = 320, int height = 240, int x = -1, int y = -1, unsigned char windowType = 0);
		~WndWindow();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

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

		int getImageSize();

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
		void setActivateGui(bool v);
		bool getActivateGui();
		
		void drawImage(Image* g);

		void guiUpdate();
		void update();
		void repaint();

		void setThreadAutoRepaint(bool v);
		void setThreadUpdateTime(unsigned int millis, unsigned int micros = 0);

		void waitTillClose();

		//adjustment stuff
		void setFocus(bool v);
		bool getFocus();

		bool getCanFocus();

		void setResizable(bool v);
		bool getResizable();

		void setMovable(bool v);
		bool getMovable();

		static void setMouseVWheelValuePointer(int* v);
		static void setMouseHWheelValuePointer(int* v);

	private:
		
		static std::vector<WndWindow*> windowList;
		static int screenWidth;
		static int screenHeight;
		static WndWindow* getWindowByHandle(HWND value);
		static void removeWindowFromList(WndWindow* value);
		static LRESULT _stdcall wndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam);

		void init(int x, int y, int width, int height, std::wstring title, unsigned char windowType);
		
		void setAllFunctionsToNull();
		
		void initBitmap();

		void setRunning(bool value);
		void setValid(bool value);
		void setFinishInit(bool value);
		void setResizing(bool value);
		void setResizeMe(bool value);

		bool getFinishInit();

		bool getResizing();
		bool getResizeMe();
		bool getRepaint();

		void run();
		void dispose();

		bool shouldFocus = false;
		void threadSetFocus();
		void setShouldFocus(bool v);
		bool getShouldFocus();

		static int* mouseVWheelPointer;
		static int* mouseHWheelPointer;
		
		
		int x = 0;
		int y = 0;
		int width = 320;
		int height = 240;

		unsigned char windowState = STATE_NORMAL;
		unsigned char windowType = NORMAL_WINDOW;

		int preX = 0;
		int preY = 0;
		
		std::wstring title = L"";

		unsigned char* wndPixels = nullptr;

		int wndPixelsSize;
		int scanLinePadding;

		std::thread* wndThread = nullptr;
		std::mutex myMutex = std::mutex();
		bool threadOwnership = true;
		bool shouldRepaint = false;
		bool autoRepaint = true;
		unsigned int sleepTimeMillis = 16;
		unsigned int sleepTimeMicros = 0;

		void threadUpdate();
		void threadGuiUpdate();
		void threadRender();
		void threadRepaint();

		HWND windowHandle;
		WNDCLASSEXW wndClass;
		HINSTANCE hins;
		HBITMAP bitmap;
		BITMAPINFO bitInfo;
		HDC myHDC;

		GuiManager* gui = nullptr;
		bool activateGui = true;

		bool valid = true;
		bool running = true;

		bool resizing = false;
		bool resizeMe = false;

		bool finishedInit = false;

		bool focused = false;
		bool canFocus = true;
		bool canResize = true;
		bool canMove = true;

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

} //NAMESPACE glib END