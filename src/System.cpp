#include "System.h"
#include "StringTools.h"
#include "MathExt.h"
#include <thread>
#include <mutex>
#include "SimpleFile.h"
#include "Input.h"

#ifdef __unix__
	#include <X11/Xlib.h>
	#include <sys/types.h>
	#include <sys/sysinfo.h>
	#include <sys/times.h>
	#include <sys/vtimes.h>
#endif

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	
	#include <Windows.h>

	#include <TlHelp32.h>
	#include <ShlObj.h>
	#include <commdlg.h>

	#include <Psapi.h>
	#include <Pdh.h>
	#pragma comment(lib, "Pdh.lib")
#endif


namespace glib
{
	
	size_t System::dbtime[16];
	unsigned int System::numberOfThreads = std::thread::hardware_concurrency();
	bool System::hasInit = false;

	const FileFilter System::ALL_FILTER = {"All Files", "."};
	const FileFilter System::IMAGE_FILTER = {"Image", ".bmp;.gif;.png;.jpg;.jpeg"};
	const FileFilter System::TEXT_FILTER = {"Text", ".txt"};
	const FileFilter System::SOUND_FILTER = {"Sound", ".wav;.ogg;.mp3"};
	const FileFilter System::VIDEO_FILTER = {"Video", ".mp4;.flv;.m4a;.wmv"};

	//Non static but global
	std::mutex lock;
	
	#ifdef _WIN32
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
	#endif

	void System::init()
	{
		#ifdef _WIN32
			lock.lock();
			if(!hasInit)
			{
				PdhOpenQueryW(NULL, NULL, &cpuQuery);
				PdhAddEnglishCounterW(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
				PdhCollectQueryData(cpuQuery);
				hasInit = true;
			}
			lock.unlock();
		#endif

	}

	size_t System::getCurrentTimeMillis()
	{
		auto t = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now());
		auto value = std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch());

