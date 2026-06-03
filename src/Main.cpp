#include "StringTools.h"
#include "System.h"
#include "WebSocket.h"
#include <atomic>
#include <functional>
#include <omp.h>
#include <thread>
#include <unordered_set>
#include "ext/KeyboardSimulator.h"
#include "Input.h"
#include "SIMD.h"
#include "SimpleGraphics.h"

using namespace smpl;

void polygonFillTest(const std::vector<Vec2f>& vertices)
{
	//somewhat naive approach. For each line, calculate the intersections with the valid scanlines based on max and min y values
	if(vertices.size() < 3)
		return;
	
	//ignoring anti-aliasing potential, drop all decimal values
	for(Vec2f v : vertices)
	{
		v.x = floor(v.x);
		v.y = floor(v.y);
	}

	//mark vertices that need to be duplicated. Duplication happens due to a merge or split vertex where the internal angle is either too small or too large.
	//Here we care about split or merge verticies with respect to the vertical axis. In this case, if both lines are on the same side of the spliting horizontal axis then it needs to be duplicated

	//find minY, maxY
	int minY = (int)vertices.front().y;
	int maxY = (int)vertices.front().y;
	int minX = (int)vertices.front().x;
	int maxX = (int)vertices.front().x;
	for(int i=1; i<vertices.size(); i++)
	{
		minY = MathExt::min(minY, (int)vertices[i].y);
		maxY = MathExt::max(maxY, (int)vertices[i].y);
		minX = MathExt::min(minX, (int)vertices[i].x);
		maxX = MathExt::max(maxX, (int)vertices[i].x);
	}

	std::vector<std::vector<float>> scanlineCriticalPoints = std::vector<std::vector<float>>(1+maxY-minY);
	

	//for each line, gather their critical points. Also duplicate those points that are considered merge/split vertices. Treat all regular vertices as normal
	for(int i=0; i<vertices.size(); i++)
	{
		int lastIndex = i-1;
		if(lastIndex < 0)
			lastIndex = vertices.size()-1;
		int nextIndex = (i+1) % vertices.size();

		bool shouldDuplicate = false;
		
		//don't need lines to determine if it is a vertex that should be duplicated
		//check if both last and next vertex are lower or higher
		if(vertices[lastIndex].y < vertices[i].y && vertices[nextIndex].y < vertices[i].y)
		{
			shouldDuplicate = true;
		}
		else if(vertices[lastIndex].y > vertices[i].y && vertices[nextIndex].y > vertices[i].y)
		{
			shouldDuplicate = true;
		}


		//calculate Lines
		Line l = Line(vertices[i], vertices[nextIndex]);
		int lMin = l.getMinY();
		int lMax = l.getMaxY();
		
		//ignore the edge points as those are vertex points for sure.
		for(int j=lMin+1; j<lMax; j++)
		{
			float criticalPoint = l.solveForX(j);
			scanlineCriticalPoints[j-minY].push_back(criticalPoint);
		}
		
		scanlineCriticalPoints[(int)vertices[nextIndex].y-minY].push_back(vertices[nextIndex].x);

		//ignore the very first point at the lines minimum Y
		if(shouldDuplicate)
		{
			// float criticalPoint = l.solveForX(lMax);
			scanlineCriticalPoints[(int)vertices[i].y-minY].push_back(vertices[i].x);
		}
	}

	Image result = Image(maxX-minX+1, maxY-minY+1);
	//for now, print out the critical points. Later, draw them
	for(int y=0; y<scanlineCriticalPoints.size(); y++)
	{
		StringTools::println("Scan Line %d:", y);
		StringTools::print("\t");

		Sort::mergeSort(scanlineCriticalPoints[y].data(), scanlineCriticalPoints[y].size());
		for(int j=0; j<scanlineCriticalPoints[y].size(); j++)
		{
			StringTools::print("%.3f, ", scanlineCriticalPoints[y][j] + abs(minX));
		}
		StringTools::print("\n");

		//actually fill between the critical points
		
		StringTools::print("\t");
		StringTools::print("\t");
		for(int j=1; j<scanlineCriticalPoints[y].size(); j+=2)
		{
			int startPoint = scanlineCriticalPoints[y][j-1] - minX;
			int endPoint = scanlineCriticalPoints[y][j] - minX;
			
			StringTools::print("(%d, %d) - ", startPoint, endPoint);
			SimpleGraphics::fillBetween(Color{255,0,0,255}, startPoint, endPoint, y, &result);
		}
		StringTools::print("\n");
	}

	result.savePNG("TestFill.png");
}

