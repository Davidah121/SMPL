// #include "StringTools.h"
// #include "System.h"
// #include "WebSocket.h"
// #include <atomic>
// #include <functional>
// #include <omp.h>
// #include <thread>
// #include <unordered_set>
// #include "ext/KeyboardSimulator.h"
// #include "Input.h"
// #include "SIMD.h"
// #include "SimpleGraphics.h"

// using namespace smpl;

// void polygonFillTest(const std::vector<Vec2f>& vertices)
// {
// 	//somewhat naive approach. For each line, calculate the intersections with the valid scanlines based on max and min y values
// 	if(vertices.size() < 3)
// 		return;
	
// 	//ignoring anti-aliasing potential, drop all decimal values
// 	for(Vec2f v : vertices)
// 	{
// 		v.x = floor(v.x);
// 		v.y = floor(v.y);
// 	}

// 	//mark vertices that need to be duplicated. Duplication happens due to a merge or split vertex where the internal angle is either too small or too large.
// 	//Here we care about split or merge verticies with respect to the vertical axis. In this case, if both lines are on the same side of the spliting horizontal axis then it needs to be duplicated

// 	//find minY, maxY
// 	int minY = (int)vertices.front().y;
// 	int maxY = (int)vertices.front().y;
// 	int minX = (int)vertices.front().x;
// 	int maxX = (int)vertices.front().x;
// 	for(int i=1; i<vertices.size(); i++)
// 	{
// 		minY = MathExt::min(minY, (int)vertices[i].y);
// 		maxY = MathExt::max(maxY, (int)vertices[i].y);
// 		minX = MathExt::min(minX, (int)vertices[i].x);
// 		maxX = MathExt::max(maxX, (int)vertices[i].x);
// 	}

// 	std::vector<std::vector<float>> scanlineCriticalPoints = std::vector<std::vector<float>>(1+maxY-minY);
	

// 	//for each line, gather their critical points. Also duplicate those points that are considered merge/split vertices. Treat all regular vertices as normal
// 	for(int i=0; i<vertices.size(); i++)
// 	{
// 		int lastIndex = i-1;
// 		if(lastIndex < 0)
// 			lastIndex = vertices.size()-1;
// 		int nextIndex = (i+1) % vertices.size();

// 		bool shouldDuplicate = false;
		
// 		//don't need lines to determine if it is a vertex that should be duplicated
// 		//check if both last and next vertex are lower or higher
// 		if(vertices[lastIndex].y < vertices[i].y && vertices[nextIndex].y < vertices[i].y)
// 		{
// 			shouldDuplicate = true;
// 		}
// 		else if(vertices[lastIndex].y > vertices[i].y && vertices[nextIndex].y > vertices[i].y)
// 		{
// 			shouldDuplicate = true;
// 		}


// 		//calculate Lines
// 		Line l = Line(vertices[i], vertices[nextIndex]);
// 		int lMin = l.getMinY();
// 		int lMax = l.getMaxY();
		
// 		//ignore the edge points as those are vertex points for sure.
// 		for(int j=lMin+1; j<lMax; j++)
// 		{
// 			float criticalPoint = l.solveForX(j);
// 			scanlineCriticalPoints[j-minY].push_back(criticalPoint);
// 		}
		
// 		scanlineCriticalPoints[(int)vertices[nextIndex].y-minY].push_back(vertices[nextIndex].x);

// 		//ignore the very first point at the lines minimum Y
// 		if(shouldDuplicate)
// 		{
// 			// float criticalPoint = l.solveForX(lMax);
// 			scanlineCriticalPoints[(int)vertices[i].y-minY].push_back(vertices[i].x);
// 		}
// 	}

// 	Image result = Image(maxX-minX+1, maxY-minY+1);
// 	//for now, print out the critical points. Later, draw them
// 	for(int y=0; y<scanlineCriticalPoints.size(); y++)
// 	{
// 		StringTools::println("Scan Line %d:", y);
// 		StringTools::print("\t");

// 		Sort::mergeSort(scanlineCriticalPoints[y].data(), scanlineCriticalPoints[y].size());
// 		for(int j=0; j<scanlineCriticalPoints[y].size(); j++)
// 		{
// 			StringTools::print("%.3f, ", scanlineCriticalPoints[y][j] + abs(minX));
// 		}
// 		StringTools::print("\n");

// 		//actually fill between the critical points
		
// 		StringTools::print("\t");
// 		StringTools::print("\t");
// 		for(int j=1; j<scanlineCriticalPoints[y].size(); j+=2)
// 		{
// 			int startPoint = scanlineCriticalPoints[y][j-1] - minX;
// 			int endPoint = scanlineCriticalPoints[y][j] - minX;
			
// 			StringTools::print("(%d, %d) - ", startPoint, endPoint);
// 			SimpleGraphics::fillBetween(Color{255,0,0,255}, startPoint, endPoint, y, &result);
// 		}
// 		StringTools::print("\n");
// 	}

// 	result.savePNG("TestFill.png");
// }

// void polygonFillTest2(const std::vector<Vec2f>& vertices)
// {
// 	//Line sweep based approach. Sort lines by min y value, add them to list of possible lines when appropriate. Remove lines that are no longer needed.
	
// 	//create all lines needed
// 	//Sort lines by min y value (std::vector<Line>) O(NLogN)
// 	//for each scanline, add line from the list of sorted lines into a temporary data structure that is used for calculations. (also std::vector<Line>) O(1) Amortized
// 	//remove all scanlines that are no longer needed. (as you come across them, move them to the end of the list and then pop back. O(1) operation ) O(1) Amortized
// 	//
	
// 	struct CriticalPointOrLine
// 	{
// 		bool isHorizontalLine = false;
// 		double p1;
// 		double p2;
// 		bool operator<(const CriticalPointOrLine& other)
// 		{
// 			if(p1 == other.p1)
// 				return other.isHorizontalLine;
// 			return p1 < other.p1;
// 		}
// 		bool operator==(const CriticalPointOrLine& other)
// 		{
// 			return p1 == other.p1 && p2 == other.p2 && isHorizontalLine == other.isHorizontalLine;
// 		}
// 	};

// 	std::vector<Line> polyLines = std::vector<Line>(vertices.size());
// 	int minY = vertices.front().y;
// 	int maxY = vertices.front().y;
// 	for(size_t i=0; i<vertices.size()-1; i++)
// 	{
// 		minY = __min(vertices[i].y, minY);
// 		maxY = __max(vertices[i].y, maxY);
// 		polyLines[i] = Line(vertices[i], vertices[i+1]);
// 	}
// 	minY = __min(vertices.back().y, minY);
// 	maxY = __max(vertices.back().y, maxY);
// 	polyLines.back() = Line(vertices.back(), vertices.front());

