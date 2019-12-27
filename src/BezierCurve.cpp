#include "BezierCurve.h"


const Class* BezierCurve::myClass = new Class("BezierCurve", {Object::myClass});
const Class* BezierCurve::getClass()
{
	return BezierCurve::myClass;
}


BezierCurve::BezierCurve()
{

}

BezierCurve::BezierCurve(const BezierCurve& o)
{
	points.clear();
	for (int i = 0; i < o.points.size(); i++)
	{
		points.push_back(o.points[i]);
	}
}

BezierCurve::~BezierCurve()
{
	points.clear();
}

void BezierCurve::addPoint(Vec2f p)
{
	points.push_back(p);
}

Vec2f BezierCurve::getPoint(int index)
{
	if (index >= 0 && index < points.size())
		return points[index];
	else
		return Vec2f();
}

Vec2f BezierCurve::getFuctionAt(double time)
{
	return blendPoints(0, time);
}

void BezierCurve::clear()
{
	points.clear();
}

Vec2f BezierCurve::blendPoints(int i, double time)
{
	if (i < points.size()-1)
	{
		return points[i] * (1-time) + blendPoints(i - 1, time) * time;
	}
	else
	{
		return points[i];
	}
	return Vec2f();
}
