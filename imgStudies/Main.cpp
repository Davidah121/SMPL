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

Vec3f testMode0(Image* img, int startX, int startY, int size)
{
    Vec3f vdc;
    for(int x=startX; x<startX+size; x++)
    {
        Color samplePixel1 = img->getPixels()[x + startY*img->getWidth()];
        vdc.x += samplePixel1.red;
        vdc.y += samplePixel1.green;
        vdc.z += samplePixel1.blue;
    }
    for(int y=startY; y<startY+size; y++)
    {
        Color samplePixel1 = img->getPixels()[startX + y*img->getWidth()];
        vdc.x += samplePixel1.red;
        vdc.y += samplePixel1.green;
        vdc.z += samplePixel1.blue;
    }
    vdc.x /= 2*size;
    vdc.y /= 2*size;
    vdc.z /= 2*size;

    Vec3f err;
    for(int y = startY+1; y < startY+size; y++)
    {
        for(int x = startX+1; x < startX+size; x++)
        {
            Color c = img->getPixels()[x + y*img->getWidth()];
            err.x += MathExt::sqr(vdc.x - c.red);
            err.y += MathExt::sqr(vdc.y - c.green);
            err.z += MathExt::sqr(vdc.z - c.blue);
        }
    }

    err.x /= (size-1)*(size-1);
    err.y /= (size-1)*(size-1);
    err.z /= (size-1)*(size-1);

    return err;
}
Vec3f testMode1(Image* img, int startX, int startY, int size)
{
    Vec3f err;
    for(int y = startY+1; y < startY+size; y++)
    {
        for(int x = startX+1; x < startX+size; x++)
        {
            Color samplePixel1, samplePixel2, samplePixel3, samplePixel4;

            samplePixel1 = img->getPixels()[x + startY*img->getWidth()];
            samplePixel2 = img->getPixels()[startX + y*img->getWidth()];
            
            if(startY+size < img->getHeight())
                samplePixel3 = img->getPixels()[x + (startY+size)*img->getWidth()];
            else
                samplePixel3 = samplePixel1;
            
            if(startX+size < img->getWidth())
                samplePixel4 = img->getPixels()[(startX+size) + y*img->getWidth()];
            else
                samplePixel4 = samplePixel2;
            
            //bilinear interpolation
            double xPercentage = (double)(x-startX) / size;
            double yPercentage = (double)(y-startY) / size;
            
            Color finalC1 = SimpleGraphics::lerp(samplePixel1, samplePixel3, xPercentage);
            Color finalC2 = SimpleGraphics::lerp(samplePixel2, samplePixel4, yPercentage);
            Color finalC3 = SimpleGraphics::lerp(finalC1, finalC2, 0.5);

            Color c = img->getPixels()[x + y*img->getWidth()];
            err.x += MathExt::sqr((double)c.red - (double)finalC3.red);
            err.y += MathExt::sqr((double)c.green - (double)finalC3.green);
            err.z += MathExt::sqr((double)c.blue - (double)finalC3.blue);
        }
    }
    
    err.x /= (size-1)*(size-1);
    err.y /= (size-1)*(size-1);
    err.z /= (size-1)*(size-1);
    return err;
}

void gradientMode0(Image* img, int startX, int startY, int size, BinarySet& output)
{
    Vec3f vdc = Vec3f(size, size, size);
    for(int x=startX; x<startX+size; x++)
    {
        Color samplePixel1 = img->getPixels()[x + startY*img->getWidth()];
        output.add(samplePixel1.red);
        output.add(samplePixel1.green);
        output.add(samplePixel1.blue);
        
        vdc.x += samplePixel1.red;
        vdc.y += samplePixel1.green;
        vdc.z += samplePixel1.blue;
    }
    for(int y=startY; y<startY+size; y++)
    {
        Color samplePixel1 = img->getPixels()[startX + y*img->getWidth()];
        output.add(samplePixel1.red);
        output.add(samplePixel1.green);
        output.add(samplePixel1.blue);
        
        vdc.x += samplePixel1.red;
        vdc.y += samplePixel1.green;
        vdc.z += samplePixel1.blue;
    }
    output.add((unsigned char)0);

    vdc.x /= 2*size;
    vdc.y /= 2*size;
    vdc.z /= 2*size;

    Color c;
    c.red = MathExt::clamp(MathExt::round(vdc.x), 0.0, 255.0);
    c.green = MathExt::clamp(MathExt::round(vdc.y), 0.0, 255.0);
    c.blue = MathExt::clamp(MathExt::round(vdc.z), 0.0, 255.0);
    c.alpha = 255;
    for(int y = startY+1; y < startY+size; y++)
    {
        for(int x = startX+1; x < startX+size; x++)
        {
            img->getPixels()[x + y*img->getWidth()] = c;
        }
    }
}

