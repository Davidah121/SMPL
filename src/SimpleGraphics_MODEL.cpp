#include "InternalGraphicsHeader.h"
#include "VectorGraphic.h"

namespace smpl
{
	
	void formatToString(StringStream& stream, const PolyCriticalPoint& v, const std::string& options)
	{
		stream.write('(');
		formatToString(stream, v.xValue, "%.3f");
		stream.write(" | ");
		formatToString(stream, v.direction, "%d");
		stream.write(")");
	}
	
	// void SimpleGraphics::drawPolygon(Vec2f* points, int size, Image* surf)
	// {
	// 	Image* otherImg;
	// 	if (surf == nullptr)
	// 		return;
	// 	else
	// 		otherImg = surf;

	// 	if (otherImg != nullptr)
	// 	{
	// 		if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
	// 		{
	// 			return;
	// 		}
	// 		//similar in nature to the vector graphic version
	// 		//always connects the last point to the first

	// 		std::vector<Vec2f> actualPoints = std::vector<Vec2f>();
	// 		for(int i=0; i<size; i++)
	// 		{
	// 			actualPoints.push_back(points[i]);
	// 		}
	// 		actualPoints.push_back(points[0]);
			
	// 		int minY = (int)points[0].y;
	// 		int maxY = (int)points[0].y;

	// 		for(int i=0; i<size; i++)
	// 		{
	// 			if((int)points[i].y < minY)
	// 			{
	// 				minY = (int)points[i].y;
	// 			}
	// 			if((int)points[i].y > maxY)
	// 			{
	// 				maxY = (int)points[i].y;
	// 			}
	// 		}

	// 		minY = MathExt::clamp(minY, 0, otherImg->getHeight()-1);
	// 		maxY = MathExt::clamp(maxY, 0, otherImg->getHeight()-1);

	// 		int yDis = 1+maxY-minY;

	// 		std::vector<std::vector<PolyCriticalPoint>> scanLines = std::vector<std::vector<PolyCriticalPoint>>(yDis);
	// 		std::vector<std::vector<std::pair<double, double>>> horizontalLinesFound = std::vector<std::vector<std::pair<double, double>>>(yDis);

	// 		for(int i=0; i<size; i++)
	// 		{
	// 			//Ax + By + C = 0
	// 			//A=(y1-y2)
	// 			//B=(x2-x1)
	// 			//C=(x1*y2 - x2*y1)
	// 			double A = (actualPoints[i].y - actualPoints[i+1].y);
	// 			double B = (actualPoints[i+1].x - actualPoints[i].x);
	// 			double C = (actualPoints[i].x*actualPoints[i+1].y - actualPoints[i+1].x*actualPoints[i].y);
	// 			bool posDir = true;

	// 			//AFAIK, if the y is going up in some way, the direction is positive
	// 			//otherwise, it is negative.
	// 			if(A<=0)
	// 			{
	// 				posDir = true;
	// 			}
	// 			else
	// 			{
	// 				posDir = false;
	// 			}
				
	// 			if(A!=0)
	// 			{
	// 				//vertical line or some other valid line
	// 				int thisMinY = (int)MathExt::round( MathExt::min( actualPoints[i].y, actualPoints[i+1].y));
	// 				int thisMaxY = (int)MathExt::round( MathExt::max( actualPoints[i].y, actualPoints[i+1].y));

	// 				thisMinY = MathExt::clamp(thisMinY, minY, maxY);
	// 				thisMaxY = MathExt::clamp(thisMaxY, minY, maxY);

	// 				for(int j=thisMinY; j<=thisMaxY; j++)
	// 				{
	// 					double xValue = -(B*j + C) / A;
	// 					xValue = MathExt::clamp<double>(xValue, 0, otherImg->getWidth());
						
	// 					scanLines[j-minY].push_back({xValue,posDir});
	// 				}
	// 			}
	// 			else
	// 			{
	// 				//horizontal line or invalid
	// 				//do nothing
	// 				int thisMinY = (int)MathExt::round( MathExt::min( actualPoints[i].y, actualPoints[i+1].y));
	// 				double thisMinX = MathExt::min( actualPoints[i].x, actualPoints[i+1].x);
	// 				double thisMaxX = MathExt::max( actualPoints[i].x, actualPoints[i+1].x);
	// 				horizontalLinesFound[thisMinY-minY].push_back({thisMinX,thisMaxX});
	// 			}
	// 		}

