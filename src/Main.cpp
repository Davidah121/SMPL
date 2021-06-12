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

#include "LCG.h"
#include "NeuralNetwork.h"

#include "ColorSpaceConverter.h"
#include "Compression.h"

#include "Audio.h"
#include "Sound.h"

#include "Sprite.h"
#include "Input.h"

#include "BitmapFont.h"
#include "ColorNameConverter.h"

#include "SimpleHashMap.h"

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
 *      GamepadXInput - May not be able to use in other OS
 *      GamepadDInput - May not be able to use in other OS
 *      WavAudio - Needs work, but not much has to change
 */

/**
 * Things to test that also need to be changed:
 *      GamepadDInput - Works, but look at the axis values more
 */

/**
 * Things being worked on currently:
 *      Image - Load DDS
 *          Also be able to save to the different formats
 *          These must be implemented through this library to maintain cross platform use.
 *          Not relying on additional tools and only the c++ library is required and not much of the c++ library.
 *      
 *      Vector Graphics - Load vector graphics files
 *          Must be implemented in this library and not use external tools.
 *          Missing a few features such as stroke, transforms, different fills
 */

/**
 * Todo List:
 *      Work on shape class by adding more collision methods.
 *      Work on Graphics class by adding more functions
 *      Work on VectorGraphic class by actually finishing it.
 *      
 *      Update all sleep functions
 *      Work on MathExt to avoid deleting memory
 *      Work on JPEG loader for invalid images in test image folder
 *      Work on making the library portable
 */

WndWindow* windowPointer;
Image* imgPointer;
Image* img;
std::string globalString = "";

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

VectorGraphic shape = VectorGraphic();