void gradientMode1(Image* img, int startX, int startY, int size, BinarySet& output)
{
    for(int x=startX; x<startX+size; x++)
    {
        Color samplePixel1 = img->getPixels()[x + startY*img->getWidth()];
        output.add(samplePixel1.red);
        output.add(samplePixel1.green);
        output.add(samplePixel1.blue);
    }
    for(int y=startY; y<startY+size; y++)
    {
        Color samplePixel1 = img->getPixels()[startX + y*img->getWidth()];
        output.add(samplePixel1.red);
        output.add(samplePixel1.green);
        output.add(samplePixel1.blue);
    }
    output.add((unsigned char)1);

    for(int y = startY+1; y < startY+size; y++)
    {
        for(int x = startX+1; x < startX+size; x++)
        {
            Color samplePixel1, samplePixel2, samplePixel3, samplePixel4;

            samplePixel1 = img->getPixels()[x + startY*img->getWidth()];
            samplePixel2 = img->getPixels()[startX + y*img->getWidth()];
            
            if(startY+size < img->getHeight())
                samplePixel3 = img->getPixels()[x + (startY+size)*img->getWidth()];
            else
                samplePixel3 = samplePixel1;
            
            if(startX+size < img->getWidth())
                samplePixel4 = img->getPixels()[(startX+size) + y*img->getWidth()];
            else
                samplePixel4 = samplePixel2;
            
            //bilinear interpolation
            double xPercentage = (double)(x-startX) / size;
            double yPercentage = (double)(y-startY) / size;
            
            Color finalC1 = SimpleGraphics::lerp(samplePixel1, samplePixel3, xPercentage);
            Color finalC2 = SimpleGraphics::lerp(samplePixel2, samplePixel4, yPercentage);
            Color finalC3 = SimpleGraphics::lerp(finalC1, finalC2, 0.5);
            img->getPixels()[x + y*img->getWidth()] = finalC3;
        }
    }
}

