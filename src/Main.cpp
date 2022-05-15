#include "StringTools.h"
#include "System.h"
#include "SimpleWindow.h"
#include "SimpleDir.h"
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

void testGradients()
{
    Image img = Image(512, 512);
    Color c1 = {0, 0, 0, 0};
    Color c2 = {255, 255, 255, 255};
    
    SimpleGraphics::drawLinearGradient(c1, c2, Vec2f(0, 0), Vec2f(512, 512), &img);
    img.savePNG("Test1.png");


    Image img2 = Image(512, 512);
    std::vector<Color> colors;
    colors.push_back( {0, 0, 0, 255} );
    colors.push_back( {0, 0, 255, 255} );
    colors.push_back( {0, 255, 0, 255} );
    colors.push_back( {255, 0, 0, 255} );
    
    // std::vector<Vec2f> points;
    // points.push_back( Vec2f(0, 0) );

    
    // SimpleGraphics::drawLinearGradient(c1, c2, Vec2f(0, 0), Vec2f(512, 512), &img2);
    // img.savePNG("Test2.png");
    

    Image img3 = Image(512, 512);
    c1 = {0, 0, 0, 255};
    c2 = {255, 0, 0, 255};
    
    SimpleGraphics::drawRadialGradient(c1, c2, Vec2f(256, 256), 256, &img3);
    img3.savePNG("Test3.png");
    
}

void testDir()
{
    SimpleDir d = SimpleDir("./");
    std::vector<std::wstring> allFiles = d.getFolders();

    for(std::wstring& str : allFiles)
    {
        StringTools::println(str);
    }
}

void initFunction(SimpleWindow* w)
{
    w->getGuiManager()->loadElementsFromFile("GuiStuff/layout.xml");
}

int testWindow()
{
    StringTools::init();
    SimpleGraphics::init();
    GuiManager::initDefaultLoadFunctions();

    WindowOptions options;
    options.threadManaged = false;
    options.initFunction = initFunction;

    SimpleWindow w = SimpleWindow("TITLE", 1280, 720, -1, -1, options);
    w.waitTillClose();
    return 0;
}

int main(int argc, char** argv)
{
    // byteSwapv1();
    // byteSwapv2();
    // testGradients();
    // testDir();
    testWindow();
    
    // StringTools::println("%x", StringTools::byteSwap(0x11223344));
    // testLinuxStuff();
    return 0;
}
