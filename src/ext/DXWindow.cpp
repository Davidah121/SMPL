#include "ext/DXWindow.h"
#include <iostream>
#include "MathExt.h"
#include "System.h"
#include "StringTools.h"
#include "Input.h"

#include <shobjidl.h>

namespace glib
{

	std::vector<DXWindow*> DXWindow::windowList = std::vector<DXWindow*>();
	int DXWindow::screenWidth = System::getDesktopWidth();
	int DXWindow::screenHeight = System::getDesktopHeight();

	int* DXWindow::mouseVWheelPointer = nullptr;
	int* DXWindow::mouseHWheelPointer = nullptr;

	const Class DXWindow::myClass = Class("DXWindow", {&Object::myClass});
	const Class* DXWindow::getClass()
	{
		return &DXWindow::myClass;
	}

	DXWindow* DXWindow::getWindowByHandle(size_t handle)
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

	void DXWindow::removeWindowFromList(DXWindow* wnd)
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

	void DXWindow::initDirectX()
	{
		StringTools::println("INIT");
		myDC = GetDC((HWND)windowHandle);

		// DXGI_SWAP_CHAIN_DESC scd;
		// ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
		// scd.BufferCount = 1;
		// scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		// scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		// scd.SampleDesc.Count = 1;
		// scd.Windowed = true;
		// scd.OutputWindow = (HWND)windowHandle;

		// D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd, &d3dSwapChain, &d3dDevice, NULL, &d3dContext);

		// ID3D11Texture2D* pBackBuff;

		// d3dSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuff);
		// d3dDevice->CreateRenderTargetView(pBackBuff, NULL, &backBuffer);
		// pBackBuff->Release();

		// d3dContext->OMSetRenderTargets(1, &backBuffer, nullptr);
		DXSingleton::init((HWND)windowHandle);

		D3D11_VIEWPORT viewport = {0};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = width;
		viewport.Height = height;
		
		DXSingleton::getContext()->RSSetViewports(1, &viewport);
	}

	void DXWindow::testDirectX()
	{
		float colorArr[4] = {0.0f, 0.2f, 0.4f, 1.0f};

		ID3D11DeviceContext* d3dContext = DXSingleton::getContext();
		ID3D11RenderTargetView* backBuffer = DXSingleton::getBackBuffer();
		IDXGISwapChain* d3dSwapChain = DXSingleton::getSwapChain();

		if(d3dContext != nullptr && backBuffer != nullptr && d3dSwapChain != nullptr)
		{
			d3dContext->ClearRenderTargetView(backBuffer, colorArr);
			d3dSwapChain->Present(0, 0);
		}
		else
		{
			//error
		}
	}

	
	void DXWindow::clearWindow(Vec4f color)
	{
		float colorArr[4] = {(float)color.x, (float)color.y, (float)color.z, (float)color.w};
		ID3D11DeviceContext* d3dContext = DXSingleton::getContext();
		ID3D11RenderTargetView* backBuffer = DXSingleton::getBackBuffer();
		
		d3dContext->ClearRenderTargetView(backBuffer, colorArr);
	}

	void DXWindow::swapBuffers()
	{
		IDXGISwapChain* d3dSwapChain = DXSingleton::getSwapChain();
		d3dSwapChain->Present(0, 0);
	}

	
	LRESULT _stdcall DXWindow::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		DXWindow* currentWindow = DXWindow::getWindowByHandle((size_t)hwnd);

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
				BeginPaint(hwnd, &ps);
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
				
				if(DXWindow::mouseVWheelPointer)
					*DXWindow::mouseVWheelPointer = GET_WHEEL_DELTA_WPARAM(wparam)/120;
				break;
			case WM_MOUSEHWHEEL:
				if (currentWindow->mouseHWheelFunction != nullptr)
					currentWindow->mouseHWheelFunction(GET_WHEEL_DELTA_WPARAM(wparam)/120);
				
				if(DXWindow::mouseHWheelPointer)
					*DXWindow::mouseHWheelPointer = GET_WHEEL_DELTA_WPARAM(wparam)/120;
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

