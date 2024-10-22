#include "StringTools.h"
#include "System.h"
#include "SimpleWindow.h"
#include <atomic>
#include "Sprite.h"
#include "SimpleGraphics.h"
#include "Compression.h"
#include "ComputerVision.h"
#include "SimpleJobQueue.h"

using namespace smpl;

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
    avgColor.red = MathExt::clamp(MathExt::round(avgColorVec.x), 0.0f, 255.0f);
    avgColor.green = MathExt::clamp(MathExt::round(avgColorVec.y), 0.0f, 255.0f);
    avgColor.blue = MathExt::clamp(MathExt::round(avgColorVec.z), 0.0f, 255.0f);
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
            minDot = perpendicularDirection.dot(Vec2f(1, 0));
            // minDot = MathExt::dot(perpendicularDirection, Vec2f(1, 0));
            v = 1;
            
            d = perpendicularDirection.dot(Vec2f(1, 1));
            // d = MathExt::dot(perpendicularDirection, Vec2f(1, 1));
            if(d < minDot)
            {
                v = 2;
            }

            d = perpendicularDirection.dot(Vec2f(0, 1));
            // d = MathExt::dot(perpendicularDirection, Vec2f(0, 1));
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

        Matrix imgAsMatrix = ComputerVision::imageToMatrix(imgs.getImage(0), ComputerVision::RED_CHANNEL);

        Matrix imgXDerivative = ComputerVision::convolution(imgAsMatrix, gx);
        Matrix imgYDerivative = ComputerVision::convolution(imgAsMatrix, gy);

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

#define FACTORY_HELPER(className) class className ## Factory : public SerializedFactory\
{\
public:\
    SerializedData createInstance(Streamable<unsigned char>* data);\
protected:\
    className ## Factory() : SerializedFactory(&className::globalClass) {}\
private:\
    static className ## Factory singleton;\
}\

FACTORY_HELPER(Vec2f);
Vec2fFactory Vec2fFactory::singleton = Vec2fFactory();
SerializedData Vec2fFactory::createInstance(Streamable<unsigned char>* data)
{
    return SerializedData();
}

class testClass1
{
public:
    testClass1()
    {
        StringTools::println("CREATE T1");
    }
    ~testClass1()
    {
        StringTools::println("DESTORY T1");
    }
};

class testClass2
{
public:
    testClass2()
    {
        StringTools::println("CREATE T2");
    }
    ~testClass2()
    {
        StringTools::println("DESTORY T2");
    }
};

// class GenericSmartMemory
// {
// public:
//     virtual ~GenericSmartMemory() {}
// };

// template<typename T>
// class TestSmartMemory : public GenericSmartMemory
// {
// public:
//     TestSmartMemory(T* pointer)
//     {
//         data = pointer;
//     }
//     ~TestSmartMemory()
//     {
//         if(data != nullptr)
//             delete data;
//         data = nullptr;
//     }
// private:
//     T* data = nullptr;
// };

