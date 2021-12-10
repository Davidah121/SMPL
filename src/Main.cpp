#include "SimpleWindow.h"
#include "SimpleGraphics.h"
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
#include "SmartMemory.h"

#include "ext/DXWindow.h"
#include "ext/DXShader.h"
#include "ext/DXModel.h"

#include "ext/GLGraphics.h"

using namespace glib;

void testGLStuff()
{
    int height = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
    GetSystemMetrics(SM_CXPADDEDBORDER));
    StringTools::println("HEIGHT %d", height);
    
    GLWindow w = GLWindow("OpenGL Window", 640, 480, -1, -1, GLWindow::NORMAL_WINDOW | GLWindow::TYPE_USER_MANAGED);

    GLShader s = GLShader("./testFiles/GLSL/texturedCube.vs", "./testFiles/GLSL/texturedCube.fs");
    GLShader surfaceShader = GLShader("./testFiles/GLSL/surfaceShader.vs", "./testFiles/GLSL/surfaceShader.fs");
    
    GLTexture texture = GLTexture("TestImages/GIF/Varying Bit size and type/basn0g01.gif");

    Model surfaceModel = Model();
    surfaceModel.addVertexFormatInfo(Model::TYPE_VEC2, Model::USAGE_POSITION);
    surfaceModel.addVertexFormatInfo(Model::TYPE_VEC2, Model::USAGE_TEXTURE);

    surfaceModel.addVec2f(Vec2f(-1, -1), 0); surfaceModel.addVec2f(Vec2f(0, 0), 1);
    surfaceModel.addVec2f(Vec2f(1, -1), 0); surfaceModel.addVec2f(Vec2f(1, 0), 1);
    surfaceModel.addVec2f(Vec2f(1, 1), 0); surfaceModel.addVec2f(Vec2f(1, 1), 1);

    surfaceModel.addVec2f(Vec2f(-1, -1), 0); surfaceModel.addVec2f(Vec2f(0, 0), 1);
    surfaceModel.addVec2f(Vec2f(1, 1), 0); surfaceModel.addVec2f(Vec2f(1, 1), 1);
    surfaceModel.addVec2f(Vec2f(-1, 1), 0); surfaceModel.addVec2f(Vec2f(0, 1), 1);

    GLModel surfGLModel = GLModel::convertModel(&surfaceModel);
    
    
    Model m = Model();
    m.loadModel("TestFiles/3DModels/box.obj");

    GLModel model = GLModel::convertModel(&m);
    Vec3f pos, rot;
    bool mouseLock = false;
    double angle = 0.0;
    // GLModel model = GLModel::generateTestModel();

    GLSurfaceParameters params = { GLSurface::COLOR_AND_DEPTH, 0, false, 0 };
    GLSurface surface = GLSurface(640, 480, params);

    GLGraphics::setClearColor(Vec4f(0.2, 0.3, 0.4, 1.0));
    GLGraphics::enableDepthTest();
    GLGraphics::setDepthTestFunction(GLGraphics::DEPTH_LESS);

    while(w.getRunning())
    {
        w.update();
        
        int midX, midY;

        surface.bind();
        GLGraphics::setClearColor(Vec4f(0.4, 0.3, 0.4, 1.0));
        GLGraphics::clear(GLGraphics::COLOR_BUFFER | GLGraphics::DEPTH_BUFFER);

        Mat4f proj = MathExt::perspectiveProjectionMatrix(320, 240, 0.1, 100, 60);
        Mat4f view = MathExt::viewMatrix(pos, rot);
        Mat4f modMat = Mat4f::getIdentity();

        midX = w.getX() + w.getWidth()/2;
        midY = w.getY() + w.getHeight()/2;

        Mat4f mat = Mat4f::getIdentity();
        s.setAsActive();
        s.setMat4("projectionMatrix", proj);
        s.setMat4("viewMatrix", view);
        s.setMat4("modelMatrix", modMat);

        texture.bind();

        model.draw();

        surface.bindTexture();

        surface.unbind();
        
        GLGraphics::setClearColor(Vec4f(0.2, 0.3, 0.4, 1.0));
        GLGraphics::clear(GLGraphics::COLOR_BUFFER | GLGraphics::DEPTH_BUFFER);
        surfaceShader.setAsActive();
        surface.bindTexture();
        surfGLModel.draw();

		w.swapBuffers();
        Input::pollInput();

        //mouse camera controls
        if(mouseLock)
        {
            int xDiff = midX - Input::getMouseX();
            int yDiff = midY - Input::getMouseY();

            rot.z += xDiff*0.01;
            rot.x += yDiff*0.01;
            System::setMousePosition(midX, midY);
        }

        if(Input::getKeyDown('A'))
        {
            Vec2f moveDir = MathExt::lengthDir(0.1, rot.z);
            pos.x += moveDir.x;
            pos.y += moveDir.y;
        }
        else if(Input::getKeyDown('D'))
        {
            Vec2f moveDir = MathExt::lengthDir(0.1, rot.z);
            pos.x -= moveDir.x;
            pos.y -= moveDir.y;
        }

        if(Input::getKeyDown('S'))
        {
            Vec2f moveDir = MathExt::lengthDir(0.1, rot.z+(PI/2));
            pos.x += moveDir.x;
            pos.y += moveDir.y;
        }
        else if(Input::getKeyDown('W'))
        {
            Vec2f moveDir = MathExt::lengthDir(0.1, rot.z+(PI/2));
            pos.x -= moveDir.x;
            pos.y -= moveDir.y;
        }

        if(Input::getKeyPressed(Input::KEY_LCONTROL))
        {
            mouseLock = !mouseLock;
            if(mouseLock)
                System::setMousePosition(midX, midY);
        }

        if(Input::getKeyDown('E'))
        {
            pos.z += 0.1;
        }
        else if(Input::getKeyDown('Q'))
        {
            pos.z -= 0.1;
        }

        if(Input::getKeyDown(Input::KEY_LEFT))
        {
            rot.z += 0.1;
        }
        else if(Input::getKeyDown(Input::KEY_RIGHT))
        {
            rot.z -= 0.1;
        }

        if(Input::getKeyDown(Input::KEY_UP))
        {
            rot.x += 0.1;
        }
        else if(Input::getKeyDown(Input::KEY_DOWN))
        {
            rot.x -= 0.1;
        }

        if(Input::getKeyDown(Input::KEY_NUMPAD_ADD))
        {
            rot.y += 0.1;
        }
        else if(Input::getKeyDown(Input::KEY_NUMPAD_SUBTRACT))
        {
            rot.y -= 0.1;
        }

        StringTools::print("Position (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);

        System::sleep(16);
        StringTools::eraseConsoleLine(false);
    }
}

