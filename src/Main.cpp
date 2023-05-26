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

#include "ComputerVision.h"

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

void dirTest()
{
	StringTools::println("Enter location:");
    std::string l = StringTools::getString();

    SimpleDir dir = SimpleDir(l);
    if(dir.doesExist())
    {
        std::vector<std::string> files = dir.getFiles();
        std::vector<std::string> folders = dir.getFolders();

        std::string folderColor = "\033[32m";
        std::string defaultColor = "\033[0m";

        StringTools::print(folderColor);
        for(std::string& s : folders)
        {
            StringTools::println(s);
        }

        StringTools::print(defaultColor);
        for(std::string& s : files)
        {
            StringTools::println(s);
        }
        
        for(int i=0; i<dir.getSize(); i++)
        {
            StringTools::println("%llu", dir.getLastChangeTime(0));
        }
    }
    else
    {
        StringTools::println("Directory does not exist (or can't be accessed).");
    }

    system("pause");
}

void testImgCompare()
{
    SimpleDir d = SimpleDir("./TestImages/compareThese");
    if(d.doesExist())
    {
        std::vector<std::pair<std::string, uint64_t>> pairs;
        std::vector<std::string> files = d.getFiles();
        size_t t1,t2,loadTime,hashTime;
        loadTime = 0;
        hashTime = 0;

        for(std::string& s : files)
        {
            StringTools::println("Loading %s", s.c_str());
            Sprite spr = Sprite();

            t1 = System::getCurrentTimeNano();
            spr.loadImage(s);
            t2 = System::getCurrentTimeNano();
            loadTime += t2-t1;

            t1 = System::getCurrentTimeNano();
            if(spr.getSize() > 0)
            {
                std::pair<std::string, uint64_t> p;
                p.first = File(s).getFileName();
                p.second = ComputerVision::dHash(spr.getImage(0));
                pairs.push_back(p);
            }
            t2 = System::getCurrentTimeNano();
            hashTime += t2-t1;
        }

        StringTools::println("\n------------------\n");
        for(int i=0; i<pairs.size(); i++)
        {
            for(int j=i+1; j<pairs.size(); j++)
            {
                int difference = ComputerVision::hammingDistanceImageHash(pairs[i].second, pairs[j].second);
                StringTools::println("%s vs %s | Difference Rating = %d / %d = %.3f", pairs[i].first.c_str(), pairs[j].first.c_str(), difference, 64, (double)difference/64.0);
            }
        }

        StringTools::println("Loading Time: %llu", loadTime);
        StringTools::println("Hashing Time: %llu", hashTime);
    }
}

void stressCompare(int count)
{
    std::vector<uint64_t> values;
    PCG randGen = PCG();
    for(int i=0; i<count; i++)
    {
        uint64_t v = randGen.get();
        v = (v << 32) + randGen.get();
        values.push_back(v);
    }

    uint64_t t1 = System::getCurrentTimeNano();
    // Sort::mergeSort(values.data(), values.size());
    int dis = 64;
    for(int i=1; i<count; i++)
    {
        int ndis = ComputerVision::hammingDistanceImageHash(values[i], values[0]);
        if(ndis < dis)
            dis = ndis;
    }
    uint64_t t2 = System::getCurrentTimeNano();
    StringTools::println("Time taken = %lluns", t2 - t1);
    StringTools::println("Minimum Distance: %d", dis);
}


// int WinMain(HINSTANCE hins, HINSTANCE preIns, LPSTR cmdline, int nShowCMD)
int main(int argc, char** argv)
{
    // quickTest();
    // testOGLWindow();
    // testOTFLoading();
    // testVectorGraphic();

    // testImgCompare();
    // stressCompare(0xFFFFFF);

    return 0;
}
