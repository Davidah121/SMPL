#include "StringTools.h"
#include "SimpleDir.h"
#include "System.h"
#include "SimpleWindow.h"
#include <atomic>
#include "WebClient.h"
#include "Input.h"
#include "Sprite.h"
#include "QuadTree.h"
#include "ColorSpaceConverter.h"
#include "SimpleGraphics.h"
#include "Compression.h"
#include "ComputerVision.h"

using namespace smpl;

bool firstConnect = true;
void testWebClient()
{
    // WebClient client = WebClient("https://www.google.com");
    WebClient client = WebClient("http://developer.mozilla.org");
    // WebClient client = WebClient("https://stackoverflow.com/questions/73916302/how-can-i-use-an-https-client-certificate-protected-api-with-firefox");
    client.setOnConnectFunc([](WebClient* wc) ->void {
        StringTools::println("CONNECTED TO %s", wc->getWebname().c_str());
        
        if(firstConnect)
        {
            WebRequest req;
            req.setHeader(WebRequest::TYPE_GET, "/en-US/docs/Web/HTTP/Redirections", true);
            wc->sendRequest(req);
            firstConnect = false;
        }
    });

    client.setOnBufferChangedFunc([](WebClient* wc, WebRequest& response, unsigned char* buffer, size_t sizeOfBuffer) ->void {
        if(wc->getLastResponseCode() == 200)
        {
            StringTools::println("Writing %llu bytes", sizeOfBuffer);
            SimpleFile testFile = SimpleFile("firefox.html", SimpleFile::WRITE_APPEND);
            if(testFile.isOpen())
            {
                testFile.writeBytes(buffer, sizeOfBuffer);
                testFile.close();
            }
        }
        else
        {
            StringTools::println("ERROR: Returned some other response that is not 200.");
            StringTools::println("Response Code = %d", wc->getLastResponseCode());

            if(sizeOfBuffer > 0)
            {
                StringTools::println("Writing %llu bytes to error buffer", sizeOfBuffer);
                SimpleFile testFile = SimpleFile("error.html", SimpleFile::WRITE_APPEND);
                if(testFile.isOpen())
                {
                    testFile.writeBytes(buffer, sizeOfBuffer);
                    testFile.close();
                }
            }
        }
    });

    client.setOnDisconnectFunc([](WebClient* wc) ->void {
        StringTools::println("DISCONNECTED From %s", wc->getHost().c_str());
    });
    
    client.alwaysSendKeepAlive(false);
    SimpleDir::deleteResource("./firefox.html");
    client.start();

    while(true)
    {
        if(client.getShouldRedirect())
            client.completeRedirect();
        
        Input::pollInput();
        if(Input::getKeyDown(Input::KEY_ESCAPE))
            break;
        System::sleep(1, 0, false);
    }
    StringTools::println("ENDING");
}

size_t subDivisions = 0;
size_t subDivisionsPerSize[8];

