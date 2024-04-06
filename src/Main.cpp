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

        if(avgDirection.getLength() < 0.01)
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
    imgs.loadImage("TestImages/examples/zjkp5ph9huj11.png");

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
        for(int i=4; i>=4; i/=2)
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
            newImg.saveBMP("TestImages/examples/QuadTreeLikeCompression_" + StringTools::toString(i) + "_GradTest_2.bmp");
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

int main(int argc, char** argv)
{
    // testWebClient();
    // testQuadTreeLikeCompression();
    testCopyDirectory();
    return 0;
}