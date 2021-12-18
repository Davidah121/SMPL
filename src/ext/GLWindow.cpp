#include "ext/GLWindow.h"
#include <iostream>
#include "MathExt.h"
#include "System.h"
#include "StringTools.h"
#include "Input.h"

namespace glib
{

	std::vector<GLWindow*> GLWindow::windowList = std::vector<GLWindow*>();
	int GLWindow::screenWidth = System::getDesktopWidth();
	int GLWindow::screenHeight = System::getDesktopHeight();

	int* GLWindow::mouseVWheelPointer = nullptr;
	int* GLWindow::mouseHWheelPointer = nullptr;

	const Class GLWindow::myClass = Class("GLWindow", {&Object::myClass});
	const Class* GLWindow::getClass()
	{
		return &GLWindow::myClass;
	}

	GLWindow* GLWindow::getWindowByHandle(size_t handle)
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

	void GLWindow::removeWindowFromList(GLWindow* wnd)
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

	void GLWindow::setPixelFormat()
	{
		// PIXELFORMATDESCRIPTOR pfd;
		// ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

		// pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		// pfd.nVersion = 1;
		// pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		// pfd.iLayerType = PFD_MAIN_PLANE;
		// pfd.cColorBits = 24;
		// pfd.cDepthBits = 32;
		// pfd.iPixelType = PFD_TYPE_RGBA;

		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		
		int pixelFormat = ChoosePixelFormat(ghDC, &pfd);

		if(pixelFormat == 0)
		{
			//error occured
			setValid(false);
			StringTools::println("cPIXEL FORMAT ERROR");
			return;
		}

		if(SetPixelFormat(ghDC, pixelFormat, &pfd) == false)
		{
			//error occured
			setValid(false);
			StringTools::println("sPIXEL FORMAT ERROR");
			return;
		}
	}

	void GLWindow::initGL()
	{
		ghDC = GetDC((HWND)windowHandle);
		setPixelFormat();
		
		if(getValid() == false)
		{
			StringTools::println("FAILED");
			setShouldEnd(true);
		}
		else
		{
			ghRC = wglCreateContext(ghDC);
			if(wglMakeCurrent(ghDC, ghRC) == false)
			{
				StringTools::println("FAILED2");
			}
		}

		GLSingleton::tryInit();
		if(!GLSingleton::getInit())
		{
			StringTools::println("GLEW ERROR");
			setShouldEnd(true);
			return;
		}
		
		glViewport(0, 0, width, height);
		glClearColor(0.0f,0.0f,0.0f,0.0f);
		glClearDepth(1.0f);
	}

	void GLWindow::testGL()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glBegin(GL_TRIANGLES);
		glColor3f(0.1, 0.2, 0.3);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(0, 1, 0);
		glEnd();

		glFlush();