void blockTruncationCoding(Image* img, int startX, int startY, int size, Vec3f avgs, Vec3f stddevs, Vec3f greaterCount, BinarySet& output)
{
    //everything in this region is just the average
    int totalSize = size*size;
    Vec3f maxCV = Vec3f(avgs.x, avgs.y, avgs.z);
    Vec3f minCV = Vec3f(avgs.x, avgs.y, avgs.z);
    if(greaterCount.x != 0)
        maxCV.x += stddevs.x*MathExt::sqrt((totalSize - greaterCount.x) / greaterCount.x);
    if(greaterCount.y != 0)
        maxCV.y += stddevs.y*MathExt::sqrt((totalSize - greaterCount.y) / greaterCount.y);
    if(greaterCount.z != 0)
        maxCV.z += stddevs.z*MathExt::sqrt((totalSize - greaterCount.z) / greaterCount.z);

    if(greaterCount.x != 0)
        minCV.x -= stddevs.x*MathExt::sqrt(greaterCount.x / (totalSize - greaterCount.x));
    if(greaterCount.y != 0)
        minCV.y -= stddevs.y*MathExt::sqrt(greaterCount.y / (totalSize - greaterCount.y));
    if(greaterCount.z != 0)
        minCV.z -= stddevs.z*MathExt::sqrt(greaterCount.z / (totalSize - greaterCount.z));
    
    Color maxC;
    maxC.red = (unsigned char)MathExt::clamp( MathExt::round(maxCV.x), 0.0, 255.0);
    maxC.green = (unsigned char)MathExt::clamp( MathExt::round(maxCV.y), 0.0, 255.0);
    maxC.blue = (unsigned char)MathExt::clamp( MathExt::round(maxCV.z), 0.0, 255.0);
    maxC.alpha = 255;
    
    Color minC;
    minC.red = (unsigned char)MathExt::clamp( MathExt::round(minCV.x), 0.0, 255.0);
    minC.green = (unsigned char)MathExt::clamp( MathExt::round(minCV.y), 0.0, 255.0);
    minC.blue = (unsigned char)MathExt::clamp( MathExt::round(minCV.z), 0.0, 255.0);
    minC.alpha = 255;

    output.add(minC.red);
    output.add(minC.green);
    output.add(minC.blue);
    output.add(maxC.red);
    output.add(maxC.green);
    output.add(maxC.blue);

    for(int y = startY; y < startY+size; y++)
    {
        for(int x = startX; x < startX+size; x++)
        {
            // Color c = {(unsigned char)avgR, (unsigned char)avgG, (unsigned char)avgB, 255}; //should do actual rounding
            Color oriC = img->getPixels()[x + y*img->getWidth()];
            Color finalC = minC;
            if(oriC.red > avgs.x)
            {
                output.add(true);
                finalC.red = maxC.red;
            }
            else
            {
                output.add(false);
            }
            if(oriC.green > avgs.y)
            {
                output.add(true);
                finalC.green = maxC.green;
            }   
            else
            {
                output.add(false);
            }
            if(oriC.blue > avgs.z)
            {
                output.add(true);
                finalC.blue = maxC.blue;
            }   
            else
            {
                output.add(false);
            }

            img->getPixels()[x + y*img->getWidth()] = finalC;
        }
    }
}

void justAvg(Image* img, int startX, int startY, int size, Vec3f avg, BinarySet& output)
{
    Color finalC;
    finalC.red = (unsigned char)MathExt::clamp(MathExt::round(avg.x), 0.0, 255.0);
    finalC.green = (unsigned char)MathExt::clamp(MathExt::round(avg.y), 0.0, 255.0);
    finalC.blue = (unsigned char)MathExt::clamp(MathExt::round(avg.z), 0.0, 255.0);
    finalC.alpha = 255;

    output.add(finalC.red);
    output.add(finalC.green);
    output.add(finalC.blue);

    for(int y = startY; y < startY+size; y++)
    {
        for(int x = startX; x < startX+size; x++)
        {
            img->getPixels()[x + y*img->getWidth()] = finalC;
        }
    }
}

void recordFlatBlock(Image* img, int startX, int startY, int size)
{
    for(int y = startY+1; y < startY+size; y++)
    {
        for(int x = startX+1; x < startX+size; x++)
        {
            img->getPixels()[x + y*img->getWidth()] = {0, 0, 0, 255};
        }
    }
}

void recursivePassAvg(Image* img, int startX, int startY, int size, int lowestSize, double threshold, BinarySet& output)
{
    subDivisions++;
    int totalSize = size*size;
    //get avg
    double avgR = 0;
    double avgG = 0;
    double avgB = 0;
    for(int y = startY; y < startY+size; y++)
    {
        for(int x = startX; x < startX+size; x++)
        {
            Color c = img->getPixels()[x + y*img->getWidth()];
            avgR += c.red;
            avgG += c.green;
            avgB += c.blue;
        }
    }

    avgR /= totalSize;
    avgG /= totalSize;
    avgB /= totalSize;

    //get Variance (MeanSquaredError)
    double errR = 0;
    double errG = 0;
    double errB = 0;
    for(int y = startY; y < startY+size; y++)
    {
        for(int x = startX; x < startX+size; x++)
        {
            Color c = img->getPixels()[x + y*img->getWidth()];
            errR += MathExt::sqr( avgR - c.red );
            errG += MathExt::sqr( avgG - c.green );
            errB += MathExt::sqr( avgB - c.blue );
        }
    }

    errR /= totalSize;
    errG /= totalSize;
    errB /= totalSize;
    
    //check against threshold
    if(size > lowestSize && (errR > threshold || errG > threshold || errB > threshold))
    {
        //split further
        int newSize = size/2;
        output.add(true);
        recursivePassAvg(img, startX, startY, newSize, lowestSize, threshold, output);
        recursivePassAvg(img, startX + newSize, startY, newSize, lowestSize, threshold, output);
        recursivePassAvg(img, startX, startY + newSize, newSize, lowestSize, threshold, output);
        recursivePassAvg(img, startX + newSize, startY + newSize, newSize, lowestSize, threshold, output);
    }
    else
    {
        output.add(false);
        //everything in this region is just the average
        justAvg(img, startX, startY, size, Vec3f(avgR, avgG, avgB), output);
        int index = MathExt::ceil(MathExt::log((double)size, 2.0));
        subDivisionsPerSize[index]++;
    }
}

