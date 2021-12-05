#include "SimpleWindow.h"
#include <iostream>
#include "MathExt.h"
#include "System.h"
#include "StringTools.h"
#include "Input.h"

namespace glib
{

	std::vector<SimpleWindow*> SimpleWindow::windowList = std::vector<SimpleWindow*>();
	int SimpleWindow::screenWidth = System::getDesktopWidth();
	int SimpleWindow::screenHeight = System::getDesktopHeight();

	int* SimpleWindow::mouseVWheelPointer = nullptr;
	int* SimpleWindow::mouseHWheelPointer = nullptr;

	const Class SimpleWindow::myClass = Class("SimpleWindow", {&Object::myClass});
	const Class* SimpleWindow::getClass()
	{
		return &SimpleWindow::myClass;
	}

	SimpleWindow* SimpleWindow::getWindowByHandle(size_t handle)
	{
		for (int i = 0; i < windowList.size(); i++)
		{
			if (handle == windowList[i]->getWindowHandle())
			{
				return windowList[i];
			}
		}

		return nullptr;
	}

	void SimpleWindow::removeWindowFromList(SimpleWindow* wnd)
	{
		int swapIndex = -1;

		for (int i = 0; i < windowList.size(); i++)
		{
			if (wnd == windowList[i])
			{
				swapIndex = i;
			}
		}

		if (swapIndex != -1)
		{
			windowList[swapIndex] = windowList[windowList.size() - 1];
			windowList.pop_back();
		}
	}
	
	LRESULT _stdcall SimpleWindow::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		SimpleWindow* currentWindow = SimpleWindow::getWindowByHandle((size_t)hwnd);

		bool canDo = false;

		HDC hdc;
		PAINTSTRUCT ps;
		HDC mem;
		BITMAP img;
		HGDIOBJ oldImg;
		RECT* rect = nullptr;

