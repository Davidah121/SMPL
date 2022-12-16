#include "StringTools.h"
#include "System.h"
#include "SimpleWindow.h"
#include "SimpleDir.h"
#include "SpatialHashing.h"
#include "IniFile.h"
#include "VectorFont.h"
#include "Network.h"
#include "Input.h"
#include <stdlib.h>
#include <atomic>

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
    glib::Font* bitmapFont = SimpleGraphics::getDefaultFont(0);

    FontCharInfo bfci = bitmapFont->getFontCharInfo( bitmapFont->getCharIndex('o') );
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

    FontCharInfo fci = vF.getFontCharInfo( vF.getCharIndex('o') );
    double scaleV = (double)vF.getFontSize() / vF.getOriginalFontSize();
    StringTools::println("FontInfo2: %d, %d, %d, %d, %d", fci.width, fci.height, fci.horizAdv, fci.xOffset, fci.yOffset);
    StringTools::println("FontInfo3: %.3f, %.3f, %.3f, %.3f, %.3f", fci.width/scaleV, fci.height/scaleV, fci.horizAdv/scaleV, fci.xOffset/scaleV, fci.yOffset/scaleV);
    
}

void testOTFLoading()
{
    VectorFont f = VectorFont();
    f.load("C:/Windows/Fonts/Arial/arial.ttf");
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

    testPerformanceCounters();
    return 0;
}