		SwapBuffers(ghDC);
	}

	void GLWindow::swapBuffers()
	{
		SwapBuffers(ghDC);
	}

	void GLWindow::disposeGL()
	{
		if(ghRC!=0)
			wglDeleteContext(ghRC);
	}

	void GLWindow::setVSync(int interval)
	{
		wglSwapIntervalEXT(interval);
		swapInterval = interval;
	}

	int GLWindow::getVSyncInterval()
	{
		return swapInterval;
	}
	
	LRESULT _stdcall GLWindow::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		GLWindow* currentWindow = GLWindow::getWindowByHandle((size_t)hwnd);

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
				
				if(GLWindow::mouseVWheelPointer)
					*GLWindow::mouseVWheelPointer = GET_WHEEL_DELTA_WPARAM(wparam)/120;
				break;
			case WM_MOUSEHWHEEL:
				if (currentWindow->mouseHWheelFunction != nullptr)
					currentWindow->mouseHWheelFunction(GET_WHEEL_DELTA_WPARAM(wparam)/120);
				
				if(GLWindow::mouseHWheelPointer)
					*GLWindow::mouseHWheelPointer = GET_WHEEL_DELTA_WPARAM(wparam)/120;
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

	GLWindow::GLWindow()
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
			wndThread = new std::thread(&GLWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
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

	GLWindow::GLWindow(std::wstring title, int width, int height, int x, int y, unsigned char windowType)
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
			wndThread = new std::thread(&GLWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
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

	GLWindow::GLWindow(std::string title, int width, int height, int x, int y, unsigned char windowType)
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
			wndThread = new std::thread(&GLWindow::init, this, this->x, this->y, this->width, this->height, this->title, this->windowType);
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

	GLWindow::~GLWindow()
	{
		dispose();
	}

	void GLWindow::close()
	{
		if(closingFunction!=nullptr)
			closingFunction();
		
		dispose();
	}

	void GLWindow::dispose()
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

			disposeGL();
			
			if (IsWindow((HWND)windowHandle))
			{
				CloseWindow((HWND)windowHandle);
				DestroyWindow((HWND)windowHandle);
			}
			UnregisterClassW(text.c_str(), hins);
			DeleteDC(ghDC);
				
			GLWindow::removeWindowFromList(this);
		}
	}

	void GLWindow::init(int x, int y, int width, int height, std::wstring title, unsigned char windowType)
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
			wndClass.lpfnWndProc = GLWindow::wndProc;
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
					case GLWindow::NORMAL_WINDOW:
						style = WS_OVERLAPPEDWINDOW;
						trueWidth += 16;
						trueHeight += 40;
						break;
					case GLWindow::BORDERLESS_WINDOW:
						style = WS_POPUP|WS_VISIBLE|WS_SYSMENU;
						break;
					default:
						style = WS_OVERLAPPEDWINDOW;
						trueWidth += 16;
						trueHeight += 40;
						break;
				}

				if(wType == GLWindow::FULLSCREEN_WINDOW)
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

					GLWindow::windowList.push_back(this);

					initGL();

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

	void GLWindow::setRunning(bool value)
	{
		myMutex.lock();
		running = value;
		myMutex.unlock();
	}

	bool GLWindow::getRunning()
	{
		bool v = true;
		myMutex.lock();
		v = running;
		myMutex.unlock();

		return v;
	}

	void GLWindow::setValid(bool value)
	{
		myMutex.lock();
		valid = value;
		myMutex.unlock();
	}

	void GLWindow::setFinishInit(bool value)
	{
		myMutex.lock();
		finishedInit = value;
		myMutex.unlock();
	}

	void GLWindow::setResizing(bool value)
	{
		resizing = value;
	}

	void GLWindow::setResizeMe(bool value)
	{
		resizeMe = value;
	}

	bool GLWindow::getFinishInit()
	{
		return finishedInit;
	}

	bool GLWindow::getResizing()
	{
		return resizing;
	}

	bool GLWindow::getResizeMe()
	{
		return resizeMe;
	}

	void GLWindow::waitTillClose()
	{
		while(getRunning())
		{
			System::sleep(1);
		}
	}

	void GLWindow::setVisible(bool value)
	{
		if (value == true)
			ShowWindow((HWND)windowHandle, SW_SHOW);
		else
			ShowWindow((HWND)windowHandle, SW_HIDE);
	}

	void GLWindow::setX(int x)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, x, this->y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->x = x;
	}

	void GLWindow::setY(int y)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->y = y;
	}

	void GLWindow::setPosition(int x, int y)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->x = x;
		this->y = y;
	}

	void GLWindow::setWidth(int width)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->width = width;
	}

	void GLWindow::setHeight(int height)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, this->width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->height = height;
	}

	void GLWindow::setSize(int width, int height)
	{
		SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
		this->width = width;
		this->height = height;
	}

	int GLWindow::getMouseX()
	{
		POINT p;
		GetCursorPos(&p);
		int borderWidth = GetSystemMetrics(SM_CXFRAME);
		
		switch (windowType)
		{
		case GLWindow::NORMAL_WINDOW:
			return p.x-(x+borderWidth);
			break;
		case GLWindow::BORDERLESS_WINDOW:
			return p.x-x;
			break;
		case GLWindow::FULLSCREEN_WINDOW:
			return p.x-x;
			break;
		default:
			return p.x-(x+borderWidth);
			break;
		}

		return 0;
	}

	int GLWindow::getMouseY()
	{
		POINT p;
		GetCursorPos(&p);
		int borderHeight = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
    							GetSystemMetrics(SM_CXPADDEDBORDER));

		switch (windowType)
		{
		case GLWindow::NORMAL_WINDOW:
			return p.y-(y+borderHeight);
			break;
		case GLWindow::BORDERLESS_WINDOW:
			return p.y-y;
			break;
		case GLWindow::FULLSCREEN_WINDOW:
			return p.x-x;
			break;
		default:
			return p.y-(y+borderHeight);
			break;
		}

		return 0;
	}

	int GLWindow::getWidth()
	{
		return width;
	}

	int GLWindow::getHeight()
	{
		return height;
	}

	int GLWindow::getX()
	{
		return x;
	}

	int GLWindow::getY()
	{
		return y;
	}

	std::wstring GLWindow::getTitle()
	{
		return title;
	}

	bool GLWindow::getValid()
	{
		bool value = true;

		myMutex.lock();
		value = this->valid;
		myMutex.unlock();

		return value;
	}

	size_t GLWindow::getWindowHandle()
	{
		return windowHandle;
	}

	void GLWindow::setAllFunctionsToNull()
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

	void GLWindow::setMouseMovedFunction(std::function<void()> function)
	{
		mouseMovedFunction = function;
	}

	void GLWindow::setClosingFunction(std::function<void()> function)
	{
		closingFunction = function;
	}

	void GLWindow::setKeyUpFunction(std::function<void(unsigned long, long)> function)
	{
		keyUpFunction = function;
	}

	void GLWindow::setKeyDownFunction(std::function<void(unsigned long, long)> function)
	{
		keyDownFunction = function;
	}

	void GLWindow::setMouseButtonDownFunction(std::function<void(int)> function)
	{
		mouseButtonDownFunction = function;
	}

	void GLWindow::setMouseButtonUpFunction(std::function<void(int)> function)
	{
		mouseButtonUpFunction = function;
	}

	void GLWindow::setMouseHWheelFunction(std::function<void(int)> function)
	{
		mouseHWheelFunction = function;
	}

	void GLWindow::setMouseWheelFunction(std::function<void(int)> function)
	{
		mouseWheelFunction = function;
	}

	void GLWindow::setMouseVWheelValuePointer(int* v)
	{
		GLWindow::mouseVWheelPointer = v;
	}

	void GLWindow::setMouseHWheelValuePointer(int* v)
	{
		GLWindow::mouseHWheelPointer = v;
	}

	void GLWindow::setFocus(bool v)
	{
		if(canFocus)
		{
			setShouldFocus(v);
		}
	}

	bool GLWindow::getFocus()
	{
		return GetFocus()==(HWND)windowHandle;
	}

	bool GLWindow::getCanFocus()
	{
		return canFocus;
	}

	void GLWindow::setShouldFocus(bool v)
	{
		myMutex.lock();
		shouldFocus = v;
		myMutex.unlock();
	}

	void GLWindow::threadSetFocus()
	{
		SetFocus((HWND)windowHandle);
	}

	bool GLWindow::getShouldFocus()
	{
		myMutex.lock();
		bool v = shouldFocus;
		myMutex.unlock();
		return v;
	}

	
	void GLWindow::setShouldEnd(bool v)
	{
		myMutex.lock();
		shouldEnd = v;
		if(threadOwnership == false)
			running = false;
		myMutex.unlock();
	}

	bool GLWindow::getShouldEnd()
	{
		myMutex.lock();
		bool v = shouldEnd;
		myMutex.unlock();
		return v;
	}
	
	void GLWindow::setResizable(bool v)
	{
		canResize = v;
	}

	bool GLWindow::getResizable()
	{
		return canResize;
	}

	void GLWindow::setMovable(bool v)
	{
		canMove = v;
	}

	bool GLWindow::getMovable()
	{
		return canMove;
	}

	void GLWindow::setThreadUpdateTime(unsigned int millis, unsigned int micros)
	{
		myMutex.lock();
		sleepTimeMillis = millis;
		sleepTimeMicros = micros;
		myMutex.unlock();
	}

	void GLWindow::run()
	{
		while (!getShouldEnd())
		{
			time_t t1 = System::getCurrentTimeMicro();
			
			threadUpdate();
			testGL();

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

	void GLWindow::threadUpdate()
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

	void GLWindow::update()
	{
		if(threadOwnership==false)
		{
			threadUpdate();
		}
	}

} //NAMESPACE glib END