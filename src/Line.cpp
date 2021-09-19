#include "Line.h"
#include "MathExt.h"

namespace glib
{

	const Class Line::myClass = Class("Line", {&Object::myClass});
	const Class* Line::getClass()
	{
		return &Line::myClass;
	}

	Line::Line()
	{
	}

	Line::Line(int x1, int y1, int x2, int y2)
	{
		point1.x = x1;
		point1.y = y1;
		point2.x = x2;
		point2.y = y2;

		toPoint = point2 - point1;
		setMinMax();
		setSlopeStuff();
	}

	Line::Line(double x1, double y1, double x2, double y2)
	{
		point1.x = x1;
		point1.y = y1;
		point2.x = x2;
		point2.y = y2;

		toPoint = point2 - point1;
		setMinMax();
		setSlopeStuff();
	}

	Line::Line(Vec2f p1, Vec2f p2)
	{
		point1 = p1;
		point2 = p2;

		toPoint = point2 - point1;
		setMinMax();
		setSlopeStuff();
	}

	Line::~Line()
	{
	}

	Vec2f Line::getPoint1()
	{
		return point1;
	}

	Vec2f Line::getPoint2()
	{
		return point2;
	}

	Vec2f Line::getToPoint()
	{
		return toPoint;
	}

	double Line::getMaxX()
	{
		return maxX;
	}

	double Line::getMinX()
	{
		return minX;
	}

	double Line::getMinY()
	{
		return minY;
	}

	double Line::getMaxY()
	{
		return maxY;
	}

	void Line::setMinMax()
	{
		if (point1.x >= point2.x)
		{
			maxX = point1.x;
			minX = point2.x;
		}
		else
		{
			maxX = point2.x;
			minX = point1.x;
		}

		if (point1.y >= point2.y)
		{
			maxY = point1.y;
			minY = point2.y;
		}
		else
		{
			maxY = point2.y;
			minY = point1.y;
		}
	}

	double Line::getSlope()
	{
		return slope;
	}

	double Line::getSlopeRelativeY()
	{
		return slopeRelY;
	}


	double Line::getYInt()
	{
		return yInt;
	}

	double Line::getXInt()
	{
		return xInt;
	}


	double Line::solveForX(double y)
	{
		if(toPoint.y!=0)
			return (slopeRelY*y) + xInt;
		else
			return point1.x;
	}

	double Line::solveForY(double x)
	{
		if(toPoint.x!=0)
			return (slope*x) + yInt;
		else
			return point1.y;
	}

	void Line::setSlopeStuff()
	{
		if(toPoint.x != 0)
		{
			slope = toPoint.y / toPoint.x;
			yInt = point1.y - slope*point1.x;
		}
		else
		{
			slope = 0;
			yInt = 0;
		}
		
		if(toPoint.y != 0)
		{
			slopeRelY = toPoint.x / toPoint.y;
			xInt = point1.x - slopeRelY*point1.y;
		}
		else
		{
			slopeRelY = 0;
			xInt = 0;
		}
	}

	Vec2f Line::getIntersection(Line other)
	{
		if(toPoint.x == 0)
		{
			//vertical line
			if(other.toPoint.x == 0)
			{
				//both vertical lines
				if(other.point1.x == point1.x)
				{
					//intersection
					return Vec2f(point1.x, (other.minY + maxY) / 2);
				}
				else
				{
					return Vec2f( INFINITY, INFINITY);
				}
			}
			else
			{
				double yVal = other.solveForY(point1.x);
				return Vec2f(point1.x, yVal);
			}
		}
		else
		{
			if(other.toPoint.x == 0)
			{
				//other is vertical line
				double yVal = solveForY(other.point1.x);
				return Vec2f(other.point1.x, yVal);
			}
			else
			{
				if(slope != other.slope)
				{
					double xVal = (other.yInt - yInt) / (slope-other.slope);
					double yVal = solveForY(xVal);

					return Vec2f(xVal, yVal);
				}
				else
				{
					//parallel lines or same line
					if(other.yInt == yInt)
					{
						//same line
						return point1;
					}
					else
					{
						return Vec2f( INFINITY, INFINITY); 
					}
				}

			}
		}

		return Vec2f( INFINITY, INFINITY);
	}

} //NAMESPACE glib END