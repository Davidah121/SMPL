#include "ext/ScreenCapturer.h"

#ifdef __WIN32
namespace smpl
{
    BitBltCapturer::BitBltCapturer()
    {
        desktopWindow = GetDesktopWindow();
        desktopDC = GetDC(desktopWindow);
        myCompatibleDC = CreateCompatibleDC(desktopDC);

        findCaptureSize();
        recreatePixelBuffer();
    }
    BitBltCapturer::~BitBltCapturer()
    {
        if(pixelStorage != nullptr)
            delete[] pixelStorage;
        DeleteDC(myCompatibleDC);
        DeleteObject(temporaryBitmap);
    }

    bool BitBltCapturer::findCaptureSize()
    {
        RECT currentWindowRect;
        GetWindowRect(desktopWindow, &currentWindowRect);
        int newWindowWidth = currentWindowRect.right - currentWindowRect.left;
        int newWindowHeight = currentWindowRect.bottom - currentWindowRect.top;
        if(newWindowHeight != lastWindowHeight || newWindowWidth != lastWindowWidth)
        {
            lastWindowHeight = newWindowHeight;
            lastWindowWidth = newWindowWidth;
            return true;
        }
        return false;
    }

    void BitBltCapturer::recreatePixelBuffer()
    {
        paddingNeeded = (lastWindowWidth*3) % sizeof(DWORD);
        if(paddingNeeded != 0)
            paddingNeeded = sizeof(DWORD)-paddingNeeded;
        int windowWidthAndPadding = lastWindowWidth+paddingNeeded;

        pixelStorage = new unsigned char[lastWindowHeight*windowWidthAndPadding*3];

        temporaryBitmap = CreateCompatibleBitmap(desktopDC, lastWindowWidth, lastWindowHeight);
    }

    smpl::Image* BitBltCapturer::capture()
    {
        if(findCaptureSize())
        {
            recreatePixelBuffer();
        }

        SelectObject(myCompatibleDC, temporaryBitmap);
        BitBlt(myCompatibleDC, 0, 0, lastWindowWidth, lastWindowHeight, desktopDC, 0, 0, SRCCOPY);

        BITMAPINFO bmi;
        ZeroMemory(&bmi, sizeof(BITMAPINFO));
        bmi.bmiHeader.biBitCount = 24;
        bmi.bmiHeader.biHeight = -lastWindowHeight;
        bmi.bmiHeader.biWidth = lastWindowWidth;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biSizeImage = 0;
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biCompression = BI_RGB;
        GetDIBits(desktopDC, temporaryBitmap, 0, lastWindowHeight, pixelStorage, &bmi, DIB_RGB_COLORS);

        smpl::Image* finalImage = new smpl::Image(lastWindowWidth, lastWindowHeight);
        smpl::Color* finalImagePixels = finalImage->getPixels();
        unsigned char* startingPoint = pixelStorage;
        for(int y=0; y<lastWindowHeight; y++)
        {
            for(int x=0; x<lastWindowWidth; x++)
            {
                //comes in BGR format instead of rgb
                smpl::Color c = {*(startingPoint+2), *(startingPoint+1), *startingPoint, 255};
                *finalImagePixels = c;

                finalImagePixels++;
                startingPoint+=3;
            }
            pixelStorage += paddingNeeded;
        }
        return finalImage;
    }
}
#endif