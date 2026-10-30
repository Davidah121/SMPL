//TODO: Drawing Sprites is currently broken. Fix it
//TODO: Scaling can now be done using SIMD. Fix it
//TODO: SimpleGraphics functions to remove colors from an image
//TODO: Gradient Fill
//TODO: Flood Fill

//TODO: Rework GUI so that all objects have margins, padding, etc.

//TODO: Ensure OpenGL functions work properly.
//TODO: Ensure DirectX functions work properly. (sometimes I want to stick a pin in this ngl)
//TODO: Allow HTTPServer and WebSocketServer to exist on the same port potentially?

//TODO: Add polygon triangulation (Depends on DCEL)

// - WORKING ON CURRENTLY -
//TODO: Fix Even-Odd rendering of polygons, etc. *May be finished. Needs further testing
//TODO: Finish DCEL
//TODO: Change how Cookies are handled. Should be importable/exportable and must be integrated with existing HTTP Server/Client



#include <Network.h>
#include "FiberTask.h"
#include "SimpleFile.h"
#include "SimplePipe.h"
#include "SimpleWindow.h"
#include "Streamable.h"
#include <System.h>
#include <WebRequest.h>
#include <File.h>
#include <StringTools.h>
#include <HttpClient.h>
#include <HttpServer.h>
#include <SimpleJSON.h>
#include <atomic>
#include <chrono>
#include <mutex>
#include <vector>
#include "Input.h"
#include "VectorFont.h"
#include "SimpleGraphics.h"

using namespace smpl;

// float findMinDistance(const BezierCurve& c1, const BezierCurve& c2, float circleRad)
// {
// 	//compute f(t) - g(t) = c1-c2 which should be a direct subtraction of points if they are the same points
// 	//make my life simple. Convert into equation
// 	//(1-t)^2 * P1 + 2(1-t)t * P2 + t^2 * P3
// 	//(1 -2t + t^2)P1 + (2t - 2t^2)P2 + t^2*P3
// 	const Vec2f* c1Points = c1.getPoints().data();
// 	const Vec2f* c2Points = c2.getPoints().data();

// 	Vec2f A1 = c1Points[2] -2*c1Points[1] + c1Points[0];
// 	Vec2f B1 = 2*(c1Points[1] - c1Points[0]);
// 	Vec2f C1 = c1Points[0];

// 	Vec2f A2 = c2Points[2] -2*c2Points[1] + c2Points[0];
// 	Vec2f B2 = 2*(c2Points[1] - c2Points[0]);
// 	Vec2f C2 = c2Points[0];

// 	Vec2f finalA = A1-A2;
// 	Vec2f finalB = B1-B2;
// 	Vec2f finalC = C1-C2;

// 	//f(t) = At^2 + Bt + C
// 	//f`(t) = 2At + B
// 	//sqrt(f(t).x^2 + f(t).y^2) = r || f(t).x^2 + f(t).y^2 = r^2
// 	//2f(t).x*f`(t).x + 2f(t).y*f`(t).y = 0

// 	//2f(t)*f`(t)

// 	//This becomes a quartic equation if we limit to nothing more than quadratic bezier curves which is all we actually need in practice
// 	//V_{1}=A^{2}
// 	//V_{2}=2AB
// 	//V_{3}=2AC+B^{2}
// 	//V_{4}=C^{2}
// 	//where A, B, C are the finalA, finalB, and finalC points
// 	//it doesn't make a lot of sense to square or multiply these as they are points/vectors so treat these as dot products probably. Maybe hadamard products

// 	Vec2f quarticV1 = finalA.hadamardProduct(finalA);
// 	Vec2f quarticV2 = 2*finalA.hadamardProduct(finalB);
// 	Vec2f quarticV3 = 2*finalA.hadamardProduct(finalC) + finalB.hadamardProduct(finalB);
// 	Vec2f quarticV4 = finalC.hadamardProduct(finalC);
// 	float actualQ1 = quarticV1.x+quarticV1.y;
// 	float actualQ2 = quarticV2.x+quarticV2.y;
// 	float actualQ3 = quarticV3.x+quarticV3.y;
// 	float actualQ4 = quarticV4.x+quarticV4.y;
	
// 	// Vec2f finalADer = 2*finalA;
// 	// Vec2f finalBDer = finalB;

