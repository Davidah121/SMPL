#include "VectorGraphic.h"
#include "SimpleGraphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

namespace glib
{
		
	#pragma region VectorLine

	const Class VectorLine::myClass = Class("VectorLine", {&VectorShape::myClass});
	const Class* VectorLine::getClass()
	{
		return &VectorLine::myClass;
	}

	VectorLine::VectorLine() : VectorShape()
	{
	}

	VectorLine::~VectorLine()
	{
	}

	void VectorLine::draw(Image* buffer, int globalWidth, int globalHeight)
	{
		
	}

	void VectorLine::drawStroke(Image* buffer, int globalWidth, int globalHeight)
	{
		//Most of the drawing has been moved to the SimpleGraphics class

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

		double preX1, preY1, preX2, preY2;
		preX1 = x1;
		preY1 = y1;
		preX2 = x2;
		preY2 = y2;

		applyTransform();
		
		if(getStrokeWidth()==1)
		{
			//method 1
			SimpleGraphics::setColor(getStrokeColor());
			buffer->drawLine(x1,y1,x2,y2);
		}
		else if(getStrokeWidth() > 1)
		{
			SimpleGraphics::setColor(getStrokeColor());
			SimpleGraphics::setFillRule(SimpleGraphics::FILL_EVEN_ODD);

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
		
		x1 = preX1;
		y1 = preY1;
		x2 = preX2;
		y2 = preY2;
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

	void VectorLine::applyTransform()
	{
		Vec3f pos1 = Vec3f(x1, y1, 1.0);
		Vec3f pos2 = Vec3f(x2, y2, 1.0);
		
		Vec3f newPos1 = getTransform() * pos1;
		Vec3f newPos2 = getTransform() * pos2;

		x1 = newPos1.x;
		y1 = newPos1.y;

		x2 = newPos2.x;
		y2 = newPos2.y;
	}

	#pragma endregion

} //NAMESPACE glib END