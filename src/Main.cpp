// #include "System.h"
// #include "WebSocket.h"
// #include <unordered_set>
// #include "ext/KeyboardSimulator.h"
// #include "Input.h"

// using namespace smpl;
// std::atomic_bool running = {true};
// std::mutex lock;

// //send random number every 100 ms
// std::unordered_set<size_t> clientsConnected;

// void interruptClose()
// {
//     running = false;
// }

// void sendToEveryone(WebSocket& ws)
// {
//     std::string randNumberAsString = StringTools::toString(rand());
//     lock.lock();
//     for(size_t id : clientsConnected)
//     {
//         StringTools::println("SEND TO ID = %llu", id);
//         ws.sendText(randNumberAsString, id);
//     }
//     lock.unlock();
// }
// void runSocketServer()
// {
//     srand(time(nullptr));
//     WebSocket ws = WebSocket(WebSocket::TYPE_SERVER, 4040, "0.0.0.0");
//     ws.setOnConnectFunction([](size_t id) ->void{
//         StringTools::println("CONNECTED TO : %llu", id);
//         lock.lock();
//         clientsConnected.insert(id);
//         lock.unlock();
//     });
//     ws.setOnDisconnectFunction([](size_t id) ->void{
//         StringTools::println("DISCONNECTED FROM : %llu", id);
//         lock.lock();
//         clientsConnected.erase(id);
//         lock.unlock();
//     });
//     ws.startNetwork();

//     System::mapInteruptSignal(interruptClose);
//     size_t lastTime = System::getCurrentTimeMillis();
//     while(running)
//     {
//         if(System::getCurrentTimeMillis() - lastTime > 500)
//         {
//             // StringTools::println("STUFF");
//             lastTime = System::getCurrentTimeMillis();
//             sendToEveryone(ws);
//         }
//         System::sleep(1, 0, false);
//     }
// }

// #include "GamepadXInput.h"
// std::atomic_bool canUse = {false};
// void testXInputEmulator()
// {
//     GamepadXInput::init();
//     NetworkConfig conf;
//     conf.amountOfConnectionsAllowed = 4;
//     conf.location = "127.0.0.1";
//     conf.port = 4040; //will need to load this from a file or something. Just a test for now
//     conf.TCP = true;
//     conf.type = Network::TYPE_CLIENT;

//     Network client = Network(conf);

//     client.setOnConnectFunction([](std::string ip, size_t id) ->void{
//         StringTools::println("CONNECTED");
//         canUse = true;
//     });
//     client.setOnDataAvailableFunction([](std::string ip, size_t id) ->void{
//         StringTools::println("SHOULDN'T GET ANY DATA. PROBLEM");
//     });
//     client.setOnDisconnectFunction([](std::string ip, size_t id) ->void{
//         StringTools::println("DISCONNECTED");
//         canUse = false;
//     });

//     client.startNetwork();

//     StringTools::println("STARTING");
//     while(true)
//     {
//         Input::pollInput();
//         GamepadXInput::update();

//         if(Input::getKeyPressed(Input::KEY_ESCAPE))
//             break;
        
//         if(GamepadXInput::isConnected(0) && canUse)
//         {
//             // StringTools::println("STATE EXIST");
//             XINPUT_STATE state = GamepadXInput::getState(0);
//             client.sendMessage(state, 0);
//         }
//         System::sleep(10);
//     }
//     GamepadXInput::dispose();
// }

// #include "SIMD.h"
// #include "SimpleGraphics.h"

// void polygonFillTest(const std::vector<Vec2f>& vertices)
// {
//     //somewhat naive approach. For each line, calculate the intersections with the valid scanlines based on max and min y values
//     if(vertices.size() < 3)
//         return;
    
//     //ignoring anti-aliasing potential, drop all decimal values
//     for(Vec2f v : vertices)
//     {
//         v.x = floor(v.x);
//         v.y = floor(v.y);
//     }

//     //mark vertices that need to be duplicated. Duplication happens due to a merge or split vertex where the internal angle is either too small or too large.
//     //Here we care about split or merge verticies with respect to the vertical axis. In this case, if both lines are on the same side of the spliting horizontal axis then it needs to be duplicated