// 	// //newton's method
// 	// float currP = 0;
// 	// for(int i=0; i<4; i++)
// 	// {
// 	// 	Vec2f f = (finalA*(currP*currP)) + (finalB*currP) + finalC;
// 	// 	Vec2f fP = (2*finalADer*currP) + finalB;
// 	// 	float finalDis = f.dot(f) - (circleRad*circleRad);
// 	// 	float finalDisDer = 2*f.dot(fP);
// 	// 	currP -= finalDis/finalDisDer;
// 	// }

// 	// float derQ1 = 4*actualQ1;
// 	// float derQ2 = 3*actualQ2;
// 	// float derQ3 = 2*actualQ3;
// 	return actualQ1+actualQ2+actualQ3+actualQ4;

// 	// float circleRadSqr = circleRad*circleRad;
// 	// Vec2f FAVec = finalC;
// 	// Vec2f FBVec = finalA+finalB+finalC;
// 	// float fa = FAVec.dot(FAVec) - circleRadSqr;
// 	// float fb = FBVec.dot(FBVec) - circleRadSqr;

// 	// const int ITERATIONS = 10;
// 	// float a = 0;
// 	// float b = 1;
// 	// float c = 0;
// 	// int side = 0;
// 	// for(int i=0; i<ITERATIONS; i++)
// 	// {
// 	// 	c = (fa*b - fb*a) / (fa-fb);
// 	// 	if(abs(b-a) < EPSILON*abs(b+a))
// 	// 		break;
// 	// 	Vec2f FCVec = finalA*c*c + finalB*c + finalC;
// 	// 	float fc = FCVec.dot(FCVec) - circleRadSqr;
// 	// 	if(fc*fb > 0)
// 	// 	{
// 	// 		b = c; fb = fc;
// 	// 		if(side == -1)
// 	// 			fa /= 2;
// 	// 		side = -1;
// 	// 	}
// 	// 	else if(fa*fc > 0)
// 	// 	{
// 	// 		a = c; fa = fc;
// 	// 		if(side == 1)
// 	// 			fb /= 2;
// 	// 		side = 1;
// 	// 	}
// 	// 	else {
// 	// 		break;
// 	// 	}
// 	// }
	
// 	// return c;
// }

// void simulationTest()
// {
// 	int width = 8;
// 	int height = 6;
// 	int wDivValue = 640/width;
// 	int hDivValue = 480/height;
// 	std::atomic_int collisionCounter = 0;
// 	std::vector<Circle> objects;
// 	std::vector<std::vector<int>> grid = std::vector<std::vector<int>>(width*height); //lazy
// 	while(true)
// 	{
		
// 		size_t t1 = System::getCurrentTimeMicro();
// 		//for each step, increment objects by 100
// 		for(int i=0; i<100; i++)
// 		{
// 			objects.push_back(Circle(1 + (rand() % 16)));
// 			objects.back().setPosition(Vec2f(rand() % 640, rand() % 480));
// 			// //calculate insert into the grids it will be in.
// 			// Circle& c = objects.back();
// 			// Vec2f pos = c.getPosition().toVec2f();

// 			// int minY = __max((int)(pos.y-c.getRadius()), 0);
// 			// int maxY = __min((int)(pos.y+c.getRadius()), 479);
// 			// int minX = __max((int)(pos.x-c.getRadius()), 0);
// 			// int maxX = __min((int)(pos.x+c.getRadius()), 639);

// 			// minY/=hDivValue;
// 			// maxY/=hDivValue;
// 			// minX/=wDivValue;
// 			// maxX/=wDivValue;

// 			// for(int y=minY; y < maxY; y++)
// 			// {
// 			// 	for(int x=minX; x < maxX; x++)
// 			// 	{
// 			// 		grid[x + y*width].push_back(objects.size());
// 			// 	}
// 			// }
// 		}

// 		//naive collision detection
// 		for(int step=0; step<8; step++)
// 		{
// 			collisionCounter = 0;
// 			for(size_t i=0; i<objects.size(); i++)
// 			{
// 				for(size_t j=i+1; j<objects.size(); j++)
// 				{
// 					CollisionMaster::collisionMethod(objects[i], objects[j]);
// 					collisionCounter++;
// 				}
// 			}
// 		}


// 		// //smarter collision
// 		// for(int step=0; step<8; step++)
// 		// {
// 		// 	// #pragma omp parallel for
// 		// 	for(size_t i=0; i<grid.size(); i++)
// 		// 	{
// 		// 		//check all objects in the list against each other
// 		// 		for(size_t j=0; j<grid[i].size(); j++)
// 		// 		{
// 		// 			for(size_t k=j+1; k<grid[i].size(); k++)
// 		// 			{
// 		// 				CollisionMaster::collisionMethod(objects[grid[i][j]], objects[grid[i][k]]);
// 		// 				collisionCounter++;
// 		// 			}
// 		// 		}
// 		// 	}
// 		// }
// 		size_t t2 = System::getCurrentTimeMicro();