void testSerialization()
{
    // Vec2f p;
    // p.x = 0.2;
    // p.y = 3.1;
    
    // StreamableVector<unsigned char> outputData;
    // SerializedData pSerialized = SERIALIZE(p);
    // pSerialized.serialize(&outputData);

    // SerializedFactory* factory = SerializedFactoryMapper::getInstance().getFactory(&Vec2f::globalClass);
    // if(factory != nullptr)
    // {
    //     SerializedData sd = factory->createInstance(nullptr);
    //     Vec2f* p = (Vec2f*)sd.getData();

    //     StringTools::println("%.3f, %.3f", p->x, p->y);
    //     delete p;
    // }

    SmartMemory<testClass1> v = SmartMemory<testClass1>::createDeleteOnLast(new testClass1());
    SmartMemory<testClass2> v2 = SmartMemory<testClass2>::createDeleteOnLast(new testClass2());

    StringTools::println("%p", v.getRawPointer());
    StringTools::println("%p", v2.getRawPointer());

    GenericSmartMemory testV = v;
    StringTools::println("%p", testV.getRawPointer());


    // SmartMemory<testClass1> mem1 = SmartMemory<testClass1>::createDeleteRights(new testClass1(), false);

    
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

void quickBezierTest()
{
    BezierCurve b = BezierCurve();
    b.addPoint(0, 1);
    b.addPoint(1.614, 0.333);
    b.addPoint(1.84, 2.13);
    b.addPoint(0.805, 0.228);

    std::vector<BezierCurve> curveSubdivisions = b.subdivide(0.5);
    for(BezierCurve& curve : curveSubdivisions)
    {
        StringTools::println("CURVE: ");
        for(Vec2f& p : curve.getPoints())
        {
            StringTools::println("\t(%.3f, %.3f)", p.x, p.y);
        }
    }
}

void testDrawEfficiency()
{
    SimpleGraphics::setAntiAliasing(true);
    Image buffer = Image(512, 512);
    Sprite loadedSprite = Sprite();
    loadedSprite.loadImage("Large_Scaled_Forest_Lizard.jpg");
    //13256100ns with no simd
    //6584300ns with sse
    //9319300ns with avx
    if(loadedSprite.getSize()>0)
        loadedSprite.getImage(0)->savePNG("JPEGLOADTEST.png", true, false, true);
    // buffer.saveBMP("testbmp.bmp", 128);
    
    // Image buffer = Image(1920, 1080);
    // Image drawImg = Image(1920, 1080);

    // //currently at 36 images per frame.
    // for(int i=0; i<10; i++)
    // {
    //     size_t count = 0;
    //     size_t t1 = System::getCurrentTimeMicro();
    //     size_t timeUsed = 0;
    //     while(true)
    //     {
    //         size_t t2 = System::getCurrentTimeMicro();
    //         if(t2-t1 >= 16666)
    //         {
    //             timeUsed = t2-t1;
    //             break;
    //         }
    //         SimpleGraphics::setColor(Color{255,255,255,255});
    //         SimpleGraphics::drawSprite(&drawImg, 0, 0, &buffer);
    //         count++;
    //     }
    //     StringTools::println("Total Time Used: %llu", timeUsed);
    //     StringTools::println("ImagesDrawn = %llu", count);
    // }
    // system("pause");
}

void contourRecursiveCall(Image* img, int x, int y, int preX, int preY, std::vector<Vec2f>& points)
{
    //moore-neighbor tracing
    const std::pair<int, int> boundaryPointsClockwise[8] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};

    Vec2f newPoint = Vec2f(x, y);
    Vec2f enterPoint = Vec2f(preX, preY);

    while(true)
    {
        if(newPoint.x < 0 || newPoint.y < 0)
            break;

        //if this point is already in the list of points, return
        for(int i=0; i<points.size(); i++)
        {
            if(newPoint == points[i])
                return;
        }
        
        points.push_back(newPoint);

        //find start of the boundary checks
        int tempX = enterPoint.x - newPoint.x;
        int tempY = enterPoint.y - newPoint.y;
        int startP = -1;
        for(int i=0; i<8; i++)
        {
            if(tempX == boundaryPointsClockwise[i].first && tempY == boundaryPointsClockwise[i].second)
            {
                startP = i % 8;
                break;
            }
        }

        if(startP < 0)
            return; //problem
        
        Vec2f nextPotentialPoint = {-1, -1};
        int oldCheckX, oldCheckY;
        for(int i=0; i<8; i++)
        {
            //move around pixel clockwise till a white pixel is found (white being a valid pixel and black being background).
            std::pair<int, int> adjustments = boundaryPointsClockwise[startP];
            int checkX = newPoint.x + adjustments.first;
            int checkY = newPoint.y + adjustments.second;

            if(checkX >= img->getWidth() || checkX < 0 || checkY >= img->getHeight() || checkY < 0)
            {
                //do nothing
            }
            else
            {
                //assume grayscale
                Color c = img->getPixels()[checkX + checkY*img->getWidth()];
                if(c.red > 0)
                {
                    //valid pixel. Part of boundary.
                    nextPotentialPoint = Vec2f(checkX, checkY);
                    enterPoint = Vec2f(oldCheckX, oldCheckY);
                    break;
                }
            }

            oldCheckX = checkX;
            oldCheckY = checkY;
            startP = (startP+1) % 8;
        }

        newPoint = nextPotentialPoint;
    }
}