void polygonFillTest2(const std::vector<Vec2f>& vertices)
{
	//Line sweep based approach. Sort lines by min y value, add them to list of possible lines when appropriate. Remove lines that are no longer needed.
	
	//create all lines needed
	//Sort lines by min y value (std::vector<Line>) O(NLogN)
	//for each scanline, add line from the list of sorted lines into a temporary data structure that is used for calculations. (also std::vector<Line>) O(1) Amortized
	//remove all scanlines that are no longer needed. (as you come across them, move them to the end of the list and then pop back. O(1) operation ) O(1) Amortized
	//
	
	struct CriticalPointOrLine
	{
		bool isHorizontalLine = false;
		double p1;
		double p2;
		bool operator<(const CriticalPointOrLine& other)
		{
			if(p1 == other.p1)
				return other.isHorizontalLine;
			return p1 < other.p1;
		}
		bool operator==(const CriticalPointOrLine& other)
		{
			return p1 == other.p1 && p2 == other.p2 && isHorizontalLine == other.isHorizontalLine;
		}
	};

	std::vector<Line> polyLines = std::vector<Line>(vertices.size());
	int minY = vertices.front().y;
	int maxY = vertices.front().y;
	for(size_t i=0; i<vertices.size()-1; i++)
	{
		minY = __min(vertices[i].y, minY);
		maxY = __max(vertices[i].y, maxY);
		polyLines[i] = Line(vertices[i], vertices[i+1]);
	}
	minY = __min(vertices.back().y, minY);
	maxY = __max(vertices.back().y, maxY);
	polyLines.back() = Line(vertices.back(), vertices.front());

	Sort::mergeSort<Line>(polyLines.data(), polyLines.size(), [](Line l1, Line l2) ->bool{
		return l1.getMinY() < l2.getMinY();
	});

	size_t indexInLines = 0;
	std::vector<Line> processingLines = std::vector<Line>();
	for(int y=minY; y<=maxY; y++)
	{
		for(; indexInLines < polyLines.size(); indexInLines++)
		{
			if(y >= polyLines[indexInLines].getMinY())
			{
				processingLines.push_back(polyLines[indexInLines]);
			}
			else
				break;
		}

		size_t i = 0;

		std::vector<CriticalPointOrLine> criticalPoints;
		while(i < processingLines.size())
		{
			if(y > processingLines[i].getMaxY())
			{
				std::swap(processingLines[i], processingLines.back());
				processingLines.pop_back();
			}
			else
			{
				//process line
				if(processingLines[i].getSlope() != 0)
				{
					criticalPoints.push_back({false, processingLines[i].solveForX(y), 0});
				}
				else
				{
					criticalPoints.push_back({true, processingLines[i].getMinX(), processingLines[i].getMaxX()});
				}
				
				i++;
			}
		}

		Sort::mergeSort<CriticalPointOrLine>(criticalPoints.data(), criticalPoints.size());
		bool skip = false;
		StringTools::print("%d: \n\t", y);
		for(size_t j=0; j<criticalPoints.size(); j++)
		{
			CriticalPointOrLine critPoint = criticalPoints[j];
			StringTools::print("{%d, %.3f, %.3f}, ", critPoint.isHorizontalLine, critPoint.p1, critPoint.p2);
		}

		StringTools::print(" || ");

		for(size_t j=0; j<criticalPoints.size(); j++)
		{
			CriticalPointOrLine critPoint = criticalPoints[j];
			if(critPoint.isHorizontalLine && (j % 2) == 1)
			{
				//if last point is on an odd index, duplicate previous point (minX of this line). Otherwise, skip
				if(criticalPoints[j+1].p1 != critPoint.p1)
				{
					StringTools::print("%.3f, ", critPoint.p2);
				}
				continue;
			}
			StringTools::print("%.3f, ", critPoint.p1);
		}

		StringTools::println("");
	}
}

void polygonFillTest3(const std::vector<Vec2f>& vertices)
{
	//Line sweep based approach. Sort lines by min y value, add them to list of possible lines when appropriate. Remove lines that are no longer needed.
	
	//create all lines needed
	//Sort lines by min y value (std::vector<Line>) O(NLogN)
	//for each scanline, add line from the list of sorted lines into a temporary data structure that is used for calculations. (also std::vector<Line>) O(1) Amortized
	//remove all scanlines that are no longer needed. (as you come across them, move them to the end of the list and then pop back. O(1) operation ) O(1) Amortized
	//
	
	struct CriticalPointOrLine
	{
		bool isHorizontalLine = false;
		double p1;
		double p2;
		bool operator<(const CriticalPointOrLine& other)
		{
			if(p1 == other.p1)
				return other.isHorizontalLine;
			return p1 < other.p1;
		}
		bool operator==(const CriticalPointOrLine& other)
		{
			return p1 == other.p1 && p2 == other.p2 && isHorizontalLine == other.isHorizontalLine;
		}
	};

	

	std::vector<Line> polyLines = std::vector<Line>(vertices.size());
	float minY = vertices.front().y;
	float maxY = vertices.front().y;
	float minX = vertices.front().x;
	float maxX = vertices.front().x;
	for(size_t i=0; i<vertices.size()-1; i++)
	{
		minY = __min(vertices[i].y, minY);
		maxY = __max(vertices[i].y, maxY);
		minX = __min(vertices[i].x, minX);
		maxX = __max(vertices[i].x, maxX);
		polyLines[i] = Line(vertices[i], vertices[i+1]);
	}
	minY = __min(vertices.back().y, minY);
	maxY = __max(vertices.back().y, maxY);
	minX = __min(vertices.back().x, minX);
	maxX = __max(vertices.back().x, maxX);
	polyLines.back() = Line(vertices.back(), vertices.front());

	Sort::mergeSort<Line>(polyLines.data(), polyLines.size(), [](Line l1, Line l2) ->bool{
		return l1.getMinY() < l2.getMinY();
	});

	Image outputImage = Image((int)(maxX-minX)+1, (int)(maxY-minY)+1);
	size_t indexInLines = 0;
	std::vector<Line> processingLines = std::vector<Line>();
	for(int y=(int)minY; y<=(int)maxY; y++)
	{
		for(; indexInLines < polyLines.size(); indexInLines++)
		{
			if(y >= polyLines[indexInLines].getMinY())
			{
				processingLines.push_back(polyLines[indexInLines]);
			}
			else
				break;
		}

		size_t i = 0;

		std::vector<CriticalPointOrLine> criticalPoints;
		while(i < processingLines.size())
		{
			if(y > processingLines[i].getMaxY())
			{
				std::swap(processingLines[i], processingLines.back());
				processingLines.pop_back();
			}
			else
			{
				//process line
				if(processingLines[i].getSlope() != 0)
				{
					criticalPoints.push_back({false, processingLines[i].solveForX(y), 0});
				}
				else
				{
					criticalPoints.push_back({true, processingLines[i].getMinX(), processingLines[i].getMaxX()});
				}
				i++;
			}
		}

		Sort::mergeSort<CriticalPointOrLine>(criticalPoints.data(), criticalPoints.size());
		std::vector<double> finalPairs;
		// bool skip = false;
		// StringTools::print("%d: \n\t", y);
		// for(size_t j=0; j<criticalPoints.size(); j++)
		// {
		//     CriticalPointOrLine critPoint = criticalPoints[j];
		//     StringTools::print("{%d, %.3f, %.3f}, ", critPoint.isHorizontalLine, critPoint.p1, critPoint.p2);
		// }

		// StringTools::print(" || ");

		for(size_t j=0; j<criticalPoints.size(); j++)
		{
			CriticalPointOrLine critPoint = criticalPoints[j];
			if(critPoint.isHorizontalLine && (j % 2) == 1)
			{
				//if last point is on an odd index, duplicate previous point (minX of this line). Otherwise, skip
				if(criticalPoints[j+1].p1 != critPoint.p1)
				{
					// StringTools::print("%.3f, ", critPoint.p2);
					finalPairs.push_back(critPoint.p2);
				}
				continue;
			}
			// StringTools::print("%.3f, ", critPoint.p1);
			finalPairs.push_back(critPoint.p1);
		}

		StringTools::print("%d:\n\t", y);
		for(size_t j=1; j<finalPairs.size(); j+=2)
		{
			int startX = MathExt::floor(finalPairs[j-1] - minX);
			int endX = MathExt::ceil(finalPairs[j] - minX);
			
			StringTools::print("(%d, %d) - ", startX, endX);
			SimpleGraphics::fillBetween(Color{255,0,0,255}, startX, endX, y-minY, &outputImage);
		}
		StringTools::println("");
	}
	outputImage.savePNG("TestFill3.png");
}