void recursivePassBlockTrunc(Image* img, int startX, int startY, int size, int lowestSize, double threshold, BinarySet& output)
{
    subDivisions++;
    int totalSize = size*size;
    //get avg
    double avgR = 0;
    double avgG = 0;
    double avgB = 0;
    for(int y = startY; y < startY+size; y++)
    {
        for(int x = startX; x < startX+size; x++)
        {
            Color c = img->getPixels()[x + y*img->getWidth()];
            avgR += c.red;
            avgG += c.green;
            avgB += c.blue;
        }
    }

    avgR /= totalSize;
    avgG /= totalSize;
    avgB /= totalSize;

    //get std-deviation
    double errR = 0;
    double errG = 0;
    double errB = 0;
    Vec3f greaterCounter;
    for(int y = startY; y < startY+size; y++)
    {
        for(int x = startX; x < startX+size; x++)
        {
            Color c = img->getPixels()[x + y*img->getWidth()];
            errR += MathExt::sqr( avgR - c.red );
            errG += MathExt::sqr( avgG - c.green );
            errB += MathExt::sqr( avgB - c.blue );
            
            if(c.red > avgR)
                greaterCounter.x++;
            if(c.green > avgG)
                greaterCounter.y++;
            if(c.blue > avgB)
                greaterCounter.z++;
        }
    }

    errR = MathExt::sqrt(errR / totalSize);
    errG = MathExt::sqrt(errG / totalSize);
    errB = MathExt::sqrt(errB / totalSize);
    
    //check against threshold
    if(size > lowestSize && (errR > threshold || errG > threshold || errB > threshold))
    {
        //split further
        int newSize = size/2;
        output.add(true);
        recursivePassBlockTrunc(img, startX, startY, newSize, lowestSize, threshold, output);
        recursivePassBlockTrunc(img, startX + newSize, startY, newSize, lowestSize, threshold, output);
        recursivePassBlockTrunc(img, startX, startY + newSize, newSize, lowestSize, threshold, output);
        recursivePassBlockTrunc(img, startX + newSize, startY + newSize, newSize, lowestSize, threshold, output);
    }
    else
    {
        output.add(false);
        //everything in this region is just the average
        blockTruncationCoding(img, startX, startY, size, Vec3f(avgR, avgG, avgB), Vec3f(errR, errG, errB), greaterCounter, output);
        int index = MathExt::ceil(MathExt::log((double)size, 2.0));
        subDivisionsPerSize[index]++;
    }
}