// 	Sort::mergeSort<Line>(polyLines.data(), polyLines.size(), [](Line l1, Line l2) ->bool{
// 		return l1.getMinY() < l2.getMinY();
// 	});

// 	size_t indexInLines = 0;
// 	std::vector<Line> processingLines = std::vector<Line>();
// 	for(int y=minY; y<=maxY; y++)
// 	{
// 		for(; indexInLines < polyLines.size(); indexInLines++)
// 		{
// 			if(y >= polyLines[indexInLines].getMinY())
// 			{
// 				processingLines.push_back(polyLines[indexInLines]);
// 			}
// 			else
// 				break;
// 		}

// 		size_t i = 0;

// 		std::vector<CriticalPointOrLine> criticalPoints;
// 		while(i < processingLines.size())
// 		{
// 			if(y > processingLines[i].getMaxY())
// 			{
// 				std::swap(processingLines[i], processingLines.back());
// 				processingLines.pop_back();
// 			}
// 			else
// 			{
// 				//process line
// 				if(processingLines[i].getSlope() != 0)
// 				{
// 					criticalPoints.push_back({false, processingLines[i].solveForX(y), 0});
// 				}
// 				else
// 				{
// 					criticalPoints.push_back({true, processingLines[i].getMinX(), processingLines[i].getMaxX()});
// 				}
				
// 				i++;
// 			}
// 		}

// 		Sort::mergeSort<CriticalPointOrLine>(criticalPoints.data(), criticalPoints.size());
// 		bool skip = false;
// 		StringTools::print("%d: \n\t", y);
// 		for(size_t j=0; j<criticalPoints.size(); j++)
// 		{
// 			CriticalPointOrLine critPoint = criticalPoints[j];
// 			StringTools::print("{%d, %.3f, %.3f}, ", critPoint.isHorizontalLine, critPoint.p1, critPoint.p2);
// 		}

// 		StringTools::print(" || ");

// 		for(size_t j=0; j<criticalPoints.size(); j++)
// 		{
// 			CriticalPointOrLine critPoint = criticalPoints[j];
// 			if(critPoint.isHorizontalLine && (j % 2) == 1)
// 			{
// 				//if last point is on an odd index, duplicate previous point (minX of this line). Otherwise, skip
// 				if(criticalPoints[j+1].p1 != critPoint.p1)
// 				{
// 					StringTools::print("%.3f, ", critPoint.p2);
// 				}
// 				continue;
// 			}
// 			StringTools::print("%.3f, ", critPoint.p1);
// 		}

// 		StringTools::println("");
// 	}
// }

// void polygonFillTest3(const std::vector<Vec2f>& vertices)
// {
// 	//Line sweep based approach. Sort lines by min y value, add them to list of possible lines when appropriate. Remove lines that are no longer needed.
	
// 	//create all lines needed
// 	//Sort lines by min y value (std::vector<Line>) O(NLogN)
// 	//for each scanline, add line from the list of sorted lines into a temporary data structure that is used for calculations. (also std::vector<Line>) O(1) Amortized
// 	//remove all scanlines that are no longer needed. (as you come across them, move them to the end of the list and then pop back. O(1) operation ) O(1) Amortized
// 	//
	
// 	struct CriticalPointOrLine
// 	{
// 		bool isHorizontalLine = false;
// 		double p1;
// 		double p2;
// 		bool operator<(const CriticalPointOrLine& other)
// 		{
// 			if(p1 == other.p1)
// 				return other.isHorizontalLine;
// 			return p1 < other.p1;
// 		}
// 		bool operator==(const CriticalPointOrLine& other)
// 		{
// 			return p1 == other.p1 && p2 == other.p2 && isHorizontalLine == other.isHorizontalLine;
// 		}
// 	};

	

// 	std::vector<Line> polyLines = std::vector<Line>(vertices.size());
// 	float minY = vertices.front().y;
// 	float maxY = vertices.front().y;
// 	float minX = vertices.front().x;
// 	float maxX = vertices.front().x;
// 	for(size_t i=0; i<vertices.size()-1; i++)
// 	{
// 		minY = __min(vertices[i].y, minY);
// 		maxY = __max(vertices[i].y, maxY);
// 		minX = __min(vertices[i].x, minX);
// 		maxX = __max(vertices[i].x, maxX);
// 		polyLines[i] = Line(vertices[i], vertices[i+1]);
// 	}
// 	minY = __min(vertices.back().y, minY);
// 	maxY = __max(vertices.back().y, maxY);
// 	minX = __min(vertices.back().x, minX);
// 	maxX = __max(vertices.back().x, maxX);
// 	polyLines.back() = Line(vertices.back(), vertices.front());

// 	Sort::mergeSort<Line>(polyLines.data(), polyLines.size(), [](Line l1, Line l2) ->bool{
// 		return l1.getMinY() < l2.getMinY();
// 	});

// 	Image outputImage = Image((int)(maxX-minX)+1, (int)(maxY-minY)+1);
// 	size_t indexInLines = 0;
// 	std::vector<Line> processingLines = std::vector<Line>();
// 	for(int y=(int)minY; y<=(int)maxY; y++)
// 	{
// 		for(; indexInLines < polyLines.size(); indexInLines++)
// 		{
// 			if(y >= polyLines[indexInLines].getMinY())
// 			{
// 				processingLines.push_back(polyLines[indexInLines]);
// 			}
// 			else
// 				break;
// 		}

// 		size_t i = 0;

// 		std::vector<CriticalPointOrLine> criticalPoints;
// 		while(i < processingLines.size())
// 		{
// 			if(y > processingLines[i].getMaxY())
// 			{
// 				std::swap(processingLines[i], processingLines.back());
// 				processingLines.pop_back();
// 			}
// 			else
// 			{
// 				//process line
// 				if(processingLines[i].getSlope() != 0)
// 				{
// 					criticalPoints.push_back({false, processingLines[i].solveForX(y), 0});
// 				}
// 				else
// 				{
// 					criticalPoints.push_back({true, processingLines[i].getMinX(), processingLines[i].getMaxX()});
// 				}
// 				i++;
// 			}
// 		}

// 		Sort::mergeSort<CriticalPointOrLine>(criticalPoints.data(), criticalPoints.size());
// 		std::vector<double> finalPairs;
// 		// bool skip = false;
// 		// StringTools::print("%d: \n\t", y);
// 		// for(size_t j=0; j<criticalPoints.size(); j++)
// 		// {
// 		//     CriticalPointOrLine critPoint = criticalPoints[j];
// 		//     StringTools::print("{%d, %.3f, %.3f}, ", critPoint.isHorizontalLine, critPoint.p1, critPoint.p2);
// 		// }