#include "DCEL.h"
void testDCEL()
{
	DCEL test = DCEL();
	test.addPolygon({
		Vec2f(0, 0),
		Vec2f(32, 32),
		Vec2f(64, 32)
	});

	StringTools::println("\n______VERTICES______\n");
	//for each Vertex, print its MemoryLocation and position
	for(size_t index=0; index < test.getTotalVertices(); index++)
	{
		StringTools::println("%p: %.3f", test.getVertex(index), test.getVertex(index)->position);
	}
	
	StringTools::println("\n______HALF EDGES______\n");
	//for each Half Edge, print its MemoryLocation and all the pointers
	for(size_t index=0; index < test.getTotalEdges(); index++)
	{
		DCELEdge* halfEdge = test.getEdge(index);
		StringTools::println("%p: ", halfEdge);
		StringTools::println("\tOrigin: %p\n\tTwin Edge: %p\n\tIncident Face: %p\n\tNext Edge: %p\n\tPrevious Edge: %p", halfEdge->origin, halfEdge->twin, halfEdge->incidentFace, halfEdge->next, halfEdge->previous);
	}

	StringTools::println("\n______FACES______\n");
	//for each Face, print its MemoryLocation and all the pointers
	for(size_t index=0; index < test.getTotalFaces(); index++)
	{
		DCELFace* face = test.getFace(index);
		StringTools::println("%p: ", face);
		StringTools::println("\tOuter Component: %p\n\tInner Components: %p\n\tClockwise: %d", face->outerComponent, face->innerComponents, face->windingOrder);
	}
}