	// 		//sort the points on the scanlines
	// 		//#pragma omp parallel for
	// 		for(int i=0; i<yDis; i++)
	// 		{
	// 			if(scanLines[i].size()>0)
	// 			{
	// 				Sort::insertionSort<PolyCriticalPoint>(scanLines[i].data(), scanLines[i].size(), [](PolyCriticalPoint a, PolyCriticalPoint b) -> bool {return a.xValue<b.xValue;});
					
	// 				//may need to insert a points to handle horizontal line cases
	// 				{
	// 					std::vector<PolyCriticalPoint> tempLines;
	// 					int j=0;
	// 					while(j < scanLines[i].size())
	// 					{
	// 						if(j == scanLines[i].size()-1)
	// 						{
	// 							tempLines.push_back(scanLines[i][j]);
	// 							j++;
	// 							break;
	// 						}
							
	// 						bool isConnected = false;
	// 						for(std::pair<double, double> p : horizontalLinesFound[i])
	// 						{
	// 							if(p.first == scanLines[i][j].xValue && p.second == scanLines[i][j+1].xValue)
	// 							{
	// 								//this is part of a horizontal line. do extra stuff
	// 								isConnected = true;
	// 							}
	// 						}
							
	// 						if(isConnected)
	// 						{
	// 							//do extra stuff
	// 							if(scanLines[i][j].direction != scanLines[i][j+1].direction)
	// 							{
	// 								tempLines.push_back(scanLines[i][j]);
	// 							}
	// 							j++;
	// 						}
	// 						else
	// 						{
	// 							//add single point
	// 							tempLines.push_back(scanLines[i][j]);
	// 							j++;
	// 						}
	// 					}
	// 					scanLines[i] = tempLines;
	// 				}
					
	// 				//rule, can not be the same if you are filling to it.
	// 				//different for even-odd and non-zero
	// 				std::vector<PolyCriticalPoint> newScanLine = std::vector<PolyCriticalPoint>();

	// 				if(fillRule == SimpleGraphics::FILL_EVEN_ODD)
	// 				{
	// 					for(int j=1; j<scanLines[i].size(); j+=2)
	// 					{
	// 						//check if its apart of a horizontal line. IE, both xValues at this y value are apart of the points exactly
	// 						if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
	// 						{
	// 							newScanLine.push_back(scanLines[i][j-1]);
	// 							newScanLine.push_back(scanLines[i][j]);
	// 						}
	// 						else
	// 						{
	// 							if(scanLines[i][j].direction != scanLines[i][j-1].direction)
	// 							{
	// 								newScanLine.push_back(scanLines[i][j-1]);
	// 								newScanLine.push_back(scanLines[i][j]);
	// 							}
	// 						}
	// 					}
	// 				}
	// 				else
	// 				{
	// 					newScanLine.push_back(scanLines[i][0]);
	// 					for(int j=1; j<scanLines[i].size(); j++)
	// 					{
	// 						if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
	// 						{
	// 							newScanLine.push_back(scanLines[i][j]);
	// 						}
	// 						else
	// 						{
	// 							if(scanLines[i][j].direction != scanLines[i][j-1].direction)
	// 							{
	// 								newScanLine.push_back(scanLines[i][j]);
	// 							}
	// 						}
	// 					}
	// 				}

	// 				scanLines[i] = newScanLine;
	// 			}
	// 		}

	// 		if(fillRule==SimpleGraphics::FILL_EVEN_ODD)
	// 		{
	// 			//even-odd rule. Fill between even to odd indicies
	// 			//not odd to even indicies
	// 			LARGE_ENOUGH_CLAUSE(yDis)
	// 			#pragma omp parallel for
	// 			for(int j=0; j<yDis; j++)
	// 			{
	// 				int actualSize = scanLines[j].size();
	// 				if(actualSize%2 == 1)
	// 				{
	// 					actualSize-=2;
	// 				}
	// 				else
	// 				{
	// 					actualSize-=1;
	// 				}
					