		if (currentWindow != nullptr)
		{
			switch (msg)
			{
			case WM_ERASEBKGND:
				break;
			case WM_PAINT:
				hdc = BeginPaint(hwnd, &ps);

				mem = CreateCompatibleDC(hdc);
				oldImg = SelectObject(mem, currentWindow->bitmap);
				GetObject(currentWindow->bitmap, sizeof(BITMAP), &img);

				BitBlt(hdc, 0, 0, img.bmWidth, img.bmHeight, mem, 0, 0, SRCCOPY);

				DeleteDC(mem);
				EndPaint(hwnd, &ps);
				break;
			case WM_CLOSE:
				if (currentWindow->closingFunction != nullptr)
					currentWindow->closingFunction();
				PostQuitMessage(0);
				currentWindow->setShouldEnd(true);
				break;
			case WM_DESTROY:
				/*
				if (currentWindow->closingFunction != nullptr)
					currentWindow->closingFunction();
				PostQuitMessage(0);
				currentWindow->setRunning(false);
				*/
				break;
			case WM_KEYDOWN:
				if (currentWindow->keyDownFunction != nullptr)
					currentWindow->keyDownFunction(wparam, lparam);
				break;
			case WM_KEYUP:
				if (currentWindow->keyUpFunction != nullptr)
					currentWindow->keyUpFunction(wparam, lparam);
				break;
			case WM_LBUTTONDOWN:
				if (currentWindow->mouseButtonDownFunction != nullptr)
					currentWindow->mouseButtonDownFunction(MOUSE_LEFT);
				break;
			case WM_MBUTTONDOWN:
				if (currentWindow->mouseButtonDownFunction != nullptr)
					currentWindow->mouseButtonDownFunction(MOUSE_MIDDLE);
				break;
			case WM_RBUTTONDOWN:
				if (currentWindow->mouseButtonDownFunction != nullptr)
					currentWindow->mouseButtonDownFunction(MOUSE_RIGHT);
				break;
			case WM_LBUTTONUP:
				if (currentWindow->mouseButtonUpFunction != nullptr)
					currentWindow->mouseButtonUpFunction(MOUSE_LEFT);
				break;
			case WM_MBUTTONUP:
				if (currentWindow->mouseButtonUpFunction != nullptr)
					currentWindow->mouseButtonUpFunction(MOUSE_MIDDLE);
				break;
			case WM_RBUTTONUP:
				if (currentWindow->mouseButtonUpFunction != nullptr)
					currentWindow->mouseButtonUpFunction(MOUSE_RIGHT);
				break;
			case WM_MOUSEWHEEL:
				if (currentWindow->mouseWheelFunction != nullptr)
					currentWindow->mouseWheelFunction(GET_WHEEL_DELTA_WPARAM(wparam)/120);
				
				if(SimpleWindow::mouseVWheelPointer)
					*SimpleWindow::mouseVWheelPointer = GET_WHEEL_DELTA_WPARAM(wparam)/120;
				break;
			case WM_MOUSEHWHEEL:
				if (currentWindow->mouseHWheelFunction != nullptr)
					currentWindow->mouseHWheelFunction(GET_WHEEL_DELTA_WPARAM(wparam)/120);
				
				if(SimpleWindow::mouseHWheelPointer)
					*SimpleWindow::mouseHWheelPointer = GET_WHEEL_DELTA_WPARAM(wparam)/120;
				break;
			case WM_MOUSEMOVE:
				if (currentWindow->mouseMovedFunction != nullptr)
					currentWindow->mouseMovedFunction();
				break;
			case WM_MOVING:
				rect = (RECT*)lparam;
				currentWindow->x = rect->left;
				currentWindow->y = rect->top;
				
				currentWindow->width = rect->right - rect->left;
				currentWindow->height = rect->bottom - rect->top;
				break;
			case WM_ENTERSIZEMOVE:
				currentWindow->setResizing(true);
				break;
			case WM_SIZING:
				rect = (RECT*)lparam;
				
				currentWindow->preX = rect->left;
				currentWindow->preY = rect->top;

				currentWindow->x = rect->left;
				currentWindow->y = rect->top;

				currentWindow->width = rect->right - rect->left;
				currentWindow->height = rect->bottom - rect->top;
				break;
			case WM_SIZE:
				if(wparam == SIZE_MAXIMIZED)
				{
					currentWindow->windowState = STATE_MAXIMIZED;
					//FIX LATER
					if(currentWindow->getResizing())
					{
						currentWindow->preX = currentWindow->x;
						currentWindow->preY = currentWindow->y;

						currentWindow->x = 0;
						currentWindow->y = 0;

						currentWindow->width = LOWORD(lparam);
						currentWindow->height = HIWORD(lparam);
					}
					else
					{
						currentWindow->setResizeMe(true);

						currentWindow->preX = currentWindow->x;
						currentWindow->preY = currentWindow->y;

						currentWindow->x = 0;
						currentWindow->y = 0;

						currentWindow->width = LOWORD(lparam);
						currentWindow->height = HIWORD(lparam);
					}
				}
				else if(wparam == SIZE_RESTORED)
				{
					currentWindow->windowState = STATE_NORMAL;
					//FIX LATER
					if(currentWindow->getResizing())
					{
						currentWindow->x = currentWindow->preX;
						currentWindow->y = currentWindow->preY;

						currentWindow->width = LOWORD(lparam);
						currentWindow->height = HIWORD(lparam);
					}
					else
					{
						currentWindow->setResizeMe(true);

						currentWindow->x = currentWindow->preX;
						currentWindow->y = currentWindow->preY;

						currentWindow->width = LOWORD(lparam);
						currentWindow->height = HIWORD(lparam);
					}
				}
				else if(wparam == SIZE_MINIMIZED)
				{
					currentWindow->preX = currentWindow->x;
					currentWindow->preY = currentWindow->y;

					currentWindow->width = LOWORD(lparam);
					currentWindow->height = HIWORD(lparam);

					currentWindow->windowState = STATE_MINIMIZED;
				}
				break;
			case WM_EXITSIZEMOVE:
				currentWindow->initBitmap();
				currentWindow->setResizing(false);
				break;
			case WM_MDIMAXIMIZE:
				break;
			case WM_SYSCOMMAND:
				if(wparam == SC_MOVE)
				{
					if(!currentWindow->getMovable())
					{
						return 0;
					}
				}
				else if(wparam == SC_SIZE)
				{
					if(!currentWindow->getResizable())
					{
						return 0;
					}
				}
				break;
			case WM_SETCURSOR:
				SetCursor( LoadCursor(NULL, IDC_ARROW) );
			default:
				break;
			}
		}