void quickDCELVertexStuff()
{
	DCEL test = DCEL();
	DCELVertex* v1 = test.addVertex(Vec2f(0, 0));
	DCELVertex* v2 = test.addVertex(Vec2f(32, 0));
	DCELVertex* v3 = test.addVertex(Vec2f(32, 32));
	test.addEdge(v1, v2);
	test.addEdge(v2, v3);
	test.addEdge(v3, v1);

	StringTools::println("BEFORE VERTEX INTERSECTION");
	StringTools::println("\n______VERTICES______\n");
	//for each Vertex, print its MemoryLocation and position
	for(size_t index=0; index < test.getTotalVertices(); index++)
	{
		StringTools::println("%p: %.3f", test.getVertex(index), test.getVertex(index)->position);
	}
	
	StringTools::println("\n______HALF EDGES______\n");
	//for each Half Edge, print its MemoryLocation and all the pointers
	for(size_t index=0; index < test.getTotalEdges(); index++)
	{
		DCELEdge* halfEdge = test.getEdge(index);
		StringTools::println("%p: ", halfEdge);
		StringTools::println("\tOrigin: %p\n\tTwin Edge: %p\n\tIncident Face: %p\n\tNext Edge: %p\n\tPrevious Edge: %p", halfEdge->origin, halfEdge->twin, halfEdge->incidentFace, halfEdge->next, halfEdge->previous);
	}

	StringTools::println("\n______FACES______\n");
	//for each Face, print its MemoryLocation and all the pointers
	for(size_t index=0; index < test.getTotalFaces(); index++)
	{
		DCELFace* face = test.getFace(index);
		StringTools::println("%p: ", face);
		StringTools::println("\tOuter Component: %p\n\tInner Components: %p\n\tClockwise: %d", face->outerComponent, face->innerComponents, face->windingOrder);
	}
	
	test.addVertex(Vec2f(16, 0));
	StringTools::println("AFTER VERTEX INTERSECTION");
	StringTools::println("\n______VERTICES______\n");
	//for each Vertex, print its MemoryLocation and position
	for(size_t index=0; index < test.getTotalVertices(); index++)
	{
		StringTools::println("%p: %.3f", test.getVertex(index), test.getVertex(index)->position);
	}
	
	StringTools::println("\n______HALF EDGES______\n");
	//for each Half Edge, print its MemoryLocation and all the pointers
	for(size_t index=0; index < test.getTotalEdges(); index++)
	{
		DCELEdge* halfEdge = test.getEdge(index);
		StringTools::println("%p: ", halfEdge);
		StringTools::println("\tOrigin: %p\n\tTwin Edge: %p\n\tIncident Face: %p\n\tNext Edge: %p\n\tPrevious Edge: %p", halfEdge->origin, halfEdge->twin, halfEdge->incidentFace, halfEdge->next, halfEdge->previous);
	}

	StringTools::println("\n______FACES______\n");
	//for each Face, print its MemoryLocation and all the pointers
	for(size_t index=0; index < test.getTotalFaces(); index++)
	{
		DCELFace* face = test.getFace(index);
		StringTools::println("%p: ", face);
		StringTools::println("\tOuter Component: %p\n\tInner Components: %p\n\tClockwise: %d", face->outerComponent, face->innerComponents, face->windingOrder);
	}
}

bool detectPrime(size_t i)
{
	if(i == 2)
		return true;

	size_t max = ceil(sqrt(i));
	for(size_t j=2; j<=max; j++)
	{
		size_t temp = i % j;
		if(temp == 0)
			return false;
	}
	return true;
}

#include "SimpleJobQueue.h"
void taskLatency()
{
	//test the speed at which openmp will call functions
	//test the speed at which our jobqueue will call functions
	//test the total speed of execution of both
	//test the total speed of single threaded too.

	const int totalThreads = 12;
	std::atomic_int totalPrimes = 0;
	size_t t1, t2;

	// t1 = System::getCurrentTimeNano();
	// for(size_t i=0; i<100000; i++)
	// {
	// 	totalPrimes += detectPrime(i);
	// }
	// t2 = System::getCurrentTimeNano();
	// StringTools::println("Total time for single threaded: %lluns - Found %d primes", t2-t1, (int)totalPrimes);

	// totalPrimes = 0;
	// t1 = System::getCurrentTimeNano();
	// omp_set_num_threads(8);
	// #pragma omp parallel for
	// for(size_t i=0; i<100000; i++)
	// {
	// 	totalPrimes += detectPrime(i);
	// }
	// t2 = System::getCurrentTimeNano();
	// StringTools::println("Total time for OMP: %lluns - Found %d primes", t2-t1, (int)totalPrimes);

	// totalPrimes = 0;
	// SimpleJobQueue jobQueue = SimpleJobQueue(8);
	// t1 = System::getCurrentTimeNano();
	// for(size_t i=0; i<100000; i++)
	// {
	// 	jobQueue.addJob([&totalPrimes, i]() ->void{
	// 		totalPrimes += detectPrime(i);
	// 	});
	// }
	// jobQueue.waitForAllJobs();
	// t2 = System::getCurrentTimeNano();
	// StringTools::println("Total time for SimpleJobQueue NAIVE: %lluns - Found %d primes", t2-t1, (int)totalPrimes);

	// totalPrimes = 0;
	// SimpleJobQueue jobQueue = SimpleJobQueue(totalThreads);
	// size_t max = 100000;
	// size_t incr = 100000/totalThreads;
	// t1 = System::getCurrentTimeNano();
	
	// size_t start = 0;
	// size_t end = 0;
	// for(size_t i=0; i<totalThreads-1; i++)
	// {
	// 	start = incr*i;
	// 	end = incr*(i+1);
	// 	jobQueue.addJob([&totalPrimes, start, end]() ->void{
	// 		for(size_t i=start; i<end; i++)
	// 		{
	// 			totalPrimes += detectPrime(i);
	// 		}
	// 	});
	// }
	// start = incr*(totalThreads-1);
	// end = max;
	
	// jobQueue.addJob([&totalPrimes, start, end]() ->void{
	// 	for(size_t i=start; i<end; i++)
	// 	{
	// 		totalPrimes += detectPrime(i);
	// 	}
	// });
	
	// jobQueue.waitForAllJobs();
	// t2 = System::getCurrentTimeNano();
	// StringTools::println("Total time for SimpleJobQueue SMARTER: %lluns - Found %d primes", t2-t1, (int)totalPrimes);
	
	totalPrimes = 0;
	t1 = System::getCurrentTimeNano();
	SimpleJobQueue jobQueue = SimpleJobQueue(totalThreads);
	jobQueue.parallelize([&totalPrimes](size_t tStart, size_t tEnd, size_t incr)->void{
		for(size_t i=tStart; i<tEnd; i+=incr)
		{
			totalPrimes += detectPrime(i);
		}
	}, 0, 100000, 1, totalThreads);
	
	t2 = System::getCurrentTimeNano();
	StringTools::println("Total time for SimpleJobQueue::parallelize(): %lluns - Found %d primes", t2-t1, (int)totalPrimes);

	// totalPrimes = 0;
	// t1 = System::getCurrentTimeNano();
	// std::vector<std::thread> threads = std::vector<std::thread>(totalThreads);
	// size_t max = 100000;
	// size_t incr = 100000/totalThreads;
	// std::function<void(std::atomic_int* totalPrimes, size_t start, size_t end)> func = [](std::atomic_int* totalPrimes, size_t start, size_t end)->void{
	// 	for(size_t i=start; i<end; i++)
	// 	{
	// 		*totalPrimes += detectPrime(i);
	// 	}
	// };

	// for(int i=0; i<totalThreads-1; i++)
	// {
	// 	threads[i] = std::thread(func, &totalPrimes, incr*i, incr*(i+1));
	// }
	// threads[totalThreads-1] = std::thread(func, &totalPrimes, incr*(totalThreads-1), max);
	
	// for(int i=0; i<totalThreads; i++)
	// {
	// 	threads[i].join();
	// }
	// t2 = System::getCurrentTimeNano();
	// StringTools::println("Total time for Raw Threads: %lluns - Found %d primes", t2-t1, (int)totalPrimes);

	//Total time for SimpleJobQueue NAIVE: 304367200ns - Found 9594 primes
	//Total time for SimpleJobQueue SMARTER: 15833800ns - Found 9593 primes
	//Total time for Raw Threads: 2142200ns - Found 9594 primes

	/*
		3837400ns		- Single threaded
		4135400ns		- OMP 
								~1.08x increase over single threaded
		304367200ns		- NAIVE JQ 
								~79.32x increase over single threaded
		3728600ns		- SMARTER JQ 
								~81.6x reduction from naive simple job queue
								~1.03x reduction from single threaded work
		2122600ns		- SMARTER2 JQ 
								~142x reduction from naive simple job queue
								~1.81 reduction from single threaded
		2142200ns		- RAW THREADS 
								~142x reduction from naive simple job queue
								~1.79 reduction from single threaded

		1443300ns		- NOTE: Thread creation taken out of the equation
		2207500ns		- NOTE: Thread creation in the equation now
		2386100ns
	*/
}

