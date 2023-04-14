#include "StringTools.h"
#include "System.h"
#include "SimpleWindow.h"
#include "SimpleDir.h"
#include "SpatialHashing.h"
#include "IniFile.h"
#include "VectorFont.h"
#include "BitmapFont.h"
#include "Network.h"
#include "Input.h"
#include <stdlib.h>
#include <atomic>
#include "ResourceManager.h"

#include "BinarySearchTree.h"
#include "Compression.h"
#include "SimpleJSON.h"

using namespace glib;


void initFunction(SimpleWindow* w)
{
    w->getGuiManager()->loadElementsFromFile("GuiStuff/layout_old.xml");
    w->setWindowAsInputFocus();
}

int testWindow()
{
    StringTools::init();
    SimpleGraphics::init();
    GuiManager::initDefaultLoadFunctions();

    WindowOptions options;
    options.threadManaged = false;
    options.iconFileString = "icon.ico";
    options.initFunction = initFunction;

    SimpleWindow w = SimpleWindow("TITLE", 1280, 720, -1, -1, options);
    w.getGuiManager()->setExpectedSize(Vec2f(1280, 720));
    w.waitTillClose();
    return 0;
}

void testVectorGraphic()
{
    VectorGraphic g1 = VectorGraphic();
    // g1.load("testSVG.svg");
    // g1.load("testFiles/SvgFiles/Five Pointed Star.svg");
    g1.load("SVGs/_ionicons_svg_md-call-tan.svg");
    VectorGraphic g2 = VectorGraphic();
    g2.load("SVGs/_ionicons_svg_md-call.svg");
    VectorGraphic g3 = VectorGraphic();
    g3.load("SVGs/_ionicons_svg_md-mail.svg");
    VectorGraphic g4 = VectorGraphic();
    g4.load("SVGs/_ionicons_svg_md-pin.svg");
    // VectorGraphic g5 = VectorGraphic();
    // g4.load("SVGs/_ionicons_svg_md-pin.svg");
    VectorGraphic g5 = VectorGraphic();
    g5.load("test.svg");
    
    Image buffer = Image(512, 512);
    buffer.setAllPixels({0,0,0,0});
    g1.draw(&buffer);
    buffer.savePNG("Test1.png");
    
    buffer.setAllPixels({0,0,0,0});
    g2.draw(&buffer);
    buffer.savePNG("Test2.png");
    
    buffer.setAllPixels({0,0,0,0});
    g3.draw(&buffer);
    buffer.savePNG("Test3.png");
    
    buffer.setAllPixels({0,0,0,0});
    g4.draw(&buffer);
    buffer.savePNG("Test4.png");
    
    buffer.setAllPixels({0,0,0,0});
    g5.setTransform(MathExt::scale2D(0.25, 0.25));
    g5.draw(&buffer);
    buffer.savePNG("Test5.png");

}

void testVectorFont()
{
    VectorFont f = VectorFont();
    f.load("SVGFonts/AnyConv.com__consolab.svg");
    StringTools::println("Loaded %llu characters", f.getListOfFontCharInfo().size());

    Image buffer = Image(f.getGraphic(0)->getWidth()/2, f.getGraphic(0)->getHeight()/2);
    
    for(int i='A'; i<='z'; i++)
    {
        int index = f.getCharIndex(i);
        if(index >= 0)
        {
            VectorGraphic* grap = f.getGraphic(index);
            if(grap != nullptr)
            {
                grap->setTransform(MathExt::scale2D(0.5, 0.5));
                SimpleGraphics::setColor({255,255,255,255});
                buffer.clearImage();
                grap->draw(&buffer);

                buffer.savePNG("TestOutput/image" + StringTools::toString(i) + ".png");
            }
        }
    }
}

void testFontDrawing()
{
    SimpleGraphics::init();
    glib::BitmapFont bitmapFont = glib::BitmapFont("SVGFonts/arialBLK_1.fnt");
    SimpleGraphics::setFont(&bitmapFont);
    // glib::Font* bitmapFont = SimpleGraphics::getDefaultFont(0);

    FontCharInfo bfci = bitmapFont.getFontCharInfo( bitmapFont.getCharIndex('T') );
    StringTools::println("FontInfo: %d, %d, %d, %d, %d", bfci.width, bfci.height, bfci.horizAdv, bfci.xOffset, bfci.yOffset);
    
    Image img = Image(128, 128);
    SimpleGraphics::drawText("This is some text", 0, 0, &img);
    img.savePNG("TestImg.png");

    VectorFont vF = VectorFont();
    vF.load("SVGFonts/ARIBLK.svg");
    vF.setFontSize(12);
    
    // StringTools::println("VectorFontSize: %d", vF.getFontSize());
    Image img2 = Image(128, 128);
    SimpleGraphics::setFont(&vF);
    SimpleGraphics::drawText("This is some text", 0, 0, &img2);
    img2.savePNG("TestImg2.png");

    FontCharInfo fci = vF.getFontCharInfo( vF.getCharIndex('T') );
    double scaleV = (double)vF.getFontSize() / vF.getOriginalFontSize();
    StringTools::println("FontInfo2: %d, %d, %d, %d, %d", fci.width, fci.height, fci.horizAdv, fci.xOffset, fci.yOffset);
    StringTools::println("FontInfo3: %.3f, %.3f, %.3f, %.3f, %.3f", fci.width/scaleV, fci.height/scaleV, fci.horizAdv/scaleV, fci.xOffset/scaleV, fci.yOffset/scaleV);
    
}

