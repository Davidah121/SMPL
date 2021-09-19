#include "System.h"
#include "StringTools.h"
#include "MathExt.h"
#include <thread>

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include "SimpleFile.h"
#include <TlHelp32.h>
#include <ShlObj.h>
#include <commdlg.h>

namespace glib
{
		
	unsigned int System::numberOfThreads = std::thread::hardware_concurrency();
	const FileFilter System::ALL_FILTER = {L"All Files", L"."};
	const FileFilter System::IMAGE_FILTER = {L"Image", L".bmp;.gif;.png;.jpg;.jpeg"};;
	const FileFilter System::TEXT_FILTER = {L"Text", L".txt"};
	const FileFilter System::SOUND_FILTER = {L"Sound", L".wav;.ogg;.mp3"};
	const FileFilter System::VIDEO_FILTER = {L"Video", L".mp4;.flv;.m4a;.wmv"};

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

	void System::emulateKeyPress(int key)
	{
		INPUT i;
		ZeroMemory(&i, sizeof(INPUT));

		i.type = INPUT_KEYBOARD;
		i.ki.wVk = key;
		i.ki.dwFlags = NULL;

		SendInput(1, &i, sizeof(INPUT));
	}

	void System::emulateKeyRelease(int key)
	{
		INPUT i;
		ZeroMemory(&i, sizeof(INPUT));

		i.type = INPUT_KEYBOARD;
		i.ki.wVk = key;
		i.ki.dwFlags = KEYEVENTF_KEYUP;

		SendInput(1, &i, sizeof(INPUT));
	}

	void System::emulateMousePress(int key)
	{
		INPUT i;
		ZeroMemory(&i, sizeof(INPUT));

		i.type = INPUT_MOUSE;
		
		switch (key)
		{
		case LEFT_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			break;
		case RIGHT_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			break;
		case MIDDLE_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			break;
		case X1_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_XDOWN;
			i.mi.mouseData = XBUTTON1;
			break;
		case X2_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_XDOWN;
			i.mi.mouseData = XBUTTON2;
			break;
		default:
			break;
		}

		SendInput(1, &i, sizeof(INPUT));
	}

	void System::emulateMouseRelease(int key)
	{
		INPUT i;
		ZeroMemory(&i, sizeof(INPUT));

		i.type = INPUT_MOUSE;

		switch (key)
		{
		case LEFT_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			break;
		case RIGHT_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			break;
		case MIDDLE_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			break;
		case X1_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_XUP;
			i.mi.mouseData = XBUTTON1;
			break;
		case X2_MOUSE_BUTTON:
			i.mi.dwFlags = MOUSEEVENTF_XUP;
			i.mi.mouseData = XBUTTON2;
			break;
		default:
			break;
		}

		SendInput(1, &i, sizeof(INPUT));
	}

	void System::emulateMouseWheel(int wheel, int amount)
	{
		INPUT i;
		ZeroMemory(&i, sizeof(INPUT));

		i.type = INPUT_MOUSE;

		switch (wheel)
		{
		case MOUSE_WHEEL_UP:
			i.mi.mouseData = -WHEEL_DELTA * amount;
			i.mi.dwFlags = MOUSEEVENTF_WHEEL;
			break;
		case MOUSE_WHEEL_DOWN:
			i.mi.mouseData = WHEEL_DELTA * amount;
			i.mi.dwFlags = MOUSEEVENTF_WHEEL;
			break;
		case MOUSE_WHEEL_LEFT:
			i.mi.mouseData = -WHEEL_DELTA * amount;
			i.mi.dwFlags = MOUSEEVENTF_HWHEEL;
			break;
		case MOUSE_WHEEL_RIGHT:
			i.mi.mouseData = WHEEL_DELTA * amount;
			i.mi.dwFlags = MOUSEEVENTF_HWHEEL;
			break;
		default:
			break;
		}

		SendInput(1, &i, sizeof(INPUT));
	}

	void System::setMousePosition(int x, int y)
	{
		SetCursorPos(x, y);
	}

	int System::getMouseX()
	{
		POINT p = {};
		GetCursorPos(&p);
		return p.x;
	}

	int System::getMouseY()
	{
		POINT p = {};
		GetCursorPos(&p);
		return p.y;
	}

	bool System::getKeyDown(int key)
	{
		bool v = (GetKeyState(key)>>15 & 0x01) == 1;
		return v;
	}

	bool System::getKeyToggled(int key)
	{
		bool v = (GetKeyState(key) & 0x01) == 1;
		return v;
	}

	bool System::getMouseDown(int value)
	{
		switch (value)
		{
		case LEFT_MOUSE_BUTTON:
			return (GetKeyState(VK_LBUTTON) >> 15) == 1;
		case RIGHT_MOUSE_BUTTON:
			return (GetKeyState(VK_RBUTTON) >> 15) == 1;
		case MIDDLE_MOUSE_BUTTON:
			return (GetKeyState(VK_MBUTTON) >> 15) == 1;
		case X1_MOUSE_BUTTON:
			return (GetKeyState(VK_XBUTTON1) >> 15) == 1;
		case X2_MOUSE_BUTTON:
			return (GetKeyState(VK_XBUTTON2) >> 15) == 1;
		default:
			return false;
		}
	}