int main(int argc, char** argv)
{
	// taskLatency();
	// // std::vector<Vec2f> vertices = {Vec2f(-49.1, 96.2), Vec2f(4.4, 37.3), Vec2f(12.6, 102)};
	// // std::vector<Vec2f> vertices = {Vec2f(100,200), Vec2f(300,200), Vec2f(275,150), Vec2f(200,150), Vec2f(150,100)};
	// // std::vector<Vec2f> vertices = {Vec2f(150,100), Vec2f(200,150), Vec2f(275,150), Vec2f(300,200), Vec2f(100,200)};
	// std::vector<Vec2f> vertices = {Vec2f(-7.5, 0), Vec2f(7.5, 0), Vec2f(-5, -10), Vec2f(0, 5), Vec2f(5, -10)};
	// // std::vector<Vec2f> vertices = {Vec2f(5, -10), Vec2f(0, 5), Vec2f(-5, -10), Vec2f(7.5, 0), Vec2f(-7.5, 0)};

	// // polygonFillTest(vertices);
	// // polygonFillTest2(vertices);
	// // polygonFillTest3(vertices);
	// Image outputImage = Image(100, 100);
	// SimpleGraphics::setColor({255,0,0,255});
	// SimpleGraphics::setFillRule(SimpleGraphics::FILL_NON_ZERO);
	// SimpleGraphics::drawPolygon(vertices.data(), vertices.size(), &outputImage);
	// outputImage.savePNG("ProperPolyTest.png");

	size_t t1, t2;
	// //vary between 0, 255
	Image img = Image(4096, 4096);
	// FlatFillFunction baseFillFunction({255, 160, 100, 255});

	// t1 = System::getCurrentTimeMicro();
	// for(int y=0; y<256; y++)
	// {
	// 	baseFillFunction.fillBetween(0, 256, y, &img);
	// }
	// t2 = System::getCurrentTimeMicro();
	// StringTools::println("Time For Flat Fill: %llu", t2-t1);
	// img.savePNG("DefaultFillFunction.png", false, false, false);

	
	LinearGradientFillFunction linGradFillFunction(Vec2f(0, 0), Vec2f(256, 256), {{0, Color{0, 0, 0, 255}}, {0.2, Color{255, 255, 0, 255}}, {0.8, Color{0, 0, 255, 255}}, {1, Color{255, 0, 0, 255}}});

	// t1 = System::getCurrentTimeMicro();
	// for(int y=0; y<256; y++)
	// {
	// 	linGradFillFunction.fillBetween(0, 256, y, &img);
	// }
	// t2 = System::getCurrentTimeMicro();
	// StringTools::println("Time For Linear Gradient Fill: %llu", t2-t1);
	// img.savePNG("LinearFillFunction.png", false, false, false);

	// RadialGradientFillFunction radGradFillFunction(Vec2f(128, 128), {{16, Color{0, 0, 0, 255}}, {32, Color{255, 255, 0, 255}}, {64, Color{0, 0, 255, 255}}, {96, Color{255, 0, 0, 255}}});
	RadialGradientFillFunction radGradFillFunction(Vec2f(2048, 2048), {{2046, Color{255, 255, 255, 255}}, {2047, Color{0, 0, 0, 0}}});

	Image maskImage = Image(4096, 4096);
	maskImage.setAllPixels({255, 255, 255, 255});
	
	// SimpleGraphics::setColor({255, 255, 255, 0});
	// SimpleGraphics::setCompositeRule(SimpleGraphics::NO_COMPOSITE);
	// SimpleGraphics::drawRect(2048-64, 2048-64, 128, 128, false, &maskImage);
	// SimpleGraphics::setCompositeRule(SimpleGraphics::COMPOSITE_SRC_OVER);

	SimpleGraphics::setMask(maskImage);

		Image tempImg = Image(256, 256);
		Image tempImg2 = Image(256, 256);
		tempImg.setAllPixels({0, 0, 0, 0});
		tempImg2.setAllPixels({0, 0, 0, 0});
	t1 = System::getCurrentTimeMicro();
	for(int i=0; i<100; i++)
	{
		SJQ_OMP_Replacement::parallelize([&img, &tempImg, &tempImg2, &radGradFillFunction, &linGradFillFunction](size_t tStart, size_t tEnd, size_t tIncr)->void{
			for(int y=tStart; y<tEnd; y+=tIncr)
			{
				radGradFillFunction.fillBetween(0, 4096, y, &img);
				// linGradFillFunction.fillBetween(0, 256, y, &tempImg2);
				// for(int x=0; x<256; x+=1)
				// {
				// 	// SIMD_U32 mult = SIMD_U32::load((uint32_t*)&tempImg.getPixels()[x + y*tempImg.getWidth()]);
				// 	// SIMD_U32 c = SIMD_U32::load((uint32_t*)&tempImg2.getPixels()[x + y*tempImg2.getWidth()]);
				// 	// SIMD_U32 outputColor = SimpleGraphics::multColor(c.values, mult.values);
				// 	// SIMD_U32 destColor = SIMD_U32::load((uint32_t*)&img.getPixels()[x + y*img.getWidth()]);
				// 	// outputColor = SimpleGraphics::blend(outputColor.values, destColor.values);
				// 	// outputColor.store((uint32_t*)&img.getPixels()[x + y*img.getWidth()]);
				// 	Color mult = tempImg.getPixels()[x + y*img.getWidth()];
				// 	Color c = tempImg2.getPixels()[x + y*img.getWidth()];
				// 	SimpleGraphics::drawPixel(x, y, SimpleGraphics::multColor(c, mult), &img);
				// }
			}
		}, 0, 4096, 1, 24);
	}
	// for(int y=0; y<256; y++)
	// {
	// 	radGradFillFunction.fillBetween(0, 256, y, &img);
	// }
	t2 = System::getCurrentTimeMicro();
	StringTools::println("Time For Radial Gradient Fill: %llu", (t2-t1)/100);
	img.savePNG("RadialFillFunction.png", false, false, false);

	
	Image img2 = Image(4096, 4096);
	// t1 = System::getCurrentTimeMicro();
	// SimpleGraphics::setColor({0, 0, 0, 255});
	// for(int i=0; i<1; i++)
	// {
	// 	SimpleGraphics::drawRect(0, 0, 4096, 4096, false, &img2);
	// }
	// t2 = System::getCurrentTimeMicro();
	// StringTools::println("Time For Draw Rect: %llu", t2-t1);

	/*
		30888	- single threaded
		7520	- OMP
		3668	- SimpleJobQueue
	*/
	
	SimpleGraphics::setAntiAliasing(true);
	t1 = System::getCurrentTimeMicro();
	SimpleGraphics::setColor({255, 255, 255, 255});
	
	for(int i=0; i<100; i++)
	{
		// SimpleGraphics::drawCircle(2048, 2048, 2047, false, &img2);
		SimpleGraphics::drawCircle(2048, 2048, 2047, 128, &img2);
	}
	t2 = System::getCurrentTimeMicro();
	StringTools::println("Time For Draw Circle: %llu", (t2-t1)/100);
	img2.savePNG("circleDraw.png", false, false, false);
	
	// t1 = System::getCurrentTimeMicro();
	// SimpleGraphics::setColor({255, 255, 0, 255});
	
	// for(int i=0; i<1000; i++)
	// {
	// 	SimpleGraphics::drawCircle(128, 128, 96, 16, &img2);
	// }
	// t2 = System::getCurrentTimeMicro();
	// StringTools::println("Time For Draw Circle2: %llu", (t2-t1)/1000);
	

	// testDCEL();
	// quickDCELVertexStuff();
	return 0;
}

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