// 		if(t2-t1 > 120000000000 || objects.size() > 200000 || collisionCounter >= 20000000) //8 milliseconds
// 		{
// 			StringTools::println("Threshold hit: %lluus at %llu objects", t2-t1, objects.size());
// 			StringTools::println("Collisions performed: %d", (int)collisionCounter);
// 			break;
// 		}
// 	}
// }

// void collisionStuff()
// {
// 	BezierCurve c1 = BezierCurve({Vec2f(-10, 0), Vec2f(64, 0), Vec2f(64, 64)});
// 	BezierCurve c2 = BezierCurve({Vec2f(0, -20), Vec2f(0, 64), Vec2f(64, 64)});

// 	size_t TOTAL_OBJECTS = 6400;
// 	std::vector<BezierCurve> allObjectPaths;
// 	for(int i=0; i<TOTAL_OBJECTS; i++)
// 	{
// 		allObjectPaths.push_back(BezierCurve({
// 			Vec2f(rand()%100, rand()%100), Vec2f(rand()%100, rand()%100), Vec2f(rand()%100, rand()%100)
// 		}));
// 	}

// 	size_t t1 = System::getCurrentTimeNano();

// 	std::vector<Line> cachedLines = std::vector<Line>(TOTAL_OBJECTS*8);
// 	for(size_t index1=0; index1<allObjectPaths.size(); index1++)
// 	{
// 		BezierCurve& startP = allObjectPaths[index1];
// 		Vec2f previousPoint = startP.getPoint(0);
// 		for(int i=1; i<=8; i++)
// 		{
// 			Vec2f nextPoint = startP.getFunctionAt(i*(1.0/7.0));
// 			cachedLines[index1*8 + i-1] = Line(previousPoint, nextPoint);
// 			previousPoint = nextPoint;
// 		}
// 	}

// 	for(size_t index1=0; index1<allObjectPaths.size(); index1+=8)
// 	{
// 		Line* startOfCachedLines = &cachedLines[index1];
// 		for(size_t index2=index1+8; index2<allObjectPaths.size(); index2+=8)
// 		{
// 			Line* startOfCachedLines2 = &cachedLines[index2];
// 			double timePoint = 0;
// 			//naive slow approach
// 			for(int i=0; i<8; i++)
// 			{
// 				for(int j=0; j<8; j++)
// 				{
// 					// double timePoint = 0;
// 					startOfCachedLines[i].getIntersectionParametric(startOfCachedLines2[j], timePoint);
// 				}
// 			}
			
// 			// std::vector<double> points = c1.findIntersectionPoints(c2, 1);
// 			// findMinDistance(*startP, *newStartP, 4);
// 			// double t;
// 			// l1.getIntersectionParametric(l2, t);
// 			// l1.getIntersectionParametric(l2, t);
// 			// l1.getIntersectionParametric(l2, t);
// 			// l1.getIntersectionParametric(l2, t);
// 		}
// 	}
// 	size_t t2 = System::getCurrentTimeNano();
// 	StringTools::println("Time taken: %lluns", t2-t1);
// 	StringTools::println("Total Checks = %llu", TOTAL_OBJECTS*TOTAL_OBJECTS/2);

// 	// std::vector<double> points = c1.findIntersectionPoints(c2, 0.001);
// 	// StringTools::println("%.5f", points);

	
// 	// auto timeV = findMinDistance(c1, c2, 4);
// 	// StringTools::println("Time of minDis = %.9f", timeV);

// 	// simulationTest();
// }

// void quickDrawText(VectorFont& fnt, const std::string& str, Image& outputImage)
// {
// 	int currX = 0;
// 	int currY = 0;
// 	for(char c : str)
// 	{
// 		if(c == '\n')
// 		{
// 			currX = 0;
// 			currY += fnt.getVerticalAdvance();
// 			continue;
// 		}
// 		FontCharInfo fci = fnt.getFontCharInfo(c);
// 		Image* cachedImg = fnt.getImage(fci);
// 		if(currX+fci.width > outputImage.getWidth())
// 		{
// 			currX = 0;
// 			currY += fnt.getVerticalAdvance();
// 		}
// 		if(currX == 0)
// 			fci.xOffset = 0;
// 		SimpleGraphics::drawImage(cachedImg, currX+fci.xOffset, currY+fci.yOffset, &outputImage);
// 		currX += fci.horizAdv;
// 	}
// }