	// 				for(int i=0; i<actualSize; i+=2)
	// 				{
	// 					//fill between spots
	// 					double fracStartX, fracEndX;
	// 					int startX = MathExt::floor(scanLines[j][i].xValue);
	// 					int endX = MathExt::ceil(scanLines[j][i+1].xValue);

	// 					int fillX = startX;
	// 					do
	// 					{
	// 						SimpleGraphics::drawPixel(fillX, j+minY, SimpleGraphics::activeColor, otherImg);
	// 						fillX++;
	// 					} while (fillX<=endX);

	// 					// fracStartX = 1.0-MathExt::frac(scanLines[j][i].xValue);
	// 					// fracEndX = MathExt::frac(scanLines[j][i+1].xValue);
	// 					// if(fracEndX <= 0.0)
	// 					// 	fracEndX = 1.0;
						
	// 					// fracStartX = 1;
	// 					// fracEndX = 1;
	// 					// Color startBlend = activeColor;
	// 					// Color endBlend = activeColor;
	// 					// startBlend.alpha = fracStartX*255;
	// 					// endBlend.alpha = fracEndX*255;
						
	// 					// SimpleGraphics::drawPixel(startX, j+minY, startBlend, otherImg);
	// 					// if(fracStartX >= 1.0)
	// 					// 	SimpleGraphics::drawPixel(startX, j+minY, SimpleGraphics::activeColor, otherImg);
						
	// 					// int fillX = startX+1;
	// 					// while(fillX <= endX-1)
	// 					// {
	// 					// 	SimpleGraphics::drawPixel(fillX, j+minY, SimpleGraphics::activeColor, otherImg);
	// 					// 	fillX++;
	// 					// }
	// 					// SimpleGraphics::drawPixel(endX, j+minY, endBlend, otherImg);
	// 				}
	// 			}
	// 			RESET_LARGE_ENOUGH_CLAUSE()
	// 		}
	// 		else
	// 		{
	// 			//non-zero rule. Fill when positive only. Not when zero or
	// 			//less.
	// 			LARGE_ENOUGH_CLAUSE(yDis)
	// 			#pragma omp parallel for
	// 			for(int j=0; j<yDis; j++)
	// 			{
	// 				int passCounter = 0;
	// 				for(int i=0; i<scanLines[j].size()-1; i++)
	// 				{
	// 					//fill between spots
	// 					int startX = MathExt::floor(scanLines[j][i].xValue);
	// 					int endX = MathExt::ceil(scanLines[j][i+1].xValue);
	// 					if(scanLines[j][i].direction == true)
	// 					{
	// 						//positive y direction
	// 						passCounter++;
	// 					}
	// 					else
	// 					{
	// 						//negative y direction
	// 						passCounter--;
	// 					}
						
	// 					if(passCounter!=0)
	// 					{
	// 						int fillX = startX;

	// 						do
	// 						{
	// 							SimpleGraphics::drawPixel(fillX, j+minY, SimpleGraphics::activeColor, otherImg);
	// 							fillX++;
	// 						} while (fillX<=endX);
							
	// 					}
	// 				}
	// 			}
	// 			RESET_LARGE_ENOUGH_CLAUSE()
	// 		}

	// 	}

	// }

	void SimpleGraphics::drawPolygonOld(const Vec2f* points, size_t size, Image* surf)
	{
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}

			if(size < 3)
				return;
			
			struct CriticalPointOrLine
			{
				bool isHorizontalLine = false;
				bool slopeGoingUp = false;
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

			std::vector<Line> polyLines = std::vector<Line>(size);
			float minY = points[0].y;
			float maxY = points[0].y;
			float minX = points[0].x;
			float maxX = points[0].x;
			for(size_t i=0; i<size-1; i++)
			{
				minY = __min(points[i].y, minY);
				maxY = __max(points[i].y, maxY);
				minX = __min(points[i].x, minX);
				maxX = __max(points[i].x, maxX);
				polyLines[i] = Line(points[i], points[i+1]);
			}
			minY = __min(points[size-1].y, minY);
			maxY = __max(points[size-1].y, maxY);
			minX = __min(points[size-1].x, minX);
			maxX = __max(points[size-1].x, maxX);
			polyLines.back() = Line(points[size-1], points[0]);

