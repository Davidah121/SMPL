#pragma once
#include "Image.h"
#include "GuiManager.h"

#ifdef LINUX
	#include <X11/Xlib.h>
	#include <X11/Xatom.h>
	#include <X11/Xutil.h>

	#include <GL/glx.h>
	#include <GL/gl.h>
	#include <GL/glu.h>

	struct MwmHints {
		unsigned long flags;
		unsigned long functions;
		unsigned long decorations;
		long input_mode;
		unsigned long status;
	};
	
	enum {
		MWM_HINTS_FUNCTIONS = (1L << 0),
		MWM_HINTS_DECORATIONS =  (1L << 1),

		MWM_FUNC_ALL = (1L << 0),
		MWM_FUNC_RESIZE = (1L << 1),
		MWM_FUNC_MOVE = (1L << 2),
		MWM_FUNC_MINIMIZE = (1L << 3),
		MWM_FUNC_MAXIMIZE = (1L << 4),
		MWM_FUNC_CLOSE = (1L << 5)
	};

#else
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
	#include <GL/glew.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <exception>
#include <thread>
#include <vector>
#include <mutex>

namespace glib
{

	class GLWindow : public Object
	{
	public:
		static const unsigned char MOUSE_LEFT = 0x00;
		static const unsigned char MOUSE_MIDDLE = 0x01;
		static const unsigned char MOUSE_RIGHT = 0x02;

		static const unsigned char NORMAL_WINDOW = 0b0000;
		static const unsigned char BORDERLESS_WINDOW = 0b0001;
		static const unsigned char FULLSCREEN_WINDOW = 0b1000;

		static const unsigned char STATE_NORMAL = 0x00;
		static const unsigned char STATE_MAXIMIZED = 0x01;
		static const unsigned char STATE_MINIMIZED = 0x02;
		
		static const unsigned char TYPE_FOCUSABLE = 0b0000;
		static const unsigned char TYPE_NONFOCUSABLE = 0b0010;

		static const unsigned char TYPE_THREAD_MANAGED = 0b0000;
		static const unsigned char TYPE_USER_MANAGED = 0b0100;

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
		 * 		The window type and window management can be combined using the logical or ( '|' ).
		 * 		(Note that FULLSCREEN_WINDOW ignores the width, height, x, y, NORMAL_WINDOW and BORDERLESS_WINDOW options)
		 * 		Valid arguments are:
		 * 			NORMAL_WINDOW
		 * 			BORDERLESS_WINDOW
		 * 			FULLSCREEN_WINDOW
		 * 			TYPE_FOCUSABLE
		 * 			TYPE_NONFOCUSABLE
		 * 			TYPE_USER_MANAGED
		 * 			TYPE_THREAD_MANAGED
		 * 		Default value is 0 which is a Normal Focusable Thread Managed Window.
		 */
		GLWindow(std::wstring title, int width = 320, int height = 240, int x = -1, int y = -1, unsigned char windowType = NORMAL_WINDOW);

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
		 * 		The window type and window management can be combined using the logical or ( '|' ).
		 * 		(Note that FULLSCREEN_WINDOW ignores the width, height, x, y, NORMAL_WINDOW and BORDERLESS_WINDOW options)
		 * 		Valid arguments are:
		 * 			NORMAL_WINDOW
		 * 			BORDERLESS_WINDOW
		 * 			FULLSCREEN_WINDOW
		 * 			TYPE_FOCUSABLE
		 * 			TYPE_NONFOCUSABLE
		 * 			TYPE_USER_MANAGED
		 * 			TYPE_THREAD_MANAGED
		 * 		Default value is 0 which is a Normal Focusable Thread Managed Window.
		 */
		GLWindow(std::string title, int width = 320, int height = 240, int x = -1, int y = -1, unsigned char windowType = NORMAL_WINDOW);

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
		 * @brief Sets if the window should be visible.
		 * 
		 * @param value 
		 */
		void setVisible(bool value);

		/**
		 * @brief Sets the x location of the window. 
		 * 		Specifically set the location of the top left corner of the window.
		 * 
		 * @param x 
		 */
		void setX(int x);

		/**
		 * @brief Sets the y location of the window. 
		 * 		Specifically set the location of the top left corner of the window.
		 * 
		 * @param y 
		 */
		void setY(int y);

		/**
		 * @brief Sets the location of the window. 
		 * 		Specifically set the location of the top left corner of the window.
		 * 
		 * @param x
		 * @param y 
		 */
		void setPosition(int x, int y);

