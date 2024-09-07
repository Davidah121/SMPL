#pragma once
#include "BuildOptions.h"
#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>
#include <chrono>
#include <functional>
#include <string>
#include "Image.h"


namespace smpl
{

	struct FileFilter
	{
		std::string name;
		std::string extensions;
	};

	class DLL_OPTION System
	{
	public:

		static size_t dbtime[16];

		/**
		 * @brief Initializes performance counters.
		 * 
		 */
		static void init();

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
		 * @brief Gets the Current Date as a tm struct.
		 * 		Thread-Safe
		 * 		Uses localtime_s or localtime_r if available. 
		 * 			Otherwise, uses a mutex.
		 * 
		 * @return std::tm 
		 */
		static std::tm getCurrentDate();

		/**
		 * @brief Gets the Current Date as a tm struct.
		 * 		Thread-Safe
		 * 		Uses localtime_s or localtime_r if available. 
		 * 			Otherwise, uses a mutex.
		 * 
		 * @param t
		 * @return std::tm 
		 */
		static std::tm convertTimeToDate(time_t t);

		/**
		 * @brief Sleeps for the specified amount of time.
		 * 		Sleeping relinquishes the thread so that it may be allocated to other tasks.
		 * 
		 * @param millis 
		 * @param micros 
		 * 		Default is 0.
		 * @param accurate
		 * 		Whether to use std::this_thread::yield for a more accurate time or use std::this_thread::sleep_for()
		 * 			sleep_for() will likely cause a context switch which will result in worse accuracy but potentially better
		 * 			system performance.
		 * 		Default is true.
		 */
		static void sleep(int millis, int micros=0, bool accurate=true);

		/**
		 * @brief Runs the function after a specified amount of time has passed.
		 * 
		 * @param function 
		 * @param millis 
		 * @param micros 
		 * 		Default is 0.
		 * @param accurate
		 * 		Whether to use std::this_thread::yield for a more accurate time or use std::this_thread::sleep_for()
		 * 			sleep_for() will likely cause a context switch which will result in worse accuracy but potentially better
		 * 			system performance.
		 * 		Default is true.
		 */
		static void delayRun(void (*function)(), int millis, int micros = 0, bool accurate=true);

		/**
		 * @brief Runs the function after a specified amount of time has passed.
		 * 
		 * @param function 
		 * @param millis 
		 * @param micros 
		 * 		Default is 0.
		 * @param accurate
		 * 		Whether to use std::this_thread::yield for a more accurate time or use std::this_thread::sleep_for()
		 * 			sleep_for() will likely cause a context switch which will result in worse accuracy but potentially better
		 * 			system performance.
		 * 		Default is true.
		 */
		static void delayRun(std::function<void()> function, int millis, int micros = 0, bool accurate=true);

		/**
		 * @brief Gets the number of threads that the processor has.
		 * 
		 * @return unsigned int 
		 */
		static unsigned int getNumberOfThreads();

		/**
		 * @brief Map the interupt signal raised by CTRL-C to a specific function.
		 * 		This is useful if you need to end a program but don't have access 
		 * 		to raw inputs. Can also do other cool things.
		 * 
		 * @param func 
		 */
		static void mapInteruptSignal(void(*func)(int));

		static const long NANOSECOND_SEC = 1000000000L;
		static const long MICROSECOND_SEC = 1000000L;
		static const long MILLISECOND_SEC = 1000L;

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
		static size_t getProcessWindow(std::string windowName);

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
		 * 		Default is "./" which either sets it to the last location it was at or the programs directory.
		 * @return std::string 
		 */
		static std::string fileDialogBox(unsigned char type = TYPE_OPEN_FILE, std::vector<FileFilter> filters = {}, std::string startDir = "./");

		/**
		 * @brief Creates a message box popup.
		 * 		Currently a wrapper around the function MessageBoxW in the Windows OS.
		 * 
		 * @param type 
		 * @param title 
		 * @param message 
		 * @return int 
		 */
		static int messageBoxPopup(unsigned int type, std::string title, std::string message);

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
		 * @return std::string 
		 */
		static std::string pasteFromClipboard();

		/**
		 * @brief Clears data from the clipboard.
		 * 
		 */
		static void clearClipboard();

		//Statistics and stuff
		/**
		 * @brief Gets the Total Virtual Memory possible.
		 * 
		 * @return size_t 
		 */
		static size_t getTotalVirtualMem();

		/**
		 * @brief Gets the Total amount of RAM available to the system.
		 * 
		 * @return size_t 
		 */
		static size_t getTotalPhysicalMem();
		
		/**
		 * @brief Gets the Total Virtual Memory available that has not been allocated.
		 * 
		 * @return size_t 
		 */
		static size_t getTotalVirtualMemAvaliable();

		/**
		 * @brief Gets the Total amount of RAM available to the system that has not been allocated.
		 * 
		 * @return size_t 
		 */
		static size_t getTotalPhysicalMemAvaliable();

		/**
		 * @brief Gets the Total amount of Virtual Memory Used by the program
		 * 
		 * @return size_t 
		 */
		static size_t getVirtualMemUsed();

		/**
		 * @brief Get the Total amount of Physical Memory Used by the program
		 * 
		 * @return size_t 
		 */
		static size_t getPhysicalMemUsed();
		
		/**
		 * @brief Get the Cpu for the entire system.
		 * 
		 * @return double 
		 */
		static double getTotalCpuUsage(); //TODO
		
		/**
		 * @brief Get the Cpu for the program.
		 * 
		 * @return double 
		 */
		static double getCpuUsage(); //TODO
	private:
		System();
		~System();
		static System singleton;
		static unsigned int numberOfThreads;
		static bool hasInit;
	};

} //NAMESPACE glib END