void recursivePass(Image* img, int startX, int startY, int size, int lowestSize, double threshold, BinarySet& output, Matrix& xDer, Matrix& yDer)
{
    subDivisions++;
    int totalSize = size*size;
    
    //get average derivative
    Vec2f avgDirection;
    Vec3f avgColorVec;
    for(int y = startY; y<startY+size; y++)
    {
        for(int x = startX; x<startX+size; x++)
        {
            Vec2f perpendicularDirection;
            Vec2f edgeDirection = Vec2f(xDer[y][x], yDer[y][x]);
            if(edgeDirection.x == 0 && edgeDirection.y != 0)
                perpendicularDirection = Vec2f(1, 0);
            else if(edgeDirection.x != 0 && edgeDirection.y == 0)
                perpendicularDirection = Vec2f(0, 1);
            else if(edgeDirection.x != 0 && edgeDirection.y != 0)
            {
                perpendicularDirection.x = 1;
                perpendicularDirection.y = edgeDirection.x / -edgeDirection.y;
            }

            avgDirection.x += xDer[y][x] / (size*size);
            avgDirection.y += yDer[y][x] / (size*size);

            avgColorVec.x += (double)img->getPixels()[x + y*img->getWidth()].red / (size*size);
            avgColorVec.y += (double)img->getPixels()[x + y*img->getWidth()].green / (size*size);
            avgColorVec.z += (double)img->getPixels()[x + y*img->getWidth()].blue / (size*size);
        }
    }

    //get std dev
    Vec2f stdDev;
    for(int y = startY; y<startY+size; y++)
    {
        for(int x = startX; x<startX+size; x++)
        {
            stdDev.x += MathExt::sqr(avgDirection.x - xDer[y][x]);
            stdDev.y += MathExt::sqr(avgDirection.y - yDer[y][x]);
        }
    }
    stdDev /= (size*size);
    stdDev.x = MathExt::sqrt(stdDev.x);
    stdDev.y = MathExt::sqrt(stdDev.y);

    Color avgColor;
    avgColor.red = MathExt::clamp(MathExt::round(avgColorVec.x), 0.0, 255.0);
    avgColor.green = MathExt::clamp(MathExt::round(avgColorVec.y), 0.0, 255.0);
    avgColor.blue = MathExt::clamp(MathExt::round(avgColorVec.z), 0.0, 255.0);
    avgColor.alpha = 255;
    
    // StringTools::println("(%d, %d) - avg: (%.3f, %.3f) std deviation: (%.3f, %.3f)", startX, startY, avgDirection.x, avgDirection.y, stdDev.x, stdDev.y);

    //check against threshold
    if(size > lowestSize && (stdDev.x > threshold || stdDev.y > threshold))
    {
        //split further
        int newSize = size/2;
        output.add(true);
        recursivePass(img, startX, startY, newSize, lowestSize, threshold, output, xDer, yDer);
        recursivePass(img, startX + newSize, startY, newSize, lowestSize, threshold, output, xDer, yDer);
        recursivePass(img, startX, startY + newSize, newSize, lowestSize, threshold, output, xDer, yDer);
        recursivePass(img, startX + newSize, startY + newSize, newSize, lowestSize, threshold, output, xDer, yDer);
    }
    else
    {
        output.add(false);
        //do gradient stuff.
        Vec2f perpendicularDirection;
        if(avgDirection.x == 0 && avgDirection.y != 0)
            perpendicularDirection = Vec2f(1, 0);
        else if(avgDirection.x != 0 && avgDirection.y == 0)
            perpendicularDirection = Vec2f(0, 1);
        else if(avgDirection.x != 0 && avgDirection.y != 0)
        {
            perpendicularDirection.x = 1;
            perpendicularDirection.y = avgDirection.x / -avgDirection.y;
        }
        perpendicularDirection = perpendicularDirection.normalize();

        //for now 4 possible vectors Vec2f(0, 0); Vec2f(1, 0); Vec2f(1,1); Vec2f(0, 1);
        double d, minDot;

        int v = 0;

        if(true)
        {
            v = 0;
            output.add(v, 7);
            output.add(avgColor.red);
            output.add(avgColor.green);
            output.add(avgColor.blue);
        }
        else
        {
            minDot = MathExt::dot(perpendicularDirection, Vec2f(1, 0));
            v = 1;
            
            d = MathExt::dot(perpendicularDirection, Vec2f(1, 1));
            if(d < minDot)
            {
                v = 2;
            }

            d = MathExt::dot(perpendicularDirection, Vec2f(0, 1));
            if(d < minDot)
            {
                v = 3;
            }
            output.add(v, 7);
        }
        
        
        for(int y=startY; y<startY+size; y++)
        {
            for(int x=startX; x<startX+size; x++)
            {
                Color finalColor;
                if(v == 1)
                {
                    finalColor = img->getPixels()[startX + y*img->getWidth()];
                }
                else if(v == 3)
                {
                    finalColor = img->getPixels()[x + startY*img->getWidth()];
                }
                else if(v == 2)
                {
                    int nX, nY;
                    if(x < y)
                    {
                        nY = y - (x-startX);
                        nX = startX;
                    }
                    else
                    {
                        nX = x - (y-startY);
                        nY = startY;
                    }
                    finalColor = img->getPixels()[nX + nY*img->getWidth()];
                }
                else if(v == 0)
                {
                    finalColor = avgColor;
                }
                img->getPixels()[x + y*img->getWidth()] = finalColor;
            }
        }
        int index = MathExt::ceil(MathExt::log((double)size, 2.0));
        subDivisionsPerSize[index]++;
    }
}