std::vector<std::vector<Vec2f>> findContours(Image* img)
{
    SmartMemory<Image> edgeImg;
    edgeImg = SmartMemory<Image>::createDeleteOnLast(SimpleGraphics::cannyEdgeFilter(img, 1.0, 0.2, 0.5));
    Image copyImg;
    copyImg.copyImage(edgeImg.getRawPointer());

    std::vector<std::vector<Vec2f>> allShapesFound;
    for(int y=0; y<copyImg.getHeight(); y++)
    {
        for(int x=0; x<copyImg.getWidth(); x++)
        {
            if(copyImg.getPixels()[x + y*copyImg.getWidth()].red == 255)
            {
                std::vector<Vec2f> boundaryPoints;
                contourRecursiveCall(&copyImg, x, y, x-1, y, boundaryPoints);

                if(boundaryPoints.size() > 0)
                {
                    StringTools::println("FOUND SOMETHING");
                    allShapesFound.push_back(boundaryPoints);
                    //remove the polygon formed by connecting the points together with lines. Assume even-odd rule
                    //Gonna cheat a little and use existing code
                    SimpleGraphics::setColor({0, 0, 0, 0});
                    SimpleGraphics::setBlendMode(SimpleGraphics::NO_COMPOSITE);
                    SimpleGraphics::setFillRule(SimpleGraphics::FILL_EVEN_ODD);
                    SimpleGraphics::drawPolygon(boundaryPoints.data(), boundaryPoints.size(), &copyImg);

                    for(int i=0; i<boundaryPoints.size(); i++)
                    {
                        img->setPixel(boundaryPoints[i].x+1, boundaryPoints[i].y, {255, 0, 0, 255});
                    }
                    return allShapesFound;
                }
            }
        }
    }

    return allShapesFound;
}

void removeBoundaryShape2(std::vector<Vec2f>& boundaryShape)
{
    std::vector<std::vector<std::pair<int, char>>> criticalPoints = std::vector<std::vector<std::pair<int, char>>>(32);
    std::vector<std::vector<std::pair<int, int>>> horizontalLines = std::vector<std::vector<std::pair<int, int>>>(32);
    // Vec2f lastToPoint = Vec2f();
    // for(int i=0; i<boundaryShape.size(); i++)
    // {
    //     int p1 = i;
    //     int p2 = (i+1) % boundaryShape.size();
    //     Vec2f v1 = boundaryShape[p1];
    //     Vec2f v2 = boundaryShape[p2];
    //     Vec2f toPoint = boundaryShape[p2] - boundaryShape[p1];
    //     if(toPoint.y == 0)
    //     {
    //         // criticalPoints[(int)v1.y].push_back({(int)v1.x, 0});
    //         // criticalPoints[(int)v2.y].push_back({(int)v2.x, 0});
    //     }
    //     else if(toPoint.y > 0)
    //     {
    //         criticalPoints[(int)v1.y].push_back({(int)v1.x, 1});
    //         if(i == boundaryShape.size()-1)
    //             criticalPoints[(int)v2.y].push_back({(int)v2.x, 1});
    //     }
    //     else
    //     {
    //         criticalPoints[(int)v1.y].push_back({(int)v1.x, -1});
    //         if(i == boundaryShape.size()-1)
    //             criticalPoints[(int)v2.y].push_back({(int)v2.x, -1});
    //     }
    //     if(toPoint.y * lastToPoint.y < 0)
    //     {
    //         criticalPoints[(int)v1.y].push_back({(int)v1.x, -2}); //duplication
    //     }
    //     lastToPoint = toPoint;
    // }

    for(int i=0; i<boundaryShape.size(); i++)
    {
        int p1 = i;
        int p2 = (i+1) % boundaryShape.size();
        Vec2f v1 = boundaryShape[p1];
        Vec2f v2 = boundaryShape[p2];
        Vec2f toPoint = v2-v1;
        BezierCurve b;
        b.addPoint(v1);
        b.addPoint(v2);

        if(toPoint.y == 0)
        {
            //special case
            if(v1.y >= 0 && v1.y < criticalPoints.size())
            {
                horizontalLines[(int)v1.y].push_back({(int)v1.x, (int)v2.x});
            }
            continue;
        }

        for(int y=0; y<criticalPoints.size(); y++)
        {
            std::vector<double> timeValues = b.findTimeForY(y);
            for(double& d : timeValues)
            {
                if(d >= 0 && d <= 1)
                {
                    Vec2f intersection = b.getFuctionAt(d);
                    if(toPoint.y > 0)
                    {
                        if(d > 0 && d < 1)
                            criticalPoints[y].push_back({(int)intersection.x, 1});
                        else
                            criticalPoints[y].push_back({(int)intersection.x, 2});
                    }
                    else if(toPoint.y < 0)
                    {
                        if(d > 0 && d < 1)
                            criticalPoints[y].push_back({(int)intersection.x, -1});
                        else
                            criticalPoints[y].push_back({(int)intersection.x, -2});
                    }
                }
            }
        }
    }

    for(int y=0; y<criticalPoints.size(); y++)
    {
        StringTools::println("%d : %llu", y, criticalPoints[y].size());
        Sort::insertionSort<std::pair<int, char>>(criticalPoints[y].data(), criticalPoints[y].size(), [](std::pair<int, char> A, std::pair<int, char> B) ->bool{
            if(A.first == B.first)
                return A.second < B.second;
            return A.first < B.first;
        });


        std::vector<int> reducedPoints = std::vector<int>();
        int lastSlope = 0;
        // bool addedLast = false;
        for(int k=0; k<criticalPoints[y].size(); k++)
        {
            //no duplicates. if 2 or -2, must include. Its on a vertex

            StringTools::println("\t(%d, %d)", criticalPoints[y][k].first, criticalPoints[y][k].second);
            //make sure it wasn't the same as the last one
            if(k > 0 && criticalPoints[y][k-1] == criticalPoints[y][k])
            {
                //skip this
                continue;
            }

            //check last slope vs this one. If the same, ignore. If opposite, include.
            //only if the points are connected via a horizontal line
            if(reducedPoints.size() > 0)
            {
                //check if last point added and the current point are connected via horizontal line
                bool connected = false;
                if(reducedPoints.size() > 0)
                {
                    for(std::pair<int, int>& potentialPair : horizontalLines[y])
                    {
                        if(potentialPair.first == criticalPoints[y][k].first)
                        {
                            if(potentialPair.second == reducedPoints.back())
                            {
                                connected = true;
                                break;
                            }
                        }
                        if(potentialPair.second == criticalPoints[y][k].first)
                        {
                            if(potentialPair.first == reducedPoints.back())
                            {
                                connected = true;
                                break;
                            }
                        }
                    }
                }

                if(connected)
                {
                    if(lastSlope != criticalPoints[y][k].second)
                    {
                        reducedPoints.push_back(criticalPoints[y][k].first);
                    }
                }
                else
                {
                    reducedPoints.push_back(criticalPoints[y][k].first);
                }
            }
            else
            {
                reducedPoints.push_back(criticalPoints[y][k].first);
            }
            lastSlope = criticalPoints[y][k].second;
        
        }

        for(int k=0; k<reducedPoints.size(); k++)
        {
            auto p = reducedPoints[k];
            StringTools::println("\t(%d)", p);
        }
    }
}

