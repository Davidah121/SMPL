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

void recursivePass(Image* img, int startX, int startY, int size, int lowestSize, double threshold, BinarySet& output, MatrixF& xDer, MatrixF& yDer)
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

        MatrixF imgAsMatrix = ComputerVision::imageToMatrix(imgs.getImage(0), ComputerVision::RED_CHANNEL);

        MatrixF imgXDerivative = ComputerVision::convolution(imgAsMatrix, gx);
        MatrixF imgYDerivative = ComputerVision::convolution(imgAsMatrix, gy);

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
    // MatrixF kernel = ComputerVision::guassianKernel(2, 1);
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

    Image* temp = s.getImage(0);
    
    // MatrixF threshMatrix = ComputerVision::imageToMatrix(s.getImage(0), 0);
    // Image* derivativeImg = ComputerVision::matrixToImage(threshMatrix);
    Image* derivativeImg = SimpleGraphics::cannyEdgeFilter(s.getImage(0), 1, 0.05, 0.19);
    MatrixF threshMatrix = ComputerVision::imageToMatrix(derivativeImg, 0);
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

void init(SimpleWindow* win)
{
    win->getGuiManager()->loadElementsFromFile("GuiStuff/TextBoxLayout.xml");
}

#include <omp.h>
void testGuiPart2()
{
    GuiManager::initDefaultLoadFunctions();
    SimpleGraphics::init();
    WindowOptions options;
    options.initFunction = init;
    SimpleWindow win = SimpleWindow("Title", 1280, 720, -1, -1, options);

    win.waitTillClose();
}

void testDrawFunctions()
{
    //Will make into unit test somehow
    Image img = Image(320, 240);
    size_t t1 = System::getCurrentTimeMicro();
    for(int i=0; i<1000000; i++)
    {
        SimpleGraphics::setColor(Color{255, 0, 0, 255});
        SimpleGraphics::drawRect(15, 217, 20, 229, true, &img);
    }
    size_t t2 = System::getCurrentTimeMicro();
    StringTools::println("TIME TAKEN: %llu", t2-t1);
}

#include "RawDataFormatter.h"
#include "DefaultSerialization.h"
#include "DefaultSerialization_SmartMemory.h"

class TestObject : SerializedObject
{
public:
    TestObject()
    {
        myValue = SmartMemory<int>::createDeleteOnLast(new int, 1);
        *myValue = 38;
    }
    ~TestObject(){}
private:
    SmartMemory<int> myValue;

SERIALIZE_CLASS(myValue);
};

void testSerializationQuick()
{
    bool doStuff = false;
    SmartMemory<int> v1;
    SmartMemory<Vec4f> v2;
    SmartMemory<TestObject> v3;
    
    if(doStuff)
    {
        SerializedStreamableFile output = SerializedStreamableFile("testSerializeFile", SerializedStreamableFile::TYPE_WRITE);
        RawDataFormatter rawFormatter = RawDataFormatter();

        v1 = SmartMemory<int>::createDeleteOnLast(new int[4], 4);
        v2 = SmartMemory<Vec4f>::createDeleteRights(new Vec4f, 1);
        v1[0] = 1; v1[1] = 2; v1[2] = 4; v1[3] = 6;
        v2->setValues(Vec4f(42.1, -32, 12.3, 1));
        v3 = SmartMemory<TestObject>::createDeleteOnLast(new TestObject, 1);
        
        staticSerializeVar(output, rawFormatter, SERIALIZE(v1));
        staticSerializeVar(output, rawFormatter, SERIALIZE(v2));
        staticSerializeVar(output, rawFormatter, SERIALIZE(v3));

        StringTools::println("OFFSET: %llu", output.size());
        DeferredSerializedSmartMemory::serialize(output, rawFormatter);
    }
    else
    {
        SerializedStreamableFile input = SerializedStreamableFile("testSerializeFile", SerializedStreamableFile::TYPE_READ);
        RawDataFormatter rawFormatter = RawDataFormatter();

        input.seek(57);
        DeferredSerializedSmartMemory::deserialize(input, rawFormatter);
        input.seek(0);

        staticDeserializeVar(input, rawFormatter, SERIALIZE(v1));
        staticDeserializeVar(input, rawFormatter, SERIALIZE(v2));
        staticDeserializeVar(input, rawFormatter, SERIALIZE(v3));

        StringTools::println("%p", v1.getPointer());
        StringTools::println("\t%llu", v1.getNumberOfElements());
        StringTools::println("\t%d", v1.getDeleteRights());
        StringTools::println("\t%d", v1.getIsArray());
        StringTools::println("\t%d", v1.getWillDeleteOnLast());
        
        StringTools::println("\t\t%d", v1[0]);
        StringTools::println("\t\t%d", v1[1]);
        StringTools::println("\t\t%d", v1[2]);
        StringTools::println("\t\t%d", v1[3]);

        
        StringTools::println("%p", v2.getPointer());
        StringTools::println("\t%llu", v2.getNumberOfElements());
        StringTools::println("\t%d", v2.getDeleteRights());
        StringTools::println("\t%d", v2.getIsArray());
        StringTools::println("\t%d", v2.getWillDeleteOnLast());
        
        StringTools::println("\t\t%.3f", v2->x);
        StringTools::println("\t\t%.3f", v2->y);
        StringTools::println("\t\t%.3f", v2->z);
        StringTools::println("\t\t%.3f", v2->w);

        
        StringTools::println("%p", v3.getPointer());
        StringTools::println("\t%llu", v3.getNumberOfElements());
        StringTools::println("\t%d", v3.getDeleteRights());
        StringTools::println("\t%d", v3.getIsArray());
        StringTools::println("\t%d", v3.getWillDeleteOnLast());
    }
}

