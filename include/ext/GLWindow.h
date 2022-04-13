#pragma once

#ifdef USE_OPENGL

	#include "SimpleWindow.h"

	#ifdef LINUX
		#include <GL/glx.h>
		#include <GL/gl.h>
		#include <GL/glu.h>
	#else
		#include "ext/GLSingleton.h"
	#endif


	namespace glib
	{

		class GLWindow : public SimpleWindow
		{
		public:
			struct WindowCreationError : public std::exception
			{
				const char* what() const throw() { return "Error creating window"; }
			};
			
			/**
			 * @brief Construct a new GLWindow object
			 * 		By default:
			 * 			the window does not have a name,
			 * 			width is 320px wide,
			 * 			height is 240px high,
			 * 			the window is centered in the screen of the main monitor,
			 * 			the window is managed by a thread,
			 * 			and the window is a normal window (has a title bar with min, max, close buttons, and is resizable) with OpenGL rendering.
			 */
			GLWindow();

			/**
			 * @brief Construct a new GLWindow object
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
			GLWindow(std::wstring title, int width = 320, int height = 240, int x = -1, int y = -1, WindowOptions windowType = {});

			/**
			 * @brief Construct a new GLWindow object
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
			GLWindow(std::string title, int width = 320, int height = 240, int x = -1, int y = -1, WindowOptions windowType = {});

			/**
			 * @brief Destroy the GLWindow object
			 * 		Any data managed by the window, such as the window's GuiManager, should be considered invalid and
			 * 		unusable.
			 * 
			 */
			~GLWindow();

			//Object and Class Stuff
			const Class* getClass();
			static const Class myClass;

			/**
			 * @brief Repaints the window.
			 * 		When the window is in USER_MANAGED mode, it repaints immediately.
			 * 		When the window is in THREAD_MANAGED mode, it request the thread to repaint.
			 * 			This is used when thread auto repaint is set to false.
			 */
			void repaint();

			
			/**
			 * @brief Waits until the window has closed to continue.
			 * 		It will process the window messages and gui until closed.
			 */
			void waitTillClose();
			
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
			void setVSync(int interval);
			
			/**
			 * @brief Gets the swap interval for vsync.
			 * 
			 * @return int 
			 */
			int getVSyncInterval();

			void testGL();
			void swapBuffers();


			
		protected:
			
			static std::vector<GLWindow*> windowList;

			static void removeWindowFromList(GLWindow* value);
			static GLWindow* getWindowByHandle(size_t value);
			
			// //CHANGE WITH OTHER OS

			// #ifdef LINUX
			// void x11EventProc();
			// #else
			// static LRESULT _stdcall wndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam);
			// #endif
			// //

			//OPENGL STUFF
			void setPixelFormat();
			void initGL();
			void disposeGL();
			int swapInterval = 0;
			//

			void init(int x, int y, int width, int height, std::wstring title, WindowOptions windowType);
			
			void run();
			void dispose();
			
			bool threadRender();
			void threadRepaint();

			#ifdef LINUX
			#else
				HDC ghDC;
				HGLRC ghRC;
			#endif
		};

	} //NAMESPACE glib END

#endif