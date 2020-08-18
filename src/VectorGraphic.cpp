#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"


#pragma region VectorShape

const Class* VectorShape::myClass = new Class("VectorShape", {Object::myClass});
const Class* VectorShape::getClass()
{
	return VectorShape::myClass;
}

void VectorShape::draw(Image* buffer, int globalWidth, int globalHeight)
{
	std::cout << "Basic one" << std::endl;
}

void VectorShape::setFillMethod(bool value)
{
	fillMethod = value;
}

bool VectorShape::getFillMethod()
{
	return fillMethod;
}

void VectorShape::setFillColor(Color k)
{
	fill = k;
}

Color VectorShape::getFillColor()
{
	return fill;
}

void VectorShape::setStrokeColor(Color k)
{
	strokeColor = k;
}

Color VectorShape::getStrokeColor()
{
	return strokeColor;
}

void VectorShape::setStrokeWidth(double k)
{
	strokeWidth = k;
}

double VectorShape::getStrokeWidth()
{
	return strokeWidth;
}

void VectorShape::setLineCap(char c)
{
	lineCap = c;
}

char VectorShape::getLineCap()
{
	return lineCap;
}

void VectorShape::setLineJoin(char c)
{
	lineJoin = c;
}

char VectorShape::getLineJoin()
{
	return lineJoin;
}

void VectorShape::setTransform(Mat3f m)
{
	transform = m;
}

Mat3f VectorShape::getTransform()
{
	return transform;
}

#pragma endregion

#pragma region VectorRectangle

const Class* VectorRectangle::myClass = new Class("VectorRectangle", {VectorShape::myClass});
const Class* VectorRectangle::getClass()
{
	return VectorRectangle::myClass;
}

VectorRectangle::VectorRectangle()
{
}

VectorRectangle::~VectorRectangle()
{
}

void VectorRectangle::draw(Image* buffer, int globalWidth, int globalHeight)
{
	//first, calc bounding box
	int x1 = MathExt::min(x, x + width);
	int x2 = MathExt::max(x, x + width);
	int y1 = MathExt::min(y, y + height);
	int y2 = MathExt::max(y, y + height);

	x1 = MathExt::clamp(x1, 0, globalWidth);
	y1 = MathExt::clamp(y1, 0, globalHeight);
	x2 = MathExt::clamp(x2, 0, globalWidth);
	y2 = MathExt::clamp(y2, 0, globalHeight);

	//some rules: for rx and ry, there is a ellipse at each corner that
	//determines if it is in the thing.
	//The min and max x values change based on the y value.
	//They could be the x1 and x2 values but not always.

	int xCenterLeft = x+rx;
	int xCenterRight = x+width-rx;
	int yCenterTop = y+ry;
	int yCenterBottom = y+height-ry;

	for (int ty = y1; ty <= y2; ty++)
	{
		//first valid x on scanline
		int tx1 = x1;
		//last valid x on scanline
		int tx2 = x2;

		if(ty < yCenterTop)
		{
			//top corner
			// ((tx - x)^2 / rx^2) + ((ty - y)^2 / ry^2) = 1
			// (tx - x)^2 = rx^2 * (1 - ((ty - y)^2 / ry^2))
			// x = sqrt( rx^2 * (1 - ((ty - y)^2 / ry^2)) ) + tx

			int temp = MathExt::sqrt( MathExt::sqr(rx) * (1-(MathExt::sqr(ty-yCenterTop)/MathExt::sqr(ry) ) ) );
			
			//left side of rectangle
			tx1 = xCenterLeft - temp;
			//right side of rectangle
			tx2 = xCenterRight + temp;
		}
		else if(ty > yCenterBottom)
		{
			//bottom corner

			int temp = MathExt::sqrt( MathExt::sqr(rx) * (1-(MathExt::sqr(ty-yCenterBottom)/MathExt::sqr(ry) ) ) );
			
			//left side of rectangle
			tx1 = xCenterLeft - temp;
			//right side of rectangle
			tx2 = xCenterRight + temp;
		}

		for (int tx = tx1; tx <= tx2; tx++)
		{
			//first valid y on column
			int ty1 = y1;
			//last valid y on column
			int ty2 = y2;

			if(tx < xCenterLeft)
			{
				//top corner
				int temp = MathExt::sqrt( MathExt::sqr(ry) * (1-(MathExt::sqr(tx-xCenterLeft)/MathExt::sqr(rx) ) ) );
				
				//left side of rectangle
				ty1 = yCenterTop - temp;
				//right side of rectangle
				ty2 = yCenterBottom + temp;
			}
			else if(tx > xCenterRight)
			{
				//bottom corner
				int temp = MathExt::sqrt( MathExt::sqr(ry) * (1-(MathExt::sqr(tx-xCenterRight)/MathExt::sqr(rx) ) ) );
				
				//left side of rectangle
				ty1 = yCenterTop - temp;
				//right side of rectangle
				ty2 = yCenterBottom + temp;
			}
			
			if (tx >= tx1 && tx <= tx2)
			{
				if (ty >= ty1 && ty <= ty2)
				{
					//setColor(x, y, fillColor);

					//Have to determine if we are on the stroke
					bool onStroke = false;

					if ((tx2 - tx) >= 0 && (tx2 - tx) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						onStroke=true;
					}
					else if ((tx - tx1) >= 0 && (tx - tx1) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						onStroke=true;
					}
					if ((ty2 - ty) >= 0 && (ty2 - ty) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						onStroke=true;
					}
					else if ((ty - ty1) >= 0 && (ty - ty1) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						onStroke=true;
					}
					if(!onStroke)
					{
						buffer->drawPixel(tx, ty, getFillColor());
					}
					else
					{
						buffer->drawPixel(tx, ty, getStrokeColor());
					}
				}
			}
		}

	}
}

void VectorRectangle::setX(double x)
{
	this->x = x;
}

double VectorRectangle::getX()
{
	return x;
}

void VectorRectangle::setY(double y)
{
	this->y = y;
}

double VectorRectangle::getY()
{
	return y;
}

void VectorRectangle::setRX(double rx)
{
	this->rx = rx;
}

double VectorRectangle::getRX()
{
	return rx;
}

void VectorRectangle::setRY(double ry)
{
	this->ry = ry;
}

double VectorRectangle::getRY()
{
	return ry;
}

