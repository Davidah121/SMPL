#pragma once

#ifdef USE_DIRECTX

	#include "SimpleWindow.h"

	#include "ext/DXSingleton.h"

	namespace glib
	{

		class DXWindow : public SimpleWindow
		{
		public:
			struct WindowCreationError : public std::exception
			{
				const char* what() const throw() { return "Error creating window"; }
			};
			
			/**
			 * @brief Construct a new DXWindow object
			 * 		By default:
			 * 			the window does not have a name,
			 * 			width is 320px wide,
			 * 			height is 240px high,
			 * 			the window is centered in the screen of the main monitor,
			 * 			the window is managed by a thread,
			 * 			and the window is a normal window (has a title bar with min, max, close buttons, and is resizable) with OpenGL rendering.
			 */
			DXWindow();

			/**
			 * @brief Construct a new DXWindow object
			 * 
			 * @param title 
			 * 		The title of the window.
			 * @param width 
			 * 		The width of the window.
			 * 		By default, it is 320px wide
			 * @param height 
			 * 		The height of the window.
			 * 		By default, it is 240px high
			 * @param x 
			 * 		The x location of the top left part of the window.
			 * 		By default, it is -1 which will set the center x of the window at the center x of the main monitor.
			 * @param y 
			 * 		The y location of the top left part of the window.
			 * 		By default, it is -1 which will set the center y of the window at the center y of the main monitor.
			 * @param windowType 
			 * 		The type of window to create.
			 * 		Default value creates a window that is a Normal Focusable Thread Managed Window.
			 * 			Normal refers to having a frame that holds the title, minimize, maximize, and close button
			 */
			DXWindow(std::wstring title, int width = 320, int height = 240, int x = -1, int y = -1, WindowOptions windowType = {});

			/**
			 * @brief Construct a new DXWindow object
			 * 
			 * @param title 
			 * 		The title of the window.
			 * @param width 
			 * 		The width of the window.
			 * 		By default, it is 320px wide
			 * @param height 
			 * 		The height of the window.
			 * 		By default, it is 240px high
			 * @param x 
			 * 		The x location of the top left part of the window.
			 * 		By default, it is -1 which will set the center x of the window at the center x of the main monitor.
			 * @param y 
			 * 		The y location of the top left part of the window.
			 * 		By default, it is -1 which will set the center y of the window at the center y of the main monitor.
			 * @param windowType 
			 * 		The type of window to create.
			 * 		Default value creates a window that is a Normal Focusable Thread Managed Window.
			 * 			Normal refers to having a frame that holds the title, minimize, maximize, and close button
			 */
			DXWindow(std::string title, int width = 320, int height = 240, int x = -1, int y = -1, WindowOptions windowType = {});

			/**
			 * @brief Destroy the DXWindow object
			 * 		Any data managed by the window, such as the window's GuiManager, should be considered invalid and
			 * 		unusable.
			 * 
			 */
			~DXWindow();

			//Object and RootClass Stuff
			static const RootClass globalClass;

			/**
			 * @brief Sets the swap interval for vsync. 
			 * 		If set to 0, vsync will be disabled.
			 * 
			 * 		Note that in order to have 30 fps on a 60hz monitor, interval should be set to 2.
			 * 		To have 30 fps on a 120hz monitor, interval should be set to 4.
			 * 
			 * 		Sleeping in software can help for specific framerates and interval could be set to 1 to sync on the next
			 * 		available frame.
			 * 
			 * @param interval 
			 * 		Specifies which frame to sync on. 
			 */
			void setVSync(unsigned int interval);
			
			/**
			 * @brief Gets the swap interval for vsync.
			 * 
			 * @return unsigned int 
			 */
			unsigned int getVSyncInterval();

			/**
			 * @brief Sets additional flags in the IDXGISwapChain->Present() function.
			 * 		Useful for things like variable refresh rate among other things.
			 * 
			 * @param flags 
			 */
			void setSwapChainFlags(unsigned int flags);
			
			void testDirectX();
			void clearWindow(Vec4f color);
			void swapBuffers();
		private:
			
			static std::vector<DXWindow*> windowList;

			static void removeWindowFromList(DXWindow* value);
			static DXWindow* getWindowByHandle(size_t value);

			
			//DirectX STUFF
			// IDXGISwapChain* d3dSwapChain = nullptr;
			// ID3D11Device* d3dDevice = nullptr;
			// ID3D11DeviceContext* d3dContext = nullptr;
			// ID3D11RenderTargetView* backBuffer = nullptr;

			void initDirectX();
			unsigned int swapInterval = 0;
			unsigned int swapFlags = 0;
			// void disposeDirectX();

			//

			void init(int x, int y, int width, int height, std::wstring title, WindowOptions windowType);

			void run();
			void dispose();

			bool threadRender();
			void threadRepaint();
		};

	} //NAMESPACE glib END
	
#endif