	DXWindow::DXWindow()
	{
		x = screenWidth / 2 - 160;
		y = screenHeight / 2 - 120;
		width = 320;
		height = 240;
		title = L"";

		setAllFunctionsToNull();

		int threadManaged = windowType & 0b0100;
		
		if(threadManaged == TYPE_THREAD_MANAGED)
		{
			threadOwnership = true;
			wndThread = new std::thread(&DXWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
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

	DXWindow::DXWindow(std::wstring title, int width, int height, int x, int y, unsigned char windowType)
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
		
		this->title = title;

		int threadManaged = windowType & 0b0100;
		
		if(threadManaged == TYPE_THREAD_MANAGED)
		{
			threadOwnership = true;
			wndThread = new std::thread(&DXWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
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

	DXWindow::DXWindow(std::string title, int width, int height, int x, int y, unsigned char windowType)
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

		this->title = StringTools::toWideString(title);

		int threadManaged = windowType & 0b0100;

		if(threadManaged == TYPE_THREAD_MANAGED)
		{
			threadOwnership = true;
			wndThread = new std::thread(&DXWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
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

	DXWindow::~DXWindow()
	{
		dispose();
	}

	void DXWindow::close()
	{
		if(closingFunction!=nullptr)
			closingFunction();
		
		dispose();
	}

	void DXWindow::dispose()
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
			DeleteDC(myDC);
				
			DXWindow::removeWindowFromList(this);
		}
	}

	void DXWindow::init(int x, int y, int width, int height, std::wstring title, unsigned char windowType)
	{
		#ifdef LINUX

		#else

			std::wstring text = title;
			text += L"_CLASS";

			hins = GetModuleHandle(NULL);

			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 0;
			wndClass.hbrBackground = (HBRUSH)(BLACK_BRUSH);
			wndClass.hCursor = LoadCursor(hins, IDC_ARROW);
			wndClass.hIcon = LoadIcon(hins, IDI_APPLICATION);
			wndClass.hInstance = hins;
			wndClass.lpfnWndProc = DXWindow::wndProc;
			wndClass.lpszClassName = text.c_str();
			wndClass.lpszMenuName = NULL;
			wndClass.style = CS_HREDRAW | CS_VREDRAW;

			int trueWidth = width;
			int trueHeight = height;
			int trueX = x;
			int trueY = y;

			if (RegisterClassW(&wndClass) != NULL)
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
					case DXWindow::NORMAL_WINDOW:
						style = WS_OVERLAPPEDWINDOW;
						trueWidth += 16;
						trueHeight += 40;
						break;
					case DXWindow::BORDERLESS_WINDOW:
						style = WS_POPUP|WS_VISIBLE|WS_SYSMENU;
						break;
					default:
						style = WS_OVERLAPPEDWINDOW;
						trueWidth += 16;
						trueHeight += 40;
						break;
				}

				if(wType == DXWindow::FULLSCREEN_WINDOW)
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
					
					windowHandle = (size_t)CreateWindowW(text.c_str(), title.c_str(), style, trueX, trueY, trueWidth, trueHeight, NULL, NULL, hins, NULL);
				}
				
				if (windowHandle != NULL)
				{
					setVisible(true);
					setValid(true);
					setShouldEnd(false);
					setRunning(true);

					DXWindow::windowList.push_back(this);

					initDirectX();

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

	void DXWindow::setRunning(bool value)
	{
		myMutex.lock();
		running = value;
		myMutex.unlock();
	}

	bool DXWindow::getRunning()
	{
		bool v = true;
		myMutex.lock();
		v = running;
		myMutex.unlock();

		return v;
	}

	void DXWindow::setValid(bool value)
	{
		myMutex.lock();
		valid = value;
		myMutex.unlock();
	}

	void DXWindow::setFinishInit(bool value)
	{
		myMutex.lock();
		finishedInit = value;
		myMutex.unlock();
	}

	void DXWindow::setResizing(bool value)
	{
		resizing = value;
	}

	void DXWindow::setResizeMe(bool value)
	{
		resizeMe = value;
	}

	bool DXWindow::getFinishInit()
	{
		return finishedInit;
	}

	bool DXWindow::getResizing()
	{
		return resizing;
	}

	bool DXWindow::getResizeMe()
	{
		return resizeMe;
	}

	void DXWindow::waitTillClose()
	{
		while(getRunning())
		{
			System::sleep(1);
		}
	}

	void DXWindow::setVisible(bool value)
	{
		if (value == true)
			ShowWindow((HWND)windowHandle, SW_SHOW);
		else
			ShowWindow((HWND)windowHandle, SW_HIDE);
	}

	void DXWindow::setX(int x)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, x, this->y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->x = x;
	}

	void DXWindow::setY(int y)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->y = y;
	}

	void DXWindow::setPosition(int x, int y)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->x = x;
		this->y = y;
	}

	void DXWindow::setWidth(int width)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->width = width;
	}

	void DXWindow::setHeight(int height)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, this->width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->height = height;
	}

