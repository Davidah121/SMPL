#include "WndWindow.h"
#include "Graphics.h"
#include "System.h"
#include <iostream>
#include "Compression.h"
#include "BinarySet.h"

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
 *          Currently, decompression on mode 1 works. Mode 0 and 2 needs work.
 *          Compression should work on both mode 0, 1, and 2. None implemented so far.
 *          After decompression is done, work on loading pngs
 *          Also, after all compression and decompression methods are done, work on saving images.
 */

/**
 * Todo List:
 *      Work on shape class by adding more collision methods.
 *      Work on GuiManager class by adding more gui objects and interactions
 *      Work on Graphics class by adding more functions
 *      Work on MathExt class by adding more functions. (DCT, etc.)
 *      Work on VectorGraphic class by actually finishing it.
 *      Work on BezierCurve class by actually finishing it.
 *      Work on WavAudio so that it plays audio correctly.
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

int main()
{
    compressionTesting();
    return 0;
}