void testQuadTreeLikeCompression()
{
    //load image. change width and height to be divisible by 32
    //put into quad tree like structure until either the size of the node = 4 pixels or
    //the is a small amount of variance in it.
    
    Sprite imgs = Sprite();
    imgs.loadImage("TestImages/examples/Flat2/screenshot_2.png");

    if(imgs.getSize() > 0)
    {
        for(int y=0; y<imgs.getImage(0)->getHeight(); y++)
        {
            for(int x=0; x<imgs.getImage(0)->getWidth(); x++)
            {
                Color c = imgs.getImage(0)->getPixels()[x + y*imgs.getImage(0)->getWidth()];
                imgs.getImage(0)->getPixels()[x + y*imgs.getImage(0)->getWidth()] = ColorSpaceConverter::convert(c, ColorSpaceConverter::RGB_TO_YCBCR);
            }
        }

        Mat3f gx = Mat3f(1, 0, -1,
                        2, 0, -2,
                        1, 0, -1);
        Mat3f gy = Mat3f( 1,  2,  1,
                        0,  0,  0,
                        -1, -2, -1);

        Matrix imgXDerivative = ComputerVision::convolution(imgs.getImage(0), &gx, ComputerVision::RED_CHANNEL, true);
        Matrix imgYDerivative = ComputerVision::convolution(imgs.getImage(0), &gy, ComputerVision::RED_CHANNEL, true);

        //save these as images
        for(int i=64; i>=1; i/=2)
        {
            Image newImg = Image();
            BinarySet output;
            newImg.copyImage(imgs.getImage(0));

            for(int y=0; y<newImg.getHeight(); y+=64)
            {
                for(int x=0; x<newImg.getWidth(); x+=64)
                {
                    recursivePass(&newImg, x, y, 64, i, 0.1, output, imgXDerivative, imgYDerivative);
                }
            }
            std::vector<unsigned char> compressedData = Compression::compressDeflate(output.getByteRef(), 1);

            Vec3f err;
            for(int y=0; y<newImg.getHeight(); y++)
            {
                for(int x=0; x<newImg.getWidth(); x++)
                {
                    Color newColor = newImg.getPixels()[x+y*newImg.getWidth()];
                    Color oriColor = imgs.getImage(0)->getPixels()[x+y*newImg.getWidth()];

                    err.x += MathExt::sqr((double)oriColor.red - (double)newColor.red) / (newImg.getWidth()*newImg.getHeight());
                    err.y += MathExt::sqr((double)oriColor.green - (double)newColor.green) / (newImg.getWidth()*newImg.getHeight());
                    err.z += MathExt::sqr((double)oriColor.blue - (double)newColor.blue) / (newImg.getWidth()*newImg.getHeight());
                    
                    Color c = newImg.getPixels()[x + y*newImg.getWidth()];
                    newImg.getPixels()[x + y*newImg.getWidth()] = ColorSpaceConverter::convert(c, ColorSpaceConverter::YCBCR_TO_RGB);
                }
            }
            StringTools::println("MEAN SQUARED ERROR: (%.3f, %.3f, %.3f)", err.x, err.y, err.z);
            StringTools::println("Threshold = %.3f, Lowest Subdivision Size = %d, Subdivisions: %llu", 0.1, i, subDivisions);
            
            StringTools::println("Final Size Before Compression = %llu", output.getByteRef().size());
            StringTools::println("Final Size After Compression = %llu", compressedData.size());
            newImg.saveBMP("TestImages/examples/Grad2/QuadTreeLikeCompression_" + StringTools::toString(i) + "_FlatTest_2.bmp");
        }
    }
    else
    {
        StringTools::println("Failed to load image");
    }

}