	void DXWindow::setSize(int width, int height)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->width = width;
		this->height = height;
	}

	int DXWindow::getMouseX()
	{
		POINT p;
		GetCursorPos(&p);
		int borderWidth = GetSystemMetrics(SM_CXFRAME);
		
		switch (windowType)
		{
		case DXWindow::NORMAL_WINDOW:
			return p.x-(x+borderWidth);
			break;
		case DXWindow::BORDERLESS_WINDOW:
			return p.x-x;
			break;
		case DXWindow::FULLSCREEN_WINDOW:
			return p.x-x;
			break;
		default:
			return p.x-(x+borderWidth);
			break;
		}

		return 0;
	}

	int DXWindow::getMouseY()
	{
		POINT p;
		GetCursorPos(&p);
		int borderHeight = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
    							GetSystemMetrics(SM_CXPADDEDBORDER));

		switch (windowType)
		{
		case DXWindow::NORMAL_WINDOW:
			return p.y-(y+borderHeight);
			break;
		case DXWindow::BORDERLESS_WINDOW:
			return p.y-y;
			break;
		case DXWindow::FULLSCREEN_WINDOW:
			return p.x-x;
			break;
		default:
			return p.y-(y+borderHeight);
			break;
		}

		return 0;
	}

	int DXWindow::getWidth()
	{
		return width;
	}

	int DXWindow::getHeight()
	{
		return height;
	}

	int DXWindow::getX()
	{
		return x;
	}

	int DXWindow::getY()
	{
		return y;
	}

	std::wstring DXWindow::getTitle()
	{
		return title;
	}

	bool DXWindow::getValid()
	{
		bool value = true;

		myMutex.lock();
		value = this->valid;
		myMutex.unlock();

		return value;
	}

	size_t DXWindow::getWindowHandle()
	{
		return windowHandle;
	}

	void DXWindow::setAllFunctionsToNull()
	{
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

	void DXWindow::setMouseMovedFunction(std::function<void()> function)
	{
		mouseMovedFunction = function;
	}

	void DXWindow::setClosingFunction(std::function<void()> function)
	{
		closingFunction = function;
	}

	void DXWindow::setKeyUpFunction(std::function<void(unsigned long, long)> function)
	{
		keyUpFunction = function;
	}

	void DXWindow::setKeyDownFunction(std::function<void(unsigned long, long)> function)
	{
		keyDownFunction = function;
	}

	void DXWindow::setMouseButtonDownFunction(std::function<void(int)> function)
	{
		mouseButtonDownFunction = function;
	}

	void DXWindow::setMouseButtonUpFunction(std::function<void(int)> function)
	{
		mouseButtonUpFunction = function;
	}

	void DXWindow::setMouseHWheelFunction(std::function<void(int)> function)
	{
		mouseHWheelFunction = function;
	}

	void DXWindow::setMouseWheelFunction(std::function<void(int)> function)
	{
		mouseWheelFunction = function;
	}

	void DXWindow::setMouseVWheelValuePointer(int* v)
	{
		DXWindow::mouseVWheelPointer = v;
	}

	void DXWindow::setMouseHWheelValuePointer(int* v)
	{
		DXWindow::mouseHWheelPointer = v;
	}

	void DXWindow::setFocus(bool v)
	{
		if(canFocus)
		{
			setShouldFocus(v);
		}
	}

	bool DXWindow::getFocus()
	{
		return GetFocus()==(HWND)windowHandle;
	}

	bool DXWindow::getCanFocus()
	{
		return canFocus;
	}

	void DXWindow::setShouldFocus(bool v)
	{
		myMutex.lock();
		shouldFocus = v;
		myMutex.unlock();
	}

	void DXWindow::threadSetFocus()
	{
		SetFocus((HWND)windowHandle);
	}

	bool DXWindow::getShouldFocus()
	{
		myMutex.lock();
		bool v = shouldFocus;
		myMutex.unlock();
		return v;
	}

	
	void DXWindow::setShouldEnd(bool v)
	{
		myMutex.lock();
		shouldEnd = v;
		if(threadOwnership == false)
			running = false;
		myMutex.unlock();
	}

	bool DXWindow::getShouldEnd()
	{
		myMutex.lock();
		bool v = shouldEnd;
		myMutex.unlock();
		return v;
	}
	
	void DXWindow::setResizable(bool v)
	{
		canResize = v;
	}

	bool DXWindow::getResizable()
	{
		return canResize;
	}

	void DXWindow::setMovable(bool v)
	{
		canMove = v;
	}

	bool DXWindow::getMovable()
	{
		return canMove;
	}

	void DXWindow::setThreadUpdateTime(unsigned int millis, unsigned int micros)
	{
		myMutex.lock();
		sleepTimeMillis = millis;
		sleepTimeMicros = micros;
		myMutex.unlock();
	}

	void DXWindow::run()
	{
		while (!getShouldEnd())
		{
			time_t t1 = System::getCurrentTimeMicro();
			
			threadUpdate();
			testDirectX();

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

	void DXWindow::threadUpdate()
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

	void DXWindow::update()
	{
		if(threadOwnership==false)
		{
			threadUpdate();
		}
	}

} //NAMESPACE glib END