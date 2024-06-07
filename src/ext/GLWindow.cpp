#include "ext/GLWindow.h"
#include <iostream>
#include "MathExt.h"
#include "System.h"
#include "StringTools.h"
#include "Input.h"
#include "ext/GLGraphics.h"

#ifdef USE_OPENGL
	namespace smpl
	{

		std::vector<GLWindow*> GLWindow::windowList = std::vector<GLWindow*>();
		const RootClass GLWindow::globalClass = CREATE_ROOT_CLASS(GLWindow, &SimpleWindow::globalClass);
        const RootClass* GLWindow::getClass()
        {
            return &GLWindow::globalClass;
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

			GLGraphics::init();
			GLGraphics::setViewport(0, 0, width, height);
			GLGraphics::setClearColor( Vec4f(0, 0, 0, 0) );
			GLGraphics::clear(GLGraphics::COLOR_BUFFER | GLGraphics::DEPTH_BUFFER);
			GLGraphics::setOrthoProjection(width, height);
			
			// glViewport(0, 0, width, height);
			// glClearColor(0.0f,0.0f,0.0f,0.0f);
			// glClearDepth(1.0f);
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

		void GLWindow::repaint()
		{
			swapBuffers();
		}

		void GLWindow::disposeGL()
		{
			if(ghRC!=0)
				wglDeleteContext(ghRC);
			ghRC = 0;
		}

		void GLWindow::setVSync(unsigned int interval)
		{
			wglSwapIntervalEXT(interval);
			swapInterval = interval;
		}

		unsigned int GLWindow::getVSyncInterval()
		{
			return swapInterval;
		}

		GLWindow::GLWindow()
			: SimpleWindow(true)
		{
			GLWindow::windowList.push_back(this);
			threadOwnership = false;
			init(-1, -1, 320, 240, L"", windowType);
		}

		GLWindow::GLWindow(std::wstring title, int width, int height, int x, int y, WindowOptions windowType)
			: SimpleWindow(true)
		{
			GLWindow::windowList.push_back(this);
			threadOwnership = false;
			init(x, y, width, height, title, windowType);
		}

		GLWindow::GLWindow(std::string title, int width, int height, int x, int y, WindowOptions windowType)
			: SimpleWindow(true)
		{
			GLWindow::windowList.push_back(this);
			threadOwnership = false;
			init(x, y, width, height, StringTools::toWideString(title), windowType);
		}

		GLWindow::~GLWindow()
		{
			dispose();
		}

		void GLWindow::dispose()
		{
			if (getValid())
			{
				SimpleWindow::dispose();
				disposeGL();
				
				#ifdef LINUX
				#else
					DeleteDC(ghDC);
					ghDC = 0;
				#endif
					
				GLWindow::removeWindowFromList(this);
			}
		}

		void GLWindow::init(int x, int y, int width, int height, std::wstring title, WindowOptions windowType)
		{
			this->x = x;
			this->y = y;
			this->width = width;
			this->height = height;
			
			this->windowType = windowType;
			this->windowType.threadManaged = SimpleWindow::TYPE_USER_MANAGED; //Must be user managed

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

				displayServer = XOpenDisplay(NULL);

				if(displayServer == NULL)
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
				

				XSelectInput(displayServer, windowHandle, ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
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

				initGL();
				gui = new GuiManager(GraphicsInterface::TYPE_OPENGL, this->width, this->height);

				if(windowType.initFunction != nullptr)
					windowType.initFunction(this);
				
				gui->setFocus(true);
				setFinishInit(true);

				if(threadManaged == TYPE_THREAD_MANAGED)
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
				
				wndClass.hIconSm = LoadIcon(hins, IDI_APPLICATION);
				wndClass.hInstance = hins;
				wndClass.lpfnWndProc = SimpleWindow::wndProc;
				wndClass.lpszClassName = text.c_str();
				wndClass.lpszMenuName = 0;
				wndClass.style = CS_HREDRAW | CS_VREDRAW;

				int trueWidth = this->width;
				int trueHeight = this->height;
				int trueX = this->x;
				int trueY = this->y;

				if (RegisterClassExW(&wndClass) != NULL)
				{
					DWORD style = 0;
					bool failed = false;

					switch(windowType.windowType)
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

					if(windowType.windowType == SimpleWindow::FULLSCREEN_WINDOW)
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

					if(windowType.focusable==TYPE_NONFOCUSABLE)
					{
						style |= WS_EX_NOACTIVATE;
					}

					if(!failed)
					{
						windowHandle = (size_t)CreateWindowExW(NULL, text.c_str(), title.c_str(), style, trueX, trueY, trueWidth, trueHeight, NULL, NULL, hins, NULL);

						DWORD attribValue = windowType.cornerType;
						DwmSetWindowAttribute((HWND)windowHandle, DWMWA_WINDOW_CORNER_PREFERENCE_CONST, &attribValue, sizeof(attribValue));
					}
					
					if (windowHandle != NULL)
					{
						this->preX = this->x;
						this->preY = this->y;
						
						setVisible(true);
						setValid(true);
						setShouldEnd(false);
						setRunning(true);

						initGL();

						//Must init gui here so openGL is initialized first
						gui = new GuiManager(GraphicsInterface::TYPE_OPENGL, this->width, this->height);

						if(windowType.initFunction != nullptr)
							windowType.initFunction(this);

						if(gui!=nullptr)
							gui->setFocus(true);
						setFinishInit(true);
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
		
		void GLWindow::finishResize()
		{
			GLGraphics::setViewport(0, 0, width, height);
			GLGraphics::setClearColor( Vec4f(0, 0, 0, 0) );
			GLGraphics::clear(GLGraphics::COLOR_BUFFER | GLGraphics::DEPTH_BUFFER);
			GLGraphics::setOrthoProjection(width, height);
		}

		bool GLWindow::threadRender()
		{
			bool changed = false;

			if(windowState != STATE_MINIMIZED)
			{
				if (gui != nullptr && activateGui)
				{
					changed = gui->renderGuiElements();
				}
			}

			return changed;
		}

		void GLWindow::threadRepaint()
		{
			//TODO - LINUX VERSION
			bool changed = false;

			//draw and send redraw message
			if(getResizing())
			{
				return;
			}

			if(getResizeMe())
			{
				setResizeMe(false);
				changed = true;
			}

			if(windowState != STATE_MINIMIZED)
			{
				bool imgChanged = threadRender();

				if(changed || imgChanged)
				{
					swapBuffers();
				}

				myMutex.lock();
				shouldRepaint=false;
				myMutex.unlock();
			}
		}

	} //NAMESPACE smpl END

#endif