void testOTFLoading()
{
    VectorFont f = VectorFont();
    f.load("C:/Windows/Fonts/calibri.ttf");
}

void testCompression()
{
    Sprite img = Sprite();
    img.loadImage("testOutput.png");
    
    if(img.getSize() > 0)
    {
        // img.getImage(0)->saveGIF("TestSave.gif", 256, false, false);
        img.getImage(0)->savePNG("TestSave.png", false, false, true);
    }
}

void printNode(int tabs, bool side, BinaryTreeNode<RBNode<int>>* n)
{
    std::string tabStr = "";
    for(int i=0; i<tabs; i++)
    {
        tabStr += '\t';
    }
    if(n!=nullptr)
    {
        StringTools::println("%s %s %c - %d \033[0m", tabStr.c_str(), (n->data.color == true) ? "\033[31m" : "", (side) ? 'R' : 'L', n->data.data);
        printNode(tabs+1, true, n->rightChild);
        printNode(tabs+1, false, n->leftChild);
    }
    else
    {
        // StringTools::println("%s %c - NIL", tabStr.c_str(), (side) ? 'R' : 'L');
    }
}

void printTree()
{
    BinarySearchTree<int> values = BinarySearchTree<int>(BinarySearchTree<int>::RED_BLACK);
    //insert 11, 2, 14, 1, 7, 15, 5, 8
    values.add(11);
    values.add(2);
    values.add(14);
    values.add(1);
    values.add(7);
    values.add(15);
    values.add(5);
    values.add(8);
    values.add(4);
    StringTools::println("---------------");
    printNode(0, true, values.getTree()->getRoot());
    
    
    values.remove(5);
    StringTools::println("---------------");
    printNode(0, true, values.getTree()->getRoot());
    
    values.remove(15);
    StringTools::println("---------------");
    printNode(0, true, values.getTree()->getRoot());
    
    values.remove(7);
    StringTools::println("---------------");
    printNode(0, true, values.getTree()->getRoot());
}


void testAlphaWindow()
{
    WindowOptions options;
    // options.windowType = SimpleWindow::BORDERLESS_WINDOW;
    options.windowType = SimpleWindow::TRANSPARENT_WINDOW;
    options.threadManaged = false;

    Image img = Image(320, 240);
    for(int y=0; y<240; y++)
    {
        for(int x=0; x<320; x++)
        {
            img.setPixel(x, y, {255, 0, 0, (unsigned char)(255-y)});
        }
    }

    SimpleWindow w = SimpleWindow("Test", 320, 240, 32, 32, options);
    w.setActivateGui(false);
    w.setPaintFunction([&w, &img]()->void{
        w.drawImage(&img);
    });

    while(w.getRunning())
    {
        Input::pollInput();

        w.setX( Input::getMouseX()-(w.getWidth()/2) );
        w.setY( Input::getMouseY()-(w.getHeight()/2) );
        
        w.update();
        w.repaint();

        if(Input::getKeyDown(Input::KEY_ESCAPE) && Input::getMouseDown(Input::RIGHT_MOUSE_BUTTON))
        {
            break;
        }
        Sleep(10);
    }
    // w.waitTillClose();
}

// #ifndef USE_OPENGL
//     #define USE_OPENGL
// #endif

// #include "ext/GLGraphics.h"

// void testOGLWindow()
// {
//     GLWindow win = GLWindow();

//     while(win.getRunning())
//     {
//         win.update();
//         win.guiUpdate();
//         GLGraphics::setClearColor(Vec4f(1,1,1,1));
//         GLGraphics::clear(GLGraphics::COLOR_BUFFER);

//         GLGraphics::setDrawColor(Vec4f(0,0,0,1));
//         GLGraphics::drawEllipse(64.0, 64.0, 32.0, 24.0, false);
        
//         GLGraphics::setDrawColor(Vec4f(1,0,0,1));
//         GLGraphics::drawEllipse(128.0, 128.0, 24.0, 32.0, true);

//         GLGraphics::drawEllipse(192.0, 192.0, Vec2f(16, 16), Vec2f(8, 24), 32);

//         win.swapBuffers();

//         System::sleep(33);
//         // win.repaint();
//     }
// }


void quickTest()
{
    const std::string rawJSON = R"R0N0(
		{
			"index1": "data",
			"index2": "data2",
			"object": {
				"objVar1": "data",
				"objVar2": 1
			},
			"array": [
				"v1",
				"v2",
				"v3"
			],
			[
				"pair1": "data1",
				"pair2": "data2"
			],
			{
				"pairO1": "data01",
				"pair02": "dataO2"
			}
		}
	)R0N0";
    glib::SimpleJSON json = glib::SimpleJSON();
    json.load((unsigned char*)rawJSON.data(), rawJSON.size());

    std::vector<std::string> searches = {""};
    std::vector<glib::JNode*> result = json.getNodesPattern(searches);

    for(int i=0; i<result.size(); i++)
    {
        StringTools::println("NAME: %s", result[i]->getName().c_str());
    }
}
// int WinMain(HINSTANCE hins, HINSTANCE preIns, LPSTR cmdline, int nShowCMD)
int main(int argc, char** argv)
{
    quickTest();
    // testOGLWindow();
    // testOTFLoading();
    // testVectorGraphic();
    return 0;
}
