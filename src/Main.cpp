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

#include "WebRequest.h"

#include "HttpServer.h"
#include "SuffixAutomaton.h"

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
    if(dir.doesExist(dir.getLocation()))
    {
        std::vector<std::string> files = dir.getFiles();
        std::vector<std::string> folders = dir.getFolders();

        std::string folderColor = "\033[32m";
        std::string defaultColor = "\033[0m";

        StringTools::print(folderColor);
        for(std::string& s : folders)
        {
            StringTools::println(s);
            StringTools::println("\t%llu", dir.getLastChangeTime(s));
        }

        StringTools::print(defaultColor);
        for(std::string& s : files)
        {
            StringTools::println(s);
            StringTools::println("\t%llu", dir.getLastChangeTime(s));
        }
    }
    else
    {
        StringTools::println("Directory does not exist (or can't be accessed).");
    }
    
    StringTools::getChar();
}

void testImgCompare()
{
    if(SimpleDir::doesExist("./TestImages/compareThese"))
    {
        SimpleDir d = SimpleDir("./TestImages/compareThese");
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

void test()
{
    StringTools::println("Enter the directory to load");
    std::string s = StringTools::getString();
    SimpleDir d = SimpleDir(s);
    auto fileList = d.getFiles();
    Sprite spr;
    for(File file : fileList)
    {
        spr.loadImage(file, false);
    }

    StringTools::println("Images loaded: %llu", spr.getSize());
    if(spr.getSize() > 0)
    {
        StringTools::println("Set the delay time between frames in milliseconds");
        s = StringTools::getString();
        int delay = StringTools::toInt(s);
        for(int i=0; i<spr.getSize(); i++)
        {
            spr.setDelayTime(i, delay);
        }

        StringTools::println("Enter the type: 0 = GIF, 1 = APNG");
        s = StringTools::getString();
        if(s[0] == '1')
            spr.saveAPNG("AnimatedPNG.png", false);
        else
            spr.saveAGIF("AnimatedGif.gif", 256, false, false);
    }
}

void testHTTPRequest()
{
    Network n = Network(Network::TYPE_CLIENT, 80, "httpbin.org", 1, true);
    n.setOnConnectFunction([&n](int id)->void {
        StringTools::println("Connected to https://httpbin.org");
    });
    n.setOnDataAvailableFunction([&n](int id)->void {
        StringTools::println("Data available: Attempting to receive");
        std::vector<char> buffer = std::vector<char>(1024);
        int count = n.receiveMessage(buffer.data(), buffer.size(), 0, true);
        StringTools::println("Read %d bytes", count);
        for(char& c : buffer)
        {
            StringTools::print("%c", c);
        }
        StringTools::println("");
    });
    n.setOnDisconnectFunction([&n](int id)->void {
        StringTools::println("Disconnected");
    });

    n.startNetwork();
    while(n.getRunning())
    {
        std::string input = StringTools::getString();
        if(input == "GET")
        {
            //form get request and send
            // GET /get HTTP/2
            // Host: httpbin.org
            // User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/113.0
            // Accept: application/json
            // Accept-Language: en-US,en;q=0.5
            // Accept-Encoding: gzip, deflate, br
            // Referer: https://httpbin.org/
            // DNT: 1
            // Connection: keep-alive
            // Sec-Fetch-Dest: empty
            // Sec-Fetch-Mode: cors
            // Sec-Fetch-Site: same-origin
            // Sec-GPC: 1
            // TE: trailers

            WebRequest w = WebRequest();
            w.setHeader(WebRequest::TYPE_GET, "/get", true);
            w.addKeyValue("Host", "httpbin.org");
            w.addKeyValue("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/113.0");
            w.addKeyValue("Accept", "application/json");
            w.addKeyValue("Accept-Language", "en-US,en;q=0.5");
            w.addKeyValue("Accept-Encoding", "gzip, deflate, br");
            w.addKeyValue("Referer", "https://httpbin.org/");
            w.addKeyValue("DNT", "1");
            w.addKeyValue("Connection", "keep-alive");
            w.addKeyValue("Sec-Fetch-Dest", "empty");
            w.addKeyValue("Sec-Fetch-Mode", "cors");
            w.addKeyValue("Sec-Fetch-Dest", "empty");
            w.addKeyValue("Sec-Fetch-Site", "same-origin");
            w.addKeyValue("Sec-GPC", "1");
            w.addKeyValue("TE", "trailers");

            std::string request = w.getRequestAsString();
            StringTools::println("SENDING THIS:\n");
            StringTools::println(request);
            StringTools::println("");
            n.sendMessage(request);
        }
        else if(input == "END")
            break;
    }
    StringTools::println("Attempting to end network");
    n.endNetwork();
}

void testHTML()
{
    std::vector<std::string> pattern = {"html", "body", "div", "div", "div", "div", "ul", "li", "span", "a"};
    SimpleXml xml = SimpleXml("temp/temp.data");
    auto results = xml.getNodesPattern(pattern);
    StringTools::println("%llu", results.size());
    for(int i=0; i<results.size(); i++)
    {
        StringTools::println(results[i]->getValue());
    }
}

#include "WebSocket.h"

WebSocket* sock;

void onConnection(int id)
{
    StringTools::println("Connected: %d", id);
}

void onDisconnection(int id)
{
    StringTools::println("Disconnected: %d", id);
}

void onNewPacket(int id)
{
    StringTools::println("PACKET REC: %d", id);
    auto packet = sock->receivePacket(id);
    switch(packet.type)
    {
        case WebSocket::TYPE_TEXT:
            for(int i=0; i<packet.buffer.size(); i++)
            {
                StringTools::print("%c", packet.buffer[i]);
            }
            break;
        case WebSocket::TYPE_BINARY:
            for(int i=0; i<packet.buffer.size(); i++)
            {
                StringTools::print("%x", packet.buffer[i]);
            }
            break;
        default:
            break;
    }
}

void init(bool type, int port)
{
    sock = new WebSocket(type, port, "localhost");
    sock->setOnConnectFunction(onConnection);
    sock->setOnDisconnectFunction(onDisconnection);
    sock->setNewPacketFunction(onNewPacket);

    sock->startNetwork();
}

void dispose()
{
    delete sock;
    sock = nullptr;
}

void run()
{
    while(true)
    {
        std::string input = StringTools::getString();
        if(input == "quit" || input == "exit")
        {
            break;
        }
        else if(input == "send")
        {
            sock->sendText("Hello");
        }
    }
}

void testWebSocketServer()
{
    init(WebSocket::TYPE_SERVER, 4040);
    run();
    dispose();
}

void testWebSocketClient()
{
    init(WebSocket::TYPE_CLIENT, 4040);
    run();
    dispose();
}

void testDropFiles()
{
    WindowOptions wo;
    wo.threadManaged = false;

    SimpleWindow w = SimpleWindow("Title", 320, 240, -1, -1, wo);
    w.waitTillClose();
}

void testHTTPServer()
{
    HttpServer server = HttpServer("127.0.0.1", 4041, 64);
    server.setLogInfo(true);
    server.start();
    while(server.getRunning())
    {
        Input::pollInput();
        if(Input::getKeyPressed(Input::KEY_ESCAPE))
        {
            break;
        }
        System::sleep(10);
    }
}

void initGui(SimpleWindow* w)
{
    w->getGuiManager()->loadElementsFromFile("GuiLayoutFile.xml");
    w->getGuiManager()->setBackgroundColor(Color{255,255,255,255});
}

void testNewGui()
{
    SimpleGraphics::init();
    GuiManager::initDefaultLoadFunctions();

    WindowOptions o;
    o.initFunction = initGui;
    SimpleWindow w = SimpleWindow("New Gui Window", 320, 240, -1, -1, o);

    w.waitTillClose();
    SimpleGraphics::dispose();
}

void testHashSpeed()
{
    Sprite s = Sprite();
    // s.loadImage("TestImages/BMP/BMPTestData/large.bmp");
    s.loadImage("TestImages/compareThese/img4_1.png");
    size_t length = s.getImage(0)->getWidth() * s.getImage(0)->getHeight();
    unsigned char* pixs = (unsigned char*)s.getImage(0)->getPixels();
    
    size_t index = 0;
    size_t endIndex = length*4;
    unsigned char* endPixs = pixs + endIndex;
    size_t t1, t2;
    size_t timeForHV1 = 0;

    // size_t actualBytes = 0;

    // t1 = System::getCurrentTimeNano();
    // std::vector<lengthPair> output1;
    // Compression::getLZ77RefPairsCHash(pixs, endIndex, &output1, 7);
    // t2 = System::getCurrentTimeNano();
    // timeForHV1 = t2-t1;
    
    // StringTools::println("Time For CHash: %llu", timeForHV1);
    // StringTools::println("OUTPUT1 SIZE: %llu", output1.size());
    
    t1 = System::getCurrentTimeNano();
    std::vector<lengthPair> output2;
    Compression::getLZ77RefPairsCSA(pixs, endIndex, &output2, 7);
    t2 = System::getCurrentTimeNano();
    timeForHV1 = t2-t1;
    
    StringTools::println("Time For CSA: %llu", timeForHV1);
    StringTools::println("OUTPUT2 SIZE: %llu", output2.size());
    
    // t1 = System::getCurrentTimeNano();
    // std::vector<lengthPair> output3;
    // Compression::getLZ77RefPairsKMP(pixs, endIndex, &output3, 7);
    // t2 = System::getCurrentTimeNano();
    // timeForHV1 = t2-t1;
    
    // StringTools::println("Time For KMP: %llu", timeForHV1);
    // StringTools::println("OUTPUT3 SIZE: %llu", output3.size());

    SimpleFile f = SimpleFile("OutputLZ1.txt", SimpleFile::WRITE);
    for(lengthPair& l : output2)
    {
        if(l.literal)
            f.writeString("LIT: " + StringTools::toString(l.v1));
        else
            f.writeString("REF: " + StringTools::toString(l.v1) + " - " + StringTools::toString(l.v2));
        f.writeLineBreak();
    }
    f.close();

    /*
    Stats:
        Time For CHash: 625554000 ns
        OUTPUT1 SIZE: 1571307 pairs

        Time For CSA: 836515700 ns
        OUTPUT2 SIZE: 1723374 pairs

        Time For KMP: 35839262700 ns
        OUTPUT3 SIZE: 1486075 pairs
     */
}

void testStreamLZSS()
{
    Sprite s = Sprite();
    s.loadImage("TestImages/BMP/BMPTestData/large.bmp");

    StreamCompressionLZSS lzCompressor = StreamCompressionLZSS(StreamCompressionLZSS::TYPE_COMPRESSION);
    size_t length = s.getImage(0)->getWidth() * s.getImage(0)->getHeight() * 4;
    unsigned char* pixs = (unsigned char*)s.getImage(0)->getPixels();

    for(int i=0; i<length; i+=4)
    {
        lzCompressor.addData(pixs, 4);
        pixs+=4;
    }
    lzCompressor.endData();

    //Get Binary Set
    BinarySet bin = lzCompressor.getBuffer();

    StringTools::println("Final Bit size = %llu", bin.size());

    // std::vector<unsigned char> compareMe = Compression::compressLZSS(pixs, length);
    // lzCompressor.clearBuffer();

    // for(int i=0; i<length; i++)
    // {
    //     if(compareMe[i] != pixs[i])
    //     {
    //         StringTools::println("BROKEN AT %d", i);
    //         break;
    //     }
    // }
}

// int WinMain(HINSTANCE hins, HINSTANCE preIns, LPSTR cmdline, int nShowCMD)
int main(int argc, char** argv)
{
    // testStreamLZSS();
    testHashSpeed();
    // testSATime();
    // StringTools::println("%d", sizeof(SFA_State));
    // testSuffixAutomaton();
    // SAVerification();
    // testNewGui();
    // testHTTPServer();
    // testDropFiles();
    // testWebSocketClient();
    // testHTML();
    // StringTools::init();
    // dirTest();
    // testHTTPRequest();
    // test();
    // quickTest();
    // testOGLWindow();
    // testOTFLoading();
    // testVectorGraphic();

    // testImgCompare();
    // stressCompare(0xFFFFFF);

    return 0;
}