// 		// StringTools::print(" || ");

// 		for(size_t j=0; j<criticalPoints.size(); j++)
// 		{
// 			CriticalPointOrLine critPoint = criticalPoints[j];
// 			if(critPoint.isHorizontalLine && (j % 2) == 1)
// 			{
// 				//if last point is on an odd index, duplicate previous point (minX of this line). Otherwise, skip
// 				if(criticalPoints[j+1].p1 != critPoint.p1)
// 				{
// 					// StringTools::print("%.3f, ", critPoint.p2);
// 					finalPairs.push_back(critPoint.p2);
// 				}
// 				continue;
// 			}
// 			// StringTools::print("%.3f, ", critPoint.p1);
// 			finalPairs.push_back(critPoint.p1);
// 		}

// 		StringTools::print("%d:\n\t", y);
// 		for(size_t j=1; j<finalPairs.size(); j+=2)
// 		{
// 			int startX = MathExt::floor(finalPairs[j-1] - minX);
// 			int endX = MathExt::ceil(finalPairs[j] - minX);
			
// 			StringTools::print("(%d, %d) - ", startX, endX);
// 			SimpleGraphics::fillBetween(Color{255,0,0,255}, startX, endX, y-minY, &outputImage);
// 		}
// 		StringTools::println("");
// 	}
// 	outputImage.savePNG("TestFill3.png");
// }

// #include "DCEL.h"
// void testDCEL()
// {
// 	DCEL test = DCEL();
// 	test.addPolygon({
// 		Vec2f(0, 0),
// 		Vec2f(32, 32),
// 		Vec2f(64, 32)
// 	});

// 	StringTools::println("\n______VERTICES______\n");
// 	//for each Vertex, print its MemoryLocation and position
// 	for(size_t index=0; index < test.getTotalVertices(); index++)
// 	{
// 		StringTools::println("%p: %.3f", test.getVertex(index), test.getVertex(index)->position);
// 	}
	
// 	StringTools::println("\n______HALF EDGES______\n");
// 	//for each Half Edge, print its MemoryLocation and all the pointers
// 	for(size_t index=0; index < test.getTotalEdges(); index++)
// 	{
// 		DCELEdge* halfEdge = test.getEdge(index);
// 		StringTools::println("%p: ", halfEdge);
// 		StringTools::println("\tOrigin: %p\n\tTwin Edge: %p\n\tIncident Face: %p\n\tNext Edge: %p\n\tPrevious Edge: %p", halfEdge->origin, halfEdge->twin, halfEdge->incidentFace, halfEdge->next, halfEdge->previous);
// 	}

// 	StringTools::println("\n______FACES______\n");
// 	//for each Face, print its MemoryLocation and all the pointers
// 	for(size_t index=0; index < test.getTotalFaces(); index++)
// 	{
// 		DCELFace* face = test.getFace(index);
// 		StringTools::println("%p: ", face);
// 		StringTools::println("\tOuter Component: %p\n\tInner Components: %p\n\tClockwise: %d", face->outerComponent, face->innerComponents, face->windingOrder);
// 	}
// }

// void quickDCELVertexStuff()
// {
// 	DCEL test = DCEL();
// 	DCELVertex* v1 = test.addVertex(Vec2f(0, 0));
// 	DCELVertex* v2 = test.addVertex(Vec2f(32, 0));
// 	DCELVertex* v3 = test.addVertex(Vec2f(32, 32));
// 	test.addEdge(v1, v2);
// 	test.addEdge(v2, v3);
// 	test.addEdge(v3, v1);

// 	StringTools::println("BEFORE VERTEX INTERSECTION");
// 	StringTools::println("\n______VERTICES______\n");
// 	//for each Vertex, print its MemoryLocation and position
// 	for(size_t index=0; index < test.getTotalVertices(); index++)
// 	{
// 		StringTools::println("%p: %.3f", test.getVertex(index), test.getVertex(index)->position);
// 	}
	
// 	StringTools::println("\n______HALF EDGES______\n");
// 	//for each Half Edge, print its MemoryLocation and all the pointers
// 	for(size_t index=0; index < test.getTotalEdges(); index++)
// 	{
// 		DCELEdge* halfEdge = test.getEdge(index);
// 		StringTools::println("%p: ", halfEdge);
// 		StringTools::println("\tOrigin: %p\n\tTwin Edge: %p\n\tIncident Face: %p\n\tNext Edge: %p\n\tPrevious Edge: %p", halfEdge->origin, halfEdge->twin, halfEdge->incidentFace, halfEdge->next, halfEdge->previous);
// 	}

// 	StringTools::println("\n______FACES______\n");
// 	//for each Face, print its MemoryLocation and all the pointers
// 	for(size_t index=0; index < test.getTotalFaces(); index++)
// 	{
// 		DCELFace* face = test.getFace(index);
// 		StringTools::println("%p: ", face);
// 		StringTools::println("\tOuter Component: %p\n\tInner Components: %p\n\tClockwise: %d", face->outerComponent, face->innerComponents, face->windingOrder);
// 	}
	
// 	test.addVertex(Vec2f(16, 0));
// 	StringTools::println("AFTER VERTEX INTERSECTION");
// 	StringTools::println("\n______VERTICES______\n");
// 	//for each Vertex, print its MemoryLocation and position
// 	for(size_t index=0; index < test.getTotalVertices(); index++)
// 	{
// 		StringTools::println("%p: %.3f", test.getVertex(index), test.getVertex(index)->position);
// 	}
	
// 	StringTools::println("\n______HALF EDGES______\n");
// 	//for each Half Edge, print its MemoryLocation and all the pointers
// 	for(size_t index=0; index < test.getTotalEdges(); index++)
// 	{
// 		DCELEdge* halfEdge = test.getEdge(index);
// 		StringTools::println("%p: ", halfEdge);
// 		StringTools::println("\tOrigin: %p\n\tTwin Edge: %p\n\tIncident Face: %p\n\tNext Edge: %p\n\tPrevious Edge: %p", halfEdge->origin, halfEdge->twin, halfEdge->incidentFace, halfEdge->next, halfEdge->previous);
// 	}

// 	StringTools::println("\n______FACES______\n");
// 	//for each Face, print its MemoryLocation and all the pointers
// 	for(size_t index=0; index < test.getTotalFaces(); index++)
// 	{
// 		DCELFace* face = test.getFace(index);
// 		StringTools::println("%p: ", face);
// 		StringTools::println("\tOuter Component: %p\n\tInner Components: %p\n\tClockwise: %d", face->outerComponent, face->innerComponents, face->windingOrder);
// 	}
// }

