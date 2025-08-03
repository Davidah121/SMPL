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
                    Vec2f intersection = b.getFunctionAt(d);
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
    //     for(int x=0; x<kernel.getColumns(); x++)
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
    win->getGuiManager()->loadElementsFromFile("GuiStuff/New_layout.xml");
    // win->getGuiManager()->alwaysRedraw(true);
}

#include <omp.h>
void testGuiPart2()
{
    GuiManager::initDefaultLoadFunctions();
    SimpleGraphics::init();
    GraphicsInterface::setType(GraphicsInterface::TYPE_SOFTWARE);
    GraphicsInterface::setDefaultType(GraphicsInterface::TYPE_SOFTWARE);
    WindowOptions options;
    options.initFunction = init;
    SimpleWindow win = SimpleWindow("Title", 1280, 720, -1, -1, options);

    win.waitTillClose();
}

// #include "NeuralNetwork.h"
// void testMLStuff()
// {

// }


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

#include "VectorFont.h"
void testFontStuff()
{
    SimpleGraphics::init();
    Image img = Image(1024, 1024);
    VectorFont f = VectorFont();
    f.load("C:\\Users\\Alan\\Pictures\\SMPLStuff\\SVGFonts/ARIBLK.svg");

    f.setFontSize(48);
    SimpleGraphics::setFont(&f);
    
    SimpleGraphics::setColor({255, 255, 255, 255});
    SimpleGraphics::clearImage(&img);

    SimpleGraphics::setColor({0,0,0,255});
    // VectorGraphic* v = f.getGraphic(f.getCharIndex(65));
    // v->draw(&img);
    SimpleGraphics::drawText("ABCabc", 0, 0, &img);

    img.savePNG("FontTest.png");
}

void testMLStuff()
{
    
}

int wrapNumber(int v, int bound)
{
    return ((unsigned int)v) % bound;
}

MatrixF convolveFull(const MatrixF& input_matrix, const MatrixF& kernel, int paddingMode)
{
    int kR = (kernel.getRows()/2);
    int kC = (kernel.getColumns()/2);
    if(kernel.getColumns() != 1 && kernel.getColumns()%2)
        kC++;
    if(kernel.getRows() != 1 && kernel.getRows()%2)
        kR++;
    
    MatrixF output = MatrixF(input_matrix.getRows()+kR, input_matrix.getColumns()+kC);
    for(size_t i=0; i<output.getRows(); i++)
    {
        int startY = i-kR;
        for(size_t j=0; j<output.getColumns(); j++)
        {
            int startX = j-kC;
            //ignore all values outside the range of the input matrix. No negatives. No values greater than rows or columns
            float sum = 0;
            for(int kernelY=0; kernelY<kernel.getRows(); kernelY++)
            {
                int currY = startY+kernelY;
                for(int kernelX=0; kernelX<kernel.getColumns(); kernelX++)
                {
                    int currX = startX+kernelX;
                    float inputMatValue = 0;
                    if(paddingMode == 1)
                    {
                        //clamp
                        int r = MathExt::clamp<int>(currY, 0, input_matrix.getRows());
                        int c = MathExt::clamp<int>(currX, 0, input_matrix.getColumns());
                        inputMatValue = input_matrix[r][c];
                    }
                    else if(paddingMode == 2)
                    {
                        //wrap
                        int r = wrapNumber(currY, input_matrix.getRows());
                        int c = wrapNumber(currX, input_matrix.getColumns());
                        inputMatValue = input_matrix[r][c];
                    }
                    else
                    {
                        //zero padding
                        if(currX >= 0 && currX < input_matrix.getColumns() && currY >= 0 && currY < input_matrix.getRows())
                            inputMatValue = input_matrix[currY][currX];
                    }
                    sum += kernel[kernelY][kernelX] * inputMatValue;
                }
            }
            output[i][j] = sum;
        }
    }
    return output;
}

