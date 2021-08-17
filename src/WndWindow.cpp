#include "WndWindow.h"
#include <iostream>
#include "MathExt.h"
#include "System.h"
#include "StringTools.h"
#include "Input.h"

namespace glib
{

	std::vector<WndWindow*> WndWindow::windowList = std::vector<WndWindow*>();
	int WndWindow::screenWidth = System::getDesktopWidth();
	int WndWindow::screenHeight = System::getDesktopHeight();

	int* WndWindow::mouseVWheelPointer = nullptr;
	int* WndWindow::mouseHWheelPointer = nullptr;

	const Class WndWindow::myClass = Class("WndWindow", {&Object::myClass});
	const Class* WndWindow::getClass()
	{
		return &WndWindow::myClass;
	}

	WndWindow* WndWindow::getWindowByHandle(HWND handle)
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

	void WndWindow::removeWindowFromList(WndWindow* wnd)
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

	LRESULT _stdcall WndWindow::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		WndWindow* currentWindow = WndWindow::getWindowByHandle(hwnd);

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
				currentWindow->setRunning(false);
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
				
				if(WndWindow::mouseVWheelPointer)
					*WndWindow::mouseVWheelPointer = GET_WHEEL_DELTA_WPARAM(wparam)/120;
				break;
			case WM_MOUSEHWHEEL:
				if (currentWindow->mouseHWheelFunction != nullptr)
					currentWindow->mouseHWheelFunction(GET_WHEEL_DELTA_WPARAM(wparam)/120);
				
				if(WndWindow::mouseHWheelPointer)
					*WndWindow::mouseHWheelPointer = GET_WHEEL_DELTA_WPARAM(wparam)/120;
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