// #include <Network.h>
// #include "FiberTask.h"
// #include "SimpleFile.h"
// #include "SimplePipe.h"
// #include "SimpleWindow.h"
// #include "Streamable.h"
// #include <System.h>
// #include <WebRequest.h>
// #include <File.h>
// #include <StringTools.h>
// #include <HttpClient.h>
// #include <HttpServer.h>
// #include <SimpleJSON.h>
// #include <atomic>
// #include <chrono>
// #include <mutex>
// #include <vector>

// using namespace smpl;

// // smpl::SimpleJSON setupOAuth(SimpleJSON clientSecretJSON, std::string scopeURL, int type)
// // {
// //     smpl::SimpleJSON outputJSON;
// //     std::string clientID = ((smpl::JPair&)clientSecretJSON["installed"]["client_id"]).getValue();
// //     std::string clientSecret = ((smpl::JPair&)clientSecretJSON["installed"]["client_secret"]).getValue();
// //     std::string authURL = ((smpl::JPair&)clientSecretJSON["installed"]["auth_uri"]).getValue();
// //     std::string tokenURL = ((smpl::JPair&)clientSecretJSON["installed"]["token_uri"]).getValue();
// //     std::string redirect_uri = "urn:ietf:wg:oauth:2.0:oob"; //if type is manual code auth