MatrixF convolveSame(const MatrixF& input_matrix, const MatrixF& kernel, int paddingMode, bool crossCorrelation)
{
    int kR = (kernel.getRows()/2);
    int kC = (kernel.getColumns()/2);
    if(crossCorrelation)
    {
        if(kernel.getRows()%2 == 0)
            kR--;
        if(kernel.getColumns()%2 == 0)
            kC--;
    }
    MatrixF output = MatrixF(input_matrix.getRows(), input_matrix.getColumns());
    for(size_t i=0; i<output.getRows(); i++)
    {
        int startY = i-kR;

        for(size_t j=0; j<output.getColumns(); j++)
        {
            int startX = j-kC;

            //ignore all values outside the range of the input matrix. No negatives. No values greater than rows or columns
            float sum = 0;
            for(int kernelY=0; kernelY<kernel.getRows(); kernelY++)
            {
                int currY = startY+kernelY;
                for(int kernelX=0; kernelX<kernel.getColumns(); kernelX++)
                {
                    int currX = startX+kernelX;
                    float inputMatValue = 0;
                    if(paddingMode == 1)
                    {
                        //clamp
                        int r = MathExt::clamp<int>(currY, 0, input_matrix.getRows());
                        int c = MathExt::clamp<int>(currX, 0, input_matrix.getColumns());
                        inputMatValue = input_matrix[r][c];
                    }
                    else if(paddingMode == 2)
                    {
                        //wrap
                        int r = wrapNumber(currY, input_matrix.getRows());
                        int c = wrapNumber(currX, input_matrix.getColumns());
                        inputMatValue = input_matrix[r][c];
                    }
                    else
                    {
                        //zero padding
                        if(currX >= 0 && currX < input_matrix.getColumns() && currY >= 0 && currY < input_matrix.getRows())
                            inputMatValue = input_matrix[currY][currX];
                    }
                    sum += kernel[kernelY][kernelX] * inputMatValue;
                }
            }
            output[i][j] = sum;
        }
    }
    return output;
}

MatrixF convolveValid(const MatrixF& input_matrix, const MatrixF& kernel)
{
    int kR = (kernel.getRows()/2);
    int kC = (kernel.getColumns()/2);
    if(kernel.getColumns() != 1 && kernel.getColumns()%2)
        kC++;
    if(kernel.getRows() != 1 && kernel.getRows()%2)
        kR++;
    MatrixF output = MatrixF(input_matrix.getRows()-kR, input_matrix.getColumns()-kC);
    for(size_t i=0; i<output.getRows(); i++)
    {
        int startY = i;
        for(size_t j=0; j<output.getColumns(); j++)
        {
            int startX = j;
            //ignore all values outside the range of the input matrix. No negatives. No values greater than rows or columns
            float sum = 0;
            for(int kernelY=0; kernelY<kernel.getRows(); kernelY++)
            {
                int currY = startY+kernelY;
                for(int kernelX=0; kernelX<kernel.getColumns(); kernelX++)
                {
                    int currX = startX+kernelX;
                    sum += kernel[kernelY][kernelX] * input_matrix[currY][currX];
                }
            }
            output[i][j] = sum;
        }
    }
    return output;
}

void convolveStuff(const MatrixF& input, const MatrixF& kernel, bool crossCorrelation, int paddingMode)
{
    if(!input.getValid() || !kernel.getValid())
        return; //THROW EXCEPTION LOSER
    MatrixF tempKernel = kernel;

    if(!crossCorrelation)
    {
        size_t i=0;
        size_t j=0;
        do
        {
            do
            {
                float temp = tempKernel[tempKernel.getRows()-i-1][tempKernel.getColumns()-j-1];
                tempKernel[tempKernel.getRows()-i-1][tempKernel.getColumns()-j-1] = tempKernel[i][j];
                tempKernel[i][j] = temp;
                j++;
            } while (j < tempKernel.getColumns()/2);
            i++;
        } while (i < tempKernel.getRows()/2);
    }

    MatrixF outputFull = convolveFull(input, tempKernel, paddingMode);
    StringTools::println("Output Matrix Convolution Full Mode");
    StringTools::println("%.3f", outputFull);

    MatrixF outputSame = convolveSame(input, tempKernel, paddingMode, crossCorrelation);
    StringTools::println("Output Matrix Convolution Same Mode");
    StringTools::println("%.3f", outputSame);

    MatrixF outputValid = convolveValid(input, tempKernel);
    StringTools::println("Output Matrix Convolution Valid Mode");
    StringTools::println("%.3f", outputValid);
}

void testConvolution()
{
    MatrixF input, kernel;
    StringTools::println("1D EXAMPLE:");
    input = std::vector<std::vector<float>>{{1, 2, 3, 4}};
    kernel = std::vector<std::vector<float>>{{1, 2}};
    convolveStuff(input, kernel, true, 1);
    
    StringTools::println("\n____________________________");
    
    StringTools::println("2D EXAMPLE:");
    input = std::vector<std::vector<float>>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    kernel = std::vector<std::vector<float>>{{1, 0}, {0, -1}};
    convolveStuff(input, kernel, true, 1);
}