void testDirectXStuff()
{
    DXWindow w = DXWindow("DirectX Window", 320, 240, -1, -1, DXWindow::NORMAL_WINDOW | DXWindow::TYPE_USER_MANAGED);

    DXShader s = DXShader("./testFiles/HLSL/testVert.cso", "./testFiles/HLSL/testFrag2.cso");
    DXModel model = DXModel();

    Vec3f colorArr = Vec3f(0.0, 0.0, 0.0);
    bool reverse = false;

    struct UniformData
    {
        float r;
        float g;
        float b;
        float a;
    };

    while(w.getRunning())
    {
        w.update();

        ID3D11RenderTargetView* renderTarget = DXSingleton::getBackBuffer();
        DXSingleton::getContext()->OMSetRenderTargets(1, &renderTarget, nullptr);

        w.clearWindow( Vec4f(0.0f, 0.2f, 0.4f, 1.0f) );

        s.setAsActive();
        // s.setVec3("inColor", colorArr);
        UniformData u = {(float)colorArr.x, (float)colorArr.y, (float)colorArr.z, 1.0f};
        s.setUniformData(&u, sizeof(UniformData), 0, DXShader::TYPE_FRAGMENT);

        model.draw();

		w.swapBuffers();

        //color logic
        if(!reverse)
        {
            if(colorArr.x < 1.0)
                colorArr.x += 0.05;
            else
            {
                if(colorArr.y < 1.0)
                    colorArr.y += 0.05;
                else
                {
                    if(colorArr.z < 1.0)
                        colorArr.z += 0.05;
                    else
                        reverse = !reverse;
                }
            }
        }
        else
        {
            if(colorArr.x > 0.0)
                colorArr.x -= 0.05;
            else
            {
                if(colorArr.y > 0.0)
                    colorArr.y -= 0.05;
                else
                {
                    if(colorArr.z > 0.0)
                        colorArr.z -= 0.05;
                    else
                        reverse = !reverse;
                }
            }
        }

        System::sleep(16);
    }
}