// uint16_t quickDiv255_2(uint16_t a)
// {
// 	uint32_t b = (uint32_t)a*0x8081;
// 	return b>>23;
// }
// uint8_t quickDiv255_3(uint8_t a, uint8_t b)
// {
// 	//a * b / 255 where b is 255 or 0
// 	if(a==1)
// 		return 1;
// 	return (a>>8) + 1;
// }

// void polygonRendering()
// {
// 	// VectorFont fnt = VectorFont();
// 	// fnt.load("C:\\Users\\Alan\\Pictures\\SMPLStuff\\SVGFonts\\ARIBLK.svg");
// 	// fnt.setFontSize(24);
// 	// SimpleGraphics::setFont(&fnt);

	
// 	// std::string str = "Hello World. \nThis is your FPS: 20";
// 	// SimpleGraphics::drawText(str, 0, 0, &outputImage);
// 	// outputImage.savePNG("testFont.png");
	
// 	// FontCharInfo fci = fnt.getFontCharInfo('A');
// 	// VectorGraphic* vgPtr = fnt.getGraphic(fci.reserved);
// 	// auto points = ((VectorPath*)vgPtr->getShape(0))->toPolygon();
// 	// StringTools::println("%.3f", points);

// 	// StringTools::println("%.6f", points);
// 	//2ndary test
// 	// std::vector<Vec2f> points = {Vec2f(1000,1000), Vec2f(1500,1000), Vec2f(1500,1500), Vec2f(1300,1300), Vec2f(1100,1300)};
// 	// std::vector<Vec2f> points = {Vec2f(500,1000), Vec2f(750,1000), Vec2f(750,1500), Vec2f(1250,1500), Vec2f(1250,1000), Vec2f(1500,1000), Vec2f(1500,1750), Vec2f(500,1750)};
	

// }

void maskTest(int w, int h)
{
	Image outputImage = Image(w, h);
	std::vector<unsigned char> mask = std::vector<unsigned char>(w*h); //pixel mask to determine what should be rendered

	size_t timeToBuildMask = 0;
	size_t timeToDrawWithMask = 0;
	size_t timeToDrawWithoutMask = 0;
	
	for(int i=0; i<1000; i++)
	{
		srand(102040201);
		size_t t1 = System::getCurrentTimeMicro();
		for(int y=0; y<h; y++)
		{
			int startX = rand() % w;
			int endX = rand() % w;
			if(startX > endX)
				std::swap(startX, endX);
			unsigned char* row = &mask[y*w];
			for(int x=startX; x<=endX; x++)
				row[x] = 0xFF;
		}
		size_t t2 = System::getCurrentTimeMicro();
		timeToBuildMask += t2-t1;
	}
	for(int i=0; i<1000; i++)	
	{
		size_t t1 = System::getCurrentTimeMicro();
		#pragma omp parallel for
		for(int y=0; y<h; y++)
		{
			Color* rowPixels = &outputImage.getPixels()[y*w];
			unsigned char* rowMask = &mask[y*w];
			for(int x=0; x<w; x++)
			{
				Color r = {255, 0, 0, 255};
				// r.alpha = (rowMask == 0) ? 0 : r.alpha;
				if(rowMask != 0)
					rowPixels[x] = SimpleGraphics::blend(r, rowPixels[x]);
			}
		}

		size_t t2 = System::getCurrentTimeMicro();
		timeToDrawWithMask += t2-t1;
	}

	for(int i=0; i<1000; i++)
	{
		srand(102040201);
		size_t t1 = System::getCurrentTimeMicro();
		#pragma omp parallel for
		for(int y=0; y<h; y++)
		{
			Color* rowPixels = &outputImage.getPixels()[y*h];
			for(int x=0; x<w; x++)
			{
				rowPixels[x] = SimpleGraphics::blend({255, 0, 0, 255}, rowPixels[x]);
			}
		}
		size_t t2 = System::getCurrentTimeMicro();
		timeToDrawWithoutMask += t2-t1;
	}

	StringTools::println("Time to build mask: %llu", timeToBuildMask/1000);
	StringTools::println("Time to draw with bit mask: %llu", timeToDrawWithMask/1000);
	StringTools::println("Time to draw without mask: %llu", timeToDrawWithoutMask/1000);
}

