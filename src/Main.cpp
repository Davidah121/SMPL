#include "WndWindow.h"
#include "Graphics.h"
#include "System.h"
#include <iostream>
#include "Compression.h"
#include "BinarySet.h"
#include "SimpleFile.h"

#include "StringTools.h"
#include "Sort.h"
#include "VectorGraphic.h"

#include "SimpleXml.h"

/**
 * Purpose:
 *      Provide a port to other systems
 *      For now, only support Windows and Linux since I can test those.
 * 
 * What classes need to be changed with preprocessor directives:
 *      WndWindow - Name also needs to be changed, A lot of work
 *      System - Just a few utility functions
 *      Input - PollInput needs to change
 *      Networking - Most of it needs work
 *      GamepadXInput - May not be able to use other OS
 *      GamepadDInput - May not be able to use other OS
 *      WavAudio - Needs work, but not much has to change
 */

/**
 * Things to test that also need to be changed:
 *      GamepadDInput - Works, but look at the axis values more
 *      WndWindow - Mostly works except the gui and other drawing methods not yet done
 *      Input
 *      Networking
 *      System - Needs
 *      WavAudio - Needs lots of work before conversion
 */

/**
 * Things being worked on currently:
 *      Compression - Deflate decompression and compression.
 *          Test Deflate decompression. Multiple points of potential failure.
 *              Mostly on the dynamic part
 *          After decompression is done, work on loading pngs
 *          Also, after all compression and decompression methods are done, work on saving images.
 *      
 *      Image - Load PNG, JPEG, DDS
 *          Also be able to save to the different formats
 *          These must be implemented through this library to maintain cross platform use.
 *          Not relying on additional tools and only the c++ library is required and not much of the c++ library.
 *      
 *      Vector Graphics - Load vector graphics files
 *          Must be implemented in this library and not use external tools.
 * 
 *      WavAudio - Must load and play audio from .wav files and .ogg files
 *          .mp3 files are under patents and therefore can not be used for our purposes.
 *          Perhaps allowing them to be loaded for personal uses would be acceptable
 *          Name of the class should also be changed.
 *          Must play audio in real time.
 *          Must have audio objects.
 */

/**
 * Todo List:
 *      Work on shape class by adding more collision methods.
 *      Work on GuiManager class by adding more gui objects and interactions
 *      Work on Graphics class by adding more functions
 *      Work on MathExt class by adding more functions. (DCT, etc.)
 *      Work on VectorGraphic class by actually finishing it.
 *      Work on WavAudio so that it plays audio correctly.
 *      
 *      Work on Image class by supporting interlacing on png and gif files.
 *      Fix StringTools at some point. Especially the conversion between char* to wchar_t*
 *      Lastly
 *      Work on making the library portable
 */

/**
 * Current Objective:
 *      VectorGraphic
 * 
 */
WndWindow* windowPointer;
Image* img;

void function1()
{
    #ifdef WINDOWS
        std::cout << "I AM A WINDOWS" << std::endl;
    #elif LINUX
        std::cout << "I AM A LINUX" << std::endl;
    #elif MAC
        std::cout << "I AM A MAC" << std::endl;
    #endif

}

void function2()
{
    if(windowPointer!=nullptr)
    {
        Image img = Image(400, 300);
        img.clearImage();
        Graphics::setColor({255,255,255,255});
        img.drawCircle(img.getWidth()/2, img.getHeight()/2, 32, false);
        windowPointer->drawImage(&img);
    }
}

void function3()
{
    if(windowPointer!=nullptr)
    {
        Graphics::setColor({255,255,255,255});
        windowPointer->drawImage(img);
    }
}

void compressionTesting()
{
    //Testing deflate on png. Skipping 0x18 0x57
    
    //Values:
    // 63 00 82 FF 60 00 A2 20 1C A8 00 10 60 B2 FE FF 07 00 68 F3 26 DA 8B A3 F8 F8 00 00 00 00
    
    //Expected Output:
    // 00 00 00 00 FF FF FF FF FF FF 00 FF FF FF 00 00 00 FF FF FF 00 FF FF FF FF FF FF FF FF FF 00 FF FF FF FF FF FF FF FF FF 00 FF FF FF FF FF FF FF FF FF

    std::vector<unsigned char> output = Compression::decompressDeflate(new unsigned char[30]{0x63, 0x00, 0x82, 0xFF, 0x60, 0x00, 0xA2, 0x20, 0x1C, 0xA8, 0x00, 0x10, 0x60, 0xB2, 0xFE, 0xFF, 0x07, 0x00, 0x68, 0xF3, 0x26, 0xDA, 0x8B, 0xA3, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00}, 30);

    for(int i=0; i<output.size(); i++)
    {
        std::cout << (int)output[i] << std::endl;
    }
}

