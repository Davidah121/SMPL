#include "BezierCurve.h"
#include "MathExt.h"
#include "StringTools.h"

namespace glib
{

	const Class BezierCurve::myClass = Class("BezierCurve", {&Object::myClass});
	const Class* BezierCurve::getClass()
	{
		return &BezierCurve::myClass;
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

	void BezierCurve::addPoint(double x, double y)
	{
		points.push_back(Vec2f(x, y));
	}

	Vec2f BezierCurve::getPoint(int index)
	{
		if (index >= 0 && index < points.size())
			return points[index];
		else
		{
			#ifdef USE_EXCEPTIONS
			throw OutOfBoundsError();
			#endif
		}
		return Vec2f();
	}

	std::vector<BezierCurve> BezierCurve::subdivide(double t)
	{
		if(points.size() < 2)
			return {};
		
		std::vector<Vec2f> pointsForFirst;
		std::vector<Vec2f> pointsForLast;

		pointsForFirst.push_back(points.front());
		pointsForLast.push_back(points.back());
		
		std::vector<Vec2f> nPoints = points;

		while(true)
		{
			std::vector<Vec2f> xPoints;
			for(int i=0; i<nPoints.size()-1; i++)
			{
				Vec2f tempBlendPoint = nPoints[i]*(1-t) + nPoints[i+1]*t;
				xPoints.push_back(tempBlendPoint);
			}

			if(xPoints.size() == 1)
			{
				pointsForFirst.push_back(xPoints[0]);
				pointsForLast.push_back(xPoints[0]);
				break;
			}
			else if(xPoints.size() > 0)
			{
				pointsForFirst.push_back(xPoints.front());
				pointsForLast.push_back(xPoints.back());
				
				nPoints = xPoints;
			}
			else
			{
				//error probably
				break;
			}
		}

		//Form bezier curve from the points
		BezierCurve b1, b2;
		for(int i=0; i<pointsForFirst.size(); i++)
		{
			b1.addPoint(pointsForFirst[i]);
		}

		for(int i=pointsForLast.size()-1; i>=0; i--)
		{
			b2.addPoint(pointsForLast[i]);
		}

		return {b1, b2};
	}

	BezierCurve BezierCurve::extract(double a, double b)
	{
		if(a < 0 || a > 1 || b < 0 || b > 1)
		{
			return BezierCurve();
		}

		std::vector<BezierCurve> tempSubdivision = subdivide(b);

		//discard the second subdivision
		//subdivide the first BezierCurve at new 'a' point

		double newABlend = 1.0/b * a;

		std::vector<BezierCurve> newSubDivision = tempSubdivision[0].subdivide(newABlend);

		return newSubDivision[1];
	}

	std::vector<Vec2f> BezierCurve::getBoundingBox()
	{
		Vec2f minPoint, maxPoint;

		if(size() >= 2)
		{
			minPoint = points[0];
			maxPoint = points[0];

			for(Vec2f& v : points)
			{
				minPoint.x = MathExt::min(v.x, minPoint.x);
				maxPoint.x = MathExt::max(v.x, maxPoint.x);

				minPoint.y = MathExt::min(v.y, minPoint.y);
				maxPoint.y = MathExt::max(v.y, maxPoint.y);
			}

			return {minPoint, maxPoint};
		}

		return {};
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

	double BezierCurve::getArcLengthAt(double time)
	{
		return getArcLengthAt(0, time);
	}

	double BezierCurve::getArcLengthAt(double startTime, double endTime)
	{
		//Using Simpson's rule for anything above degree 1
		double arcLength = 0;
		int subdivisions = 2 * points.size();
		double totalTime = endTime-startTime;
		double deltaX = totalTime/subdivisions;
		Vec2f derVec = Vec2f(); //Derivative

		switch (points.size())
		{
		case 0:
			//no points
			break;
		case 1:
			//one point
			arcLength = 0;
			break;
		case 2:
			//line
			arcLength = MathExt::distanceTo(points[0], points[1]) * totalTime;
			break;
		default:
			//Simpson's rule
			//subdivisions = 4 * points.size()
			//each time section is approximated by Simpson's rule of n = 4
			derVec = getDerivativeAt(startTime);
			arcLength += MathExt::vecLength(derVec);

			for(int i=1; i<subdivisions-1; i++)
			{
				derVec = getDerivativeAt(startTime + deltaX*i);
				if(i%2 == 1)
					arcLength += 4 * MathExt::vecLength(derVec);
				else
					arcLength += 2 * MathExt::vecLength(derVec);
			}

			derVec = getDerivativeAt(endTime);
			arcLength += MathExt::vecLength(derVec);
			
			arcLength *= deltaX/3.0;
			break;
		}

		return arcLength;
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
		double du;
		double preTime = 0;
		
		GeneralMathFunction f = GeneralMathFunction();
		f.setFunction( [this, Y](double t) -> double{
			return this->getFuctionAt(t).y - Y;
		});

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
			//attempt to solve each section of the bezier curve separately.
			du = 1.0 / size();
			for(int i=1; i<=size(); i++)
			{
				double nTime = du*i;
				double possibleSolution = MathExt::bisectionMethod(&f, preTime, nTime, 10);
				timeValues.push_back(possibleSolution);
				preTime = nTime;
			}
			break;
		}

		std::vector<double> realTimeValues = std::vector<double>();
		
		for(int i=0; i<timeValues.size(); i++)
		{
			if(timeValues[i] == NAN)
				continue;
			if(timeValues[i]>=0.0 && timeValues[i]<=1.0)
				realTimeValues.push_back(timeValues[i]);
		}
		return realTimeValues;
	}

	std::vector<double> BezierCurve::findTimeForX(double X)
	{
		std::vector<double> timeValues = std::vector<double>();
		double A,B,C,D;
		double du;
		double preTime = 0;
		
		GeneralMathFunction f = GeneralMathFunction();
		f.setFunction( [this, X](double t) -> double{
			return this->getFuctionAt(t).x - X;
		});

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
			//attempt to solve each section of the bezier curve separately.
			du = 1.0 / size();
			for(int i=1; i<=size(); i++)
			{
				double nTime = du*i;
				double possibleSolution = MathExt::bisectionMethod(&f, preTime, nTime, 10);
				timeValues.push_back(possibleSolution);
				preTime = nTime;
			}
			break;
		}

		std::vector<double> realTimeValues;
		for(int i=0; i<timeValues.size(); i++)
		{
			if(timeValues[i] == NAN)
				continue;
			if(timeValues[i]>=0 && timeValues[i]<=1)
				realTimeValues.push_back(timeValues[i]);
		}

		return realTimeValues;
	}