// void maskTest2(int w, int h)
// {
// 	Image outputImage = Image(w, h);
// 	std::vector<unsigned char> mask = std::vector<unsigned char>(w*h); //pixel mask to determine what should be rendered

// 	size_t timeToBuildMask = 0;
// 	size_t timeToDrawWithMask = 0;
// 	size_t timeToDrawWithoutMask = 0;

// 	{
// 		srand(102040201);
// 		size_t t1 = System::getCurrentTimeMicro();
// 		for(int y=0; y<h; y++)
// 		{
// 			int startX = rand() % w;
// 			int endX = rand() % w;
// 			if(startX > endX)
// 				std::swap(startX, endX);
// 			unsigned char* row = &mask[y*w];
// 			for(int x=startX; x<=endX; x++)
// 				row[x] = 0xFF;
// 		}
// 		size_t t2 = System::getCurrentTimeMicro();
// 		timeToBuildMask += t2-t1;
// 	}

// 	for(int i=0; i<100; i++)	
// 	{
// 		size_t t1 = System::getCurrentTimeMicro();
// 		__m128i baseColor = _mm_set1_epi32(0xFF0000FF);
// 		__m128i emptyColor = _mm_set1_epi32(0x00000000);

// 		for(int y=0; y<h; y++)
// 		{
// 			Color* rowPixels = &outputImage.getPixels()[y*w];
// 			unsigned int* rowMask = (unsigned int*)&mask[y*w];
// 			for(int x=0; x<w; x+=4)
// 			{
// 				__m128i rowPixelsSIMD = _mm_loadu_si128((__m128i*)rowPixels);
// 				__m128i rowMaskSIMD = _mm_loadu_si128((__m128i*)rowMask);
// 				rowMaskSIMD = _mm_cvtepi8_epi32(rowMaskSIMD);

// 				__m128i actualColor = _mm_and_si128(baseColor, rowMaskSIMD);
// 				__m128i outputColor = SimpleGraphics::blend(rowPixelsSIMD, actualColor);
				
// 				_mm_store_si128((__m128i*)rowPixels, outputColor);
// 				rowPixels+=4;
// 				rowMask+=4;
// 			}
// 		}

// 		size_t t2 = System::getCurrentTimeMicro();
// 		timeToDrawWithMask += t2-t1;
// 	}

// 	for(int i=0; i<100; i++)
// 	{
// 		srand(102040201);
// 		size_t t1 = System::getCurrentTimeMicro();
// 		__m128i baseColor = _mm_set1_epi32(0xFF0000FF);
// 		for(int y=0; y<h; y++)
// 		{
// 			Color* rowPixels = &outputImage.getPixels()[y*h];
// 			for(int x=0; x<w; x+=4)
// 			{
// 				__m128i rowPixelsSIMD = _mm_loadu_si128((__m128i*)rowPixels);
// 				__m128i outputColor = SimpleGraphics::blend(rowPixelsSIMD, baseColor);
				
// 				_mm_store_si128((__m128i*)rowPixels, outputColor);
// 				rowPixels+=4;
// 			}
// 		}
// 		size_t t2 = System::getCurrentTimeMicro();
// 		timeToDrawWithoutMask += t2-t1;
// 	}

