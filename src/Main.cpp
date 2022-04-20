#include "StringTools.h"
#include "System.h"
#include "SimpleWindow.h"
#include <stdlib.h>

using namespace glib;

void testLinuxStuff()
{
    StringTools::init();
    SimpleGraphics::init();
    GuiManager::initDefaultLoadFunctions();
    
    std::function<void(SimpleWindow*)> initFunction = [](SimpleWindow* windowHandle) ->void {
        windowHandle->getGuiManager()->loadElementsFromFile("Test.xml");
    };

    SimpleWindow w = SimpleWindow("Windows Ver", 640, 480, -1, -1, {0, true, true, 0, initFunction});
    w.waitTillClose();
}

void byteSwapv1()
{
    Image img = Image(640, 480);
    unsigned char* wndPixels = new unsigned char[640*480*3];
    int* imgPixels = (int*)img.getPixels();
    
    unsigned char* start = wndPixels;
    unsigned char* end = wndPixels + (640*480*3);
    int padding = 0;
    int tX = 0;
    
    size_t t1,t2;
    size_t totalT;
    for(int i=0; i<100; i++)
    {
        t1 = System::getCurrentTimeNano();
        while(start<end)
        {
            int v = StringTools::byteSwap(*imgPixels) << 8;
            *((int*)start) = v;
            start += 3;
            imgPixels++;
            tX++;

            if(tX >= img.getWidth())
            {
                tX = 0;
                start += padding;
            }
        }
        t2 = System::getCurrentTimeNano();
            totalT += t2-t1;
    }
    StringTools::println("AVG Time Taken: %llu", totalT/100);
    delete[] wndPixels;
}

void byteSwapv2()
{
    Image img = Image(640, 480);
    int padding = 0;
    unsigned char* wndPixels = new unsigned char[640*480*3]; //Last 3 bytes are padding.
    Color* imgPixels = (Color*)img.getPixels();
    
    unsigned char* start = wndPixels;
    unsigned char* end = wndPixels + (640*480*3);
    int tX = 0;
    
    size_t t1,t2;
    size_t totalT;
    for(int i=0; i<100; i++)
    {
        t1 = System::getCurrentTimeNano();
        while(start<end)
        {
            start[0] = imgPixels->blue;
            start[1] = imgPixels->green;
            start[2] = imgPixels->red;
            
            start += 3;
            imgPixels++;
            tX++;

            if(tX >= img.getWidth())
            {
                tX = 0;
                start += padding;
            }
        }
        t2 = System::getCurrentTimeNano();
        totalT += t2-t1;
    }
    StringTools::println("AVG Time Taken: %llu", totalT/100);
    delete[] wndPixels;
}

int main(int argc, char** argv)
{
    byteSwapv1();
    byteSwapv2();
    

    // StringTools::println("%x", StringTools::byteSwap(0x11223344));
    // testLinuxStuff();
    return 0;
}