void compressionTesting2()
{
    SimpleFile f = SimpleFile("C:\\Users\\Alan\\source\\repos\\ImageLibrary\\TestImages\\PNG\\Varying bit sizes and types\\basn0g01.png", SimpleFile::READ);
    if(f.isOpen())
    {
        std::vector<unsigned char> fileData = f.readFullFileAsBytes();
        f.close();

        std::vector<unsigned char> output = Compression::decompressDeflate(&fileData[0x3B], 0x97 - 0x3B);

        
        std::cout << "Information: Size-" << output.size() << ", is 1 bit, is 32x32, 1024 values" << std::endl;
        
        for(int i=0; i<output.size(); i++)
        {
            std::cout << (int)output[i] << std::endl;
            
        }
        
    }
    else
    {
        std::cout << "No file" << std::endl;
    }
    
}

void thingTest()
{
    StringTools::init();
    StringTools::println("Loading C:/Users/Alan/Documents/GitHub/Mia/CupTexture.png");
    
    std::string filename = "C:/Users/Alan/Documents/GitHub/Mia/CupTexture.png";
    int amount = 0;
    Image** kImgs = Image::loadImage(filename, &amount);

    if(amount > 0)
    {
        StringTools::println("Successful Load.");
        StringTools::out << "Width is: " << kImgs[0]->getWidth() << ", Height is: " << kImgs[0]->getHeight() << StringTools::lineBreak;
    }
    else
    {
        StringTools::println("Unsuccessful Load.");
    }
    
    for(int i=0; i<amount; i++)
    {
        delete kImgs[i];
    }
    delete[] kImgs;
    
    system("pause");
}

void testImageLoader()
{
    StringTools::init();
    //StringTools::println("Enter the png file to load.");
    //std::string filename = StringTools::getString();

    StringTools::println("Loading C:/Users/Alan/Documents/GitHub/Mia/CupTexture.png");
    
    std::string filename = "C:/Users/Alan/Documents/GitHub/Mia/CupTexture.png";
    int amount = 0;
    Image** kImgs = Image::loadImage(filename, &amount);

    if(amount > 0)
    {
        StringTools::println("Successful Load.");
        StringTools::out << "Width is: " << kImgs[0]->getWidth() << ", Height is: " << kImgs[0]->getHeight() << StringTools::lineBreak;
    }
    else
    {
        StringTools::println("Unsuccessful Load. Closing");
        system("pause");
        exit(0);
    }

    while(true)
    {
        StringTools::println("Enter the x and y coordinates to check a pixel (use commas). Empty string to quit.");
        std::string input = StringTools::getString();

        if(input=="")
        {
            break;
        }
        else
        {
            std::vector<std::string> split = StringTools::splitString(input, ',');
            int x = std::stoi(split[0]);
            int y = std::stoi(split[1]);

            Color c = kImgs[0]->getPixel(x,y);

            StringTools::out << "The color at (" << x << ", " << y << ") is " << c.red << ", " << c.green << ", " << c.blue << ", " << c.alpha << StringTools::lineBreak;
        }
    }
    
    for(int i=0; i<amount; i++)
    {
        delete kImgs[i];
    }
    delete[] kImgs;
}

void testDrawingImage()
{
    StringTools::init();

    int amount = 0;
    img = Image::loadImage("C:/Users/Alan/Documents/GitHub/Mia/CupTexture.png", &amount)[0];
	//img = Image::loadImage("C:\\Users\\Alan\\source\\repos\\ImageLibrary\\TestImages\\PNG\\FilterTest\\f00n2c08.png", &amount)[0];
    
    windowPointer = new WndWindow(L"Displaying CupTexture", 600, 600);
    windowPointer->setPaintFunction(function3);

    while(windowPointer->getRunning())
    {
        windowPointer->repaint();
        System::sleep(16, 666);
    }
    
    if(windowPointer!=nullptr)
        delete windowPointer;
    

    if(img!=nullptr)
        delete img;

}

void testSort()
{
    int* list = new int[8]{10,80,30,90,40,50,70,20};
    std::function compare = [](int a,int b) -> bool { return a < b; };

    Sort::quickSort<int>(list, 8, compare);

    for(int i=0; i<8; i++)
    {
        std::cout << list[i] << std::endl;
    }
}