	int System::getDesktopWidth()
	{
		return GetSystemMetrics(SM_CXSCREEN);
	}

	int System::getDesktopHeight()
	{
		return GetSystemMetrics(SM_CYSCREEN);
	}

	int System::getAmountOfMonitors()
	{
		return GetSystemMetrics(SM_CMONITORS);
	}

	void System::saveScreenShot(HWND hwnd, File file)
	{
		HWND wndHandle = hwnd;
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
	}

	Image* System::getScreenShot(HWND hwnd)
	{
		HWND wndHandle = hwnd;
		HDC hdc = GetDC(wndHandle);

		RECT rect;
		GetWindowRect(hwnd, &rect);

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
	}

	void System::saveScreenShotDesktop(File f)
	{
		System::saveScreenShot(GetDesktopWindow(), f);
	}

	Image* System::getScreenShotDesktop()
	{
		return System::getScreenShot(GetDesktopWindow());
	}

	void System::paintImageToWindow(HWND hwnd, Image* img, int startX, int startY)
	{
		HDC hdc = GetDC(hwnd);
		HDC mem = CreateCompatibleDC(hdc);

		RECT rect;
		GetWindowRect(hwnd, &rect);

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
	}

	void System::paintImageToDesktop(Image* img, int startX, int startY)
	{
		System::paintImageToWindow(GetDesktopWindow(), img, startX, startY);
	}

	unsigned long System::getProcessID(std::wstring processName)
	{
		DWORD pid = 0;
		HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPMODULE, 0);
		if (hndl)
		{
			PROCESSENTRY32W  process = { sizeof(PROCESSENTRY32W) };
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
	}

	HWND System::getProcessWindow(std::wstring windowName)
	{
		HWND hwnd = FindWindowW(NULL, windowName.c_str());
		if(hwnd == 0)
			hwnd = FindWindowW(windowName.c_str(), NULL);

		return hwnd;
	}

	std::wstring System::fileDialogBox(unsigned char type, std::vector<FileFilter> filters, std::wstring startDir)
	{
		//older win32 method. Works just fine so I'm keeping it.
		
		//build the filter text
		std::vector<wchar_t> filterText;
		
		for(int i=0; i<filters.size(); i++)
		{
			for(int k=0; k<filters[i].name.size(); k++)
			{
				filterText.push_back(filters[i].name[k]);
			}
			filterText.push_back(L'\0');

			std::vector<std::wstring> exts = StringTools::splitString(filters[i].extensions, L',');
			for(int j=0; j<exts.size(); i++)
			{
				if(j>0)
				{
					filterText.push_back( L';' );
				}
				filterText.push_back(L'*');
				for(int k=0; k<exts[j].size(); k++)
				{
					filterText.push_back(exts[j][k]);
				}
			}
			filterText.push_back(L'\0');
		}

		if(filters.size()==0)
		{
		}

		wchar_t* initDir = nullptr;
		if(!startDir.empty())
		{
			initDir = startDir.data();
		}

		if(type==TYPE_OPEN_FILE)
		{
			OPENFILENAMEW resStruct = {0};
			wchar_t* stringArray = new wchar_t[MAX_PATH]{0};

			resStruct.lStructSize = sizeof(OPENFILENAMEW);
			resStruct.lpstrFile = stringArray;
			resStruct.nMaxFile = MAX_PATH;
			resStruct.lpstrFilter = filterText.data();
			resStruct.nFilterIndex = 1;
			resStruct.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			resStruct.lpstrInitialDir = initDir;

			bool result = GetOpenFileNameW(&resStruct);
			
			std::wstring finalString = stringArray;
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
			return finalString;
		}
		else if(type==TYPE_SAVE_FILE)
		{
			OPENFILENAMEW resStruct = {0};
			wchar_t* stringArray = new wchar_t[MAX_PATH]{0};

			resStruct.lStructSize = sizeof(OPENFILENAMEW);
			resStruct.lpstrFile = stringArray;
			resStruct.nMaxFile = MAX_PATH;
			resStruct.lpstrFilter = filterText.data();
			resStruct.nFilterIndex = 1;
			resStruct.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
			resStruct.lpstrInitialDir = initDir;

			bool result = GetSaveFileNameW(&resStruct);
			
			std::wstring finalString = stringArray;
			delete[] stringArray;

			return finalString;
		}

		return L"";
	}

	
	int System::messageBoxPopup(unsigned int type, std::wstring title, std::wstring message)
	{
		return MessageBoxW(NULL, message.c_str(), title.c_str(), type);
	}

	void System::copyToClipboard(std::string str)
	{
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
	}

	void System::copyToClipboard(std::wstring str)
	{
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
	}

	std::wstring System::pasteFromClipboard()
	{
		if(!OpenClipboard(NULL))
			return L"";

		HANDLE clipboardData = GetClipboardData(CF_UNICODETEXT);
		wchar_t* textPointer = (wchar_t*)GlobalLock(clipboardData);

		std::wstring finalText = textPointer;

		GlobalUnlock(clipboardData);
		CloseClipboard();

		return finalText;
	}
	
} //NAMESPACE glib END