// int main(int argc, char** argv)
// {
// 	// taskLatency();
// 	// // std::vector<Vec2f> vertices = {Vec2f(-49.1, 96.2), Vec2f(4.4, 37.3), Vec2f(12.6, 102)};
// 	// // std::vector<Vec2f> vertices = {Vec2f(100,200), Vec2f(300,200), Vec2f(275,150), Vec2f(200,150), Vec2f(150,100)};
// 	// // std::vector<Vec2f> vertices = {Vec2f(150,100), Vec2f(200,150), Vec2f(275,150), Vec2f(300,200), Vec2f(100,200)};
// 	// std::vector<Vec2f> vertices = {Vec2f(-7.5, 0), Vec2f(7.5, 0), Vec2f(-5, -10), Vec2f(0, 5), Vec2f(5, -10)};
// 	// // std::vector<Vec2f> vertices = {Vec2f(5, -10), Vec2f(0, 5), Vec2f(-5, -10), Vec2f(7.5, 0), Vec2f(-7.5, 0)};

// 	// // polygonFillTest(vertices);
// 	// // polygonFillTest2(vertices);
// 	// // polygonFillTest3(vertices);
// 	// Image outputImage = Image(100, 100);
// 	// SimpleGraphics::setColor({255,0,0,255});
// 	// SimpleGraphics::setFillRule(SimpleGraphics::FILL_NON_ZERO);
// 	// SimpleGraphics::drawPolygon(vertices.data(), vertices.size(), &outputImage);
// 	// outputImage.savePNG("ProperPolyTest.png");

// 	size_t t1, t2;
// 	// //vary between 0, 255
// 	Image img = Image(4096, 4096);
// 	// FlatFillFunction baseFillFunction({255, 160, 100, 255});

// 	// t1 = System::getCurrentTimeMicro();
// 	// for(int y=0; y<256; y++)
// 	// {
// 	// 	baseFillFunction.fillBetween(0, 256, y, &img);
// 	// }
// 	// t2 = System::getCurrentTimeMicro();
// 	// StringTools::println("Time For Flat Fill: %llu", t2-t1);
// 	// img.savePNG("DefaultFillFunction.png", false, false, false);

	
// 	LinearGradientFillFunction linGradFillFunction(Vec2f(0, 0), Vec2f(256, 256), {{0, Color{0, 0, 0, 255}}, {0.2, Color{255, 255, 0, 255}}, {0.8, Color{0, 0, 255, 255}}, {1, Color{255, 0, 0, 255}}});

// 	// t1 = System::getCurrentTimeMicro();
// 	// for(int y=0; y<256; y++)
// 	// {
// 	// 	linGradFillFunction.fillBetween(0, 256, y, &img);
// 	// }
// 	// t2 = System::getCurrentTimeMicro();
// 	// StringTools::println("Time For Linear Gradient Fill: %llu", t2-t1);
// 	// img.savePNG("LinearFillFunction.png", false, false, false);

// 	// RadialGradientFillFunction radGradFillFunction(Vec2f(128, 128), {{16, Color{0, 0, 0, 255}}, {32, Color{255, 255, 0, 255}}, {64, Color{0, 0, 255, 255}}, {96, Color{255, 0, 0, 255}}});
// 	RadialGradientFillFunction radGradFillFunction(Vec2f(2048, 2048), {{2046, Color{255, 255, 255, 255}}, {2047, Color{0, 0, 0, 0}}});

// 	Image maskImage = Image(4096, 4096);
// 	maskImage.setAllPixels({255, 255, 255, 255});
	
// 	// SimpleGraphics::setColor({255, 255, 255, 0});
// 	// SimpleGraphics::setCompositeRule(SimpleGraphics::NO_COMPOSITE);
// 	// SimpleGraphics::drawRect(2048-64, 2048-64, 128, 128, false, &maskImage);
// 	// SimpleGraphics::setCompositeRule(SimpleGraphics::COMPOSITE_SRC_OVER);

// 	SimpleGraphics::setMask(maskImage);

// 		Image tempImg = Image(256, 256);
// 		Image tempImg2 = Image(256, 256);
// 		tempImg.setAllPixels({0, 0, 0, 0});
// 		tempImg2.setAllPixels({0, 0, 0, 0});
// 	t1 = System::getCurrentTimeMicro();
// 	for(int i=0; i<100; i++)
// 	{
// 		SJQ_OMP_Replacement::parallelize([&img, &tempImg, &tempImg2, &radGradFillFunction, &linGradFillFunction](size_t tStart, size_t tEnd, size_t tIncr)->void{
// 			for(int y=tStart; y<tEnd; y+=tIncr)
// 			{
// 				radGradFillFunction.fillBetween(0, 4096, y, &img);
// 				// linGradFillFunction.fillBetween(0, 256, y, &tempImg2);
// 				// for(int x=0; x<256; x+=1)
// 				// {
// 				// 	// SIMD_U32 mult = SIMD_U32::load((uint32_t*)&tempImg.getPixels()[x + y*tempImg.getWidth()]);
// 				// 	// SIMD_U32 c = SIMD_U32::load((uint32_t*)&tempImg2.getPixels()[x + y*tempImg2.getWidth()]);
// 				// 	// SIMD_U32 outputColor = SimpleGraphics::multColor(c.values, mult.values);
// 				// 	// SIMD_U32 destColor = SIMD_U32::load((uint32_t*)&img.getPixels()[x + y*img.getWidth()]);
// 				// 	// outputColor = SimpleGraphics::blend(outputColor.values, destColor.values);
// 				// 	// outputColor.store((uint32_t*)&img.getPixels()[x + y*img.getWidth()]);
// 				// 	Color mult = tempImg.getPixels()[x + y*img.getWidth()];
// 				// 	Color c = tempImg2.getPixels()[x + y*img.getWidth()];
// 				// 	SimpleGraphics::drawPixel(x, y, SimpleGraphics::multColor(c, mult), &img);
// 				// }
// 			}
// 		}, 0, 4096, 1, 24);
// 	}
// 	// for(int y=0; y<256; y++)
// 	// {
// 	// 	radGradFillFunction.fillBetween(0, 256, y, &img);
// 	// }
// 	t2 = System::getCurrentTimeMicro();
// 	StringTools::println("Time For Radial Gradient Fill: %llu", (t2-t1)/100);
// 	img.savePNG("RadialFillFunction.png", false, false, false);

	
// 	Image img2 = Image(4096, 4096);
// 	// t1 = System::getCurrentTimeMicro();
// 	// SimpleGraphics::setColor({0, 0, 0, 255});
// 	// for(int i=0; i<1; i++)
// 	// {
// 	// 	SimpleGraphics::drawRect(0, 0, 4096, 4096, false, &img2);
// 	// }
// 	// t2 = System::getCurrentTimeMicro();
// 	// StringTools::println("Time For Draw Rect: %llu", t2-t1);