			Sort::mergeSort<Line>(polyLines.data(), polyLines.size(), [](Line l1, Line l2) ->bool{
				return l1.getMinY() < l2.getMinY();
			});

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
							criticalPoints.push_back({false, processingLines[i].getToPoint().y > 0, processingLines[i].solveForX(y), 0});
						}
						else
						{
							criticalPoints.push_back({true, false, processingLines[i].getMinX(), processingLines[i].getMaxX()});
						}
						i++;
					}
				}

				Sort::mergeSort<CriticalPointOrLine>(criticalPoints.data(), criticalPoints.size());
				
				std::vector<double> finalPairs;
				if(fillRule == FILL_EVEN_ODD)
				{
					//fixing things up. for now, assume only even-odd works if at all
					for(size_t j=0; j<criticalPoints.size(); j++)
					{
						CriticalPointOrLine critPoint = criticalPoints[j];
						if(critPoint.isHorizontalLine && (j % 2) == 1)
						{
							//if last point is on an odd index, duplicate previous point (minX of this line). Otherwise, skip
							//also handle the case where the horizontal line's end point is not the next point.
							if(criticalPoints[j+1].p1 != critPoint.p1)
							{
								finalPairs.push_back(critPoint.p2);
							}
							continue;
						}
						finalPairs.push_back(critPoint.p1);
					}

				}
				else
				{
					//fill as long as the total number of slope changes is not zero
					//ignore horizontal lines
					int slopeCounter = 0;
					// finalPairs.push_back(criticalPoints[0].p1);

					for(CriticalPointOrLine critPoint : criticalPoints)
					{
						if(critPoint.isHorizontalLine)
							continue;
						
						if(slopeCounter != 0)
							finalPairs.push_back(critPoint.p1);

						slopeCounter += critPoint.slopeGoingUp ? 1 : -1;
						
						//duplicate last point since stuff between the next point should be included in the fill.
						//done to reduce duplicate code
						if(slopeCounter != 0)
							finalPairs.push_back(critPoint.p1);
					}
				}
				
				//fill between end points.
				if(SimpleGraphics::antiAliasing == true)
				{
					for(size_t j=1; j<finalPairs.size(); j+=2)
					{
						SimpleGraphics::fillBetweenAntiAliased(activeColor, finalPairs[j-1] - minX, finalPairs[j] - minX, y-minY, otherImg);
					}
				}
				else
				{
					for(size_t j=1; j<finalPairs.size(); j+=2)
					{
						SimpleGraphics::fillBetween(activeColor, MathExt::floor(finalPairs[j-1] - minX), MathExt::ceil(finalPairs[j] - minX), y-minY, otherImg);
					}
				}
			}
		}

	}

	void SimpleGraphics::drawPolygon(const Vec2f *points, size_t size, Image *surf)
	{
		drawPolygon(std::vector<const Vec2f*>{points},std::vector<size_t>{size}, surf);
	}
	
	void SimpleGraphics::drawPolygon(const std::vector<std::vector<Vec2f>>& polygons, Image* surf)
	{
		std::vector<const Vec2f*> ptrs = std::vector<const Vec2f*>(polygons.size());
		std::vector<size_t> sizeOfGons = std::vector<size_t>(polygons.size());

		for(size_t i=0; i<polygons.size(); i++)
		{
			ptrs[i] = polygons[i].data();
			sizeOfGons[i] = polygons[i].size();
		}
		drawPolygon(ptrs, sizeOfGons, surf);
	}

	void SimpleGraphics::drawPolygon(const std::vector<const Vec2f*>& polygons, const std::vector<size_t>& sizeOfEachGon, Image* surf)
	{
		if (surf == nullptr)
			return;

		if(surf->getWidth()<=0 || surf->getHeight()<=0)
			return;

		if(polygons.size() == 0 && sizeOfEachGon.size() == 0)
			return;

		//at least one of the gons must have a size > 0
		bool validGonExist = false;
		float minYFloat = INFINITY;
		float maxYFloat = -INFINITY;
		for(size_t index=0; index<polygons.size(); index++)
		{
			if(sizeOfEachGon[index] >= 3)
			{
				for(size_t i=0; i<sizeOfEachGon[index]; i++)
				{
					minYFloat = __min(polygons[index][i].y, minYFloat);
					maxYFloat = __max(polygons[index][i].y, maxYFloat);
				}
			}
		}

		//invalid points found or none of the polygons provided have a valid size. Must be a fully closed shape. Can't just be a line
		if(minYFloat == INFINITY || maxYFloat == -INFINITY)
			return;

		//create a list of Lines for each gon.
		std::vector<std::vector<Line>> allGons;
		for(size_t index=0; index<polygons.size(); index++)
		{
			if(sizeOfEachGon[index] >= 3)
			{
				allGons.push_back(std::vector<Line>(sizeOfEachGon[index]));
				for(size_t i=0; i<sizeOfEachGon[index]; i++)
				{
					size_t j = (i+1) % sizeOfEachGon[index];
					allGons.back()[i] = Line(polygons[index][i], polygons[index][j]);
				}
			}
		}

		int minY = MathExt::round(minYFloat);
		int maxY = MathExt::round(maxYFloat);
		
		//limit minY and maxY to image bounds
		minY = MathExt::clamp(minY, 0, surf->getHeight());
		maxY = MathExt::clamp(maxY, 0, surf->getHeight());

		static const float GRAPHICS_EPISLON = 1E-4;

		SET_OMP_THREADS(OMP_DESIRED_THREADS(maxY-minY, OMP_HEAVY_TASK));
		#pragma omp parallel for
		for(size_t y=minY; y<maxY; y++)
		{
			//cast horizontal line through all lines
			std::vector<PathCriticalPoint> critPoints;
			Line horizontalLine = Line(0, y, surf->getWidth(), y); //can do this too

			std::vector<std::pair<float, float>> delayedHorizontalLines;

			size_t lastNormalLine = 0;
			bool horizLine = false;
			//whenever you find a horizontal line, 
			for(auto& gon : allGons)
			{
				for(size_t i=0; i<gon.size(); i++)
				{
					Line& l = gon[i];
					//current line is horizontal
					if(l.getToPoint().y == 0)
					{
						//actual rule. If the previous line and the next line have the same y derivatives directions, ignore
						//otherwise, ignore but also ignore the last end point of the previous connected lines.

						//didn't collide with it anyway. Ignore it
						if(l.getPoint1().y != y)
							continue;
						
						delayedHorizontalLines.push_back({l.getMinX(), l.getMaxX()});
						if(getFillRule() == SimpleGraphics::FILL_NON_ZERO)
						{
							//for now, assume the last line and next line are not horizontal too
							size_t jPrev = i-1;
							if(jPrev == SIZE_MAX)
								jPrev = gon.size()-1;
							size_t jNext = (i+1) % gon.size();

							//just care about end points and their distance from our horizontal scanline
							float prevY = gon[jPrev].getPoint1().y - y;
							float nextY = gon[jNext].getPoint2().y - y;

							if(prevY*nextY < 0)
							{
								//only keep 1 point. It does not matter which one but to keep things simple, duplicate
								//the first end point and set its slope to be opposite sign of the previous slope so non-zero works
								
								PathCriticalPoint pcp;
								if(l.getToPoint().x > 0)
									pcp.xValue = l.getPoint1().x;
								else
									pcp.xValue = l.getPoint2().x;
								pcp.onVertex = true;
								pcp.ySlopeDirection = gon[jPrev].getToPoint().y <= 0; //flip slope of the last line
								pcp.v1Above = true;
								pcp.v2Above = false;

								critPoints.push_back(pcp);
							}
						}
						
						//both lie on the same side, ignore the horizontal line. (draw it separately later?)
						continue;
						
					}

					double timePoint = 0;
					if(l.getIntersectionParametric(horizontalLine, timePoint))
					{
						if(timePoint >= -GRAPHICS_EPISLON && timePoint <= 1+GRAPHICS_EPISLON)
						{
							Vec2f intersectionPoint = l.getPoint1() + l.getToPoint()*timePoint;
							PathCriticalPoint pcp;
							pcp.xValue = intersectionPoint.x;
							pcp.onVertex = timePoint <= GRAPHICS_EPISLON || timePoint >= 1-GRAPHICS_EPISLON;
							pcp.ySlopeDirection = l.getToPoint().y > 0;
							pcp.v1Above = l.getPoint1().y > y;
							pcp.v2Above = l.getPoint2().y > y;

							//push back a value that drops everything below epsilon
							// pcp.xValue = MathExt::roundToDecimal(pcp.xValue, 4);
							critPoints.push_back(pcp);
						}
					}
				}
			}

			if(critPoints.empty())
				continue;
			
			//sort critical points
			Sort::insertionSort<PathCriticalPoint>(critPoints.data(), critPoints.size(), [](const PathCriticalPoint& a, const PathCriticalPoint& b) ->bool{ return a.xValue < b.xValue; });

			//do extra parsing for even-odd rule or non-zero
			std::vector<PathCriticalPoint> newScanLinePoints = std::vector<PathCriticalPoint>();
			
			if(SimpleGraphics::getFillRule() == SimpleGraphics::FILL_EVEN_ODD)
			{
				//assuming everything was already calculated, this should be easy
				for(PathCriticalPoint& pcp : critPoints)
				{
					newScanLinePoints.push_back(pcp);
					if(pcp.onVertex)
					{
						// if(pcp.horizontalConnects)
						// 	newScanLinePoints.pop_back(); //ignore it for now
						if(pcp.v1Above == pcp.v2Above)
							newScanLinePoints.push_back(pcp);
					}
				}
			}
			else
			{
				int counter = 0;
				newScanLinePoints.push_back(critPoints.front());
				counter += (critPoints.front().ySlopeDirection) ? 1 : -1;
				for(size_t j=1; j<critPoints.size(); j++)
				{
					if(counter == 0)
						newScanLinePoints.push_back(critPoints[j]);

					counter += (critPoints[j].ySlopeDirection) ? 1 : -1;

					if(counter == 0)
						newScanLinePoints.push_back(critPoints[j]);
				}
			}

			//no need for the extra copy now. Just fill with the new points directly
			
			size_t actualSize = newScanLinePoints.size();
			if(actualSize%2 == 1)
			{
				actualSize-=1;
			}
			
			for(size_t i=0; i<actualSize; i+=2)
			{
				//fill between spots
				int startX = MathExt::clamp<int>(MathExt::floor(newScanLinePoints[i].xValue), 0, surf->getWidth());
				int endX = MathExt::clamp<int>(MathExt::ceil(newScanLinePoints[i+1].xValue), 0, surf->getWidth());
				SIMD_U32 simdColor = activeColor.toUInt();
				SimpleGraphics::fillBetween(simdColor, startX, endX, y, surf);
			}

			for(size_t i=0; i<delayedHorizontalLines.size(); i++)
			{
				//fill between spots
				int startX = MathExt::clamp<int>(MathExt::floor(delayedHorizontalLines[i].first), 0, surf->getWidth());
				int endX = MathExt::clamp<int>(MathExt::ceil(delayedHorizontalLines[i].second), 0, surf->getWidth());
				SIMD_U32 simdColor = activeColor.toUInt();
				SimpleGraphics::fillBetween(simdColor, startX, endX, y, surf);
			}
		}
		RESET_LARGE_ENOUGH_CLAUSE()
		
	}
	
	void SimpleGraphics::drawModel(Model* model, Image* texture, Image* surf)
	{
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		if (otherImg != nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}

			std::vector<VertexFormat> modelVertFormat = model->getVertexFormatInfomation();

			bool setPos = false;
			bool setTex = false;
			int startOfPosition = 0; //must be >= 0
			int startOfTexture = 0; //Can be -1 but must be >= 0 for textures to work
			
			for(int i=0; i<modelVertFormat.size(); i++)
			{
				if(modelVertFormat[i].usage == Model::USAGE_POSITION)
					if(modelVertFormat[i].type == Model::TYPE_VEC2)
						setPos = true;

				if(modelVertFormat[i].usage == Model::USAGE_TEXTURE)
					if(modelVertFormat[i].type == Model::TYPE_VEC2)
						setTex = true;

				if(!setPos)
					setPos += modelVertFormat[i].size;
				if(!setTex)
					setTex += modelVertFormat[i].size;

				if(setPos && setTex)
					break;
			}

			int amtVert = model->size();

            bool order = false;
			if(startOfPosition<0)
			{
				return;
			}

			switch (model->getModelFormat())
			{
			case Model::POINTS:
				//draw pixel at point x,y
				for(int i=0; i<amtVert; i++)
				{
					std::vector<int> vertInfo = model->getVertex(i);
					float* positionData = (float*)&vertInfo[startOfPosition];

					Vec2f pos = Vec2f( positionData[0], positionData[1] );
					
					SimpleGraphics::drawPixel(pos.x, pos.y, SimpleGraphics::activeColor, surf);
				}
				break;
			case Model::LINES:
				//draw lines at point x,y to point x2,y2
				for(int i=1; i<amtVert; i+=2)
				{
					std::vector<int> vertInfo = model->getVertex(i-1);
					std::vector<int> vertInfo2 = model->getVertex(i);

					float* positionData1 = (float*)&vertInfo[startOfPosition];
					float* positionData2 = (float*)&vertInfo2[startOfPosition];

					Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
					Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );

					SimpleGraphics::drawLine((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, surf);
				}
				break;
            case Model::LINE_STRIP:
                //draw lines at point x,y to point x2,y2
				for(int i=1; i<amtVert; i++)
				{
					std::vector<int> vertInfo = model->getVertex(i-1);
					std::vector<int> vertInfo2 = model->getVertex(i);

					float* positionData1 = (float*)&vertInfo[startOfPosition];
					float* positionData2 = (float*)&vertInfo2[startOfPosition];

					Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
					Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );

					SimpleGraphics::drawLine((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, surf);
				}
				break;
            case Model::LINE_LOOP:
                //draw lines at point x,y to point x2,y2
				for(int i=1; i<amtVert+1; i++)
				{
					std::vector<int> vertInfo = model->getVertex(i-1);
					std::vector<int> vertInfo2;
					if( i == amtVert)
						vertInfo2 = model->getVertex(0);
					else
						vertInfo2 = model->getVertex(i);

					float* positionData1 = (float*)&vertInfo[startOfPosition];
					float* positionData2 = (float*)&vertInfo2[startOfPosition];

					Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
					Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );

					SimpleGraphics::drawLine((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, surf);
				}
				break;
			case Model::TRIANGLES:
				//draw triangles at point x,y to point x2,y2 to point x3,y3
				for(int i=2; i<amtVert; i+=3)
				{
					std::vector<int> vertInfo = model->getVertex(i-2);
					std::vector<int> vertInfo2 = model->getVertex(i-1);
					std::vector<int> vertInfo3 = model->getVertex(i);

					float* positionData1 = (float*)&vertInfo[startOfPosition];
					float* positionData2 = (float*)&vertInfo2[startOfPosition];
					float* positionData3 = (float*)&vertInfo3[startOfPosition];

					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)&vertInfo[startOfTexture];
						float* textureData2 = (float*)&vertInfo2[startOfTexture];
						float* textureData3 = (float*)&vertInfo3[startOfTexture];
						
						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}
				}
				break;
			case Model::TRIANGLE_FAN:
				//draw triangles using first point to point x2,y2 to point x3,y3
				
				for(int i=2; i<amtVert; i+=1)
				{
					std::vector<int> vertInfo = model->getVertex(0);
					std::vector<int> vertInfo2 = model->getVertex(i-1);
					std::vector<int> vertInfo3 = model->getVertex(i);

					float* positionData1 = (float*)&vertInfo[startOfPosition];
					float* positionData2 = (float*)&vertInfo2[startOfPosition];
					float* positionData3 = (float*)&vertInfo3[startOfPosition];
					
					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)&vertInfo[startOfTexture];
						float* textureData2 = (float*)&vertInfo2[startOfTexture];
						float* textureData3 = (float*)&vertInfo3[startOfTexture];
						
						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}
				}
				break;
            case Model::TRIANGLE_STRIP:
                for(int i=2; i<amtVert; i++)
                {
					std::vector<int> vertInfo, vertInfo2, vertInfo3;

					if(!order)
					{
						vertInfo = model->getVertex(i-2);
						vertInfo2 = model->getVertex(i-1);
					}
					else
					{
						vertInfo = model->getVertex(i-1);
						vertInfo2 = model->getVertex(i-2);
					}

					vertInfo3 = model->getVertex(i);
					
					float* positionData1 = (float*)&vertInfo[startOfPosition];
					float* positionData2 = (float*)&vertInfo2[startOfPosition];
					float* positionData3 = (float*)&vertInfo3[startOfPosition];

					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)&vertInfo[startOfTexture];
						float* textureData2 = (float*)&vertInfo2[startOfTexture];
						float* textureData3 = (float*)&vertInfo3[startOfTexture];
						
						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}
                    order = !order;
                }

                break;
			case Model::QUADS:
				//draw 2 triangles using 4 points
				for(int i=3; i<amtVert; i+=4)
				{
					std::vector<int> vertInfo, vertInfo2, vertInfo3, vertInfo4;

					vertInfo = model->getVertex(i-3);
					vertInfo2 = model->getVertex(i-2);
					vertInfo3 = model->getVertex(i-1);
					vertInfo4 = model->getVertex(i);
					
					
					float* positionData1 = (float*)&vertInfo[startOfPosition];
					float* positionData2 = (float*)&vertInfo2[startOfPosition];
					float* positionData3 = (float*)&vertInfo3[startOfPosition];
					float* positionData4 = (float*)&vertInfo4[startOfPosition];
					
					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);

						pos = Vec2f( positionData1[0], positionData1[1] );
						pos2 = Vec2f( positionData3[0], positionData3[1] );
						pos3 = Vec2f( positionData4[0], positionData4[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)&vertInfo[startOfTexture];
						float* textureData2 = (float*)&vertInfo2[startOfTexture];
						float* textureData3 = (float*)&vertInfo3[startOfTexture];
						float* textureData4 = (float*)&vertInfo4[startOfTexture];

						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);

						pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						pos2 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );
						pos3 = Vec4f( positionData4[0], positionData4[1], textureData4[0], textureData4[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}
				}
				break;
            case Model::QUAD_STRIP:
                //draw 2 triangles using 4 points

				for(int i=3; i<amtVert; i+=2)
				{
					std::vector<int> vertInfo, vertInfo2, vertInfo3, vertInfo4;

					if(!order)
					{
						vertInfo = model->getVertex(i-3);
						vertInfo2 = model->getVertex(i-2);
						vertInfo3 = model->getVertex(i-1);
						vertInfo4 = model->getVertex(i);
					}
                    else
					{
                        vertInfo = model->getVertex(i-1);
						vertInfo2 = model->getVertex(i-2);
						vertInfo3 = model->getVertex(i-3);
						vertInfo4 = model->getVertex(i);
                    }

					float* positionData1 = (float*)&vertInfo[startOfPosition];
					float* positionData2 = (float*)&vertInfo2[startOfPosition];
					float* positionData3 = (float*)&vertInfo3[startOfPosition];
					float* positionData4 = (float*)&vertInfo4[startOfPosition];
					
					if(startOfTexture<0)
					{
						Vec2f pos = Vec2f( positionData1[0], positionData1[1] );
						Vec2f pos2 = Vec2f( positionData2[0], positionData2[1] );
						Vec2f pos3 = Vec2f( positionData3[0], positionData3[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);

						pos = Vec2f( positionData1[0], positionData1[1] );
						pos2 = Vec2f( positionData3[0], positionData3[1] );
						pos3 = Vec2f( positionData4[0], positionData4[1] );
						
						SimpleGraphics::drawTriangle((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, (int)pos3.x, (int)pos3.y, false, surf);
					}
					else
					{
						float* textureData1 = (float*)&vertInfo[startOfTexture];
						float* textureData2 = (float*)&vertInfo2[startOfTexture];
						float* textureData3 = (float*)&vertInfo3[startOfTexture];
						float* textureData4 = (float*)&vertInfo4[startOfTexture];

						Vec4f pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						Vec4f pos2 = Vec4f( positionData2[0], positionData2[1], textureData2[0], textureData2[1] );
						Vec4f pos3 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);

						pos = Vec4f( positionData1[0], positionData1[1], textureData1[0], textureData1[1] );
						pos2 = Vec4f( positionData3[0], positionData3[1], textureData3[0], textureData3[1] );
						pos3 = Vec4f( positionData4[0], positionData4[1], textureData4[0], textureData4[1] );

						SimpleGraphics::drawTexturedTriangle(pos, pos2, pos3, texture, surf);
					}

                    order = !order;
				}
                break;
			default:
				break;
			}
		}
	}

} //NAMESPACE glib END