//     //find minY, maxY
//     int minY = (int)vertices.front().y;
//     int maxY = (int)vertices.front().y;
//     int minX = (int)vertices.front().x;
//     int maxX = (int)vertices.front().x;
//     for(int i=1; i<vertices.size(); i++)
//     {
//         minY = MathExt::min(minY, (int)vertices[i].y);
//         maxY = MathExt::max(maxY, (int)vertices[i].y);
//         minX = MathExt::min(minX, (int)vertices[i].x);
//         maxX = MathExt::max(maxX, (int)vertices[i].x);
//     }

//     std::vector<std::vector<float>> scanlineCriticalPoints = std::vector<std::vector<float>>(1+maxY-minY);
    

//     //for each line, gather their critical points. Also duplicate those points that are considered merge/split vertices. Treat all regular vertices as normal
//     for(int i=0; i<vertices.size(); i++)
//     {
//         int lastIndex = i-1;
//         if(lastIndex < 0)
//             lastIndex = vertices.size()-1;
//         int nextIndex = (i+1) % vertices.size();

//         bool shouldDuplicate = false;
        
//         //don't need lines to determine if it is a vertex that should be duplicated
//         //check if both last and next vertex are lower or higher
//         if(vertices[lastIndex].y < vertices[i].y && vertices[nextIndex].y < vertices[i].y)
//         {
//             shouldDuplicate = true;
//         }
//         else if(vertices[lastIndex].y > vertices[i].y && vertices[nextIndex].y > vertices[i].y)
//         {
//             shouldDuplicate = true;
//         }


//         //calculate Lines
//         Line l = Line(vertices[i], vertices[nextIndex]);
//         int lMin = l.getMinY();
//         int lMax = l.getMaxY();
        
//         //ignore the edge points as those are vertex points for sure.
//         for(int j=lMin+1; j<lMax; j++)
//         {
//             float criticalPoint = l.solveForX(j);
//             scanlineCriticalPoints[j-minY].push_back(criticalPoint);
//         }
        
//         scanlineCriticalPoints[(int)vertices[nextIndex].y-minY].push_back(vertices[nextIndex].x);

//         //ignore the very first point at the lines minimum Y
//         if(shouldDuplicate)
//         {
//             // float criticalPoint = l.solveForX(lMax);
//             scanlineCriticalPoints[(int)vertices[i].y-minY].push_back(vertices[i].x);
//         }
//     }

//     Image result = Image(maxX-minX+1, maxY-minY+1);
//     //for now, print out the critical points. Later, draw them
//     for(int y=0; y<scanlineCriticalPoints.size(); y++)
//     {
//         StringTools::println("Scan Line %d:", y);
//         StringTools::print("\t");

//         Sort::mergeSort(scanlineCriticalPoints[y].data(), scanlineCriticalPoints[y].size());
//         for(int j=0; j<scanlineCriticalPoints[y].size(); j++)
//         {
//             StringTools::print("%.3f, ", scanlineCriticalPoints[y][j] + abs(minX));
//         }
//         StringTools::print("\n");

//         //actually fill between the critical points
        
//         StringTools::print("\t");
//         StringTools::print("\t");
//         for(int j=1; j<scanlineCriticalPoints[y].size(); j+=2)
//         {
//             int startPoint = scanlineCriticalPoints[y][j-1] - minX;
//             int endPoint = scanlineCriticalPoints[y][j] - minX;
            
//             StringTools::print("(%d, %d) - ", startPoint, endPoint);
//             SimpleGraphics::fillBetween(Color{255,0,0,255}, startPoint, endPoint, y, &result);
//         }
//         StringTools::print("\n");
//     }

//     result.savePNG("TestFill.png");
// }

// void polygonFillTest2(const std::vector<Vec2f>& vertices)
// {
//     //Line sweep based approach. Sort lines by min y value, add them to list of possible lines when appropriate. Remove lines that are no longer needed.
    
//     //create all lines needed
//     //Sort lines by min y value (std::vector<Line>) O(NLogN)
//     //for each scanline, add line from the list of sorted lines into a temporary data structure that is used for calculations. (also std::vector<Line>) O(1) Amortized
//     //remove all scanlines that are no longer needed. (as you come across them, move them to the end of the list and then pop back. O(1) operation ) O(1) Amortized
//     //
    