		return DefWindowProcW(hwnd, msg, wparam, lparam);
	}

	SimpleWindow::SimpleWindow()
	{
		x = screenWidth / 2 - 160;
		y = screenHeight / 2 - 120;
		width = 320;
		height = 240;
		title = L"";

		setAllFunctionsToNull();

		gui = new GuiManager(this->width, this->height);

		int threadManaged = windowType & 0b0100;
		
		if(threadManaged == TYPE_THREAD_MANAGED)
		{
			threadOwnership = true;
			wndThread = new std::thread(&SimpleWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
		}
		else
		{
			threadOwnership = false;
			init(this->x, this->y, this->width, this->height, this->title, this->windowType);
		}

		while (getFinishInit() != true)
		{
			std::this_thread::yield();
		}
	}

	SimpleWindow::SimpleWindow(std::wstring title, int width, int height, int x, int y, unsigned char windowType)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;

		this->windowType = windowType;

		if (this->width < 0)
			this->width = 320;

		if (this->height < 0)
			this->height = 240;

		if (this->x < 0)
			this->x = screenWidth / 2 - (width/2);

		if (this->y < 0)
			this->y = screenHeight / 2 - (height/2);


		setAllFunctionsToNull();
		gui = new GuiManager(this->width, this->height);
		
		this->title = title;

		int threadManaged = windowType & 0b0100;
		
		if(threadManaged == TYPE_THREAD_MANAGED)
		{
			threadOwnership = true;
			wndThread = new std::thread(&SimpleWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
		}
		else
		{
			threadOwnership = false;
			init(this->x, this->y, this->width, this->height, this->title, this->windowType);
		}

		while (getFinishInit() != true)
		{
			std::this_thread::yield();
		}
	}

	SimpleWindow::SimpleWindow(std::string title, int width, int height, int x, int y, unsigned char windowType)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		
		this->windowType = windowType;

		if (this->width < 0)
			this->width = 320;

		if (this->height < 0)
			this->height = 240;

		if (this->x < 0)
			this->x = screenWidth / 2 - (width/2);

		if (this->y < 0)
			this->y = screenHeight / 2 - (height/2);

		setAllFunctionsToNull();
		gui = new GuiManager(this->width, this->height);

		this->title = StringTools::toWideString(title);

		int threadManaged = windowType & 0b0100;

		if(threadManaged == TYPE_THREAD_MANAGED)
		{
			threadOwnership = true;
			wndThread = new std::thread(&SimpleWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
		}
		else
		{
			threadOwnership = false;
			init(this->x, this->y, this->width, this->height, this->title, this->windowType);
		}

		while (getFinishInit() != true)
		{
			std::this_thread::yield();
		}
	}

	SimpleWindow::~SimpleWindow()
	{
		dispose();
	}

	void SimpleWindow::close()
	{
		if(closingFunction!=nullptr)
			closingFunction();
		
		dispose();
	}

	void SimpleWindow::dispose()
	{
		if (getValid())
		{
			std::wstring text = title;
			text += L"_CLASS";

			setShouldEnd(true);
			setValid(false);

			if (wndThread != nullptr)
			{
				if (wndThread->joinable())
					wndThread->join();
				
				wndThread = nullptr;
			}
			
			if (IsWindow((HWND)windowHandle))
			{
				CloseWindow((HWND)windowHandle);
				DestroyWindow((HWND)windowHandle);
			}
			UnregisterClassW(text.c_str(), hins);
			DeleteObject(bitmap);
			DeleteDC(myHDC);

			if(gui!=nullptr)
				delete gui;
			gui = nullptr;

			if(wndPixels!=nullptr)
				delete[] wndPixels;
			
			wndPixels = nullptr;
				
			SimpleWindow::removeWindowFromList(this);
		}
	}

	void SimpleWindow::init(int x, int y, int width, int height, std::wstring title, unsigned char windowType)
	{
		#ifdef LINUX

			displayServer = XOpenDisplay(NULL);

			if(display == NULL)
			{
				//error connecting to XServer
				setValid(false);
				setShouldEnd(true);
				setRunning(false);

				setFinishInit(true);
				return;
			}

			int trueWidth = width;
			int trueHeight = height;
			int trueX = x;
			int trueY = y;

			int wType = windowType & 0b0001;
			if(windowType & 0b1000)
				wType = FULLSCREEN_WINDOW;
			
			int focusable = windowType & 0b0010;
			int threadManaged = windowType & 0b0100;

			int borderWidth = 1;

			if(focusable==TYPE_NONFOCUSABLE)
			{
				
			}

			screen = DefaultScreen(displayServer);
			windowHandle = XCreateSimpleWindow(displayServer, RootWindow(displayServer, screen), trueX, trueY, trueWidth, trueHeight, borderWidth, BlackPixel(display, screen), WhitePixel(display, screen));

			//set window name
			std::string cTitle = StringTools::toCString(title);
			XStoreName(displayServer, windowHandle, cTitle.c_str());

			//if window is borderless
			if(windowType == SimpleWindow::BORDERLESS_WINDOW)
			{
				Atom mwmHintsProperty = XInternAtom(displayServer, "_MOTIF_WM_HINTS", 0);
				MwmHints hints = {};
				hints.flags = MWM_HINTS_DECORATIONS;
				hints.decorations = 0;
				XChangeProperty(displayServer, windowHandle, mwmHintsProperty, mwmHintsProperty, 32, PropModeReplace, (unsigned char*)&hints, 5);
			}
			else if(windowType == SimpleWindow::FULLSCREEN_WINDOW)
			{
				Atom atoms[2] = {
					XInternAtom(displayServer, "_NET_WM_STATE_FULLSCREEN", 0), 0
				};
				XChangeProperty(displayServer, windowHandle, XInternAtom(displayServer, "_NET_WM_STATE", 0), XA_ATOM, 32, PropModeReplace, (unsigned char*)atoms, 1);

				this->x = 0;
				this->y = 0;
				this->width = System::getDestopWidth();
				this->height = System::getDesktopHeight();
			}
			

			XSelectInput(displayServer, windowHandle, ExposureMask | KeyPressMask);
			XMapWindow(displayServer, windowHandle);

			setVisible(true);
			setValid(true);
			setShouldEnd(false);
			setRunning(true);

			initBitmap();
			SimpleWindow::windowList.push_back(this);

			setFinishInit(true);

			if(threadManaged == TYPE_THREAD_MANAGED)
				run();
		#else

			std::wstring text = title;
			text += L"_CLASS";

			hins = GetModuleHandle(NULL);

			wndClass.cbClsExtra = 0;
			wndClass.cbSize = sizeof(WNDCLASSEX);
			wndClass.cbWndExtra = 0;
			wndClass.hbrBackground = (HBRUSH)(BLACK_BRUSH);
			wndClass.hCursor = LoadCursor(hins, IDC_ARROW);
			wndClass.hIcon = LoadIcon(hins, IDI_APPLICATION);
			wndClass.hIconSm = LoadIcon(hins, IDI_APPLICATION);
			wndClass.hInstance = hins;
			wndClass.lpfnWndProc = SimpleWindow::wndProc;
			wndClass.lpszClassName = text.c_str();
			wndClass.lpszMenuName = NULL;
			wndClass.style = CS_HREDRAW | CS_VREDRAW;

			int trueWidth = width;
			int trueHeight = height;
			int trueX = x;
			int trueY = y;

			if (RegisterClassExW(&wndClass) != NULL)
			{
				int wType = windowType & 0b0001;
				if(windowType & 0b1000)
					wType = FULLSCREEN_WINDOW;
				
				int focusable = windowType & 0b0010;
				int threadManaged = windowType & 0b0100;

				DWORD style = 0;
				bool failed = false;

				switch(wType)
				{
					case SimpleWindow::NORMAL_WINDOW:
						style = WS_OVERLAPPEDWINDOW;
						trueWidth += 16;
						trueHeight += 40;
						break;
					case SimpleWindow::BORDERLESS_WINDOW:
						style = WS_POPUP|WS_VISIBLE|WS_SYSMENU;
						break;
					default:
						style = WS_OVERLAPPEDWINDOW;
						trueWidth += 16;
						trueHeight += 40;
						break;
				}

				if(wType == SimpleWindow::FULLSCREEN_WINDOW)
				{
					HMONITOR hmon = MonitorFromWindow(NULL, MONITOR_DEFAULTTONEAREST);
					MONITORINFO mi = { sizeof(MONITORINFO) };

					style = WS_POPUP|WS_VISIBLE;
					if(!GetMonitorInfo(hmon, &mi))
					{	
						//failed getting monitor information
						failed = true;
						UnregisterClassW(text.c_str(), hins);
						setValid(false);
						setShouldEnd(true);
						setRunning(false);

						setFinishInit(true);

						#ifdef USE_EXCEPTIONS
							throw WindowCreationError();
						#endif
					}
					
					trueX = mi.rcMonitor.left;
					trueY = mi.rcMonitor.top;
					trueWidth = mi.rcMonitor.right - mi.rcMonitor.left;
					trueHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

					this->width = trueWidth;
					this->height = trueHeight;
					this->x = trueX;
					this->y = trueY;
				}

				if(focusable==TYPE_NONFOCUSABLE)
				{
					style |= WS_EX_NOACTIVATE;
				}

				if(!failed)
				{
					windowHandle = (size_t)CreateWindowExW(NULL, text.c_str(), title.c_str(), style, trueX, trueY, trueWidth, trueHeight, NULL, NULL, hins, NULL);
				}
				
				if (windowHandle != NULL)
				{
					setVisible(true);
					setValid(true);
					setShouldEnd(false);
					setRunning(true);

					initBitmap();
					SimpleWindow::windowList.push_back(this);

					setFinishInit(true);

					if(threadManaged == TYPE_THREAD_MANAGED)
						run();
				}
				else
				{
					//dispose
					//The class registered, but the window creation failed
					UnregisterClassW(text.c_str(), hins);
					setValid(false);
					setShouldEnd(true);
					setRunning(false);

					setFinishInit(true);
					//std::cout << "Failed window creation" << std::endl;
				}
			}
			else
			{
				//dispose
				//Nothing because the window wasn't created and the
				//class failed to register
				setValid(false);
				setShouldEnd(true);
				setRunning(false);

				setFinishInit(true);
			}

		#endif

	}

	void SimpleWindow::initBitmap()
	{
		bitInfo.bmiHeader.biSize = sizeof(bitInfo.bmiHeader);
		bitInfo.bmiHeader.biWidth = width;
		bitInfo.bmiHeader.biHeight = -height;
		bitInfo.bmiHeader.biCompression = BI_RGB;
		bitInfo.bmiHeader.biPlanes = 1;
		bitInfo.bmiHeader.biBitCount = 24;
		bitInfo.bmiHeader.biSizeImage = 0;

		bitInfo.bmiHeader.biXPelsPerMeter = width;
		bitInfo.bmiHeader.biYPelsPerMeter = height;

		bitInfo.bmiHeader.biClrImportant = 0;
		bitInfo.bmiHeader.biClrUsed = 0;

		bitInfo.bmiColors->rgbRed = 0;
		bitInfo.bmiColors->rgbGreen = 0;
		bitInfo.bmiColors->rgbBlue = 0;
		bitInfo.bmiColors->rgbReserved = 0;

		scanLinePadding = width%4;

		wndPixelsSize = (width*3 + scanLinePadding) * height;

		if(wndPixels != nullptr)
		{
			DeleteObject(bitmap);
			DeleteDC(myHDC);
			delete[] wndPixels;
		}

		wndPixels = new unsigned char[wndPixelsSize];

		std::memset(wndPixels,0,wndPixelsSize);
		myHDC = GetDC((HWND)windowHandle);

		bitmap = CreateCompatibleBitmap(myHDC, width, height);

		if(gui!=nullptr)
		{
			gui->resizeImage(width, height);
		}
	}

	void SimpleWindow::setRunning(bool value)
	{
		myMutex.lock();
		running = value;
		myMutex.unlock();
	}

	bool SimpleWindow::getRunning()
	{
		bool v = true;
		myMutex.lock();
		v = running;
		myMutex.unlock();

		return v;
	}

	void SimpleWindow::setValid(bool value)
	{
		myMutex.lock();
		valid = value;
		myMutex.unlock();
	}

	void SimpleWindow::setFinishInit(bool value)
	{
		myMutex.lock();
		finishedInit = value;
		myMutex.unlock();
	}

	void SimpleWindow::setResizing(bool value)
	{
		resizing = value;
	}

	void SimpleWindow::setResizeMe(bool value)
	{
		resizeMe = value;
	}

	bool SimpleWindow::getFinishInit()
	{
		return finishedInit;
	}

	bool SimpleWindow::getResizing()
	{
		return resizing;
	}

	bool SimpleWindow::getResizeMe()
	{
		return resizeMe;
	}

	bool SimpleWindow::getRepaint()
	{
		bool v;
		myMutex.lock();
		if(autoRepaint)
			v = true;
		else
			v = shouldRepaint;
		myMutex.unlock();

		return v;
	}

	void SimpleWindow::setThreadAutoRepaint(bool v)
	{
		myMutex.lock();
		autoRepaint = v;
		myMutex.unlock();
	}

	void SimpleWindow::waitTillClose()
	{
		while(getRunning())
		{
			System::sleep(1);
		}
	}

	void SimpleWindow::setVisible(bool value)
	{
		if (value == true)
			ShowWindow((HWND)windowHandle, SW_SHOW);
		else
			ShowWindow((HWND)windowHandle, SW_HIDE);
	}

	void SimpleWindow::setX(int x)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, x, this->y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->x = x;
	}

	void SimpleWindow::setY(int y)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->y = y;
	}

	void SimpleWindow::setPosition(int x, int y)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->x = x;
		this->y = y;
	}

	void SimpleWindow::setWidth(int width)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->width = width;
	}

	void SimpleWindow::setHeight(int height)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, this->width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->height = height;
	}

	void SimpleWindow::setSize(int width, int height)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->width = width;
		this->height = height;
	}

	int SimpleWindow::getMouseX()
	{
		POINT p;
		GetCursorPos(&p);
		int borderWidth = GetSystemMetrics(SM_CXFRAME);
		
		switch (windowType)
		{
		case SimpleWindow::NORMAL_WINDOW:
			return p.x-(x+borderWidth);
			break;
		case SimpleWindow::BORDERLESS_WINDOW:
			return p.x-x;
			break;
		case SimpleWindow::FULLSCREEN_WINDOW:
			return p.x-x;
			break;
		default:
			return p.x-(x+borderWidth);
			break;
		}

		return 0;
	}

	int SimpleWindow::getMouseY()
	{
		POINT p;
		GetCursorPos(&p);
		int borderHeight = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
    							GetSystemMetrics(SM_CXPADDEDBORDER));

		switch (windowType)
		{
		case SimpleWindow::NORMAL_WINDOW:
			return p.y-(y+borderHeight);
			break;
		case SimpleWindow::BORDERLESS_WINDOW:
			return p.y-y;
			break;
		case SimpleWindow::FULLSCREEN_WINDOW:
			return p.x-x;
			break;
		default:
			return p.y-(y+borderHeight);
			break;
		}

		return 0;
	}

	int SimpleWindow::getWidth()
	{
		return width;
	}

	int SimpleWindow::getHeight()
	{
		return height;
	}

	int SimpleWindow::getX()
	{
		return x;
	}

	int SimpleWindow::getY()
	{
		return y;
	}

	std::wstring SimpleWindow::getTitle()
	{
		return title;
	}

	int SimpleWindow::getImageSize()
	{
		return wndPixelsSize;
	}

	bool SimpleWindow::getValid()
	{
		bool value = true;

		myMutex.lock();
		value = this->valid;
		myMutex.unlock();

		return value;
	}

	size_t SimpleWindow::getWindowHandle()
	{
		return windowHandle;
	}

	void SimpleWindow::setAllFunctionsToNull()
	{
		paintFunction = nullptr;
		keyUpFunction = nullptr;
		keyDownFunction = nullptr;
		mouseDoubleClickFunction = nullptr;
		mouseButtonDownFunction = nullptr;
		mouseButtonUpFunction = nullptr;
		mouseWheelFunction = nullptr;
		mouseHWheelFunction = nullptr;
		mouseMovedFunction = nullptr;
		closingFunction = nullptr;
	}

	void SimpleWindow::setPaintFunction(std::function<void()> function)
	{
		paintFunction = function;
	}

	void SimpleWindow::setMouseMovedFunction(std::function<void()> function)
	{
		mouseMovedFunction = function;
	}

	void SimpleWindow::setClosingFunction(std::function<void()> function)
	{
		closingFunction = function;
	}

	void SimpleWindow::setKeyUpFunction(std::function<void(unsigned long, long)> function)
	{
		keyUpFunction = function;
	}

	void SimpleWindow::setKeyDownFunction(std::function<void(unsigned long, long)> function)
	{
		keyDownFunction = function;
	}

	void SimpleWindow::setMouseButtonDownFunction(std::function<void(int)> function)
	{
		mouseButtonDownFunction = function;
	}

	void SimpleWindow::setMouseButtonUpFunction(std::function<void(int)> function)
	{
		mouseButtonUpFunction = function;
	}

	void SimpleWindow::setMouseHWheelFunction(std::function<void(int)> function)
	{
		mouseHWheelFunction = function;
	}

	void SimpleWindow::setMouseWheelFunction(std::function<void(int)> function)
	{
		mouseWheelFunction = function;
	}

	GuiManager* SimpleWindow::getGuiManager()
	{
		return gui;
	}

	void SimpleWindow::setActivateGui(bool v)
	{
		activateGui = v;
	}

	bool SimpleWindow::getActivateGui()
	{
		return activateGui;
	}

	void SimpleWindow::setMouseVWheelValuePointer(int* v)
	{
		SimpleWindow::mouseVWheelPointer = v;
	}

	void SimpleWindow::setMouseHWheelValuePointer(int* v)
	{
		SimpleWindow::mouseHWheelPointer = v;
	}

	void SimpleWindow::setFocus(bool v)
	{
		if(canFocus)
		{
			setShouldFocus(v);
		}
	}

	bool SimpleWindow::getFocus()
	{
		return GetFocus()==(HWND)windowHandle;
	}

	bool SimpleWindow::getCanFocus()
	{
		return canFocus;
	}

	void SimpleWindow::setShouldFocus(bool v)
	{
		myMutex.lock();
		shouldFocus = v;
		myMutex.unlock();
	}

	void SimpleWindow::threadSetFocus()
	{
		SetFocus((HWND)windowHandle);
	}

	bool SimpleWindow::getShouldFocus()
	{
		myMutex.lock();
		bool v = shouldFocus;
		myMutex.unlock();
		return v;
	}

	
	void SimpleWindow::setShouldEnd(bool v)
	{
		myMutex.lock();
		shouldEnd = v;
		if(threadOwnership == false)
			running = false;
		myMutex.unlock();
	}

	bool SimpleWindow::getShouldEnd()
	{
		myMutex.lock();
		bool v = shouldEnd;
		myMutex.unlock();
		return v;
	}
	
	void SimpleWindow::setResizable(bool v)
	{
		canResize = v;
	}

	bool SimpleWindow::getResizable()
	{
		return canResize;
	}

	void SimpleWindow::setMovable(bool v)
	{
		canMove = v;
	}

	bool SimpleWindow::getMovable()
	{
		return canMove;
	}

	void SimpleWindow::setThreadUpdateTime(unsigned int millis, unsigned int micros)
	{
		myMutex.lock();
		sleepTimeMillis = millis;
		sleepTimeMicros = micros;
		myMutex.unlock();
	}

	void SimpleWindow::drawImage(Image* g)
	{
		if (g != nullptr)
		{
			if (g->getWidth() == width && g->getHeight() == height)
			{
				//same size
				Color* imgPixelsStart = g->getPixels();
				unsigned char* wndPixelsStart = wndPixels;
				unsigned char* wndPixelsEnd = wndPixels + wndPixelsSize;
				
				int x = 0;
				int scanLineNum = 0;

				while (wndPixelsStart < wndPixelsEnd)
				{
					*wndPixelsStart = (*imgPixelsStart).blue;
					*(wndPixelsStart + 1) = (*imgPixelsStart).green;
					*(wndPixelsStart + 2) = (*imgPixelsStart).red;

					wndPixelsStart += 3;
					imgPixelsStart++;
					x++;

					if(x>=width)
					{
						scanLineNum++;
						x=0;
						wndPixelsStart += scanLinePadding;
					}
				}
			}
			else
			{
				Color* imgPixelsStart = g->getPixels();
				Color* imgPixelsEnd = imgPixelsStart + (g->getWidth()*g->getHeight());

				unsigned char* wndPixelsStart = wndPixels;
				unsigned char* wndPixelsEnd = wndPixels + wndPixelsSize;

				int side = g->getWidth() - width;
				int absAddAmount = MathExt::abs(g->getWidth() - width);

				//FIX WIDTH AND HEIGHT ADJUST
				
				int tX = 0;

				if (side > 0)
				{
					//img width > window width
					while (wndPixelsStart < wndPixelsEnd && imgPixelsStart < imgPixelsEnd)
					{
						*wndPixelsStart = (*imgPixelsStart).blue;
						*(wndPixelsStart + 1) = (*imgPixelsStart).green;
						*(wndPixelsStart + 2) = (*imgPixelsStart).red;

						imgPixelsStart++;
						wndPixelsStart += 3;
						tX++;
						
						if (tX >= width)
						{
							wndPixelsStart += scanLinePadding;
							imgPixelsStart += absAddAmount;
							tX = 0;
						}
					}
				}
				else
				{
					//FIX WIDTH AND HEIGHT ADJUST
					//img width < window width
					while (wndPixelsStart < wndPixelsEnd && imgPixelsStart < imgPixelsEnd)
					{
						*wndPixelsStart = (*imgPixelsStart).blue;
						*(wndPixelsStart + 1) = (*imgPixelsStart).green;
						*(wndPixelsStart + 2) = (*imgPixelsStart).red;

						imgPixelsStart++;
						wndPixelsStart += 3;
						tX++;

						if (tX >= g->getWidth())
						{
							wndPixelsStart += absAddAmount*3 + scanLinePadding;
							tX = 0;
						}
					}
				}
			}
		}
	}

	void SimpleWindow::run()
	{
		while (!getShouldEnd())
		{
			time_t t1 = System::getCurrentTimeMicro();
			
			threadUpdate();
			threadGuiUpdate();

			if(getRepaint())
				threadRepaint();

			time_t timeNeeded = 0;
			myMutex.lock();
			timeNeeded = sleepTimeMillis*1000 + sleepTimeMicros;
			myMutex.unlock();

			time_t t2 = System::getCurrentTimeMicro();
			time_t timePassed = t2-t1;
			
			time_t waitTime = timeNeeded - timePassed;

			System::sleep(waitTime/1000, waitTime%1000);
		}

		setRunning(false);
	}

	void SimpleWindow::threadUpdate()
	{
		MSG m;
		ZeroMemory(&m, sizeof(MSG));
		while(PeekMessage(&m, (HWND)windowHandle, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&m);
			DispatchMessage(&m);
		}
		
		if(getShouldFocus())
		{
			threadSetFocus();
		}
	}

	void SimpleWindow::threadGuiUpdate()
	{
		Input::pollInput();
		if(windowState != STATE_MINIMIZED)
		{
			if (gui != nullptr && activateGui)
			{
				if(windowType == SimpleWindow::NORMAL_WINDOW)
				{
					if(windowState != STATE_MAXIMIZED)
					{
						gui->setWindowX(x+8);
						gui->setWindowY(y+32);
					}
					else
					{
						gui->setWindowX(x);
						gui->setWindowY(y+24);
					}
				}
				else
				{
					gui->setWindowX(x);
					gui->setWindowY(y);
				}

				gui->updateGuiElements();
			}
		}
	}

	void SimpleWindow::threadRender()
	{
		if(windowState != STATE_MINIMIZED)
		{
			if (gui != nullptr && activateGui)
			{
				gui->renderGuiElements();
				drawImage(gui->getImage());
			}
		}

		if (paintFunction != nullptr)
			paintFunction();
	}

	void SimpleWindow::threadRepaint()
	{
		//draw and send redraw message
		if(getResizing())
		{
			return;
		}

		if(getResizeMe())
		{
			initBitmap();
			setResizeMe(false);
		}

		if(windowState != STATE_MINIMIZED)
		{
			threadRender();
			int value = SetDIBits(myHDC, bitmap, 0, height, &wndPixels[0], &bitInfo, DIB_RGB_COLORS);
			
			RedrawWindow((HWND)windowHandle, NULL, NULL, RDW_INVALIDATE);

			myMutex.lock();
			shouldRepaint=false;
			myMutex.unlock();
		}
	}


	void SimpleWindow::update()
	{
		if(threadOwnership==false)
		{
			threadUpdate();
		}
	}

	void SimpleWindow::repaint()
	{
		if(threadOwnership==false)
		{
			threadRepaint();
		}
		else
		{
			myMutex.lock();
			shouldRepaint=true;
			myMutex.unlock();
		}
	}

	void SimpleWindow::guiUpdate()
	{
		if(threadOwnership==false)
		{
			threadGuiUpdate();
		}
	}

} //NAMESPACE glib END