		return value.count();
	}

	size_t System::getCurrentTimeMicro()
	{
		auto t = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now());
		auto value = std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch());

		return value.count();
	}

	size_t System::getCurrentTimeNano()
	{
		auto t = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now());
		auto value = std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch());

		return value.count();
	}
	
	std::tm System::getCurrentDate()
	{
		time_t currentTime = time(nullptr);
		return System::convertTimeToDate(currentTime);
	}

	std::tm System::convertTimeToDate(time_t t)
	{
		std::tm currentStoredDate;
		
		#ifdef __unix__
			#ifdef __STDC_LIB_EXT1__
				localtime_s(&t, &currentStoredDate);
			#else
				lock.lock();
				currentStoredDate = *std::localtime(&t); //No need to delete.
				lock.unlock();
			#endif
		#endif

		#ifdef _WIN32
			localtime_s(&currentStoredDate, &t);
		#else
			lock.lock();
			currentStoredDate = *std::localtime(&t); //No need to delete.
			lock.unlock();
		#endif
		
		return currentStoredDate;
	}

	void System::sleep(int millis, int micros)
	{
		std::chrono::microseconds timespan(millis*1000 + micros);
		std::this_thread::sleep_for(timespan);
	}

	void System::delayRun(void(*function)(), int millis, int micros)
	{
		sleep(millis, micros);
		function();
	}

	void System::delayRun(std::function<void()> function, int millis, int micros)
	{
		sleep(millis, micros);
		function();
	}

	unsigned int System::getNumberOfThreads()
	{
		return numberOfThreads;
	}

	int System::getMouseX()
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			POINT p = {};
			GetCursorPos(&p);
			return p.x;

		#endif
		return -1;
	}

	int System::getMouseY()
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			POINT p = {};
			GetCursorPos(&p);
			return p.y;

		#endif
		return -1;
	}

	bool System::getKeyDown(int key)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			bool v = (GetKeyState(key)>>15 & 0x01) == 1;
			return v;

		#endif
		return false;
	}

	bool System::getKeyToggled(int key)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32
			bool v = (GetKeyState(key) & 0x01) == 1;
			return v;
		#endif
		return false;
	}

	bool System::getMouseDown(int value)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			switch (value)
			{
			case Input::LEFT_MOUSE_BUTTON:
				return (GetKeyState(VK_LBUTTON) >> 15) == 1;
			case Input::RIGHT_MOUSE_BUTTON:
				return (GetKeyState(VK_RBUTTON) >> 15) == 1;
			case Input::MIDDLE_MOUSE_BUTTON:
				return (GetKeyState(VK_MBUTTON) >> 15) == 1;
			case Input::X1_MOUSE_BUTTON:
				return (GetKeyState(VK_XBUTTON1) >> 15) == 1;
			case Input::X2_MOUSE_BUTTON:
				return (GetKeyState(VK_XBUTTON2) >> 15) == 1;
			default:
				return false;
			}

		#endif
		return false;
	}

	int System::getDesktopWidth()
	{
		#ifdef _WIN32
		return GetSystemMetrics(SM_CXSCREEN);
		#endif

		#ifdef __unix__
		int w = 0;
		Display* d = XOpenDisplay(NULL);
		if(d == nullptr)
			return w;
		int s = DefaultScreen(d);
		w = DisplayWidth(d, s);
		return w;
		#endif
		
		return -1;
	}

	int System::getDesktopHeight()
	{
		#ifdef _WIN32
		return GetSystemMetrics(SM_CYSCREEN);
		#endif

		#ifdef __unix__
		int h = 0;
		Display* d = XOpenDisplay(NULL);
		if(d == nullptr)
			return h;
		int s = DefaultScreen(d);
		h = DisplayHeight(d, s);
		return h;
		#endif

		return -1;
	}

	int System::getAmountOfMonitors()
	{
		#ifdef _WIN32
		return GetSystemMetrics(SM_CMONITORS);
		#endif
		
		#ifdef __unix__
		int m = 0;
		Display* d = XOpenDisplay(NULL);
		if(d == nullptr)
			return m;
		// return ((_XPrivDisplay)display)->screens; //TODO
		#endif
		return 0;
	}

	void System::saveScreenShot(size_t hwnd, File file)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			HWND wndHandle = (HWND)hwnd;
			HDC hdc = GetDC(wndHandle);

			HBITMAP bitmap = CreateCompatibleBitmap(hdc, getDesktopWidth(), getDesktopHeight());
			BITMAPINFO bmi;
			ZeroMemory(&bmi, sizeof(BITMAPINFO));
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biHeight = getDesktopHeight();
			bmi.bmiHeader.biWidth = getDesktopWidth();
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biSizeImage = 0;
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biCompression = BI_RGB;

			int windowWidth = bmi.bmiHeader.biWidth;
			int windowHeight = bmi.bmiHeader.biHeight;

			int scanLinePadding = windowWidth%4;
			int size = (windowWidth*3 + scanLinePadding) * windowHeight;

			unsigned char* pixels = new unsigned char[size];

			HDC capDC = CreateCompatibleDC(hdc);
			HGDIOBJ hOld = SelectObject(capDC, bitmap);
			BitBlt(capDC, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, hdc, 0, 0, SRCCOPY);
			SelectObject(capDC, hOld);
			DeleteDC(capDC);

			int returnVal = GetDIBits(hdc, bitmap, 0, getDesktopHeight(), pixels, &bmi, DIB_RGB_COLORS);

			BITMAPFILEHEADER bmfh;
			bmfh.bfType = ((WORD)'M' << 8) + (WORD)'B';
			bmfh.bfReserved1 = 0;
			bmfh.bfReserved2 = 0;
			bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			bmfh.bfSize = bmfh.bfOffBits + size;

			SimpleFile f = SimpleFile(file, SimpleFile::WRITE);

			if (f.isOpen())
			{
				f.writeBytes((unsigned char*)& bmfh, sizeof(BITMAPFILEHEADER));
				f.writeBytes((unsigned char*)& bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
				f.writeBytes(pixels, size);
				f.close();
			}

			DeleteObject(bitmap);
			delete[] pixels;

		#endif
	}

	Image* System::getScreenShot(size_t hwnd)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32
			HWND wndHandle = (HWND)hwnd;
			HDC hdc = GetDC(wndHandle);

			RECT rect;
			GetWindowRect(wndHandle, &rect);

			int windowWidth = rect.right - rect.left;
			int windowHeight = rect.bottom - rect.top;

			HBITMAP bitmap = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
			BITMAPINFO bmi;
			ZeroMemory(&bmi, sizeof(BITMAPINFO));
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biHeight = windowHeight;
			bmi.bmiHeader.biWidth = windowWidth;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biSizeImage = 0;
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biCompression = BI_RGB;

			int scanLinePadding = windowWidth%4;

			int size = (windowWidth*3 + scanLinePadding) * windowHeight;

			if(size <= 0)
			{
				DeleteObject(bitmap);
				return nullptr;
			}
			
			unsigned char* pixels = new unsigned char[size];

			HDC capDC = CreateCompatibleDC(hdc);
			HGDIOBJ hOld = SelectObject(capDC, bitmap);
			BitBlt(capDC, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, hdc, 0, 0, SRCCOPY);
			SelectObject(capDC, hOld);
			DeleteDC(capDC);

			int returnVal = GetDIBits(hdc, bitmap, 0, windowHeight, pixels, &bmi, DIB_RGB_COLORS);

			Image* finalImage = new Image(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight);

			unsigned char* startP = pixels;
			unsigned char* endP = pixels + size;
			
			for(int y=finalImage->getHeight()-1; y>=0; y--)
			{
				for(int x=0; x<finalImage->getWidth(); x++)
				{
					Color c = {0,0,0,255};
					c.blue = *startP;
					startP++;
					c.green = *startP;
					startP++;
					c.red = *startP;
					startP++;

					finalImage->setPixel(x,y,c);
				}
				startP += scanLinePadding;
			}

			DeleteObject(bitmap);
			delete[] pixels;
			
			return finalImage;
		#endif

		return nullptr;
	}

	void System::saveScreenShotDesktop(File f)
	{
		#ifdef _WIN32
		System::saveScreenShot((size_t)GetDesktopWindow(), f);
		#endif
	}

	Image* System::getScreenShotDesktop()
	{
		#ifdef _WIN32
		return System::getScreenShot((size_t)GetDesktopWindow());
		#endif
		return nullptr;
	}

	void System::paintImageToWindow(size_t hwnd, Image* img, int startX, int startY)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			HDC hdc = GetDC((HWND)hwnd);
			HDC mem = CreateCompatibleDC(hdc);

			RECT rect;
			GetWindowRect((HWND)hwnd, &rect);

			int windowWidth = MathExt::abs(rect.right - rect.left);
			int windowHeight = MathExt::abs(rect.bottom - rect.top);

			int hei = MathExt::clamp(img->getHeight(), 0, windowHeight);
			int wid = MathExt::clamp(img->getWidth(), 0, windowWidth);

			HBITMAP bitmap = CreateCompatibleBitmap(hdc, wid, hei);
			BITMAP bimg;

			BITMAPINFO bmi;
			ZeroMemory(&bmi, sizeof(BITMAPINFO));
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biHeight = hei;
			bmi.bmiHeader.biWidth = wid;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biSizeImage = 0;
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biCompression = BI_RGB;

			int scanLinePadding = windowWidth%4;
			int size = (windowWidth*3 + scanLinePadding) * windowHeight;

			unsigned char* pixels = new unsigned char[size];
			unsigned char* startP = pixels;

			//copy image into pixels
			for(int y = hei-1; y>=0; y--)
			{
				for(int x = 0; x<wid; x++)
				{
					Color c = img->getPixel(x, y);
					*startP = c.blue;
					startP++;
					*startP = c.green;
					startP++;
					*startP = c.red;
					startP++;
				}
				startP += scanLinePadding;
			}

			SetDIBits(hdc, bitmap, 0, hei, &pixels[0], &bmi, DIB_RGB_COLORS);
			
			
			HGDIOBJ hOld = SelectObject(mem, bitmap);
			GetObject(bitmap, sizeof(BITMAP), &bimg);
			BitBlt(hdc, startX, startY, bimg.bmWidth, bimg.bmHeight, mem, 0, 0, SRCCOPY);
			DeleteDC(mem);

			DeleteObject(bitmap);
			delete[] pixels;

		#endif
	}

	void System::paintImageToDesktop(Image* img, int startX, int startY)
	{
		#ifdef _WIN32
			System::paintImageToWindow((size_t)GetDesktopWindow(), img, startX, startY);
		#endif
	}

	unsigned long System::getProcessID(std::wstring processName)
	{
		//TODO - LINUX VERSION
		#ifdef _WIN32

			DWORD pid = 0;
			HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPMODULE, 0);
			if (hndl)
			{
				PROCESSENTRY32W process = { sizeof(PROCESSENTRY32W) };
				Process32FirstW(hndl, &process);
				do
				{
					std::wstring thisProcess = process.szExeFile;
					if (thisProcess == processName)
					{
						pid = process.th32ProcessID;
						break;
					}
					
				} while (Process32NextW(hndl, &process));

				CloseHandle(hndl);
			}

			return pid;

		#endif
		return 0;
	}

	size_t System::getProcessWindow(std::string windowName)
	{
		//TODO - LINUX VERSION
		#ifdef _WIN32

			HWND hwnd = FindWindowA(NULL, windowName.c_str());
			if(hwnd == 0)
				hwnd = FindWindowA(windowName.c_str(), NULL);

			return (size_t)hwnd;

		#endif
		return 0;
	}

	std::string System::fileDialogBox(unsigned char type, std::vector<FileFilter> filters, std::string startDir)
	{
		//TODO - __unix__ VERSION
		#ifdef _WIN32

			//older win32 method. Works just fine so I'm keeping it.
			
			//build the filter text
			std::vector<char> filterText;
			
			for(int i=0; i<filters.size(); i++)
			{
				for(int k=0; k<filters[i].name.size(); k++)
				{
					filterText.push_back(filters[i].name[k]);
				}
				filterText.push_back('\0');

				std::vector<std::string> exts = StringTools::splitString(filters[i].extensions, ',');
				for(int j=0; j<exts.size(); j++)
				{
					if(j>0)
					{
						filterText.push_back( ';' );
					}
					filterText.push_back('*');
					for(int k=0; k<exts[j].size(); k++)
					{
						filterText.push_back(exts[j][k]);
					}
				}
				filterText.push_back('\0');
			}

			if(filters.size()==0)
			{
			}

			char* initDir = nullptr;
			if(!startDir.empty())
			{
				initDir = startDir.data();
			}

			if(type==TYPE_OPEN_FILE)
			{
				OPENFILENAMEA resStruct = {0};
				char* stringArray = new char[MAX_PATH]{0};

				resStruct.lStructSize = sizeof(OPENFILENAMEA);
				resStruct.lpstrFile = stringArray;
				resStruct.nMaxFile = MAX_PATH;
				resStruct.lpstrFilter = filterText.data();
				resStruct.nFilterIndex = 1;
				resStruct.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				resStruct.lpstrInitialDir = initDir;

				bool result = GetOpenFileNameA(&resStruct);
				
				std::string finalString = stringArray;
				delete[] stringArray;

				return finalString;
			}
			else if(type==TYPE_OPEN_FOLDER)
			{
				std::wstring finalString = L"";
				HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
				COINIT_DISABLE_OLE1DDE);

				if (SUCCEEDED(hr))
				{
					IFileOpenDialog *pFileOpen;

					// Create the FileOpenDialog object.
					hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
							IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

					if (SUCCEEDED(hr))
					{
						DWORD flags;
						pFileOpen->GetOptions(&flags);
						pFileOpen->SetOptions(flags | FOS_PICKFOLDERS);
						// Show the Open dialog box.
						hr = pFileOpen->Show(NULL);

						// Get the file name from the dialog box.
						if (SUCCEEDED(hr))
						{
							IShellItem *pItem;
							hr = pFileOpen->GetResult(&pItem);
							if (SUCCEEDED(hr))
							{
								PWSTR pszFilePath;
								hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

								// Display the file name to the user.
								if (SUCCEEDED(hr))
								{
									finalString = pszFilePath;
									CoTaskMemFree(pszFilePath);
								}
								pItem->Release();
							}
						}
						pFileOpen->Release();
					}
					CoUninitialize();
				}
				return StringTools::toUTF8String(finalString);
			}
			else if(type==TYPE_SAVE_FILE)
			{
				OPENFILENAMEA resStruct = {0};
				char* stringArray = new char[MAX_PATH]{0};

				resStruct.lStructSize = sizeof(OPENFILENAMEA);
				resStruct.lpstrFile = stringArray;
				resStruct.nMaxFile = MAX_PATH;
				resStruct.lpstrFilter = filterText.data();
				resStruct.nFilterIndex = 1;
				resStruct.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
				resStruct.lpstrInitialDir = initDir;

				bool result = GetSaveFileNameA(&resStruct);
				
				std::string finalString = stringArray;
				delete[] stringArray;

				return finalString;
			}

		#endif

		return "";
	}
	
	int System::messageBoxPopup(unsigned int type, std::string title, std::string message)
	{
		//TODO - LINUX VERSION
		#ifdef _WIN32
			return MessageBoxA(NULL, message.c_str(), title.c_str(), type);
		#endif
		return 0;
	}

	void System::copyToClipboard(std::string str)
	{
		//TODO - LINUX VERSION
		#ifdef _WIN32

			if(!OpenClipboard(NULL))
				return;

			EmptyClipboard();

			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, str.size()+1);
			char* strPointer = (char*)GlobalLock(hMem);
			
			for(int i=0; i<str.size(); i++)
				strPointer[i] = str[i];
			
			strPointer[str.size()] = '0';

			GlobalUnlock(hMem);

			SetClipboardData(CF_TEXT, hMem);
			CloseClipboard();

		#endif
	}

	void System::copyToClipboard(std::wstring str)
	{
		//TODO - LINUX VERSION
		#ifdef _WIN32

			if(!OpenClipboard(NULL))
				return;

			EmptyClipboard();

			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (str.size()+1)*2);
			wchar_t* strPointer = (wchar_t*)GlobalLock(hMem);

			for(int i=0; i<str.size(); i++)
				strPointer[i] = str[i];
			
			strPointer[str.size()] = L'0';

			GlobalUnlock(hMem);

			SetClipboardData(CF_UNICODETEXT, hMem);
			CloseClipboard();

		#endif
	}

	std::string System::pasteFromClipboard()
	{
		//TODO - LINUX VERSION
		#ifdef _WIN32

			if(!OpenClipboard(NULL))
				return "";

			std::wstring finalText;
			HANDLE clipboardData = GetClipboardData(CF_UNICODETEXT);
			if(clipboardData != NULL)
			{
				wchar_t* textPointer = (wchar_t*)GlobalLock(clipboardData);
				if(textPointer != nullptr)
				{
					finalText = textPointer;
				}

				GlobalUnlock(clipboardData);
			}

			CloseClipboard();

			return StringTools::toUTF8String(finalText);
		#endif
		return "";
	}

	
	void System::clearClipboard()
	{
		//TODO - LINUX VERSION
		#ifdef _WIN32

			if(!OpenClipboard(NULL))
				return;

			EmptyClipboard();
			CloseClipboard();

		#endif
	}

	size_t System::getTotalVirtualMem()
	{
		#ifdef _WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return memInfo.ullTotalPageFile;
		#endif
		return 0;
	}

	size_t System::getTotalPhysicalMem()
	{
		#ifdef _WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return memInfo.ullTotalPhys;
		#endif
		return 0;
	}
	
	size_t System::getTotalVirtualMemAvaliable()
	{
		#ifdef _WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return memInfo.ullAvailPageFile;
		#endif
		return 0;
	}

	size_t System::getTotalPhysicalMemAvaliable()
	{
		#ifdef _WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return memInfo.ullAvailPhys;
		#endif
		return 0;
	}

	size_t System::getVirtualMemUsed()
	{
		#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(PROCESS_MEMORY_COUNTERS_EX));
		return pmc.PrivateUsage;
		#endif
		return 0;
	}

	size_t System::getPhysicalMemUsed()
	{
		#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(PROCESS_MEMORY_COUNTERS_EX));
		return pmc.WorkingSetSize;
		#endif
		return 0;
	}
	
	double System::getTotalCpuUsage()
	{
		
		#ifdef _WIN32
		System::init();
		
		lock.lock();
		PDH_FMT_COUNTERVALUE counterVal;
		PdhCollectQueryData(cpuQuery);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		lock.unlock();
		
		return counterVal.doubleValue;
		#endif
		return 0;
	}
	
	double System::getCpuUsage()
	{
		System::init();
		
		return 0;
	}
	
} //NAMESPACE glib END