		/**
		 * @brief Sets the width of the window.
		 * 
		 * @param width 
		 */
		void setWidth(int width);

		/**
		 * @brief Sets the height of the window
		 * 
		 * @param height 
		 */
		void setHeight(int height);

		/**
		 * @brief Sets the size of the window
		 * 
		 * @param width 
		 * @param height 
		 */
		void setSize(int width, int height);

		/**
		 * @brief Gets the mouse x position relative to the location of the window.
		 * 		Specifically, relative to the location of the start of the drawing area of the window.
		 * 
		 * @return int 
		 */
		int getMouseX();

		/**
		 * @brief Gets the mouse y position relative to the location of the window.
		 * 		Specifically, relative to the location of the start of the drawing area of the window.
		 * 
		 * @return int 
		 */
		int getMouseY();

		/**
		 * @brief Gets the width of the window
		 * 
		 * @return int 
		 */
		int getWidth();

		/**
		 * @brief Gets the height of the window
		 * 
		 * @return int 
		 */
		int getHeight();

		/**
		 * @brief Gets the x position of the window
		 * 
		 * @return int 
		 */
		int getX();

		/**
		 * @brief Gets the y position of the window
		 * 
		 * @return int 
		 */
		int getY();

		/**
		 * @brief Gets the title of the window
		 * 
		 * @return std::wstring 
		 */
		std::wstring getTitle();

		/**
		 * @brief Returns if the window is valid.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getValid();

		/**
		 * @brief Returns if the window is running or not.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getRunning();

		/**
		 * @brief Closes the window
		 * 
		 */
		void close();

		/**
		 * @brief Gets the Window Handle
		 * 		Returns a value which represents the window.
		 * 		Windows uses a HWND
		 * 		Linux uses an unsigned int to store windows
		 * 
		 * @return size_t
		 */
		size_t getWindowHandle();

		/**
		 * @brief Sets the Mouse Moved Function
		 * 		The function will be called when the mouse has moved inside the window.
		 * 
		 * @param function 
		 */
		void setMouseMovedFunction(std::function<void()> function);

		/**
		 * @brief Sets the Closing Function
		 * 		The function will be called right before the window is closed.
		 * 
		 * @param function 
		 */
		void setClosingFunction(std::function<void()> function);

		/**
		 * @brief Sets the Key Up Function
		 * 		The function will be called when a key has been released.
		 * 		The first argument (unsigned long) contains the keycode
		 * 		The second argument (long) contains additional information
		 * 
		 * @param function 
		 */
		void setKeyUpFunction(std::function<void(unsigned long, long)> function);

		/**
		 * @brief Sets the Key Down Function
		 * 		The function will be called when a key has been pressed.
		 * 		The first argument (unsigned long) contains the keycode
		 * 		The second argument (long) contains additional information
		 * 
		 * @param function 
		 */
		void setKeyDownFunction(std::function<void(unsigned long, long)> function);

		/**
		 * @brief Sets the Mouse Button Down Function
		 * 		The function will be called when a mouse button has been pressed.
		 * 		The integer value represents which mouse button was pressed.
		 * 
		 * @param function 
		 */
		void setMouseButtonDownFunction(std::function<void(int)> function);

		/**
		 * @brief Sets the Mouse Button Up Function
		 * 		The function will be called when a mouse button has been released.
		 * 		The integer value represents which mouse button was released.
		 * 
		 * @param function 
		 */
		void setMouseButtonUpFunction(std::function<void(int)> function);

		/**
		 * @brief Sets the Mouse HWheel Function
		 * 		The function will be called when the horizontal mouse wheel has been used
		 * 		and the window is focused.
		 * 		The integer value represents which direction it was scrolled in and the intensity.
		 * 
		 * @param function 
		 */
		void setMouseHWheelFunction(std::function<void(int)> function);

		/**
		 * @brief Sets the Mouse Wheel Function
		 * 		The function will be called when the vertical mouse wheel has been used
		 * 		and the window is focused.
		 * 		The integer value represents which direction it was scrolled in and the intensity.
		 * 
		 * @param function 
		 */
		void setMouseWheelFunction(std::function<void(int)> function);

		/**
		 * @brief Updates the window.
		 * 		This function is only used when the window is in USER_MANAGED mode.
		 * 		This allows the events for the window to be processed.
		 */
		void update();

