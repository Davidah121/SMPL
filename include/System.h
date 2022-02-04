#pragma once

#include <chrono>
#include <functional>
#include <string>
#include "Image.h"



namespace glib
{

	struct FileFilter
	{
		std::wstring name;
		std::wstring extensions;
	};

	class System
	{
	public:

		static size_t dbtime[16];
		static const int LEFT_MOUSE_BUTTON = 0;
		static const int RIGHT_MOUSE_BUTTON = 1;
		static const int MIDDLE_MOUSE_BUTTON = 2;
		static const int MOUSE_WHEEL_UP = 3;
		static const int MOUSE_WHEEL_DOWN = 4;
		static const int MOUSE_WHEEL_LEFT = 5;
		static const int MOUSE_WHEEL_RIGHT = 6;
		static const int X1_MOUSE_BUTTON = 7;
		static const int X2_MOUSE_BUTTON = 8;

		/**
		 * @brief Returns the current time in Milliseconds.
		 * 		Specifically, returns the time since epoch in Milliseconds.
		 * 
		 * @return size_t 
		 */
		static size_t getCurrentTimeMillis();

		/**
		 * @brief Returns the current time in Microseconds.
		 * 		Specifically, returns the time since epoch in Microseconds.
		 * 
		 * @return size_t 
		 */
		static size_t getCurrentTimeMicro();

		/**
		 * @brief Returns the current time in Nanoseconds.
		 * 		Specifically, returns the time since epoch in Nanoseconds.
		 * 
		 * @return size_t 
		 */
		static size_t getCurrentTimeNano();

		/**
		 * @brief Sleeps for the specified amount of time.
		 * 		Sleeping relinquishes the thread so that it may be allocated to other tasks.
		 * 
		 * @param millis 
		 * @param micros 
		 */
		static void sleep(int millis, int micros=0);

		/**
		 * @brief Runs the function after a specified amount of time has passed.
		 * 
		 * @param function 
		 * @param millis 
		 * @param micros 
		 */
		static void delayRun(void (*function)(), int millis, int micros = 0);

		/**
		 * @brief Runs the function after a specified amount of time has passed.
		 * 
		 * @param function 
		 * @param millis 
		 * @param micros 
		 */
		static void delayRun(std::function<void()> function, int millis, int micros = 0);

		/**
		 * @brief Gets the number of threads that the processor has.
		 * 
		 * @return unsigned int 
		 */
		static unsigned int getNumberOfThreads();

		static const long NANOSECOND_SEC = 1000000000L;
		static const long MICROSECOND_SEC = 1000000L;
		static const long MILLISECOND_SEC = 1000L;

		/**
		 * @brief Emulates a key press.
		 * 		Key is considered down until it is released.
		 * 		May not work in other OS.
		 * 
		 * @param key 
		 */
		static void emulateKeyPress(int key);

		/**
		 * @brief Emulates a key release.
		 * 		May not work in other OS.
		 * 
		 * @param key 
		 */
		static void emulateKeyRelease(int key);

		/**
		 * @brief Emulates a mouse press.
		 * 		The button is considered down until it is released.
		 * 		May not work in other OS.
		 * 
		 * @param button 
		 */
		static void emulateMousePress(int button);

		/**
		 * @brief Emulates a mouse release.
		 * 		May not work in other OS.
		 * 
		 * @param key 
		 */
		static void emulateMouseRelease(int button);

		/**
		 * @brief Emulates the mouse wheel moving.
		 * 		
		 * @param wheel 
		 * 		Valid values are:
		 * 			MOUSE_WHEEL_UP
		 * 			MOUSE_WHEEL_DOWN
		 * 			MOUSE_WHEEL_LEFT
		 * 			MOUSE_WHEEL_RIGHT
		 * @param amount 
		 * 		Any positive or negative value.
		 */
		static void emulateMouseWheel(int wheel, int amount);

		/**
		 * @brief Sets the Mouse Position
		 * 
		 * @param x 
		 * @param y 
		 */
		static void setMousePosition(int x, int y);

		/**
		 * @brief Gets the Mouse X position
		 * 
		 * @return int 
		 */
		static int getMouseX();

		/**
		 * @brief Gets the Mouse Y position
		 * 
		 * @return int 
		 */
		static int getMouseY();
		
		/**
		 * @brief Gets whether the key is down
		 * 
		 * @param key 
		 * @return true 
		 * @return false 
		 */
		static bool getKeyDown(int key);

		/**
		 * @brief Gets whether the key is toggled
		 * 		For keys like CapsLock or NumLock
		 * 
		 * @param key 
		 * @return true 
		 * @return false 
		 */
		static bool getKeyToggled(int key);

		/**
		 * @brief Gets whether the mouse button is down
		 * 
		 * @param value 
		 * @return true 
		 * @return false 
		 */
		static bool getMouseDown(int value);

		/**
		 * @brief Gets the Width of the Desktop Window.
		 * 		Affectively, the width of the monitor.
		 * 
		 * @return int 
		 */
		static int getDesktopWidth();