// 	/*
// 		30888	- single threaded
// 		7520	- OMP
// 		3668	- SimpleJobQueue
// 	*/
	
// 	SimpleGraphics::setAntiAliasing(true);
// 	t1 = System::getCurrentTimeMicro();
// 	SimpleGraphics::setColor({255, 255, 255, 255});
	
// 	for(int i=0; i<100; i++)
// 	{
// 		// SimpleGraphics::drawCircle(2048, 2048, 2047, false, &img2);
// 		SimpleGraphics::drawCircle(2048, 2048, 2047, 2046, &img2);
// 	}
// 	t2 = System::getCurrentTimeMicro();
// 	StringTools::println("Time For Draw Circle: %llu", (t2-t1)/100);
// 	img2.savePNG("circleDraw.png", false, false, false);
	
// 	// t1 = System::getCurrentTimeMicro();
// 	// SimpleGraphics::setColor({255, 255, 0, 255});
	
// 	// for(int i=0; i<1000; i++)
// 	// {
// 	// 	SimpleGraphics::drawCircle(128, 128, 96, 16, &img2);
// 	// }
// 	// t2 = System::getCurrentTimeMicro();
// 	// StringTools::println("Time For Draw Circle2: %llu", (t2-t1)/1000);
	

// 	// testDCEL();
// 	// quickDCELVertexStuff();
// 	return 0;
// }

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

using namespace smpl;

// smpl::SimpleJSON setupOAuth(SimpleJSON clientSecretJSON, std::string scopeURL, int type)
// {
//     smpl::SimpleJSON outputJSON;
//     std::string clientID = ((smpl::JPair&)clientSecretJSON["installed"]["client_id"]).getValue();
//     std::string clientSecret = ((smpl::JPair&)clientSecretJSON["installed"]["client_secret"]).getValue();
//     std::string authURL = ((smpl::JPair&)clientSecretJSON["installed"]["auth_uri"]).getValue();
//     std::string tokenURL = ((smpl::JPair&)clientSecretJSON["installed"]["token_uri"]).getValue();
//     std::string redirect_uri = "urn:ietf:wg:oauth:2.0:oob"; //if type is manual code auth

//     std::string finalURL = authURL + "?client_id=" + clientID
//                      + "&response_type=code&scope=" + scopeURL
//                      + "&access_type=offline&redirect_uri=";
//     std::string finalAuthCode = "";
	
//     if(type == 0)
//     {
//         NetworkConfig config;
//         config.amountOfConnectionsAllowed = 4;
//         config.location = "127.0.0.1";
//         config.port = 0;
//         config.secure = false;
//         config.TCP = true;
//         config.type = Network::TYPE_SERVER;

//         std::mutex quickMut;
//         std::string responseURL = "";

//         std::string indexHTML = "<!DOCTYPE html><html><body><h1>Safe to close now</h1></body></html>";
//         HttpServer server = HttpServer(config, 1);
//         server.setGetFuncMapper([&indexHTML, &responseURL, &quickMut](HttpServer* servP, const WebRequest& req, const std::vector<unsigned char>& body, std::string location, size_t id) -> char{
			
//             // url should contain the response code needed
//             // DO NOT LET THE USER GO TO THE LOCAL HOST AS IT WILL MESS UP THIS PROCESS. SERVER WILL CLOSE RIGHT AFTER DOING THIS
//             quickMut.lock();
//             responseURL = req.getUrl();
//             quickMut.unlock();

//             //send html response back. Doesn't matter if they asked for index.html or anything. Always send this. Make it look pretty one day
//             WebRequest response;
//             response.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 200 OK", false);
//             response.addKeyValue("Content-Type", WebRequest::getMimeTypeFromExt("html"));
//             response.addKeyValue("Content-Length", StringTools::toString(indexHTML.size()));
			
//             servP->sendResponse(response, location, id, req);
//             servP->sendMessage(indexHTML.c_str(), indexHTML.size(), id);
			
//             return HttpServer::STATUS_DONE;
//         });

//         server.start();
//         int actualPort = server.getNetworkConnection()->getPort();
		
//         finalURL += "http://127.0.0.1:" + StringTools::toString(actualPort);
//         system(("start msedge \"" + finalURL + "\"").c_str());

//         while(server.getRunning())
//         {
//             System::sleep(10, 0, false);

//             quickMut.lock();
//             bool canQuit = !responseURL.empty();
//             quickMut.unlock();

//             if(canQuit)
//                 break;
//         }

//         //parse URL
		
//     }
//     else
//     {
//         finalURL += "urn:ietf:wg:oauth:2.0:oob";
//         system(("start msedge \"" + finalURL + "\"").c_str());

//         smpl::StringTools::print("ENTER AUTH CODE: ");
//         finalAuthCode = smpl::StringTools::getString();
//     }
// }

// smpl::SimpleJSON setupOAUTH(smpl::File clientSecretJSON, std::string scopeURL)
// {
//     smpl::SimpleJSON outputJSON;

//     smpl::SimpleJSON clientInfo = smpl::SimpleJSON(clientSecretJSON.getFullFileName());
//     std::string clientID = ((smpl::JPair&)clientInfo["installed"]["client_id"]).getValue();
//     std::string clientSecret = ((smpl::JPair&)clientInfo["installed"]["client_secret"]).getValue();
//     std::string authURL = ((smpl::JPair&)clientInfo["installed"]["auth_uri"]).getValue();
//     std::string tokenURL = "https://www.googleapis.com/oauth2/v4/token";
	
//     std::string finalURL = authURL + "?client_id=" + clientID
//                      + "&response_type=code&scope=" + scopeURL
//                      + "&access_type=offline&redirect_uri=urn:ietf:wg:oauth:2.0:oob";
	
//     system(("start msedge \"" + finalURL + "\"").c_str());

//     smpl::StringTools::print("ENTER AUTH CODE: ");
//     std::string finalAuthCode = smpl::StringTools::getString();
	
//     if(finalAuthCode.empty())
//         return outputJSON;
	
//     std::string body = "client_id="+clientID+"&client_secret="+clientSecret+"&code="+finalAuthCode
//                     + "&redirect_uri=urn:ietf:wg:oauth:2.0:oob&grant_type=authorization_code";