// #include "NeuralNetwork.h"
// void testMLStuff()
// {

// }

void testLargeEnoughClause()
{
    Image surf = Image(32, 32);
    // SimpleFile hybridOMPCSV = SimpleFile("hybridOMP.csv", SimpleFile::ASCII | SimpleFile::WRITE);
    // for(int s=1; s<512; s++)
    // {
    //     size_t t1 = System::getCurrentTimeMicro();
    //     SimpleGraphics::drawRect(0, 0, s, s, false, &surf);
    //     size_t t2 = System::getCurrentTimeMicro();

    //     hybridOMPCSV.writeString(StringTools::formatString("%d, %llu", s*s, t2-t1));
    //     hybridOMPCSV.writeLineBreak();
    // }
    // hybridOMPCSV.close();

    SimpleGraphics::setColor({0, 0, 0, 0});
    for(int i=0; i<32; i++)
        SimpleGraphics::drawRect(0, 0, 32, 32, false, &surf);
    surf.savePNG("testForAlpha.png", true);
    
    // SimpleFile yesOMPCSV = SimpleFile("yesOMP.csv", SimpleFile::ASCII | SimpleFile::WRITE);
    // for(int s=1; s<512; s++)
    // {
    //     size_t t1 = System::getCurrentTimeMicro();
    //     SimpleGraphics::drawRectOMP(0, 0, s, s, false, &surf);
    //     size_t t2 = System::getCurrentTimeMicro();

    //     yesOMPCSV.writeString(StringTools::formatString("%d, %llu", s*s, t2-t1));
    //     yesOMPCSV.writeLineBreak();
    // }
    // yesOMPCSV.close();

}

#define USE_OPENGL
#include "ext/GLWindow.h"
#include "ext/GLGraphics.h"

GLSurface* surf = nullptr;
bool test1 = false;
void oglPaint(SimpleWindow* win)
{
    surf->bind();
    GLGraphics::setClearColor(Vec4f(0, 0, 0, 0));
    GLGraphics::clear(GLGraphics::COLOR_BUFFER);
    GLGraphics::setDrawColor(Vec4f(1, 0, 0, 1));
    GLGraphics::drawRectangle(32, 32, 32, 32, false);
    // GLGraphics::drawCircle(64, 64, 32.0f, 12.0f);
    // GLGraphics::drawEllipse(64, 64, 32.0f, 24.0f, false);
    surf->unbind();

    GLGraphics::drawSurface(0, 0, 1280, 720, surf);

    if(test1 == false)
    {
        Image* imgP = surf->toImage();
        imgP->savePNG("TestOGL.png");
        delete imgP;
    }
    test1 = true;
}
void openGLTest()
{
    GuiManager::initDefaultLoadFunctions();
    SimpleGraphics::init();
    GraphicsInterface::setType(GraphicsInterface::TYPE_OPENGL);
    GraphicsInterface::setDefaultType(GraphicsInterface::TYPE_OPENGL);
    WindowOptions options;
    options.initFunction = init;
    GLWindow win = GLWindow("Title", 1280, 720, -1, -1, options);
    GLGraphics::init();
    // win.setPaintFunction(oglPaint);
    // win.setActivateGui(false);

    win.waitTillClose();
}

void testString2(const StringBridge& strB)
{
    StringTools::println("%s", strB.toPrintableFormat().c_str());
}

void testStringStuff()
{
    std::string str1 = "test";
    std::wstring str2 = L"Test2.0";
    std::u32string str3 = U"A bigg test";

    StringBridge s = str1;
    StringBridge s2 = "test1";
    
    StringBridge s3 = str2;
    StringBridge s4 = "test2";
    
    StringBridge s5 = str3;
    StringBridge s6 = "test3";

    testString2(str1);
    testString2(UnicodeStringBridge(str2));
    testString2(str3);

    testString2(s);
    testString2(s2);
    testString2(s3);
    testString2(s4);
    testString2(s5);
    testString2(s6);
}