// //     std::string finalURL = authURL + "?client_id=" + clientID
// //                      + "&response_type=code&scope=" + scopeURL
// //                      + "&access_type=offline&redirect_uri=";
// //     std::string finalAuthCode = "";
	
// //     if(type == 0)
// //     {
// //         NetworkConfig config;
// //         config.amountOfConnectionsAllowed = 4;
// //         config.location = "127.0.0.1";
// //         config.port = 0;
// //         config.secure = false;
// //         config.TCP = true;
// //         config.type = Network::TYPE_SERVER;

// //         std::mutex quickMut;
// //         std::string responseURL = "";

// //         std::string indexHTML = "<!DOCTYPE html><html><body><h1>Safe to close now</h1></body></html>";
// //         HttpServer server = HttpServer(config, 1);
// //         server.setGetFuncMapper([&indexHTML, &responseURL, &quickMut](HttpServer* servP, WebRequest& req, std::vector<unsigned char>& body, std::string location, size_t id) -> char{
			
// //             // url should contain the response code needed
// //             // DO NOT LET THE USER GO TO THE LOCAL HOST AS IT WILL MESS UP THIS PROCESS. SERVER WILL CLOSE RIGHT AFTER DOING THIS
// //             quickMut.lock();
// //             responseURL = req.getUrl();
// //             quickMut.unlock();

// //             //send html response back. Doesn't matter if they asked for index.html or anything. Always send this. Make it look pretty one day
// //             WebRequest response;
// //             response.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 200 OK", false);
// //             response.addKeyValue("Content-Type", WebRequest::getMimeTypeFromExt("html"));
// //             response.addKeyValue("Content-Length", StringTools::toString(indexHTML.size()));
			
// //             servP->sendResponse(response, location, id, req);
// //             servP->sendMessage(indexHTML.c_str(), indexHTML.size(), id);
			
// //             return HttpServer::STATUS_DONE;
// //         });

// //         server.start();
// //         int actualPort = server.getNetworkConnection()->getPort();
		
// //         finalURL += "http://127.0.0.1:" + StringTools::toString(actualPort);
// //         system(("start msedge \"" + finalURL + "\"").c_str());

// //         while(server.getRunning())
// //         {
// //             System::sleep(10, 0, false);

// //             quickMut.lock();
// //             bool canQuit = !responseURL.empty();
// //             quickMut.unlock();

// //             if(canQuit)
// //                 break;
// //         }

// //         //parse URL
		
// //     }
// //     else
// //     {
// //         finalURL += "urn:ietf:wg:oauth:2.0:oob";
// //         system(("start msedge \"" + finalURL + "\"").c_str());

// //         smpl::StringTools::print("ENTER AUTH CODE: ");
// //         finalAuthCode = smpl::StringTools::getString();
// //     }
// // }

// // smpl::SimpleJSON setupOAUTH(smpl::File clientSecretJSON, std::string scopeURL)
// // {
// //     smpl::SimpleJSON outputJSON;

// //     smpl::SimpleJSON clientInfo = smpl::SimpleJSON(clientSecretJSON.getFullFileName());
// //     std::string clientID = ((smpl::JPair&)clientInfo["installed"]["client_id"]).getValue();
// //     std::string clientSecret = ((smpl::JPair&)clientInfo["installed"]["client_secret"]).getValue();
// //     std::string authURL = ((smpl::JPair&)clientInfo["installed"]["auth_uri"]).getValue();
// //     std::string tokenURL = "https://www.googleapis.com/oauth2/v4/token";
	
// //     std::string finalURL = authURL + "?client_id=" + clientID
// //                      + "&response_type=code&scope=" + scopeURL
// //                      + "&access_type=offline&redirect_uri=urn:ietf:wg:oauth:2.0:oob";
	
// //     system(("start msedge \"" + finalURL + "\"").c_str());

// //     smpl::StringTools::print("ENTER AUTH CODE: ");
// //     std::string finalAuthCode = smpl::StringTools::getString();
	
// //     if(finalAuthCode.empty())
// //         return outputJSON;
	
// //     std::string body = "client_id="+clientID+"&client_secret="+clientSecret+"&code="+finalAuthCode
// //                     + "&redirect_uri=urn:ietf:wg:oauth:2.0:oob&grant_type=authorization_code";

// //     smpl::HttpClient client = smpl::HttpClient(tokenURL);
// //     smpl::WebRequest requestAuthHeaders = smpl::WebRequest();
// //     requestAuthHeaders.setHeader(smpl::WebRequest::TYPE_POST, "/oauth2/v4/token");
// //     requestAuthHeaders.addKeyValue("Host", "www.googleapis.com");
// //     requestAuthHeaders.addKeyValue("User-Agent", "TestApplication1");
// //     requestAuthHeaders.addKeyValue("Content-Length", smpl::StringTools::toString(body.size()));
// //     requestAuthHeaders.addKeyValue("Content-Type", "application/x-www-form-urlencoded");