	std::vector<double> BezierCurve::findTimeForPoint(double x, double y)
	{
		std::vector<double> xTimes = findTimeForX(x);
		std::vector<double> yTimes = findTimeForY(y);

		//for all matching times, add to the final list.
		std::vector<double> finalTimes;

		for(int i=0; i<xTimes.size(); i++)
		{
			for(int j=0; j<yTimes.size(); j++)
			{
				if(xTimes[i] == yTimes[j])
				{
					finalTimes.push_back(xTimes[i]);
				}
			}
		}

		return finalTimes;
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

		#ifdef USE_EXCEPTIONS
		throw BezierCurve::BlendPointsError();
		#endif

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

		#ifdef USE_EXCEPTIONS
		throw BezierCurve::BlendPointsError();
		#endif
		
		return Vec2f();
	}

	std::vector<BezierCurve> BezierCurve::approximateCircle(double radius, double x, double y, bool cubic)
	{
		std::vector<BezierCurve> curves;
		if(cubic)
		{
			double du = 2*PI / 4;
			for(int i=0; i<4; i++)
			{
				BezierCurve b = approximateArc(radius, radius, du*i, du*(i+1), x, y, true);
				curves.push_back(b);
			}
		}
		else
		{
			double du = 2*PI / 8;
			for(int i=0; i<8; i++)
			{
				BezierCurve b = approximateArc(radius, radius, du*i, du*(i+1), x, y, false);
				curves.push_back(b);
			}
		}

		return curves;
	}

	std::vector<BezierCurve> BezierCurve::approximateEllipse(double xradius, double yradius, double x, double y, bool cubic)
	{
		std::vector<BezierCurve> curves;
		if(cubic)
		{
			double du = 2*PI / 4;
			for(int i=0; i<4; i++)
			{
				BezierCurve b = approximateArc(xradius, yradius, du*i, du*(i+1), x, y, true);
				curves.push_back(b);
			}
		}
		else
		{
			double du = 2*PI / 8;
			for(int i=0; i<8; i++)
			{
				BezierCurve b = approximateArc(xradius, yradius, du*i, du*(i+1), x, y, false);
				curves.push_back(b);
			}
		}

		return curves;
	}

	BezierCurve BezierCurve::approximateArc(double xRadius, double yRadius, double startAngle, double endAngle, double x, double y, bool cubic)
	{
		BezierCurve b = BezierCurve();
		Vec2f offset = Vec2f(x,y);

		if(cubic)
		{
			Vec2f p1,p2,p3,p4;
			double midAngle = (startAngle+endAngle)/2;
			p1.x = xRadius*MathExt::cos(startAngle);
			p1.y = yRadius*MathExt::sin(startAngle);
			p4.x = xRadius*MathExt::cos(endAngle);
			p4.y = yRadius*MathExt::sin(endAngle);

			//derivatives at t=0 && t=1 should have the same direction but not necessarily the same constants
			//The constant c does not need to be solved for y. Not sure why.
			double temp = (8.0/3.0)*xRadius*MathExt::cos(midAngle) - (4.0/3.0)*p1.x - (4.0/3.0)*p4.x;
			double c = temp / (-xRadius*MathExt::sin(startAngle) + xRadius*MathExt::sin(endAngle));

			p2.x = p1.x + (-c*xRadius*MathExt::sin(startAngle));
			p3.x = p4.x + (c*xRadius*MathExt::sin(endAngle));

			p2.y = p1.y + (c*yRadius*MathExt::cos(startAngle));
			p3.y = p4.y + (-c*yRadius*MathExt::cos(endAngle));

			//Add points to bezier curve
			b.addPoint(p1+offset);
			b.addPoint(p2+offset);
			b.addPoint(p3+offset);
			b.addPoint(p4+offset);
		}
		else
		{
			Vec2f p1,p2,p3;
			double midAngle = (startAngle+endAngle)/2;
			p1.x = xRadius*MathExt::cos(startAngle);
			p1.y = yRadius*MathExt::sin(startAngle);
			p3.x = xRadius*MathExt::cos(endAngle);
			p3.y = yRadius*MathExt::sin(endAngle);

			p2.x = 2*(xRadius*MathExt::cos(midAngle) - 0.25*p1.x - 0.25*p3.x);
			p2.y = 2*(yRadius*MathExt::sin(midAngle) - 0.25*p1.y - 0.25*p3.y);
			
			//Add points to bezier curve
			b.addPoint(p1+offset);
			b.addPoint(p2+offset);
			b.addPoint(p3+offset);
		}

		return b;
	}

} //NAMESPACE glib END