//     struct CriticalPointOrLine
//     {
//         bool isHorizontalLine = false;
//         double p1;
//         double p2;
//         bool operator<(const CriticalPointOrLine& other)
//         {
//             if(p1 == other.p1)
//                 return other.isHorizontalLine;
//             return p1 < other.p1;
//         }
//         bool operator==(const CriticalPointOrLine& other)
//         {
//             return p1 == other.p1 && p2 == other.p2 && isHorizontalLine == other.isHorizontalLine;
//         }
//     };

//     std::vector<Line> polyLines = std::vector<Line>(vertices.size());
//     int minY = vertices.front().y;
//     int maxY = vertices.front().y;
//     for(size_t i=0; i<vertices.size()-1; i++)
//     {
//         minY = __min(vertices[i].y, minY);
//         maxY = __max(vertices[i].y, maxY);
//         polyLines[i] = Line(vertices[i], vertices[i+1]);
//     }
//     minY = __min(vertices.back().y, minY);
//     maxY = __max(vertices.back().y, maxY);
//     polyLines.back() = Line(vertices.back(), vertices.front());

//     Sort::mergeSort<Line>(polyLines.data(), polyLines.size(), [](Line l1, Line l2) ->bool{
//         return l1.getMinY() < l2.getMinY();
//     });

//     size_t indexInLines = 0;
//     std::vector<Line> processingLines = std::vector<Line>();
//     for(int y=minY; y<=maxY; y++)
//     {
//         for(; indexInLines < polyLines.size(); indexInLines++)
//         {
//             if(y >= polyLines[indexInLines].getMinY())
//             {
//                 processingLines.push_back(polyLines[indexInLines]);
//             }
//             else
//                 break;
//         }

//         size_t i = 0;

//         std::vector<CriticalPointOrLine> criticalPoints;
//         while(i < processingLines.size())
//         {
//             if(y > processingLines[i].getMaxY())
//             {
//                 std::swap(processingLines[i], processingLines.back());
//                 processingLines.pop_back();
//             }
//             else
//             {
//                 //process line
//                 if(processingLines[i].getSlope() != 0)
//                 {
//                     criticalPoints.push_back({false, processingLines[i].solveForX(y), 0});
//                 }
//                 else
//                 {
//                     criticalPoints.push_back({true, processingLines[i].getMinX(), processingLines[i].getMaxX()});
//                 }
                
//                 i++;
//             }
//         }

//         Sort::mergeSort<CriticalPointOrLine>(criticalPoints.data(), criticalPoints.size());
//         bool skip = false;
//         StringTools::print("%d: \n\t", y);
//         for(size_t j=0; j<criticalPoints.size(); j++)
//         {
//             CriticalPointOrLine critPoint = criticalPoints[j];
//             StringTools::print("{%d, %.3f, %.3f}, ", critPoint.isHorizontalLine, critPoint.p1, critPoint.p2);
//         }

//         StringTools::print(" || ");

//         for(size_t j=0; j<criticalPoints.size(); j++)
//         {
//             CriticalPointOrLine critPoint = criticalPoints[j];
//             if(critPoint.isHorizontalLine && (j % 2) == 1)
//             {
//                 //if last point is on an odd index, duplicate previous point (minX of this line). Otherwise, skip
//                 if(criticalPoints[j+1].p1 != critPoint.p1)
//                 {
//                     StringTools::print("%.3f, ", critPoint.p2);
//                 }
//                 continue;
//             }
//             StringTools::print("%.3f, ", critPoint.p1);
//         }

//         StringTools::println("");
//     }
// }

// void polygonFillTest3(const std::vector<Vec2f>& vertices)
// {
//     //Line sweep based approach. Sort lines by min y value, add them to list of possible lines when appropriate. Remove lines that are no longer needed.
    
