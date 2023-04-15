#pragma once
#include "Image.h"
#include "GuiManager.h"

#ifndef NO_WINDOW

	#ifdef __unix__
		#include <X11/Xlib.h>
		#include <X11/Xatom.h>
		#include <X11/Xutil.h>

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

		#ifndef NOMINMAX
			#define NOMINMAX
		#endif
		
		#include <Windows.h>
		#include <dwmapi.h>
		
		#pragma comment(lib, "Msimg32.lib")
		#pragma comment(lib, "Dwmapi.lib")
	#endif

	#include <exception>
	#include <thread>
	#include <vector>
	#include <mutex>

	#ifndef DWMWA_WINDOW_CORNER_PREFERENCE_CONST
		#define DWMWA_WINDOW_CORNER_PREFERENCE_CONST 33
	#endif
	#ifndef DWMWCP_DEFAULT_CONST
		#define DWMWCP_DEFAULT_CONST 0
	#endif
	#ifndef DWMWCP_DONOTROUND_CONST
		#define DWMWCP_DONOTROUND_CONST 1
	#endif
	#ifndef DWMWCP_ROUND_CONST
		#define DWMWCP_ROUND_CONST 2
	#endif
	#ifndef DWMWCP_ROUNDSMALL_CONST
		#define DWMWCP_ROUNDSMALL_CONST 3
	#endif


	namespace glib
	{
		class SimpleWindow;
		
		struct WindowOptions
		{
			unsigned char windowType = 0;
			bool focusable = true;
			bool threadManaged = true;
			unsigned char cornerType = DWMWCP_DEFAULT_CONST;
			bool iconIsFile = true;
			std::string iconFileString;
			std::function<void(SimpleWindow*)> initFunction = nullptr;
		};

		class SimpleWindow : public Object
		{
		public:
			static const unsigned char MOUSE_LEFT = 0x00;
			static const unsigned char MOUSE_MIDDLE = 0x01;
			static const unsigned char MOUSE_RIGHT = 0x02;

			static const unsigned char NORMAL_WINDOW = 0;
			static const unsigned char BORDERLESS_WINDOW = 1;
			static const unsigned char FULLSCREEN_WINDOW = 2;
			static const unsigned char TRANSPARENT_WINDOW = 3;

			static const unsigned char STATE_NORMAL = 0x00;
			static const unsigned char STATE_MAXIMIZED = 0x01;
			static const unsigned char STATE_MINIMIZED = 0x02;
			
			static const bool TYPE_FOCUSABLE = true;
			static const bool TYPE_NONFOCUSABLE = false;

			static const bool TYPE_THREAD_MANAGED = true;
			static const bool TYPE_USER_MANAGED = false;

			static const unsigned char TYPE_DEFAULT_CORNER = DWMWCP_DEFAULT_CONST;
			static const unsigned char TYPE_NO_ROUND_CORNER = DWMWCP_DONOTROUND_CONST;
			static const unsigned char TYPE_ROUND_CORNER = DWMWCP_ROUND_CONST;
			static const unsigned char TYPE_SMALLROUND_CORNER = DWMWCP_ROUNDSMALL_CONST;
			
			struct WindowCreationError : public std::exception
			{
				const char* what() const noexcept { return "Error creating window"; }
			};
			
			/**
			 * @brief Construct a new SimpleWindow object
			 * 		By default:
			 * 			the window does not have a name,
			 * 			width is 320px wide,
			 * 			height is 240px high,
			 * 			the window is centered in the screen of the main monitor,
			 * 			the window is managed by a thread,
			 * 			and the window is a normal window (has a title bar with min, max, close buttons, and is resizable) with software rendering.
			 */
			SimpleWindow();

			/**
			 * @brief Construct a new SimpleWindow object
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
			SimpleWindow(std::wstring title, int width = 320, int height = 240, int x = -1, int y = -1, WindowOptions windowType = {});

			/**
			 * @brief Construct a new SimpleWindow object
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
			SimpleWindow(std::string title, int width = 320, int height = 240, int x = -1, int y = -1, WindowOptions windowType = {});

			/**
			 * @brief Destroy the SimpleWindow object
			 * 		Any data managed by the window, such as the window's GuiManager, should be considered invalid and
			 * 		unusable.
			 * 
			 */
			~SimpleWindow();

			//Object and Class Stuff
			static const Class globalClass;

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
			 * @brief Sets the size of the window to the GuiManager surface.
			 * 
			 */
			void resizeToGuiManager();

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
			 * @brief Gets the size of the pixel array for the window.
			 * 		The pixel array is in B,G,R format.
			 * 
			 * @return int 
			 */
			int getImageSize();

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
			 * @brief Sets the Paint Function
			 * 		The function will be called when the window needs to repaint and allows custom rendering
			 * 		using the drawImage() function.
			 * 
			 * @param function 
			 */
			void setPaintFunction(std::function<void()> function);

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
			 * @brief Gets the GuiManager for the window
			 * 
			 * @return GuiManager* 
			 */
			GuiManager* getGuiManager();

			/**
			 * @brief Sets whether the GuiManager for the window should be active.
			 * 
			 * @param v 
			 */
			void setActivateGui(bool v);

			/**
			 * @brief Gets whether the GuiManager for the window is active.
			 * 
			 * @return true 
			 * @return false 
			 */
			bool getActivateGui();
			
			/**
			 * @brief Sets the pixels of the window to the pixels of an image.
			 * 		The image does not have to be the same size as the window.
			 * @param g 
			 */
			void drawImage(Image* g);

			/**
			 * @brief Updates the GuiManager for the window.
			 * 		This function is only used when the window is in USER_MANAGED mode.
			 */
			void guiUpdate();

			/**
			 * @brief Updates the window.
			 * 		This function is only used when the window is in USER_MANAGED mode.
			 * 		This allows the events for the window to be processed.
			 */
			void update();

			/**
			 * @brief Repaints the window.
			 * 		When the window is in USER_MANAGED mode, it repaints immediately.
			 * 		When the window is in THREAD_MANAGED mode, it request the thread to repaint.
			 * 			This is used when thread auto repaint is set to false.
			 */
			void repaint();

			/**
			 * @brief Sets if the window should auto repaint when managed by a thread.
			 * 
			 * @param v 
			 */
			void setThreadAutoRepaint(bool v);

			/**
			 * @brief Sets the minimum amount of time for updating and repainting the window.
			 * 		The time taken can be above the specified time depending on the complexity
			 * 		of the update and rendering functions.
			 * 
			 * @param millis 
			 * @param micros 
			 */
			void setThreadUpdateTime(size_t millis, size_t micros = 0);

			/**
			 * @brief Waits until the window has closed to continue.
			 * 		If the window is user managed, it will process the window messages
			 * 		and gui until closed. Otherwise, it just sleeps till close.
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
			 * @brief Returns whether the window should be repainted or not.
			 * 		Can reduce the number of redraws needed as long as nothing has changed.
			 * 
			 * @return true 
			 * @return false 
			 */
			bool getRepaint();

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
			 * @brief Sets the window as the focus for the Input Class.
			 * 		This causes the Input class to use this windows mouse wheel values and 
			 * 		receive characters typed into the window.
			 * 		
			 * 		Note: This does not include normal raw key presses as they are independent of the window.
			 * 		Only one window can be the Input focus.
			 */
			void setWindowAsInputFocus();

		protected:
			
			/**
			 * @brief Construct a new Simple Window object. This is used for objects
			 * 		that wish to extend from the SimpleWindow class but not recreate a lot of basic
			 * 		implementation.
			 * 
			 * @param NoCreation 
			 */
			SimpleWindow(bool NoCreation);

			static std::vector<SimpleWindow*> windowList;
			static int screenWidth;
			static int screenHeight;

			static void removeWindowFromList(SimpleWindow* value);
			static SimpleWindow* getWindowByHandle(size_t value);

			//CHANGE WITH OTHER OS

			#ifdef __unix__
			void x11EventProc();
			#else
			static LRESULT _stdcall wndProc(HWND hwnd, UINT uint, WPARAM wparam, LPARAM lparam);
			#endif
			//

			void init(int x, int y, int width, int height, std::wstring title, WindowOptions windowType);
			
			void setAllFunctionsToNull();
			
			void initBitmap();

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
			bool getShouldEnd();

			int x = 0;
			int y = 0;
			int width = 320;
			int height = 240;

			unsigned char windowState = STATE_NORMAL;
			WindowOptions windowType = {};

			int preX = 0;
			int preY = 0;
			
			std::wstring title = L"";

			unsigned char* wndPixels = nullptr;

			int wndPixelsSize = 0;
			int scanLinePadding = 0;

			std::thread* wndThread = nullptr;
			std::mutex myMutex = std::mutex();
			bool threadOwnership = true;
			bool shouldRepaint = false;
			bool autoRepaint = true;
			size_t sleepTimeMillis = 16;
			size_t sleepTimeMicros = 0;

			
			void threadUpdate();
			void threadGuiUpdate();
			bool threadRender();
			void threadRepaint();

			//At the cost of potential portability and bad code.
			size_t windowHandle = 0;

			#ifdef __unix__
				Display* displayServer = nullptr;
				int screen = -1;
				GC gc = 0;
				Atom wmDeleteMessage = 0;
				XImage* drawableImage = nullptr;
			#else
				WNDCLASSEXW wndClass = {};
				HINSTANCE hins = NULL;
				HBITMAP bitmap = NULL;
				HICON handleToIcon = NULL;
				BITMAPINFO bitInfo = {};
				HDC myHDC = NULL;
			#endif
			
			GuiManager* gui = nullptr;
			bool activateGui = true;

			bool valid = true;
			bool running = true;

			bool resizing = false;
			bool resizeMe = false;

			bool finishedInit = false;

			bool focused = false;
			bool canFocus = true;
			bool canResize = true;
			bool canMove = true;
			bool noWindowProcPaint = false;

			std::function<void()> paintFunction;
			std::function<void()> mouseMovedFunction;
			std::function<void()> closingFunction;

			std::function<void(unsigned long, long)> keyUpFunction;
			std::function<void(unsigned long, long)> keyDownFunction;
			
			std::function<void(int)> mouseButtonDownFunction;
			std::function<void(int)> mouseButtonUpFunction;
			std::function<void(int)> mouseWheelFunction;
			std::function<void(int)> mouseHWheelFunction;

			std::function<void(int)> internalMouseWheelFunction;
			std::function<void(int)> internalMouseHWheelFunction;
			std::function<void(unsigned int, unsigned int)> internalCharValFunction;
		};

	} //NAMESPACE glib END

#endif