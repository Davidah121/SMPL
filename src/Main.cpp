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

#include "Algorithms.h"

using namespace glib;

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
 *      Adjust Chroma Subsampling for JPEGs to have more consistent results with other loaders.
 *      Work on making the library portable
 */

WndWindow* windowPointer;
Image* imgPointer;
Image* img;
std::string globalString = "";

VectorGraphic shape = VectorGraphic();

void testAntiAliasingSVG()
{
    // VectorRectangle rect = VectorRectangle();
    // rect.setX(64);
    // rect.setY(64);
    // rect.setWidth(64);
    // rect.setHeight(64);
    // rect.setFillColor({0,0,0,255});
    
    // rect.setRX(8);
    // rect.setRY(8);
    // shape.addShape(&rect);

    // VectorEllipse ellipse = VectorEllipse();
    // ellipse.setX(32);
    // ellipse.setY(32);
    // ellipse.setXRadius(8);
    // ellipse.setYRadius(16);
    // ellipse.setFillColor({0,0,255,64});
    // shape.addShape(&ellipse);


    VectorCircle circle = VectorCircle();
    circle.setX(64);
    circle.setY(64);
    circle.setRadius(16);
    circle.setFillColor({255,255,255,255});
    shape.addShape(&circle);

    // VectorPath p = VectorPath();
    // p.addMoveTo(32,32);
    // p.addArcToRel(16,16,0,0,0,16,-16);
    // p.addClosePath();

    // p.addMoveTo(96,32);
    // p.addArcToRel(16,16,0,0,1,16,-16);
    // p.addClosePath();

    // p.addMoveTo(32,96);
    // p.addArcToRel(16,16,0,1,0,16,-16);
    // p.addClosePath();

    // p.addMoveTo(96,96);
    // p.addArcToRel(16,16,0,1,1,16,-16);
    // p.addClosePath();

    // shape.addShape(&p);


    Image drawImg = Image(128, 128);
    drawImg.setAllPixels({0,0,0,255});
    VectorGraphic::setAALevel(2);
    shape.draw(&drawImg);

    // Graphics::setColor({64,0,0,255});
    // drawImg.drawCircle(32,32,16,false);

    drawImg.savePNG("testAntiAliasing.png");
}