void testCopyDirectory()
{
    SimpleDir::copyResource("C:/Drive_D/Games/Steam/userdata/167820327/2054970/remote/win64_save", "TestImages/examples/BT3");
}

void streamDecompressTest()
{
    size_t t1 = System::getCurrentTimeMillis();
    std::string file1 = "BigCompressTest";
    std::string file2 = "C:/Users/Alan/Desktop/DoThese/2023_11_05/Launch out into the deep_uncompressed.mp4";
    // std::string file1 = "BaselineCompressedData";
    // std::string file2 = "BaselineOutput2";
    SimpleFile f = SimpleFile(file1, SimpleFile::READ);
    SimpleFile f2 = SimpleFile(file2, SimpleFile::WRITE);
    StreamCompressionLZSS compressor = StreamCompressionLZSS(StreamCompressionLZSS::TYPE_DECOMPRESSION);

    if(f.isOpen() && f2.isOpen())
    {
        unsigned char* buffer = new unsigned char[0xFFFF];
        while(true)
        {
            size_t actualRead = f.readBytes((char*)buffer, 0xFFFF);
            if(actualRead > 0)
            {
                compressor.addData(buffer, actualRead);
                
                if(compressor.getBuffer().getByteRef().size() > 0)
                {
                    //try to write.
                    f2.writeBytes(compressor.getBuffer().getByteRef().data(), compressor.getBuffer().getByteRef().size());
                    compressor.clearBuffer();
                }
            }
            else
                break;
        }

        f2.close();
        f.close();
        delete[] buffer;
    }
    
    size_t t2 = System::getCurrentTimeMillis();
    size_t readSize = SimpleDir::getReferenceSize(file1);
    size_t writeSize = SimpleDir::getReferenceSize(file2);
    StringTools::println("Time to read and decompress = %llu", t2-t1);

    StringTools::println("Read in %llu bytes", readSize);
    StringTools::println("Wrote out %llu bytes", writeSize);

    double timeInSeconds = (double)(t2-t1) / 1000;
    double speedInBytes = (double)readSize / timeInSeconds;
    double speedInMB = speedInBytes / 1000000;

    StringTools::println("Decompression Speed = %.3f MB/s", speedInMB);
}

