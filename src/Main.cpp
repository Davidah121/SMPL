#include "StringTools.h"
#include "System.h"
#include "SimpleWindow.h"
#include "SimpleDir.h"
#include "SpatialHashing.h"
#include "IniFile.h"
#include "VectorFont.h"
#include "Network.h"
#include <stdlib.h>
#include <atomic>

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
    w->getGuiManager()->loadElementsFromFile("GuiStuff/layout.xml");
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

    // GuiCustomObject obj = GuiCustomObject();
    // obj.setRenderFunction([](GuiGraphicsInterface* interface) -> void{
    //     SimpleGraphics::setAntiAliasing(true);
    //     interface->setColor(Color{0,0,0,255});
    //     interface->drawEllipse(128, 128, 32, 48, false);
    //     interface->drawEllipse(256, 256, 48, 32, false);

    //     interface->setColor(Color{0,0,255,255});
    //     interface->drawRect(128, 128, 128+1, 128+1, false);
    //     interface->drawRect(256, 256, 256+1, 256+1, false);
    // });
    
    // GuiTextBox obj = GuiTextBox( 32, 32, 256, 24 );
    // obj.getTextBlockElement()->setBaseX(4);
    // obj.getTextBlockElement()->setBaseY(4);
    // obj.getTextBlockElement()->setMaxWidth( 256-8 );
    
    // w.getGuiManager()->addElement(&obj);
    
    w.getGuiManager()->setExpectedSize(Vec2f(1280, 720));
    // w.getGuiManager()->alwaysInvalidateImage(true);
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

void testVectorGraphic()
{
    VectorGraphic g1 = VectorGraphic();
    // g1.load("testSVG.svg");
    // g1.load("testFiles/SvgFiles/Five Pointed Star.svg");
    g1.load("SVGs/_ionicons_svg_md-call-tan.svg");
    VectorGraphic g2 = VectorGraphic();
    g2.load("SVGs/_ionicons_svg_md-call.svg");
    VectorGraphic g3 = VectorGraphic();
    g3.load("SVGs/_ionicons_svg_md-mail.svg");
    VectorGraphic g4 = VectorGraphic();
    g4.load("SVGs/_ionicons_svg_md-pin.svg");
    // VectorGraphic g5 = VectorGraphic();
    // g4.load("SVGs/_ionicons_svg_md-pin.svg");
    
    Image buffer = Image(512, 512);
    buffer.setAllPixels({0,0,0,0});
    g1.draw(&buffer);
    buffer.savePNG("Test1.png");
    
    buffer.setAllPixels({0,0,0,0});
    g2.draw(&buffer);
    buffer.savePNG("Test2.png");
    
    buffer.setAllPixels({0,0,0,0});
    g3.draw(&buffer);
    buffer.savePNG("Test3.png");
    
    buffer.setAllPixels({0,0,0,0});
    g4.draw(&buffer);
    buffer.savePNG("Test4.png");

}

void testVectorFont()
{
    VectorFont f = VectorFont();
    f.load("SVGFonts/AnyConv.com__consolab.svg");
    StringTools::println("Loaded %llu characters", f.getListOfFontCharInfo().size());

    Image buffer = Image(f.getGraphic(0)->getWidth()/2, f.getGraphic(0)->getHeight()/2);
    
    for(int i='A'; i<='z'; i++)
    {
        int index = f.getCharIndex(i);
        if(index >= 0)
        {
            VectorGraphic* grap = f.getGraphic(index);
            if(grap != nullptr)
            {
                grap->setTransform(MathExt::scale2D(0.5, 0.5));
                SimpleGraphics::setColor({255,255,255,255});
                buffer.clearImage();
                grap->draw(&buffer);

                buffer.savePNG("TestOutput/image" + StringTools::toString(i) + ".png");
            }
        }
    }
}

