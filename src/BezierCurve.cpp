#include "BezierCurve.h"
#include "MathExt.h"
#include "StringTools.h"

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
	//recursive definition that takes n^2 time
	//blend P[n] with P[n+1] to form C[n]
	//blend C[n] with C[n+1] to form B[n]
	//explicit definition takes approximately n^2 time as well
		//binomialCoefficient takes n time
		//called n times

	return blendPointsRecursive(0, points.size()-1, time);
}

Vec2f BezierCurve::getDerivativeAt(double time)
{
	return blendPointsDerivativeRecursive(0, points.size()-1, time);
}

Vec2f BezierCurve::getSimpleDerivativeAt(double time)
{
	//assume the points are separated by time segments
	//here, we just return the rate of change to the next time segment.
	//useful only when determining when the average rate switches
	
	int segments = points.size()-1;
	int currentSegment = (int)(time * segments);

	if(time==1)
	{
		currentSegment-=1;
	}
	return points[currentSegment+1] - points[currentSegment];
}

void BezierCurve::clear()
{
	points.clear();
}

int BezierCurve::size()
{
	return points.size();
}

std::vector<double> BezierCurve::findTimeForY(double Y)
{
	std::vector<double> timeValues = std::vector<double>();
	double A,B,C,D;
	switch (points.size())
	{
	case 0:
		//no points
		break;
	case 1:
		//only one point
		if(points[0].y == Y)
		{
			timeValues.push_back(0);
		}
		break;
	case 2:
		//linear
		//Ax + B = 0
		A = points[1].y - points[0].y;
		B = points[0].y - Y;
		timeValues = MathExt::solveLinear(A, B);
		break;
	case 3:
		//quadratic
		//Ax^2 + Bx + C = 0
		A = points[0].y - 2*points[1].y + points[2].y;
		B = -2*points[0].y + 2*points[1].y;
		C = points[0].y - Y;
		A = MathExt::roundToDecimal(A, 10);
		B = MathExt::roundToDecimal(B, 10);
		C = MathExt::roundToDecimal(C, 10);
		timeValues = MathExt::solveQuadraticReal(A,B,C);
		break;
	case 4:
		//cubic
		//Ax^3 + Bx^2 + Cx + D = 0
		A = -points[0].y + 3*points[1].y - 3*points[2].y + points[3].y;
		B = 3*points[0].y - 6*points[1].y + 3*points[2].y;
		C = -3*points[0].y + 3*points[1].y;
		D = points[0].y - Y;
		A = MathExt::roundToDecimal(A, 10);
		B = MathExt::roundToDecimal(B, 10);
		C = MathExt::roundToDecimal(C, 10);
		D = MathExt::roundToDecimal(D, 10);
		timeValues = MathExt::solveCubicReal(A,B,C,D);
		break;
	default:
		break;
	}

	std::vector<double> realTimeValues = std::vector<double>();
	
	for(int i=0; i<timeValues.size(); i++)
	{
		if(timeValues[i]>=0.0 && timeValues[i]<=1.0)
			realTimeValues.push_back(timeValues[i]);
	}
	return realTimeValues;
}

std::vector<double> BezierCurve::findTimeForX(double X)
{
	std::vector<double> timeValues = std::vector<double>();
	double A,B,C,D;
	switch (points.size())
	{
	case 0:
		//no points
		break;
	case 1:
		//only one point
		if(points[0].x == X)
		{
			timeValues.push_back(0);
		}
		break;
	case 2:
		//linear
		//Ay + B = 0
		A = points[1].x - points[0].x;
		B = points[0].x - X;
		timeValues = MathExt::solveLinear(A, B);
		break;
	case 3:
		//quadratic
		//Ay^2 + By + C = 0
		A = points[0].x - 2*points[1].x + points[2].x;
		B = -2*points[0].x + 2*points[1].x;
		C = points[0].x - X;
		timeValues = MathExt::solveQuadraticReal(A,B,C);
		break;
	case 4:
		//cubic
		//Ay^3 + By^2 + Cy + D = 0
		A = -points[0].x + 3*points[1].x - 3*points[2].x + points[3].x;
		B = 3*points[0].x - 6*points[1].x + 3*points[2].x;
		C = -3*points[0].x + 3*points[1].x;
		D = points[0].x - X;
		timeValues = MathExt::solveCubicReal(A,B,C,D);
		break;
	default:
		break;
	}

	std::vector<double> realTimeValues;
	for(int i=0; i<timeValues.size(); i++)
	{
		if(timeValues[i]>=0 && timeValues[i]<=1)
			realTimeValues.push_back(timeValues[i]);
	}

	return realTimeValues;
}

Vec2f BezierCurve::blendPointsRecursive(int start, int end, double time)
{
	if (start<end)
	{
		return (blendPointsRecursive(start,end-1,time)*(1-time)) + (blendPointsRecursive(start+1,end,time)*time);
	}
	else if (start==end)
	{
		return points[start];
	}

	// blP(0, 1-1, 0) * (1) + blP(1, 1, 0) * (0)
		//p1*(1) + p2*(0)

	// blP(0, 2-1, 0) * (1) + blP(1, 2, 0) * (0)
		// blP(0,1,0) * (1) + blP(1,2,0) * (0)
		// (blP(0,0,0)*1 + blP(1,1,0)*(0)) *(1) + (blP(1,1,0)*1 + blP(2,2,0)*(0)) *(1)
			//((p1*1 + p2*0)*1) + ((p2*1 + p3*0)*0)

			//(p1*(1-t) + p2*t)*(1-t) + (p2*(1-t) + p3*t)*t
			//p1*(1-t)^2 + 2*p2*t*(1-t) + p3*t^2

	return Vec2f();
}

Vec2f BezierCurve::blendPointsExplicit(double time)
{
	Vec2f finalValue = Vec2f();
	double tInverse = 1 - time;
	int n = points.size();

	for(int i=0; i<n; i++)
	{
		finalValue += points[i] * (MathExt::binomialCoefficient(n, i) * MathExt::pow(tInverse,n-i) * MathExt::pow(time,i));
	}

	return finalValue;
}

Vec2f BezierCurve::blendPointsDerivativeRecursive(int start, int end, double time)
{
	if (start<end)
	{
		Vec2f v1 = -blendPointsRecursive(start, end-1, time);
		Vec2f v2 = blendPointsDerivativeRecursive(start, end-1, time) * (1-time);
		Vec2f v3 = blendPointsRecursive(start+1, end, time);
		Vec2f v4 = blendPointsDerivativeRecursive(start+1, end, time) * time;
		return v1+v2+v3+v4;
	}
	else if (start==end)
	{
		return Vec2f(0,0);
	}

	return Vec2f();
}