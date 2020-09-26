#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

#pragma region VectorLine

const Class* VectorLine::myClass = new Class("VectorLine", {VectorShape::myClass});
const Class* VectorLine::getClass()
{
	return VectorLine::myClass;
}

VectorLine::VectorLine() : VectorShape()
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

void VectorLine::drawStroke(Image* buffer, int globalWidth, int globalHeight)
{
	
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