void contourTest()
{
    // Matrix kernel = ComputerVision::guassianKernel(2, 1);
    // for(int y=0; y<kernel.getRows(); y++)
    // {
    //     for(int x=0; x<kernel.getCols(); x++)
    //     {
    //         StringTools::print("%.4f\t", kernel[y][x]);
    //     }
    //     StringTools::println("");
    // }

    // // std::vector<Vec2f> validPoints = {Vec2f(0, 0), Vec2f(1,0), Vec2f(2,0), Vec2f(3,0), Vec2f(4,0), Vec2f(5, 0), Vec2f(5, 1), Vec2f(5, 2), Vec2f(5, 3), Vec2f(5, 4), Vec2f(4, 3), Vec2f(2, 3), Vec2f(1, 2), Vec2f(0, 3), Vec2f(0, 2), Vec2f(0, 1)};
    // // removeBoundaryShape2(validPoints);
    Sprite s;
    s.loadImage("./contourStuff/artifact.png");
    if(s.getSize() <= 0)
    {
        StringTools::println("FAILED TO LOAD");
        return;
    }
    //make grayscale


    Image* derivativeImg = SimpleGraphics::cannyEdgeFilter(s.getImage(0), 1, 0.05, 0.19);
    Matrix threshMatrix = ComputerVision::imageToMatrix(derivativeImg, 0);
    Image* threshImg = ComputerVision::matrixToImage(threshMatrix);

    StringTools::println("FINDING CONTOURS");
    std::vector<std::vector<Vec2f>> contours = ComputerVision::findContours(threshImg);
    StringTools::println("CONTOURS FOUND");

    SimpleGraphics::setColor({255, 0, 0, 255});
    for(std::vector<Vec2f>& boundary : contours)
    {
        Vec2f minPoint = Vec2f((float)INT_MAX, (float)INT_MAX);
        Vec2f maxPoint = Vec2f((float)INT_MIN, (float)INT_MIN);
        
        for(Vec2f& point : boundary)
        {
            minPoint.x = MathExt::min(minPoint.x, point.x);
            minPoint.y = MathExt::min(minPoint.y, point.y);
            maxPoint.x = MathExt::max(maxPoint.x, point.x);
            maxPoint.y = MathExt::max(maxPoint.y, point.y);
            s.getImage(0)->setPixel((int)point.x, (int)point.y, {255, 0, 0, 255});
        }
        // SimpleGraphics::drawRect((int)minPoint.x, (int)minPoint.y, (int)maxPoint.x, (int)maxPoint.y, true, s.getImage(0));
    }

    s.getImage(0)->savePNG("contourStuff/FoundContours.png", false);
    threshImg->savePNG("contourStuff/preProcess.png", false);
    delete threshImg;
    delete derivativeImg;
}