//     //create all lines needed
//     //Sort lines by min y value (std::vector<Line>) O(NLogN)
//     //for each scanline, add line from the list of sorted lines into a temporary data structure that is used for calculations. (also std::vector<Line>) O(1) Amortized
//     //remove all scanlines that are no longer needed. (as you come across them, move them to the end of the list and then pop back. O(1) operation ) O(1) Amortized
//     //
    
//     struct CriticalPointOrLine
//     {
//         bool isHorizontalLine = false;
//         double p1;
//         double p2;
//         bool operator<(const CriticalPointOrLine& other)
//         {
//             if(p1 == other.p1)
//                 return other.isHorizontalLine;
//             return p1 < other.p1;
//         }
//         bool operator==(const CriticalPointOrLine& other)
//         {
//             return p1 == other.p1 && p2 == other.p2 && isHorizontalLine == other.isHorizontalLine;
//         }
//     };

    

//     std::vector<Line> polyLines = std::vector<Line>(vertices.size());
//     float minY = vertices.front().y;
//     float maxY = vertices.front().y;
//     float minX = vertices.front().x;
//     float maxX = vertices.front().x;
//     for(size_t i=0; i<vertices.size()-1; i++)
//     {
//         minY = __min(vertices[i].y, minY);
//         maxY = __max(vertices[i].y, maxY);
//         minX = __min(vertices[i].x, minX);
//         maxX = __max(vertices[i].x, maxX);
//         polyLines[i] = Line(vertices[i], vertices[i+1]);
//     }
//     minY = __min(vertices.back().y, minY);
//     maxY = __max(vertices.back().y, maxY);
//     minX = __min(vertices.back().x, minX);
//     maxX = __max(vertices.back().x, maxX);
//     polyLines.back() = Line(vertices.back(), vertices.front());

//     Sort::mergeSort<Line>(polyLines.data(), polyLines.size(), [](Line l1, Line l2) ->bool{
//         return l1.getMinY() < l2.getMinY();
//     });

//     Image outputImage = Image((int)(maxX-minX)+1, (int)(maxY-minY)+1);
//     size_t indexInLines = 0;
//     std::vector<Line> processingLines = std::vector<Line>();
//     for(int y=(int)minY; y<=(int)maxY; y++)
//     {
//         for(; indexInLines < polyLines.size(); indexInLines++)
//         {
//             if(y >= polyLines[indexInLines].getMinY())
//             {
//                 processingLines.push_back(polyLines[indexInLines]);
//             }
//             else
//                 break;
//         }

//         size_t i = 0;

//         std::vector<CriticalPointOrLine> criticalPoints;
//         while(i < processingLines.size())
//         {
//             if(y > processingLines[i].getMaxY())
//             {
//                 std::swap(processingLines[i], processingLines.back());
//                 processingLines.pop_back();
//             }
//             else
//             {
//                 //process line
//                 if(processingLines[i].getSlope() != 0)
//                 {
//                     criticalPoints.push_back({false, processingLines[i].solveForX(y), 0});
//                 }
//                 else
//                 {
//                     criticalPoints.push_back({true, processingLines[i].getMinX(), processingLines[i].getMaxX()});
//                 }
//                 i++;
//             }
//         }

//         Sort::mergeSort<CriticalPointOrLine>(criticalPoints.data(), criticalPoints.size());
//         std::vector<double> finalPairs;
//         // bool skip = false;
//         // StringTools::print("%d: \n\t", y);
//         // for(size_t j=0; j<criticalPoints.size(); j++)
//         // {
//         //     CriticalPointOrLine critPoint = criticalPoints[j];
//         //     StringTools::print("{%d, %.3f, %.3f}, ", critPoint.isHorizontalLine, critPoint.p1, critPoint.p2);
//         // }

//         // StringTools::print(" || ");

//         for(size_t j=0; j<criticalPoints.size(); j++)
//         {
//             CriticalPointOrLine critPoint = criticalPoints[j];
//             if(critPoint.isHorizontalLine && (j % 2) == 1)
//             {
//                 //if last point is on an odd index, duplicate previous point (minX of this line). Otherwise, skip
//                 if(criticalPoints[j+1].p1 != critPoint.p1)
//                 {
//                     // StringTools::print("%.3f, ", critPoint.p2);
//                     finalPairs.push_back(critPoint.p2);
//                 }
//                 continue;
//             }
//             // StringTools::print("%.3f, ", critPoint.p1);
//             finalPairs.push_back(critPoint.p1);
//         }

