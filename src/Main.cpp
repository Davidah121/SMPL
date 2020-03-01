#include "WndWindow.h"
#include "Graphics.h"
#include "System.h"
#include <iostream>
#include "Compression.h"
#include "BinarySet.h"
#include "SimpleFile.h"
#include "StringTools.h"

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
 * 
 *      Lastly
 *      Work on making the library portable
 */

WndWindow* windowPointer;

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
        Image img = Image(320, 240);
        img.clearImage();
        Graphics::setColor({255,255,255,255});
        img.drawCircle(img.getWidth()/2, img.getHeight()/2, 32, false);

        windowPointer->drawImage(&img);
    }
}

void compressionTesting()
{
    //Testing deflate on png. Skipping 0x18 0x57
    
    //Values:
    // 63 00 82 FF 60 00 A2 20 1C A8 00 10 60 B2 FE FF 07 00 68 F3 26 DA 8B A3 F8 F8 00 00 00 00
    
    //Expected Output:
    // 00 00 00 00 FF FF FF FF FF FF 00 FF FF FF 00 00 00 FF FF FF 00 FF FF FF FF FF FF FF FF FF 00 FF FF FF FF FF FF FF FF FF 00 FF FF FF FF FF FF FF FF FF

    std::vector<unsigned char> output = Compression::decompressDeflate(new unsigned char[]{0x63, 0x00, 0x82, 0xFF, 0x60, 0x00, 0xA2, 0x20, 0x1C, 0xA8, 0x00, 0x10, 0x60, 0xB2, 0xFE, 0xFF, 0x07, 0x00, 0x68, 0xF3, 0x26, 0xDA, 0x8B, 0xA3, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00}, 30);

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

    WndWindow k = WndWindow("CupTexture", 512, 512);

    /*
    StringTools::println("Loading C:/Users/Alan/Documents/GitHub/Mia/CupTexture.png");
    
    std::string filename = "C:/Users/Alan/Documents/GitHub/Mia/CupTexture.png";
    int amount = 0;
    Image** kImgs = Image::loadImage(filename, &amount);
    */
    int amount = 1;
    Image** kImgs = new Image*[1];
    kImgs[0] = new Image(512,512);
    Graphics::setColor({200,100,150,255});
    kImgs[0]->drawRect(32,32,400,400,false);

    if(amount > 0)
    {
        StringTools::println("Successful Load.");
        StringTools::out << "Width is: " << kImgs[0]->getWidth() << ", Height is: " << kImgs[0]->getHeight() << StringTools::lineBreak;

        while(k.getRunning())
        {
            k.drawImage(kImgs[0]);
            k.repaint();
        }
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
}
int main()
{
    testDrawingImage();
    return 0;
}