	WndWindow::WndWindow()
	{
		x = screenWidth / 2 - 160;
		y = screenHeight / 2 - 120;
		width = 320;
		height = 240;
		title = L"";

		setAllFunctionsToNull();

		gui = new GuiManager(new Image(this->width, this->height), true);

		int threadManaged = windowType & 0b0100;
		
		if(threadManaged == TYPE_THREAD_MANAGED)
		{
			threadOwnership = true;
			wndThread = new std::thread(&WndWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
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

	WndWindow::WndWindow(std::wstring title, int width, int height, int x, int y, unsigned char windowType)
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
		gui = new GuiManager(new Image(this->width, this->height), true);
		
		this->title = title;

		int threadManaged = windowType & 0b0100;
		
		if(threadManaged == TYPE_THREAD_MANAGED)
		{
			threadOwnership = true;
			wndThread = new std::thread(&WndWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
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

	WndWindow::WndWindow(std::string title, int width, int height, int x, int y, unsigned char windowType)
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
		gui = new GuiManager(new Image(this->width, this->height), true);

		this->title = StringTools::toWideString(title);

		int threadManaged = windowType & 0b0100;

		if(threadManaged == TYPE_THREAD_MANAGED)
		{
			threadOwnership = true;
			wndThread = new std::thread(&WndWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
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

	WndWindow::~WndWindow()
	{
		dispose();
	}

	void WndWindow::close()
	{
		if(closingFunction!=nullptr)
			closingFunction();
		
		setRunning(false);
		dispose();
	}

	void WndWindow::dispose()
	{
		if (getValid())
		{
			std::wstring text = title;
			text += L"_CLASS";

			setRunning(false);
			setValid(false);

			if (wndThread != nullptr)
			{
				if (wndThread->joinable())
					wndThread->join();
				
				wndThread = nullptr;
			}
			
			if (IsWindow(windowHandle))
			{
				CloseWindow(windowHandle);
				DestroyWindow(windowHandle);
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
				
			WndWindow::removeWindowFromList(this);
		}
	}

	void WndWindow::init(int x, int y, int width, int height, std::wstring title, unsigned char windowType)
	{
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
		wndClass.lpfnWndProc = WndWindow::wndProc;
		wndClass.lpszClassName = text.c_str();
		wndClass.lpszMenuName = NULL;
		wndClass.style = CS_HREDRAW | CS_VREDRAW;

		int trueWidth = width;
		int trueHeight = height;

		if (RegisterClassExW(&wndClass) != NULL)
		{
			int wType = windowType & 0b0001;
			int focusable = windowType & 0b0010;
			int threadManaged = windowType & 0b0100;

			DWORD style = 0;

			switch(windowType)
			{
				case WndWindow::NORMAL_WINDOW:
					style = WS_OVERLAPPEDWINDOW;
					trueWidth += 16;
					trueHeight += 40;
					break;
				case WndWindow::BORDERLESS_WINDOW:
					style = WS_POPUP|WS_VISIBLE|WS_SYSMENU;
					break;
				default:
					style = WS_OVERLAPPEDWINDOW;
					trueWidth += 16;
					trueHeight += 40;
					break;
			}

			if(focusable==TYPE_NONFOCUSABLE)
			{
				style |= WS_EX_NOACTIVATE;
			}

			windowHandle = CreateWindowExW(NULL, text.c_str(), title.c_str(), style, x, y, trueWidth, trueHeight, NULL, NULL, hins, NULL);
			

			if (windowHandle != NULL)
			{
				setVisible(true);
				setValid(true);
				setRunning(true);

				initBitmap();
				WndWindow::windowList.push_back(this);

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
			setRunning(false);

			setFinishInit(true);
			//std::cout << "Failed class creation" << std::endl;
			//std::cout << GetLastError() << std::endl;
		}

	}

	void WndWindow::initBitmap()
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
		myHDC = GetDC(windowHandle);

		bitmap = CreateCompatibleBitmap(myHDC, width, height);

		if(gui!=nullptr)
		{
			gui->resizeImage(width, height);
		}
	}

	void WndWindow::setRunning(bool value)
	{
		myMutex.lock();
		running = value;
		myMutex.unlock();
	}

	bool WndWindow::getRunning()
	{
		bool v = true;
		myMutex.lock();
		v = running;
		myMutex.unlock();

		return v;
	}

	void WndWindow::setValid(bool value)
	{
		myMutex.lock();
		valid = value;
		myMutex.unlock();
	}

	void WndWindow::setFinishInit(bool value)
	{
		myMutex.lock();
		finishedInit = value;
		myMutex.unlock();
	}

	void WndWindow::setResizing(bool value)
	{
		resizing = value;
	}

	void WndWindow::setResizeMe(bool value)
	{
		resizeMe = value;
	}

	bool WndWindow::getFinishInit()
	{
		return finishedInit;
	}

	bool WndWindow::getResizing()
	{
		return resizing;
	}

	bool WndWindow::getResizeMe()
	{
		return resizeMe;
	}

	bool WndWindow::getRepaint()
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

	void WndWindow::setThreadAutoRepaint(bool v)
	{
		myMutex.lock();
		autoRepaint = v;
		myMutex.unlock();
	}

	void WndWindow::waitTillClose()
	{
		while(getRunning())
		{
			System::sleep(1);
		}
	}

	void WndWindow::setVisible(bool value)
	{
		if (value == true)
			ShowWindow(windowHandle, SW_SHOW);
		else
			ShowWindow(windowHandle, SW_HIDE);
	}

	void WndWindow::setX(int x)
	{
		SetWindowPos(windowHandle, HWND_TOP, x, this->y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->x = x;
	}

	void WndWindow::setY(int y)
	{
		SetWindowPos(windowHandle, HWND_TOP, this->x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->y = y;
	}

	void WndWindow::setPosition(int x, int y)
	{
		SetWindowPos(windowHandle, HWND_TOP, x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->x = x;
		this->y = y;
	}

	void WndWindow::setWidth(int width)
	{
		SetWindowPos(windowHandle, HWND_TOP, this->x, this->y, width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->width = width;
	}

	void WndWindow::setHeight(int height)
	{
		SetWindowPos(windowHandle, HWND_TOP, this->x, this->y, this->width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->height = height;
	}

	void WndWindow::setSize(int width, int height)
	{
		SetWindowPos(windowHandle, HWND_TOP, this->x, this->y, width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->width = width;
		this->height = height;
	}

	int WndWindow::getMouseX()
	{
		POINT p;
		GetCursorPos(&p);

		switch (windowType)
		{
		case WndWindow::NORMAL_WINDOW:
			return p.x-(x+16);
			break;
		case WndWindow::BORDERLESS_WINDOW:
			return p.x-x;
			break;
		default:
			return p.x-(x+16);
			break;
		}

		return 0;
	}

	int WndWindow::getMouseY()
	{
		POINT p;
		GetCursorPos(&p);

		switch (windowType)
		{
		case WndWindow::NORMAL_WINDOW:
			return p.y-(y+40);
			break;
		case WndWindow::BORDERLESS_WINDOW:
			return p.y-y;
			break;
		default:
			return p.y-(y+40);
			break;
		}

		return 0;
	}

	int WndWindow::getWidth()
	{
		return width;
	}

	int WndWindow::getHeight()
	{
		return height;
	}

	std::wstring WndWindow::getTitle()
	{
		return title;
	}

	int WndWindow::getImageSize()
	{
		return wndPixelsSize;
	}

	bool WndWindow::getValid()
	{
		bool value = true;

		myMutex.lock();
		value = this->valid;
		myMutex.unlock();

		return value;
	}

	HWND WndWindow::getWindowHandle()
	{
		return windowHandle;
	}

	void WndWindow::setAllFunctionsToNull()
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

	void WndWindow::setPaintFunction(void(*function)(void))
	{
		paintFunction = function;
	}

	void WndWindow::setKeyUpFunction(void(*function)(WPARAM, LPARAM))
	{
		keyUpFunction = function;
	}

	void WndWindow::setKeyDownFunction(void(*function)(WPARAM, LPARAM))
	{
		keyDownFunction = function;
	}

	void WndWindow::setMouseButtonDownFunction(void(*function)(int))
	{
		mouseButtonDownFunction = function;
	}

	void WndWindow::setMouseButtonUpFunction(void(*function)(int))
	{
		mouseButtonUpFunction = function;
	}

	void WndWindow::setMouseHWheelFunction(void(*function)(int))
	{
		mouseHWheelFunction = function;
	}

	void WndWindow::setMouseWheelFunction(void(*function)(int))
	{
		mouseWheelFunction = function;
	}

	void WndWindow::setMouseButtonMovedFunction(void(*function)(void))
	{
		mouseMovedFunction = function;
	}

	void WndWindow::setClosingFunction(void(*function)(void))
	{
		closingFunction = function;
	}

	GuiManager* WndWindow::getGuiManager()
	{
		return gui;
	}

	void WndWindow::setActivateGui(bool v)
	{
		activateGui = v;
	}

	bool WndWindow::getActivateGui()
	{
		return activateGui;
	}

	void WndWindow::setMouseVWheelValuePointer(int* v)
	{
		WndWindow::mouseVWheelPointer = v;
	}

	void WndWindow::setMouseHWheelValuePointer(int* v)
	{
		WndWindow::mouseHWheelPointer = v;
	}

	void WndWindow::setFocus(bool v)
	{
		if(canFocus)
		{
			setShouldFocus(v);
		}
	}

	bool WndWindow::getFocus()
	{
		return GetFocus()==windowHandle;
	}

	bool WndWindow::getCanFocus()
	{
		return canFocus;
	}

	void WndWindow::setShouldFocus(bool v)
	{
		myMutex.lock();
		shouldFocus = v;
		myMutex.unlock();
	}

	void WndWindow::threadSetFocus()
	{
		SetFocus(windowHandle);
	}

	bool WndWindow::getShouldFocus()
	{
		myMutex.lock();
		bool v = shouldFocus;
		myMutex.unlock();
		return v;
	}

	void WndWindow::setResizable(bool v)
	{
		canResize = v;
	}

	bool WndWindow::getResizable()
	{
		return canResize;
	}

	void WndWindow::setMovable(bool v)
	{
		canMove = v;
	}

	bool WndWindow::getMovable()
	{
		return canMove;
	}

	void WndWindow::setThreadUpdateTime(unsigned int millis, unsigned int micros)
	{
		myMutex.lock();
		sleepTimeMillis = millis;
		sleepTimeMicros = micros;
		myMutex.unlock();
	}

	void WndWindow::drawImage(Image* g)
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

	void WndWindow::run()
	{
		while (getRunning())
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
	}

	void WndWindow::threadUpdate()
	{
		MSG m;
		ZeroMemory(&m, sizeof(MSG));
		while(PeekMessage(&m, windowHandle, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&m);
			DispatchMessage(&m);
		}
		
		if(getShouldFocus())
		{
			threadSetFocus();
		}
	}

	void WndWindow::threadGuiUpdate()
	{
		Input::pollInput();
		if(windowState != STATE_MINIMIZED)
		{
			if (gui != nullptr && activateGui)
			{
				if(windowType == WndWindow::NORMAL_WINDOW)
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

	void WndWindow::threadRender()
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

	void WndWindow::threadRepaint()
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
			
			RedrawWindow(windowHandle, NULL, NULL, RDW_INVALIDATE);

			myMutex.lock();
			shouldRepaint=false;
			myMutex.unlock();
		}
	}


	void WndWindow::update()
	{
		if(threadOwnership==false)
		{
			threadUpdate();
		}
	}

	void WndWindow::repaint()
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

	void WndWindow::guiUpdate()
	{
		if(threadOwnership==false)
		{
			threadGuiUpdate();
		}
	}

} //NAMESPACE glib END