void VectorRectangle::setWidth(double width)
{
	this->width = width;
}

double VectorRectangle::getWidth()
{
	return width;
}

void VectorRectangle::setHeight(double height)
{
	this->height = height;
}

double VectorRectangle::getHeight()
{
	return height;
}

#pragma endregion

#pragma region VectorCircle

const Class* VectorCircle::myClass = new Class("VectorCircle", {VectorShape::myClass});
const Class* VectorCircle::getClass()
{
	return VectorCircle::myClass;
}

VectorCircle::VectorCircle()
{
}

VectorCircle::~VectorCircle()
{
}

void VectorCircle::draw(Image* buffer, int globalWidth, int globalHeight)
{
	//first, calc bounding box
	int x1 = cx-radius;
	int x2 = cx+radius;
	int y1 = cy-radius;
	int y2 = cy+radius;

	x1 = MathExt::clamp(x1, 0, globalWidth);
	y1 = MathExt::clamp(y1, 0, globalHeight);
	x2 = MathExt::clamp(x2, 0, globalWidth);
	y2 = MathExt::clamp(y2, 0, globalHeight);

	double radSqr = MathExt::sqr(radius);
	double strokeRad = MathExt::sqr(radius-this->getStrokeWidth());

	for(int j=y1; j<y2; j++)
	{
		for(int i=x1; i<x2; i++)
		{
			double thisPointDis = MathExt::sqr(i-cx) + MathExt::sqr(j-cy);
			if(thisPointDis<radSqr)
			{
				if(thisPointDis>strokeRad)
				{
					//stroke
					buffer->drawPixel(i, j, getStrokeColor());
				}
				else
				{
					//fill
					buffer->drawPixel(i, j, getFillColor());
				}
				
			}
		}
	}
}

void VectorCircle::setX(double x)
{
	this->cx = x;
}

double VectorCircle::getX()
{
	return cx;
}

void VectorCircle::setY(double y)
{
	this->cy = y;
}

double VectorCircle::getY()
{
	return cy;
}

void VectorCircle::setRadius(double r)
{
	this->radius = r;
}

double VectorCircle::getRadius()
{
	return this->radius;
}

#pragma endregion

#pragma region VectorLine

const Class* VectorLine::myClass = new Class("VectorLine", {VectorShape::myClass});
const Class* VectorLine::getClass()
{
	return VectorLine::myClass;
}

VectorLine::VectorLine()
{
}

VectorLine::~VectorLine()
{
}

void VectorLine::draw(Image* buffer, int globalWidth, int globalHeight)
{
	//Most of the drawing has been moved to the Graphics class

	//for dealing with stroke width, we will draw a polygon with
	//no stroke. Extending when needed. A Circle is drawn at the ends
	//when linecap round is chosen.

	//For the 3 types of linecaps:
	/**
	 * Butt
	 * 		Like with squarecap, a rectangle is drawn around
	 * 		each valid point. The line does not extend farther
	 * 		than the endpoints. Not valid when both endpoints are
	 * 		the same.
	 * Square
	 * 		Draw a square that is the size of the stroke-width/2
	 * 		around each valid point. Can extend farther than the
	 * 		endpoints. Valid when both endpoints are the same.
	 * Round
	 * 		Draw a circle that is the size of the stroke-width/2
	 * 		around each valid point. Can extend farther than the
	 * 		endpoints. Valid when both endpoints are the same.
	 */
	
	if(getStrokeWidth()==1)
	{
		//method 1
		Graphics::setColor(getStrokeColor());
		buffer->drawLine(x1,y1,x2,y2);
	}
	else if(getStrokeWidth() > 1)
	{
		Graphics::setColor(getStrokeColor());
		Graphics::setFillRule(Graphics::FILL_EVEN_ODD);

		//deal with the linecaps
		//create the 4 points
		Vec2f toEndPoint = Vec2f(x2-x1, y2-y1);
		Vec2f toEndPointScaled = MathExt::normalize(toEndPoint) * (getStrokeWidth()/2);
		
		Vec2f newPoint1 = Vec2f( x1-toEndPointScaled.y, y1+toEndPointScaled.x);
		Vec2f newPoint2 = Vec2f( x1+toEndPointScaled.y, y1-toEndPointScaled.x);
		Vec2f newPoint3 = newPoint2 + toEndPoint;
		Vec2f newPoint4 = newPoint1 + toEndPoint;

		buffer->drawPolygon(new Vec2f[4]{newPoint1, newPoint2, newPoint3, newPoint4}, 4);
	}
	
}

void VectorLine::setX1(double x)
{
	this->x1 = x;
}

double VectorLine::getX1()
{
	return x1;
}

void VectorLine::setY1(double y)
{
	this->y1 = y;
}

double VectorLine::getY1()
{
	return y1;
}

void VectorLine::setX2(double x)
{
	this->x2 = x;
}

double VectorLine::getX2()
{
	return x2;
}

void VectorLine::setY2(double y)
{
	this->y2 = y;
}

double VectorLine::getY2()
{
	return y2;
}

#pragma endregion

#pragma region VectorEllipse

const Class* VectorEllipse::myClass = new Class("VectorEllipse", {VectorShape::myClass});
const Class* VectorEllipse::getClass()
{
	return VectorEllipse::myClass;
}

VectorEllipse::VectorEllipse()
{
}

VectorEllipse::~VectorEllipse()
{
}

void VectorEllipse::draw(Image* buffer, int globalWidth, int globalHeight)
{
	//first, calc bounding box
	int x1 = cx-rx;
	int x2 = cx+rx;
	int y1 = cy-ry;
	int y2 = cy+ry;

	x1 = MathExt::clamp(x1, 0, globalWidth);
	y1 = MathExt::clamp(y1, 0, globalHeight);
	x2 = MathExt::clamp(x2, 0, globalWidth);
	y2 = MathExt::clamp(y2, 0, globalHeight);

	double radXSqr = MathExt::sqr(rx);
	double radYSqr = MathExt::sqr(ry);
	double strokeXRad = MathExt::sqr(rx-this->getStrokeWidth());
	double strokeYRad = MathExt::sqr(ry-this->getStrokeWidth());

	for(int j=y1; j<y2; j++)
	{
		for(int i=x1; i<x2; i++)
		{
			double ySide = MathExt::sqr(cy-j) / radYSqr;
			double xSide = MathExt::sqr(cx-i) / radXSqr;

			if(xSide+ySide == 1)
			{

				double yRadSide = MathExt::sqr(cy-j) / strokeXRad;
				double xRadSide = MathExt::sqr(cx-i) / strokeYRad;

				if(xRadSide+yRadSide != 1)
				{
					//stroke
					buffer->drawPixel(i, j, getStrokeColor());
				}
				else
				{
					//fill
					buffer->drawPixel(i, j, getFillColor());
				}
			}
			
		}
	}
}