void testGuiStuff()
{
    SimpleWindow w = SimpleWindow("Gui Testing", 320, 240);

    std::vector<GuiInstance*> elements;

    GuiContextMenu m = GuiContextMenu();
    m.getListElement()->setElementSpacing(20);
    m.getListElement()->setExpectedWidth(52);

    for(int i=0; i<5; i++)
    {
        GuiTextBlock* b = new GuiTextBlock(0, 0, 48, 16);
        b->setText("Data " + std::to_string(i));

        GuiRectangleButton* but = new GuiRectangleButton(2, 2, 48, 16);
        but->addChild(b);
        but->setBackgroundColor({0, 0, 0, 0});
        but->setOutlineColor({0, 0, 0, 0});
        but->setFocusOutlineColor({0, 0, 0, 0});

        elements.push_back(b);
        elements.push_back(but);
        m.getListElement()->addElement(but);
    }

    w.getGuiManager()->addElement(&m);
    w.waitTillClose();

    for(int i=0; i<elements.size(); i++)
    {
        delete elements[i];
    }
}

class TestClass
{
public:
    TestClass()
    {

    }
    ~TestClass()
    {
        StringTools::println("Calling Destructor TestClass for %p", this);
        memLoc.unsafeRemove();
    }
private:
    SmartMemory<TestClass> memLoc = SmartMemory<TestClass>(this);
};

class Instance
{
public:
    Instance()
    {
    }

    ~Instance()
    {
        StringTools::println("Calling Destructor INSTANCE for %p", this);
        memLoc.unsafeRemove();
    }
private:
    SmartMemory<Instance> memLoc = SmartMemory<Instance>(this);
};

class Instance2
{
public:
    Instance2()
    {
    }

    ~Instance2()
    {
        StringTools::println("Calling Destructor INSTANCE2 for %p", this);
    }
};