// #ifndef USE_OPENGL
//     #define USE_OPENGL
// #endif
// #include "ext/GLSingleton.h"
// #include "ext/GLGraphics.h"
// #include "ext/GLWindow.h"
// #include "Input.h"

// void openGLTest()
// {
//     GLWindow window = GLWindow("TestOPENGL", 640, 480);
//     window.setActivateGui(false);
//     window.setVSync(1);
//     int fps = 0;
//     size_t timeEllapsed = 0;

//     while(window.getRunning())
//     {
//         size_t startTime = System::getCurrentTimeMicro();
//         Input::pollInput();
//         window.update();

//         //other stuff
//         GLGraphics::setClearColor(Vec4f(1, 1, 1, 1));
//         GLGraphics::clear(GLGraphics::COLOR_BUFFER);
//         GLGraphics::setDrawColor(Vec4f(1, 0, 0, 1));
//         GLGraphics::drawRectangle(0, 0, 64, 64, false);
//         GLGraphics::setDrawColor(Vec4f(1, 0, 1, 1));
//         GLGraphics::drawCircle(64, 64, 32, false);

//         window.forceRepaint();
//         window.repaint();
//         size_t timeUsed = System::getCurrentTimeMicro()-startTime;

//         if(timeUsed < 16666) //1/60
//         {
//             size_t timeNeeded = 16666 - timeUsed;
//             System::sleep(0, timeNeeded, true);
//             timeEllapsed += 16666;
//         }
//         else
//         {
//             timeEllapsed += timeUsed;
//         }
//         fps++;

//         if(timeEllapsed >= 16666*60)
//         {
//             StringTools::println("FPS: %d - %llu", fps, timeEllapsed);
//             fps = 0;
//             timeEllapsed -= 16666*60;
//             if(timeEllapsed < 16666)
//                 timeEllapsed = 0; //less than 1 frame left. Consider it okay
//         }
//     }
// }

#include "Graph.h"
void testGraph()
{
    std::vector<double> heuristicValues = {0.0, 4.0, 2.0, 4.0, 4.5, 2.0, 0.0};
    UndirectedMatrixGraph graph = UndirectedMatrixGraph(7);
    graph.addEdge(0, 1, 1.5);
    graph.addEdge(1, 2, 2.0);
    graph.addEdge(2, 3, 3.0);

    graph.addEdge(3, 6, 4.0);
    graph.addEdge(0, 4, 2.0);
    graph.addEdge(4, 5, 3.0);
    graph.addEdge(5, 6, 2.0);

    //A* should give back {0, 4, 5, 6}. Not {0, 1, 2, 3, 6}
    PathInfo p = graph.shortestPath(0, 6, [&heuristicValues](size_t v1, size_t v2) -> double{
        return heuristicValues[v1];
    });

    StringTools::println("PathCost = %.3f", p.cost);
    StringTools::print("{");
    for(int i=0; i<p.path.size(); i++)
    {
        StringTools::print("  %llu  ", p.path[i]);
    }
    StringTools::println("}");
}