//         StringTools::print("%d:\n\t", y);
//         for(size_t j=1; j<finalPairs.size(); j+=2)
//         {
//             int startX = MathExt::floor(finalPairs[j-1] - minX);
//             int endX = MathExt::ceil(finalPairs[j] - minX);
            
//             StringTools::print("(%d, %d) - ", startX, endX);
//             SimpleGraphics::fillBetween(Color{255,0,0,255}, startX, endX, y-minY, &outputImage);
//         }
//         StringTools::println("");
//     }
//     outputImage.savePNG("TestFill3.png");
// }

// #include "DCEL.h"
// void testDCEL()
// {
//     DCEL test = DCEL();
//     test.addPolygon({
//         Vec2f(0, 0),
//         Vec2f(32, 32),
//         Vec2f(64, 32)
//     });

//     StringTools::println("\n______VERTICES______\n");
//     //for each Vertex, print its MemoryLocation and position
//     for(size_t index=0; index < test.getTotalVertices(); index++)
//     {
//         StringTools::println("%p: %.3f", test.getVertex(index), test.getVertex(index)->position);
//     }
    
//     StringTools::println("\n______HALF EDGES______\n");
//     //for each Half Edge, print its MemoryLocation and all the pointers
//     for(size_t index=0; index < test.getTotalEdges(); index++)
//     {
//         DCELEdge* halfEdge = test.getEdge(index);
//         StringTools::println("%p: ", halfEdge);
//         StringTools::println("\tOrigin: %p\n\tTwin Edge: %p\n\tIncident Face: %p\n\tNext Edge: %p\n\tPrevious Edge: %p", halfEdge->origin, halfEdge->twin, halfEdge->incidentFace, halfEdge->next, halfEdge->previous);
//     }

//     StringTools::println("\n______FACES______\n");
//     //for each Face, print its MemoryLocation and all the pointers
//     for(size_t index=0; index < test.getTotalFaces(); index++)
//     {
//         DCELFace* face = test.getFace(index);
//         StringTools::println("%p: ", face);
//         StringTools::println("\tOuter Component: %p\n\tInner Components: %p\n\tClockwise: %d", face->outerComponent, face->innerComponents, face->windingOrder);
//     }
// }

// void quickDCELVertexStuff()
// {
//     DCEL test = DCEL();
//     DCELVertex* v1 = test.addVertex(Vec2f(0, 0));
//     DCELVertex* v2 = test.addVertex(Vec2f(32, 0));
//     DCELVertex* v3 = test.addVertex(Vec2f(32, 32));
//     test.addEdge(v1, v2);
//     test.addEdge(v2, v3);
//     test.addEdge(v3, v1);

//     StringTools::println("BEFORE VERTEX INTERSECTION");
//     StringTools::println("\n______VERTICES______\n");
//     //for each Vertex, print its MemoryLocation and position
//     for(size_t index=0; index < test.getTotalVertices(); index++)
//     {
//         StringTools::println("%p: %.3f", test.getVertex(index), test.getVertex(index)->position);
//     }
    
//     StringTools::println("\n______HALF EDGES______\n");
//     //for each Half Edge, print its MemoryLocation and all the pointers
//     for(size_t index=0; index < test.getTotalEdges(); index++)
//     {
//         DCELEdge* halfEdge = test.getEdge(index);
//         StringTools::println("%p: ", halfEdge);
//         StringTools::println("\tOrigin: %p\n\tTwin Edge: %p\n\tIncident Face: %p\n\tNext Edge: %p\n\tPrevious Edge: %p", halfEdge->origin, halfEdge->twin, halfEdge->incidentFace, halfEdge->next, halfEdge->previous);
//     }

//     StringTools::println("\n______FACES______\n");
//     //for each Face, print its MemoryLocation and all the pointers
//     for(size_t index=0; index < test.getTotalFaces(); index++)
//     {
//         DCELFace* face = test.getFace(index);
//         StringTools::println("%p: ", face);
//         StringTools::println("\tOuter Component: %p\n\tInner Components: %p\n\tClockwise: %d", face->outerComponent, face->innerComponents, face->windingOrder);
//     }
    