//     smpl::HttpClient client = smpl::HttpClient(tokenURL);
//     smpl::WebRequest requestAuthHeaders = smpl::WebRequest();
//     requestAuthHeaders.setHeader(smpl::WebRequest::TYPE_POST, "/oauth2/v4/token");
//     requestAuthHeaders.addKeyValue("Host", "www.googleapis.com");
//     requestAuthHeaders.addKeyValue("User-Agent", "TestApplication1");
//     requestAuthHeaders.addKeyValue("Content-Length", smpl::StringTools::toString(body.size()));
//     requestAuthHeaders.addKeyValue("Content-Type", "application/x-www-form-urlencoded");

//     std::atomic_bool isConnected = true;
//     client.setOnConnectFunc([&requestAuthHeaders, &body](smpl::HttpClient* clientPointer) -> void{
//         int bytesSent = clientPointer->sendRequest(requestAuthHeaders);
//         smpl::StringTools::println("BYTES SENT: %d vs EXPECTED: %llu", bytesSent, requestAuthHeaders.getBytesInRequest());
//         bytesSent = clientPointer->sendMessage(body.c_str(), body.size());
//         smpl::StringTools::println("BYTES SENT: %d vs EXPECTED: %llu", bytesSent, body.size());
//         smpl::StringTools::println("CONNECTED");
//     });
	
//     client.setOnBufferChangedFunc([&outputJSON, &isConnected](smpl::HttpClient* clientPointer, smpl::WebRequest& request, unsigned char* buffer, size_t bufferSize) ->void {
//         //should be a json. Load as one. Should disconnect right after too
//         smpl::StringTools::println("DATA RECEIVED");
//         smpl::StreamableArray<unsigned char> streamableBuffer(buffer, bufferSize, false);
//         outputJSON = smpl::SimpleJSON(&streamableBuffer);
//         clientPointer->disconnect();
//         isConnected = false;
//     });

//     client.setOnDisconnectFunc([&isConnected](smpl::HttpClient* clientPointer) ->void{
//         isConnected = false;
//         smpl::StringTools::println("DISCONNECTED");
//     });
//     client.start();

//     if(client.getTimeoutOccurred())
//     {
//         smpl::StringTools::println("FAILED TO CONNECT");
//         isConnected = false;
//     }

//     while(client.isValid() && isConnected)
//     {
//         if(client.getShouldRedirect())
//             client.completeRedirect();
//         smpl::System::sleep(10, 0, false);
//     }

//     return outputJSON;
// }

// void createOAuthServer(smpl::File clientSecretJSON, std::string scopeURL)
// {


//     NetworkConfig config;
//     config.amountOfConnectionsAllowed = 4;
//     config.location = "127.0.0.1";
//     config.port = 0;
//     config.secure = false;
//     config.TCP = true;
//     config.type = Network::TYPE_SERVER;

//     std::string indexHTML = "<!DOCTYPE html><html><body><p>Safe to close now</p></body></html>";
//     HttpServer server = HttpServer(config, 2);
//     server.setGetFuncMapper([&indexHTML](HttpServer* servP, const WebRequest& req, const std::vector<unsigned char>& body, std::string location, size_t id) -> char{
//         StringTools::println(req.getUrl());
//         WebRequest response;
//         response.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 200 OK", false);
//         response.addKeyValue("Content-Type", WebRequest::getMimeTypeFromExt("html"));
//         response.addKeyValue("Content-Length", StringTools::toString(indexHTML.size()));
		
//         servP->sendResponse(response, location, id, req);
// 		servP->staggeredSend(id, indexHTML.c_str(), 0, indexHTML.size()-1);
//         servP->sendMessage(indexHTML.c_str(), indexHTML.size(), id);
		
//         return HttpServer::STATUS_DONE;
//     });

//     server.start();
//     int actualPort = server.getNetworkConnection()->getPort();

//     StringTools::println("Server on: http://127.0.0.1:%d", actualPort);

//     smpl::SimpleJSON clientInfo = smpl::SimpleJSON(clientSecretJSON.getFullFileName());
//     std::string clientID = ((smpl::JPair&)clientInfo["installed"]["client_id"]).getValue();
//     std::string clientSecret = ((smpl::JPair&)clientInfo["installed"]["client_secret"]).getValue();
//     std::string authURL = ((smpl::JPair&)clientInfo["installed"]["auth_uri"]).getValue();
//     std::string tokenURL = "https://www.googleapis.com/oauth2/v4/token";
	
//     std::string finalURL = authURL + "?client_id=" + clientID
//                      + "&response_type=code&scope=" + scopeURL
//                      + "&access_type=offline&redirect_uri=http://127.0.0.1:" + StringTools::toString(actualPort);
	
//     StringTools::println(finalURL);
	
//     system(("start msedge \"" + finalURL + "\"").c_str());

//     while(server.getRunning())
//     {
//         System::sleep(10, 0, false);
//     }
// }

#include "Input.h"
std::atomic_bool running = {true};
void interruptFunc()
{
	running = false;
}

void runBasicHTTPServer()
{
	timeBeginPeriod(1);
	size_t t1 = System::getCurrentTimeMicro();
	NetworkConfig config;
	config.amountOfConnectionsAllowed = 0xFFFF;
	config.location = "127.0.0.1";
	config.port = 8080;
	config.secure = false;
	config.type = Network::TYPE_SERVER;
	config.TCP = true;

	HttpServer server = HttpServer(config, 12);
	System::mapInteruptSignal(interruptFunc);
	// server.setLogInfo(true);
	// server.setAllowPersistentConnection(false);
	server.start();

	StringTools::println("http://127.0.0.1:8080");
	while(running)
	{
		System::sleep(1, 0, true);
	}
	size_t t2 = System::getCurrentTimeMicro();
	StringTools::println("Time elapsed: %llu", t2-t1);
	timeEndPeriod(10);
}