float fastExp4(float x)  // quartic spline approximation
{
    union { float f; int32_t i; } reinterpreter;

    reinterpreter.i = (int32_t)(12102203.0f*x) + 127*(1 << 23);
    int32_t m = (reinterpreter.i >> 7) & 0xFFFF;  // copy mantissa
    // empirical values for small maximum relative error (1.21e-5):
    reinterpreter.i += (((((((((((3537*m) >> 16)
        + 13668)*m) >> 18) + 15817)*m) >> 14) - 80470)*m) >> 11);
    return reinterpreter.f;
}

float fastExpOther(float x)
{
    const float a = (1<<22) / 0.69314718056;
    const int b = 127 * (1<<23);
    int r = a*x;
    int s = b+r;
    int t = b-r;
    return (*(float*)&s) / (*(float*)&t);
}

float fastExpOther2(float x)
{
    const float a = 12102203.0f;
    const int b = 127 * (1<<23) - 298765;
    int r = a*x;
    int s = b+r;
    return (*(float*)&s);
}

float fastExpOther3(float x)
{
    const float a = 12102203.0f;
    const int b = 127 * (1<<23);
    int r = a*x;
    int s = b+r;
    return (*(float*)&s);
}

float approxExp2(float x)
{
    double act = 1 + ((double)x / (1LL<<32));
    for(int i=0; i<32; i++)
        act *= act;
    return act;
}

__m128 powInt(__m128 x, int power)
{
    __m128 result = _mm_set1_ps(1.0f);
    for(int i=0; i<power; i++)
    {
        result = _mm_mul_ps(result, x);
    }
    return result;
}

__m128 exp(__m128 x)
{
    const __m128 divValue1 = _mm_set1_ps(1.0f/2.0f);
    const __m128 divValue2 = _mm_set1_ps(1.0f/6.0f);
    const __m128 divValue3 = _mm_set1_ps(1.0f/24.0f);
    const __m128 divValue4 = _mm_set1_ps(1.0f/120.0f);
    const __m128 divValue5 = _mm_set1_ps(1.0f/720.0f);

    __m128 approx = _mm_add_ps(_mm_set1_ps(1.0f), x);
    __m128 powValue = _mm_mul_ps(x, x);

    approx = _mm_add_ps(approx, _mm_mul_ps(divValue1, powValue));
    powValue = _mm_mul_ps(powValue, x);
    approx = _mm_add_ps(approx, _mm_mul_ps(divValue2, powValue));
    powValue = _mm_mul_ps(powValue, x);
    approx = _mm_add_ps(approx, _mm_mul_ps(divValue3, powValue));
    powValue = _mm_mul_ps(powValue, x);
    approx = _mm_add_ps(approx, _mm_mul_ps(divValue4, powValue));
    powValue = _mm_mul_ps(powValue, x);
    approx = _mm_add_ps(approx, _mm_mul_ps(divValue5, powValue));
    
    return approx;
}

float agm(float a, float b, int iterations)
{
    float lastA = a;
    float lastB = b;
    for(int i=0; i<iterations; i++)
    {
        float currA = (lastA + lastB)/2.0;
        float currB = sqrtf(lastA*lastB);

        lastA = currA;
        lastB = currB;
    }

    return lastA;
}

//pretty high accuracy.
//33 instructions. Approx Cycles = 126
//~4 cycles per instructions
__m128 ln(__m128 x)
{
    //let a = 1. Center around 1 such that ln(1) = 0
    //uses the ln(x + a) taylor series. Converges faster
    const int ONLY_EXPONENT = 0b01111111100000000000000000000000;
    const __m128 LN2  = _mm_set1_ps(0.69314718056);
    const __m128 DIV1 = _mm_set1_ps(1.0/3.0);
    const __m128 DIV2 = _mm_set1_ps(1.0/5.0);
    const __m128 DIV3 = _mm_set1_ps(1.0/7.0);
    const __m128 DIV4 = _mm_set1_ps(1.0/9.0);
    const __m128 DIV5 = _mm_set1_ps(1.0/11.0);
    const __m128 DIV6 = _mm_set1_ps(1.0/13.0);

    //extract the base 2 exponent
    __m128i exponent = _mm_srli_epi32(TYPE_PUN(x, __m128i), 23);
    exponent = _mm_and_si128(exponent, _mm_set1_epi32(0xFF));
    exponent = _mm_sub_epi32(exponent, _mm_set1_epi32(127));
    __m128 finalAdjustment = _mm_mul_ps(_mm_cvtepi32_ps(exponent), LN2);
    __m128 exponentAdjust = _mm_and_ps(x, _mm_set1_ps(TYPE_PUN(ONLY_EXPONENT, float)));

    x = _mm_sub_ps(_mm_div_ps(x, exponentAdjust), _mm_set1_ps(1));
    __m128 y = _mm_div_ps(x, _mm_add_ps(x, _mm_set1_ps(2)));

    __m128 approx = y;
    __m128 ySqr = _mm_mul_ps(y, y);
    __m128 yExp = _mm_mul_ps(y, ySqr);

    approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV1));
    yExp = _mm_mul_ps(yExp, ySqr);
    approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV2));
    yExp = _mm_mul_ps(yExp, ySqr);
    approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV3));
    yExp = _mm_mul_ps(yExp, ySqr);
    approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV4));
    yExp = _mm_mul_ps(yExp, ySqr);
    approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV5));
    yExp = _mm_mul_ps(yExp, ySqr);
    approx = _mm_add_ps(approx, _mm_mul_ps(yExp, DIV6));

    approx = _mm_mul_ps(approx, _mm_set1_ps(2));
    return _mm_add_ps(approx, finalAdjustment);
}