void drawLoadedSvg(std::string file)
{
    shape.load(file);
    Mat3f viewBoxMat = shape.getViewBox();

    for(int i=0; i<3; i++)
    {
        for(int i2=0; i2<3; i2++)
        {
            StringTools::print("%.3f, ",viewBoxMat[i][i2]);
        }
        StringTools::println("");
    }

    Image nImg = Image(shape.getWidth(), shape.getHeight());
    img = &nImg;

    shape.draw(&nImg);

    nImg.savePNG("testSVGDraw.png");

    // windowPointer = new WndWindow(L"Displaying Path Arcs", shape.getWidth(), shape.getHeight());
    // windowPointer->setActivateGui(false);
    // windowPointer->setPaintFunction(svgTest);
    // windowPointer->setThreadAutoRepaint(false);

    // windowPointer->repaint();
    // windowPointer->waitTillClose();
    
    // if(windowPointer!=nullptr)
    //     delete windowPointer;
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

void audioTest()
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

void testArithmeticCoding()
{
    std::vector<unsigned char> data = {'W', 'I', 'K', 'I'};
    std::vector<double> percentages;

    double compValue = Compression::compressArithmetic(data, percentages);

    std::vector<unsigned char> decompMessage = Compression::decompressArithmetic(compValue, 4, percentages);

    for(int i=0; i<decompMessage.size(); i++)
    {
        StringTools::println("%c : %d", decompMessage[i], decompMessage[i]);
    }
}

void testCollision()
{
    Polygon2D k = Polygon2D();
    k.addVertex(Vec2f(0,0));
    k.addVertex(Vec2f(200,200));
    k.addVertex(Vec2f(400,0));

    Polygon2D k2 = Polygon2D();
    k2.addVertex(Vec2f(25,10));
    k2.addVertex(Vec2f(40,30));
    k2.addVertex(Vec2f(50,20));

    Vec2f pos1, pos2;

    pos1 = Vec2f(32,32);
    pos2 = Vec2f(128,256);
    
    GuiCustomObject cus = GuiCustomObject();
    cus.setUpdateFunction([&pos1,&pos2]()->void{
        Input::pollInput();
        if(Input::getKeyDown('W'))
        {
            pos1.y-=1;
        }
        else if(Input::getKeyDown('S'))
        {
            pos1.y+=1;
        }
        if(Input::getKeyDown('A'))
        {
            pos1.x-=1;
        }
        else if(Input::getKeyDown('D'))
        {
            pos1.x+=1;
        }

        if(Input::getKeyDown(Input::KEY_UP))
        {
            pos2.y-=1;
        }
        else if(Input::getKeyDown(Input::KEY_DOWN))
        {
            pos2.y+=1;
        }
        if(Input::getKeyDown(Input::KEY_LEFT))
        {
            pos2.x-=1;
        }
        else if(Input::getKeyDown(Input::KEY_RIGHT))
        {
            pos2.x+=1;
        }

        
    });

    cus.setRenderFunction([&pos1,&pos2, &k, &k2](Image* surf)->void{
        VectorGraphic g = VectorGraphic();

        k.setPosition(pos1);
        k2.setPosition(pos2);

        VectorPolygon r = VectorPolygon();
        r.setFillColor({255,255,255,128});
        r.addPoint(k.getVertex(0));
        r.addPoint(k.getVertex(1));
        r.addPoint(k.getVertex(2));

        VectorPolygon e = VectorPolygon();
        e.setFillColor({255,0,255,128});
        e.addPoint(k2.getVertex(0));
        e.addPoint(k2.getVertex(1));
        e.addPoint(k2.getVertex(2));

        g.addShape(&r);
        g.addShape(&e);

        g.draw(surf);

        bool collision = CollisionMaster::SeparatingAxisTheorem(&k, &k2);
        if(!collision)
            Graphics::setColor({255,255,255,255});
        else
            Graphics::setColor({255,0,0,255});
        surf->drawRect(0, 0, 32, 32, false);

        Graphics::setColor({0,128,64,255});
        surf->drawRect(k.getCenterPoint().x-4, k.getCenterPoint().y-4, k.getCenterPoint().x+4, k.getCenterPoint().y+4, false);
        surf->drawRect(k2.getCenterPoint().x-4, k2.getCenterPoint().y-4, k2.getCenterPoint().x+4, k2.getCenterPoint().y+4, false);
        
        surf->drawLine(k.getCenterPoint().x, k.getCenterPoint().y, k.getVertex(0).x, k.getVertex(0).y);
        surf->drawLine(k.getCenterPoint().x, k.getCenterPoint().y, k.getVertex(1).x, k.getVertex(1).y);
        surf->drawLine(k.getCenterPoint().x, k.getCenterPoint().y, k.getVertex(2).x, k.getVertex(2).y);
        
    });

    WndWindow window = WndWindow("TT)", 640,480);
    window.getGuiManager()->addElement(&cus);
    window.waitTillClose();
}

void testSVGTransforms()
{
    GuiCustomObject cus = GuiCustomObject();

    double rot = 0;
    cus.setRenderFunction([&rot](Image* surf)->void{
        VectorPath p = VectorPath();
        p.setFillColor(ColorNameConverter::NameToColor("blue"));
        p.addMoveTo(64,64);
        p.addQuadToRel(32,32,64,0);
        p.addQuadToShortRel(32,32);
        p.addClosePath();

        p.setTransform(MathExt::scale2D(2,3)*MathExt::rotation2D(MathExt::toRad(90.0),96,96));

        p.draw(surf, surf->getWidth(), surf->getHeight());
    });

    WndWindow window = WndWindow("SVG Transforms", 640,480, -1, -1, WndWindow::TYPE_USER_MANAGED);
    window.getGuiManager()->addElement(&cus);

    bool first = false;
    while(window.getRunning())
    {
        window.update();
        window.guiUpdate();

        if(!first)
        {
            window.repaint();
            first = false;
        }

        System::sleep(10,0);
    }
    // window.waitTillClose();
}

int main(int argc, char** argv)
{
    StringTools::init();
    //Graphics::init();
    testSVGTransforms();

    //system("pause");
    return 0;
}