		/**
		 * @brief Sets the minimum amount of time for updating and repainting the window.
		 * 		The time taken can be above the specified time depending on the complexity
		 * 		of the update and rendering functions.
		 * 
		 * @param millis 
		 * @param micros 
		 */
		void setThreadUpdateTime(unsigned int millis, unsigned int micros = 0);

		/**
		 * @brief Waits until the window has closed to continue.
		 * 
		 */
		void waitTillClose();

		//adjustment stuff

		/**
		 * @brief Sets whether the window should be focused.
		 * 		If the window can be focused, it requests the OS to be in focus.
		 * 		May be unsuccessful.
		 * 
		 * @param v 
		 */
		void setFocus(bool v);

		/**
		 * @brief Gets whether the window is focused.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getFocus();

		/**
		 * @brief Gets whether the window can be focused.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getCanFocus();

		/**
		 * @brief Sets whether the window can be resized.
		 * 
		 * @param v 
		 */
		void setResizable(bool v);

		/**
		 * @brief Gets whether the window can be resized.
		 * 
		 */
		bool getResizable();

		/**
		 * @brief Sets whether the window can be moved.
		 * 
		 * @param v 
		 */
		void setMovable(bool v);

		/**
		 * @brief Gets whether the window can be moved.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getMovable();

		/**
		 * @brief Sets the Mouse VWheel Value Pointer 
		 * 		This value will be modified by the window in focus.
		 * 		Allows the Input class or any other class to get mouse wheel values with out accessing the window directly.
		 * 
		 * @param v 
		 */
		static void setMouseVWheelValuePointer(int* v);

		/**
		 * @brief Sets the Mouse HWheel Value Pointer 
		 * 		This value will be modified by the window in focus.
		 * 		Allows the Input class or any other class to get mouse wheel values with out accessing the window directly.
		 * 
		 * @param v 
		 */
		static void setMouseHWheelValuePointer(int* v);

        
        void testGL();
		bool getShouldEnd();
		void swapBuffers();
		
	private:
		
		static std::vector<GLWindow*> windowList;
		static int screenWidth;
		static int screenHeight;

		static void removeWindowFromList(GLWindow* value);
		static GLWindow* getWindowByHandle(size_t value);

		//CHANGE WITH OTHER OS

		#ifdef LINUX

		#else
		static LRESULT _stdcall wndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam);
		#endif
		//

        //OPENGL STUFF
        void setPixelFormat();
        void initGL();
        void disposeGL();
        //

		void init(int x, int y, int width, int height, std::wstring title, unsigned char windowType);
		
		void setAllFunctionsToNull();
		
		void setRunning(bool value);
		void setValid(bool value);
		void setFinishInit(bool value);
		void setResizing(bool value);
		void setResizeMe(bool value);

		bool getFinishInit();

		bool getResizing();
		bool getResizeMe();

		void run();
		void dispose();

		bool shouldFocus = false;
		void threadSetFocus();
		void setShouldFocus(bool v);
		bool getShouldFocus();

		bool shouldEnd = false;
		void setShouldEnd(bool v);

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

		std::thread* wndThread = nullptr;
		std::mutex myMutex = std::mutex();
		bool threadOwnership = true;

		unsigned int sleepTimeMillis = 16;
		unsigned int sleepTimeMicros = 0;

		void threadUpdate();

		//At the cost of potential portability and bad code.
		size_t windowHandle;

		#ifdef LINUX
			Display* displayServer;
			int screen = -1;
			// Pixmap bitmap;
		#else
			WNDCLASSW wndClass;
			HINSTANCE hins;
			HDC ghDC;
            HGLRC ghRC;
		#endif
		
		bool valid = true;
		bool running = true;

		bool resizing = false;
		bool resizeMe = false;

		bool finishedInit = false;

		bool focused = false;
		bool canFocus = true;
		bool canResize = true;
		bool canMove = true;

		std::function<void()> mouseMovedFunction;
		std::function<void()> closingFunction;

		std::function<void(unsigned long, long)> keyUpFunction;
		std::function<void(unsigned long, long)> keyDownFunction;

		std::function<void(int)> mouseDoubleClickFunction;
		std::function<void(int)> mouseButtonDownFunction;
		std::function<void(int)> mouseButtonUpFunction;
		std::function<void(int)> mouseWheelFunction;
		std::function<void(int)> mouseHWheelFunction;
	};

} //NAMESPACE glib END