__m128 log2(__m128 x)
{
    const __m128 adjustment = _mm_set1_ps(log(E)/log(2));
    return _mm_mul_ps(ln(x), adjustment);
}

__m128 log10(__m128 x)
{
    const __m128 adjustment = _mm_set1_ps(log(E));
    return _mm_mul_ps(ln(x), adjustment);
}

__m128 logBase(__m128 x, float base)
{
    __m128 adjustment = _mm_set1_ps(log(E) / log(base));
    return _mm_mul_ps(ln(x), adjustment);
}

__m128 pow(__m128 x, __m128 y)
{
    //exp(y*ln(x))
    return exp( _mm_mul_ps(y, ln(x)) );
}

void approxFunctions()
{
    const size_t size = 174*256;
    // for(int polyDegree = 1; polyDegree <= 10; polyDegree++)
    // {
        float totalError = 0;
        float minError = 0xFFFFFF;
        float maxError = 0;
        for(size_t i=0; i<size; i++)
        {
            float x = -87 + (float)i/256;
            float exactValue = exp(x);
            // float approxValue = fastExp4(x);
            float approxValue = fastExpOther(x);
            // float approxValue = 1 + x;
            // float powValue = x*x;
            // for(int j=2; j<=polyDegree; j++)
            // {
            //     approxValue += powValue / MathExt::factorial(j);
            //     powValue *= x;
            // }

            // float currErr = MathExt::abs((approxValue - exactValue)/exactValue);
            float currErr = MathExt::abs(approxValue - exactValue);
            minError = MathExt::min(minError, currErr);
            maxError = MathExt::max(maxError, currErr);
            totalError += currErr;
        }
        // StringTools::println("Statistics for Taylor Series of Degree %d", polyDegree);
        StringTools::println("\tTOTAL ERROR = %.9f", totalError);
        StringTools::println("\tMIN ERROR = %.9f", minError);
        StringTools::println("\tMAX ERROR = %.9f", maxError);

        StringTools::println("EXACT = %.9f", exp(87));
        StringTools::println("APPROX1 = %.9f", approxExp2(87));
        StringTools::println("RELATIVE ERROR = %.9f", (approxExp2(87)-exp(87)) / exp(87));
        
    // }

    // const float LN2 = 0.69314718056f;
    // for(int i=0; i<10; i++)
    // {
    //     float totalError = 0;
    //     float minError = 0xFFFFFF;
    //     float maxError = 0;
    //     for(int j=2; j<size; j++)
    //     {
    //         float v = 1.0f/j;
    //         float s = v*256;
    //         float div = 2*agm(1, 4.0f/s, i);
    //         float approxValue = PI / div;
    //         approxValue -= 8*LN2;
    //         // if(div == 0)
    //         //     StringTools::println("%d", j);

    //         //compare approxValue to ln(v)
    //         float exactValue = std::log(v) / std::log(E);

    //         float currErr = MathExt::abs((approxValue - exactValue)/exactValue);
    //         minError = MathExt::min(minError, currErr);
    //         maxError = MathExt::max(maxError, currErr);
    //         totalError += currErr;
    //     }
    //     StringTools::println("Statistics for HALLEY's Method of Degree %d", i);
    //     StringTools::println("\tTOTAL ERROR = %.9f", totalError);
    //     StringTools::println("\tMIN ERROR = %.9f", minError);
    //     StringTools::println("\tMAX ERROR = %.9f", maxError);
    // }

    // float maxError = 0;

    // for(int i=0; i<1000; i++)
    // {
    //     float x = (float)i/100.0f;
    //     float approx = approxLN2_quadratic(x);
    //     float real = std::log2f(x);
    //     float currErr = MathExt::abs((approx - real)/real);
    //     // StringTools::println("RELATIVE ERROR ln(%.5f) = %.5f", x, currErr);
    //     maxError = __max(currErr, maxError);
    // }
    // StringTools::println("MAX ERROR = %.5f", maxError);

    // int v = 0b00111110100000000000000000000000;
    // float b = *(float*)&v;

    // StringTools::println("%.3f", b);
}