float findMinDistance(const BezierCurve& c1, const BezierCurve& c2, float circleRad)
{
	//compute f(t) - g(t) = c1-c2 which should be a direct subtraction of points if they are the same points
	//make my life simple. Convert into equation
	//(1-t)^2 * P1 + 2(1-t)t * P2 + t^2 * P3
	//(1 -2t + t^2)P1 + (2t - 2t^2)P2 + t^2*P3
	const Vec2f* c1Points = c1.getPoints().data();
	const Vec2f* c2Points = c2.getPoints().data();

	Vec2f A1 = c1Points[2] -2*c1Points[1] + c1Points[0];
	Vec2f B1 = 2*(c1Points[1] - c1Points[0]);
	Vec2f C1 = c1Points[0];

	Vec2f A2 = c2Points[2] -2*c2Points[1] + c2Points[0];
	Vec2f B2 = 2*(c2Points[1] - c2Points[0]);
	Vec2f C2 = c2Points[0];

	Vec2f finalA = A1-A2;
	Vec2f finalB = B1-B2;
	Vec2f finalC = C1-C2;

	//f(t) = At^2 + Bt + C
	//f`(t) = 2At + B
	//sqrt(f(t).x^2 + f(t).y^2) = r || f(t).x^2 + f(t).y^2 = r^2
	//2f(t).x*f`(t).x + 2f(t).y*f`(t).y = 0

	//2f(t)*f`(t)

	//This becomes a quartic equation if we limit to nothing more than quadratic bezier curves which is all we actually need in practice
	//V_{1}=A^{2}
	//V_{2}=2AB
	//V_{3}=2AC+B^{2}
	//V_{4}=C^{2}
	//where A, B, C are the finalA, finalB, and finalC points
	//it doesn't make a lot of sense to square or multiply these as they are points/vectors so treat these as dot products probably. Maybe hadamard products

	Vec2f quarticV1 = finalA.hadamardProduct(finalA);
	Vec2f quarticV2 = 2*finalA.hadamardProduct(finalB);
	Vec2f quarticV3 = 2*finalA.hadamardProduct(finalC) + finalB.hadamardProduct(finalB);
	Vec2f quarticV4 = finalC.hadamardProduct(finalC);
	float actualQ1 = quarticV1.x+quarticV1.y;
	float actualQ2 = quarticV2.x+quarticV2.y;
	float actualQ3 = quarticV3.x+quarticV3.y;
	float actualQ4 = quarticV4.x+quarticV4.y;
	
	// Vec2f finalADer = 2*finalA;
	// Vec2f finalBDer = finalB;

	// //newton's method
	// float currP = 0;
	// for(int i=0; i<4; i++)
	// {
	// 	Vec2f f = (finalA*(currP*currP)) + (finalB*currP) + finalC;
	// 	Vec2f fP = (2*finalADer*currP) + finalB;
	// 	float finalDis = f.dot(f) - (circleRad*circleRad);
	// 	float finalDisDer = 2*f.dot(fP);
	// 	currP -= finalDis/finalDisDer;
	// }

	// float derQ1 = 4*actualQ1;
	// float derQ2 = 3*actualQ2;
	// float derQ3 = 2*actualQ3;
	return actualQ1+actualQ2+actualQ3+actualQ4;

	// float circleRadSqr = circleRad*circleRad;
	// Vec2f FAVec = finalC;
	// Vec2f FBVec = finalA+finalB+finalC;
	// float fa = FAVec.dot(FAVec) - circleRadSqr;
	// float fb = FBVec.dot(FBVec) - circleRadSqr;

	// const int ITERATIONS = 10;
	// float a = 0;
	// float b = 1;
	// float c = 0;
	// int side = 0;
	// for(int i=0; i<ITERATIONS; i++)
	// {
	// 	c = (fa*b - fb*a) / (fa-fb);
	// 	if(abs(b-a) < EPSILON*abs(b+a))
	// 		break;
	// 	Vec2f FCVec = finalA*c*c + finalB*c + finalC;
	// 	float fc = FCVec.dot(FCVec) - circleRadSqr;
	// 	if(fc*fb > 0)
	// 	{
	// 		b = c; fb = fc;
	// 		if(side == -1)
	// 			fa /= 2;
	// 		side = -1;
	// 	}
	// 	else if(fa*fc > 0)
	// 	{
	// 		a = c; fa = fc;
	// 		if(side == 1)
	// 			fb /= 2;
	// 		side = 1;
	// 	}
	// 	else {
	// 		break;
	// 	}
	// }
	
	// return c;
}

void simulationTest()
{
	int width = 8;
	int height = 6;
	int wDivValue = 640/width;
	int hDivValue = 480/height;
	std::atomic_int collisionCounter = 0;
	std::vector<Circle> objects;
	std::vector<std::vector<int>> grid = std::vector<std::vector<int>>(width*height); //lazy
	while(true)
	{
		
		size_t t1 = System::getCurrentTimeMicro();
		//for each step, increment objects by 100
		for(int i=0; i<100; i++)
		{
			objects.push_back(Circle(1 + (rand() % 16)));
			objects.back().setPosition(Vec2f(rand() % 640, rand() % 480));
			// //calculate insert into the grids it will be in.
			// Circle& c = objects.back();
			// Vec2f pos = c.getPosition().toVec2f();

			// int minY = __max((int)(pos.y-c.getRadius()), 0);
			// int maxY = __min((int)(pos.y+c.getRadius()), 479);
			// int minX = __max((int)(pos.x-c.getRadius()), 0);
			// int maxX = __min((int)(pos.x+c.getRadius()), 639);

			// minY/=hDivValue;
			// maxY/=hDivValue;
			// minX/=wDivValue;
			// maxX/=wDivValue;

			// for(int y=minY; y < maxY; y++)
			// {
			// 	for(int x=minX; x < maxX; x++)
			// 	{
			// 		grid[x + y*width].push_back(objects.size());
			// 	}
			// }
		}

		//naive collision detection
		for(int step=0; step<8; step++)
		{
			collisionCounter = 0;
			for(size_t i=0; i<objects.size(); i++)
			{
				for(size_t j=i+1; j<objects.size(); j++)
				{
					CollisionMaster::collisionMethod(objects[i], objects[j]);
					collisionCounter++;
				}
			}
		}


		// //smarter collision
		// for(int step=0; step<8; step++)
		// {
		// 	// #pragma omp parallel for
		// 	for(size_t i=0; i<grid.size(); i++)
		// 	{
		// 		//check all objects in the list against each other
		// 		for(size_t j=0; j<grid[i].size(); j++)
		// 		{
		// 			for(size_t k=j+1; k<grid[i].size(); k++)
		// 			{
		// 				CollisionMaster::collisionMethod(objects[grid[i][j]], objects[grid[i][k]]);
		// 				collisionCounter++;
		// 			}
		// 		}
		// 	}
		// }
		size_t t2 = System::getCurrentTimeMicro();

		if(t2-t1 > 120000000000 || objects.size() > 200000 || collisionCounter >= 20000000) //8 milliseconds
		{
			StringTools::println("Threshold hit: %lluus at %llu objects", t2-t1, objects.size());
			StringTools::println("Collisions performed: %d", (int)collisionCounter);
			break;
		}
	}
}

