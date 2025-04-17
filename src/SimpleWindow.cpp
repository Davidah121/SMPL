#include "SimpleWindow.h"
#include <iostream>
#include <string.h>
#include "MathExt.h"
#include "System.h"
#include "StringTools.h"
#include "Input.h"

#ifndef NO_WINDOW

	namespace smpl
	{
		std::vector<SimpleWindow*> SimpleWindow::windowList = std::vector<SimpleWindow*>();
		int SimpleWindow::screenWidth = System::getDesktopWidth();
		int SimpleWindow::screenHeight = System::getDesktopHeight();
		
		#ifdef __unix__
		int SimpleWindow::BorderHeight = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
							GetSystemMetrics(SM_CXPADDEDBORDER) + 8);
		int SimpleWindow::BorderWidth = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER))*2;
		#else
		int SimpleWindow::BorderHeight = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
							GetSystemMetrics(SM_CXPADDEDBORDER) + 8);
		int SimpleWindow::BorderWidth = (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER))*2;
		#endif
		
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
		
		#ifdef __unix__
			void SimpleWindow::x11EventProc()
			{
				XEvent event;
				XNextEvent(displayServer, &event);

				switch (event.type)
				{
				case Expose:
					//Handled below
					break;
				case KeyPress:
					if(keyDownFunction != nullptr)
						keyDownFunction(this, event.xkey.keycode, 0);
					break;
				case KeyRelease:
					if(keyUpFunction != nullptr)
						keyUpFunction(this, event.xkey.keycode, 0);
					break;
				case ButtonPress:
					if(mouseButtonDownFunction != nullptr)
						mouseButtonDownFunction(this, event.xbutton.button);
					break;
				case ButtonRelease:
					if(mouseButtonUpFunction != nullptr)
						mouseButtonUpFunction(this, event.xbutton.button);
					break;
				case MotionNotify:
					if(mouseMovedFunction != nullptr)
						mouseMovedFunction(this);
					break;
				case DestroyNotify:
					if(closingFunction != nullptr)
						closingFunction(this);
					setShouldEnd(true);
					break;
				case CreateNotify:
					break;
				case ResizeRequest:
					break;
				case ClientMessage:
					if (event.xclient.data.l[0] == wmDeleteMessage)
					{
						if(closingFunction != nullptr)
							closingFunction(this);
						setShouldEnd(true);
					}
					break;
				case ConfigureNotify:
					x = event.xconfigure.x;
					y = event.xconfigure.y;
					if(width != event.xconfigure.width ||
						height != event.xconfigure.height)
					{
						width = event.xconfigure.width;
						height = event.xconfigure.height;
						initBitmap();
					}
				default:
					break;
				}

				if(event.type == Expose)
				{
					if(drawableImage != nullptr)
					{
						XPutImage(displayServer, (Window)windowHandle, gc, drawableImage, 0, 0, 0, 0, width, height);
					}
				}
			}
		#else

			#ifdef _WIN32

			bool SimpleWindow::processRawTouch(HWND hwnd, WPARAM wparam, LPARAM lparam)
			{
				bool bHandled = false;
				uint32_t cInputs = LOWORD(wparam);
				PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];

				if(pInputs != nullptr)
				{
					if(GetTouchInputInfo((HTOUCHINPUT)lparam, cInputs, pInputs, sizeof(TOUCHINPUT)))
					{
						//always consider it handled
						bHandled = true;
						if(rawTouchInput != nullptr)
							rawTouchInput(this, pInputs, cInputs);
					}
					delete[] pInputs;
				}

				if(bHandled)
				{
					CloseTouchInputHandle((HTOUCHINPUT)lparam);
				}
				return bHandled;
			}
			
			bool SimpleWindow::processGesture(HWND hwnd, WPARAM wparam, LPARAM lparam)
			{
				GESTUREINFO gi;
				ZeroMemory(&gi, sizeof(GESTUREINFO));
				gi.cbSize = sizeof(GESTUREINFO);

				bool res = GetGestureInfo((HGESTUREINFO)lparam, &gi);
				bool handled = false;

				if(res)
				{
					if(gestureTouchInput != nullptr)
					{
						handled = true;
						gestureTouchInput(this, gi);
					}
					handled = true;
				}

				return handled;
			}

			void SimpleWindow::setRawTouchFunction(std::function<void(SimpleWindow*, TOUCHINPUT*, int)> function)
			{
				if(function != nullptr)
				{
					rawTouchInput = function;
					RegisterTouchWindow((HWND)windowHandle, 0); //register for raw touch input
				}
			}

			void SimpleWindow::setGestureInputFunction(std::function<void(SimpleWindow*, GESTUREINFO)> function)
			{
				gestureTouchInput = function;
			}

			#endif

			LRESULT _stdcall SimpleWindow::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
			{
				SimpleWindow* currentWindow = SimpleWindow::getWindowByHandle((size_t)hwnd);

				bool canDo = false;

				HDC hdc;
				HDC deskHDC;
				PAINTSTRUCT ps;
				HDC mem;
				BITMAP img = {};
				HGDIOBJ oldImg;
				RECT* rect = nullptr;
				BLENDFUNCTION blend;
				blend.BlendOp = AC_SRC_OVER;
				blend.BlendFlags = 0;
				blend.SourceConstantAlpha = 255;
				blend.AlphaFormat = AC_SRC_ALPHA;




				if (currentWindow != nullptr)
				{
					POINT pt;
					pt.x = currentWindow->getX();
					pt.y = currentWindow->getY();

					POINT ptZero = {0}; //The origin point of the monitor its drawn on. 0,0 for primary. 

					SIZE siz;
					siz.cx=currentWindow->getWidth();
					siz.cy=currentWindow->getHeight();

					switch (msg)
					{
					case WM_SETFOCUS:
						if(currentWindow->gui!=nullptr)
							currentWindow->gui->setFocus(true);
						break;
					case WM_KILLFOCUS:
						if(currentWindow->gui!=nullptr)
							currentWindow->gui->setFocus(false);
						break;
					case WM_ERASEBKGND:
						break;
					case WM_PAINT:
						if(currentWindow->noWindowProcPaint == false)
						{
							hdc = BeginPaint(hwnd, &ps);

							mem = CreateCompatibleDC(hdc);
							oldImg = SelectObject(mem, currentWindow->bitmap);
							GetObject(currentWindow->bitmap, sizeof(BITMAP), &img);

							
							if(currentWindow->windowType.windowType == TRANSPARENT_WINDOW)
							{
								// Alpha blend with the pixels below on the desktop
								deskHDC = GetDC(NULL);
								UpdateLayeredWindow(hwnd, deskHDC, &pt, &siz, mem, &ptZero, RGB(0, 0, 0), &blend, ULW_ALPHA);
							}
							else
							{
								BitBlt(hdc, 0, 0, img.bmWidth, img.bmHeight, mem, 0, 0, SRCCOPY);
							}

							DeleteDC(mem);
							EndPaint(hwnd, &ps);
						}
						break;
					case WM_CLOSE:
						if (currentWindow->closingFunction != nullptr)
							currentWindow->closingFunction(currentWindow);
						PostQuitMessage(0);
						currentWindow->setShouldEnd(true);
						break;
					case WM_DESTROY:
						break;
						
					//The following exist just to prevent multilingual text input from creating a new popup box when not desired.
					case WM_SYSCHAR:
						if(!currentWindow->getAllowKeyInput())
							return 0;
						break;
					case WM_SYSDEADCHAR:
						if(!currentWindow->getAllowKeyInput())
							return 0;
						break;
					case WM_DEADCHAR:
						if(!currentWindow->getAllowKeyInput())
							return 0;
						break;
					/////////////////////////////////////////

					case WM_CHAR:
						if(!currentWindow->getAllowKeyInput())
							return 0;
						if(currentWindow->internalCharValFunction != nullptr && wparam >= 0x20)
							currentWindow->internalCharValFunction(currentWindow, wparam, lparam);
						break;
					case WM_KEYDOWN:
						if(!currentWindow->getAllowKeyInput())
							return 0;

						if (currentWindow->keyDownFunction != nullptr)
							currentWindow->keyDownFunction(currentWindow, wparam, lparam);
						
						if(wparam == (Input::KEY_DELETE&0xFF) || wparam == (Input::KEY_LEFT&0xFF) || wparam == (Input::KEY_RIGHT&0xFF)
						|| wparam == (Input::KEY_DOWN&0xFF) || wparam == (Input::KEY_UP&0xFF) || wparam == (Input::KEY_BACKSPACE&0xFF)
						|| wparam == (Input::KEY_ENTER&0xFF))
						{
							//Exceptions. These keys are not apart of WM_CHAR but are useful when editing text.
							if(currentWindow->internalCharValFunction != nullptr)
							{
								if(wparam == (Input::KEY_ENTER & 0xFF))
									currentWindow->internalCharValFunction(currentWindow, '\n', lparam);
								else
									currentWindow->internalCharValFunction(currentWindow, wparam | Input::NEGATIVE, lparam);
							}
						}
						
						break;
					case WM_KEYUP:
						if(!currentWindow->getAllowKeyInput())
							return 0;
						if (currentWindow->keyUpFunction != nullptr)
							currentWindow->keyUpFunction(currentWindow, wparam, lparam);
						break;
					case WM_LBUTTONDOWN:
						if (currentWindow->mouseButtonDownFunction != nullptr)
							currentWindow->mouseButtonDownFunction(currentWindow, MOUSE_LEFT);
						break;
					case WM_MBUTTONDOWN:
						if (currentWindow->mouseButtonDownFunction != nullptr)
							currentWindow->mouseButtonDownFunction(currentWindow, MOUSE_MIDDLE);
						break;
					case WM_RBUTTONDOWN:
						if (currentWindow->mouseButtonDownFunction != nullptr)
							currentWindow->mouseButtonDownFunction(currentWindow, MOUSE_RIGHT);
						break;
					case WM_LBUTTONUP:
						if (currentWindow->mouseButtonUpFunction != nullptr)
							currentWindow->mouseButtonUpFunction(currentWindow, MOUSE_LEFT);
						break;
					case WM_MBUTTONUP:
						if (currentWindow->mouseButtonUpFunction != nullptr)
							currentWindow->mouseButtonUpFunction(currentWindow, MOUSE_MIDDLE);
						break;
					case WM_RBUTTONUP:
						if (currentWindow->mouseButtonUpFunction != nullptr)
							currentWindow->mouseButtonUpFunction(currentWindow, MOUSE_RIGHT);
						break;
					case WM_MOUSEWHEEL:
						if (currentWindow->mouseWheelFunction != nullptr)
							currentWindow->mouseWheelFunction(currentWindow, GET_WHEEL_DELTA_WPARAM(wparam)/120);

						if(currentWindow->internalMouseWheelFunction != nullptr)
							currentWindow->internalMouseWheelFunction(currentWindow, GET_WHEEL_DELTA_WPARAM(wparam)/120);

						break;
					case WM_MOUSEHWHEEL:
						if (currentWindow->mouseHWheelFunction != nullptr)
							currentWindow->mouseHWheelFunction(currentWindow, GET_WHEEL_DELTA_WPARAM(wparam)/120);
						
						if(currentWindow->internalMouseHWheelFunction != nullptr)
							currentWindow->internalMouseHWheelFunction(currentWindow, GET_WHEEL_DELTA_WPARAM(wparam)/120);

						break;
					case WM_MOUSEMOVE:
						if (currentWindow->mouseMovedFunction != nullptr)
							currentWindow->mouseMovedFunction(currentWindow);
						break;
					case WM_GESTURE:
						break;
					case WM_MOVING:
						rect = (RECT*)lparam;
						currentWindow->x = rect->left;
						currentWindow->y = rect->top;
						
						if(currentWindow->windowType.windowType == SimpleWindow::NORMAL_WINDOW)
						{
							currentWindow->width = rect->right - rect->left - BorderWidth;
							currentWindow->height = rect->bottom - rect->top - BorderHeight;
						}
						else
						{
							currentWindow->width = rect->right - rect->left;
							currentWindow->height = rect->bottom - rect->top;
						}

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

						if(currentWindow->windowType.windowType == SimpleWindow::NORMAL_WINDOW)
						{
							currentWindow->width = rect->right - rect->left - BorderWidth;
							currentWindow->height = rect->bottom - rect->top - BorderHeight;
						}
						else
						{
							currentWindow->width = rect->right - rect->left;
							currentWindow->height = rect->bottom - rect->top;
						}

						break;
					case WM_SIZE:
						if(wparam == SIZE_MAXIMIZED)
						{
							currentWindow->windowState = STATE_MAXIMIZED;
							//FIX LATER
							currentWindow->preX = currentWindow->x;
							currentWindow->preY = currentWindow->y;

							currentWindow->x = 0;
							currentWindow->y = 0;

							if(currentWindow->width != LOWORD(lparam))
								currentWindow->setResizeMe(true);
							if(currentWindow->height != HIWORD(lparam))
								currentWindow->setResizeMe(true);
							
							currentWindow->width = LOWORD(lparam);
							currentWindow->height = HIWORD(lparam);

							currentWindow->redrawGui = true;
							currentWindow->finishResize();
						}
						else if(wparam == SIZE_RESTORED)
						{
							if(currentWindow->windowState == STATE_MAXIMIZED || currentWindow->windowState == STATE_MINIMIZED)
							{
								currentWindow->setResizeMe(true);
								currentWindow->redrawGui = true;
							}
							
							currentWindow->windowState = STATE_NORMAL;
							//FIX LATER
							
							currentWindow->x = currentWindow->preX;
							currentWindow->y = currentWindow->preY;
							
							currentWindow->width = LOWORD(lparam);
							currentWindow->height = HIWORD(lparam);
							
							if(!currentWindow->getResizing() || currentWindow->redrawGui)
							{
								currentWindow->setResizeMe(true);
								currentWindow->finishResize();
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
						//TODO: do this stuff on actual resize. Lazy
						currentWindow->redrawGui = true;
						currentWindow->finishResize();
						currentWindow->setResizing(false);
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
						SetCursor( LoadCursor(0, IDC_ARROW) );
						break;
					case WM_TOUCH:
						if(currentWindow->processRawTouch(hwnd, wparam, lparam))
							return 0;
						break;
					default:
						break;
					}
				}

				return DefWindowProcW(hwnd, msg, wparam, lparam);
			}
		#endif

		SimpleWindow::SimpleWindow(bool NoCreation)
		{
			//Does nothing as it should
			SimpleWindow::windowList.push_back(this);
		}

		SimpleWindow::SimpleWindow()
		{
			SimpleWindow::windowList.push_back(this);

			if(this->windowType.threadManaged == TYPE_THREAD_MANAGED)
			{
				threadOwnership = true;
				wndThread = new std::thread(&SimpleWindow::init, this, -1, -1, 320, 240, L"", windowType);
			}
			else
			{
				threadOwnership = false;
				init(-1, -1, 320, 240, L"", windowType);
			}

			while (getFinishInit() != true)
			{
				std::this_thread::yield();
			}
		}

		SimpleWindow::SimpleWindow(std::wstring title, int width, int height, int x, int y, WindowOptions windowType)
		{
			SimpleWindow::windowList.push_back(this);
			if(windowType.threadManaged == TYPE_THREAD_MANAGED)
			{
				threadOwnership = true;
				wndThread = new std::thread(&SimpleWindow::init, this, x, y, width, height, title, windowType);
			}
			else
			{
				threadOwnership = false;
				init(x, y, width, height, title, windowType);
			}

			while (getFinishInit() != true)
			{
				std::this_thread::yield();
			}
		}

		SimpleWindow::SimpleWindow(std::string title, int width, int height, int x, int y, WindowOptions windowType)
		{
			SimpleWindow::windowList.push_back(this);
			if(windowType.threadManaged == TYPE_THREAD_MANAGED)
			{
				threadOwnership = true;
				wndThread = new std::thread(&SimpleWindow::init, this, x, y, width, height, StringTools::toWideString(title), windowType);
			}
			else
			{
				threadOwnership = false;
				init(x, y, width, height, StringTools::toWideString(title), windowType);
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
				closingFunction(this);
			
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
				
				#ifdef __unix__
					XFreeGC(displayServer, gc);
					XDestroyWindow(displayServer, (Window)windowHandle);
					XCloseDisplay(displayServer);
					XDestroyImage(drawableImage); //Deletes wndPixels
					wndPixels = nullptr;

					windowHandle = 0;
					displayServer = nullptr;
				#else
					RevokeDragDrop((HWND)windowHandle);
					delete myDragDrop;
					myDragDrop = nullptr;
					
					if (IsWindow((HWND)windowHandle))
					{
						CloseWindow((HWND)windowHandle);
						DestroyWindow((HWND)windowHandle);
					}
					UnregisterClassW(text.c_str(), hins);

					if(bitmap != 0)
						DeleteObject(bitmap);

					if(handleToIcon != 0)
						DeleteObject(handleToIcon);
					
					if(myHDC != 0)
						DeleteDC(myHDC);

					bitmap = 0;
					myHDC = 0;
					windowHandle = 0;
				#endif

				if(gui!=nullptr)
					delete gui;
				gui = nullptr;

				if(wndPixels!=nullptr)
					delete[] wndPixels;
				
				wndPixels = nullptr;
					
				SimpleWindow::removeWindowFromList(this);
			}
		}

		void SimpleWindow::init(int x, int y, int width, int height, std::wstring title, WindowOptions windowType)
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
				this->x = (screenWidth / 2) - (width/2);

			if (this->y < 0)
				this->y = (screenHeight / 2) - (height/2);
			
			this->title = title;

			setAllFunctionsToNull();
			if(windowList.size() == 1)
			{
				setWindowAsInputFocus();
			}

			#ifdef __unix__

				displayServer = XOpenDisplay(0);

				if(displayServer == 0)
				{
					//error connecting to XServer
					setValid(false);
					setShouldEnd(true);
					setRunning(false);

					setFinishInit(true);
					return;
				}

				int trueWidth = this->width;
				int trueHeight = this->height;
				int trueX = this->x;
				int trueY = this->y;

				int borderWidth = 1;

				if(windowType.focusable==false)
				{
					
				}
				XVisualInfo vInfo;
				XMatchVisualInfo(displayServer, DefaultScreen(displayServer), 32, TrueColor, &vInfo);

				XSetWindowAttributes attr;
				attr.colormap = XCreateColormap(displayServer, DefaultRootWindow(displayServer), vInfo.visual, AllocNone);
				attr.border_pixel = borderWidth;
				attr.background_pixel = 0;
				
				screen = DefaultScreen(displayServer);
				windowHandle = XCreateWindow(displayServer, DefaultRootWindow(displayServer), trueX, trueY, trueWidth, trueHeight, borderWidth, vInfo.depth, InputOutput, vInfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);
				
				//set window name
				std::string cTitle = StringTools::toCString(title);
				XStoreName(displayServer, windowHandle, cTitle.c_str());

				//if window is borderless
				if(windowType.windowType == SimpleWindow::BORDERLESS_WINDOW)
				{
					Atom mwmHintsProperty = XInternAtom(displayServer, "_MOTIF_WM_HINTS", 0);
					MwmHints hints = {};
					hints.flags = MWM_HINTS_DECORATIONS;
					hints.decorations = 0;
					XChangeProperty(displayServer, windowHandle, mwmHintsProperty, mwmHintsProperty, 32, PropModeReplace, (unsigned char*)&hints, 5);
				}
				else if(windowType.windowType == SimpleWindow::FULLSCREEN_WINDOW)
				{
					Atom atoms[2] = {
						XInternAtom(displayServer, "_NET_WM_STATE_FULLSCREEN", 0), 0
					};
					XChangeProperty(displayServer, windowHandle, XInternAtom(displayServer, "_NET_WM_STATE", 0), XA_ATOM, 32, PropModeReplace, (unsigned char*)atoms, 1);

					this->x = 0;
					this->y = 0;
					this->width = System::getDesktopWidth();
					this->height = System::getDesktopHeight();
				}
				

				XSelectInput(displayServer, windowHandle, ExposureMask | SubstructureNotifyMask | StructureNotifyMask | ResizeRedirectMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
				XMapWindow(displayServer, windowHandle);

				wmDeleteMessage = XInternAtom(displayServer, "WM_DELETE_WINDOW", 0);
				XSetWMProtocols(displayServer, windowHandle, &wmDeleteMessage, 1);

				gc = XCreateGC(displayServer, windowHandle, 0, 0);

				this->preX = this->x;
				this->preY = this->y;
				
				setVisible(true);
				setValid(true);
				setShouldEnd(false);
				setRunning(true);

				initBitmap();
				gui = new GuiManager(GraphicsInterface::TYPE_SOFTWARE, this->width, this->height);

				if(windowType.initFunction != nullptr)
					windowType.initFunction(this);
				
				gui->setFocus(true);
				setFinishInit(true);

				if(windowType.threadManaged == TYPE_THREAD_MANAGED)
					run();
			#else

				std::wstring text = title;
				text += L"_CLASS";

				hins = GetModuleHandle(0);

				wndClass.cbClsExtra = 0;
				wndClass.cbSize = sizeof(WNDCLASSEX);
				wndClass.cbWndExtra = 0;
				wndClass.hbrBackground = (HBRUSH)(BLACK_BRUSH);
				wndClass.hCursor = LoadCursor(hins, IDC_ARROW);

				if(windowType.iconFileString.empty())
					wndClass.hIcon = LoadIcon(hins, IDI_APPLICATION);
				else
				{
					if(windowType.iconIsFile)
						handleToIcon = (HICON)LoadImageA(NULL, windowType.iconFileString.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
					else
						handleToIcon = (HICON)LoadImageA(hins, windowType.iconFileString.c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

					wndClass.hIcon = handleToIcon;
				}
				
				wndClass.hIconSm = wndClass.hIcon;
				wndClass.hInstance = hins;
				wndClass.lpfnWndProc = SimpleWindow::wndProc;
				wndClass.lpszClassName = text.c_str();
				wndClass.lpszMenuName = 0;
				wndClass.style = CS_HREDRAW | CS_VREDRAW;

				int trueWidth = this->width;
				int trueHeight = this->height;
				int trueX = this->x;
				int trueY = this->y;

				if (RegisterClassExW(&wndClass) != 0)
				{
					DWORD style = 0;
					bool failed = false;

					switch(windowType.windowType)
					{
						case SimpleWindow::NORMAL_WINDOW:
							style = WS_OVERLAPPEDWINDOW;
							trueWidth += BorderWidth;
							trueHeight += BorderHeight;
							break;
						case SimpleWindow::BORDERLESS_WINDOW:
							style = WS_POPUP|WS_VISIBLE|WS_SYSMENU;
							break;
						case SimpleWindow::TRANSPARENT_WINDOW:
							style = WS_POPUP|WS_VISIBLE|WS_SYSMENU;
							break;
						default:
							style = WS_OVERLAPPEDWINDOW;
							trueWidth += BorderWidth;
							trueHeight += BorderHeight;
							break;
					}

					if(windowType.windowType == SimpleWindow::FULLSCREEN_WINDOW)
					{
						HMONITOR hmon = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
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
					
					if(windowType.focusable==TYPE_NONFOCUSABLE)
					{
						style |= WS_EX_NOACTIVATE;
					}

					if(!failed)
					{
						if(windowType.windowType == TRANSPARENT_WINDOW)
							windowHandle = (size_t)CreateWindowExW(WS_EX_LAYERED, text.c_str(), title.c_str(), style, trueX, trueY, trueWidth, trueHeight, 0, 0, hins, 0);
						else
							windowHandle = (size_t)CreateWindowExW(0, text.c_str(), title.c_str(), style, trueX, trueY, trueWidth, trueHeight, 0, 0, hins, 0);
							

						DWORD attribValue = windowType.cornerType;
						DwmSetWindowAttribute((HWND)windowHandle, DWMWA_WINDOW_CORNER_PREFERENCE_CONST, &attribValue, sizeof(attribValue));
					}
					
					if (windowHandle != 0)
					{
						this->preX = this->x;
						this->preY = this->y;
						
						setVisible(true);
						setValid(true);
						setShouldEnd(false);
						setRunning(true);

						WinSingleton::tryInit();
						myDragDrop = new DragDrop((HWND)windowHandle);
						RegisterDragDrop((HWND)windowHandle, (LPDROPTARGET)myDragDrop);


						initBitmap();
						gui = new GuiManager(GraphicsInterface::TYPE_SOFTWARE, this->width, this->height);

						if(windowType.initFunction != nullptr)
							windowType.initFunction(this);
						
						gui->setFocus(true);
						setFinishInit(true);

						if(windowType.threadManaged == TYPE_THREAD_MANAGED)
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
					// StringTools::println("%d", GetLastError());
					setValid(false);
					setShouldEnd(true);
					setRunning(false);

					setFinishInit(true);
				}

			#endif

		}

		void SimpleWindow::finishResize()
		{
			initBitmap();
			if(gui!=nullptr)
			{
				gui->resizeImage(width, height);
				if(redrawGui)
				{
					gui->forceRedraw();
					redrawGui = false;
				}
			}
			resizing = false;
			resizeMe = false;
		}

		void SimpleWindow::initBitmap()
		{
			#ifdef __unix__
				if(wndPixels != nullptr)
				{
					XDestroyImage(drawableImage); //destroys wndPixels
					drawableImage = nullptr;
					wndPixels = nullptr;
				}
				wndPixelsSize = (width*height*4);
				wndPixels = new unsigned char[wndPixelsSize];

				XVisualInfo vInfo;
				XMatchVisualInfo(displayServer, DefaultScreen(displayServer), 32, TrueColor, &vInfo);
				drawableImage = XCreateImage(displayServer, vInfo.visual, vInfo.depth,
										ZPixmap, 0, (char*)wndPixels, width, height, 32, 4*width);
			#else
				if(wndPixels != nullptr)
				{
					DeleteObject(bitmap);
					delete[] wndPixels;
					wndPixels = nullptr;
				}

				bitInfo.bmiHeader.biSize = sizeof(bitInfo.bmiHeader);
				bitInfo.bmiHeader.biWidth = width;
				bitInfo.bmiHeader.biHeight = -height;
				bitInfo.bmiHeader.biCompression = BI_RGB;
				bitInfo.bmiHeader.biPlanes = 1;
				bitInfo.bmiHeader.biBitCount = 32;
				bitInfo.bmiHeader.biSizeImage = 0;

				bitInfo.bmiHeader.biXPelsPerMeter = width;
				bitInfo.bmiHeader.biYPelsPerMeter = height;

				bitInfo.bmiHeader.biClrImportant = 0;
				bitInfo.bmiHeader.biClrUsed = 0;

				bitInfo.bmiColors->rgbRed = 0;
				bitInfo.bmiColors->rgbGreen = 0;
				bitInfo.bmiColors->rgbBlue = 0;
				bitInfo.bmiColors->rgbReserved = 0;

				myHDC = GetDC((HWND)windowHandle);

				bitmap = CreateCompatibleBitmap(myHDC, width, height);

				if(wndPixels != nullptr)
				{
					delete[] wndPixels;
					wndPixels = nullptr;
				}

				// scanLinePadding = width%4;
				// wndPixelsSize = (width*3 + scanLinePadding) * height;
				scanLinePadding = 0;
				wndPixelsSize = width*height*4;

				wndPixels = new unsigned char[wndPixelsSize];
				memset(wndPixels,0,wndPixelsSize);
			#endif
			
			mustRepaint = true;
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
			if(this->windowType.threadManaged == SimpleWindow::TYPE_THREAD_MANAGED)
			{
				while(getRunning())
				{
					System::sleep(1, 0, false);
				}
			}
			else
			{
				run();
			}
		}

		void SimpleWindow::setVisible(bool value)
		{
			#ifdef __unix__
				if (value == true)
					XMapWindow(displayServer, (Window)windowHandle);
				else
					XUnmapWindow(displayServer, (Window)windowHandle);
			#else
				if (value == true)
					ShowWindow((HWND)windowHandle, SW_SHOW);
				else
					ShowWindow((HWND)windowHandle, SW_HIDE);
			#endif
		}

		void SimpleWindow::setX(int x)
		{
			#ifdef __unix__
				XMoveWindow(displayServer, (Window)windowHandle, x, this->y);
			#else
				SetWindowPos((HWND)windowHandle, HWND_TOP, x, this->y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
			#endif
			this->x = x;
		}

		void SimpleWindow::setY(int y)
		{
			#ifdef __unix__
				XMoveWindow(displayServer, (Window)windowHandle, this->x, y);
			#else
				SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
			#endif
			this->y = y;
		}

		void SimpleWindow::setPosition(int x, int y)
		{
			#ifdef __unix__
				XMoveWindow(displayServer, (Window)windowHandle, x, y);
			#else
				SetWindowPos((HWND)windowHandle, HWND_TOP, x, y, this->width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
			#endif
			this->x = x;
			this->y = y;
		}

		void SimpleWindow::setWidth(int width)
		{
			#ifdef __unix__
				XResizeWindow(displayServer, (Window)windowHandle, width, this->height);
			#else
				SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, width, this->height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
			#endif
			this->width = width;
		}

		void SimpleWindow::setHeight(int height)
		{
			#ifdef __unix__
				XResizeWindow(displayServer, (Window)windowHandle, this->width, height);
			#else
				SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, this->width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
			#endif
			this->height = height;
		}

		void SimpleWindow::setSize(int width, int height)
		{
			#ifdef __unix__
				XResizeWindow(displayServer, (Window)windowHandle, width, height);
			#else
				SetWindowPos((HWND)windowHandle, HWND_TOP, this->x, this->y, width, height, SWP_ASYNCWINDOWPOS | SWP_NOREDRAW);
			#endif
			this->width = width;
			this->height = height;
		}

		void SimpleWindow::resizeToGuiManager()
		{
			auto guiSurf = gui->getSurface();
			if(guiSurf != nullptr)
			{
				setSize(guiSurf->getWidth(), guiSurf->getHeight());
			}
		}

		int SimpleWindow::getMouseX()
		{
			int mx = 0;
			int borderWidth = 0;

			#ifdef __unix__
			
			int winX, winY, rootX, rootY = 0;
			unsigned int mask = 0;
			Window childWin, rootWin;

			XQueryPointer(displayServer, RootWindow(displayServer, screen),
			&childWin, &rootWin, &winX, &winY, &rootX, &rootY, &mask);

			mx = rootX;
			borderWidth = 1; //Set when creating the window
			
			#else
				POINT p;
				GetCursorPos(&p);
				borderWidth = GetSystemMetrics(SM_CXFRAME);
				mx = p.x;
			#endif
			
			switch (windowType.windowType)
			{
			case SimpleWindow::NORMAL_WINDOW:
				return mx-(x+borderWidth);
				break;
			case SimpleWindow::BORDERLESS_WINDOW:
				return mx-x;
				break;
			case SimpleWindow::FULLSCREEN_WINDOW:
				return mx-x;
				break;
			default:
				return mx-(x+borderWidth);
				break;
			}

			return 0;
		}

		int SimpleWindow::getMouseY()
		{
			int my = 0;
			int borderHeight = 0;

			#ifdef __unix__
			
				int winX, winY, rootX, rootY = 0;
				unsigned int mask = 0;
				Window childWin, rootWin;

				XQueryPointer(displayServer, RootWindow(displayServer, screen),
				&childWin, &rootWin, &winX, &winY, &rootX, &rootY, &mask);

				my = rootY;
				borderHeight = 1 + 13; //Set when creating the window
			
			#else
				POINT p;
				GetCursorPos(&p);
				borderHeight = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
									GetSystemMetrics(SM_CXPADDEDBORDER));
				my = p.y;
			#endif
			
			switch (windowType.windowType)
			{
			case SimpleWindow::NORMAL_WINDOW:
				return my-(y+borderHeight);
				break;
			case SimpleWindow::BORDERLESS_WINDOW:
				return my-y;
				break;
			case SimpleWindow::FULLSCREEN_WINDOW:
				return my-y;
				break;
			default:
				return my-(y+borderHeight);
				break;
			}

			return my;
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
			mouseButtonDownFunction = nullptr;
			mouseButtonUpFunction = nullptr;
			mouseWheelFunction = nullptr;
			mouseHWheelFunction = nullptr;
			mouseMovedFunction = nullptr;
			closingFunction = nullptr;
		}

		void SimpleWindow::setPaintFunction(std::function<void(SimpleWindow*)> function)
		{
			paintFunction = function;
		}

		void SimpleWindow::setMouseMovedFunction(std::function<void(SimpleWindow*)> function)
		{
			mouseMovedFunction = function;
		}

		void SimpleWindow::setClosingFunction(std::function<void(SimpleWindow*)> function)
		{
			closingFunction = function;
		}

		void SimpleWindow::setKeyUpFunction(std::function<void(SimpleWindow*, unsigned long, long)> function)
		{
			keyUpFunction = function;
		}

		void SimpleWindow::setKeyDownFunction(std::function<void(SimpleWindow*, unsigned long, long)> function)
		{
			keyDownFunction = function;
		}

		void SimpleWindow::setMouseButtonDownFunction(std::function<void(SimpleWindow*, int)> function)
		{
			mouseButtonDownFunction = function;
		}

		void SimpleWindow::setMouseButtonUpFunction(std::function<void(SimpleWindow*, int)> function)
		{
			mouseButtonUpFunction = function;
		}

		void SimpleWindow::setMouseHWheelFunction(std::function<void(SimpleWindow*, int)> function)
		{
			mouseHWheelFunction = function;
		}

		void SimpleWindow::setMouseWheelFunction(std::function<void(SimpleWindow*, int)> function)
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

		void SimpleWindow::setWindowAsInputFocus()
		{
			internalCharValFunction = Input::adjustCurrCharVal;
			internalMouseWheelFunction = Input::adjustVerticalScroll;
			internalMouseHWheelFunction = Input::adjustHorizontalScroll;
		}

		void SimpleWindow::setFocus(bool v)
		{
			if(canFocus)
			{
				setShouldFocus(v);
				if(threadOwnership == false)
				{
					threadSetFocus();
				}
			}
		}

		bool SimpleWindow::getFocus()
		{
			#ifdef __unix__
				Window currFocusedWindow;
				int other;
				XGetInputFocus(displayServer, &currFocusedWindow, &other);
				return (currFocusedWindow == (Window)windowHandle);
			#else
				return GetFocus()==(HWND)windowHandle;
			#endif
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
			#ifdef __unix__
				XSetInputFocus(displayServer, (Window)windowHandle, RevertToNone, CurrentTime);
			#else
				SetFocus((HWND)windowHandle);
			#endif
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

		void SimpleWindow::allowKeyInput(bool k)
		{
			myMutex.lock();
			this->processKeyInputs = k;
			myMutex.unlock();
		}

		bool SimpleWindow::getAllowKeyInput()
		{
			bool b;
			myMutex.lock();
			b = processKeyInputs;
			myMutex.unlock();
			return b;
		}

		void SimpleWindow::setThreadUpdateTime(size_t millis, size_t micros)
		{
			myMutex.lock();
			sleepTimeMillis = millis;
			sleepTimeMicros = micros;
			myMutex.unlock();
		}

		void SimpleWindow::drawImage(Image* g)
		{
			#ifdef __unix__
			Color* imgPixels = (Color*)g->getPixels();
			int* windowPixels = (int*)wndPixels;
			int* endWindowPixels = windowPixels + width*height;
			Color* endImgPixels = imgPixels + g->getWidth()*g->getHeight();
			
			if(width == g->getWidth() && height == g->getHeight())
			{
				while(windowPixels < endWindowPixels)
				{
					
					int v = imgPixels->alpha; //switch from RGBA to ARGB
					v = (v<<8) + imgPixels->red;
					v = (v<<8) + imgPixels->green;
					v = (v<<8) + imgPixels->blue;
					// int v = 0xFF000000 + (((*imgPixels) & 0xFFFFFF));
					*windowPixels = v;
					windowPixels++;
					imgPixels++;
				}
			}
			else
			{
				int minWidth = MathExt::min(width, g->getWidth());
				int wndPixelAdjust = width - minWidth;
				int imgPixelAdjust = g->getWidth() - minWidth;
				int tX = 0;
				while(windowPixels < endWindowPixels && imgPixels < endImgPixels)
				{
					int v = imgPixels->alpha; //switch from RGBA to ARGB
					v = (v<<8) + imgPixels->red;
					v = (v<<8) + imgPixels->green;
					v = (v<<8) + imgPixels->blue;
					*windowPixels = v;
					windowPixels++; 
					imgPixels++;
					tX++;
					if(tX >= minWidth)
					{
						tX = 0;
						windowPixels += wndPixelAdjust;
						imgPixels += imgPixelAdjust;
					}
				}
			}
			#else

				if (g != nullptr)
				{
					if (g->getWidth() == width && g->getHeight() == height)
					{
						//same size
						Color* imgPixelsStart = g->getPixels();
						unsigned char* wndPixelsStart = wndPixels;
						unsigned char* wndPixelsEnd = wndPixels + wndPixelsSize;
						int tX = 0;
						while (wndPixelsStart < wndPixelsEnd)
						{
							//switch from rgba to bgra
							//normally a byteswap and left rotation is all that is neccessary but must use premultiplied alpha
							
							if(this->windowType.windowType == TRANSPARENT_WINDOW)
							{
								*wndPixelsStart = (unsigned char)(imgPixelsStart->alpha * ((float)imgPixelsStart->blue/255.0));
								*(wndPixelsStart + 1) = (unsigned char)(imgPixelsStart->alpha * ((float)imgPixelsStart->green/255.0));
								*(wndPixelsStart + 2) = (unsigned char)(imgPixelsStart->alpha * ((float)imgPixelsStart->red/255.0));
							}
							else
							{
								*wndPixelsStart = imgPixelsStart->blue;
								*(wndPixelsStart + 1) = imgPixelsStart->green;
								*(wndPixelsStart + 2) = imgPixelsStart->red;
							}
							*(wndPixelsStart + 3) = (*imgPixelsStart).alpha;

							wndPixelsStart += 4;
							imgPixelsStart++;
							tX++;

							if(tX>=width)
							{
								tX=0;
								wndPixelsStart += scanLinePadding;
							}
						}
					}
					else
					{
						Color* imgPixelsStart = g->getPixels();
						Color* imgPixelsEnds = g->getPixels() + (g->getWidth()*g->getHeight());
						unsigned char* wndPixelsStart = wndPixels;
						unsigned char* wndPixelsEnd = wndPixels + wndPixelsSize;

						int minWidth = MathExt::min(width, g->getWidth());
						int wndPixelWidthPadding = width - minWidth;
						int imgWidthPadding = g->getWidth() - minWidth;
						int tX = 0;

						while(wndPixelsStart < wndPixelsEnd && imgPixelsStart < imgPixelsEnds)
						{
							if(this->windowType.windowType == TRANSPARENT_WINDOW)
							{
								wndPixelsStart[0] = (unsigned char)(imgPixelsStart->alpha * ((float)imgPixelsStart->blue/255.0));
								wndPixelsStart[1] = (unsigned char)(imgPixelsStart->alpha * ((float)imgPixelsStart->green/255.0));
								wndPixelsStart[2] = (unsigned char)(imgPixelsStart->alpha * ((float)imgPixelsStart->red/255.0));
							}
							else
							{
								wndPixelsStart[0] = imgPixelsStart->blue;
								wndPixelsStart[1] = imgPixelsStart->green;
								wndPixelsStart[2] = imgPixelsStart->red;
							}
							wndPixelsStart[3] = imgPixelsStart->red;
							
							wndPixelsStart+=4; 
							imgPixelsStart++;
							tX++;
							if(tX >= minWidth)
							{
								tX = 0;
								wndPixelsStart += wndPixelWidthPadding + scanLinePadding;
								imgPixelsStart += imgWidthPadding;
							}
						}
					}
				}
			#endif
		}

		void SimpleWindow::run()
		{
			while (!getShouldEnd())
			{
				size_t t1 = System::getCurrentTimeMicro();

				threadUpdate();
				if(getShouldEnd())
					break;

				threadGuiUpdate();
				if(getRepaint())
					threadRepaint();

				size_t timeNeeded = 0;
				myMutex.lock();
				timeNeeded = sleepTimeMillis*1000 + sleepTimeMicros;
				myMutex.unlock();

				size_t t2 = System::getCurrentTimeMicro();
				size_t timePassed = t2-t1;
				
				if(timePassed < timeNeeded)
				{
					size_t waitTime = timeNeeded - timePassed;
					System::sleep(waitTime/1000, waitTime%1000);
				}
			}

			setRunning(false);
		}

		void SimpleWindow::threadUpdate()
		{
			#ifdef __unix__
				x11EventProc();
			#else
				MSG m;
				ZeroMemory(&m, sizeof(MSG));
				while(PeekMessage(&m, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&m);
					DispatchMessage(&m);
				}
			#endif

			if(getShouldEnd())
				return;
			
			if(getShouldFocus())
			{
				threadSetFocus();
			}
		}

		void SimpleWindow::threadGuiUpdate()
		{
			if(!activateGui)
				return;

			Input::pollInput();
			if(windowState != STATE_MINIMIZED)
			{
				if (gui != nullptr && activateGui)
				{
					if(windowType.windowType == SimpleWindow::NORMAL_WINDOW)
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

		bool SimpleWindow::threadRender()
		{
			bool changed = false;

			if(windowState != STATE_MINIMIZED)
			{
				if (gui != nullptr && activateGui)
				{
					changed = gui->renderGuiElements();
					if(changed && gui->getSurface() != nullptr)
					{
						if(gui->getSurface()->getType() == GraphicsInterface::TYPE_SOFTWARE)
						{
							Image* surface = (Image*)gui->getSurface()->getSurface();
							if(surface != nullptr)
								drawImage(surface);
						}
					}
				}
			}

			if (paintFunction != nullptr)
			{
				paintFunction(this);
				changed = true;
			}

			return changed;
		}

		void SimpleWindow::threadRepaint()
		{
			//TODO - __unix__ VERSION
			bool changed = false;

			//draw and send redraw message
			if(getResizing())
			{
				return;
			}

			changed |= mustRepaint;

			if(windowState != STATE_MINIMIZED)
			{
				bool imgChanged = threadRender();

				if(changed || imgChanged)
				{
					#ifdef __unix__
						//Simpler than XSendEvent
						XClearArea(displayServer, (Window)windowHandle, 0, 0, 1, 1, true);
					#else
						int value = SetDIBits(myHDC, bitmap, 0, height, &wndPixels[0], &bitInfo, DIB_RGB_COLORS);
						RedrawWindow((HWND)windowHandle, 0, 0, RDW_INVALIDATE);
					#endif
				}

				myMutex.lock();
				shouldRepaint = false;
				mustRepaint = false;
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
				shouldRepaint = true;
				threadRepaint();
			}
			else
			{
				myMutex.lock();
				shouldRepaint=true;
				myMutex.unlock();
			}
		}
		
		void SimpleWindow::forceRepaint()
		{
			myMutex.lock();
			mustRepaint = true;
			myMutex.unlock();
		}

		void SimpleWindow::guiUpdate()
		{
			if(threadOwnership==false)
			{
				threadGuiUpdate();
			}
		}

	} //NAMESPACE glib END

#endif