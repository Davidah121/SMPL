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
 *      Compression - Deflate compression.
 *          
 *      Image - Load JPEG, DDS
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
 *      Fix StringTools at some point. Especially the conversion between char* to wchar_t*
 *      Lastly
 *      Work on making the library portable
 */

WndWindow* windowPointer;
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

void testXML(std::wstring filename)
{
    SimpleXml x = SimpleXml();
    x.load(filename);

    StringTools::println(x.nodes[0]->title);
    StringTools::println(x.nodes[0]->value);
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
        Graphics::setColor({255,255,255,255});
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

    windowPointer = new WndWindow("Image Tester", 1000,1000);
//C:\Users\Alan\Pictures\Screenshots\Screenshot (195).gif
//C:\Users\Alan\source\repos\ImageLibrary\TestImages\PNG\Varying bit sizes and types
//C:\Users\Alan\source\repos\ImageLibrary\TestImages\PNG\Interlacing


    windowPointer->setPaintFunction( paintFunction );

    while(windowPointer->getRunning())
    {
        windowPointer->repaint();
        System::sleep(16,666);
    }

    //img->saveGIF("File.gif");
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

    StringTools::out << "output for 0,0: " << network.run({0,0})[0] << StringTools::lineBreak;
    
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
                StringTools::out << w1 << ", " << w2 << ", " << nx << ", " << ny << StringTools::lineBreak;
                StringTools::out << MathExt::distanceTo(0,0,nx,ny) << StringTools::lineBreak;
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

                StringTools::out << "The network states that it believes that the point is in the circle with " << output[0] << " probability.\n" << StringTools::lineBreak;
            }
        }
    }

}