void VectorEllipse::setX(double x)
{
	this->cx = x;
}

double VectorEllipse::getX()
{
	return cx;
}

void VectorEllipse::setY(double y)
{
	this->cy = y;
}

double VectorEllipse::getY()
{
	return cy;
}

void VectorEllipse::setXRadius(double r)
{
	this->rx = r;
}

double VectorEllipse::getXRadius()
{
	return this->rx;
}

void VectorEllipse::setYRadius(double r)
{
	this->ry = r;
}

double VectorEllipse::getYRadius()
{
	return this->ry;
}

#pragma endregion

#pragma region VectorPolygon

const Class* VectorPolygon::myClass = new Class("VectorPolygon", {VectorShape::myClass});
const Class* VectorPolygon::getClass()
{
	return VectorPolygon::myClass;
}

VectorPolygon::VectorPolygon()
{
}

VectorPolygon::~VectorPolygon()
{
}

void VectorPolygon::draw(Image* buffer, int globalWidth, int globalHeight)
{
	//Everything done here has been moved to the Graphics class
	//Hence, we will use that method which will be optimized
	//separately.
	Graphics::setColor(getFillColor());
	Graphics::setFillRule(getFillMethod());

	buffer->drawPolygon(points.data(), points.size());

	if(getStrokeWidth()==1)
	{
		Graphics::setColor(getStrokeColor());
		Graphics::setFillRule(Graphics::FILL_EVEN_ODD);
		for(int i=0; i<points.size(); i++)
		{
			if(i<points.size()-1)
			{
				buffer->drawLine(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
			}
			else
			{
				buffer->drawLine(points[i].x, points[i].y, points[0].x, points[0].y);
			}
		}
	}
	else if(getStrokeWidth()>1)
	{
		Graphics::setColor(getStrokeColor());
		Graphics::setFillRule(Graphics::FILL_EVEN_ODD);
		for(int i=0; i<points.size(); i++)
		{
			if(i<points.size()-1)
			{
				//create the 4 points
				Vec2f toEndPoint = Vec2f(points[i+1].x-points[i].x, points[i+1].y-points[i].y);
				Vec2f toEndPointScaled = MathExt::normalize(toEndPoint) * getStrokeWidth()/2;
				
				Vec2f newPoint1 = Vec2f( points[i].x-toEndPointScaled.y, points[i].y+toEndPointScaled.x);
				Vec2f newPoint2 = Vec2f( points[i].x+toEndPointScaled.y, points[i].y-toEndPointScaled.x);
				Vec2f newPoint3 = newPoint2 + toEndPoint;
				Vec2f newPoint4 = newPoint1 + toEndPoint;
				
				buffer->drawPolygon(new Vec2f[4]{newPoint1, newPoint2, newPoint3, newPoint4}, 4);
			}
			else
			{
				Vec2f toEndPoint = Vec2f(points[0].x-points[i].x, points[0].y-points[i].y);
				Vec2f toEndPointScaled = MathExt::normalize(toEndPoint) * getStrokeWidth()/2;
				
				Vec2f newPoint1 = Vec2f( points[i].x-toEndPointScaled.y, points[i].y+toEndPointScaled.x);
				Vec2f newPoint2 = Vec2f( points[i].x+toEndPointScaled.y, points[i].y-toEndPointScaled.x);
				Vec2f newPoint3 = newPoint2 + toEndPoint;
				Vec2f newPoint4 = newPoint1 + toEndPoint;
				
				buffer->drawPolygon(new Vec2f[4]{newPoint1, newPoint2, newPoint3, newPoint4}, 4);
			}
		}
	}
}

void VectorPolygon::addPoint(double x, double y)
{
	points.push_back(Vec2f(x,y));
}
void VectorPolygon::addPoint(Vec2f v)
{
	points.push_back(v);
}

Vec2f VectorPolygon::getPoint(int index)
{
	return points[index];
}
double VectorPolygon::getPointX(int index)
{
	return points[index].x;
}
double VectorPolygon::getPointY(int index)
{
	return points[index].y;
}

int VectorPolygon::size()
{
	return points.size();
}

#pragma endregion

#pragma region VectorPolyline

const Class* VectorPolyline::myClass = new Class("VectorPolyline", {VectorShape::myClass});
const Class* VectorPolyline::getClass()
{
	return VectorPolyline::myClass;
}

VectorPolyline::VectorPolyline()
{
}

VectorPolyline::~VectorPolyline()
{
}

void VectorPolyline::draw(Image* buffer, int globalWidth, int globalHeight)
{
	//Everything done here has been moved to the Graphics class
	//Hence, we will use that method which will be optimized
	//separately.
	//The difference here will be in how we draw the stroke.
	Graphics::setColor(getFillColor());
	Graphics::setFillRule(getFillMethod());

	buffer->drawPolygon(points.data(), points.size());

	if(getStrokeWidth()==1)
	{
		Graphics::setColor(getStrokeColor());
		Graphics::setFillRule(Graphics::FILL_EVEN_ODD);
		for(int i=0; i<points.size()-1; i++)
		{
			buffer->drawLine(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
		}
	}
	else if(getStrokeWidth()>1)
	{
		Graphics::setColor(getStrokeColor());
		Graphics::setFillRule(Graphics::FILL_EVEN_ODD);
		for(int i=0; i<points.size()-1; i++)
		{
			//create the 4 points
			Vec2f toEndPoint = Vec2f(points[i+1].x-points[i].x, points[i+1].y-points[i].y);
			Vec2f toEndPointScaled = MathExt::normalize(toEndPoint) * getStrokeWidth()/2;
			
			Vec2f newPoint1 = Vec2f( points[i].x-toEndPointScaled.y, points[i].y+toEndPointScaled.x);
			Vec2f newPoint2 = Vec2f( points[i].x+toEndPointScaled.y, points[i].y-toEndPointScaled.x);
			Vec2f newPoint3 = newPoint2 + toEndPoint;
			Vec2f newPoint4 = newPoint1 + toEndPoint;
			
			buffer->drawPolygon(new Vec2f[4]{newPoint1, newPoint2, newPoint3, newPoint4}, 4);
		}
	}
}

void VectorPolyline::addPoint(double x, double y)
{
	points.push_back(Vec2f(x,y));
}
void VectorPolyline::addPoint(Vec2f v)
{
	points.push_back(v);
}

Vec2f VectorPolyline::getPoint(int index)
{
	return points[index];
}
double VectorPolyline::getPointX(int index)
{
	return points[index].x;
}
double VectorPolyline::getPointY(int index)
{
	return points[index].y;
}

int VectorPolyline::size()
{
	return points.size();
}

#pragma endregion

#pragma region VectorPath
const Class* VectorPath::myClass = new Class("VectorPath", {VectorShape::myClass});
const Class* VectorPath::getClass()
{
	return VectorPath::myClass;
}

VectorPath::VectorPath()
{
	
}

VectorPath::~VectorPath()
{
	StringTools::println("Deleting");
	clear();
}

void VectorPath::draw(Image* img, int globalWidth, int globalHeight)
{
	Vec2f currentPos;
	Vec2f extraPos;
	Vec2f closeTo;

	int minY = 0;
	int maxY = globalHeight-1;
	int yDis = 1+maxY-minY;

	bool endOfCommands=false;
	
	std::vector<criticalPoint>* scanLines = new std::vector<criticalPoint>[yDis]();
	std::vector<int>* strokeScanLines = new std::vector<int>[yDis]();

	for(int i=0; i<commands.size(); i++)
	{
		switch(commands[i].c)
		{
			case 'M':
				//no drawing
				currentPos = *(commands[i].points);
				closeTo = currentPos;
				break;
			case 'm':
				//no drawing
				currentPos += *(commands[i].points);
				closeTo = currentPos;
				break;
			case 'L':
				//record
				drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				currentPos = *(commands[i].points);
				break;
			case 'l':
				//record
				drawLineTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				currentPos += *(commands[i].points);
				break;
			case 'H':
				//no drawing
				drawHorizontalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				currentPos.x = commands[i].points->x;
				break;
			case 'h':
				//no drawing
				drawHorizontalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				currentPos.x += commands[i].points->x;
				break;
			case 'V':
				//record
				drawVerticalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				currentPos.y = commands[i].points->y;
				break;
			case 'v':
				//record
				drawVerticalTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				currentPos.y += commands[i].points->y;
				break;
			case 'Q':
				//record
				drawQuadTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				extraPos = commands[i].points[0];
				currentPos = commands[i].points[1];
				break;
			case 'q':
				//record
				drawQuadTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
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
				drawQuadShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);

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
				drawQuadShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				
				currentPos += commands[i].points[0];
				break;
			case 'C':
				//record
				drawCubicTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				extraPos = commands[i].points[1];
				currentPos = commands[i].points[2];
				break;
			case 'c':
				//record
				drawCubicTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				extraPos = currentPos + commands[i].points[1];
				currentPos += commands[i].points[2];
				break;
			case 'S':
				//record, check previous index as well
				if(commands[i-1].c == 'S' || commands[i-1].c == 's' || commands[i-1].c == 'C' || commands[i-1].c == 'c')
					extraPos = currentPos + (currentPos-extraPos);
				else
					extraPos = currentPos;

				drawCubicShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				extraPos = commands[i].points[0];
				currentPos = commands[i].points[1];
				break;
			case 's':
				//record, check previous index as well
				if(commands[i-1].c == 'S' || commands[i-1].c == 's' || commands[i-1].c == 'C' || commands[i-1].c == 'c')
					extraPos = currentPos + (currentPos-extraPos);
				else
					extraPos = currentPos;
					
				drawCubicShortTo(currentPos, extraPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				extraPos = currentPos + commands[i].points[0];
				currentPos += commands[i].points[1];
				break;
			case 'A':
				//record. Also difficult
				drawArcTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, false);
				currentPos = commands[i].points[2];
				break;
			case 'a':
				//record. Also difficult
				drawArcTo(currentPos, commands[i], minY, maxY, scanLines, strokeScanLines, true);
				currentPos += commands[i].points[2];
				break;
			case 'Z':
				//record
				drawCloseTo(currentPos, closeTo, minY, maxY, scanLines, strokeScanLines);
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
		if(scanLines[i].size()>0)
		{
			Sort::insertionSort<criticalPoint>(scanLines[i].data(), scanLines[i].size(), [](criticalPoint a, criticalPoint b) -> bool {return a.xValue<b.xValue;});
			
			// StringTools::out << "Scanline " << i << ": ";

			// for(int k=0; k<scanLines[i].size(); k++)
			// {
			// 	StringTools::out << scanLines[i][k].xValue << ", ";
			// }
			// StringTools::println("");
			
			//rule, can not be the same if you are filling to it.
			//different for even-odd and non-zero
			std::vector<criticalPoint> newScanLine = std::vector<criticalPoint>();

			if(getFillMethod() == VectorShape::EVEN_ODD_RULE)
			{
				for(int j=1; j<scanLines[i].size(); j+=2)
				{
					criticalPoint p1 = scanLines[i][j-1];
					criticalPoint p2 = scanLines[i][j];

					if(p1.xValue != p2.xValue)
					{
						newScanLine.push_back(p1);
						newScanLine.push_back(p2);
					}
					else
					{
						bool oldDir = scanLines[i][j].isYPositive;
						bool newDir = scanLines[i][j-1].isYPositive;

						if(newDir != oldDir)
						{
							newScanLine.push_back(scanLines[i][j-1]);
							newScanLine.push_back(scanLines[i][j]);
						}
					}
				}
			}
			else
			{
				newScanLine.push_back(scanLines[i][0]);
				for(int j=1; j<scanLines[i].size(); j++)
				{
					if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
					{
						newScanLine.push_back(scanLines[i][j]);
					}
					else
					{
						bool oldDir = scanLines[i][j].isYPositive;
						bool newDir = scanLines[i][j-1].isYPositive;

						if(newDir != oldDir)
						{
							newScanLine.push_back(scanLines[i][j]);
						}
					}
				}
			}

			scanLines[i] = newScanLine;
		}

		
	}

	if(getFillMethod()==VectorShape::EVEN_ODD_RULE)
	{
		//even-odd rule. Fill between even to odd indicies
		//not odd to even indicies
		for(int j=0; j<yDis; j++)
		{
			if(scanLines[j].size()>1)
			{
				int actualSize = scanLines[j].size();
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
					double fracStartX = 1-MathExt::frac(scanLines[j][i].xValue);
					double fracEndX = MathExt::frac(scanLines[j][i+1].xValue);
					
					int startX = MathExt::ceil(scanLines[j][i].xValue);
					int endX = MathExt::floor(scanLines[j][i+1].xValue);

					int fillX = startX;
					Color middleCol = getFillColor();
					Color startLineCol = middleCol;
					Color endLineCol = middleCol;
					startLineCol.alpha = (unsigned char)(startLineCol.alpha*fracStartX);
					endLineCol.alpha = (unsigned char)(endLineCol.alpha*fracEndX);
					
					img->drawPixel(startX, j+minY, startLineCol);
					fillX++;
					while(fillX < endX)
					{
						img->drawPixel(fillX, j+minY, middleCol);
						fillX++;
					}
					img->drawPixel(endX, j+minY, endLineCol);
				}
			}
		}
		
	}
	else
	{
		//non-zero rule. Fill when positive only. Not when zero or
		//less.
		for(int j=0; j<yDis; j++)
		{
			int passCounter = 0;
			if(scanLines[j].size()>1)
			{
				for(int i=0; i<scanLines[j].size()-1; i++)
				{
					//fill between spots
					double fracStartX = MathExt::frac(scanLines[j][i].xValue);
					double fracEndX = MathExt::frac(scanLines[j][i+1].xValue);
					
					int startX = MathExt::round(scanLines[j][i].xValue);
					int endX = MathExt::round(scanLines[j][i+1].xValue);

					bool dir = scanLines[j][i].isYPositive;

					if(dir == true)
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
						int fillX = startX;
						Color middleCol = getFillColor();
						Color startLineCol = middleCol;
						Color endLineCol = middleCol;
						startLineCol.alpha = (unsigned char)(startLineCol.alpha*fracStartX);
						endLineCol.alpha = (unsigned char)(endLineCol.alpha*fracEndX);
						
						img->drawPixel(startX, j+minY, startLineCol);
						fillX++;
						while(fillX < endX)
						{
							img->drawPixel(fillX, j+minY, middleCol);
							fillX++;
						}
						img->drawPixel(endX, j+minY, endLineCol);
					}
				}
			}
		}
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
}

#pragma region PATH_DRAW_FUNCTIONS

void VectorPath::drawLineTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	int actualMinY = 0;
	int actualMaxY = 0;

	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);

	if(relative==false)
	{
		b.addPoint(*(command.points));

		actualMinY = MathExt::ceil(MathExt::min( currentPos.y, command.points->y));
		actualMaxY = MathExt::floor(MathExt::max( currentPos.y, command.points->y));
	}
	else
	{
		b.addPoint(currentPos + *(command.points));

		actualMinY = MathExt::ceil(MathExt::min( currentPos.y, currentPos.y + command.points->y));
		actualMaxY = MathExt::floor(MathExt::max( currentPos.y, currentPos.y + command.points->y));
	}
	
	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times;
		//The line is not a horizontal line. Scanlines are handled separately
		if(actualMinY!=actualMaxY)
			times = b.findTimeForY(y);
		
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			//because it is only 2 points, we can use get simple derivative and it will be accurate.
			Vec2f dir = b.getSimpleDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}

	//Stroke drawing
	if(getStrokeWidth()>0)
	{
		double halfStrokeWidth = getStrokeWidth()/2;
		BezierCurve strokeCurve1 = BezierCurve();
		BezierCurve strokeCurve2 = BezierCurve();
		BezierCurve strokeCurveConn1 = BezierCurve();
		BezierCurve strokeCurveConn2 = BezierCurve();
		Vec2f invVec = MathExt::normalize(MathExt::inverseVec(b.getSimpleDerivativeAt(0))) * halfStrokeWidth;

		strokeCurve1.addPoint( b.getPoint(0) + invVec );
		strokeCurve2.addPoint( b.getPoint(0) - invVec );

		strokeCurve1.addPoint( b.getPoint(1) + invVec );
		strokeCurve2.addPoint( b.getPoint(1) - invVec );

		strokeCurveConn1.addPoint( strokeCurve1.getPoint(0) );
		strokeCurveConn1.addPoint( strokeCurve2.getPoint(0) );

		strokeCurveConn2.addPoint( strokeCurve1.getPoint(1) );
		strokeCurveConn2.addPoint( strokeCurve2.getPoint(1) );
		
		actualMinY = MathExt::ceil(MathExt::min( strokeCurve1.getPoint(0).y, strokeCurve1.getPoint(1).y));
		actualMaxY = MathExt::floor(MathExt::max( strokeCurve1.getPoint(0).y, strokeCurve1.getPoint(1).y));
	}
}

