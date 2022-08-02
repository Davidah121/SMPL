#include "StringTools.h"
#include "System.h"
#include "SimpleWindow.h"
#include "SimpleDir.h"
#include "SpatialHashing.h"
#include "IniFile.h"
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

    SimpleWindow w = SimpleWindow("Windows Ver", 640, 480, -1, -1, {0, true, true, 0, false, "", initFunction});
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
    SimpleDir d = SimpleDir("C:");
    int index = d.getReferenceLocation("Stuff");
    size_t size = d.getReferenceSize(index);

    StringTools::println("Size of C:/Stuff: %llu", size);

    
    d = SimpleDir("C:/Users/Alan/Documents/VSCodeProjects");
    index = d.getReferenceLocation("GLib");
    size = d.getReferenceSize(index);

    StringTools::println("Size of C:/Users/Alan/Documents/VSCodeProjects/GLib: %llu", size);
}

void paintFunc(int width, int height)
{
    Image temp = Image(width, height);
    temp.setAllPixels({255,255,255,255});

    SimpleGraphics::resetClippingPlane();
    SimpleGraphics::setColor({0,0,0,255});
    SimpleGraphics::drawRect(32, 32, 64, 64, false, &temp);

    SimpleGraphics::setClippingRect(Box2D(96, 32, 128, 64));
    SimpleGraphics::setColor({0,255,0,255});
    SimpleGraphics::drawRect(96, 32, 128, 64, false, &temp);

    SimpleGraphics::resetClippingPlane();
    SimpleGraphics::setColor({0,0,0,255});
    SimpleGraphics::drawText("-_\\|]/fqoij.vcjo!~@#$)(*_>,/", 160, 32, &temp);
    
    //fix png saving
    temp.savePNG("testClip.png");
}

void initFunction(SimpleWindow* w)
{
    // w->getGuiManager()->loadElementsFromFile("GuiStuff/layout.xml");
    w->setWindowAsInputFocus();
}

int testWindow()
{
    StringTools::init();
    SimpleGraphics::init();
    GuiManager::initDefaultLoadFunctions();

    WindowOptions options;
    options.threadManaged = false;
    options.iconFileString = "icon.ico";
    options.initFunction = initFunction;

    SimpleWindow w = SimpleWindow("TITLE", 1280, 720, -1, -1, options);
    // GuiContainer c1 = GuiContainer();
    // // c1.setClearColor(Color{255,255,255,255});
    // c1.setBaseX(32);
    // c1.setBaseY(32);

    // GuiList l1 = GuiList(8, 8);
    // l1.setElementSpacing(4);
    // l1.setOutlineColor(Color{0, 0, 255, 255});

    // GuiRectangleButton b1 = GuiRectangleButton(-12, 0, 24, 24);
    // l1.addChild(&b1);
    
    // GuiTextBox t1 = GuiTextBox(0, 0, 64, 24);
    // l1.addChild(&t1);

    // c1.addChild(&l1);

    // w.getGuiManager()->addElement(&c1);

    GuiCustomObject obj = GuiCustomObject();
    obj.setRenderFunction([](GuiGraphicsInterface* interface) -> void{
        SimpleGraphics::setAntiAliasing(true);
        interface->setColor(Color{0,0,0,255});
        interface->drawEllipse(128, 128, 32, 48, false);
        interface->drawEllipse(256, 256, 48, 32, false);

        interface->setColor(Color{0,0,255,255});
        interface->drawRect(128, 128, 128+1, 128+1, false);
        interface->drawRect(256, 256, 256+1, 256+1, false);
    });

    w.getGuiManager()->addElement(&obj);
    
    w.getGuiManager()->setExpectedSize(Vec2f(1280, 720));
    w.getGuiManager()->alwaysInvalidateImage(true);
    w.waitTillClose();
    return 0;
}

void testSpatialHashing()
{
    SpatialHashing<Vec2f> spatialGrid = SpatialHashing<Vec2f>(4, 4, 32 ,32);
    //grid 0,0
    spatialGrid.addObject(Vec2f(2,2), Point2D(2,2));
    spatialGrid.addObject(Vec2f(1,2), Point2D(1,2));
    //grid 2,0
    spatialGrid.addObject(Vec2f(20,1), Point2D(20,1));
    spatialGrid.addObject(Vec2f(23,6), Point2D(23,6));
    //grid 0,3
    spatialGrid.addObject(Vec2f(5,31), Point2D(5,31));
    spatialGrid.addObject(Vec2f(2,28), Point2D(2,28));
    //grid 1,2
    spatialGrid.addObject(Vec2f(9,17), Point2D(9,17));
    spatialGrid.addObject(Vec2f(12,20), Point2D(12,20));

    //Grid 0,0 && Grid 1,0
    spatialGrid.addObject(Vec2f(8,0), Point2D(8,0));
    
    //Grid 0,0 && Grid 0,1
    spatialGrid.addObject(Vec2f(0,8), Point2D(0,8));
    
    //Grid 0,0 && Grid 0,1 && Grid 1,0 && Grid 1,1
    spatialGrid.addObject(Vec2f(8,8), Point2D(8,8));

    for(int j=0; j<4; j++)
    {
        for(int i=0; i<4; i++)
        {
            auto objs = spatialGrid.getObjects(i, j);
            StringTools::println("Grid (%d, %d)", i, j);
            for(SpatialHashingObject<Vec2f>& o : objs)
            {
                StringTools::println("\t(%.3f, %.3f)", o.getData().x, o.getData().y);
            }
        }
    }

    
}

void testINI(bool readTest)
{
    if(readTest)
    {
        IniFile f = IniFile();
        bool valid = f.load("test.ini");

        if(valid)
        {
            std::vector<std::string> sections = f.getSections();
            for(std::string& sStr : sections)
            {
                std::vector<std::string> keys = f.getKeys(sStr);

                StringTools::println("SECTION: %s", sStr.c_str());
                for(std::string& kStr : keys)
                {
                    std::string vStr = f.readValue(sStr, kStr);
                    StringTools::println("\t%s = %s", kStr.c_str(), vStr.c_str());
                }
            }
        }
        else
        {
            StringTools::println("ERROR LOADING");
        }

    }
    else
    {
        IniFile f = IniFile();
        f.writeValue("SectionTest", "Key1", "Value1");
        f.writeValue("SectionTest", "Key2", "Value2");
        f.writeValue("SectionTest", "Key3", "Value3");
        f.writeValue("SectionTest", "Key4", "Value4");
        f.writeValue("SectionTest", "Key1", "Value6");

        f.save("test.ini");
    }
}

void testDir2()
{
    SimpleDir d = SimpleDir("./");
    for(int i=0; i<d.getSize(); i++)
    {
        StringTools::println("%s", d.getReferenceName(i).c_str());
    }
    File temp = "test.txt";
    StringTools::println(temp.getFileNameWithExt());
}

// int WinMain(HINSTANCE hins, HINSTANCE preIns, LPSTR cmdline, int nShowCMD)
int main()
{
    // Sleep(1000);
    // testWindow();
    testDir2();
    // testSpatialHashing();
    return 0;
}