// 	StringTools::println("Time to build mask: %llu", timeToBuildMask/100);
// 	StringTools::println("Time to draw with bit mask: %llu", timeToDrawWithMask/100);
// 	StringTools::println("Time to draw without mask: %llu", timeToDrawWithoutMask/100);
// }
void maskTest2(int w, int h)
{
	Image outputImage = Image(w, h);
	std::vector<unsigned int> mask = std::vector<unsigned int>(w*h); //pixel mask to determine what should be rendered

	const SIMD_SSE<uint32_t> baseColor = 0xFF0000FF;
	const SIMD_SSE<uint32_t> emptyColor = 0x00000000;

	size_t timeToBuildMask = 0;
	size_t timeToDrawWithMask = 0;
	size_t timeToDrawWithoutMask = 0;

	{
		srand(102040201);
		size_t t1 = System::getCurrentTimeMicro();
		for(int y=0; y<h; y++)
		{
			int startX = rand() % w;
			int endX = rand() % w;
			if(startX > endX)
				std::swap(startX, endX);
			unsigned int* row = &mask[y*w];
			for(int x=startX; x<=endX; x++)
				row[x] = 0xFFFFFFFF;
		}
		size_t t2 = System::getCurrentTimeMicro();
		timeToBuildMask += t2-t1;
	}

	for(int i=0; i<1000; i++)	
	{
		size_t t1 = System::getCurrentTimeMicro();
		#pragma omp parallel for
		for(int y=0; y<h; y++)
		{
			Color* rowPixels = &outputImage.getPixels()[y*w];
			unsigned int* rowMask = (unsigned int*)&mask[y*w];
			for(int x=0; x<w; x+=4)
			{
				SIMD_SSE<uint32_t> rowPixelsSIMD = SIMD_SSE<uint32_t>::load((uint32_t*)rowPixels);
				SIMD_SSE<uint32_t> rowMaskSIMD = SIMD_SSE<uint32_t>::load((uint32_t*)rowMask);
				
				SIMD_SSE<uint32_t> actualColor = baseColor & rowMaskSIMD;
				SIMD_SSE<uint32_t> outputColor = SimpleGraphics::blend(rowPixelsSIMD.values, actualColor.values);
				outputColor.store((uint32_t*)rowPixels);

				rowPixels+=4;
				rowMask+=4;
			}
		}

		size_t t2 = System::getCurrentTimeMicro();
		timeToDrawWithMask += t2-t1;
	}

	for(int i=0; i<1000; i++)
	{
		srand(102040201);
		size_t t1 = System::getCurrentTimeMicro();
		#pragma omp parallel for
		for(int y=0; y<h; y++)
		{
			Color* rowPixels = &outputImage.getPixels()[y*h];
			for(int x=0; x<w; x+=4)
			{
				SIMD_SSE<uint32_t> rowPixelsSIMD = SIMD_SSE<uint32_t>::load((uint32_t*)rowPixels);
				SIMD_SSE<uint32_t> outputColor = SimpleGraphics::blend(rowPixelsSIMD.values, baseColor.values);
				
				outputColor.store((uint32_t*)rowPixels);
				rowPixels+=4;
			}
		}
		size_t t2 = System::getCurrentTimeMicro();
		timeToDrawWithoutMask += t2-t1;
	}

	StringTools::println("Time to build mask: %llu", timeToBuildMask/1000);
	StringTools::println("Time to draw with bit mask: %llu", timeToDrawWithMask/1000);
	StringTools::println("Time to draw without mask: %llu", timeToDrawWithoutMask/1000);
}
void maskTest3(int w, int h)
{
	Image outputImage = Image(w, h);
	Image mask = Image(w, h);


	const SIMD_SSE<uint32_t> baseColor = 0xFF0000FF;
	const SIMD_SSE<uint32_t> emptyColor = 0x00000000;

	size_t timeToBuildMask = 0;
	size_t timeToDrawWithMask = 0;
	size_t timeToDrawWithoutMask = 0;

	SimpleGraphics::setCompositeRule(SimpleGraphics::NO_COMPOSITE);
	for(int i=0; i<1000; i++)
	{
		mask.setAllPixels({0, 0, 0, 0});
		srand(102040201);
		size_t t1 = System::getCurrentTimeMicro();
		for(int y=0; y<h; y++)
		{
			int startX = rand() % w;
			int endX = rand() % w;
			if(startX > endX)
				std::swap(startX, endX);
			
			SimpleGraphics::fillBetween({0xFF, 0xFF, 0xFF, 0xFF}, startX, endX, y, &mask);
		}
		size_t t2 = System::getCurrentTimeMicro();
		timeToBuildMask += t2-t1;
	}
	SimpleGraphics::setMask(mask);

	
	SimpleGraphics::setCompositeRule(SimpleGraphics::COMPOSITE_SRC_OVER);
	SimpleGraphics::enableMask(true);
	for(int i=0; i<1000; i++)	
	{
		size_t t1 = System::getCurrentTimeMicro();
		SimpleGraphics::drawRect(0, 0, w, h, false, &outputImage);
		size_t t2 = System::getCurrentTimeMicro();
		timeToDrawWithMask += t2-t1;
	}

	SimpleGraphics::enableMask(false);
	for(int i=0; i<1000; i++)
	{
		srand(102040201);
		size_t t1 = System::getCurrentTimeMicro();
		SimpleGraphics::drawRect(0, 0, w, h, false, &outputImage);
		size_t t2 = System::getCurrentTimeMicro();
		timeToDrawWithoutMask += t2-t1;
	}

	StringTools::println("Time to build mask: %llu", timeToBuildMask/1000);
	StringTools::println("Time to draw with bit mask: %llu", timeToDrawWithMask/1000);
	StringTools::println("Time to draw without mask: %llu", timeToDrawWithoutMask/1000);
}

