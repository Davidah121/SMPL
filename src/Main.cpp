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

#include "ModelBuilder.h"

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

void testModelBuilder()
{
    GLWindow w = GLWindow("OpenGL Window", 640, 480, -1, -1, GLWindow::NORMAL_WINDOW | GLWindow::TYPE_USER_MANAGED);
    
    Model* m = ModelBuilder::createSphere(1, 10);
    GLModel gm = GLModel::convertModel(m);

    GLGraphics::init();
    GLGraphics::setClearColor(Vec4f(0.2, 0.3, 0.4, 1.0));
    GLGraphics::enableDepthTest();
    GLGraphics::setDepthTestFunction(GLGraphics::DEPTH_LESS);
    // GLGraphics::disableFaceCulling();
    GLGraphics::enableFaceCulling();
    GLGraphics::setFaceCullingType(GLGraphics::CULL_BACK);
    GLGraphics::setOrthoProjection(640, 480);

    GLFont glFont = GLFont("Resources/DefaultFont.fnt");

    GLGraphics::setFont(&glFont);

    GLShader s = GLShader("Resources/glsl/vs/3dVertShader.vs", "Resources/glsl/fs/3dFragShader.fs");

    Vec3f pos, rot, lightPos;
    float dirToLight;
    bool mouseLock = false;
    lightPos = Vec3f(0, -10, 10);

    while(w.getRunning())
    {
        w.update();
        Input::pollInput();


        GLGraphics::clear(GLGraphics::COLOR_BUFFER | GLGraphics::DEPTH_BUFFER);

        s.setAsActive();
        Mat4f proj = MathExt::perspectiveProjectionMatrix(640, 480, 0.1, 100, 60);

        Vec3f toPos = MathExt::sphericalCoord(1, rot.z, rot.x)+pos;
        // Mat4f view = MathExt::lookAtMatrix(pos, toPos, Vec3f(0,0,1));
        Mat4f view = MathExt::viewMatrix(pos, rot, true);

        // lightPos.setValues( MathExt::lengthDir(10, dirToLight));
        dirToLight += PI/60;

        s.setMat4("projectionMatrix", proj);
        s.setMat4("viewMatrix", view);
        s.setVec3("lightPos", lightPos);
        // s.setVec3("inColor", Vec3f(1,0,0));
        
        gm.draw();
        s.deactivateCurrentShader();

        GLGraphics::drawText( StringTools::formatString("(%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z), 0, 0);
        // GLGraphics::drawRectangle(0, 0, 32, 32, false);
        w.swapBuffers();

        
        if(Input::getKeyDown('W'))
        {
            Vec2f temp = MathExt::lengthDir(0.2, rot.z, true);
            pos.x+=temp.x;
            pos.y+=temp.y;
        }
        else if(Input::getKeyDown('S'))
        {
            Vec2f temp = MathExt::lengthDir(0.2, rot.z + PI, true);
            pos.x+=temp.x;
            pos.y+=temp.y;
        }

        if(Input::getKeyDown('A'))
        {
            Vec2f temp = MathExt::lengthDir(0.2, rot.z - PI/2, true);
            pos.x+=temp.x;
            pos.y+=temp.y;
        }
        else if(Input::getKeyDown('D'))
        {
            Vec2f temp = MathExt::lengthDir(0.2, rot.z + PI/2, true);
            pos.x+=temp.x;
            pos.y+=temp.y;
        }

        if(Input::getKeyDown('E'))
        {
            pos.z+=0.1;
        }
        else if(Input::getKeyDown('Q'))
        {
            pos.z-=0.1;
        }

        if(mouseLock)
        {
            Vec2f mouseDisToCenter;
            mouseDisToCenter.x = Input::getMouseX() - (w.getX() + w.getWidth()/2);
            mouseDisToCenter.y = Input::getMouseY() - (w.getY() + w.getHeight()/2);
            
            rot.z += mouseDisToCenter.x/180;
            rot.y += mouseDisToCenter.y/180;

            System::setMousePosition(w.getX() + w.getWidth()/2, w.getY() + w.getHeight()/2);
        }

        if(Input::getKeyPressed(Input::KEY_CONTROL))
        {
            mouseLock = !mouseLock;

            if(mouseLock)
                System::setMousePosition(w.getX() + w.getWidth()/2, w.getY() + w.getHeight()/2);
        }

        if(Input::getKeyDown(Input::KEY_LEFT))
        {
            rot.z-=PI/180;
        }
        else if(Input::getKeyDown(Input::KEY_RIGHT))
        {
            rot.z+=PI/180;
        }

        if(Input::getKeyDown(Input::KEY_UP))
        {
            rot.y-=PI/180;
        }
        else if(Input::getKeyDown(Input::KEY_DOWN))
        {
            rot.y+=PI/180;
        }

        if(Input::getKeyDown(Input::KEY_MINUS))
        {
            rot.x-=PI/180;
        }
        else if(Input::getKeyDown(Input::KEY_PLUS))
        {
            rot.x+=PI/180;
        }
        System::sleep(16);
    }

    delete m;
}

void testCompression()
{
    StringTools::println("Enter file to compress:");
    std::string filename = StringTools::getString();

    SimpleFile f = SimpleFile(filename, SimpleFile::READ);
    if(f.isOpen())
    {
        std::vector<unsigned char> fileBytes = f.readFullFileAsBytes();
        f.close();

        BinaryTree<HuffmanNode> huffTree;

        std::vector<unsigned char> deflateData = Compression::compressDeflate(fileBytes, 128, 7, true);
        std::vector<unsigned char> lzwData = Compression::compressLZW(fileBytes, 8);
        std::vector<unsigned char> lzssData = Compression::compressLZSS(fileBytes);
        std::vector<unsigned char> huffmanData = Compression::compressHuffman(fileBytes, &huffTree);

        StringTools::println("Original Size: %llu", fileBytes.size());
        StringTools::println("Deflate Size + %%: %llu, %f", deflateData.size(), (double)deflateData.size() / fileBytes.size());
        StringTools::println("LZW Size + %%: %llu, %f", lzwData.size(), (double)lzwData.size() / fileBytes.size());
        StringTools::println("LZSS Size + %%: %llu, %f", lzssData.size(), (double)lzssData.size() / fileBytes.size());
        StringTools::println("Huffman Only Size + %%: %llu, %f", huffmanData.size(), (double)huffmanData.size() / fileBytes.size());
        
    }
    else
    {
        StringTools::println("COULDN'T OPEN FILE");
    }
}

void neuralTest()
{
    //Setup neural network
    NeuralNetwork n = NeuralNetwork();
    // n.importNetwork("TestNetwork.xml");
    n.addLayerToEnd(2);
    n.addLayerToEnd(2);
    n.addLayerToEnd(2);
    n.resetNetwork();
    n.setLearningRate(0.3);

    std::vector<Vec2f> testDataPoints;
    for(int i=0; i<=4; i++)
    {
        double length = (double)i*1.5;
        for(int j=0; j<20; j++)
        {
            double dir = j*2.0*PI / 48.0;
            testDataPoints.push_back( MathExt::lengthDir(length, dir) );
        }
    }

    Sort::shuffle(testDataPoints.data(), testDataPoints.size());

    int testDataIndex = 0;
    //train the neural network on all inputs
    while(true)
    {
        StringTools::println("T for train, C for cost, R for run, V for view, E for exit.");
        char input = StringTools::getChar();

        if(input == 'T')
        {
            for(int it=0; it<1000; it++)
            {
                //collect 10 inputs
                std::vector<std::vector<double>> collectionOfInputs = std::vector<std::vector<double>>();
                std::vector<std::vector<double>> collectionOfOutputs = std::vector<std::vector<double>>();
                
                int i = 0;
                while(i < 10)
                {
                    std::vector<double> subInputs;
                    subInputs.push_back( MathExt::logisticsSigmoid( testDataPoints[testDataIndex+i].x ) );
                    subInputs.push_back( MathExt::logisticsSigmoid( testDataPoints[testDataIndex+i].y ) );

                    collectionOfInputs.push_back(subInputs);
                    collectionOfOutputs.push_back( { (testDataPoints[testDataIndex+i].getLength() <= 2) ? 1.0 : 0.0, 0.0} );

                    i++;
                    testDataIndex = (testDataIndex+1) % testDataPoints.size();
                }

                n.testTrain( collectionOfInputs, collectionOfOutputs);
            }
        }
        else if(input == 'C')
        {
            std::vector<std::vector<double>> collectionOfInputs = std::vector<std::vector<double>>();
            std::vector<std::vector<double>> collectionOfOutputs = std::vector<std::vector<double>>();
            
            int i = 0;
            while(i < testDataPoints.size())
            {
                std::vector<double> subInputs;
                subInputs.push_back( MathExt::logisticsSigmoid( testDataPoints[testDataIndex+i].x ) );
                subInputs.push_back( MathExt::logisticsSigmoid( testDataPoints[testDataIndex+i].y ) );

                collectionOfInputs.push_back(subInputs);
                collectionOfOutputs.push_back( { (testDataPoints[i].getLength() <= 2) ? 1.0 : 0.0, 0.0} );

                i++;
            }

            double totalCost = 0;
            std::vector<double> costValues = n.getCost( collectionOfInputs, collectionOfOutputs);
            for(int k=0; k<costValues.size(); k++)
            {
                // StringTools::println("Cost for [%.3f, %.3f] = %f", collectionOfInputs[k][0], collectionOfInputs[k][1], costValues[k]);
                totalCost += costValues[k];
            }

            StringTools::println("Total cost over all samples = %f", totalCost);
            StringTools::println("Average cost over all samples = %f", totalCost/costValues.size());
        }
        else if(input == 'R')
        {
            double v1 = std::stod(StringTools::getString());
            double v2 = std::stod(StringTools::getString());

            std::vector<double> inputs = {v1/6, v2/6};
            std::vector<double> outputs = n.run(inputs);

            StringTools::println("Inputs [%.3f, %.3f] = Outputs [%.3f]", v1, v2, outputs[0]);
        }
        else if(input == 'V')
        {
            double w11 = n.getStartLayer()->getNeuron(0).getWeight(0);
            double w21 = n.getStartLayer()->getNeuron(1).getWeight(0);
            
            double w12 = n.getStartLayer()->getNeuron(0).getWeight(1);
            double w22 = n.getStartLayer()->getNeuron(1).getWeight(1);

            double w13 = n.getStartLayer()->getNeuron(0).getWeight(2);
            double w23 = n.getStartLayer()->getNeuron(1).getWeight(2);

            StringTools::println("W11 = %f", w11);
            StringTools::println("W21 = %f", w21);
            StringTools::println("W12 = %f", w12);
            StringTools::println("W22 = %f", w22);
            StringTools::println("W13 = %f", w13);
            StringTools::println("W23 = %f", w23);
        }
        else if(input == 'E')
        {
            break;
        }
    }

    n.exportTestInformation("After2.xml");
}

void neuralVisualizer()
{
    SimpleWindow window = SimpleWindow("Neural Visualizer", 640, 480);

    bool changed = true;
    srand(time(NULL));

    NeuralNetwork network = NeuralNetwork();
    network.addLayerToEnd(2);
    network.addLayerToEnd(3);
    network.addLayerToEnd(1);
    network.resetNetwork();
    network.setLearningRate(1);

    std::vector<std::vector<Image>> networkNeuronGradient = std::vector<std::vector<Image>>(3);
    networkNeuronGradient[0] = std::vector<Image>(2);
    networkNeuronGradient[1] = std::vector<Image>(3);
    networkNeuronGradient[2] = std::vector<Image>(1);

    for(int i=0; i<networkNeuronGradient.size(); i++)
    {
        for(int j=0; j<networkNeuronGradient[i].size(); j++)
        {
            networkNeuronGradient[i][j] = Image(64, 64);
        }
    }
    

    GuiManager::initDefaultLoadFunctions();
    window.getGuiManager()->loadElementsFromFile("GuiStuff/base.xml");

    //set resetButton function
    std::vector<GuiInstance*> elements = window.getGuiManager()->getElements();

    if(elements.size() > 0)
    {
        GuiRectangleButton* resetButton = (GuiRectangleButton*)elements[1];
        resetButton->setOnClickReleaseFunction( [&network, &changed](GuiInstance* ins) -> void{
            network.resetNetwork();
            changed = true; 
        });

        GuiRectangleButton* trainButton = (GuiRectangleButton*)elements[3];
        trainButton->setOnClickReleaseFunction( [&network, &changed](GuiInstance* ins) -> void{
            
            std::vector<std::vector<double>> inputs = std::vector<std::vector<double>>();
            std::vector<std::vector<double>> outputs = std::vector<std::vector<double>>();

            double dir = MathExt::toRad((double)(rand()%360));
            double length = (double)(rand()%6);

            Vec2f v = MathExt::lengthDir(length, dir);
            inputs.push_back( { MathExt::logisticsSigmoid(v.x), MathExt::logisticsSigmoid(v.y)} );
            outputs.push_back( { (v.getLength() <= 2) ? 1.0 : 0.0} );
            
            network.testTrain(inputs, outputs);
            changed = true; 
        });

        GuiRectangleButton* trainButton2 = (GuiRectangleButton*)elements[5];
        trainButton2->setOnClickHoldFunction( [&network, &changed](GuiInstance* ins) -> void{
            
            std::vector<std::vector<double>> inputs = std::vector<std::vector<double>>();
            std::vector<std::vector<double>> outputs = std::vector<std::vector<double>>();

            double dir = MathExt::toRad((double)(rand()%360));
            double length = (double)(rand()%6);

            Vec2f v = MathExt::lengthDir(length, dir);
            inputs.push_back( { MathExt::logisticsSigmoid(v.x), MathExt::logisticsSigmoid(v.y)} );
            outputs.push_back( { (v.getLength() <= 2) ? 1.0 : 0.0} );
            
            network.testTrain(inputs, outputs);
            changed = true; 
        });

        GuiRectangleButton* costButton = (GuiRectangleButton*)elements[7];
        costButton->setOnClickReleaseFunction( [&network](GuiInstance* ins) -> void{

            std::vector<std::vector<double>> inputs = std::vector<std::vector<double>>();
            std::vector<std::vector<double>> outputs = std::vector<std::vector<double>>();

            double dir = MathExt::toRad((double)(rand()%360));
            double length = (double)(rand()%6);

            Vec2f v = MathExt::lengthDir(length, dir);
            inputs.push_back( { MathExt::logisticsSigmoid(v.x), MathExt::logisticsSigmoid(v.y)} );
            outputs.push_back( { (v.getLength() <= 2) ? 1.0 : 0.0} );
            std::vector<double> costs = network.getCost(inputs, outputs);
            StringTools::println("COST for %f,%f = %f", inputs[0][0], inputs[0][1], costs[0]);
        });
    }


    GuiCustomObject neuralGuiObj = GuiCustomObject();
    neuralGuiObj.setUpdateFunction( [&network, &networkNeuronGradient, &changed]() -> void{

        if(!changed)
            return;
        
        NeuralLayer* l = network.getStartLayer();

        int index = 0;
        while(l != nullptr)
        {
            if(l == network.getStartLayer())
            {
                std::vector<Neuron> neurons = l->getListOfNeurons();
                for(int i=0; i<neurons.size(); i++)
                {
                    if(i == 0)
                    {
                        //x values
                        for(int y=0; y<networkNeuronGradient[index][i].getHeight(); y++)
                        {
                            for(int x=0; x<networkNeuronGradient[index][i].getWidth(); x++)
                            {
                                Color c = {0,0,0,255};
                                c.red = (unsigned char)(255 * MathExt::logisticsSigmoid( x-(networkNeuronGradient[index][i].getWidth()/2) ));
                                c.green = c.red;
                                c.blue = c.red;
                                networkNeuronGradient[index][i].setPixel(x, y, c);
                            }
                        }
                    }
                    else if(i == 1)
                    {
                        //y values
                        for(int y=0; y<networkNeuronGradient[index][i].getHeight(); y++)
                        {
                            Color c = {0,0,0,255};
                            c.red = (unsigned char)(255 * MathExt::logisticsSigmoid( y-(networkNeuronGradient[index][i].getHeight()/2) ));
                            c.green = c.red;
                            c.blue = c.red;
                            for(int x=0; x<networkNeuronGradient[index][i].getWidth(); x++)
                            {
                                networkNeuronGradient[index][i].setPixel(x, y, c);
                            }
                        }
                    }
                }
            }
            else
            {
                std::vector<Neuron> neurons = l->getListOfNeurons();
                std::vector<Neuron> preNeurons = l->getPreviousLayer()->getListOfNeurons();
                
                for(int i=0; i<neurons.size(); i++)
                {
                    for(int y=0; y<networkNeuronGradient[index][i].getHeight(); y++)
                    {
                        for(int x=0; x<networkNeuronGradient[index][i].getWidth(); x++)
                        {
                            Color c = {0, 0, 0, 255};
                            double res = 0;
                            for(int j=0; j<preNeurons.size(); j++)
                            {
                                double tempIntensity = (double)networkNeuronGradient[index-1][j].getPixel(x,y).red / 255.0;
                                res += tempIntensity*preNeurons[j].getWeight(i);
                            }
                            
                            c.red = (unsigned char)(255 * MathExt::logisticsSigmoid(res + l->getPreviousLayer()->getBiasValue(i)));
                            c.green = c.red;
                            c.blue = c.red;

                            networkNeuronGradient[index][i].setPixel(x, y, c);
                        }
                    }
                }
            }

            index++;
            l = l->getNextLayer();
        }

        changed = false;
    });

    neuralGuiObj.setRenderFunction( [&network, &networkNeuronGradient](Image* surf) -> void{
        
        //draw gradients
        for(int i=0; i<networkNeuronGradient.size(); i++)
        {
            int xOffset = 128+(i*128);
            for(int j=0; j<networkNeuronGradient[i].size(); j++)
            {
                int yOffset = 96+(j*96);

                SimpleGraphics::drawImage(&networkNeuronGradient[i][j], xOffset, yOffset, surf);
            }
        }

        //draw connections from first layer to second
        
        std::vector<Neuron> neurons = network.getStartLayer()->getListOfNeurons();
        int xOffset = 128;
        for(int j=0; j<neurons.size(); j++)
        {
            int yOffset = 96+(j*96);

            for(int k=0; k<neurons[j].size(); k++)
            {
                Color c;
                int yOffsetOther = 96+(k*96);
                if(neurons[j][k] > 0)
                {
                    c.red = neurons[j][k]*32;
                    c.alpha = 255;
                }
                else
                {
                    c.blue = neurons[j][k]*32;
                    c.alpha = 255;
                }

                SimpleGraphics::setColor(c);
                SimpleGraphics::drawLine(xOffset+32, yOffset+32, xOffset+32+128, 32+yOffsetOther, surf);
            }
        }
        
        neurons = network.getStartLayer()->getNextLayer()->getListOfNeurons();
        xOffset = 256;
        for(int j=0; j<neurons.size(); j++)
        {
            int yOffset = 96+(j*96);

            for(int k=0; k<neurons[j].size(); k++)
            {
                Color c;
                int yOffsetOther = 96+(k*96);
                if(neurons[j][k] > 0)
                {
                    c.red = neurons[j][k]*32;
                    c.alpha = 255;
                }
                else
                {
                    c.blue = neurons[j][k]*32;
                    c.alpha = 255;
                }

                SimpleGraphics::setColor(c);
                SimpleGraphics::drawLine(xOffset+32, yOffset+32, xOffset+32+128, 32+yOffsetOther, surf);
            }
        }
    });

    window.getGuiManager()->addElement(&neuralGuiObj);

    window.waitTillClose();
}

int main(int argc, char** argv)
{
    StringTools::init();

    SimpleGraphics::init();

    // testLoadGui();

    //Adjust compression algorithms to adjust pointer to output instead of returning a new list and copying it.
    // testSavePNG();

    // neuralTest();
    neuralVisualizer();

    // testQuickHash();
    // testModelBuilder();
    // testGLStuff();

    // testCompression();
    
    return 0;
}