void VectorPath::drawVerticalTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	int actualMinY, actualMaxY;
	if(!relative)
	{
		actualMinY = MathExt::ceil(MathExt::min( currentPos.y, command.points->y));
		actualMaxY = MathExt::floor(MathExt::max( currentPos.y, command.points->y));
	}
	else
	{
		actualMinY = MathExt::ceil(MathExt::min( currentPos.y, currentPos.y+command.points->y));
		actualMaxY = MathExt::floor(MathExt::max( currentPos.y, currentPos.y+command.points->y));
	}
	
	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	if(!relative)
	{
		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			scanLines[y].push_back({currentPos.x, (command.points->y - currentPos.y) >0, 1});
		}
	}
	else
	{
		for(int y=actualMinY; y<=actualMaxY; y++)
		{
			scanLines[y].push_back({currentPos.x, command.points->y >0, 1});
		}
	}
	
}

void VectorPath::drawHorizontalTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	if(getStrokeWidth()>0)
	{
		double halfStrokeWidth = getStrokeWidth()/2;
		//stroke stuff
		int actualMinY = MathExt::clamp((int)MathExt::ceil(currentPos.y-halfStrokeWidth ), minY, maxY);
		int actualMaxY = MathExt::clamp((int)MathExt::floor(currentPos.y+halfStrokeWidth ), minY, maxY);

		if(!relative)
		{
			for(int y=actualMinY; y<=actualMaxY; y++)
			{
				strokeScanLines[y].push_back((int)MathExt::round(currentPos.x));
				strokeScanLines[y].push_back((int)MathExt::round(command.points->x));
			}
		}
		else
		{
			for(int y=actualMinY; y<=actualMaxY; y++)
			{
				strokeScanLines[y].push_back((int)MathExt::round(currentPos.x));
				strokeScanLines[y].push_back((int)MathExt::round(currentPos.x+command.points->x));
			}
		}
	}
}