void recursivePassGrad(Image* img, int startX, int startY, int size, int lowestSize, double threshold, BinarySet& output)
{
    subDivisions++;
    int totalSize = size*size;
    
    //get MeanSquaredError
    Vec3f mode0Err = testMode0(img, startX, startY, size);
    Vec3f mode1Err = testMode1(img, startX, startY, size);

    // StringTools::println("(%d, %d) - MODE0 - (%.3f, %.3f, %.3f)", startX, startY, mode0Err.x, mode0Err.y, mode0Err.z);
    // StringTools::println("(%d, %d) - MODE1 - (%.3f, %.3f, %.3f)", startX, startY, mode1Err.x, mode1Err.y, mode1Err.z);
    
    bool mode0TooHigh = (mode0Err.x > threshold) || (mode0Err.y > threshold) || (mode0Err.z > threshold);
    bool mode1TooHigh = (mode1Err.x > threshold) || (mode1Err.y > threshold) || (mode1Err.z > threshold);
    
    
    //check against threshold
    if(size > lowestSize && (mode0TooHigh && mode0TooHigh))
    {
        //split further
        int newSize = size/2;
        output.add(true);
        recursivePassGrad(img, startX, startY, newSize, lowestSize, threshold, output);
        recursivePassGrad(img, startX + newSize, startY, newSize, lowestSize, threshold, output);
        recursivePassGrad(img, startX, startY + newSize, newSize, lowestSize, threshold, output);
        recursivePassGrad(img, startX + newSize, startY + newSize, newSize, lowestSize, threshold, output);
    }
    else
    {
        output.add(false);
        //do gradient stuff.
        if(!mode0TooHigh)
            gradientMode0(img, startX, startY, size, output);
        else
        {
            if(mode0Err.getLength() < mode1Err.getLength())
                gradientMode0(img, startX, startY, size, output);
            else
                gradientMode1(img, startX, startY, size, output);
        }
        int index = MathExt::ceil(MathExt::log((double)size, 2.0));
        subDivisionsPerSize[index]++;
    }
}

void recursivePass(Image* img, int startX, int startY, int size, int lowestSize, double threshold, BinarySet& output, int type)
{
    if(type == 0)
        recursivePassAvg(img, startX, startY, size, lowestSize, threshold, output);
    else if(type == 1)
        recursivePassBlockTrunc(img, startX, startY, size, lowestSize, threshold, output);
    else if(type == 2)
        recursivePassGrad(img, startX, startY, size, lowestSize, threshold, output);
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
        for(int i=64; i>=2; i/=2)
        {
            Image newImg = Image();
            BinarySet output;
            newImg.copyImage(imgs.getImage(0));

            for(int y=0; y<newImg.getHeight(); y++)
            {
                for(int x=0; x<newImg.getWidth(); x++)
                {
                    Color c = newImg.getPixels()[x + y*newImg.getWidth()];
                    newImg.getPixels()[x + y*newImg.getWidth()] = ColorSpaceConverter::convert(c, ColorSpaceConverter::RGB_TO_YCBCR);
                }
            }

            for(int y=0; y<newImg.getHeight(); y+=64)
            {
                for(int x=0; x<newImg.getWidth(); x+=64)
                {
                    recursivePass(&newImg, x, y, 64, i, 1.0, output, 2);
                }
            }
            std::vector<unsigned char> compressedData = Compression::compressDeflate(output.getByteRef(), 1);

            Vec3f err;
            for(int y=0; y<newImg.getHeight(); y++)
            {
                for(int x=0; x<newImg.getWidth(); x++)
                {
                    Color c = newImg.getPixels()[x + y*newImg.getWidth()];
                    newImg.getPixels()[x + y*newImg.getWidth()] = ColorSpaceConverter::convert(c, ColorSpaceConverter::YCBCR_TO_RGB);

                    Color newColor = newImg.getPixels()[x+y*newImg.getWidth()];
                    Color oriColor = imgs.getImage(0)->getPixels()[x+y*newImg.getWidth()];

                    err.x += MathExt::sqr((double)oriColor.red - (double)newColor.red) / (newImg.getWidth()*newImg.getHeight());
                    err.y += MathExt::sqr((double)oriColor.green - (double)newColor.green) / (newImg.getWidth()*newImg.getHeight());
                    err.z += MathExt::sqr((double)oriColor.blue - (double)newColor.blue) / (newImg.getWidth()*newImg.getHeight());
                }
            }
            StringTools::println("MEAN SQUARED ERROR: (%.3f, %.3f, %.3f)", err.x, err.y, err.z);
            StringTools::println("Threshold = %.3f, Lowest Subdivision Size = %d, Subdivisions: %llu", 1.0, i, subDivisions);
            
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

int main(int argc, char** argv)
{
    // testWebClient();
    testQuadTreeLikeCompression();
    return 0;
}