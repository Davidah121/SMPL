#include "System.h"
#include "StringTools.h"
#include "MathExt.h"
#include <thread>
#include <Windows.h>
#include "SimpleFile.h"

std::chrono::high_resolution_clock::time_point System::startTime;
std::chrono::high_resolution_clock::time_point System::endTime;
unsigned int System::numberOfThreads = std::thread::hardware_concurrency();

System::System()
{
}


System::~System()
{
}

unsigned long System::getCurrentTimeMillis()
{
	auto t = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now());
	auto value = std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch());

	return (unsigned long)value.count();
}

unsigned long System::getCurrentTimeMicro()
{
	auto t = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now());
	auto value = std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch());

	return (unsigned long)value.count();
}

unsigned long System::getCurrentTimeNano()
{
	auto t = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now());
	auto value = std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch());

	return (unsigned long)value.count();
}

void System::sleep(int millis, int micros)
{
	unsigned long startTime = getCurrentTimeMicro();

	unsigned long finalTime = millis * 1000 + micros;
	while ((getCurrentTimeMicro() - startTime) < finalTime)
	{
		std::this_thread::yield();
	}
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

void System::startTimeMeasurement()
{
	startTime = std::chrono::high_resolution_clock::now();
}

unsigned long System::endTimeMeasurement()
{
	endTime = std::chrono::high_resolution_clock::now();

	auto t = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

	return (unsigned long)t.count();
}

unsigned int System::getNumberOfThreads()
{
	return numberOfThreads;
}

void System::dispose()
{
	
}

void System::emulateKeypress(int key)
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

void System::saveScreenShot(HWND hwnd, std::string filename)
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

	int size = bmi.bmiHeader.biHeight * bmi.bmiHeader.biWidth * 3;
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
	
	SimpleFile f = SimpleFile(filename, SimpleFile::WRITE);

	if (f.isOpen())
	{
		f.writeBytes((unsigned char*)& bmfh, sizeof(BITMAPFILEHEADER));
		f.writeBytes((unsigned char*)& bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
		f.writeBytes(pixels, size);
		f.close();
	}
}

void System::saveScreenShot(HWND hwnd, std::wstring filename)
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

	int size = bmi.bmiHeader.biHeight * bmi.bmiHeader.biWidth * 3;
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

	SimpleFile f = SimpleFile(filename, SimpleFile::WRITE);

	if (f.isOpen())
	{
		f.writeBytes((unsigned char*)& bmfh, sizeof(BITMAPFILEHEADER));
		f.writeBytes((unsigned char*)& bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
		f.writeBytes(pixels, size);
		f.close();
	}
}

Image* System::getScreenShot(HWND hwnd)
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

	int size = bmi.bmiHeader.biHeight * bmi.bmiHeader.biWidth * 3;
	unsigned char* pixels = new unsigned char[size];

	HDC capDC = CreateCompatibleDC(hdc);
	HGDIOBJ hOld = SelectObject(capDC, bitmap);
	BitBlt(capDC, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, hdc, 0, 0, SRCCOPY);
	SelectObject(capDC, hOld);
	DeleteDC(capDC);

	int returnVal = GetDIBits(hdc, bitmap, 0, getDesktopHeight(), pixels, &bmi, DIB_RGB_COLORS);

	Image* finalImage = new Image(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight);

	Color* startPixels = finalImage->getPixels();
	unsigned char* startP = pixels;
	unsigned char* endP = pixels + size;
	while(startP < endP)
	{
		(*startPixels).alpha = 255;

		(*startPixels).blue = *startP;
		startP++;
		(*startPixels).green = *startP;
		startP++;
		(*startPixels).red = *startP;
		startP++;

		startPixels++;
	}
			
	
	return finalImage;
}

void System::saveScreenShotDesktop(std::string filename)
{
	System::saveScreenShot(GetDesktopWindow(), filename);
}

void System::saveScreenShotDesktop(std::wstring filename)
{
	System::saveScreenShot(GetDesktopWindow(), filename);
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

	int size = bmi.bmiHeader.biHeight * bmi.bmiHeader.biWidth * 3;
	unsigned char* pixels = new unsigned char[size];
	unsigned char* startP = pixels;

	//copy image into pixels
	for(int y = 0; y<hei; y++)
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
	}

	SetDIBits(hdc, bitmap, 0, hei, &pixels[0], &bmi, DIB_RGB_COLORS);
	
	
	HGDIOBJ hOld = SelectObject(mem, bitmap);
	GetObject(bitmap, sizeof(BITMAP), &bimg);
	BitBlt(hdc, startX, startY, bimg.bmWidth, bimg.bmHeight, mem, 0, 0, SRCCOPY);
	DeleteDC(mem);
}

void System::paintImageToDesktop(Image* img, int startX, int startY)
{
	System::paintImageToWindow(GetDesktopWindow(), img, startX, startY);
}