#include "Tensor.h"
void testTensor()
{
    Tensor<float> tensor3 = std::vector<std::vector<std::vector<float>>>{
        {{1.0f, 1.0f, 1.0f}, 
        {1.0f, 1.0f, 1.0f}},

        {{2.0f, 2.0f, 2.0f}, 
        {2.0f, 2.0f, 2.0f}},

        {{3.0f, 3.0f, 3.0f}, 
        {3.0f, 3.0f, 3.0f}},

        {{4.0f, 4.0f, 4.0f}, 
        {4.0f, 4.0f, 4.0f}}
    };
    MatrixF singleMat2 = std::vector<std::vector<float>>{
        {0.1, 0.2},
        {0.3, 0.4},
        {0.5, 0.6}
    };

    Tensor<float> result = tensor3 * singleMat2;
    StringTools::println("%.3f", result);
}

int testTemplateMatching()
{
    Sprite templateImg;
    templateImg.loadImage("template.png");

    Sprite source;
    source.loadImage("source.png");

    if(templateImg.getSize() == 0)
    {
        printf("Template Image not found\n");
        return -1;
    }

    if(source.getSize() == 0)
    {
        printf("Source Image not found\n");
        return -2;
    }

    MatrixF templateKernel = ComputerVision::imageToMatrix(templateImg.getImage(0), 0);
    MatrixF sourceMat = ComputerVision::imageToMatrix(source.getImage(0), 0);
    
    templateKernel = templateKernel.broadcastSubtract(templateKernel.mean());
    sourceMat = sourceMat.broadcastSubtract(sourceMat.mean());

    Matrix<ComplexNumber> matchingMat = MathExt::crossCorrelationFFT(sourceMat, templateKernel, MathExt::CONVOLVE_SAME, 0);
    MatrixF outputSimilarity = ComputerVision::readjustIntensity((MatrixF)matchingMat, 0.0, 1.0);
    StringTools::println("DONE");
    return 0;
}

void testFFT2D()
{

    // [[ 1  4  7  6  0]
    // [ 6 20 28 21  0]
    // [15 44 52 36  0]
    // [14 37 42 27  0]
    // [ 0  0  0  0  0]]
    Matrix<ComplexNumber> temp = std::vector<std::vector<ComplexNumber>>{
        {ComplexNumber(1, 0), ComplexNumber(2, 0), ComplexNumber(3, 0), ComplexNumber(0, 0)},
        {ComplexNumber(4, 0), ComplexNumber(5, 0), ComplexNumber(6, 0), ComplexNumber(0, 0)},
        {ComplexNumber(7, 0), ComplexNumber(8, 0), ComplexNumber(9, 0), ComplexNumber(0, 0)},
        {ComplexNumber(0, 0), ComplexNumber(0, 0), ComplexNumber(0, 0), ComplexNumber(0, 0)}
    };
    Matrix<ComplexNumber> kernel = std::vector<std::vector<ComplexNumber>>{
        {ComplexNumber(1, 0), ComplexNumber(2, 0)},
        {ComplexNumber(2, 0), ComplexNumber(3, 0)}
    };
    // Matrix<ComplexNumber> frequency_matrix = MathExt::fastFourierTransform2D(temp, false);
    // Matrix<ComplexNumber> inverse_matrix = MathExt::fastFourierTransform2D(frequency_matrix, true);

    // StringTools::println("Frequency Matrix:\n%.3f\n\nInverse Matrix:\n%.3f", frequency_matrix, inverse_matrix);

    Matrix<ComplexNumber> result = MathExt::convolutionFFT(temp, kernel, MathExt::CONVOLVE_FULL, 0);
    StringTools::println("%.0f", result);
}

void testNewPrint()
{
    std::vector<Vec2f> numbers = {Vec2f(1.2, 3.42), Vec2f(1.1111, 81.23)};
    std::vector<std::string> names = {"Name1", "Name2"};

    std::string test1 = "numbers";
    std::string test2 = "names";
    
    MatrixF temp = std::vector<std::vector<float>>{
        {1, 2, 3, 0},
        {4, 5, 6, 0},
        {7, 8, 9, 0},
        {0, 0, 0, 0}
    };
    
    StringTools::println("String example \"%s\", \"%s\"\n List of numbers: %.3f", test1, test2, numbers);
    StringTools::println("List of names: %s", names);
    StringTools::println("%f", temp);

    ComplexNumber test = (ComplexNumber)1;
    StringTools::println("ComplexNumber Test = %f", test);
    
}

