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
#include "VectorFont.h"

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

void testImageLoader()
{
    StringTools::init();
    //StringTools::println("Enter the png file to load.");
    //std::string filename = StringTools::getString();

    std::string filename = "C:/Users/Alan/source/repos/ImageLibrary/TestImages/PNG/FilterTest/f99n0c04.png";
    StringTools::print("Loading ");
    StringTools::println(filename);
    
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
    
    kImgs[0]->saveBMP("test1.bmp");
    for(int i=0; i<amount; i++)
    {
        delete kImgs[i];
    }
    delete[] kImgs;
}

VectorGraphic shape = VectorGraphic();

void svgTest()
{
    if(windowPointer!=nullptr)
    {
        Graphics::setColor({255,255,255,255});
        shape.getImageBuffer()->clearImage();

        shape.draw();

        Graphics::setColor({0,0,0,255});
        shape.getImageBuffer()->drawCircle(25, 24, 2, false);

        windowPointer->drawImage(shape.getImageBuffer());
    }
}

void drawLoadedSvg(std::string file)
{
    StringTools::init();
    shape.load(file);
    shape.setTransform(MathExt::rotation2D( MathExt::toRad(90.0), 51.0/2, 48.0/2) );
    Mat3f viewBoxMat = shape.getTransform();

    for(int i=0; i<3; i++)
    {
        for(int i2=0; i2<3; i2++)
        {
            StringTools::out << viewBoxMat[i][i2] << ", ";
        }
        StringTools::out << StringTools::lineBreak;
    }

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

void testXML(std::string filename)
{
    SimpleXml x = SimpleXml();
    x.load(filename);

    StringTools::println(x.nodes[0]->title);
    StringTools::println(x.nodes[0]->value);
}

void testFontSVG(std::string filename)
{
    VectorFont ft = VectorFont();
    ft.load(filename);

    for(FontCharInfo fci : ft.getListOfFontCharInfo())
    {
        StringTools::out << fci.x << ", " << fci.y << ", ";
        StringTools::out << fci.width << ", " << fci.height << ", ";
        StringTools::out << fci.horizAdv << ", " << fci.unicodeValue << StringTools::lineBreak;
    }

    VectorGraphic* g = ft.getVectorSprite()->getGraphic(2);
    
    shape = *g;

    shape.setTransform( MathExt::translation2D(500, 500) * MathExt::scale2D(0.25, 0.25) );

    windowPointer = new WndWindow("two", g->getWidth(), g->getHeight());
    windowPointer->setPaintFunction(svgTest);
    windowPointer->repaint();

    while(windowPointer->getRunning())
    {
    }
    
}


void testMatrixStuff()
{
    Mat3f q = MathExt::scale2D(2, 3);
    Mat3f p = MathExt::translation2D(32, 32);
    
    Mat3f x = q*p;

    for(int i=0; i<q.getRows(); i++)
    {
        for(int j=0; j<q.getCols(); j++)
        {
            StringTools::out << q[i][j] << ", ";
        }
        StringTools::println("");
    }

    for(int i=0; i<p.getRows(); i++)
    {
        for(int j=0; j<p.getCols(); j++)
        {
            StringTools::out << p[i][j] << ", ";
        }
        StringTools::println("");
    }

    for(int i=0; i<x.getRows(); i++)
    {
        for(int j=0; j<x.getCols(); j++)
        {
            StringTools::out << x[i][j] << ", ";
        }
        StringTools::println("");
    }
}

void paintFunction()
{
    if(windowPointer!=nullptr)
    {
        Graphics::setColor({255,204,0,255});
        Image background = Image(img->getWidth(), img->getHeight());
        background.clearImage();
        background.drawSprite(img, 0, 0);

        windowPointer->drawImage(&background);
    }
}
void testImageDisplay()
{
    StringTools::out << "Enter image name: ";
    std::string filename = StringTools::getString();

    int amountOfImages = 0;
    Image** imgArr = Image::loadImage(filename, &amountOfImages);

    if(amountOfImages==0 || imgArr == nullptr)
    {
        StringTools::out << filename.c_str() << " was not able to be loaded." << StringTools::lineBreak;
        return;
    }
    img = imgArr[0];

    windowPointer = new WndWindow("Image Tester", 320,240);
//C:\Users\Alan\Pictures\Screenshots\Screenshot (195).gif
//C:\Users\Alan\source\repos\ImageLibrary\TestImages\PNG\Varying bit sizes and types
//C:\Users\Alan\source\repos\ImageLibrary\TestImages\PNG\Interlacing


    windowPointer->setPaintFunction( paintFunction );

    while(windowPointer->getRunning())
    {
        windowPointer->repaint();
        System::sleep(16,666);
    }

    delete windowPointer;
    delete[] imgArr;
}

void testLZW()
{
    //string A B A A B A C B C C B A B C
    std::vector<unsigned char> data = Compression::decompressLZW({0x43, 0x50, 0x90, 0x10, 0x2D, 0x21}, 3);

    for(unsigned char c : data)
    {
        StringTools::println( c );
    }

    //string A B A A B A C B C C B A B C
    std::string text = "ABAABACBCCBABC";
    std::vector<unsigned char> textConv = std::vector<unsigned char>();
    for(char c : text)
    {
        textConv.push_back((unsigned char) c);
    }
    std::vector<unsigned char> data2 = Compression::compressLZW(textConv);

    for(unsigned char c : data2)
    {
        StringTools::println( (int)c );
    }
}

int main(int argc, char** argv)
{
    
    StringTools::init();
    //testImageLoader();
    //testFontSVG("C:\\Users\\Alan\\Documents\\VSCodeProjects\\GLib\\SVGFonts\\My Font - SVG Font - 2020.8.12-21.40.21.svg");
    
    //testLZW();
    testImageDisplay();
    
    //testXML("C:/Users/Alan/Documents/VSCodeProjects/GLib/testFiles/XmlFiles/test2.xml");
    //drawLoadedSvg("C:/Users/Alan/Documents/VSCodeProjects/GLib/SVGs/_ionicons_svg_md-mail.svg");
    //testXML("C:/Users/Alan/Documents/VSCodeProjects/GLib/SVGs/_ionicons_svg_md-mail.svg");
    //drawLoadedSvg("C:/Users/Alan/Documents/VSCodeProjects/GLib/testFiles/SvgFiles/Five Pointed Star.svg");
    //drawLoadedSvg("testFiles/SvgFiles/transforms.svg");
    //testMatrixStuff();

    system("pause");
    return 0;
}