void streamCompressTest()
{
    size_t timeToCompress = 0;

    // size_t t1 = System::getCurrentTimeMillis();
    std::string file1 = "C:/Users/Alan/Desktop/DoThese/2023_11_05/Launch out into the deep.mp4";
    std::string file2 = "BigCompressTest";
    // std::string file1 = "BaselineOutput";
    // std::string file2 = "BaselineCompressedData";
    SimpleFile f = SimpleFile(file1, SimpleFile::READ);
    SimpleFile f2 = SimpleFile(file2, SimpleFile::WRITE);
    StreamCompressionLZSS compressor = StreamCompressionLZSS(StreamCompressionLZSS::TYPE_COMPRESSION);

    if(f.isOpen() && f2.isOpen())
    {
        unsigned char* buffer = new unsigned char[0xFFFF];
        BinarySet leftovers;
        while(true)
        {
            size_t actualRead = f.readBytes((char*)buffer, 0xFFFF);
            if(actualRead > 0)
            {
                size_t t1 = System::getCurrentTimeMicro();
                compressor.addData(buffer, actualRead);
                size_t t2 = System::getCurrentTimeMicro();
                timeToCompress += (t2-t1);
                
                if(compressor.getBuffer().size() % 8 == 0)
                {
                    //try to write.
                    f2.writeBytes(compressor.getBuffer().getByteRef().data(), compressor.getBuffer().getByteRef().size());
                    compressor.clearBuffer();
                }
            }
            else
                break;
        }
        compressor.endData();
        //try to write.
        f2.writeBytes(compressor.getBuffer().getByteRef().data(), compressor.getBuffer().getByteRef().size());

        f2.close();
        f.close();
        delete[] buffer;
    }
    
    // size_t t2 = System::getCurrentTimeMillis();

    size_t readSize = SimpleDir::getReferenceSize(file1);
    size_t writeSize = SimpleDir::getReferenceSize(file2);
    StringTools::println("Time to read and compress = %lluus", timeToCompress);

    StringTools::println("Read in %llu bytes", readSize);
    StringTools::println("Wrote out %llu bytes", writeSize);

    double timeInSeconds = (double)(timeToCompress) / 1000000;
    double compressRatio = (double)readSize/(double)writeSize;
    double speedInBytes = (double)readSize / timeInSeconds;
    double speedInMB = speedInBytes / 1000000;

    StringTools::println("Compression Ratio = %.3f", compressRatio);
    StringTools::println("Compression Speed = %.3f MB/s", speedInMB);
}

void testLZ77CSA()
{
    std::string file1 = "BaselineOutput";
    SimpleFile f = SimpleFile(file1, SimpleFile::READ);
    std::vector<lengthPair> outputPairs1;
    std::vector<lengthPair> outputPairs2;
    bool same = false;
    if(f.isOpen())
    {
        auto data = f.readFullFileAsBytes();
        f.close();

        Compression::getLZ77RefPairsCHash(data.data(), data.size(), &outputPairs1, 7);
        Compression::getLZ77RefPairsCSA(data.data(), data.size(), &outputPairs2, 7);
    }
    
    StringTools::println("TotalSize: %llu", SimpleDir::getReferenceSize(file1));
    StringTools::println("%llu", outputPairs1.size());
    StringTools::println("%llu", outputPairs2.size());
}

void quickCompression()
{
    std::string file1 = "screenshot.png";
    Sprite s;
    s.loadImage(file1);
    std::vector<unsigned char> outputData;
    size_t compressTime = 0;
    if(s.getSize() > 0)
    {

        size_t t1,t2;
        t1 = System::getCurrentTimeMillis();
        // outputData = Compression::compressDeflate(data, 1, 7, true);
        // s.getImage(0)->savePNG("betterScreenshot.png", false, false, true);
        s.getImage(0)->saveGIF("betterScreenshot.gif", 256, false, false);
        compressTime = System::getCurrentTimeMillis() - t1;
        // s.getImage(0)->saveBMP("ScreenshotAsBMP.bmp");

        // SimpleFile f2 = SimpleFile("MyCurrentImplementation2", SimpleFile::WRITE);
        // f2.writeBytes(outputData.data(), outputData.size());
        // f2.close();
    }
    
    StringTools::println("TotalSize: %llu", SimpleDir::getReferenceSize(file1));
    StringTools::println("Time to compress = %llu", compressTime);
}

template<typename T>
class Streamable
{
public:
    virtual void push_back(T) = 0;
    virtual void seek(size_t) = 0;
    virtual T get() = 0;
    virtual void get(T*, size_t size) = 0;
    virtual void pop_back() = 0;
    virtual size_t size() = 0;
};

template<typename T>
class StreamableVector : public Streamable<T>
{
public:
    StreamableVector()
    {
        buffer = std::shared_ptr<std::vector<T>>(new std::vector<T>());
    }
    ~StreamableVector()
    {

    }