#include "DCEL.h"
void dcelTest()
{
	DCEL graph = DCEL();
	// graph.addPolygon({Vec2f(0, 0), Vec2f(10, 0), Vec2f(10, 10), Vec2f(0, 10)}); //outer box
	// graph.addPolygon({Vec2f(4, 4), Vec2f(8, 4), Vec2f(8, 8), Vec2f(4, 8)});	//inner box

	graph.addPolygon({Vec2f(32, 32), Vec2f(96, 32), Vec2f(96, 96), Vec2f(32, 96)}); //A
	graph.addPolygon({Vec2f(64, 64), Vec2f(128, 64), Vec2f(128, 128), Vec2f(64, 128)}); //B

	graph.generateFaces();

	//list all the vertices
	StringTools::print("All verticies: \n\t");
	for(size_t i=0; i<graph.getTotalVertices(); i++)
	{
		auto p = graph.getVertex(i);
		StringTools::print("%.3f, ", p->position);
	}
	
	StringTools::print("\n\nAll Edges: \n\t");
	for(size_t i=0; i<graph.getTotalEdges(); i++)
	{
		auto e = graph.getEdge(i);
		if(e->next != nullptr)
			StringTools::print("[%.3f, %.3f] | ", e->origin->position, e->next->origin->position);
		else
			StringTools::print("[PROBLEM?] | ");
	}

	StringTools::println("\n\nAll Faces:");
	for(size_t i=0; i<graph.getTotalFaces(); i++)
	{
		auto f = graph.getFace(i);
		StringTools::println("\tFace %llu: ORDER = %d", i, f->edgeOrder);
		if(f->outerComponent != nullptr)
			StringTools::println("\t\tEdge Start Point %.3f", f->outerComponent->origin->position);
		else
			StringTools::print("\t\tEDGE PROBLEM?");
	}
	StringTools::println("\n");

	SimpleGraphics::setColor({255, 0, 0, 255});
	for(size_t i=0; i<graph.getTotalFaces(); i++)
	{
		Image testImage = Image(256, 256);
		testImage.setAllPixels({0, 0, 0, 0});

		auto f = graph.getFace(i);
		auto currEdge = f->outerComponent;
		do
		{
			Line l = currEdge->getLine();
			SimpleGraphics::drawLine(l.getPoint1().x, l.getPoint1().y, l.getPoint2().x, l.getPoint2().y, &testImage);
			currEdge = currEdge->next;
		} while(currEdge != f->outerComponent);
		
		testImage.savePNG("Image_"+StringTools::toString(i)+".png");
	}
}

#include "OAuth.h"


void testClient()
{
	//use https://echo.free.beeceptor.com or its http version to test HttpClient
	//also https://httpbin.org/
	bool isConnected = true;
	HttpClient client = HttpClient(URL("https://echo.free.beeceptor.com"));
	WebRequest req = WebRequest::createQuickWebRequest(WebRequest::TYPE_GET, "/");

	client.waitOnConnect();
	if(client.getTimeoutOccurred())
	{
		StringTools::println("TIMEOUT");
		return;
	}

	client.sendRequest(req);
	client.waitOnDataReceived(true);
	StringTools::println("Response = %u", client.getResponse().getStatus());
	if(client.getResponse().getStatus() == 200)
	{
		for(unsigned char c : client.getResponseBody())
		{
			StringTools::print("%c", c);
		}
	}
}

Image img1 = Image(3840, 2160);
Image img2 = Image(8, 8);
int counter = 57000;
void paintMe(SimpleWindow* ptr)
{
	StringTools::println("Counter: %d\n\tTimeToRenderApprox: %llu", counter, ptr->getTimeFromLastPaint());
	SET_OMP_THREADS(4)
	#pragma omp parallel for
	for(int i=0; i<counter; i++)
	{
		int x = (counter<<3) % 3840;
		int y = (counter<<3) / 3840;

		SimpleGraphics::drawSprite(&img2, x, y, &img1);
	}
	ptr->drawImage(&img1);
}

void testSimpleGUI()
{
	GuiManager::initDefaultLoadFunctions();
	WindowOptions options;
	options.initFunction = [](SimpleWindow* ptr) -> void{
		ptr->setThreadUpdateTime(16, 666);
		// ptr->getGuiManager()->loadElementsFromFile("GuiStuff/New_layout.xml");
		ptr->setPaintFunction(paintMe);
	};

	// SimpleWindow w = SimpleWindow("Title", 2560, 1440, -1, -1, options);
	SimpleWindow w = SimpleWindow("Title", 3840, 2160, -1, -1, options);
	
	w.waitTillClose();
}