void svgTest()
{
    if(windowPointer!=nullptr)
    {
        Graphics::setColor({255,255,255,255});
        img->clearImage();

        shape.draw(img);

        Graphics::setColor({0,0,0,255});

        windowPointer->drawImage(img);
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
            StringTools::print("%d, ",viewBoxMat[i][i2]);
        }
        StringTools::println("");
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

void testFontSVG(std::wstring filename)
{
    VectorFont ft = VectorFont();
    ft.load(filename);

    /*
    for(FontCharInfo fci : ft.getListOfFontCharInfo())
    {
        StringTools::out << fci.x << ", " << fci.y << ", ";
        StringTools::out << fci.width << ", " << fci.height << ", ";
        StringTools::out << fci.horizAdv << ", " << fci.unicodeValue << StringTools::lineBreak;
    }
    */
    
    
    VectorGraphic* g = ft.getVectorSprite()->getGraphic(ft.getCharIndex('A'));
    
    shape = *g;

    img = new Image(800, 800);
    shape.setTransform( MathExt::translation2D(300, 200) * MathExt::scale2D(0.25, 0.25) );

    windowPointer = new WndWindow("two", img->getWidth(), img->getHeight());
    windowPointer->setPaintFunction(svgTest);
    windowPointer->repaint();

    while(windowPointer->getRunning())
    {
    }
    
}

void paintFont()
{
    if(windowPointer!=nullptr)
    {
        Graphics::setColor({255,255,255,255});
        img->clearImage();
        Graphics::setColor({0,0,0,255});
        img->drawText(globalString, 0, img->getHeight()/2);

        windowPointer->drawImage(img);
    }
}

void testFontGraphics()
{
    BitmapFont font = BitmapFont("./testFiles/BitmapFontFiles/bmpFont.ft");
    Graphics::setFont(&font);
    
    img = new Image(640, 480);

    windowPointer = new WndWindow("font testing", img->getWidth(), img->getHeight());
    windowPointer->setPaintFunction(paintFont);
    windowPointer->repaint();

    while(windowPointer->getRunning())
    {
        windowPointer->repaint();
        StringTools::println("Enter String");
        globalString = StringTools::getString();
    }
}

void testNeuralNetwork()
{
    //neural network to recognize when a point is inside a circle of radius 1.

    srand(time(NULL));
    StringTools::println("Creating Network");
    /*
    NeuralNetwork network = NeuralNetwork();
    network.addLayerToEnd(2); //Input Layer
    network.addLayerToEnd(2); //Hidden Layer
    network.addLayerToEnd(2); //Output Layer

    network.resetNetwork(); //Set all of the weights and bias values
    network.setLearningRate(0.5);

    NeuralLayer* nLayer = network.getStartLayer();
    nLayer->getNeuron(0).setWeight(0, 0.15);
    nLayer->getNeuron(0).setWeight(1, 0.25);
    nLayer->getNeuron(1).setWeight(0, 0.20);
    nLayer->getNeuron(1).setWeight(1, 0.3);
    nLayer->setBiasValue(0, 0.35);
    nLayer->setBiasValue(1, 0.35);
    nLayer = nLayer->getNextLayer();
    
    nLayer->getNeuron(0).setWeight(0, 0.40);
    nLayer->getNeuron(0).setWeight(1, 0.50);
    nLayer->getNeuron(1).setWeight(0, 0.45);
    nLayer->getNeuron(1).setWeight(1, 0.55);
    nLayer->setBiasValue(0, 0.6);
    nLayer->setBiasValue(1, 0.6);

    std::vector< std::vector<double> > trainingInputs = std::vector< std::vector<double> >();
    std::vector< std::vector<double> > trainingOutputs = std::vector< std::vector<double> >();

    for(int i=0; i<1; i++)
    {
        trainingInputs.push_back( {0.05,0.1} );
        trainingOutputs.push_back( {0.01,0.99} );
    }
    */

    NeuralNetwork network = NeuralNetwork();
    network.addLayerToEnd(2); //Input Layer
    network.addLayerToEnd(4); //Hidden Layer
    network.addLayerToEnd(4); //Hidden Layer
    network.addLayerToEnd(1); //Output Layer

    network.resetNetwork(); //Set all of the weights and bias values
    network.setLearningRate(0.5);
    network.exportTestInformation("Before.xml");

    std::vector< std::vector<double> > trainingInputs = std::vector< std::vector<double> >();
    std::vector< std::vector<double> > trainingOutputs = std::vector< std::vector<double> >();

    for(int i=0; i<10; i++)
    {
        trainingInputs.push_back( {0,0} );
        trainingOutputs.push_back( {0} );
    }

    StringTools::println("Training Network");
    
    int modV = 1000000;
    LCG r = LCG(rand(), 12354, 0, modV);

    StringTools::println("output for 0,0: %d", network.run({0,0})[0]);
    
    for(int i=0; i<100000; i++)
    {
        //fill trainingInputs and Outputs
        for(int i2=0; i2<1; i2++)
        {
            double w1 = (double)r.get()/modV;
            double w2 = (double)r.get()/modV;

            double nx = (w1-0.5)*3;
            double ny = (w2-0.5)*3;

            if(i==0)
            {
                StringTools::println("%d, %d, %d, %d", w1, w2, nx, ny);
                StringTools::println("%d", MathExt::distanceTo(0,0,nx,ny));
            }

            double r = (MathExt::distanceTo(0,0,nx,ny) <= 1) ? 1.0 : 0.0;
            
            trainingInputs[i2][0] = w1;
            trainingInputs[i2][1] = w2;
            trainingOutputs[i2][0] = r;
        }

        network.train(trainingInputs, trainingOutputs);
    }


    network.exportTestInformation("After.xml");
    
    StringTools::println("Testing Network");
    while(true)
    {
        StringTools::println("Enter the x and y coordinates to check if in circle of radius 1 (use commas). Empty string to quit.");
        StringTools::println("Note that the maximum and minimum values allowed are -1.5 and 1.5 respectfully.");

        std::string input = StringTools::getString();

        if(input=="")
        {
            StringTools::println("\n");
            break;
        }
        else
        {
            std::vector<std::string> split = StringTools::splitString(input, ',');
            if(split.size()==2)
            {
                double x = MathExt::clamp( (std::stod(split[0]) + 1.5) / 3.0, 0.0, 1.0);
                double y = MathExt::clamp( (std::stod(split[1]) + 1.5) / 3.0, 0.0, 1.0);

                std::vector<double> inputs = {x, y};
                std::vector<double> output = network.run(inputs);

                StringTools::println("The network states that it believes that the point is in the circle with %d probability.\n", output[0]);
            }
        }
    }

}

void testColorPalette()
{
    StringTools::print("Enter image name: ");
    std::string filename = StringTools::getString();
    //C:\Users\Alan\source\repos\ImageLibrary\TestImages\PNG\Varying bit sizes and types\basn3p08.png
    int amountOfImages = 0;
    Image** imgArray = Image::loadImage(filename, &amountOfImages);

    Image* img = nullptr;
    if(amountOfImages>0)
    {
        img = imgArray[0];

        StringTools::print("Enter number of colors: ");
        std::string colorNumber = StringTools::getString();

        int num = StringTools::toInt(colorNumber);

        ColorPalette temp;

        StringTools::print("Quantization method? 1=meanCut, 2=medianCut, 3=kMeans: ");
        std::string method = StringTools::getString();
        int mNum = StringTools::toInt(method);

        StringTools::print("Unique only? y=yes, n=no: ");
        bool unique = StringTools::getChar() == 'y';

        StringTools::print("Convert to LAB before Conversion? y=yes, n=no: ");
        bool labSpace = StringTools::getChar() == 'y';

        unsigned long t1 = System::getCurrentTimeNano();
        if(mNum == 2)
        {
            temp = ColorPalette::generateOptimalPalette(img->getPixels(), img->getWidth() * img->getHeight(), num, ColorPalette::MEDIAN_CUT, labSpace, unique);
        }
        else if(mNum == 3)
        {
            temp = ColorPalette::generateOptimalPalette(img->getPixels(), img->getWidth() * img->getHeight(), num, ColorPalette::K_MEANS, labSpace, unique);
        }
        else
        {
            temp = ColorPalette::generateOptimalPalette(img->getPixels(), img->getWidth() * img->getHeight(), num, ColorPalette::MEAN_CUT, labSpace, unique);
        }
        unsigned long t2 = System::getCurrentTimeNano();

        StringTools::println("TimeTaken: %u", t2-t1);
        
        //./TestImages/GIF/Varying Bit size and type/basn3p08.gif
        
        temp.reBalance();

        img->setPalette(temp);

        StringTools::print("Dither Image? y=yes n=no: ");
        bool ditherConfirm = StringTools::getChar() == 'y';
        
        t1 = System::getCurrentTimeNano();

        if(ditherConfirm)
            Graphics::ditherImage(img, Graphics::FLOYD_DITHER);
        else
            img->enforcePalette();

        t2 = System::getCurrentTimeNano();

        StringTools::println("TimeTaken: %u", t2-t1);
        
        img->savePNG("paletteTest.png");
    }
    else
    {
        StringTools::println("Error on load Image");
    }

    delete img;
    delete[] imgArray;
}

void testColorConvert()
{
    Color c = Color{109, 128, 192, 255};

    Color xyz = ColorSpaceConverter::convert(c, ColorSpaceConverter::RGB_TO_XYZ);
    Color rgb = ColorSpaceConverter::convert(xyz, ColorSpaceConverter::XYZ_TO_RGB);
    Color lab = ColorSpaceConverter::convert(c, ColorSpaceConverter::RGB_TO_LAB);
    Color lrgb = ColorSpaceConverter::convert(lab, ColorSpaceConverter::LAB_TO_RGB);

    StringTools::println("ORIGINAL: %d, %d, %d", c.red, c.green, c.blue);
    StringTools::println("XYZ: %d, %d, %d", xyz.red, xyz.green, xyz.blue);
    StringTools::println("RGB: %d, %d, %d", rgb.red, rgb.green, rgb.blue);
    StringTools::println("LAB: %d, %d, %d", lab.red, lab.green, lab.blue);
    StringTools::println("lRGB: %d, %d, %d", lrgb.red, lrgb.green, lrgb.blue);
}

void testPNGSave()
{
    int amountOfImages = 0;
    Image** imgAr = Image::loadImage("abc123d.png", &amountOfImages);
    //Image** imgAr = Image::loadImage("paletteTest.png", &amountOfImages);
    if(amountOfImages<=0)
    {
        StringTools::println("ERROR ON LOAD");
        return;
    }
    else
    {
        StringTools::println("Loaded %d images", amountOfImages);
    }
    
    time_t t1 = System::getCurrentTimeMicro();
    imgAr[0]->savePNG("test1234.png", true, 0);
    time_t t2 = System::getCurrentTimeMicro();
    
    StringTools::println("Time taken = %u", (t2-t1));

    t1 = System::getCurrentTimeMicro();
    imgAr[0]->saveGIF("test1234.gif", 256, true, false);
    t2 = System::getCurrentTimeMicro();

    StringTools::println("Time taken = %u", (t2-t1));

    delete[] imgAr;
}

void gifSize()
{
    Sprite k = Sprite();
    time_t t1 = System::getCurrentTimeMicro();
    k.loadImage("Screenshot (409).png");
    time_t t2 = System::getCurrentTimeMicro();
    StringTools::println("Time to load: %u", t2-t1);

    if(k.getSize()>0)
    {
        t1 = System::getCurrentTimeMicro();
        
        k.getImage(0)->saveGIF("nSizeGif.gif", 256, false, false);
        t2 = System::getCurrentTimeMicro();

        StringTools::println("Time to save: %u", t2-t1);
        //k.getImage(0)->saveGIF("nSizeGifDither.gif", 256, true, false);
    }

    t1 = System::getCurrentTimeMicro();
}

void currentTest()
{
    // AudioOut::init();

    // Sound s = Sound();
    // s.loadSound("./testFiles/Audio/TetrisTheme.wav");
    
    // AudioOut::addSound(&s);

    // while(true)
    // {
    //     char c = StringTools::getChar();

    //     if(c == 'p')
    //     {
    //         s.play();
    //     }
    //     else if(c == 's')
    //     {
    //         s.stop();
    //     }
    //     else if(c == 'x')
    //     {
    //         AudioOut::dispose();
    //         break;
    //     }
    // }


    AudioIn::init();

    while(true)
    {
        char c = StringTools::getChar();

        if(c == 'r')
        {
            AudioIn::record();
        }
        else if(c == 's')
        {
            AudioIn::stop();
        }
        else if(c == 'x')
        {
            AudioIn::stop();
            break;
        }
    }

    Sound s = Sound();
    s.copyData(AudioIn::getAudioData().data(), AudioIn::getAudioData().size());
    s.saveWAV("recordedWav.wav");

    AudioIn::dispose();
}

void gradientTest()
{
    int wid, hei;

    Sprite img = Sprite();
    img.loadImage("./testFiles/edgeImg.png");
    
    Graphics::convertToColorSpace(img.getImage(0), ColorSpaceConverter::RGB_TO_YCBCR);

    wid = img.getImage(0)->getWidth();
    hei = img.getImage(0)->getHeight();
    
    std::vector<std::vector<Vec2f>> imgGrad = Graphics::calculateGradient(img.getImage(0), Graphics::RED_CHANNEL);

    //save as 3 images.

    //x gradient
    Image k = Image(wid,hei);
    for(int y=0; y<hei; y++)
    {
        for(int x=0; x<wid; x++)
        {
            Color c;
            c.red = (unsigned char)MathExt::clamp(imgGrad[y][x].x, 0.0, 255.0);
            c.green = c.red;
            c.blue = c.red;
            c.alpha = 255;
            k.setPixel(x,y,c);
        }
    }

    k.savePNG("xGradImg.png");

    //y gradient
    Image k2 = Image(wid,hei);
    for(int y=0; y<hei; y++)
    {
        for(int x=0; x<wid; x++)
        {
            Color c;
            c.red = (unsigned char)MathExt::clamp(imgGrad[y][x].y, 0.0, 255.0);
            c.green = c.red;
            c.blue = c.red;
            c.alpha = 255;
            k2.setPixel(x,y,c);
        }
    }

    k2.savePNG("yGradImg.png");

    //x+y gradient
    Image k3 = Image(wid,hei);
    for(int y=0; y<hei; y++)
    {
        for(int x=0; x<wid; x++)
        {
            Color c;
            c.red = (unsigned char)MathExt::clamp(MathExt::vecLength(imgGrad[y][x]), 0.0, 255.0);
            c.green = c.red;
            c.blue = c.red;
            c.alpha = 255;
            k3.setPixel(x,y,c);
        }
    }

    k3.savePNG("xyGradImg.png");
}

void fileTest()
{
    Image k = Image(640, 480);

    Graphics::setClippingRect( Box2D(64, 64, 128, 128) );
    Graphics::setColor({255,0,0,255});

    k.drawCircle(64,64,64,false);
    k.savePNG("clippingTest.png");
}

void testGui()
{
    WndWindow w = WndWindow("test", 640, 480, -1, -1, WndWindow::NORMAL_WINDOW);
    
    GuiContainer mainContainer;

    GuiRectangleButton applyBut = GuiRectangleButton(640-64, 480-52, 64, 24);
    GuiTextBlock applyText = GuiTextBlock(0, 0, 64, 24);

    applyBut.setOnClickFunction([](GuiInstance* ins)->void{
        std::wstring info = System::fileDialogBox(System::TYPE_SAVE_FILE);
        StringTools::println("GOT: %ls", info.c_str());
    });

    GuiRectangleButton resetBut = GuiRectangleButton(640-64, 480-26, 64, 24);
    GuiTextBlock resetText = GuiTextBlock(0, 0, 64, 24);

    resetBut.setOnClickFunction([](GuiInstance* ins)->void{
        std::wstring info = System::fileDialogBox(System::TYPE_OPEN_FOLDER);
        StringTools::println("GOT: %ls", info.c_str());
    });

    GuiRectangleButton loadBut = GuiRectangleButton(12, 12, 64, 24);
    GuiTextBlock loadText = GuiTextBlock(0, 0, 64, 24);

    loadBut.setOnClickFunction([](GuiInstance* ins)->void{
        std::wstring info = System::fileDialogBox(System::TYPE_OPEN_FILE);
        StringTools::println("GOT: %ls", info.c_str());
    });

    mainContainer.addChild(&applyBut);
    mainContainer.addChild(&resetBut);
    mainContainer.addChild(&loadBut);

    w.getGuiManager()->addElement(&mainContainer);

    w.setThreadAutoRepaint(true);
    w.setThreadUpdateTime(16);

    w.waitTillClose();
}

void ssePaint()
{
    //must test every draw function in graphics to ensure same results under SSE and AVX instructions
    Image img = Image(640, 640);

    Graphics::setColor({255,0,0,224});
    Color verifyColor = {0,255,255,128};

    #pragma region DRAW_RECT

    Graphics::drawRect(32, 32, 64, 64, false, &img);
    img.drawPixel(32,32,verifyColor);
    img.drawPixel(64,32,verifyColor);
    img.drawPixel(64,64,verifyColor);
    img.drawPixel(32,64,verifyColor);

    Graphics::drawRect(67, 33, 78, 41, false, &img);
    img.drawPixel(67,33,verifyColor);
    img.drawPixel(78,33,verifyColor);
    img.drawPixel(78,41,verifyColor);
    img.drawPixel(67,41,verifyColor);

    #pragma endregion

    #pragma region DRAW_LINE

    Graphics::drawLine(96, 32, 128, 32, &img);
    img.drawPixel(96,32,verifyColor);
    img.drawPixel(128,32,verifyColor);

    Graphics::drawLine(96, 35, 127, 35, &img);
    img.drawPixel(96,35,verifyColor);
    img.drawPixel(127,35,verifyColor);

    Graphics::drawLine(99, 37, 125, 37, &img);
    img.drawPixel(99,37,verifyColor);
    img.drawPixel(125,37,verifyColor);

    #pragma endregion

    #pragma region DRAW_CIRCLE

    Graphics::drawCircle(160, 32, 16, false, &img);
    Graphics::drawCircle(160, 64, 11, false, &img);
    
    #pragma endregion

    #pragma region DRAW_IMG
    Sprite testDrawImg = Sprite();
    testDrawImg.loadImage("m4o2z38ykg801.png");

    Graphics::drawSprite(testDrawImg.getImage(0), 127, 128, &img);
    
    #pragma endregion

    #pragma region DRAW_TRIANGLE
    
    Graphics::drawTriangle(250, 25, 280, 26, 270, 47, false, &img);
    img.drawPixel(250,25,verifyColor);
    img.drawPixel(280,26,verifyColor);
    img.drawPixel(270,47,verifyColor);

    Graphics::setColor({0,255,0,128});
    Graphics::drawTriangle(250, 24, 280, 25, 270, 4, false, &img);
    img.drawPixel(250,24,verifyColor);
    img.drawPixel(280,25,verifyColor);
    img.drawPixel(270,4,verifyColor);

    #pragma endregion

    img.savePNG("SSE & AVX Test.png");
}

void testGui2()
{
    WndWindow guiWindow = WndWindow("GUI", 640, 480);
    guiWindow.setActivateGui(true);

    GuiRectangleButton startBut = GuiRectangleButton(320-32, 240-12, 64, 24);
    GuiTextBlock startText = GuiTextBlock(0, 0, 64, 24);
    startText.setText("START");
    startBut.addChild(&startText);

    GuiRectangleButton stopBut = GuiRectangleButton(320-32, 272-12, 64, 24);
    GuiTextBlock stopText = GuiTextBlock(0, 0, 64, 24);
    stopText.setText("STOP");
    stopBut.addChild(&stopText);

    guiWindow.getGuiManager()->addElement(&startBut);
    guiWindow.getGuiManager()->addElement(&stopBut);

    while(guiWindow.getRunning())
    {
        guiWindow.repaint();
        System::sleep(10);
    }

}

void testNewFFT()
{
    std::vector<double> baseData = std::vector<double>(8);
    // baseData[0] = ComplexNumber(MathExt::cos(0.0 * 3.0/8.0), 0);
    // baseData[1] = ComplexNumber(MathExt::cos(1.0 * 3.0/8.0), 0);
    // baseData[2] = ComplexNumber(MathExt::cos(2.0 * 3.0/8.0), 0);
    // baseData[3] = ComplexNumber(MathExt::cos(3.0 * 3.0/8.0), 0);
    // baseData[4] = ComplexNumber(MathExt::cos(4.0 * 3.0/8.0), 0);
    // baseData[5] = ComplexNumber(MathExt::cos(5.0 * 3.0/8.0), 0);
    // baseData[6] = ComplexNumber(MathExt::cos(6.0 * 3.0/8.0), 0);
    // baseData[7] = ComplexNumber(MathExt::cos(7.0 * 3.0/8.0), 0);
    for(int i=0; i<baseData.size(); i++)
    {
        baseData[i] = i;
    }


    time_t t1, t2;
    t1 = System::getCurrentTimeNano();
    std::vector<double> referenceValues = MathExt::cosineTransform(baseData.data(), baseData.size(), false);
    t2 = System::getCurrentTimeNano();

    StringTools::println("Time for base: %llu", t2-t1);

    std::vector<double> referenceFastValues = std::vector<double>(8);
    t1 = System::getCurrentTimeNano();
    
    MathExt::FCT8(baseData.data(), referenceFastValues.data(), false);
    t2 = System::getCurrentTimeNano();

    StringTools::println("Time for fft: %llu", t2-t1);
    
    // t1 = System::getCurrentTimeNano();
    // std::vector<ComplexNumber> testFastValues = MathExt::fastFourierTransformTest(baseData.data(), baseData.size(), false);
    // t2 = System::getCurrentTimeNano();

    // StringTools::println("Time for nfft: %llu", t2-t1);
    
    StringTools::println("DCT   |   FDCT");
    for(int i=0; i<baseData.size(); i++)
    {
        StringTools::println("%.3f | %.3f", referenceValues[i], referenceFastValues[i]);
    }
}

void testJPEG()
{
    Sprite k = Sprite();
    Sprite k2 = Sprite();
    Sprite k3 = Sprite();

    time_t t1 = System::getCurrentTimeMicro();
    k.loadImage("./TestImages/JPEG/61eY5p914hL._AC_SX466_.jpg");
    time_t t2 = System::getCurrentTimeMicro();

    StringTools::println("TimeTaken: %u", t2-t1);

    t1 = System::getCurrentTimeMicro();
    k2.loadImage("background.bmp");
    t2 = System::getCurrentTimeMicro();

    StringTools::println("TimeTaken: %u", t2-t1);

    t1 = System::getCurrentTimeMicro();
    k3.loadImage("background.png");
    t2 = System::getCurrentTimeMicro();

    StringTools::println("TimeTaken: %u", t2-t1);

    if(k.getImage(0)!=nullptr)
        k.getImage(0)->saveBMP("testJPEG.bmp");
}

int main(int argc, char** argv)
{
    StringTools::init();
    Graphics::init();

    testJPEG();
    //testNewFFT();
    //testGui2();
    //ssePaint();
    //testGui();

    //fileTest();

    //gradientTest();
    //currentTest();

    //gifSize();

    //testPNGSave();

    //testColorPalette();
    //testColorConvert();

    //testFontSVG("C:\\Users\\Alan\\Documents\\VSCodeProjects\\GLib\\SVGFonts\\My Font - SVG Font - 2020.8.12-21.40.21.svg");
    
    //testFontGraphics();

    //testFontSVG(L"C:\\Users\\Alan\\Documents\\VSCodeProjects\\GLib\\SVGFonts\\AnyConv.com__consolab.svg");
    
    //drawLoadedSvg("C:/Users/Alan/Documents/VSCodeProjects/GLib/SVGs/_ionicons_svg_md-mail.svg");
    //drawLoadedSvg("C:/Users/Alan/Documents/VSCodeProjects/GLib/testFiles/SvgFiles/Five Pointed Star.svg");
    //drawLoadedSvg("testFiles/SvgFiles/transforms.svg");

    //testNeuralNetwork();

    system("pause");
    return 0;
}