void VectorPath::drawQuadTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);

	int actualMinY = MathExt::ceil(currentPos.y);
	int actualMaxY = MathExt::floor(currentPos.y);

	for(int j=0; j<2; j++)
	{
		if(!relative)
		{
			b.addPoint(command.points[j]);
			if(command.points[j].y < actualMinY)
			{
				actualMinY = MathExt::ceil(command.points[j].y);
			}
			if(command.points[j].y > actualMaxY)
			{
				actualMaxY = MathExt::floor(command.points[j].y);
			}
		}
		else
		{
			b.addPoint(currentPos+command.points[j]);
			if(currentPos.y+command.points[j].y < actualMinY)
			{
				actualMinY = MathExt::ceil(currentPos.y+command.points[j].y);
			}
			if(currentPos.y+command.points[j].y > actualMaxY)
			{
				actualMaxY = MathExt::floor(currentPos.y+command.points[j].y);
			}
		}
	}

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y >0, 1});
		}
	}
}

void VectorPath::drawQuadShortTo(Vec2f currentPos, Vec2f extraPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
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

	int actualMinY = MathExt::ceil(currentPos.y);
	int actualMaxY = MathExt::floor(currentPos.y);
	for(int j=0; j<b.size(); j++)
	{
		if(b.getPoint(j).y < actualMinY)
		{
			actualMinY = MathExt::ceil(b.getPoint(j).y);
		}
		if(b.getPoint(j).y > actualMaxY)
		{
			actualMaxY = MathExt::floor(b.getPoint(j).y);
		}
	}

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);
	
	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}
}

