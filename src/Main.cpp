#include "StringTools.h"
#include "System.h"
#include "SimpleWindow.h"
#include "SimpleDir.h"
#include <stdlib.h>
#include "Compression.h"

#include "Sprite.h"
#include "ComputerVision.h"
#include "SuffixAutomaton.h"
#include "RadixTree.h"

#include "LCG.h"
#include <set>

#include "SimplePipe.h"

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


void testDropFiles()
{
    WindowOptions wo;
    wo.threadManaged = false;

    SimpleWindow w = SimpleWindow("Title", 320, 240, -1, -1, wo);
    w.waitTillClose();
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
    SimpleWindow w = SimpleWindow("New Gui Window", 640, 480, -1, -1, o);

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

    t1 = System::getCurrentTimeNano();
    std::vector<lengthPair> output1;
    Compression::getLZ77RefPairsCHash(pixs, endIndex, &output1, 7);
    t2 = System::getCurrentTimeNano();
    timeForHV1 = t2-t1;
    
    StringTools::println("Time For CHash: %llu", timeForHV1);
    StringTools::println("OUTPUT1 SIZE: %llu", output1.size());
    
    t1 = System::getCurrentTimeNano();
    std::vector<lengthPair> output2;
    Compression::getLZ77RefPairsCSATest(pixs, endIndex, &output2, 7);
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

    // SimpleFile f = SimpleFile("OutputLZ1.txt", SimpleFile::WRITE);
    // for(lengthPair& l : output2)
    // {
    //     if(l.literal)
    //         f.writeString("LIT: " + StringTools::toString(l.v1));
    //     else
    //         f.writeString("REF: " + StringTools::toString(l.v1) + " - " + StringTools::toString(l.v2));
    //     f.writeLineBreak();
    // }
    // f.close();

    SimpleFile f = SimpleFile("OutputLZ1", SimpleFile::WRITE);
    index = 0;
    for(lengthPair& l : output1)
    {
        if(l.literal)
        {
            f.writeByte(l.v1 & 0xFF);
            index++;
        }
        else
        {
            f.writeBytes(&pixs[index-l.v2], l.v1);
            index += l.v1;
        }
    }
    f.close();

    SimpleFile f2 = SimpleFile("OutputLZ2", SimpleFile::WRITE);
    index = 0;
    for(lengthPair& l : output2)
    {
        if(l.literal)
        {
            f2.writeByte(l.v1 & 0xFF);
            index++;
        }
        else
        {
            f2.writeBytes(&pixs[index-l.v2], l.v1);
            index += l.v1;
        }
    }
    f2.close();

    SimpleFile f3 = SimpleFile("BaselineOutput", SimpleFile::WRITE);
    unsigned char* startPixs = pixs;

    while (startPixs < endPixs)
    {
        f3.writeByte(*startPixs);
        startPixs++;
    }
    
    f3.close();

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

void correctFCT8(double* src, double* dst, bool inverse)
{
    if(!inverse)
    {
        const float mx00 = src[0] + src[7];
        const float mx01 = src[1] + src[6];
        const float mx02 = src[2] + src[5];
        const float mx03 = src[3] + src[4];
        const float mx04 = src[0] - src[7];
        const float mx05 = src[1] - src[6];
        const float mx06 = src[2] - src[5];
        const float mx07 = src[3] - src[4];
        const float mx08 = mx00 + mx03;
        const float mx09 = mx01 + mx02;
        const float mx0a = mx00 - mx03;
        const float mx0b = mx01 - mx02;
        const float mx0c = 1.38703984532215f*mx04 + 0.275899379282943f*mx07;
        const float mx0d = 1.17587560241936f*mx05 + 0.785694958387102f*mx06;
        const float mx0e = -0.785694958387102f*mx05 + 1.17587560241936f*mx06;
        const float mx0f = 0.275899379282943f*mx04 - 1.38703984532215f*mx07;
        const float mx10 = 0.353553390593274f * (mx0c - mx0d);
        const float mx11 = 0.353553390593274f * (mx0e - mx0f);
        dst[0] = 0.353553390593274f * (mx08 + mx09);
        dst[1] = 0.353553390593274f * (mx0c + mx0d);
        dst[2] = 0.461939766255643f*mx0a + 0.191341716182545f*mx0b;
        dst[3] = 0.707106781186547f * (mx10 - mx11);
        dst[4] = 0.353553390593274f * (mx08 - mx09);
        dst[5] = 0.707106781186547f * (mx10 + mx11);
        dst[6] = 0.191341716182545f*mx0a - 0.461939766255643f*mx0b;
        dst[7] = 0.353553390593274f * (mx0e + mx0f);
    }
    else
    {
		const float mx00 = 1.4142135623731f  *src[0];
		const float mx01 = 1.38703984532215f *src[1] + 0.275899379282943f*src[7];
		const float mx02 = 1.30656296487638f *src[2] + 0.541196100146197f*src[6];
		const float mx03 = 1.17587560241936f *src[3] + 0.785694958387102f*src[5];
		const float mx04 = 1.4142135623731f  *src[4];
		const float mx05 = -0.785694958387102f*src[3] + 1.17587560241936f*src[5];
		const float mx06 = 0.541196100146197f*src[2] - 1.30656296487638f*src[6];
		const float mx07 = -0.275899379282943f*src[1] + 1.38703984532215f*src[7];
		const float mx09 = mx00 + mx04;
		const float mx0a = mx01 + mx03;
		const float mx0b = 1.4142135623731f*mx02;
		const float mx0c = mx00 - mx04;
		const float mx0d = mx01 - mx03;
		const float mx0e = 0.353553390593274f * (mx09 - mx0b);
		const float mx0f = 0.353553390593274f * (mx0c + mx0d);
		const float mx10 = 0.353553390593274f * (mx0c - mx0d);
		const float mx11 = 1.4142135623731f*mx06;
		const float mx12 = mx05 + mx07;
		const float mx13 = mx05 - mx07;
		const float mx14 = 0.353553390593274f * (mx11 + mx12);
		const float mx15 = 0.353553390593274f * (mx11 - mx12);
		const float mx16 = 0.5f*mx13;
		dst[0] = 0.25f * (mx09 + mx0b) + 0.353553390593274f*mx0a;
		dst[1] = 0.707106781186547f * (mx0f + mx15);
		dst[2] = 0.707106781186547f * (mx0f - mx15);
		dst[3] = 0.707106781186547f * (mx0e + mx16);
		dst[4] = 0.707106781186547f * (mx0e - mx16);
		dst[5] = 0.707106781186547f * (mx10 - mx14);
		dst[6] = 0.707106781186547f * (mx10 + mx14);
		dst[7] = 0.25f * (mx09 + mx0b) - 0.353553390593274f*mx0a;
    }
}

void FasterFCT8(double* arr, double* output, bool inverse)
{
    //constants for later
    const double root2 = 1.41421356237;
    const double cosC1 = 0.980785280403; //kcn -> c3 -> cos(PI/16)
    const double sinC1 = 0.195090322016; //kcn -> c3 -> sin(PI/16)
    const double cosC3 = 0.831469612303; //kcn -> c3 -> cos(3PI/16)
    const double sinC3 = 0.55557023302; //kcn -> c3 -> sin(3PI/16)
    const double root2CosC1 = 1.38703984532; //kcn -> sqrt(2)c1 -> sqrt(2)cos(PI/16)
    const double root2SinC1 = 0.275899379283; //kcn -> sqrt(2)c1 -> sqrt(2)sin(PI/16)
    const double invsqrt2 = 0.707106781187;//invsqrt2;
    const double invsqrt2h = 0.353553390593;//invsqrt2*0.5f;

    if(!inverse)
    {
        double scaledInput[8];
        for(int i=0; i<8; i++)
            scaledInput[i] = arr[i]*invsqrt2h;

        //stage1
        double s1_x0 = scaledInput[0] + scaledInput[7];
        double s1_x1 = scaledInput[1] + scaledInput[6];
        double s1_x2 = scaledInput[2] + scaledInput[5];
        double s1_x3 = scaledInput[3] + scaledInput[4];
        double s1_x4 = scaledInput[3] - scaledInput[4];
        double s1_x5 = scaledInput[2] - scaledInput[5];
        double s1_x6 = scaledInput[1] - scaledInput[6];
        double s1_x7 = scaledInput[0] - scaledInput[7];

        // StringTools::println("s1_x0 = %.10f", s1_x0);
        // StringTools::println("s1_x1 = %.10f", s1_x1);
        // StringTools::println("s1_x2 = %.10f", s1_x2);
        // StringTools::println("s1_x3 = %.10f", s1_x3);
        // StringTools::println("s1_x4 = %.10f", s1_x4);
        // StringTools::println("s1_x5 = %.10f", s1_x5);
        // StringTools::println("s1_x6 = %.10f", s1_x6);
        // StringTools::println("s1_x7 = %.10f", s1_x7);
        // StringTools::println("");

        //stage2
        double s2_x0 = s1_x0 + s1_x3;
        double s2_x1 = s1_x1 + s1_x2;
        double s2_x2 = s1_x1 - s1_x2;
        double s2_x3 = s1_x0 - s1_x3;

        double temp = cosC3*(s1_x4 + s1_x7);
        double s2_x4 = (sinC3 - cosC3)*s1_x7 + temp;
        double s2_x7 = -(cosC3 + sinC3)*s1_x4 + temp;
        
        double temp2 = cosC1*(s1_x5 + s1_x6);
        double s2_x5 = (sinC1 - cosC1)*s1_x6 + temp2;
        double s2_x6 = -(cosC1 + sinC1)*s1_x5 + temp2;

        
        // StringTools::println("s2_x0 = %.10f", s2_x0);
        // StringTools::println("s2_x1 = %.10f", s2_x1);
        // StringTools::println("s2_x2 = %.10f", s2_x2);
        // StringTools::println("s2_x3 = %.10f", s2_x3);
        // StringTools::println("s2_x4 = %.10f", s2_x4);
        // StringTools::println("s2_x5 = %.10f", s2_x5);
        // StringTools::println("s2_x6 = %.10f", s2_x6);
        // StringTools::println("s2_x7 = %.10f", s2_x7);
        // StringTools::println("");
        
        //stage3
        output[0] = s2_x0 + s2_x1;
        output[4] = s2_x0 - s2_x1;
        
        double temp3 = root2CosC1*(s2_x2 + s2_x3);
        output[2] = (root2SinC1 - root2CosC1)*s2_x3 + temp3;
        output[6] = -(root2CosC1 + root2SinC1)*s2_x2 + temp3;

        double s3_x4 = s2_x4 + s2_x6;
        double s3_x5 = s2_x7 - s2_x5;
        double s3_x6 = s2_x4 - s2_x6;
        double s3_x7 = s2_x7 + s2_x5;

        
        // StringTools::println("s3_x0 = %.10f", output[0]);
        // StringTools::println("s3_x1 = %.10f", output[4]);
        // StringTools::println("s3_x2 = %.10f", output[2]);
        // StringTools::println("s3_x3 = %.10f", output[6]);
        // StringTools::println("s3_x4 = %.10f", s3_x4);
        // StringTools::println("s3_x5 = %.10f", s3_x5);
        // StringTools::println("s3_x6 = %.10f", s3_x6);
        // StringTools::println("s3_x7 = %.10f", s3_x7);
        // StringTools::println("");
        
        //stage4
        output[7] = s3_x7 - s3_x4;
        output[3] = root2*s3_x5;
        output[5] = root2*s3_x6;
        output[1] = s3_x7 + s3_x4;

        // StringTools::println("output[%d] = %.10f", 0, output[0]);
        // StringTools::println("output[%d] = %.10f", 1, output[1]);
        // StringTools::println("output[%d] = %.10f", 2, output[2]);
        // StringTools::println("output[%d] = %.10f", 3, output[3]);
        // StringTools::println("output[%d] = %.10f", 4, output[4]);
        // StringTools::println("output[%d] = %.10f", 5, output[5]);
        // StringTools::println("output[%d] = %.10f", 6, output[6]);
        // StringTools::println("output[%d] = %.10f", 7, output[7]);
        // StringTools::println("");
    }
    else
    {
        double s4[8];
        for(int i=0; i<8; i++)
            s4[i] = arr[i] / invsqrt2h;
        

        //o7 = s3_x7 - s3_x4
        //o1 = s3_x7 + s3_x4
        //x0 = o1 + o7 = 2(s3_x7)
        //stage4
        double s3_x0 = s4[0];
        double s3_x1 = s4[4];
        double s3_x2 = s4[2];
        double s3_x3 = s4[6];
        double s3_x4 = (s4[1] - s4[7])*0.5;
        double s3_x5 = s4[3]*invsqrt2;
        double s3_x6 = s4[5]*invsqrt2;
        double s3_x7 = (s4[1] + s4[7])*0.5;

        double s2_x0 = (s3_x0 + s3_x1)*0.5;
        double s2_x1 = (s3_x0 - s3_x1)*0.5;
        double s2_x2 = (cosC1*s3_x2 - sinC1*s3_x3)*invsqrt2;
        double s2_x3 = (cosC1*s3_x3 + sinC1*s3_x2)*invsqrt2;

        double s2_x4 = (s3_x4 + s3_x6)*0.5;
        double s2_x5 = (s3_x7 - s3_x5)*0.5;
        double s2_x6 = (s3_x4 - s3_x6)*0.5;
        double s2_x7 = (s3_x7 + s3_x5)*0.5;

        double s1_x0 = (s2_x0 + s2_x3)*0.5;
        double s1_x1 = (s2_x1 + s2_x2)*0.5;
        double s1_x2 = (s2_x1 - s2_x2)*0.5;
        double s1_x3 = (s2_x1 - s2_x3)*0.5;

        double s1_x4 = (cosC3*s2_x4 - sinC3*s2_x7);
        double s1_x5 = (cosC1*s2_x5 - sinC1*s2_x6);
        double s1_x6 = (cosC1*s2_x6 + sinC1*s2_x5);
        double s1_x7 = (cosC3*s2_x7 + sinC3*s2_x4);

        output[0] = (s1_x0 + s1_x7)*0.5;
        output[1] = (s1_x1 + s1_x6)*0.5;
        output[2] = (s1_x2 + s1_x5)*0.5;
        output[3] = (s1_x3 + s1_x4)*0.5;
        output[4] = (s1_x3 - s1_x4)*0.5;
        output[5] = (s1_x2 - s1_x5)*0.5;
        output[6] = (s1_x1 - s1_x6)*0.5;
        output[7] = (s1_x0 - s1_x7)*0.5;
    }
}

void testFasterFCT()
{
    double input[8];
    double output1[8];
    double output2[8];
    size_t t1,t2;

    LCG lcg = LCG(System::getCurrentTimeMicro());
    for(int k=0; k<8; k++)
    {
        input[k] = (lcg.get() % 1024);
    }

    t1 = System::getCurrentTimeNano();
    for(int i=0; i<100000; i++)
    {
        MathExt::FCT8(input, output1, true);
    }
    t2 = System::getCurrentTimeNano();
    StringTools::println("Total Time for baseline IDCT8 = %llu", (t2-t1));

    // t1 = System::getCurrentTimeNano();
    // for(int i=0; i<100000; i++)
    // {
    //     correctFCT8(input, output2, true);
    // }
    // t2 = System::getCurrentTimeNano();
    // StringTools::println("Total Time for new IDCT8 = %llu", (t2-t1));

    // for(int k=0; k<8; k++)
    // {
    //     if(MathExt::abs(output1[k] - output2[k]) > 0.00001)
    //     {
    //         StringTools::println("Problem at %d. %.7f vs %.7f", k, output1[k], output2[k]);
    //     }
    // }
}

void testBetterBinSetStuff()
{
    BinarySet b = BinarySet();
    b.add(0b11010100, 8);
    b.add(0b01101001, 8);
    b.add(0b10001010, 8);

    b.setBitOrder(BinarySet::RMSB);

    //001010111001011001010001
    //01010011010011101010

    //0b00101011
    //0b10010110
    //0b01010001

    //0010 1010 0011 0100 1100 0101
    size_t total = 0;
    size_t t1 = System::getCurrentTimeNano();
    for(int i=0; i<1000000; i++)
    {
        total += b.getBits(1,15,false);
    }
    size_t t2 = System::getCurrentTimeNano();
    StringTools::println("%llu - avg: %llu", (t2-t1), (t2-t1)/1000000);
    StringTools::println("Total: %llu", total);
    
    // StringTools::println("b[1 - 9] = %d", b.getBits(1, 9, true));
    
    // for(int i=0; i<24; i++)
    //     StringTools::println("b[%d] = %d", i, b.getBit(i));

    // for(int i=0; i<24; i++)
    //     StringTools::print("%d", b.getBit(i));
    // StringTools::println("");
}

void testConvolutionStuff()
{
    Sprite s;
    s.loadImage("Bikesgray.png");
    if(s.getSize() <= 0)
    {
        StringTools::println("ERROR ON LOADING TEST IMG");
        return;
    }

    Image* grayscaleImg = SimpleGraphics::convertToGrayscale(s.getImage(0));

    Mat3f gx = Mat3f(1, 0, -1,
                        2, 0, -2,
                        1, 0, -1);
    Mat3f gy = Mat3f( 1,  2,  1,
                        0,  0,  0,
                        -1, -2, -1);

    Matrix imgXDerivative = ComputerVision::convolutionNormalized(grayscaleImg, &gx, ComputerVision::RED_CHANNEL);
    Matrix imgYDerivative = ComputerVision::convolutionNormalized(grayscaleImg, &gy, ComputerVision::RED_CHANNEL);
    int counter = 0;
    double minV = 10000;
    double maxV = 0;
    double avg = 0;
    for(int y=0; y<imgXDerivative.getRows(); y++)
    {
        for(int x=0; x<imgXDerivative.getCols(); x++)
        {
            double xV = (imgXDerivative[y][x]+0.5)*255.0;
            avg += xV;
            if(xV >= 128)
            {
                counter++;
            }
            maxV = __max(xV, maxV);
            minV = __min(xV, minV);
        }
    }
    StringTools::println("%d", counter);
    StringTools::println("%.5f", maxV);
    StringTools::println("%.5f", minV);
    StringTools::println("%.5f", avg / (imgXDerivative.getRows() * imgYDerivative.getCols()));
    Image xDerImg = Image(grayscaleImg->getWidth(), grayscaleImg->getHeight());
    Image yDerImg = Image(grayscaleImg->getWidth(), grayscaleImg->getHeight());
    
    //Saving memory allocation cost by resuing grayscaleImg
    Color* finalImgPixels = grayscaleImg->getPixels();
    Color* xDerImgPixels = xDerImg.getPixels();
    Color* yDerImgPixels = yDerImg.getPixels();
    for(int y=0; y<grayscaleImg->getHeight(); y++)
    {
        for(int x=0; x<grayscaleImg->getWidth(); x++)
        {
            Color c;
            // double len = MathExt::sqrt( MathExt::sqr(imgXDerivative[y][x]) + MathExt::sqr(imgYDerivative[y][x]) );
            // c.red = MathExt::clamp(len*255.0, 0.0, 255.0);
            // c.green = c.red;
            // c.blue = c.red;
            // c.alpha = 255;
            // *finalImgPixels = c;
            // finalImgPixels++;

            //test stuff. Ignore
            c.red = MathExt::clamp((imgXDerivative[y][x]+0.5)*255.0, 0.0, 255.0);
            c.green = c.red;
            c.blue = c.red;
            c.alpha = 255;
            
            *xDerImgPixels = c;

            c.red = MathExt::clamp((imgYDerivative[y][x]+0.5)*255.0, 0.0, 255.0);
            c.green = c.red;
            c.blue = c.red;
            c.alpha = 255;
            
            *yDerImgPixels = c;
            
            xDerImgPixels++;
            yDerImgPixels++;
        }
    }

    grayscaleImg->saveBMP("sobelEdge.bmp");
    xDerImg.saveBMP("xDerNormalized.bmp");
    yDerImg.saveBMP("yDerNormalized.bmp");
    delete grayscaleImg;
}

void testPipe()
{
    SimplePipe p = SimplePipe("ffmpeg", SimplePipe::READ | SimplePipe::ASCII);
    
    while(true)
    {
        char c = p.readByte();
        if(p.isEndOfFile())
            break;
        StringTools::print("%c", c);
    }
    p.close();

    StringTools::println("EXIT CODE: %d", p.getExitCode());
}

// int WinMain(HINSTANCE hins, HINSTANCE preIns, LPSTR cmdline, int nShowCMD)
int main(int argc, char** argv)
{
    //TODO: Improve JPEG efficiency. (Basically FCT8x8 needs to be a bit faster. This is where most of the time is spent. (2/3))

    testPipe();
    // testConvolutionStuff();
    // testBetterBinSetStuff();
    // testFasterFCT();
    // testFDCT();
    // testStreamLZSS();
    // testHashSpeed();
    // testNewGui();
    // testDropFiles();
    // testOGLWindow();

    return 0;
}