void tPaint()
{
    if(windowPointer!=nullptr)
    {
        StringTools::println("AP");
        Graphics::setColor({255,255,255,255});
        VectorGraphic m = VectorGraphic(320, 240);
        m.getImageBuffer()->clearImage();
        /*
        VectorRectangle r = VectorRectangle();
        r.setX(32);
        r.setY(32);
        r.setWidth(128);
        r.setHeight(128);
        r.setRX(8);
        r.setRY(8);
        r.setFillColor({255,0,0,255});
        r.setStrokeColor({0,0,0,255});
        r.setStrokeWidth(1);

        VectorRectangle r2 = VectorRectangle();
        r2.setX(32);
        r2.setY(32);
        r2.setWidth(128);
        r2.setHeight(128);
        r2.setFillColor({0,0,0,0});
        r2.setStrokeColor({0,0,0,255});
        r2.setStrokeWidth(1);
        */

        /*
        //Start with polygons
        VectorPolygon r = VectorPolygon();
        r.addPoint(50, 5);
        r.addPoint(21, 95);
        r.addPoint(98, 40);
        r.addPoint(2, 40);
        r.addPoint(79, 95);
        r.setFillColor({255,0,0,255});
        r.setStrokeWidth(1);
        r.setStrokeColor({0,0,0,255});
        r.setFillMethod(VectorShape::EVEN_ODD_RULE);

        //Rectangle with Paths
        VectorPath p1 = VectorPath();
        StringTools::println("C1");
        p1.addMoveTo(10,10);
        StringTools::println("C2");
        p1.addHorizontalToRel(80);
        StringTools::println("C3");
        p1.addVerticalToRel(80);
        StringTools::println("C4");
        p1.addHorizontalToRel(-80);
        StringTools::println("C5");
        p1.addClosePath();
        StringTools::println("Done");
        p1.setFillColor({0,0,0,255});
        */
        
        //Quadratic test with Paths
        VectorPath p1 = VectorPath();
        StringTools::println("C1");
        p1.addMoveTo(10,80);
        StringTools::println("C2");
        p1.addCubicTo(40,10, 65,10, 95,80);
        StringTools::println("C3");
        p1.addCubicToShort(150,150, 180,80);

        p1.setFillColor({0,0,0,255});
        p1.setFillMethod(VectorShape::NON_ZERO);

        VectorPolyline p2 = VectorPolyline();
        p2.addPoint(10,80);
        p2.addPoint(40,10);
        p2.addPoint(65,10);
        p2.addPoint(95,80);
        p2.addPoint(125,150);
        p2.addPoint(150,150);
        p2.addPoint(180,80);

        p2.setFillColor({0,0,0,0});
        p2.setStrokeWidth(1);
        p2.setStrokeColor({255,0,0,255});

        /*
        VectorPolygon r2 = VectorPolygon();
        r2.addPoint(150, 0);
        r2.addPoint(121, 90);
        r2.addPoint(198, 35);
        r2.addPoint(102, 35);
        r2.addPoint(179, 90);
        r2.setFillColor({0,0,0,255});
        r2.setStrokeWidth(2);
        r2.setStrokeColor({255,0,0,255});
        r2.setFillMethod(VectorShape::EVEN_ODD_RULE);
        */
        /** 
         * Looks like this in svg 
         * 
         * <svg width="320" height="240">
         *      <rect x="32" y="32" rx="8" ry="8" width="128" height="128" fill="rgb(255,0,0)" fill-opacity="1" stroke="black" stroke-width="1"/>
         * </svg>
         */
        
        m.addShape(&p1);
        m.addShape(&p2);
        //m.addShape(&r);
        //m.addShape(&r2);
        //m.addShape(&r3);

        m.draw();

        Image* img = m.getImageBuffer();
        
        Graphics::setColor({0,255,0,255});
        img->drawLine(0,0,320,0);

        Graphics::setColor({255,0,0,255});
        //img->drawRect(100,10,180,90,false);
        /*
        img->drawPixel(10, 10, {255,0,0,255});
        img->drawPixel(10, 90, {255,0,0,255});
        img->drawPixel(90, 90, {255,0,0,255});
        img->drawPixel(90, 10, {255,0,0,255});
        */
        windowPointer->drawImage(img);
    }
}