void testDeleteStuff()
{
    // SmartMemory<Image> imgPointer = SmartMemory<Image>(new Image(320, 240));

    // imgPointer.getData()->setAllPixels({200, 100, 150, 240});

    // SmartMemory<Image> imgPointer2 = imgPointer;

    // SmartMemory<int> intPointer = SmartMemory<int>(new int[3]);

    // Instance ins = Instance();
    // SmartMemory<Instance> ins2 = SmartMemory<Instance>(new Instance(), true);
    
    // Instance* insP = new Instance();

    // SmartMemory<int> intPointer = SmartMemory<int>(new int[4]);

    // size_t t1,t2;
    // int* p;
    
    // t1 = System::getCurrentTimeNano();
    // p = intPointer.getRawPointer();
    // t2 = System::getCurrentTimeNano();

    // StringTools::println("%p, %d, TIME: %llu", &p[0], p[0], t2-t1);
    
    // std::shared_ptr<int> sharedP = std::shared_ptr<int>(new int[4]);
    // std::shared_ptr<int> sharedP2 = sharedP;

    // t1 = System::getCurrentTimeNano();
    // p = sharedP2.get();
    // t2 = System::getCurrentTimeNano();

    // StringTools::println("%p, %d, TIME: %llu", &p[0], p[0], t2-t1);

    SmartMemory<Instance> insArray = SmartMemory<Instance>(new Instance[4], true, true);
    insArray.~SmartMemory();

    // std::shared_ptr<Instance2> sharedP = std::shared_ptr<Instance2>(new Instance2[4]);
    // sharedP.reset();

    // Instance* ins = new Instance[20]();
    // StringTools::println("ARRAY %p", ins);
    // delete[] ins;

    //SUMMARY, Create and Delete performance within margin of error with no debugging stuff attached.
    //Should be less memory as only a boolean is added to determine if it can be deleted
    //performance is likely worse as it must search the list for the data first.


    // size_t t1, t2;
    // t1 = System::getCurrentTimeMicro();
    // std::unique_ptr<Image> imgPointer = std::unique_ptr<Image>(new Image(320, 240));
    // t2 = System::getCurrentTimeMicro();
    // StringTools::println("Time to create unique_ptr<Image> = %llu", t2-t1);

    
    // t1 = System::getCurrentTimeMicro();
    // imgPointer.reset();
    // t2 = System::getCurrentTimeMicro();
    // StringTools::println("Time to destroy unique_ptr<Image> = %llu", t2-t1);

    // t1 = System::getCurrentTimeMicro();
    // SmartMemory<Image> imgPointer2 = SmartMemory<Image>(new Image(320, 240));
    // t2 = System::getCurrentTimeMicro();
    // StringTools::println("Time to create SmartMemory<Image> = %llu", t2-t1);

    
    // t1 = System::getCurrentTimeMicro();
    // imgPointer2.~SmartMemory();
    // t2 = System::getCurrentTimeMicro();
    // StringTools::println("Time to destroy SmartMemory<Image> = %llu", t2-t1);
}

