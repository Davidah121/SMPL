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
        
        if(ditherConfirm)
            Graphics::ditherImage(img, Graphics::FLOYD_DITHER);
        else
            img->enforcePalette();
        
        img->saveBMP("paletteTest.bmp");
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
    StringTools::println("Took: %d",(t2-t1));

    t1 = System::getCurrentTimeNano();
    std::vector<ComplexNumber> fastFourier = MathExt::fastFourierTransform(arr, size);
    t2 = System::getCurrentTimeNano();

    StringTools::println("fastFourier done");
    StringTools::println("Took: ",(t2-t1));
    
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

    Image::IMAGE_SAVE_ALPHA = true;
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

void testScale()
{
    int amountOfImages = 0;
    Image** imgAr = Image::loadImage("./ScaleTest/abc123d.png", &amountOfImages);
    if(amountOfImages<=0)
    {
        StringTools::println("ERROR ON LOAD");
        return;
    }

    Image* img1 = Graphics::scaleImage(imgAr[0], 1.3, 1.1, Graphics::NEAREST_NEIGHBOR_FILTER);
    StringTools::println("Finished NN");
    img1->savePNG("./ScaleTest/test/img_NN.png");
    StringTools::println("Finished saving NN");
    delete img1;
    
    Image* img2 = Graphics::scaleImage(imgAr[0], 1.5, 1.15, Graphics::BILINEAR_FILTER);
    StringTools::println("Finished BL");
    img2->savePNG("./ScaleTest/test/img_BL.png");
    StringTools::println("Finished saving BL");
    delete img2;

    Image* img3 = Graphics::scaleImage(imgAr[0], 1.7, 1.25, Graphics::BICUBIC_FILTER);
    StringTools::println("Finished BC");
    img3->savePNG("./ScaleTest/test/img_BC.png");
    StringTools::println("Finished saving BC");
    delete img3;

    delete imgAr[0];
    delete[] imgAr;
}

/*
void printThing1(const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vwprintf(fmt, args);

    va_end(args);
}

void printThing2(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);

    va_end(args);
}
*/

void drawAnimation()
{
    if(windowPointer!=nullptr)
    {
        Graphics::setColor({255,255,255,255});
        Image background = Image(windowPointer->getWidth(), windowPointer->getHeight());
        background.clearImage();

        if(img!=nullptr)
            background.drawSprite(img, 0, 0);

        windowPointer->drawImage(&background);
    }
}

void testAnimatedImages()
{
    windowPointer = new WndWindow("Title", 1280, 720);
    windowPointer->setPaintFunction(drawAnimation);

    int amountOfImages = 0;
    Sprite s = Sprite();
    int currTime = 0;
    int currIndex = 0;
    int time = -1;

    int stepsToChange = -1;

    StringTools::println("Controls: F=changeFile, T=changeTime, Q=Quit");

    while(windowPointer->getRunning())
    {
        Input::pollInput();
        if(Input::getKeyPressed('F'))
        {
            //Change file input
            StringTools::print("Input Image Name: ");
            std::string command = StringTools::getString();
            s.loadImage(command);

            StringTools::println("Loaded %d Images", s.getSize());

            currIndex = 0;
            time = -1;
            stepsToChange = -1;
            img = s.getImage(currIndex);
        }
        else if (Input::getKeyPressed('T'))
        {
            StringTools::print("Input frames per second (max of 60): ");
            std::string command = StringTools::getString();

            if(s.getSize()>0)
            {
                time = MathExt::min(stoi(command), 60);
                stepsToChange = 60 / time;
            }
        }
        else if (Input::getKeyPressed('V'))
        {
            StringTools::print("Set frame: ");
            std::string command = StringTools::getString();
            int frame = stoi(command);
            if(s.getSize()>0)
            {
                time = -1;
                stepsToChange=-1;
                currIndex = 0;
                img = s.getImage(frame % s.getSize());
            }
        }
        else if (Input::getKeyPressed('Q'))
        {
            windowPointer->close();
            break;
        }

        currTime++;

        if(stepsToChange>0)
        {
            if(currTime>=stepsToChange)
            {
                currTime = 0;
                currIndex = (currIndex+1) % s.getSize();
                
                img = s.getImage(currIndex);
            }
        }
        
        windowPointer->repaint();
        System::sleep(16,666);
    }

    delete windowPointer;
}

void testGIF()
{
    int imgs = 0;
    Image** arr = Image::loadImage("fuavs6gs0w801.gif", &imgs);

    if(imgs>0)
    {
        StringTools::println("Images %d", imgs);
        for(int i=0; i<imgs; i++)
        {
            delete arr[i];
        }
        delete[] arr;
    }
}

int main(int argc, char** argv)
{
    StringTools::init();
    //testScale();

    //testAudio();

    //imageExtenderThing();
    
    //testFourierTransform();

    //testCRC();
    //testPNGSave();
    //testAnimatedImages();
    //testGIF();

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