void doFFT(ComplexNumber* arr, int size, bool inverse)
{
    if(size>=4)
    {
        int newSize = (size/2);
        doFFT(arr, newSize, inverse);
        doFFT(arr+newSize, newSize, inverse);
        
        double angle = 0;
        angle = (-2.0*PI)/size;

        SIMD_SSE<ComplexNumber> multiplier = ComplexNumber(1, 0);
        SIMD_SSE<ComplexNumber> multInc = ComplexNumber(MathExt::cos(angle), MathExt::sin(angle));
        multInc *= ComplexNumber(MathExt::cos(angle), MathExt::sin(angle));
        for(int i=0; i<newSize; i+=2)
        {
            SIMD_SSE<ComplexNumber> even=SIMD_SSE<ComplexNumber>::load(&arr[i]);
            SIMD_SSE<ComplexNumber> odd=SIMD_SSE<ComplexNumber>::load(&arr[i+1])*multiplier;
            (even+odd).store(&arr[i]);
            (even-odd).store(&arr[i+1]);
            multiplier *= multInc;
        }

    }
    else if(size==2)
    {
        SIMD_SSE<ComplexNumber> even=SIMD_SSE<ComplexNumber>::load(&arr[0]);
        SIMD_SSE<ComplexNumber> odd=SIMD_SSE<ComplexNumber>::load(&arr[1]);
        (even+odd).store(&arr[0]);
        (even-odd).store(&arr[1]);
    }
}

void testSIMDStuff()
{
    Matrix<ComplexNumber> arr = Matrix<ComplexNumber>(1, 0x1000000);
    for(int i=0; i<arr.getRows()*arr.getColumns(); i++)
    {
        arr[0][i].real = rand();
        arr[0][i].imaginary = 0;
    }
    size_t startTime = System::getCurrentTimeMicro();
    for(int i=0; i<10; i++)
    {
        // MathExt::fastFourierTransform2DInline(arr, false);
        MathExt::fastFourierTransformInline(arr, true);
    }
    size_t totalTime = System::getCurrentTimeMicro() - startTime;
    StringTools::println("TotalTime: %llu", totalTime);
    StringTools::println("AverageTime: %llu", totalTime/10);
}

#include "HttpServer.h"
void testHttpStuff()
{
    NetworkConfig config;
    config.amountOfConnectionsAllowed = 4096;
    config.location = "0.0.0.0";
    config.port = 12345;
    config.TCP = true;
    config.type = Network::TYPE_SERVER;
    HttpServer sab = HttpServer(config, 8);
    sab.setAllowPersistentConnection(false);
    sab.start();

    while(sab.getRunning())
    {
        System::sleep(1, 0, false);
    }
}

#include "CatmullRom.h"
void testCatmullRom()
{
    CatmullRom cr = CatmullRom();
    cr.addPoint(Vec2f(32, 32));
    cr.addPoint(Vec2f(64, 12));
    cr.addPoint(Vec2f(96, 64));
    cr.addPoint(Vec2f(32, 96));
    cr.setTension(0.5);

    BezierCurve curve1 = cr.getCurve(0);
    BezierCurve curve2 = cr.getCurve(1);
    BezierCurve curve3 = cr.getCurve(2);
    
    StringTools::println("Curve1: %.3f\n", curve1.getPoints());
    StringTools::println("Curve2: %.3f\n", curve2.getPoints());
    StringTools::println("Curve3: %.3f\n", curve3.getPoints());
}

void testPolygonStuff()
{
    VectorGraphic v = VectorGraphic(512, 512);
    v.load("C:/Users/Alan/Pictures/SMPLStuff/SVGs/_ionicons_svg_md-call.svg");


    Image surf = Image(v.getWidth(), v.getHeight());
    v.draw(&surf);
    surf.savePNG("output_1.png", true, false, false);
}

int main(int argc, char** argv)
{
    // testPolygonStuff();
    testHttpStuff();
    // testSIMDStuff();
    // testTemplateMatching();
    // testFFT2D();
    // testNewPrint();
    // testTensor();
	// testNetworkStuff();
	// testHashStuff();
    // testFontStuff();
    // testGraph();
    
    // openGLTest();
    // testGuiPart2();

    // testBezierCurveProjection();

    // contourTest();
    // testSerializationQuick();
    // testDrawFunctions();
    // testMLStuff();

    // testStringStuff();
    // testLargeEnoughClause();

	// testSSEStuff();

    return 0;
}

//TODO: POLYGON TRIANGULATION, MAKE MANY OPERATIONS CONST&
//TODO: Redesign GraphicsInterface. Should be able to be expanded upon without modifying source code. Should be clear and consistent as to which type is being used. (no creating assets for the wrong type)
//TODO: Adjust Gui Layout code. Should give informatigrayscaleSourceon about content width and height even if that isn't the actual final width and height. This also means that even with overflow, content width and height can be larger than the maximum allowed width and height.



//TODO: Serialized Code should be updated to add the option to generate Serializer code for non SerializedObjects. Also should allow 