void testColorPalette()
{
    StringTools::out << "Enter image name: ";
    std::string filename = StringTools::getString();
    //C:\Users\Alan\source\repos\ImageLibrary\TestImages\PNG\Varying bit sizes and types\basn3p08.png
    int amountOfImages = 0;
    Image** imgArray = Image::loadImage(filename, &amountOfImages);

    Image* img = nullptr;
    if(amountOfImages>0)
    {
        img = imgArray[0];

        StringTools::out << "Enter number of colors: ";
        std::string colorNumber = StringTools::getString();

        int num = StringTools::toInt(colorNumber);

        ColorPalette temp;

        StringTools::out << "Quantization method? 1=meanCut, 2=medianCut, 3=kMeans: ";
        std::string method = StringTools::getString();
        int mNum = StringTools::toInt(method);

        StringTools::out << "Unique only? y=yes, n=no: ";
        bool unique = StringTools::getChar() == 'y';

        StringTools::out << "Convert to LAB before Conversion? y=yes, n=no: ";
        bool labSpace = StringTools::getChar() == 'y';

        
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
        
        //./TestImages/GIF/Varying Bit size and type/basn3p08.gif
        
        temp.reBalance();
        img->setPalette(temp);

        StringTools::out << "Dither Image? y=yes n=no: ";
        bool ditherConfirm = StringTools::getChar() == 'y';
        unsigned long t1 = System::getCurrentTimeMicro();

        if(ditherConfirm)
            Graphics::ditherImage(img, Graphics::FLOYD_DITHER);
        else
            img->enforcePalette();
        
        unsigned long t2 = System::getCurrentTimeMicro();
        StringTools::out << "Time taken: " << (t2-t1) << StringTools::lineBreak;
        
        //img->saveBMP("paletteTest.bmp");
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

    StringTools::out << "ORIGINAL: " << c.red << ", " << c.green << ", " << c.blue << StringTools::lineBreak;
    StringTools::out << "XYZ: " << xyz.red << ", " << xyz.green << ", " << xyz.blue << StringTools::lineBreak;
    StringTools::out << "RGB: " << rgb.red << ", " << rgb.green << ", " << rgb.blue << StringTools::lineBreak;

    StringTools::out << "lab: " << lab.red << ", " << lab.green << ", " << lab.blue << StringTools::lineBreak;
    StringTools::out << "lrgb: " << lrgb.red << ", " << lrgb.green << ", " << lrgb.blue << StringTools::lineBreak;
}

void testFourierTransform()
{
    int samplesPerSec = 10;
    int size = 16;
    int padding = size-samplesPerSec;

    double* arr = new double[size];
    
    double frequency = 4; //A4
    double mult = 2.0*PI*frequency;

    for(int i=0; i<samplesPerSec; i++)
    {
        arr[i] = sin( mult * (double)i/samplesPerSec );
    }
    
    unsigned t1 = System::getCurrentTimeNano();
    std::vector<ComplexNumber> normalFourier = MathExt::fourierTransform(arr, size, samplesPerSec);
    unsigned t2 = System::getCurrentTimeNano();

    StringTools::println("normalFourier done");
    StringTools::out << "Took: " << (t2-t1) << StringTools::lineBreak;

    t1 = System::getCurrentTimeNano();
    std::vector<ComplexNumber> fastFourier = MathExt::fastFourierTransform(arr, size);
    t2 = System::getCurrentTimeNano();

    StringTools::println("fastFourier done");
    StringTools::out << "Took: " << (t2-t1) << StringTools::lineBreak;
    
    //save as a .csv

    SimpleFile f = SimpleFile("results.csv", SimpleFile::WRITE);

    std::string header = "Normal, Fast";
    f.writeString(header);
    f.writeLineBreak();
    
    for(int i=0; i<samplesPerSec/2; i++)
    {
        double length1 = 2 * MathExt::sqrt(MathExt::sqr(normalFourier[i].real) + MathExt::sqr(normalFourier[i].imaginary));
        double length2 = 2 * MathExt::sqrt(MathExt::sqr(fastFourier[i].real) + MathExt::sqr(fastFourier[i].imaginary));
        
        // length1/=samplesPerSec;
        // length2/=samplesPerSec;
        
        std::string finalString = "";
        finalString += (std::to_string(length1) + ", ") + std::to_string(length2);

        f.writeString(finalString);
        f.writeLineBreak();
    }

    f.close();
}

void testAudio()
{
    Sound s = Sound();
    s.loadSound("./testFiles/Audio/At DEWM's GATE.wav");
    s.setLoop(true);
    s.setLoopStart(18.0);
    s.setLoopEnd(54.0);

    Sound s2 = Sound();
    s2.loadSound("./testFiles/Audio/Explosion.wav");
    
    Audio::init();
    Audio::addSound(&s);
    Audio::addSound(&s2);

    Audio::setVolume(1);
    
    while(true)
    {
        StringTools::print("type stuff: ");
        std::string line = StringTools::getString();
        if(line == "end")
        {
            break;
        }
        else if(line == "play_s1")
        {
            s.play();
        }
        else if(line == "play_s2")
        {
            s2.play();
        }
        else if(line == "stop_s1")
        {
            s.stop();
        }
        else if(line == "stop_s2")
        {
            s2.stop();
        }
        else if(line == "pause_s1")
        {
            s.pause();
        }
        else if(line == "pause_s2")
        {
            s2.pause();
        }
        else if(line == "volume")
        {
            StringTools::println("Enter volume");
            double v = std::stod(StringTools::getString());
            Audio::setVolume(v);
        }
    }

    Audio::dispose();
}

void imageExtenderThing()
{
    //always extend to 1920 x 1080
    Image newImg = Image(1920, 1080);

    int amountOfImages = 0;
    Image** loadImg = Image::loadImage("abc123d.png", &amountOfImages);

    if(amountOfImages<=0)
    {
        StringTools::println("Couldn't load image");
        return;
    }

    Image* otherImg = loadImg[0];

    //extend out into white
    //let 8 be the maximum allowed pixels till white
    int xDis = newImg.getWidth() - otherImg->getWidth();
    int yDis = newImg.getHeight() - otherImg->getHeight();

    int pixsToWhiteX = MathExt::clamp(xDis, 0, xDis/2);
    int pixsToWhiteY = MathExt::clamp(yDis, 0, 8);
    

    int x1 = xDis/2;
    int y1 = yDis/2;
    int x2 = x1 + otherImg->getWidth() - 1;
    int y2 = y1 + otherImg->getHeight() - 1;

    float xF, yF;

    for(int y=0; y<newImg.getHeight(); y++)
    {
        int yVal = y;
        yVal = MathExt::clamp(yVal, y1, y2);

        yVal -= y1;
        if(y < y1)
        {
            //up
            yF = 1.0 - ((double)MathExt::clamp( y1-y, 0, pixsToWhiteY) / pixsToWhiteY );
        }
        else if(y > y2)
        {
            //down
            yF = 1.0 - ((double)MathExt::clamp( y-y2, 0, pixsToWhiteY) / pixsToWhiteY );
        }
        else
        {
            yF = 1.0;
        }

        for(int x=0; x<newImg.getWidth(); x++)
        {
            int xVal = x;
            xVal = MathExt::clamp(xVal, x1, x2);

            xVal -= x1;
            
            Color c1 = otherImg->getPixel(xVal, yVal);
            Color c2 = {255, 255, 255, 255};

            if(x < x1)
            {
                //left
                xF = 1.0 - ((double)MathExt::clamp( x1-x, 0, pixsToWhiteX) / pixsToWhiteX );
            }
            else if(x > x2)
            {
                //right
                xF = 1.0 - ((double)MathExt::clamp( x-x2, 0, pixsToWhiteX) / pixsToWhiteX );
            }
            else
            {
                xF = 1.0;
            }

            Color finalColor = Graphics::lerp(c2, c1, xF * yF);
            newImg.setPixel(x, y, finalColor);
        }
    }

    newImg.saveBMP("thingTest.bmp");

    delete otherImg;
    delete[] loadImg;
}

void testPNGSave()
{
    int amountOfImages = 0;
    Image** imgAr = Image::loadImage("C:/Users/Alan/source/repos/ImageLibrary/TestImages/PNG/Varying bit sizes and types/basn3p08.png", &amountOfImages);
    if(amountOfImages<=0)
    {
        StringTools::println("ERROR ON LOAD");
        return;
    }

    imgAr[0]->savePNG("test1234.png");

    delete[] imgAr;
}

void testCRC()
{
    unsigned char* data = new unsigned char[9]{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

    unsigned int remainder = Compression::crc(data, 9, Compression::CRC_8);

    StringTools::println( StringTools::toHexString(remainder & 0xFF));
    delete[] data;
}

int main(int argc, char** argv)
{
    StringTools::init();

    //testAudio();

    //imageExtenderThing();
    
    //testFourierTransform();

    //testCRC();
    //testPNGSave();

    testColorPalette();
    //testColorConvert();

    //testImageLoader();
    //testFontSVG("C:\\Users\\Alan\\Documents\\VSCodeProjects\\GLib\\SVGFonts\\My Font - SVG Font - 2020.8.12-21.40.21.svg");
    
    //testFontGraphics();

    //testFontSVG(L"C:\\Users\\Alan\\Documents\\VSCodeProjects\\GLib\\SVGFonts\\AnyConv.com__consolab.svg");
    //testLZW();
    //testImageDisplay();
    
    //testXML("C:/Users/Alan/Documents/VSCodeProjects/GLib/testFiles/XmlFiles/test2.xml");
    //drawLoadedSvg("C:/Users/Alan/Documents/VSCodeProjects/GLib/SVGs/_ionicons_svg_md-mail.svg");
    //testXML("C:/Users/Alan/Documents/VSCodeProjects/GLib/SVGs/_ionicons_svg_md-mail.svg");
    //drawLoadedSvg("C:/Users/Alan/Documents/VSCodeProjects/GLib/testFiles/SvgFiles/Five Pointed Star.svg");
    //drawLoadedSvg("testFiles/SvgFiles/transforms.svg");
    //testMatrixStuff();

    //testNeuralNetwork();

    system("pause");
    return 0;
}
