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

void saveAnimatedGif()
{
	Sprite s;
	s.loadImage("C:\\Stuff\\avatar_1.gif");
	StringTools::println("%llu, %d, %d", s.getSize(), s.shouldLoop(), s.getDelayTime());

	bool v = s.saveAGIF("test.gif");
	StringTools::println("Successful = %d", v);
}

// int WinMain(HINSTANCE hins, HINSTANCE preIns, LPSTR cmdline, int nShowCMD)
int main(int argc, char** argv)
{
	saveAnimatedGif();
    // quickTest();
    // testOGLWindow();
    // testOTFLoading();
    // testVectorGraphic();
    return 0;
}
