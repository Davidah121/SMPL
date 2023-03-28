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
#include "SuffixArray.h"

#include "ext/TouchSimulator.h"

// #include <zlib/zlib.h>

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

void testAddressSanitizer()
{
    int* p = new int[2];
    p[0] = 1;
    p[1] = 2;

    delete[] p;
    int a = p[0];
    int b = p[1];
    StringTools::println("%d, %d", a, b);
}

void testDialogBox()
{
	glib::FileFilter xmlFileFilter = {"XML", ".xml"};

	glib::File f = glib::System::fileDialogBox( glib::System::TYPE_OPEN_FILE, {xmlFileFilter} );
}

void testPerformanceCounters()
{
    StringTools::println("%.3f", System::getTotalCpuUsage());
    System::sleep(1000);
    StringTools::println("%.3f", System::getTotalCpuUsage());
}

void testBase64()
{
    SimpleFile f = SimpleFile("Base64Stuff.txt", SimpleFile::READ);
    auto c = f.readFullFileAsBytes();
    auto d = StringTools::base64Decode(c);
    StringTools::println("%llu, %llu", c.size(), d.size());
    f.close();

    SimpleFile f2 = SimpleFile("CMM.db", SimpleFile::WRITE);
    f2.writeBytes((unsigned char*)d.data(), d.size());
    f2.close();

}