//     test.addVertex(Vec2f(16, 0));
//     StringTools::println("AFTER VERTEX INTERSECTION");
//     StringTools::println("\n______VERTICES______\n");
//     //for each Vertex, print its MemoryLocation and position
//     for(size_t index=0; index < test.getTotalVertices(); index++)
//     {
//         StringTools::println("%p: %.3f", test.getVertex(index), test.getVertex(index)->position);
//     }
    
//     StringTools::println("\n______HALF EDGES______\n");
//     //for each Half Edge, print its MemoryLocation and all the pointers
//     for(size_t index=0; index < test.getTotalEdges(); index++)
//     {
//         DCELEdge* halfEdge = test.getEdge(index);
//         StringTools::println("%p: ", halfEdge);
//         StringTools::println("\tOrigin: %p\n\tTwin Edge: %p\n\tIncident Face: %p\n\tNext Edge: %p\n\tPrevious Edge: %p", halfEdge->origin, halfEdge->twin, halfEdge->incidentFace, halfEdge->next, halfEdge->previous);
//     }

//     StringTools::println("\n______FACES______\n");
//     //for each Face, print its MemoryLocation and all the pointers
//     for(size_t index=0; index < test.getTotalFaces(); index++)
//     {
//         DCELFace* face = test.getFace(index);
//         StringTools::println("%p: ", face);
//         StringTools::println("\tOuter Component: %p\n\tInner Components: %p\n\tClockwise: %d", face->outerComponent, face->innerComponents, face->windingOrder);
//     }
// }

// #include "SIMD.h"
// #include "HttpServer.h"

// char getRequestHandler(HttpServer* server, WebRequest& request, std::vector<unsigned char>& bodyData, std::string ip, size_t requesterID)
// {
//     std::string s = StringTools::toLowercase(request.getUrl());

//     //prevent access to any non root folder
//     File fullFilename = s;
//     if(fullFilename.getPath().empty() || fullFilename.getPath() == "./" || fullFilename.getPath() == "/")
//     {
//         return server->defaultGetFunction(request, ip, requesterID);
//     }
//     server->send404Error(requesterID);
//     return HttpServer::STATUS_ERROR;
// }

// void quit()
// {
//     running = false;
//     StringTools::println("QUITING");
// }

// int main(int argc, char** argv)
// {
//     // std::string expires = "Fri, 12 Sep 2025 22:21:38 GMT";
//     // time_t t = System::timeFromDateString("%a, %d %b %Y %T", expires);
//     // std::tm date = System::convertTimeToDate(t);
//     // StringTools::println("%d", date.tm_hour);

//     // StringTools::println("%s", System::timeToDateString(t));
    
    
//     // Image* img = System::getScreenShotDesktop();
//     // img->savePNG("testShot.png", false, false, true);
//     // delete img;

//     // testXInputEmulator();

    
//     // // std::vector<Vec2f> vertices = {Vec2f(-49.1, 96.2), Vec2f(4.4, 37.3), Vec2f(12.6, 102)};
//     // // std::vector<Vec2f> vertices = {Vec2f(100,200), Vec2f(300,200), Vec2f(275,150), Vec2f(200,150), Vec2f(150,100)};
//     // // std::vector<Vec2f> vertices = {Vec2f(150,100), Vec2f(200,150), Vec2f(275,150), Vec2f(300,200), Vec2f(100,200)};
//     // std::vector<Vec2f> vertices = {Vec2f(-7.5, 0), Vec2f(7.5, 0), Vec2f(-5, -10), Vec2f(0, 5), Vec2f(5, -10)};
//     // // std::vector<Vec2f> vertices = {Vec2f(5, -10), Vec2f(0, 5), Vec2f(-5, -10), Vec2f(7.5, 0), Vec2f(-7.5, 0)};