void collisionStuff()
{
	BezierCurve c1 = BezierCurve({Vec2f(-10, 0), Vec2f(64, 0), Vec2f(64, 64)});
	BezierCurve c2 = BezierCurve({Vec2f(0, -20), Vec2f(0, 64), Vec2f(64, 64)});

	size_t TOTAL_OBJECTS = 6400;
	std::vector<BezierCurve> allObjectPaths;
	for(int i=0; i<TOTAL_OBJECTS; i++)
	{
		allObjectPaths.push_back(BezierCurve({
			Vec2f(rand()%100, rand()%100), Vec2f(rand()%100, rand()%100), Vec2f(rand()%100, rand()%100)
		}));
	}

	size_t t1 = System::getCurrentTimeNano();

	std::vector<Line> cachedLines = std::vector<Line>(TOTAL_OBJECTS*8);
	for(size_t index1=0; index1<allObjectPaths.size(); index1++)
	{
		BezierCurve& startP = allObjectPaths[index1];
		Vec2f previousPoint = startP.getPoint(0);
		for(int i=1; i<=8; i++)
		{
			Vec2f nextPoint = startP.getFunctionAt(i*(1.0/7.0));
			cachedLines[index1*8 + i-1] = Line(previousPoint, nextPoint);
			previousPoint = nextPoint;
		}
	}

	for(size_t index1=0; index1<allObjectPaths.size(); index1+=8)
	{
		Line* startOfCachedLines = &cachedLines[index1];
		for(size_t index2=index1+8; index2<allObjectPaths.size(); index2+=8)
		{
			Line* startOfCachedLines2 = &cachedLines[index2];
			double timePoint = 0;
			//naive slow approach
			for(int i=0; i<8; i++)
			{
				for(int j=0; j<8; j++)
				{
					// double timePoint = 0;
					startOfCachedLines[i].getIntersectionParametric(startOfCachedLines2[j], timePoint);
				}
			}
			
			// std::vector<double> points = c1.findIntersectionPoints(c2, 1);
			// findMinDistance(*startP, *newStartP, 4);
			// double t;
			// l1.getIntersectionParametric(l2, t);
			// l1.getIntersectionParametric(l2, t);
			// l1.getIntersectionParametric(l2, t);
			// l1.getIntersectionParametric(l2, t);
		}
	}
	size_t t2 = System::getCurrentTimeNano();
	StringTools::println("Time taken: %lluns", t2-t1);
	StringTools::println("Total Checks = %llu", TOTAL_OBJECTS*TOTAL_OBJECTS/2);

	// std::vector<double> points = c1.findIntersectionPoints(c2, 0.001);
	// StringTools::println("%.5f", points);

	
	// auto timeV = findMinDistance(c1, c2, 4);
	// StringTools::println("Time of minDis = %.9f", timeV);

	// simulationTest();
}

int main(int argc, char** argv)
{
    // smpl::SimpleJSON authStuff = setupOAUTH("client_secret.json", 
    //                                         "https://www.googleapis.com/auth/youtube");
	
    // // smpl::StringTools::println("%s", authStuff.getString(true));
    // createOAuthServer("client_secret.json", "https://www.googleapis.com/auth/youtube");

    // auto output = StringTools::parseURLArguments("/?code=4/0ATX87lOb7x-IZz7zUHd7I_kfdejcagXqNZV0KQkFhlXTbCuL9yErhU2GtIcyUYOX9N2icQ&scope=https://www.googleapis.com/auth/youtube");
    // StringTools::println("%s", output);
	
	// runBasicHTTPServer();
	
	// System::sleep(5000, 0, false);
	// collisionStuff();
	// UnionedJNode node = UnionedJNode(JPair(""));
	// UnionedJNode node2 = UnionedJNode(JObject(""));
	// UnionedJNode node3 = UnionedJNode(JArray(""));

	// printStuff((JNode&)node);
	// printStuff((JNode&)node2);
	// printStuff((JNode&)node3);

	// node3.getArray().push_back(JPair("N1", "V1"));
	// node3.getArray().push_back(JPair("N2", "V2"));
	// node3.getArray().push_back(JPair("N3", "V3"));
	// node3.getArray().push_back(JPair("N4", "V4"));

	// system("pause");
	// JArray arr = JArray("");
	// for(int i=0; i<16000; i++)
	// {
	// 	arr.push_back(JObject());
	// 	arr.back().getObject().insert(JPair("n1", "v1"));
	// 	arr.back().getObject().insert(JPair("n2", "v1"));
	// 	arr.back().getObject().insert(JPair("n3", "v1"));
	// 	arr.back().getObject().insert(JPair("n4", "v1"));
	// 	arr.back().getObject().insert(JPair("n5", "v1"));
	// 	arr.back().getObject().shrink();
	// }
	// arr.shrink();
	// system("pause");
	system("pause");
	SimpleJSON json = SimpleJSON((std::string)"largeJSON.json");
	system("pause");

	// sizeof(SimpleHashSet<JNode, JNodeHash, JNodeEquals>)
	
    return 0;
}

//client_id: Kd4Ue8jYiWhocNK1eSuE1WZ7
//client_secret: lPktggkJ7GIt8Jg9wexta1506Pyv22Z3uAXgbRAb590G1W7q
//login: bewildered-skylark@example.com
//password: Powerful-Partridge-56


/*
Destroying
TotalJobs: 7010
TimeBetweenJobs Accumulated: 10002116
TimeBetweenJobs Average: 1426
ThreadID = 2
        Jobs Executed: 2291
        Time Waiting since start: 13023378us
        Time Waiting: 8514675us
        Time Getting Job: 7906us
        Time Post Processing Job: 1012us
ThreadID = 1
        Jobs Executed: 2274
        Time Waiting since start: 13022144us
        Time Waiting: 8513441us
        Time Getting Job: 8307us
        Time Post Processing Job: 1203us
ThreadID = 0
        Jobs Executed: 2314
        Time Waiting since start: 13010186us
        Time Waiting: 8501483us
        Time Getting Job: 8913us
        Time Post Processing Job: 1239us
ThreadID = 3
        Jobs Executed: 2239
        Time Waiting since start: 13038605us
        Time Waiting: 8529902us
        Time Getting Job: 9016us
        Time Post Processing Job: 1049us
threads joined
*/

/*
Destroying
TotalJobs: 8386
TimeBetweenJobs Accumulated: 10003633
TimeBetweenJobs Average: 1192
ThreadID = 0
        Jobs Executed: 8764
        Time Waiting since start: 10912913us
        Time Waiting: 8073929us
        Time Getting Job: 18048us
        Time Post Processing Job: 2666us
threads joined
*/