void VectorPath::drawCubicTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);

	int actualMinY = MathExt::ceil(currentPos.y);
	int actualMaxY = MathExt::floor(currentPos.y);

	for(int j=0; j<3; j++)
	{
		if(!relative)
		{
			b.addPoint(command.points[j]);
			if(command.points[j].y < actualMinY)
			{
				actualMinY = MathExt::ceil(command.points[j].y);
			}
			if(command.points[j].y > actualMaxY)
			{
				actualMaxY = MathExt::floor(command.points[j].y);
			}
		}
		else
		{
			b.addPoint(currentPos+command.points[j]);
			if(currentPos.y+command.points[j].y < actualMinY)
			{
				actualMinY = MathExt::ceil(currentPos.y+command.points[j].y);
			}
			if(currentPos.y+command.points[j].y > actualMaxY)
			{
				actualMaxY = MathExt::floor(currentPos.y+command.points[j].y);
			}
		}
	}

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}
}

void VectorPath::drawCubicShortTo(Vec2f currentPos, Vec2f extraPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
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

	int actualMinY = currentPos.y;
	int actualMaxY = currentPos.y;
	for(int j=1; j<b.size(); j++)
	{
		if(b.getPoint(j).y < actualMinY)
		{
			actualMinY = MathExt::ceil(b.getPoint(j).y);
		}
		if(b.getPoint(j).y > actualMaxY)
		{
			actualMaxY = MathExt::floor(b.getPoint(j).y);
		}
	}

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}
}