void renderQueue()
{
	Image* mainSurface = new Image(3840, 2160);
	Image* charImg = new Image(8, 8);

	SimpleJobQueue jobQueue = SimpleJobQueue(4);
	// std::vector<std::function<void()>> allJobs;
	// allJobs.reserve(counter);
	// // std::function<void(int, int)> notInline = [mainSurface, charImg](int x, int y) -> void{
	// // 	SimpleGraphics::drawSprite(charImg, x, y, mainSurface);
	// // };
	
	// size_t t1 = System::getCurrentTimeNano();
	// for(int i=0; i<counter; i++)
	// {
	// 	int x = (i<<3) % 3840;
	// 	int y = (i<<3) / 3840;
	// 	// jobQueue.addJob([mainSurface, charImg, x, y]() -> void{
	// 	// 	SimpleGraphics::drawSprite(charImg, x, y, mainSurface);
	// 	// });
	// 	allJobs.push_back([mainSurface, charImg, x, y]() -> void{
	// 		SimpleGraphics::drawSprite(charImg, x, y, mainSurface);
	// 	});
	// }
	// jobQueue.addJobs(allJobs);
	// size_t t2 = System::getCurrentTimeNano();
	// StringTools::println("Time Taken: %llu", t2-t1);

	// t1 = System::getCurrentTimeNano();
	// jobQueue.waitForAllJobs();
	// t2 = System::getCurrentTimeNano();
	// StringTools::println("Time Taken: %llu", t2-t1);


	// size_t t1 = System::getCurrentTimeNano();
	// SET_OMP_THREADS(4)
	// #pragma omp parallel for
	// for(int i=0; i<counter; i++)
	// {
	// 	int x = (i<<3) % 3840;
	// 	int y = (i<<3) / 3840;

	// 	SimpleGraphics::drawSprite(charImg, x, y, mainSurface);
	// }
	// size_t t2 = System::getCurrentTimeNano();
	// StringTools::println("Time Taken: %llu", t2-t1);

	
	size_t t1 = System::getCurrentTimeNano();
	jobQueue.parallelize([charImg, mainSurface](size_t start, size_t end, size_t incr)->void{
		for(int i=start; i<end; i+=incr)
		{
			int x = (i<<3) % 3840;
			int y = (i<<3) / 3840;

			SimpleGraphics::drawSprite(charImg, x, y, mainSurface);
		}
	}, 0, counter, 1, 4);
	size_t t2 = System::getCurrentTimeNano();
	StringTools::println("Time Taken: %llu", t2-t1);
	delete mainSurface;
	delete charImg;
}

int main(int argc, char** argv)
{
	//RGBA Byteswap-> ABGR LeftRotate-> BGRA
	// auto jsonOutput = getOAuthTokens("client_secret.json", "https://www.googleapis.com/auth/youtube");
	// //refresh token if required
	// bool okay = refreshOAuthToken(jsonOutput);
	
	// OAuth authentication = OAuth::getOAuthToken(SimpleJSON("client_secret.json"), "https://www.googleapis.com/auth/youtube");
	// StringTools::println("%s", authentication.getRawToken().getString());

	// OAuth authentication = OAuth::useExistingToken(SimpleJSON("client_secret.json"), SimpleJSON("token.json"));
	// StringTools::println("%s", authentication.getRawToken().getString());

	// testClient();
	
	// runBasicHTTPServer();
	
	// System::sleep(5000, 0, false);
	// collisionStuff();

	// maskTest(1024, 1024);
	// maskTest2(1024, 1024);
	// maskTest3(1024, 1024);

	// dcelTest();

	// testSimpleGUI();
	renderQueue();

	return 0;
}


/*
	Known Gui Issues:
		- Automatic Scaling of objects with respect to a given size
			Works but not as well as it should. This is also why Grid doesn't work super well
		- Text Rendering
			Gotta fix how the boxes are laid out and wrapped. Also fixes the issue with moving up and down for text selection
	Desired output:
		- Remain under 500 MB of ram for what should be a basic GUI.
		- Rendered with software rendering
			Optional OpenGL or DirectX11
		- Should function with a limited number of cores
			No more than 4 so this needs to be tested on weaker hardware
		- Should run well with a UHD window with current hardware
			Ideally renders at 60FPS but 30FPS is fine
			Not required to run on limited hardware. Just a stress test
		
*/