    virtual void push_back(T data)
    {
        buffer->push_back(data);
    }
    virtual void seek(size_t index)
    {
        offset = index;
    }
    virtual T get()
    {
        return buffer->at(offset++);
    }
    virtual void get(T* input, size_t size)
    {
        for(size_t i=0; i<size; i++)
            input[i] = buffer->at(offset+i);
    }
    virtual void pop_back()
    {
        buffer->pop_back();
    }
    virtual size_t size()
    {
        return buffer->size();
    }
private:
    std::shared_ptr<std::vector<T>> buffer = nullptr;
    size_t offset = 0;
};

void testSerialization()
{
    // Vec2f p;
    // SerializedObject* so = &p;
    // StringTools::println("Class of p = %s", so->getClass()->getName().c_str());
    // StringTools::println("Direct parents of P:");
    // auto parentList = so->getClass()->getListOfParents();
    // for(const RootClass* parentClass : parentList)
    // {
    //     StringTools::println("\t%s", parentClass->getName().c_str());
    // }

    // if(so->getClass()->isDerivedFrom(&Object::globalClass))
    //     StringTools::println("P also derives from Object");
    // else
    //     StringTools::println("P does not derive from Object");

    // std::streambuf buffer = std::basic_streambuf()
    // std::iostream inputStream = std::iostream();

}

void init(SimpleWindow* win)
{
    // GuiButton* but = new GuiButton();
    // but->setMinWidth(32);
    // but->setMinHeight(32);
    // but->setBackgroundColor({255,0,0,255});
    
    
    win->getGuiManager()->loadElementsFromFile("GuiLayoutFile.xml");
    // win->getGuiManager()->addElement(SmartMemory<GuiItem>::createNoDelete(but, false));
    // win->getGuiManager()->addToDisposeList(SmartMemory<GuiItem>::createNoDelete(but, false));
    // win->setActivateGui(false);
}

void testGuiPart2()
{
    GuiManager::initDefaultLoadFunctions();
    SimpleGraphics::init();
    WindowOptions options;
    options.initFunction = init;
    SimpleWindow win = SimpleWindow("Title", 320, 240, -1, -1, options);

    win.waitTillClose();
}

void saStuff()
{
    Sprite s;
    s.loadImage("screenshot.png");
    if(s.getSize() > 0)
    {
        size_t N = s.getImage(0)->getWidth() * s.getImage(0)->getHeight();
        Color* pixels = s.getImage(0)->getPixels();
        size_t t1 = System::getCurrentTimeNano();
        SuffixAutomaton sa = SuffixAutomaton(N*4);
        for(int i=0; i<N; i++)
        {
            sa.extend(pixels[i].red);
            sa.extend(pixels[i].green);
            sa.extend(pixels[i].blue);
            sa.extend(pixels[i].alpha);
        }
        sa.mapAllPositions();
        size_t t2 = System::getCurrentTimeNano();
        StringTools::println("%lluns", t2-t1);
    }
}

void tryCannyEdgeDetection()
{
    Sprite s;
    s.loadImage("Large_Scaled_Forest_Lizard.bmp");
    if(s.getSize() > 0)
    {
        Image* newImage = SimpleGraphics::cannyEdgeFilter(s.getImage(0), 0.09, 0.21);
        if(newImage != nullptr)
        {
            newImage->savePNG("TestCanny.png", false);
            delete newImage;
        }
    }
}

int main(int argc, char** argv)
{
    //TODO: Add all patterns found for SuffixAutomaton. (Should give all start indicies) (Post processing function O(N))
    //TODO: Add min and max adjustments for canny edge filter
    // testWebClient();
    // testQuadTreeLikeCompression();
    // testCopyDirectory();
    // testLZ77CSA();
    // quickCompression();
    // streamCompressTest();
    // streamDecompressTest();

    testSerialization();
    // testGuiPart2();
    // saStuff();
    // tryCannyEdgeDetection();
    return 0;
}