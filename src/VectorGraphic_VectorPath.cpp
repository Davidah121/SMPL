#include "VectorGraphic.h"
#include "SimpleGraphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

namespace smpl
{
	bool VectorPath::oldMethod = false;
	Image* surfImg = nullptr;
	int count = 0;

	#pragma region VectorPath

	VectorPath::VectorPath() : VectorShape()
	{
	}

	VectorPath::VectorPath(const VectorPath& other) : VectorShape()
	{
		copy((VectorPath&)other);
	}

	void VectorPath::operator=(VectorPath& other)
	{
		copy(other);
	}

	void VectorPath::copy(VectorPath& other)
	{
		this->setFillColor( other.getFillColor() );
		this->setStrokeColor( other.getStrokeColor() );
		this->setFillMethod( other.getFillMethod() );
		this->setStrokeWidth( other.getStrokeWidth() );
		this->setTransform( other.getTransform() );
		this->setLineCap( other.getLineCap() );
		this->setLineJoin( other.getLineJoin() );

		commands = other.commands;
	}

	VectorPath::~VectorPath()
	{
		clear();
	}

	void VectorPath::draw(Image* img, int globalWidth, int globalHeight)
	{
		//copy to old vectorpath
		surfImg = img;
		count = 0;
		VectorPath copyVal = VectorPath(*this);

		// applyTransform();

		Vec2f currentPos;
		Vec2f extraPos;
		Vec2f closeTo;

		int minY = 0;
		int maxY = globalHeight-1;
		int yDis = 1+maxY-minY;

		if(globalHeight == 0 || globalWidth == 0)
		{
			minY = 0;
			maxY = img->getHeight()-1;
			yDis = 1+maxY-minY;
		}

		bool endOfCommands=false;
		
		PathScanLineInfo scanLineInfo = PathScanLineInfo(yDis);
		
		for(int i=0; i<commands.size(); i++)
		{
			switch(commands[i].c)
			{
				case 'M':
					//no drawing
					currentPos = commands[i].points[0];
					closeTo = currentPos;
					break;
				case 'm':
					//no drawing
					currentPos += commands[i].points[0];
					closeTo = currentPos;
					break;
				case 'L':
					//record
					drawLineTo(currentPos, commands[i], minY, maxY, scanLineInfo, false);
					currentPos = commands[i].points[0];
					break;
				case 'l':
					//record
					drawLineTo(currentPos, commands[i], minY, maxY, scanLineInfo, true);
					currentPos += commands[i].points[0];
					break;
				case 'H':
					//no drawing
					drawHorizontalTo(currentPos, commands[i], minY, maxY, scanLineInfo, false);
					// drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
					currentPos.x = commands[i].points[0].x;
					break;
				case 'h':
					//no drawing
					drawHorizontalTo(currentPos, commands[i], minY, maxY, scanLineInfo, true);
					// drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
					currentPos += commands[i].points[0];
					break;
				case 'V':
					//record
					//drawVerticalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
					drawLineTo(currentPos, commands[i], minY, maxY, scanLineInfo, false);
					currentPos.y = commands[i].points[0].y;
					break;
				case 'v':
					//record
					//drawVerticalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
					drawLineTo(currentPos, commands[i], minY, maxY, scanLineInfo, true);
					currentPos += commands[i].points[0];
					break;
				case 'Q':
					//record
					drawQuadTo(currentPos, commands[i], minY, maxY, scanLineInfo, false);
					extraPos = commands[i].points[0];
					currentPos = commands[i].points[1];
					break;
				case 'q':
					//record
					drawQuadTo(currentPos, commands[i], minY, maxY, scanLineInfo, true);
					extraPos = currentPos+commands[i].points[0];
					currentPos += commands[i].points[1];
					break;
				case 'T':
					//record
					if(commands[i-1].c == 'Q' || commands[i-1].c == 'q' || commands[i-1].c == 'T' || commands[i-1].c == 't')
					{
						extraPos = currentPos + (currentPos-extraPos);
					}
					else
					{
						extraPos = currentPos;
					}
					drawQuadShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLineInfo, false);

					currentPos = commands[i].points[0];
					break;
				case 't':
					if(commands[i-1].c == 'Q' || commands[i-1].c == 'q' || commands[i-1].c == 'T' || commands[i-1].c == 't')
					{
						extraPos = currentPos + (currentPos-extraPos);
					}
					else
					{
						extraPos = currentPos;
					}
					drawQuadShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLineInfo, true);
					
					currentPos += commands[i].points[0];
					break;
				case 'C':
					//record
					drawCubicTo(currentPos, commands[i], minY, maxY, scanLineInfo, false);
					extraPos = commands[i].points[1];
					currentPos = commands[i].points[2];
					break;
				case 'c':
					//record
					drawCubicTo(currentPos, commands[i], minY, maxY, scanLineInfo, true);
					extraPos = currentPos + commands[i].points[1];
					currentPos += commands[i].points[2];
					break;
				case 'S':
					//record, check previous index as well
					if(commands[i-1].c == 'S' || commands[i-1].c == 's' || commands[i-1].c == 'C' || commands[i-1].c == 'c')
						extraPos = currentPos + (currentPos-extraPos);
					else
						extraPos = currentPos;

					drawCubicShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLineInfo, false);
					extraPos = commands[i].points[0];
					currentPos = commands[i].points[1];
					break;
				case 's':
					//record, check previous index as well
					if(commands[i-1].c == 'S' || commands[i-1].c == 's' || commands[i-1].c == 'C' || commands[i-1].c == 'c')
						extraPos = currentPos + (currentPos-extraPos);
					else
						extraPos = currentPos;
						
					drawCubicShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLineInfo, true);
					extraPos = currentPos + commands[i].points[0];
					currentPos += commands[i].points[1];
					break;
				case 'A':
					//record. Also difficult
					drawArcTo(currentPos, commands[i], minY, maxY, scanLineInfo, false);
					currentPos = commands[i].points[2];
					break;
				case 'a':
					//record. Also difficult
					drawArcTo(currentPos, commands[i], minY, maxY, scanLineInfo, true);
					currentPos += commands[i].points[2];
					break;
				case 'Z':
					//record
					drawCloseTo(currentPos, closeTo, minY, maxY, scanLineInfo);
					currentPos = closeTo;
					break;
				default:
					endOfCommands=true;
					break;
			}

			if(endOfCommands)
				break;
		}

		for(int i=0; i<yDis; i++)
		{
			if(scanLineInfo.scanLines[i].size()>0)
			{
				//sort by xValue. Sort by end point or not
				Sort::insertionSort<PathCriticalPoint>(scanLineInfo.scanLines[i].data(), scanLineInfo.scanLines[i].size(), [](PathCriticalPoint a, PathCriticalPoint b) -> bool {return a.xValue<b.xValue;});


				std::vector<PathCriticalPoint> newScanLine = std::vector<PathCriticalPoint>();
				//may need to insert a points to handle horizontal line cases
				{
					std::vector<PathCriticalPoint> tempLines;
					int j=0;
					while(j < scanLineInfo.scanLines[i].size())
					{
						if(j == scanLineInfo.scanLines[i].size()-1)
						{
							tempLines.push_back(scanLineInfo.scanLines[i][j]);
							j++;
							break;
						}
						
						bool isConnected = false;
						for(std::pair<double, double> p : scanLineInfo.horizontalLines[i])
						{
							if(p.first == scanLineInfo.scanLines[i][j].xValue && p.second == scanLineInfo.scanLines[i][j+1].xValue)
							{
								//this is part of a horizontal line. do extra stuff
								isConnected = true;
							}
						}
						
						if(isConnected)
						{
							//do extra stuff
							if(scanLineInfo.scanLines[i][j].direction != scanLineInfo.scanLines[i][j+1].direction)
							{
								tempLines.push_back(scanLineInfo.scanLines[i][j]);
							}
							j++;
						}
						else
						{
							//add single point
							tempLines.push_back(scanLineInfo.scanLines[i][j]);
							j++;
						}
					}
					scanLineInfo.scanLines[i] = tempLines;
				}

				
				//rule, can not be the same if you are filling to it.
				//different for even-odd and non-zero

				if(getFillMethod() == SimpleGraphics::FILL_EVEN_ODD)
				{
					for(int j=1; j<scanLineInfo.scanLines[i].size(); j+=2)
					{
						//check if its apart of a horizontal line. IE, both xValues at this y value are apart of the points exactly
						if(scanLineInfo.scanLines[i][j].xValue != scanLineInfo.scanLines[i][j-1].xValue)
						{
							newScanLine.push_back(scanLineInfo.scanLines[i][j-1]);
							newScanLine.push_back(scanLineInfo.scanLines[i][j]);
						}
						else
						{
							if(scanLineInfo.scanLines[i][j].direction != scanLineInfo.scanLines[i][j-1].direction)
							{
								newScanLine.push_back(scanLineInfo.scanLines[i][j-1]);
								newScanLine.push_back(scanLineInfo.scanLines[i][j]);
							}
						}
					}
				}
				else
				{
					newScanLine.push_back(scanLineInfo.scanLines[i][0]);
					for(int j=1; j<scanLineInfo.scanLines[i].size(); j++)
					{
						if(scanLineInfo.scanLines[i][j].xValue != scanLineInfo.scanLines[i][j-1].xValue)
						{
							newScanLine.push_back(scanLineInfo.scanLines[i][j]);
						}
						else
						{
							if(scanLineInfo.scanLines[i][j].direction != scanLineInfo.scanLines[i][j-1].direction)
							{
								newScanLine.push_back(scanLineInfo.scanLines[i][j]);
							}
						}
					}
				}

				scanLineInfo.scanLines[i] = newScanLine;
			}
		}

		
		if(getFillMethod()==SimpleGraphics::FILL_EVEN_ODD)
		{
			//even-odd rule. Fill between even to odd indicies
			//not odd to even indicies
			LARGE_ENOUGH_CLAUSE(yDis)
			#pragma omp parallel for
			for(int j=0; j<yDis; j++)
			{
				int actualSize = scanLineInfo.scanLines[j].size();
				if(actualSize%2 == 1)
				{
					actualSize-=2;
				}
				else
				{
					actualSize-=1;
				}
				

				for(int i=0; i<actualSize; i+=2)
				{
					//fill between spots
					double fracStartX, fracEndX;
					int startX = MathExt::floor(scanLineInfo.scanLines[j][i].xValue);
					int endX = MathExt::ceil(scanLineInfo.scanLines[j][i+1].xValue);
					SIMD_U32 simdColor = getFillColor().toUInt();
					SimpleGraphics::fillBetween(simdColor, startX, endX, j+minY, surfImg);
				}
			}
			RESET_LARGE_ENOUGH_CLAUSE()
		}
		else
		{
			//non-zero rule. Fill when positive only. Not when zero or
			//less.
			LARGE_ENOUGH_CLAUSE(yDis)
			#pragma omp parallel for
			for(int j=0; j<yDis; j++)
			{
				int passCounter = 0;
				for(int i=0; i<scanLineInfo.scanLines[j].size()-1; i++)
				{
					//fill between spots
					int startX = MathExt::floor(scanLineInfo.scanLines[j][i].xValue);
					int endX = MathExt::ceil(scanLineInfo.scanLines[j][i+1].xValue);
					if(scanLineInfo.scanLines[j][i].direction.y > 0)
					{
						//positive y direction
						passCounter++;
					}
					else
					{
						//negative y direction
						passCounter--;
					}
					
					if(passCounter!=0)
					{
						SIMD_U32 simdColor = getFillColor().toUInt();
						SimpleGraphics::fillBetween(simdColor, startX, endX, j+minY, surfImg);
					}
				}
			}
			RESET_LARGE_ENOUGH_CLAUSE()
		}

		//strokes
		/*
		for(int j=0; j<yDis; j++)
		{
			if(scanLines[j].size()>1)
			{
				for(int i=0; i<scanLines[j].size()-1; i++)
				{
					double halfWidth = getStrokeWidth()/2;
					int startX = scanLines[j][i].xValue;
					int endX = scanLines[j][i+1].xValue;

					if(scanLines[j][i].horizontalLine==true && scanLines[j][i+1].horizontalLine==true)
					{
						int tempFillX = MathExt::ceil(startX-halfWidth);
						int tempEndX = MathExt::floor(endX+halfWidth);

						int tempYStart = MathExt::ceil(j+minY-halfWidth);
						int tempYEnd = MathExt::floor(j+minY+halfWidth);

						do
						{
							for(int y = tempYStart; y<=tempYEnd; y++)
							{
								img->drawPixel(tempFillX, y, getStrokeColor());
							}
							tempFillX++;
						} while (tempFillX <= tempEndX);
					}
					
					
					//end points stroke
					Vec2f strokeDir = MathExt::normalize(MathExt::inverseVec(scanLines[j][i].direction));
					Vec2f strokeDir2 = MathExt::normalize(MathExt::inverseVec(scanLines[j][i+1].direction));

					for(double strokeLocation=-halfWidth; strokeLocation<halfWidth; strokeLocation+=1.0)
					{
						int p1StartX = MathExt::ceil(startX+(strokeDir.x*strokeLocation));
						int p1EndX = MathExt::floor(endX+(strokeDir2.x*strokeLocation));

						int p1StartY = MathExt::ceil(j+minY+(strokeDir.y*strokeLocation));
						int p1EndY = MathExt::floor(j+minY+(strokeDir2.y*strokeLocation));

						img->drawPixel(p1StartX, p1StartY, getStrokeColor());
						img->drawPixel(p1EndX, p1EndY, getStrokeColor());
					}
				}
			}
			
		}
		*/
		copy(copyVal);
	}

	void VectorPath::drawStroke(Image* buffer, int globalWidth, int globalHeight)
	{
	}

	#pragma region PATH_DRAW_FUNCTIONS

	void VectorPath::drawLineTo(Vec2f currentPos, PathCommand command, int minY, int maxY, PathScanLineInfo& scanLineInfo, bool relative)
	{
		int actualMinY = 0;
		int actualMaxY = 0;

		BezierCurve b = BezierCurve();
		b.addPoint(currentPos);

		if(relative==false)
		{
			b.addPoint(command.points[0]);

			actualMinY = (int)MathExt::floor(MathExt::min( currentPos.y, command.points[0].y));
			actualMaxY = (int)MathExt::ceil(MathExt::max( currentPos.y, command.points[0].y));
		}
		else
		{
			b.addPoint(currentPos + command.points[0]);

			actualMinY = (int)MathExt::floor(MathExt::min( currentPos.y, currentPos.y + command.points[0].y));
			actualMaxY = (int)MathExt::ceil(MathExt::max( currentPos.y, currentPos.y + command.points[0].y));
		}

		actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
		actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

		if(actualMaxY == actualMinY)
		{
			//horizontal line
			//because it is only 2 points, we can use get simple derivative and it will be accurate.
			Vec2f dir = b.getSimpleDerivativeAt(0);
			double minX = MathExt::min(b.getPoint(0).x, b.getPoint(1).x);
			double maxX = MathExt::max(b.getPoint(0).x, b.getPoint(1).x);
			scanLineInfo.horizontalLines[actualMinY].push_back({minX, maxX});
			return;
		}

		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			std::vector<double> times = b.findTimeForY(y, true);
			for(int t=0; t<times.size(); t++)
			{
				//because it is only 2 points, we can use get simple derivative and it will be accurate.
				Vec2f v = b.getFunctionAt(times[t]);
				Vec2f dir = b.getDerivativeAt(times[t]);
				
				scanLineInfo.scanLines[y].push_back({v.x, dir});
			}
		}
	}

	void VectorPath::drawVerticalTo(Vec2f currentPos, PathCommand command, int minY, int maxY, PathScanLineInfo& scanLineInfo, bool relative)
	{
		int actualMinY, actualMaxY;
		if(!relative)
		{
			actualMinY = (int)MathExt::ceil(MathExt::min( currentPos.y, command.points[0].y));
			actualMaxY = (int)MathExt::floor(MathExt::max( currentPos.y, command.points[0].y));
		}
		else
		{
			actualMinY = (int)MathExt::ceil(MathExt::min( currentPos.y, currentPos.y+command.points[0].y));
			actualMaxY = (int)MathExt::floor(MathExt::max( currentPos.y, currentPos.y+command.points[0].y));
		}
		
		actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
		actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);
		Vec2f dir = (command.points[0].y >= 0) ? Vec2f(0,1) : Vec2f(0,-1);

		if(!relative)
		{
			for(int y=actualMinY; y<=actualMaxY; y++)
			{
				scanLineInfo.scanLines[y].push_back({currentPos.x, dir});
			}
		}
		else
		{
			for(int y=actualMinY; y<=actualMaxY; y++)
			{
				scanLineInfo.scanLines[y].push_back({currentPos.x, dir});
			}
		}
		
	}

	void VectorPath::drawHorizontalTo(Vec2f currentPos, PathCommand command, int minY, int maxY, PathScanLineInfo& scanLineInfo, bool relative)
	{
		int yVal = (int)MathExt::floor(currentPos.y);
		double minX, maxX;
		if(!relative)
		{
			minX = MathExt::min(currentPos.x, command.points[0].x);
			maxX = MathExt::max(currentPos.x, command.points[0].x);
		}
		else
		{
			minX = MathExt::min(currentPos.x, currentPos.x + command.points[0].x);
			maxX = MathExt::max(currentPos.x, currentPos.x + command.points[0].x);
		}
		scanLineInfo.horizontalLines[yVal].push_back({minX, maxX});
	}

	void VectorPath::drawQuadTo(Vec2f currentPos, PathCommand command, int minY, int maxY, PathScanLineInfo& scanLineInfo, bool relative)
	{
		BezierCurve b = BezierCurve();
		b.addPoint(currentPos);

		int actualMinY = (int)MathExt::floor(currentPos.y);
		int actualMaxY = (int)MathExt::ceil(currentPos.y);

		for(int j=0; j<2; j++)
		{
			if(!relative)
			{
				b.addPoint(command.points[j]);
				if(command.points[j].y < actualMinY)
				{
					actualMinY = (int)MathExt::floor(command.points[j].y);
				}
				if(command.points[j].y > actualMaxY)
				{
					actualMaxY = (int)MathExt::ceil(command.points[j].y);
				}
			}
			else
			{
				b.addPoint(currentPos+command.points[j]);
				if(currentPos.y+command.points[j].y < actualMinY)
				{
					actualMinY = (int)MathExt::floor(currentPos.y+command.points[j].y);
				}
				if(currentPos.y+command.points[j].y > actualMaxY)
				{
					actualMaxY = (int)MathExt::ceil(currentPos.y+command.points[j].y);
				}
			}
		}

		actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
		actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

		//check if horizontal line
		if(actualMaxY == actualMinY)
		{
			//horizontal line
			//because it is only 2 points, we can use get simple derivative and it will be accurate.
			Vec2f dir = b.getSimpleDerivativeAt(0);
			double minX = MathExt::min(b.getPoint(0).x, b.getPoint(2).x);
			double maxX = MathExt::max(b.getPoint(0).x, b.getPoint(2).x);
			scanLineInfo.horizontalLines[actualMinY].push_back({minX, maxX});
			return;
		}

		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			std::vector<double> times = b.findTimeForY(y, true);
			for(int t=0; t<times.size(); t++)
			{
				Vec2f v = b.getFunctionAt(times[t]);
				Vec2f dir = b.getDerivativeAt(times[t]);
				
				scanLineInfo.scanLines[y].push_back({v.x, dir});
			}
		}
	}

	void VectorPath::drawQuadShortTo(Vec2f currentPos, Vec2f extraPos, PathCommand command, int minY, int maxY, PathScanLineInfo& scanLineInfo, bool relative)
	{
		BezierCurve b = BezierCurve();
		b.addPoint(currentPos);

		if(extraPos!=currentPos)
		{
			b.addPoint(extraPos);
		}
		
		if(!relative)
			b.addPoint(command.points[0]);
		else
			b.addPoint(currentPos+command.points[0]);

		int actualMinY = (int)MathExt::floor(currentPos.y);
		int actualMaxY = (int)MathExt::ceil(currentPos.y);
		for(int j=0; j<b.size(); j++)
		{
			if(b.getPoint(j).y < actualMinY)
			{
				actualMinY = (int)MathExt::floor(b.getPoint(j).y);
			}
			if(b.getPoint(j).y > actualMaxY)
			{
				actualMaxY = (int)MathExt::ceil(b.getPoint(j).y);
			}
		}

		actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
		actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);
		
		//check if horizontal line
		if(actualMaxY == actualMinY)
		{
			Vec2f dir = b.getSimpleDerivativeAt(0);
			double minX = MathExt::min(b.getPoint(0).x, b.getPoint(2).x);
			double maxX = MathExt::max(b.getPoint(0).x, b.getPoint(2).x);
			scanLineInfo.horizontalLines[actualMinY].push_back({minX, maxX});
			return;
		}

		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			std::vector<double> times = b.findTimeForY(y, true);
			for(int t=0; t<times.size(); t++)
			{
				Vec2f v = b.getFunctionAt(times[t]);
				Vec2f dir = b.getDerivativeAt(times[t]);
				
				scanLineInfo.scanLines[y].push_back({v.x, dir});
			}
		}
	}

	void VectorPath::drawCubicTo(Vec2f currentPos, PathCommand command, int minY, int maxY, PathScanLineInfo& scanLineInfo, bool relative)
	{
		BezierCurve b = BezierCurve();
		b.addPoint(currentPos);

		int actualMinY = (int)MathExt::floor(currentPos.y);
		int actualMaxY = (int)MathExt::ceil(currentPos.y);

		for(int j=0; j<3; j++)
		{
			if(!relative)
			{
				b.addPoint(command.points[j]);
				if(command.points[j].y < actualMinY)
				{
					actualMinY = (int)MathExt::floor(command.points[j].y);
				}
				if(command.points[j].y > actualMaxY)
				{
					actualMaxY = (int)MathExt::ceil(command.points[j].y);
				}
			}
			else
			{
				b.addPoint(currentPos+command.points[j]);
				if(currentPos.y+command.points[j].y < actualMinY)
				{
					actualMinY = (int)MathExt::floor(currentPos.y+command.points[j].y);
				}
				if(currentPos.y+command.points[j].y > actualMaxY)
				{
					actualMaxY = (int)MathExt::ceil(currentPos.y+command.points[j].y);
				}
			}
		}

		actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
		actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

		//check if horizontal line
		if(actualMaxY == actualMinY)
		{
			//horizontal line
			//because it is only 2 points, we can use get simple derivative and it will be accurate.
			Vec2f dir = b.getSimpleDerivativeAt(0);
			double minX = MathExt::min(b.getPoint(0).x, b.getPoint(3).x);
			double maxX = MathExt::max(b.getPoint(0).x, b.getPoint(3).x);
			scanLineInfo.horizontalLines[actualMinY].push_back({minX, maxX});
			return;
		}

		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			std::vector<double> times = b.findTimeForY(y, true);
			for(int t=0; t<times.size(); t++)
			{
				Vec2f v = b.getFunctionAt(times[t]);
				Vec2f dir = b.getDerivativeAt(times[t]);
				
				scanLineInfo.scanLines[y].push_back({v.x, dir});
			}
		}
	}

	void VectorPath::drawCubicShortTo(Vec2f currentPos, Vec2f extraPos, PathCommand command, int minY, int maxY, PathScanLineInfo& scanLineInfo, bool relative)
	{
		BezierCurve b = BezierCurve();
		b.addPoint(currentPos);
		if(extraPos!=currentPos)
		{
			b.addPoint(extraPos);
		}
		if(!relative)
		{
			b.addPoint(command.points[0]);
			b.addPoint(command.points[1]);
		}
		else
		{
			b.addPoint(currentPos+command.points[0]);
			b.addPoint(currentPos+command.points[1]);
		}

		int actualMinY = (int)MathExt::floor(currentPos.y);
		int actualMaxY = (int)MathExt::floor(currentPos.y);
		for(int j=1; j<b.size(); j++)
		{
			if(b.getPoint(j).y < actualMinY)
			{
				actualMinY = (int)MathExt::floor(b.getPoint(j).y);
			}
			if(b.getPoint(j).y > actualMaxY)
			{
				actualMaxY = (int)MathExt::ceil(b.getPoint(j).y);
			}
		}

		actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
		actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);
		
		//check if horizontal line
		if(actualMaxY == actualMinY)
		{
			//horizontal line
			//because it is only 2 points, we can use get simple derivative and it will be accurate.
			Vec2f dir = b.getSimpleDerivativeAt(0);
			double minX = MathExt::min(b.getPoint(0).x, b.getPoint(3).x);
			double maxX = MathExt::max(b.getPoint(0).x, b.getPoint(3).x);
			scanLineInfo.horizontalLines[actualMinY].push_back({minX, maxX});
			return;
		}

		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			std::vector<double> times = b.findTimeForY(y, true);
			for(int t=0; t<times.size(); t++)
			{
				Vec2f v = b.getFunctionAt(times[t]);
				Vec2f dir = b.getDerivativeAt(times[t]);

				scanLineInfo.scanLines[y].push_back({v.x, dir});
			}
		}
	}

	void VectorPath::drawArcTo(Vec2f currentPos, PathCommand command, int minY, int maxY, PathScanLineInfo& scanLineInfo, bool relative)
	{
		//first, parse information from command
		Vec2f arcRadi = command.points[0];
		double arcAngle = command.points[1].x;
		Vec2f arcEndPos = command.points[2];

		if(relative)
		{
			arcEndPos = currentPos + command.points[2];
		}
		
		Vec2f dirVec = arcEndPos - currentPos;
		bool slopeDir = dirVec.y<=0;
		
		double midPointX = (currentPos.x + arcEndPos.x)/2;

		//solves arc system. Returns an array of vectors
		//arcInfo[0] is the final radi of the ellipse. If it did not need to be expanded, this will be the same
		//arcInfo[1] is one of the position for the 2 ellipses
		//arcInfo[2] is the other position for the 2 ellipses
		std::vector<Vec2f> arcInfo = getArcStuff(arcRadi, arcAngle, currentPos, arcEndPos);

		//isSweep determines whether you go clockwise or counter-clockwise
		//isLargeArc picks the larger of the 2 options you have
		Vec2f finalArcPosition;
		bool isLargeArc = command.points[1].y >= 2.0;
		bool isSweep = (command.points[1].y==1.0 || command.points[1].y==3.0);

		if(isSweep)
		{
			//counter-clockwise
			if(isLargeArc)
			{
				if(slopeDir)
				{
					if(arcInfo[1].x < midPointX)
						finalArcPosition = arcInfo[1];
					else
						finalArcPosition = arcInfo[2];
				}
				else
				{
					if(arcInfo[1].x >= midPointX)
						finalArcPosition = arcInfo[1];
					else
						finalArcPosition = arcInfo[2];
				}
			}
			else
			{
				if(slopeDir)
				{
					if(arcInfo[1].x >= midPointX)
						finalArcPosition = arcInfo[1];
					else
						finalArcPosition = arcInfo[2];
				}
				else
				{
					if(arcInfo[1].x < midPointX)
						finalArcPosition = arcInfo[1];
					else
						finalArcPosition = arcInfo[2];
				}
			}
		}
		else
		{
			//clockwise
			if(isLargeArc)
			{
				if(slopeDir)
				{
					if(arcInfo[1].x >= midPointX)
						finalArcPosition = arcInfo[1];
					else
						finalArcPosition = arcInfo[2];
				}
				else
				{
					if(arcInfo[1].x < midPointX)
						finalArcPosition = arcInfo[1];
					else
						finalArcPosition = arcInfo[2];
				}
			}
			else
			{
				if(slopeDir)
				{
					if(arcInfo[1].x < midPointX)
						finalArcPosition = arcInfo[1];
					else
						finalArcPosition = arcInfo[2];
				}
				else
				{
					if(arcInfo[1].x >= midPointX)
						finalArcPosition = arcInfo[1];
					else
						finalArcPosition = arcInfo[2];
				}
			}
		}

		int actualMinY, actualMaxY, radiY;
		double floatMinY, floatMaxY;
		
		///FIX LATER
		//Finds the min and max y values allowed for the arc.
		//Finds the angles that will produce the absolute min and max y bounds for the ellipse
		//	then clamps it down to the allowed range picking the closest angle. That angle is used
		//	to find the min and max y values. (Note that the arc angle is added to the ranges. It works for some reason.)

		double ang1 = MathExt::dirToPointDeg(finalArcPosition.x, finalArcPosition.y, currentPos.x, currentPos.y);
		double ang2 = MathExt::dirToPointDeg(finalArcPosition.x, finalArcPosition.y, arcEndPos.x, arcEndPos.y);
		
		ang1 += arcAngle;
		ang2 += arcAngle;

		double minYAngleThing, maxYAngleThing;

		double absoluteMinYAngle, absoluteMaxYAngle;

		absoluteMinYAngle = MathExt::darctan2(-arcInfo[0].y*MathExt::dcos(arcAngle), arcInfo[0].x*MathExt::dsin(arcAngle));

		absoluteMinYAngle = MathExt::angleToStandardRange(absoluteMinYAngle, true);
		absoluteMaxYAngle = MathExt::angleToStandardRange(absoluteMinYAngle + 180, true);

		minYAngleThing = MathExt::angleClampDeg(absoluteMinYAngle, ang1, ang2, !isSweep);
		maxYAngleThing = MathExt::angleClampDeg(absoluteMaxYAngle, ang1, ang2, !isSweep);
		
		floatMinY = arcInfo[0].y*MathExt::dsin(minYAngleThing)*MathExt::dcos(arcAngle) - 
				arcInfo[0].x*MathExt::dcos(minYAngleThing)*MathExt::dsin(arcAngle);

		floatMaxY = arcInfo[0].y*MathExt::dsin(maxYAngleThing)*MathExt::dcos(arcAngle) - 
			arcInfo[0].x*MathExt::dcos(maxYAngleThing)*MathExt::dsin(arcAngle);

		floatMinY += finalArcPosition.y;
		floatMaxY += finalArcPosition.y;

		if(floatMinY > floatMaxY)
		{
			double tempMin = floatMinY;
			floatMinY = floatMaxY;
			floatMaxY = tempMin;
		}
		
		actualMinY = (int)MathExt::round(floatMinY);
		actualMaxY = (int)MathExt::round(floatMaxY);
		
		actualMinY = MathExt::clamp(actualMinY, minY, maxY);
		actualMaxY = MathExt::clamp(actualMaxY, minY, maxY);

		BezierCurve separatingLine = BezierCurve();
		separatingLine.addPoint(Vec2f(MathExt::round(currentPos.x), MathExt::round(currentPos.y)));
		separatingLine.addPoint(Vec2f(MathExt::round(arcEndPos.x), MathExt::round(arcEndPos.y)));

		bool minSide = (arcEndPos - currentPos).y < 0;
		bool slopeOfSeparatingLine = !minSide;
		if(!isSweep)
		{
			minSide = !minSide;
		}

		//distance from the center of the arc to where we start.
		radiY = (int)MathExt::floor(actualMinY - finalArcPosition.y);
		arcRadi = arcInfo[0];

		double yAdjustmentMin = 1-MathExt::frac(floatMinY);
		double yAdjustmentMax = 1-MathExt::frac(floatMaxY);

		for(int y=actualMinY; y<=actualMaxY; y++, radiY++)
		{
			//get x points for the circle
			#pragma region Math_Behind_The_Constants
			// ((cos(0)*x + sin(0)*y)^2 / rx^2) + ((cos(0)*y - sin(0)*x)^2 / ry^2) = 1
			// ((c*x + A)^2 / rx^2) + ((B - s*x)^2 / ry^2) = 1
			// ((c^2*x^2 + 2cAx + A^2) / rx^2 ) + ((B^2 -2sBx + s^2*x^2) / ry^2) = 1
			// (c^2/rx^2 + s^2/ry^2)x^2 + (2cA/rx^2 - 2sB/ry^2)x + (A^2/rx^2 + B^2/ry^2 - 1) = 0
			//group into constants
			//C1 = cos^2(angle)/rx^2 + sin^2(angle)/ry^2
			//C2 = 2cos(angle)sin(angle)y/rx^2 - 2sin(angle)cos(angle)y/ry^2
			//C3 = (sin(angle)y)^2/rx^2 + (cos(angle)y)^2/ry^2 - 1
			#pragma endregion
			double sinValue, cosValue, C1, C2, C3;

			sinValue = MathExt::sin(MathExt::toRad(arcAngle));
			cosValue = MathExt::cos(MathExt::toRad(arcAngle));
			C1 = MathExt::sqr(cosValue/arcRadi.x) + MathExt::sqr(sinValue/arcRadi.y);

			C2 = 2.0*cosValue*sinValue*(radiY+0.5) * (1.0/MathExt::sqr(arcRadi.x) - 1.0/MathExt::sqr(arcRadi.y));
			C3 = MathExt::sqr( sinValue*(radiY+0.5) / arcRadi.x ) + MathExt::sqr( cosValue*(radiY+0.5) / arcRadi.y ) - 1.0;

			std::vector<double> solutions = MathExt::solveQuadraticReal(C1, C2, C3);
			if(solutions.size()==2)
			{
				//fill from min to max
				//deal with direction and large arc

				double minX = finalArcPosition.x + MathExt::min(solutions[0], solutions[1]);
				double maxX = finalArcPosition.x + MathExt::max(solutions[0], solutions[1]);
				
				bool isMinXSlopePos = false;
				bool isMaxXSlopePos = false;

				//Since we only care if the y slope is greater than 0, this shortcut can be taken.
				//For all ellipses, if the x value is less than the center point, the y slope will be negative.
				//if the x value is greater than the center point, the y slope will be positive.
				
				std::vector<double> times = separatingLine.findTimeForY(y, true);

				//TODO: FIX DIRECTION LATER
				if(times.size()>0)
				{
					double solveX = separatingLine.getFunctionAt(times[0]).x;
					if(!minSide)
					{
						if(solveX > minX)
						{
							scanLineInfo.scanLines[y].push_back({maxX, Vec2f(0,1)});
						}
						else
						{
							scanLineInfo.scanLines[y].push_back({minX, Vec2f(0,-1)});
							scanLineInfo.scanLines[y].push_back({maxX, Vec2f(0,1)});
						}
					}
					else
					{
						if(solveX < maxX)
						{
							scanLineInfo.scanLines[y].push_back({minX, Vec2f(0,-1)});
						}
						else
						{
							scanLineInfo.scanLines[y].push_back({minX, Vec2f(0,-1)});
							scanLineInfo.scanLines[y].push_back({maxX, Vec2f(0,1)});
						}
					}
					
				}
				else
				{
					scanLineInfo.scanLines[y].push_back({minX, Vec2f(0,-1)});
					scanLineInfo.scanLines[y].push_back({maxX, Vec2f(0,1)});
				}
			}
			
		}				
	}

	void VectorPath::drawCloseTo(Vec2f currentPos, Vec2f closePoint, int minY, int maxY, PathScanLineInfo& scanLineInfo)
	{
		BezierCurve b = BezierCurve();
		b.addPoint(currentPos);
		b.addPoint(closePoint);

		int actualMinY = (int)MathExt::floor(MathExt::min( currentPos.y, closePoint.y));
		int actualMaxY = (int)MathExt::ceil(MathExt::max( currentPos.y, closePoint.y));

		actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
		actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

		if(actualMaxY == actualMinY)
		{
			//horizontal line
			//because it is only 2 points, we can use get simple derivative and it will be accurate.
			Vec2f dir = b.getSimpleDerivativeAt(0);
			
			if(b.getPoint(0) != b.getPoint(1))
			{
				Vec2f dir = b.getSimpleDerivativeAt(0);
				double minX = MathExt::min(b.getPoint(0).x, b.getPoint(1).x);
				double maxX = MathExt::max(b.getPoint(0).x, b.getPoint(1).x);
				scanLineInfo.horizontalLines[actualMinY].push_back({minX, maxX});
			}
			return;
		}

		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			std::vector<double> times = b.findTimeForY(y, true);
			for(int t=0; t<times.size(); t++)
			{
				Vec2f v = b.getFunctionAt(times[t]);
				Vec2f dir = b.getDerivativeAt(times[t]);
				
				scanLineInfo.scanLines[y].push_back({v.x, dir});
			}
		}
	}
		

	#pragma endregion

	void VectorPath::addMoveTo(double x, double y)
	{
		commands.push_back( {'M', {Vec2f(x,y)} });
	}

	void VectorPath::addMoveTo(Vec2f p)
	{
		commands.push_back( {'M', {p} });
	}

	void VectorPath::addMoveToRel(double x, double y)
	{
		commands.push_back( {'m', {Vec2f(x,y)} });
	}

	void VectorPath::addMoveToRel(Vec2f p)
	{
		commands.push_back( {'m', {p} });
	}

	void VectorPath::addLineTo(double x, double y)
	{
		commands.push_back( {'L', {Vec2f(x,y)} });
	}

	void VectorPath::addLineTo(Vec2f p)
	{
		commands.push_back( {'L', {p} });
	}

	void VectorPath::addLineToRel(double x, double y)
	{
		commands.push_back( {'l', {Vec2f(x,y)} });
	}

	void VectorPath::addLineToRel(Vec2f p)
	{
		commands.push_back( {'l', {p} });
	}

	void VectorPath::addHorizontalTo(double x)
	{
		commands.push_back( {'H', {Vec2f(x,0)} });
	}

	void VectorPath::addHorizontalToRel(double x)
	{
		commands.push_back( {'h', {Vec2f(x,0)} });
	}

	void VectorPath::addVerticalTo(double y)
	{
		commands.push_back( {'V', {Vec2f(0,y)} });
	}

	void VectorPath::addVerticalToRel(double y)
	{
		commands.push_back( {'v', {Vec2f(0,y)} });
	}

	void VectorPath::addClosePath()
	{
		commands.push_back( {'Z', {} } );
	}

	void VectorPath::addQuadTo(double x1, double y1, double x2, double y2)
	{
		commands.push_back( {'Q', {Vec2f(x1,y1), Vec2f(x2,y2)} });
	}

	void VectorPath::addQuadTo(Vec2f p1, Vec2f p2)
	{
		commands.push_back( {'Q', {p1, p2} });
	}

	void VectorPath::addQuadToRel(double x1, double y1, double x2, double y2)
	{
		commands.push_back( {'q', {Vec2f(x1,y1), Vec2f(x2,y2)} });
	}

	void VectorPath::addQuadToRel(Vec2f p1, Vec2f p2)
	{
		commands.push_back( {'q', {p1, p2} });
	}

	void VectorPath::addQuadToShort(double x, double y)
	{
		commands.push_back( {'T', {Vec2f(x,y)} });
	}

	void VectorPath::addQuadToShort(Vec2f p)
	{
		commands.push_back( {'T', {p} });
	}

	void VectorPath::addQuadToShortRel(double x, double y)
	{
		commands.push_back( {'t', {Vec2f(x,y)} });
	}

	void VectorPath::addQuadToShortRel(Vec2f p)
	{
		commands.push_back( {'t', {p} });
	}

	void VectorPath::addCubicTo(double x1, double y1, double x2, double y2, double x3, double y3)
	{
		commands.push_back( {'C', {Vec2f(x1,y1), Vec2f(x2,y2), Vec2f(x3,y3)} });
	}

	void VectorPath::addCubicTo(Vec2f p1, Vec2f p2, Vec2f p3)
	{
		commands.push_back( {'C', {p1, p2, p3} });
	}

	void VectorPath::addCubicToRel(double x1, double y1, double x2, double y2, double x3, double y3)
	{
		commands.push_back( {'c', {Vec2f(x1,y1), Vec2f(x2,y2), Vec2f(x3,y3)} });
	}

	void VectorPath::addCubicToRel(Vec2f p1, Vec2f p2, Vec2f p3)
	{
		commands.push_back( {'c', {p1, p2, p3} });
	}

	void VectorPath::addCubicToShort(double x1, double y1, double x2, double y2)
	{
		commands.push_back( {'S', {Vec2f(x1,y1), Vec2f(x2,y2)} });
	}

	void VectorPath::addCubicToShort(Vec2f p1, Vec2f p2)
	{
		commands.push_back( {'S', {p1, p2} });
	}

	void VectorPath::addCubicToShortRel(double x1, double y1, double x2, double y2)
	{
		commands.push_back( {'s', {Vec2f(x1,y1), Vec2f(x2,y2)} });
	}

	void VectorPath::addCubicToShortRel(Vec2f p1, Vec2f p2)
	{
		commands.push_back( {'s', {p1, p2} });
	}

	void VectorPath::addArcTo(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y)
	{
		//pack into 3 vectors
		double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
		commands.push_back( {'A', {Vec2f(rx,ry), Vec2f(angle,comboVal), Vec2f(x,y)} });
	}

	void VectorPath::addArcTo(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p)
	{
		//pack into 3 vectors
		double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
		commands.push_back( {'A', {radi, Vec2f(angle,comboVal), p} });
	}

	void VectorPath::addArcToRel(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y)
	{
		//pack into 3 vectors
		double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
		commands.push_back( {'a', {Vec2f(rx,ry), Vec2f(angle,comboVal), Vec2f(x,y)} });
	}

	void VectorPath::addArcToRel(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p)
	{
		//pack into 3 vectors
		double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
		commands.push_back( {'a', {radi, Vec2f(angle,comboVal), p} });
	}

	void VectorPath::clear()
	{
		commands.clear();
	}

	int VectorPath::size()
	{
		return commands.size();
	}

	std::vector<Vec2f> VectorPath::getArcStuff(Vec2f radi, double angle, Vec2f p1, Vec2f p2)
	{
		//also center a point at zero to make things easier
		//p1 will be zero by default
		//first determine if the intersections make a vertical line.
		//if so, a divide by zero error can occur and requires a swap of variable.
		//both radii must not be 0.

		if(radi.x !=0 && radi.y !=0 )
		{
			Vec2f dir = p2-p1;

			double cosValue = MathExt::dcos(angle);
			double sinValue = MathExt::dsin(angle);

			double A = (MathExt::sqr( cosValue ) / MathExt::sqr(radi.x)) +  (MathExt::sqr( sinValue ) / MathExt::sqr(radi.y));
			double B = 2.0*cosValue*sinValue*(1.0/MathExt::sqr(radi.x) - 1.0/MathExt::sqr(radi.y));
			double C = (MathExt::sqr(sinValue) / MathExt::sqr(radi.x)) + (MathExt::sqr(cosValue) / MathExt::sqr(radi.y));
			
			double l1 = 2.0*A*dir.x + B*dir.y;
			double l2 = B*dir.x + 2.0*C*dir.y;
			double l3 = -(A*MathExt::sqr(dir.x)+ B*dir.x*dir.y + C*MathExt::sqr(dir.y));

			//Helps to avoid floating point errors due to very low values close to 0
			if(MathExt::abs(l1) > MathExt::abs(l2) && l1 != 0)
			{
				//solve with respect to y
				double Q = -l2/l1;
				double D = -l3/l1;

				double n1 = A*MathExt::sqr(Q) + B*Q + C;
				double n2 = 2*A*Q*D + B*D;
				double n3 = A*MathExt::sqr(D) - 1;

				//check if it can be solved. If not, the midpoint is the center of both ellipses and the radius must be expanded
				std::vector<double> solutions = MathExt::solveQuadraticReal(n1,n2,n3);
				if(solutions.size()==0)
				{
					//couldn't be solved must expand radius
					Vec2f midPoint = (p1+p2)/2;
					Vec2f solvePoint = p2-midPoint;
					double F1 = cosValue*solvePoint.x + sinValue*solvePoint.y;
					double F2 = cosValue*solvePoint.y - sinValue*solvePoint.x;
					double ratio = radi.y/radi.x;

					double finalRadius = MathExt::sqrt( MathExt::sqr(F1) + MathExt::sqr( F2/ratio ) );

					return { Vec2f(finalRadius, ratio*finalRadius), midPoint, midPoint };
				}
				else
				{
					//solved, find x values
					double x1 = Q*solutions[0] + D;
					double x2 = Q*solutions[1] + D;
					
					//The points represent how far we should move from 0,0
					//Here, the original value is added back to fix the offset that was made originally
					//for easier computation

					Vec2f newP1 = p1 + Vec2f(x1,solutions[0]);
					Vec2f newP2 = p1 + Vec2f(x2,solutions[1]);
					
					return {radi, newP1, newP2};
				}
			}
			else
			{
				//solve with respect to x
				double Q = -l1/l2;
				double D = -l3/l2;

				double n1 = A*MathExt::sqr(Q) + B*Q + C;
				double n2 = 2*A*Q*D + B*D;
				double n3 = A*MathExt::sqr(D) - 1;

				//check if it can be solved. If not, the midpoint is the center of both ellipses and the radius must be expanded
				std::vector<double> solutions = MathExt::solveQuadraticReal(n1,n2,n3);
				if(solutions.size()==0)
				{
					//couldn't be solved must expand radius
					Vec2f midPoint = (p1+p2)/2;
					Vec2f solvePoint = p2-midPoint;
					double F1 = cosValue*solvePoint.x + sinValue*solvePoint.y;
					double F2 = cosValue*solvePoint.y - sinValue*solvePoint.x;
					double ratio = radi.y/radi.x;

					double finalRadius = MathExt::sqrt( MathExt::sqr(F1) + MathExt::sqr( F2/ratio ) );

					return { Vec2f(finalRadius, ratio*finalRadius), midPoint, midPoint };
				}
				else
				{
					//solved, find y values
					double y1 = Q*solutions[0] + D;
					double y2 = Q*solutions[1] + D;
					
					//The points represent how far we should move from 0,0
					//Here, the original value is added back to fix the offset that was made originally
					//for easier computation

					Vec2f newP1 = p1 + Vec2f(solutions[0],y1);
					Vec2f newP2 = p1 + Vec2f(solutions[1],y2);
					
					return {radi, newP1, newP2};
				}
			}
		}
		
		return {};
	}

	void VectorPath::applyTransform()
	{
		Vec2f currentPos;
		for(PathCommand& com : commands)
		{
			double translationToo = 1.0;
			if( std::islower(com.c) )
				translationToo = 0.0;

			int commandVal = std::tolower(com.c);

			if(commandVal == 'm')
			{
				Vec3f pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));

				if(translationToo==1.0)
					currentPos = com.points[0];
				else
					currentPos += com.points[0];

				com.points[0] = Vec2f(pos.x, pos.y);
			}
			else if(commandVal == 'l')
			{
				Vec3f pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));

				if(translationToo==1.0)
					currentPos = com.points[0];
				else
					currentPos += com.points[0];
					
				com.points[0] = Vec2f(pos.x, pos.y);
			}
			else if(commandVal == 'h')
			{
				Vec3f pos;
				if(translationToo==1.0)
				{
					pos = (getTransform() * Vec3f( com.points[0].x, currentPos.y, translationToo ));
					currentPos.x = com.points[0].x;
				}
				else
				{
					pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
					currentPos.x += com.points[0].x;
				}
					
				com.points[0] = Vec2f(pos.x, pos.y);
				com.c = (translationToo==0.0) ? 'l' : 'L';
			}
			else if(commandVal == 'v')
			{
				Vec3f pos;
				if(translationToo==1.0)
				{
					pos = (getTransform() * Vec3f( currentPos.x, com.points[0].y, translationToo ));
					currentPos.y = com.points[0].y;
				}
				else
				{
					pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
					currentPos.y += com.points[0].y;
				}
				
				com.points[0] = Vec2f(pos.x, pos.y);
				com.c = (translationToo==0.0) ? 'l' : 'L';
			}
			else if(commandVal == 'q')
			{
				Vec3f pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
				com.points[0] = Vec2f(pos.x, pos.y);
				pos = (getTransform() * Vec3f( com.points[1].x, com.points[1].y, translationToo ));
					
				if(translationToo==1.0)
					currentPos = com.points[1];
				else
					currentPos += com.points[1];
					
				com.points[1] = Vec2f(pos.x, pos.y);
			}
			else if(commandVal == 't')
			{
				Vec3f pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
				
				if(translationToo==1.0)
					currentPos = com.points[0];
				else
					currentPos += com.points[0];

				com.points[0] = Vec2f(pos.x, pos.y);
			}
			else if(commandVal == 'c')
			{
				Vec3f pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
				com.points[0] = Vec2f(pos.x, pos.y);
				pos = (getTransform() * Vec3f( com.points[1].x, com.points[1].y, translationToo ));
				com.points[1] = Vec2f(pos.x, pos.y);
				pos = (getTransform() * Vec3f( com.points[2].x, com.points[2].y, translationToo ));

				if(translationToo==1.0)
					currentPos = com.points[2];
				else
					currentPos += com.points[2];

				com.points[2] = Vec2f(pos.x, pos.y);
			}
			else if(commandVal == 's')
			{
				Vec3f pos = (getTransform() * Vec3f( com.points[0].x, com.points[0].y, translationToo ));
				com.points[0] = Vec2f(pos.x, pos.y);
				pos = (getTransform() * Vec3f( com.points[1].x, com.points[1].y, translationToo ));
				
				if(translationToo==1.0)
					currentPos = com.points[1];
				else
					currentPos += com.points[1];

				com.points[1] = Vec2f(pos.x, pos.y);
			}
			else if(commandVal == 'a')
			{
				Mat3f transformMat = getTransform();

				double xScale = Vec2f(transformMat[0][0], transformMat[0][1]).getLength();
				double yScale = Vec2f(transformMat[1][0], transformMat[1][1]).getLength();
				Vec3f rotPos = transformMat * Vec3f(1,0,0); 
				
				com.points[0] = Vec2f(com.points[0].x*xScale, com.points[0].y*yScale);
				com.points[1] = Vec2f(MathExt::angleToStandardRange(com.points[1].x + MathExt::darctan2(rotPos.y, rotPos.x), true), com.points[1].y);
				
				Vec3f pos = (transformMat * Vec3f(com.points[2].x, com.points[2].y, translationToo));
				
				if(translationToo==1.0)
					currentPos = com.points[2];
				else
					currentPos += com.points[2];

				com.points[2] = Vec2f(pos.x, pos.y);
			}

		}
	}

	#pragma endregion

} //NAMESPACE glib END