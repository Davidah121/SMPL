
#ifdef USE_DIRECTX

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
		const RootClass DXWindow::globalClass = RootClass("DXWindow", {"SimpleWindow"});

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
			this->myHDC = GetDC((HWND)windowHandle);

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
			DXSingleton::init((HWND)windowHandle, true);

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
			d3dSwapChain->Present(swapInterval, swapFlags);
		}

		void DXWindow::setVSync(unsigned int interval)
		{
			swapInterval = interval;
		}
		
		unsigned int DXWindow::getVSyncInterval()
		{
			return swapInterval;
		}
		
		void DXWindow::setSwapChainFlags(unsigned int flags)
		{
			swapFlags = flags;
		}

		DXWindow::DXWindow()
			: SimpleWindow(true)
		{
			DXWindow::windowList.push_back(this);
			threadOwnership = false;
			init(-1, -1, 320, 240, L"", windowType);
		}

		DXWindow::DXWindow(std::wstring title, int width, int height, int x, int y, WindowOptions windowType)
			: SimpleWindow(true)
		{
			DXWindow::windowList.push_back(this);
			threadOwnership = false;
			init(x, y, width, height, title, windowType);
		}

		DXWindow::DXWindow(std::string title, int width, int height, int x, int y, WindowOptions windowType)
			: SimpleWindow(true)
		{
			DXWindow::windowList.push_back(this);
			threadOwnership = false;
			init(x, y, width, height, StringTools::toWideString(title), windowType);
		}

		DXWindow::~DXWindow()
		{
			dispose();
		}

		void DXWindow::dispose()
		{
			if (getValid())
			{
				SimpleWindow::dispose();
				DXWindow::removeWindowFromList(this);
			}
		}

		void DXWindow::init(int x, int y, int width, int height, std::wstring title, WindowOptions windowType)
		{
			setClass(globalClass);
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
				//throw error cause linux does not have DirectX
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

						initDirectX();

						//Must init gui here so DirectX is initialized first
						gui = new GuiManager(GuiManager::TYPE_DIRECTX, this->width, this->height);

						if(windowType.initFunction != nullptr)
							windowType.initFunction(this);

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

		void DXWindow::run()
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

		bool DXWindow::threadRender()
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

		void DXWindow::threadRepaint()
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

	} //NAMESPACE glib END

#endif