//     // // polygonFillTest(vertices);
//     // // polygonFillTest2(vertices);
//     // // polygonFillTest3(vertices);
//     // Image outputImage = Image(100, 100);
//     // SimpleGraphics::setColor({255,0,0,255});
//     // SimpleGraphics::setFillRule(SimpleGraphics::FILL_NON_ZERO);
//     // SimpleGraphics::drawPolygon(vertices.data(), vertices.size(), &outputImage);
//     // outputImage.savePNG("ProperPolyTest.png");
    

//     // StringTools::println("%d", SIMD_U32::SIZE);
//     // StringTools::println("%d", 14);
//     // StringTools::println("%llu", SIMD_U32::getSIMDBound(14));


//     //vary between 0, 255
//     Image img = Image(256, 256);
//     FlatFillFunction baseFillFunction({255, 160, 100, 255});

//     for(int y=0; y<256; y++)
//     {
//         baseFillFunction.fillBetween(0, 256, y, &img);
//     }
//     img.savePNG("DefaultFillFunction.png", false, false, false);

    
//     LinearGradientFillFunction linGradFillFunction(Vec2f(0, 0), Vec2f(256, 256), {{0, Color{0, 0, 0, 255}}, {1, Color{255, 0, 0, 255}}});

//     for(int y=0; y<256; y++)
//     {
//         linGradFillFunction.fillBetween(0, 256, y, &img);
//     }
//     img.savePNG("LinearFillFunction.png", false, false, false);

//     // testDCEL();
//     // quickDCELVertexStuff();

//     // NetworkConfig conf;
//     // conf.amountOfConnectionsAllowed = 64;
//     // conf.location = "0.0.0.0";
//     // conf.port = 44369; //will need to load this from a file or something. Just a test for now
//     // conf.TCP = true;
//     // conf.type = Network::TYPE_SERVER;
//     // conf.secure = true;

//     // HttpServer server = HttpServer(conf, 2, true, "openssl_2.crt", "openssl_2.key");
//     // server.setAllowPersistentConnection(false);
    
//     // server.setGetFuncMapper([](HttpServer* server, WebRequest& request, std::vector<unsigned char>& bodyData, std::string ip, size_t requesterID) ->char{
//     //     return getRequestHandler(server, request, bodyData, ip, requesterID);
//     // });
//     // server.setLogInfo(true);
//     // System::mapInteruptSignal(quit);

//     // server.start();

//     // while(running)
//     // {
//     //     System::sleep(10, 0, false);
//     // }
//     return 0;
// }

// //TODO: Drawing Sprites is currently broken. Fix it
// //TODO: Scaling can now be done using SIMD. Fix it
// //TODO: SimpleGraphics functions to remove colors from an image
// //TODO: Gradient Fill
// //TODO: Flood Fill

// //TODO: Rework GUI so that all objects have margins, padding, etc.

// //TODO: Ensure OpenGL functions work properly.
// //TODO: Ensure DirectX functions work properly. (sometimes I want to stick a pin in this ngl)
// //TODO: Allow HTTPServer and WebSocketServer to exist on the same port potentially?

// //TODO: Add polygon triangulation (Depends on DCEL)

// // - WORKING ON CURRENTLY -
// //TODO: Fix Even-Odd rendering of polygons, etc. *May be finished. Needs further testing
// //TODO: Finish DCEL
// //TODO: Change how Cookies are handled. Should be importable/exportable and must be integrated with existing HTTP Server/Client






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
#include <handleapi.h>
#include <minwinbase.h>
#include <mutex>
#include <processthreadsapi.h>
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
//         server.setGetFuncMapper([&indexHTML, &responseURL, &quickMut](HttpServer* servP, WebRequest& req, std::vector<unsigned char>& body, std::string location, size_t id) -> char{
            
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
//     server.setGetFuncMapper([&indexHTML](HttpServer* servP, WebRequest& req, std::vector<unsigned char>& body, std::string location, size_t id) -> char{
//         StringTools::println(req.getUrl());
//         WebRequest response;
//         response.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 200 OK", false);
//         response.addKeyValue("Content-Type", WebRequest::getMimeTypeFromExt("html"));
//         response.addKeyValue("Content-Length", StringTools::toString(indexHTML.size()));
        
//         servP->sendResponse(response, location, id, req);
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

// #include <Windows.h>