void polygonTesting()
{
    if(windowPointer!=nullptr)
    {
        Image img = Image(640, 480);
        img.clearImage();
        
        Graphics::setColor({255,255,255,255});
        img.drawRect(0,0,320,240,false);

        Graphics::setColor({255,0,0,255});
        
        Graphics::setFillRule( Graphics::FILL_EVEN_ODD );
        
        Vec2f* points = new Vec2f[4];
        /*
        points[0] = Vec2f(30, 34);
        points[1] = Vec2f(95, 99);
        points[2] = Vec2f(99, 95);
        points[3] = Vec2f(34, 30);
        */

        points[0] = Vec2f(9.10557, 10.44721);
        points[1] = Vec2f(49.10557, 60.44721);
        points[2] = Vec2f(50.89442, 59.55278);
        points[3] = Vec2f(10.89442, 9.55278);
        
        img.drawPolygon(points, 4);
        

        windowPointer->drawImage(&img);
    }
}

void fullSVGNoArc()
{
    if(windowPointer!=nullptr)
    {
        VectorGraphic g = VectorGraphic(512,512);
        Graphics::setColor({255,255,255,255});
        g.getImageBuffer()->clearImage();
        
        VectorPath p = VectorPath();
        p.addMoveTo(256, 32);
        p.addCubicToRel(-88.004,0, -160,70.557, -160,156.801);
        p.addCubicTo(96,306.4, 256,480, 256,480);
        p.addCubicToShortRel(160,-173.6, 160,-291.199);
        p.addCubicTo(416,102.557, 344.004,32, 256,32);
        p.addClosePath();
        
        p.addMoveToRel(0,212.801);
        p.addCubicToRel(-31.996,0, -57.144,-24.645, -57.144,-56);
        p.addCubicToRel(0,-31.357, 25.147,-56, 57.144,-56);
        p.addCubicToShortRel(57.144,24.643, 57.144,56);
        p.addCubicToRel(0,31.355, -25.148,56, -57.144,56);
        p.addClosePath();
        

        p.setFillColor({0,0,0,255});

        g.addShape(&p);

        g.draw();

        windowPointer->drawImage(g.getImageBuffer());
    }
}

void horizontalPathStuff()
{
    if(windowPointer!=nullptr)
    {
        VectorGraphic g = VectorGraphic(512,512);
        Graphics::setColor({255,255,255,255});
        g.getImageBuffer()->clearImage();
        
        VectorPath p = VectorPath();

        p.addMoveTo(32,32);
        p.addLineTo(64,32);
        p.addLineTo(64,64);
        p.addLineTo(32,64);
        p.addClosePath();

        p.setFillColor({0,0,0,255});
        p.setStrokeColor({255,0,0,255});
        p.setStrokeWidth(2);

        g.addShape(&p);

        g.draw();

        Graphics::setColor({0,255,0,255});
        g.getImageBuffer()->drawLine(32,32,64,32);
        g.getImageBuffer()->drawLine(32,32,32,64);
        g.getImageBuffer()->drawLine(32,64,64,64);
        g.getImageBuffer()->drawLine(64,32,64,64);


        windowPointer->drawImage(g.getImageBuffer());
    }
}

void strokePathTest()
{
    if(windowPointer!=nullptr)
    {
        VectorGraphic g = VectorGraphic(512,512);
        Graphics::setColor({255,255,255,255});
        g.getImageBuffer()->clearImage();
        
        VectorPath p = VectorPath();

        p.addMoveTo(10,30);
        p.addLineTo(10,60);
        p.addLineToRel(30,0);
        p.addClosePath();
        //p.addQuadTo(30,10,50,30);

        p.setFillColor({0,0,0,255});
        p.setStrokeColor({0,0,255,255});
        //p.setStrokeWidth(1);

        g.addShape(&p);

        g.draw();


        windowPointer->drawImage(g.getImageBuffer());
    }
}

void arcTest()
{
    if(windowPointer!=nullptr)
    {
        VectorGraphic g = VectorGraphic(325,325);
        Graphics::setColor({255,255,255,255});
        g.getImageBuffer()->clearImage();
        
        VectorPath p = VectorPath();
        p.addMoveTo(80,80);
        p.addArcTo(45,45,0,false,false,125,125);
        p.addLineTo(125,80);
        p.addClosePath();
        p.setFillColor({0,255,0,255});
        g.addShape(&p);
        
        VectorPath p2 = VectorPath();
        p2.addMoveTo(230,80);
        p2.addArcTo(45,45,0,true,false,275,125);
        p2.addLineTo(275,80);
        p2.addClosePath();
        p2.setFillColor({255,0,0,255});
        g.addShape(&p2);

        VectorPath p3 = VectorPath();
        p3.addMoveTo(80,230);
        p3.addArcTo(45,45,0,false,true,125,275);
        p3.addLineTo(125,230);
        p3.addClosePath();
        p3.setFillColor({255,0,255,255});
        g.addShape(&p3);
        
        VectorPath p4 = VectorPath();
        p4.addMoveTo(230,230);
        p4.addArcTo(45,45,0,true,true,275,275);
        p4.addLineTo(275,230);
        p4.addClosePath();
        p4.setFillColor({0,0,255,255});
        g.addShape(&p4);
        
        g.draw();

        windowPointer->drawImage(g.getImageBuffer());
    }
}