void testResourceManager()
{
    ResourceManager<Image> imageManager = ResourceManager<Image>();
    imageManager.addResource(new Image(32, 32), "TestImg1", false);
    imageManager.addResource(new Image(32, 32), "TestImg2", false);
    imageManager.addResource(new Image(32, 32), "TestImg3", false);
    imageManager.addResource(imageManager.getResource("TestImg1").getPointer(), "TestImg1", false);

    StringTools::println("%llu", imageManager.size());
    StringTools::println("%p", imageManager.getResource("TestImg1").getRawPointer());
    StringTools::println("%p", imageManager.getResource("TestImg2").getRawPointer());
    StringTools::println("%p", imageManager.getResource("TestImg3").getRawPointer());
    StringTools::println("%p", imageManager.getResource("TestImg4").getRawPointer());
    
    imageManager.clear();
    StringTools::println("EARLY CLEAR");
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

void printNode(int tabs, bool side, BinaryTreeNode<RBNode<uint32_t>>* n)
{
    std::string tabStr = "";
    for(int i=0; i<tabs; i++)
    {
        tabStr += '\t';
    }
    if(n!=nullptr)
    {
        StringTools::println("%s %s %c - %u \033[0m", tabStr.c_str(), (n->data.color == true) ? "\033[31m" : "", (side) ? 'R' : 'L', n->data.data);
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

void simulateTouch()
{
    TouchSimulator::init();
    // TouchSimulator::test();
    TouchSimulator::setDown(0, 320, 320);
    TouchSimulator::setDown(1, 640, 640);
    TouchSimulator::inject();
    // if(TouchSimulator::inject())
    //     StringTools::println("OKAY");
    // else
    //     StringTools::println("NOT OKAY");
    
    for(int i=0; i<100; i++)
    {
        TouchSimulator::setDown(0, 320+i, 320+i);
        TouchSimulator::setDown(1, 640+i, 640+i);
        TouchSimulator::inject();
        // if(TouchSimulator::inject())
        //     StringTools::println("OKAY-loop");
        // else
        //     StringTools::println("NOT OKAY-loop");
        Sleep(1);
    }

    TouchSimulator::setUp(0);
    TouchSimulator::setUp(1);
    TouchSimulator::inject();
    // if(TouchSimulator::inject())
    //     StringTools::println("OKAY");
    // else
    //     StringTools::println("NOT OKAY");
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

void testXML()
{
    const std::string rawHTML = R"R0N0(
		<!DOCTYPE html>
		<html>
			<head>
                <script>
                    //<end>
                    var v = 1;

                    //</end>
                </script>
			</head>
		</html>
	)R0N0";

    const std::string rawHTML2 = R"R0N0(
        <html>
            value1
            j
            <head>
                <script>
                    var v = 1;
                    var c = 2;
                </script>
            </head>
        </html>
    )R0N0";
    
    glib::SimpleXml html = glib::SimpleXml();
    html.loadFromBytes((unsigned char*)rawHTML.data(), rawHTML.size());

    StringTools::println("%llu", html.getNodes().size());
}

void traverseInOrder(BinaryTreeNode<RBNode<uint32_t>>* n)
{
    if(n != nullptr)
    {
        traverseInOrder(n->leftChild);
        StringTools::println("%u", n->data.data);
        traverseInOrder(n->rightChild);
    }
}

void testStreamLZSS(int type)
{
    //!Does not work. A bunch of memory errors and junk.
    //!Also, Just not as fast as I thought it would be. ( sad :( )
    //! Put into visual studios and profile it.

    Sprite spr = Sprite();
    spr.loadImage("testOutput.png");

    unsigned char* dataToCompress = (unsigned char*)spr.getImage(0)->getPixels();
    size_t sizeOfData = spr.getImage(0)->getWidth()*spr.getImage(0)->getHeight()*4;

    size_t t1, t2;

    if (type == 0)
    {
        // t1 = System::getCurrentTimeNano();
        // StreamCompressionLZSS com = StreamCompressionLZSS(StreamCompressionLZSS::TYPE_COMPRESSION);
        // com.addData(dataToCompress, sizeOfData);
        // t2 = System::getCurrentTimeNano();

        // StringTools::println("Time for updated stream compression: %lluns", t2 - t1);
    }
    else if (type == 1)
    {
        //28966929600ns
        t1 = System::getCurrentTimeNano();
        Compression::compressLZSS(dataToCompress, sizeOfData);
        t2 = System::getCurrentTimeNano();

        StringTools::println("Time for default compression: %lluns", t2 - t1);
    }
    else
    {
        t1 = System::getCurrentTimeNano();
        std::vector<unsigned char> output = Compression::compressDeflate(dataToCompress, sizeOfData, 1, 7);
        t2 = System::getCurrentTimeNano();
        StringTools::println("Time for my deflate: %lluns, %llu", t2-t1, output.size());
    }

    // std::vector<unsigned char> buffer = std::vector<unsigned char>(1<<24);
    // unsigned long bufferLen = 1<<24;

    // t1 = System::getCurrentTimeNano();
    // compress2(buffer.data(), &bufferLen, dataToCompress, sizeOfData/12, 9);
    // t2 = System::getCurrentTimeNano();
    // StringTools::println("Time for zlib: %lluns, %llu", t2-t1, bufferLen);

}

void testBST()
{
    BinaryTree<int> tree1 = BinaryTree<int>();
    auto k1 = new BinaryTreeNode<int>();
    auto k2 = new BinaryTreeNode<int>();

    k1->data = 1;
    k2->data = 2;

    
    tree1.setRootNode(k1);
    tree1.setLeftNode(k1, k2);
    
    StringTools::println("K1 = %p, %p, %p, %p", k1->parent, k1->leftChild, k1->rightChild, k1);
    StringTools::println("K2 = %p, %p, %p, %p", k2->parent, k2->leftChild, k2->rightChild, k2);

    tree1.swapNodes(k1, k2);
    
    StringTools::println("K1 = %p, %p, %p, %p", k1->parent, k1->leftChild, k1->rightChild, k1);
    StringTools::println("K2 = %p, %p, %p, %p", k2->parent, k2->leftChild, k2->rightChild, k2);

}

void testXML2()
{
    SimpleXml f = SimpleXml("test1.html");
    StringTools::println("%llu", f.getNodes().size());
    std::vector<std::string> pattern = {"html", "body", "div", "div", "table", "tr", "td","textarea"};
    auto results = f.getNodesPattern(pattern);

    if(results.size() == 0)
        StringTools::println("No Nodes Found");
    
    for(auto r : results)
    {
        StringTools::println("Potential Node: %s", r->getTitle().c_str());
    }
}

// int WinMain(HINSTANCE hins, HINSTANCE preIns, LPSTR cmdline, int nShowCMD)
int main(int argc, char** argv)
{
    // testWindow();
    // testVectorGraphic();
    // testVectorFont();
    // testFontDrawing();

    // testOTFLoading();
    // testAddressSanitizer();
    // testDialogBox();

    // testPerformanceCounters();
    // testBase64();

    // testResourceManager();

    // testCompression();
    // printTree();
    // simulateTouch();
    // testAlphaWindow();
    // testXML();
    // testBSAT();
    // testStreamLZSS(0);
    // testBST();

    // testSuffixStuff();

    testXML();

    return 0;
}