// void EmbedCalc(HWND hWnd)
// {
//     HWND calcHwnd = FindWindow(NULL, "Calculator");
//     if(calcHwnd != NULL)
//     {
//         // Change the parent so the calc window belongs to our apps main window 
//         SetParent(calcHwnd, hWnd);

//         // Update the style so the calc window is embedded in our main window
//         SetWindowLong(calcHwnd, GWL_STYLE, GetWindowLong(calcHwnd, GWL_STYLE) | WS_CHILD);

//         // We need to update the position as well since changing the parent does not
//         // adjust it automatically.
//         SetWindowPos(calcHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE);
        
//     }
//     else {
//         StringTools::println("Failed to find window");
//     }
// }

void runBasicHTTPServer()
{
	NetworkConfig config;
	config.amountOfConnectionsAllowed = 65535;
	config.location = "127.0.0.1";
	config.port = 4040;
	config.secure = false;
	config.TCP = true;
	config.type = Network::TYPE_SERVER;
	HttpServer serv = HttpServer(config, 8, false, "cert.pem", "key.pem");
	serv.setAllowPersistentConnection(false);
	// serv.setLogInfo(true);
	serv.start();

	while(serv.getRunning())
	{
		System::sleep(1, 0, false);
	}
}

std::vector<size_t> times;
void test1()
{
	for(int i=0; i<100; i++)
	{
		std::cout << "Test1() = " << i << std::endl;
		times.push_back(System::getCurrentTimeNano());
		ThisFiberTask::yield();
		times.push_back(System::getCurrentTimeNano());
	}
}

void test2()
{
	for(int i=0; i<100; i++)
	{
		std::cout << "Test2() = " << i << std::endl;
		times.push_back(System::getCurrentTimeNano());
		ThisFiberTask::yield();
		times.push_back(System::getCurrentTimeNano());
	}
}

void test3()
{
	while(true)
	{
		ThisFiberTask::yield();
	}
}
void quickYieldingTest()
{
	// FiberTask task1 = FiberTask(test1);
	// FiberTask task2 = FiberTask(test2);

	// FiberTaskScheduler::addTask(&task1);
	// FiberTaskScheduler::addTask(&task2);

	// //run all task till completion
	// FiberTaskScheduler::run();

	// for(int i=2; i<times.size()-2; i+=2)
	// {
	// 	StringTools::println("Time to context switch: %llu", times[i+1] - times[i]);
	// }
	
	FiberTask task1 = FiberTask(test3);
	size_t t1 = System::getCurrentTimeNano();
	for(int i=0; i<1000000; i++)
	{
		task1.run();
	}
	size_t t2 = System::getCurrentTimeNano();
	StringTools::println("Time to complete: %llu", t2-t1);
	StringTools::println("AVG Time to Context switch round trip: %llu", (t2-t1)/1000000);
	StringTools::println("AVG Time to Context switch: %llu", (t2-t1)/1000000/2);

}

void testJSON()
{
	std::string body = "{\"Username\": \"user\", \"Password\": \"password\"}";
	SimpleJSON j = SimpleJSON((unsigned char*)body.c_str(), body.size());
}
int main()
{
    // smpl::SimpleJSON authStuff = setupOAUTH("client_secret.json", 
    //                                         "https://www.googleapis.com/auth/youtube");
    
    // // smpl::StringTools::println("%s", authStuff.getString(true));
    // createOAuthServer("client_secret.json", "https://www.googleapis.com/auth/youtube");

    // auto output = StringTools::parseURLArguments("/?code=4/0ATX87lOb7x-IZz7zUHd7I_kfdejcagXqNZV0KQkFhlXTbCuL9yErhU2GtIcyUYOX9N2icQ&scope=https://www.googleapis.com/auth/youtube");
    // StringTools::println("%s", output);
	
	runBasicHTTPServer();
    // quickYieldingTest();
	// testJSON();
    return 0;
}

//client_id: Kd4Ue8jYiWhocNK1eSuE1WZ7
//client_secret: lPktggkJ7GIt8Jg9wexta1506Pyv22Z3uAXgbRAb590G1W7q
//login: bewildered-skylark@example.com
//password: Powerful-Partridge-56