		/**
		 * @brief Gets the Height of the Desktop Window.
		 * 		Affectively, the height of the monitor.
		 * 
		 * @return int 
		 */
		static int getDesktopHeight();

		/**
		 * @brief Gets the amount of monitors connected
		 * 
		 * @return int 
		 */
		static int getAmountOfMonitors();
		
		/**
		 * @brief Saves a screenshot from the specified window to a file as a .bmp
		 * 		Note that the HWND may not contain any pixel data.
		 * 
		 * @param window 
		 * 		If 0 or NULL, uses the desktop window which encapsulates everything.
		 * @param filename 
		 */
		static void saveScreenShot(size_t window, File file);

		/**
		 * @brief Gets a screenshot from the specified window as an Image*.
		 * 		The Image* should be deleted when no longer needed.
		 * 
		 * @param window 
		 * 		If 0 or NULL, uses the desktop window which encapsulates everything.
		 * @return Image* 
		 */
		static Image* getScreenShot(size_t window);

		/**
		 * @brief Saves a screenshot from the desktop window to a file as a .bmp
		 * 
		 * @param filename 
		 */
		static void saveScreenShotDesktop(File file);

		/**
		 * @brief Gets a screenshot from the desktop window as an Image*.
		 * 		The Image* should be deleted when no longer needed.
		 * 
		 * @return Image* 
		 */
		static Image* getScreenShotDesktop();

		/**
		 * @brief Paints the image on the desktop window at the specified location.
		 * 		It is over written when the desktop window considers that section invalid.
		 * 		A repaint can invalidate a section.
		 * 
		 * @param img 
		 * @param startX 
		 * @param startY 
		 */
		static void paintImageToDesktop(Image* img, int startX = 0, int startY = 0);

		/**
		 * @brief Paints the image on the specified window at the specified location.
		 * 		It is over written when the desktop window considers that section invalid.
		 * 		A repaint can invalidate a section.
		 * 
		 * @param window 
		 * 		If 0 or NULL, uses the desktop window which encapsulates everything.
		 * @param img 
		 * @param startX 
		 * @param startY 
		 */
		static void paintImageToWindow(size_t window, Image* img, int startX = 0, int startY = 0);

		/**
		 * @brief Gets the ProcessID of the application by its process name
		 * 
		 * @param processName 
		 * @return unsigned long 
		 */
		static unsigned long getProcessID(std::wstring processName);

		/**
		 * @brief Gets the Window for a process by the window name.
		 * 
		 * @param windowName 
		 * @return size_t 
		 */
		static size_t getProcessWindow(std::wstring windowName);

		static const unsigned char TYPE_OPEN_FILE = 0;
		static const unsigned char TYPE_OPEN_FOLDER = 1;
		static const unsigned char TYPE_SAVE_FILE = 2;

		static const FileFilter ALL_FILTER;
		static const FileFilter IMAGE_FILTER;
		static const FileFilter TEXT_FILTER;
		static const FileFilter SOUND_FILTER;
		static const FileFilter VIDEO_FILTER;
		
		/**
		 * @brief Opens a file dialog box to either save, load, or select a directory.
		 * 		
		 * 
		 * @param type 
		 * 		Valid types are:
		 * 			TYPE_OPEN_FILE
		 * 			TYPE_OPEN_FOLDER
		 * 			TYPE_SAVE_FILE
		 * 		Default is TYPE_OPEN_FILE
		 * @param filters 
		 * 		A selection of file filters that are desired.
		 * 		Some default filters are available.
		 * 		Default is no filters.
		 * @param startDir 
		 * 		The starting directory for the file dialog box.
		 * 		Default is L"./" which either sets it to the last location it was at or the programs directory.
		 * @return std::wstring 
		 */
		static std::wstring fileDialogBox(unsigned char type = TYPE_OPEN_FILE, std::vector<FileFilter> filters = {}, std::wstring startDir = L"./");

		/**
		 * @brief Creates a message box popup.
		 * 		Currently a wrapper around the function MessageBoxW in the Windows OS.
		 * 
		 * @param type 
		 * @param title 
		 * @param message 
		 * @return int 
		 */
		static int messageBoxPopup(unsigned int type, std::wstring title, std::wstring message);

		/**
		 * @brief Copies text into the clipboard
		 * 
		 * @param text 
		 */
		static void copyToClipboard(std::string text);

		/**
		 * @brief Copies text into the clipboard
		 * 
		 * @param text 
		 */
		static void copyToClipboard(std::wstring text);

		/**
		 * @brief Retrieves the currently copied text from the clipboard if it is text.
		 * 
		 * @return std::wstring 
		 */
		static std::wstring pasteFromClipboard();

		/**
		 * @brief Clears data from the clipboard.
		 * 
		 */
		static void clearClipboard();
		
	private:
		static unsigned int numberOfThreads;
	};

} //NAMESPACE glib END