void arcTest2()
{
    if(windowPointer!=nullptr)
    {
        VectorGraphic g = VectorGraphic(320,320);
        Graphics::setColor({255,255,255,255});
        g.getImageBuffer()->clearImage();

        VectorPath p = VectorPath();
        p.addMoveTo(10,315);
        p.addLineTo(110, 215);
        p.addArcTo(30,50,0,false,true,162.55,162.45);
        p.addLineTo(172.55,152.45);
        p.addArcTo(30,50,-45,false,true,215.1,109.9);
        p.addLineTo(315,10);
        
        p.setFillColor({0,128,0,128});

        g.addShape(&p);

        g.draw();

        windowPointer->drawImage(g.getImageBuffer());
    }
}

void testRect()
{
    StringTools::init();

    windowPointer = new WndWindow(L"Displaying Path Arcs", 325, 325);
    windowPointer->setPaintFunction(arcTest2);

    windowPointer->repaint();

    while(windowPointer->getRunning())
    {
    }
    
    if(windowPointer!=nullptr)
        delete windowPointer;
    

    if(img!=nullptr)
        delete img;
}

void printNodeInfo(int indent, XmlNode* node)
{
    for(int i=0; i<indent; i++)
    {
        StringTools::print("\t");
    }

    StringTools::out << "<" << node->title.c_str();
    for(XmlAttribute a : node->attributes)
    {
        StringTools::out << " " << a.name.c_str() << "=\"" << a.value.c_str() << "\"";
    }

    if(node->value != "")
    {
        StringTools::out << ">" << node->value.c_str();
        StringTools::out << "</" << node->title.c_str() << ">" << StringTools::lineBreak;
        return;
    }
    else
    {
        if(node->childNodes.size()==0)
        {
            StringTools::out << "/>" << StringTools::lineBreak;
            return;
        }
        else
        {
            StringTools::out << ">" << node->value.c_str();
            StringTools::out << StringTools::lineBreak;
        }
    }
        
    
    for(XmlNode* otherNodes : node->childNodes)
    {
        printNodeInfo(indent+1, otherNodes);
    }

    for(int i=0; i<indent; i++)
    {
        StringTools::print("\t");
    }
    StringTools::out << "</" << node->title.c_str() << ">" << StringTools::lineBreak;
}

void printNodeInfoTree(int indent, XmlNode* n)
{
    for(int i=0; i<indent; i++)
    {
        StringTools::print("\t");
    }
    StringTools::out << n->title.c_str() << StringTools::lineBreak;

    for(XmlNode* node : n->childNodes)
    {
        printNodeInfoTree(indent+1, node);
    }
}

void testSimpleXml(std::string file)
{
    SimpleXml xml = SimpleXml();
    xml.load(file);
    
    for(XmlNode* node : xml.nodes)
    {
        printNodeInfo(0, node);
    }
}

void testXmlTreeStruct(std::string file)
{
    SimpleXml xml = SimpleXml();
    xml.load(file);
    
    for(XmlNode* node : xml.nodes)
    {
        printNodeInfoTree(0, node);
    }
}

VectorGraphic shape = VectorGraphic();

void svgTest()
{
    if(windowPointer!=nullptr)
    {
        Graphics::setColor({255,255,255,255});
        shape.getImageBuffer()->clearImage();

        shape.draw();

        windowPointer->drawImage(shape.getImageBuffer());
    }
}

void drawLoadedSvg(std::string file)
{
    StringTools::init();
    shape.load(file);

    windowPointer = new WndWindow(L"Displaying Path Arcs", shape.getWidth(), shape.getHeight());
    
    windowPointer->setPaintFunction(svgTest);

    windowPointer->repaint();

    while(windowPointer->getRunning())
    {
    }
    
    if(windowPointer!=nullptr)
        delete windowPointer;
    

    if(img!=nullptr)
        delete img;
}

int main(int argc, char** argv)
{
    StringTools::init();
    drawLoadedSvg("C:/Users/Alan/Documents/VSCodeProjects/GLib/SVGs/_ionicons_svg_md-mail.svg");
    system("pause");
    return 0;
}