void testFontDrawing()
{
    SimpleGraphics::init();
    glib::Font* bitmapFont = SimpleGraphics::getDefaultFont(0);

    FontCharInfo bfci = bitmapFont->getFontCharInfo( bitmapFont->getCharIndex('o') );
    StringTools::println("FontInfo: %d, %d, %d, %d, %d", bfci.width, bfci.height, bfci.horizAdv, bfci.xOffset, bfci.yOffset);
    
    Image img = Image(128, 128);
    SimpleGraphics::drawText("This is some text", 0, 0, &img);
    img.savePNG("TestImg.png");

    VectorFont vF = VectorFont();
    vF.load("SVGFonts/ARIBLK.svg");
    vF.setFontSize(12);
    
    // StringTools::println("VectorFontSize: %d", vF.getFontSize());
    Image img2 = Image(128, 128);
    SimpleGraphics::setFont(&vF);
    SimpleGraphics::drawText("This is some text", 0, 0, &img2);
    img2.savePNG("TestImg2.png");

    FontCharInfo fci = vF.getFontCharInfo( vF.getCharIndex('o') );
    double scaleV = (double)vF.getFontSize() / vF.getOriginalFontSize();
    StringTools::println("FontInfo2: %d, %d, %d, %d, %d", fci.width, fci.height, fci.horizAdv, fci.xOffset, fci.yOffset);
    StringTools::println("FontInfo3: %.3f, %.3f, %.3f, %.3f, %.3f", fci.width/scaleV, fci.height/scaleV, fci.horizAdv/scaleV, fci.xOffset/scaleV, fci.yOffset/scaleV);
    
}

void testPolygonStuff()
{
    Image img = Image(128, 128);

    std::vector<Vec2f> points = {Vec2f(64, 24), Vec2f(80, 80), Vec2f(32, 48), Vec2f(96, 48), Vec2f(48, 80)};
    SimpleGraphics::drawPolygon(points.data(), points.size(), &img);

    Vec2f midPoint;
    for(int i=0; i<points.size(); i++)
    {
        midPoint += points[i];
    }
    midPoint /= points.size();

    for(int i=0; i<points.size(); i++)
    {
        SimpleGraphics::setColor({255,0,0,255});
        SimpleGraphics::drawLine(points[i].x, points[i].y, midPoint.x, midPoint.y, &img);
    }

    img.savePNG("polything.png");
}

void testFileStuff()
{
    #ifdef _WIN32
        StringTools::println("Is windows 32bit or 64bit");
    #endif
    #ifdef __linux__
        StringTools::println("Is linux 32bit or 64bit");
    #endif
    #ifdef __APPLE__
        StringTools::println("Is mac os x 32bit or 64bit");
    #endif
}

void networkTest(bool type)
{
    Network k = Network(type, 4040, "192.168.0.246");
    std::atomic_bool connected = false;

    k.setOnConnectFunction([type, &k, &connected](int id) ->void{
        StringTools::println("Connected");
        connected = true;
        if(type == Network::TYPE_CLIENT)
        {
            std::string msg = "Hello";
            k.sendMessage(msg, id);
        }
    });
    k.setOnMessageArrivedFunction([&k](int id) ->void{
        std::string msg = "";
        k.receiveMessage(msg, id);
        StringTools::println("Received: %s", msg.c_str());
    });
    k.setOnDisconnectFunction([&k, &connected](int id) ->void{
        StringTools::println("Disconnected");
        connected = false;
        k.endNetwork();
    });

    k.startNetwork();

    while(k.getRunning() && !connected)
    {
        if(k.getTimeoutOccurred())
        {
            StringTools::println("Timeout has occured");
            break;
        }
        
        System::sleep(10);
    }

    while(k.getRunning())
    {
        if(StringTools::getString() == "end")
            break;
    }
}

void testOTFLoading()
{
    VectorFont f = VectorFont();
    f.load("arial.ttf");
}

void testINI()
{
    glib::IniFile f = glib::IniFile();
    bool loaded = f.load("config.ini");
    if(loaded)
    {
        int allowedConnections = glib::StringTools::toInt( f.readValue("Network", "AllowedConnections") );
    }
}

void testDatePicker()
{
    SimpleGraphics::init();

    WindowOptions options;
    options.threadManaged = false;

    SimpleWindow w = SimpleWindow("Title", 640, 480, -1, -1, options);

    GuiDatePicker testObject = GuiDatePicker(32, 32, 96, 24); //memory issue
    // GuiTextBox testObject = GuiTextBox(32, 32, 96, 24);
    w.getGuiManager()->addElement(&testObject);

    while(w.getRunning())
    {
        w.update();
        w.guiUpdate();
        w.repaint();

        System::sleep(10);
    }
}

// int WinMain(HINSTANCE hins, HINSTANCE preIns, LPSTR cmdline, int nShowCMD)
int main(int argc, char** argv)
{
    //testINI();
    // Sleep(1000);
    // std::string com = argv[1];
    // if(com == "client")
    //     networkTest(Network::TYPE_CLIENT);
    // else
    //     networkTest(Network::TYPE_SERVER);
    
    testWindow();
    // testVectorGraphic();
    // testVectorFont();
    // testSpatialHashing();
    // testFontDrawing();
    // testPolygonStuff();
    // testFileStuff();

    // testOTFLoading();

    // testDatePicker();

    return 0;
}