#include "VectorFont.h"
void testFontStuff()
{
    SimpleGraphics::init();
    Image img = Image(1024, 1024);
    VectorFont f = VectorFont();
    f.load("SVGFonts/ARIBLK.svg");

    f.setFontSize(12);
    SimpleGraphics::setFont(&f);
    
    SimpleGraphics::setColor({255, 255, 255, 255});
    SimpleGraphics::clearImage(&img);

    SimpleGraphics::setColor({0,0,0,255});
    // VectorGraphic* v = f.getGraphic(f.getCharIndex(65));
    // v->draw(&img);
    SimpleGraphics::drawText("ABCabc", 0, 0, &img);

    img.savePNG("FontTest.png");
}

#include "LCG.h"

template<typename T>
int64_t pivotData(T* data, int64_t size, int64_t indexOfPivot)
{
    int64_t i = -1;
    int64_t j = size;
    T pivotData = data[indexOfPivot];

    while(true)
    {
        do
        {
            i++;
        } while(data[i] < pivotData);
        
        do
        {
            j--;
        } while(pivotData < data[j]);

        if(i >= j)
            break;
        //swap
        T temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }

    return j;
}

template<typename T>
void quickSort(T* data, int64_t size)
{
    if(size <= 1)
        return;
    if(size == 2)
    {
        if(data[0] > data[1])
        {
            T temp = data[0];
            data[0] = data[1];
            data[1] = temp;
        }
        return;
    }
    //create pivot point.
    int64_t pivotPoint = size/2;
    int64_t finalLocationOfPivotIndex = pivotData(data, size, pivotPoint);

    quickSort(data, finalLocationOfPivotIndex+1);
    quickSort(data+finalLocationOfPivotIndex+1, size - (finalLocationOfPivotIndex+1));
}


/*
    start from i==0 to i==size-1
    pivot point is given to us

    if data[i] < data[pivot point] && i < pivot point
        do nothing. Its good
    else if data[i] < data[pivot point] && i > pivot point
        swap data
    
    if data[i] > data[pivot point] && i > pivot point
        do nothing. Its good
    else if data[i] > data[pivot point] && i < pivot point
        swap data
    
    the assumption is that everything to the left is less than or equal

*/

void testSort()
{
    //generate 100 random numbers between 0 - 0xFFF
    LCG numGenerator = LCG(time(nullptr));
    std::vector<int> nums;
    for(int i=0; i<100; i++)
        nums.push_back(numGenerator.get() % 0xFFF);
    
    quickSort(nums.data(), nums.size());

    for(int i=0; i<100; i++)
        std::cout << nums[i] << std::endl;
}

void testBezierCurveProjection()
{
    BezierCurve b = BezierCurve();
    b.addPoint(32, 32);
    b.addPoint(96, 16);
    b.addPoint(96, 96);
    b.addPoint(64, 64);

    MatrixF distanceMatrix = MatrixF(128, 128);
    double thickness = 1;
    for(int iterations=1; iterations<=5; iterations++)
    {
        for(int y=0; y<128; y++)
        {
            for(int x=0; x<128; x++)
            {
                double guessT = b.findTimeForMinDis(Vec2f(x, y), iterations);
                Vec2f pointOnCurve = b.getFuctionAt(guessT);
                double len = (pointOnCurve - Vec2f(x,y)).getLength();
                distanceMatrix[y][x] = (thickness+1)-len / thickness;
                distanceMatrix[y][x] = MathExt::clamp(distanceMatrix[y][x], 0.0f, 1.0f);
            }
        }

        ComputerVision::readjustIntensity(distanceMatrix, 0, 1);
        StringTools::println("DONE");
    }
    Image* img = ComputerVision::matrixToImage(distanceMatrix);
    img->savePNG("TEST CURVE.png");
    delete img;
}

int main(int argc, char** argv)
{
    // testSort();
    // testFontStuff();
    // testGraph();
    
    openGLTest();

    // testBezierCurveProjection();

    // contourTest();
    // testGuiPart2();
    // testSerializationQuick();
    // testDrawFunctions();
    // testMLStuff();

    // testStringStuff();
    // testLargeEnoughClause();

    return 0;
}

//TODO: POLYGON TRIANGULATION, MAKE MANY OPERATIONS CONST&
//TODO: Redesign GraphicsInterface. Should be able to be expanded upon without modifying source code. Should be clear and consistent as to which type is being used. (no creating assets for the wrong type)
//TODO: Adjust Gui Layout code. Should give information about content width and height even if that isn't the actual final width and height. This also means that even with overflow, content width and height can be larger than the maximum allowed width and height.