void demo()
{
    SimpleWindow w = SimpleWindow("BezierGon Demo", 640, 480);

    GuiCustomObject g = GuiCustomObject();
    BezierCurve b, b2, b3;

    Vec2f p1 = Vec2f(128, 128);
    Vec2f p2 = Vec2f(196, 128);
    Vec2f p3 = Vec2f(196, 196);

    int selection = -1;

    g.setUpdateFunction( [&b, &b2, &b3, &p1, &p2, &p3, &selection, &w]()->void{

        int mX, mY;
        mX = w.getMouseX();
        mY = w.getMouseY();
        Point2D mousePoint = Point2D(Vec2f(mX, mY));

        if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
        {
            StringTools::println("MousePressed");
            StringTools::println("%d,%d", mX, mY);
            Circle c = Circle(6);

            c.setPosition(p1);
            if(CollisionMaster::getCollision(&c, &mousePoint))
            {
                StringTools::println("Selection0");
                selection = 0;
            }
            else
            {
                c.setPosition(p2);
                if(CollisionMaster::getCollision(&c, &mousePoint))
                {
                    selection = 1;
                }
                else
                {
                    c.setPosition(p3);
                    if(CollisionMaster::getCollision(&c, &mousePoint))
                    {
                        selection = 2;
                    }
                    else
                    {
                        selection = -1;
                    }
                }
            }
        }

        if(Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
        {
            selection = -1;
        }

        if(selection>=0)
        {
            if(selection==0)
            {
                p1 = Vec2f(mX, mY);
            }
            else if(selection==1)
            {
                p2 = Vec2f(mX, mY);
            }
            else if(selection==2)
            {
                p3 = Vec2f(mX, mY);
            }
        }

        b = BezierCurve();
        b2 = BezierCurve();
        b3 = BezierCurve();

        b.addPoint(p1);
        b.addPoint(p2);
        b.addPoint(p3);

        Vec2f s1 = b.getPoint(1) - b.getPoint(0);
        Vec2f s2 = b.getPoint(2) - b.getPoint(1);
        Vec2f midS = s1+s2;

        s1 = MathExt::inverseVec(s1).normalize() * 10;
        s2 = MathExt::inverseVec(s2).normalize() * 10;
        midS = MathExt::inverseVec(midS).normalize() * 10;

        b2.addPoint(p1+s1);
        b2.addPoint(p2+midS);
        b2.addPoint(p3+s2);

        b3.addPoint(p1-s1);
        b3.addPoint(p2-midS);
        b3.addPoint(p3-s2);
    });

    g.setRenderFunction( [&b, &b2, &b3, &p1, &p2, &p3](Image* surf)->void{

        SimpleGraphics::setColor({0,0,0,255});
        SimpleGraphics::drawBezierCurve(b, 10, false, surf);

        SimpleGraphics::setColor({0,255,0,255});
        SimpleGraphics::drawBezierCurve(b2, 10, false, surf);

        SimpleGraphics::setColor({0,0,255,255});
        SimpleGraphics::drawBezierCurve(b3, 10, false, surf);

        SimpleGraphics::setColor({0, 0, 0, 255});
        SimpleGraphics::drawLine((int)b3.getFuctionAt(0).x, (int)b3.getFuctionAt(0).y, (int)b2.getFuctionAt(0).x, (int)b2.getFuctionAt(0).y, surf);
        SimpleGraphics::drawLine((int)b3.getFuctionAt(0.5).x, (int)b3.getFuctionAt(0.5).y, (int)b2.getFuctionAt(0.5).x, (int)b2.getFuctionAt(0.5).y, surf);
        SimpleGraphics::drawLine((int)b3.getFuctionAt(1).x, (int)b3.getFuctionAt(1).y, (int)b2.getFuctionAt(1).x, (int)b2.getFuctionAt(1).y, surf);

        SimpleGraphics::setColor({255, 255, 255, 255});
        SimpleGraphics::drawCircle((int)p1.x, (int)p1.y, 6, false, surf);
        SimpleGraphics::drawCircle((int)p2.x, (int)p2.y, 6, false, surf);
        SimpleGraphics::drawCircle((int)p3.x, (int)p3.y, 6, false, surf);

    });

    w.getGuiManager()->addElement(&g);
    w.waitTillClose();
}

void testLoadGui()
{
    GuiManager::initDefaultLoadFunctions();

    SimpleWindow w = SimpleWindow("GuiLoading", 320, 240);

    w.getGuiManager()->loadElementsFromFile("Test.xml");

    w.waitTillClose();
}

void testSavePNG()
{
    Sprite k;
    k.loadImage("C:/Users/Alan/Pictures/Screenshot 2021-12-08 201519.png");

    size_t t1,t2;
    t1 = System::getCurrentTimeNano();
    if(k.getSize()>0)
        k.getImage(0)->savePNG("testOutput.png", false, false, true);
    t2 = System::getCurrentTimeNano();

    StringTools::println("TIME TAKEN %llu", t2-t1);

    t1 = System::getCurrentTimeNano();
    if(k.getSize()>0)
        k.getImage(0)->saveGIF("testOutput.gif", 256, false, false);
    t2 = System::getCurrentTimeNano();

    StringTools::println("TIME TAKEN %llu", t2-t1);
}



// void testQuickHash()
// {
//     size_t t1,t2;
//     size_t totalTimeCHash = 0;
//     size_t totalTimeTHash = 0;

//     StringTools::println("std::unordered_multimap<std::string, std::string>");
//     t1 = System::getCurrentTimeNano();
//     std::unordered_map<std::string, std::string> map1 = std::unordered_map<std::string, std::string>(16384);
//     // map1.max_load_factor(1);
//     t2 = System::getCurrentTimeNano();

//     StringTools::println("\tCREATE TIME: %llu + %llu", t2-t1, map1.bucket_count());
//     totalTimeCHash += t2-t1;

//     t1 = System::getCurrentTimeNano();
//     for(int i=0; i<1000000; i++)
//     {
//         std::pair<std::string, std::string> pair = {std::to_string(i%16384), std::to_string(i)};
//         map1.insert( pair );
//     }
//     t2 = System::getCurrentTimeNano();

//     StringTools::println("\tINSERT TIME: %llu", t2-t1);
//     totalTimeCHash += t2-t1;

//     t1 = System::getCurrentTimeNano();

//     auto it = map1.find(std::to_string(4024));

//     t2 = System::getCurrentTimeNano();

//     StringTools::println("\tFIND TIME: %llu", t2-t1);
//     totalTimeCHash += t2-t1;


//     t1 = System::getCurrentTimeNano();
//     map1.erase(std::to_string(4024));
//     t2 = System::getCurrentTimeNano();

//     StringTools::println("\tDELETE TIME: %llu", t2-t1);
//     totalTimeCHash += t2-t1;

//     t1 = System::getCurrentTimeNano();
//     map1.rehash( map1.bucket_count()*2 );
//     t2 = System::getCurrentTimeNano();
//     StringTools::println("\tREHASH TIME: %llu | Bucket Size: %llu", t2-t1, map1.bucket_count());
//     totalTimeCHash += t2-t1;

//     t1 = System::getCurrentTimeNano();
//     map1.clear();
//     t2 = System::getCurrentTimeNano();
//     StringTools::println("\tCLEAR TIME: %llu", t2-t1);
//     totalTimeCHash += t2-t1;

    
//     StringTools::println("\tTOTAL TIME: %llu", totalTimeCHash);



//     StringTools::println("TestHash<std::string, std::string>");
//     t1 = System::getCurrentTimeNano();
//     TestHash<std::string, std::string> map2(TestHash<std::string, std::string>::MODE_UNIQUE_KEY);
//     map2.setMaxLoadFactor(-1);
//     t2 = System::getCurrentTimeNano();

//     StringTools::println("\tCREATE TIME: %llu", t2-t1);
//     totalTimeTHash += t2-t1;

//     t1 = System::getCurrentTimeNano();
//     for(int i=0; i<1000000; i++)
//     {
//         map2.addElement( std::to_string(i%16384), std::to_string(i) );
//     }
//     t2 = System::getCurrentTimeNano();

//     StringTools::println("\tINSERT TIME: %llu", t2-t1);
//     totalTimeTHash += t2-t1;

//     t1 = System::getCurrentTimeNano();

//     std::vector<TestHashPair<std::string, std::string>> collection2 = map2.getElement(std::to_string(4024));

//     t2 = System::getCurrentTimeNano();

//     StringTools::println("\tCOLLECTION FIND TIME: %llu", t2-t1);
//     totalTimeTHash += t2-t1;


//     t1 = System::getCurrentTimeNano();
//     map2.removeFirstElement(std::to_string(4024));
//     t2 = System::getCurrentTimeNano();

//     StringTools::println("\tDELETE TIME: %llu", t2-t1);
//     totalTimeTHash += t2-t1;

//     t1 = System::getCurrentTimeNano();
//     map2.rehash();
//     t2 = System::getCurrentTimeNano();
//     StringTools::println("\tREHASH TIME: %llu | Bucket Size: %llu", t2-t1, map2.amountOfBuckets());
//     totalTimeTHash += t2-t1;

//     t1 = System::getCurrentTimeNano();
//     map2.clear();
//     t2 = System::getCurrentTimeNano();
//     StringTools::println("\tCLEAR TIME: %llu", t2-t1);
//     totalTimeTHash += t2-t1;
    
//     StringTools::println("\tTOTAL TIME: %llu", totalTimeTHash);
//     StringTools::println("TOTAL TIME SAVE: %llu", totalTimeCHash-totalTimeTHash);
// }

int main(int argc, char** argv)
{
    StringTools::init();

    SimpleGraphics::init();

    // testLoadGui();
    testSavePNG();
    // testQuickHash();
    
    return 0;
}