void findApproximations()
{
    std::vector<Vec2f> desiredPoints;
    float incV = PI/6;
    for(int i=0; i<=6; i++)
    {
        desiredPoints.push_back(Vec2f(incV*i, sin(incV*i)));
    }
    // PolynomialMathFunction f = MathExt::fitPolynomial({Vec2f(0, 0), Vec2f(PI/4, sin(PI/4)), Vec2f(PI/2, 1), Vec2f(3*PI/4, sin(3*PI/4)), Vec2f(PI, 0)});
    PolynomialMathFunction f = MathExt::fitPolynomial(desiredPoints);

    std::vector<Vec2f> potentialPoints;
    incV = PI / 63;
    for(int i=0; i<64; i++)
    {
        potentialPoints.push_back(Vec2f(incV*i, sin(incV*i)));
    }
    PolynomialMathFunction g = MathExt::linearRegression(potentialPoints, 7);

    StringTools::print("F(x) = ");
    for(int i=0; i<f.size(); i++)
    {
        StringTools::print("%.9fx^%d + ", f.getConstant(i), i);
    }
    StringTools::println("\n\n\n");
    StringTools::print("G(x) = ");
    for(int i=0; i<g.size(); i++)
    {
        StringTools::print("%.9fx^%d + ", g.getConstant(i), i);
    }
    StringTools::println("");
}

void testChebyShev()
{
    GeneralMathFunction f = GeneralMathFunction();
    f.setFunction([](double x) ->double{
        return std::exp2(x);
    });

    int degree = 5;
    double a = 0;
    double b = 1;
    //try linear spaced
    //range [0, 1]
    std::vector<Vec2f> linPoints;
    for(int i=0; i<degree+1; i++)
    {
        double x = a + ((b-a)/degree)*i;
        double y = f.solve(x);
        linPoints.push_back(Vec2f(x, y));
    }
    PolynomialMathFunction p = MathExt::fitPolynomial(linPoints);

    //try chebyshev
    PolynomialMathFunction p2 = MathExt::chebyshevApproximation(&f, a, b, degree, false);

    StringTools::println("Polynomial for linear spaced points: ");
    StringTools::println("\t%s", p.toString().c_str());
    
    StringTools::println("Polynomial for chebyshev nodes: ");
    StringTools::println("\t%s", p2.toString().c_str());
}

float divExponent(float x)
{
    union {float floatP; int floatAsInt;} v, v2;
    v.floatP = x;
    int expOnly = (v.floatAsInt >> 23) & 0xFF;
    v2.floatAsInt = expOnly << 23;

    return v.floatP / v2.floatP;
}

int main(int argc, char** argv)
{
    testChebyShev();
    // float potentialValue = 3.99999;
    // float potentialValue2 = 0.5124;
    // float potentialValue3 = 0.0;
    
    // StringTools::println("%.5f -> %.5f", potentialValue, divExponent(potentialValue));
    // StringTools::println("%.5f -> %.5f", potentialValue2, divExponent(potentialValue2));
    // StringTools::println("%.5f -> %.5f", potentialValue3, divExponent(potentialValue3));

    // findApproximations();
    // approxFunctions();
    // testGraph();
    // openGLTest();
    // contourTest();
    // testDrawEfficiency();
    // quickBezierTest();
    // testSerialization();
    // quickMapDatastructure();

    return 0;
}

//TODO: POLYGON TRIANGULATION, MAKE MANY OPERATIONS CONST&