void VectorPath::drawArcTo(Vec2f currentPos, PathCommand command, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines, bool relative)
{
	//first, parse information from command
	Vec2f arcRadi = command.points[0];
	double arcAngle = command.points[1].x;
	Vec2f arcEndPos = command.points[2];

	if(relative)
	{
		arcEndPos += currentPos;
	}

	double xDir = arcEndPos.x - currentPos.x;
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
			if(xDir>0)
			{
				if(arcInfo[1].x > midPointX)
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
			if(xDir>0)
			{
				if(arcInfo[1].x < midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
			else
			{
				if(arcInfo[1].x > midPointX)
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
			if(xDir>0)
			{
				if(arcInfo[1].x < midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
			else
			{
				if(arcInfo[1].x > midPointX)
					finalArcPosition = arcInfo[1];
				else
					finalArcPosition = arcInfo[2];
			}
		}
		else
		{
			if(xDir>0)
			{
				if(arcInfo[1].x > midPointX)
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

	int actualMinY, actualMaxY, radiY;

	//check if both solutions are the same
	if(arcInfo[1] == arcInfo[2])
	{
		//will always be a large arc
		if(isSweep)
		{
			actualMinY = (int)MathExt::ceil(finalArcPosition.y-MathExt::max(arcInfo[0].y, arcInfo[0].x));
			actualMaxY = (int)MathExt::floor(MathExt::max(currentPos.y, arcEndPos.y));
		}
		else
		{
			actualMinY = (int)MathExt::ceil(MathExt::min(currentPos.y, arcEndPos.y));
			actualMaxY = (int)MathExt::floor(finalArcPosition.y+MathExt::max(arcInfo[0].y, arcInfo[0].x));
		}
	}
	else
	{
		if(!isLargeArc)
		{
			actualMinY = (int)MathExt::ceil(MathExt::min(currentPos.y, arcEndPos.y));
			actualMaxY = (int)MathExt::floor(MathExt::max(currentPos.y, arcEndPos.y));
		}
		else
		{
			actualMinY = (int)MathExt::ceil(finalArcPosition.y-MathExt::max(arcInfo[0].y, arcInfo[0].x));
			actualMaxY = (int)MathExt::floor(finalArcPosition.y+MathExt::max(arcInfo[0].y, arcInfo[0].x));
		}
	}

	actualMinY = MathExt::clamp(actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp(actualMaxY, minY, maxY);

	BezierCurve separatingLine = BezierCurve();
	separatingLine.addPoint(currentPos);
	separatingLine.addPoint(arcEndPos);

	bool minSide = (arcEndPos - currentPos).y < 0;
	bool slopeOfSeparatingLine = !minSide;
	if(!isSweep)
	{
		minSide = !minSide;
	}

	//distance from the center of the arc to where we start.
	radiY = actualMinY - finalArcPosition.y;

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

		double sinValue = MathExt::sin(MathExt::toRad(arcAngle));
		double cosValue = MathExt::cos(MathExt::toRad(arcAngle));
		double C1 = MathExt::sqr(cosValue/arcRadi.x) + MathExt::sqr(sinValue/arcRadi.y);
		double C2 = 2.0*cosValue*sinValue*radiY * (1.0/MathExt::sqr(arcRadi.x) - 1.0/MathExt::sqr(arcRadi.y));
		double C3 = MathExt::sqr( sinValue*radiY / arcRadi.x ) + MathExt::sqr( cosValue*radiY / arcRadi.y ) - 1.0;

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
			
			std::vector<double> times = separatingLine.findTimeForY(y);

			if(minSide)
			{
				//change max x
				bool isPos = minX>finalArcPosition.x;
				
				if(times.size() > 0)
				{
					double solveX = separatingLine.getFuctionAt(times[0]).x;

					if((int)MathExt::ceil(minX) < solveX)
					{
						scanLines[y].push_back({minX, isPos, 1});
						scanLines[y].push_back({solveX, !isPos, 1});
					}
					else
					{
						scanLines[y].push_back({solveX, isPos, 1});
						scanLines[y].push_back({solveX, !isPos, 1});
					}
				}
				else
				{
					scanLines[y].push_back({minX, isPos, 1});
					scanLines[y].push_back({maxX, !isPos, 1});
				}
			}
			else
			{
				//change min x
				bool isPos = maxX>finalArcPosition.x;

				if(times.size() > 0)
				{
					double solveX = separatingLine.getFuctionAt(times[0]).x;

					if((int)MathExt::ceil(maxX) > solveX)
					{
						scanLines[y].push_back({maxX, isPos, 1});
						scanLines[y].push_back({solveX, !isPos, 1});
					}
					else
					{
						scanLines[y].push_back({solveX, isPos, 1});
						scanLines[y].push_back({solveX, !isPos, 1});
					}
				}
				else
				{
					scanLines[y].push_back({maxX, isPos, 1});
					scanLines[y].push_back({minX, !isPos, 1});
				}
			}
			
			/*
			if(!isSweep)
			{
				//change max x
				scanLines[y].push_back({(int)MathExt::ceil(minX), minX>finalArcPosition.x});
				if(times.size() > 0)
				{
					//maxX = MathExt::max(maxX, separatingLine.getFuctionAt(times[0]).x);
				}
				else
				{
					scanLines[y].push_back({(int)MathExt::floor(maxX), maxX>finalArcPosition.x});
				}
			}
			else
			{
				//change min x
				scanLines[y].push_back({(int)MathExt::floor(maxX), maxX>finalArcPosition.x});
				if(times.size() > 0)
				{
					//minX = MathExt::min(minX, separatingLine.getFuctionAt(times[0]).x);
				}
				else
				{
					scanLines[y].push_back({(int)MathExt::ceil(minX), minX>finalArcPosition.x});
				}
			}
			*/
			
		}
		else
		{
			//Outside of the bounds of the ellipse. Do nothing.
		}
	}				
}

void VectorPath::drawCloseTo(Vec2f currentPos, Vec2f closePoint, int minY, int maxY, std::vector<criticalPoint>* scanLines, std::vector<int>* strokeScanLines)
{
	BezierCurve b = BezierCurve();
	b.addPoint(currentPos);
	b.addPoint(closePoint);

	int actualMinY = MathExt::ceil(MathExt::min( currentPos.y, closePoint.y));
	int actualMaxY = MathExt::floor(MathExt::max( currentPos.y, closePoint.y));

	actualMinY = MathExt::clamp((int)actualMinY, minY, maxY);
	actualMaxY = MathExt::clamp((int)actualMaxY, minY, maxY);

	for(int y=actualMinY; y<=actualMaxY; y++)
	{
		std::vector<double> times = b.findTimeForY(y);
		for(int t=0; t<times.size(); t++)
		{
			Vec2f v = b.getFuctionAt(times[t]);
			Vec2f dir = b.getSimpleDerivativeAt(times[t]);
			
			scanLines[y].push_back({v.x, dir.y>0, 1});
		}
	}
}
	

#pragma endregion

void VectorPath::addMoveTo(double x, double y)
{
	commands.push_back( {'M', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addMoveTo(Vec2f p)
{
	commands.push_back( {'M', new Vec2f[1]{p} });
}

void VectorPath::addMoveToRel(double x, double y)
{
	commands.push_back( {'m', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addMoveToRel(Vec2f p)
{
	commands.push_back( {'m', new Vec2f[1]{p} });
}

void VectorPath::addLineTo(double x, double y)
{
	commands.push_back( {'L', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addLineTo(Vec2f p)
{
	commands.push_back( {'L', new Vec2f[1]{p} });
}

void VectorPath::addLineToRel(double x, double y)
{
	commands.push_back( {'l', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addLineToRel(Vec2f p)
{
	commands.push_back( {'l', new Vec2f[1]{p} });
}

void VectorPath::addHorizontalTo(double x)
{
	commands.push_back( {'H', new Vec2f[1]{Vec2f(x,0)} });
}

void VectorPath::addHorizontalToRel(double x)
{
	commands.push_back( {'h', new Vec2f[1]{Vec2f(x,0)} });
}

void VectorPath::addVerticalTo(double y)
{
	commands.push_back( {'V', new Vec2f[1]{Vec2f(0,y)} });
}

void VectorPath::addVerticalToRel(double y)
{
	commands.push_back( {'v', new Vec2f[1]{Vec2f(0,y)} });
}

void VectorPath::addClosePath()
{
	commands.push_back( {'Z', nullptr});
}

void VectorPath::addQuadTo(double x1, double y1, double x2, double y2)
{
	commands.push_back( {'Q', new Vec2f[2]{Vec2f(x1,y1), Vec2f(x2,y2)} });
}

void VectorPath::addQuadTo(Vec2f p1, Vec2f p2)
{
	commands.push_back( {'Q', new Vec2f[2]{p1, p2} });
}

void VectorPath::addQuadToRel(double x1, double y1, double x2, double y2)
{
	commands.push_back( {'q', new Vec2f[2]{Vec2f(x1,y1), Vec2f(x2,y2)} });
}

void VectorPath::addQuadToRel(Vec2f p1, Vec2f p2)
{
	commands.push_back( {'q', new Vec2f[2]{p1, p2} });
}

void VectorPath::addQuadToShort(double x, double y)
{
	commands.push_back( {'T', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addQuadToShort(Vec2f p)
{
	commands.push_back( {'T', new Vec2f[1]{p} });
}

void VectorPath::addQuadToShortRel(double x, double y)
{
	commands.push_back( {'t', new Vec2f[1]{Vec2f(x,y)} });
}

void VectorPath::addQuadToShortRel(Vec2f p)
{
	commands.push_back( {'t', new Vec2f[1]{p} });
}

void VectorPath::addCubicTo(double x1, double y1, double x2, double y2, double x3, double y3)
{
	commands.push_back( {'C', new Vec2f[3]{Vec2f(x1,y1), Vec2f(x2,y2), Vec2f(x3,y3)} });
}

void VectorPath::addCubicTo(Vec2f p1, Vec2f p2, Vec2f p3)
{
	commands.push_back( {'C', new Vec2f[3]{p1, p2, p3} });
}

void VectorPath::addCubicToRel(double x1, double y1, double x2, double y2, double x3, double y3)
{
	commands.push_back( {'c', new Vec2f[3]{Vec2f(x1,y1), Vec2f(x2,y2), Vec2f(x3,y3)} });
}

void VectorPath::addCubicToRel(Vec2f p1, Vec2f p2, Vec2f p3)
{
	commands.push_back( {'c', new Vec2f[3]{p1, p2, p3} });
}

void VectorPath::addCubicToShort(double x1, double y1, double x2, double y2)
{
	commands.push_back( {'S', new Vec2f[2]{Vec2f(x1,y1), Vec2f(x2,y2)} });
}

void VectorPath::addCubicToShort(Vec2f p1, Vec2f p2)
{
	commands.push_back( {'S', new Vec2f[2]{p1, p2} });
}

void VectorPath::addCubicToShortRel(double x1, double y1, double x2, double y2)
{
	commands.push_back( {'s', new Vec2f[2]{Vec2f(x1,y1), Vec2f(x2,y2)} });
}

void VectorPath::addCubicToShortRel(Vec2f p1, Vec2f p2)
{
	commands.push_back( {'s', new Vec2f[2]{p1, p2} });
}

void VectorPath::addArcTo(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y)
{
	//pack into 3 vectors
	double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
	commands.push_back( {'A', new Vec2f[3]{Vec2f(rx,ry), Vec2f(angle,comboVal), Vec2f(x,y)} });
}

void VectorPath::addArcTo(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p)
{
	//pack into 3 vectors
	double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
	commands.push_back( {'A', new Vec2f[3]{radi, Vec2f(angle,comboVal), p} });
}

void VectorPath::addArcToRel(double rx, double ry, double angle, bool largeArcFlag, bool sweepFlag, double x, double y)
{
	//pack into 3 vectors
	double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
	commands.push_back( {'a', new Vec2f[3]{Vec2f(rx,ry), Vec2f(angle,comboVal), Vec2f(x,y)} });
}

void VectorPath::addArcToRel(Vec2f radi, double angle, bool largeArcFlag, bool sweepFlag, Vec2f p)
{
	//pack into 3 vectors
	double comboVal = (double)largeArcFlag*2 + (double)sweepFlag; //0.0=0,0 - 1.0=0,1 - 2.0=1,0 - 3.0=1,1
	commands.push_back( {'a', new Vec2f[3]{radi, Vec2f(angle,comboVal), p} });
}

void VectorPath::clear()
{
	for(int i=0; i<commands.size(); i++)
	{
		if(commands[i].points!=nullptr)
			delete[] commands[i].points;
	}
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

		if(l1!=0)
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

#pragma endregion

#pragma region VectorGraphic

const Class* VectorGraphic::myClass = new Class("VectorGraphic", {Object::myClass});
const Class* VectorGraphic::getClass()
{
	return VectorGraphic::myClass;
}

VectorGraphic::VectorGraphic(int width, int height)
{
	this->width = width;
	this->height = height;

	if(width!=0 && height!=0)
	{
		buffer = new Image(width, height);
	}
	//invalid otherwise and will morph to the image it will be drawn upon.
}

VectorGraphic::VectorGraphic(const VectorGraphic& c)
{
	this->width = c.width;
	this->height = c.height;

	if(width!=0 && height!=0)
	{
		buffer = new Image(width, height);
	}

	for(int i=0; i<c.shapes.size(); i++)
	{
		shapes.push_back(c.shapes[i]);
	}
}
VectorGraphic::~VectorGraphic()
{
	clearShapes();
	if(buffer!=nullptr)
		delete buffer;
}

void VectorGraphic::addShape(VectorShape* k)
{
	shapes.push_back(k);
}

void VectorGraphic::clearShapes()
{
	shapes.clear();
}

void VectorGraphic::draw()
{
	if(buffer!=nullptr)
	{
		
		Mat3f globalTransform = viewBox * transform;
		Mat3f temp = Mat3f::getIdentity();
		Mat3f finalTransform;

		
		for (int i = 0; i < shapes.size(); i++)
		{
			temp = shapes[i]->getTransform();

			finalTransform = globalTransform;

			shapes[i]->setTransform(finalTransform);

			shapes[i]->draw(this->buffer, this->width, this->height);

			shapes[i]->setTransform(temp);
			
		}
		
	}
}

void VectorGraphic::draw(Image* buffer)
{
	Mat3f globalTransform = viewBox * transform;

	for (int i = 0; i < shapes.size(); i++)
	{
		Mat3f temp = shapes[i]->getTransform();

		Mat3f finalTransform = globalTransform * temp;
		shapes[i]->setTransform(finalTransform);
		
		shapes[i]->draw(buffer, this->width, this->height);

		shapes[i]->setTransform(temp);
	}
}

void VectorGraphic::setTransform(Mat3f m)
{
	transform = m;
}

Mat3f VectorGraphic::getTransform()
{
	return transform;
}

void VectorGraphic::setViewBox(Mat3f m)
{
	viewBox = m;
}

Mat3f VectorGraphic::getViewBox()
{
	return viewBox;
}

Image* VectorGraphic::getImageBuffer()
{
	return buffer;
}

#pragma endregion