// //     std::atomic_bool isConnected = true;
// //     client.setOnConnectFunc([&requestAuthHeaders, &body](smpl::HttpClient* clientPointer) -> void{
// //         int bytesSent = clientPointer->sendRequest(requestAuthHeaders);
// //         smpl::StringTools::println("BYTES SENT: %d vs EXPECTED: %llu", bytesSent, requestAuthHeaders.getBytesInRequest());
// //         bytesSent = clientPointer->sendMessage(body.c_str(), body.size());
// //         smpl::StringTools::println("BYTES SENT: %d vs EXPECTED: %llu", bytesSent, body.size());
// //         smpl::StringTools::println("CONNECTED");
// //     });
	
// //     client.setOnBufferChangedFunc([&outputJSON, &isConnected](smpl::HttpClient* clientPointer, smpl::WebRequest& request, unsigned char* buffer, size_t bufferSize) ->void {
// //         //should be a json. Load as one. Should disconnect right after too
// //         smpl::StringTools::println("DATA RECEIVED");
// //         smpl::StreamableArray<unsigned char> streamableBuffer(buffer, bufferSize, false);
// //         outputJSON = smpl::SimpleJSON(&streamableBuffer);
// //         clientPointer->disconnect();
// //         isConnected = false;
// //     });

// //     client.setOnDisconnectFunc([&isConnected](smpl::HttpClient* clientPointer) ->void{
// //         isConnected = false;
// //         smpl::StringTools::println("DISCONNECTED");
// //     });
// //     client.start();

// //     if(client.getTimeoutOccurred())
// //     {
// //         smpl::StringTools::println("FAILED TO CONNECT");
// //         isConnected = false;
// //     }

// //     while(client.isValid() && isConnected)
// //     {
// //         if(client.getShouldRedirect())
// //             client.completeRedirect();
// //         smpl::System::sleep(10, 0, false);
// //     }

// //     return outputJSON;
// // }

// // void createOAuthServer(smpl::File clientSecretJSON, std::string scopeURL)
// // {


// //     NetworkConfig config;
// //     config.amountOfConnectionsAllowed = 4;
// //     config.location = "127.0.0.1";
// //     config.port = 0;
// //     config.secure = false;
// //     config.TCP = true;
// //     config.type = Network::TYPE_SERVER;

// //     std::string indexHTML = "<!DOCTYPE html><html><body><p>Safe to close now</p></body></html>";
// //     HttpServer server = HttpServer(config, 2);
// //     server.setGetFuncMapper([&indexHTML](HttpServer* servP, WebRequest& req, std::vector<unsigned char>& body, std::string location, size_t id) -> char{
// //         StringTools::println(req.getUrl());
// //         WebRequest response;
// //         response.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 200 OK", false);
// //         response.addKeyValue("Content-Type", WebRequest::getMimeTypeFromExt("html"));
// //         response.addKeyValue("Content-Length", StringTools::toString(indexHTML.size()));
		
// //         servP->sendResponse(response, location, id, req);
// //         servP->sendMessage(indexHTML.c_str(), indexHTML.size(), id);
		
// //         return HttpServer::STATUS_DONE;
// //     });

// //     server.start();
// //     int actualPort = server.getNetworkConnection()->getPort();

// //     StringTools::println("Server on: http://127.0.0.1:%d", actualPort);

// //     smpl::SimpleJSON clientInfo = smpl::SimpleJSON(clientSecretJSON.getFullFileName());
// //     std::string clientID = ((smpl::JPair&)clientInfo["installed"]["client_id"]).getValue();
// //     std::string clientSecret = ((smpl::JPair&)clientInfo["installed"]["client_secret"]).getValue();
// //     std::string authURL = ((smpl::JPair&)clientInfo["installed"]["auth_uri"]).getValue();
// //     std::string tokenURL = "https://www.googleapis.com/oauth2/v4/token";
	
// //     std::string finalURL = authURL + "?client_id=" + clientID
// //                      + "&response_type=code&scope=" + scopeURL
// //                      + "&access_type=offline&redirect_uri=http://127.0.0.1:" + StringTools::toString(actualPort);
	
// //     StringTools::println(finalURL);
	
// //     system(("start msedge \"" + finalURL + "\"").c_str());

// //     while(server.getRunning())
// //     {
// //         System::sleep(10, 0, false);
// //     }
// // }

// int main()
// {
//     // smpl::SimpleJSON authStuff = setupOAUTH("client_secret.json", 
//     //                                         "https://www.googleapis.com/auth/youtube");
	
//     // // smpl::StringTools::println("%s", authStuff.getString(true));
//     // createOAuthServer("client_secret.json", "https://www.googleapis.com/auth/youtube");

//     // auto output = StringTools::parseURLArguments("/?code=4/0ATX87lOb7x-IZz7zUHd7I_kfdejcagXqNZV0KQkFhlXTbCuL9yErhU2GtIcyUYOX9N2icQ&scope=https://www.googleapis.com/auth/youtube");
//     // StringTools::println("%s", output);
	
// 	// runBasicHTTPServer();
//     return 0;
// }

// //client_id: Kd4Ue8jYiWhocNK1eSuE1WZ7
// //client_secret: lPktggkJ7GIt8Jg9wexta1506Pyv22Z3uAXgbRAb590G1W7q
// //login: bewildered-skylark@example.com
// //password: Powerful-Partridge-56