#include "BezierCurve.h"
#include "MathExt.h"
#include "StringTools.h"

namespace smpl
{
	const RootClass BezierCurve::globalClass = CREATE_ROOT_CLASS(BezierCurve, &Object::globalClass);
	const RootClass* BezierCurve::getClass()
	{
		return &BezierCurve::globalClass;
	}

	BezierCurve::BezierCurve()
	{
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

	bool BezierCurve::insertPoint(size_t index, Vec2f p)
	{
		if(index < points.size())
		{
			points.push_back(p);
			for(int i=points.size()-1; i>=index; i--)
			{
				Vec2f temp = points[i-1];
				points[i-1] = points[i];
				points[i] = temp;
			}
			return true;
		}
		return false;
	}

	bool BezierCurve::insertPoint(size_t index, double x, double y)
	{
		if(index < points.size())
		{
			points.push_back(Vec2f(x, y));
			for(int i=points.size()-1; i>=index; i--)
			{
				Vec2f temp = points[i-1];
				points[i-1] = points[i];
				points[i] = temp;
			}
			return true;
		}
		return false;
	}

	void BezierCurve::removePoint(size_t index)
	{
		if(index < points.size())
		{
			std::vector<Vec2f> nPoints;
			for(int i=0; i<points.size(); i++)
			{
				if(i != index)
					nPoints.push_back(points[i]);
			}
			points = nPoints;
		}
	}

	void BezierCurve::setPoint(size_t index, Vec2f p)
	{
		if(index < points.size())
			points[index] = p;
	}

	void BezierCurve::setPoint(size_t index, double x, double y)
	{
		if(index < points.size())
			points[index] = Vec2f(x, y);
	}

	Vec2f BezierCurve::getPoint(size_t index)
	{
		if (index < points.size())
			return points[index];
		else
		{
			#ifdef USE_EXCEPTIONS
			throw OutOfBoundsError();
			#endif
		}
		return Vec2f();
	}

	std::vector<Vec2f>& BezierCurve::getPoints()
	{
		return points;
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
			for(size_t i=0; i<nPoints.size()-1; i++)
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
		for(size_t i=0; i<pointsForFirst.size(); i++)
		{
			b1.addPoint(pointsForFirst[i]);
		}

		for(long i=(long)pointsForLast.size()-1; i>=0; i--)
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

		return blendPointsRecursive(0, (int)(points.size()-1), time);
	}

	Vec2f BezierCurve::getDerivativeAt(double time)
	{
		return blendPointsDerivativeRecursive(0, (int)(points.size()-1), time);
	}

	Vec2f BezierCurve::getSimpleDerivativeAt(double time)
	{
		//assume the points are separated by time segments
		//here, we just return the rate of change to the next time segment.
		//useful only when determining when the average rate switches
		
		int segments = (int)(points.size()-1);
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
		int subdivisions = (int)(2 * points.size());
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
			//subdivisions = 2 * points.size()
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

	double BezierCurve::findTimeForMinDis(double x, double y, unsigned int maxIterations)
	{
		return findTimeForMinDis( Vec2f(x, y), maxIterations );
	}

	double BezierCurve::findTimeForMinDis(Vec2f p, unsigned int maxIterations)
	{
		//use secant method to solve 
		double xn[3] = {NAN, 1, 0};
		double sol[3] = {NAN, 0, 0};

		double solAt0 = 0;
		double solAt1 = 0;

		Vec2f bt = this->getFuctionAt(0);
		Vec2f bpt = this->getDerivativeAt(0);
		sol[2] = 2*(p.x - bt.x)*(-bpt.x) + 2*(p.y - bt.y)*(-bpt.y);
		
		bt = this->getFuctionAt(1);
		bpt = this->getDerivativeAt(1);
		sol[1] = 2*(p.x - bt.x)*(-bpt.x) + 2*(p.y - bt.y)*(-bpt.y);
		
		if(sol[2] == 0)
			return 0;
		if(sol[1] == 0)
			return 1;

		solAt0 = sol[2];
		solAt1 = sol[1];
		
		for(int i=0; i<maxIterations; i++)
		{
			double num = sol[1]*(xn[1] - xn[2]);
			double div = sol[1] - sol[2];
			if(div == 0)
			{
				//error occured
				xn[0] = NAN;
				break;
			}

			xn[0] = xn[1] - num/div;
			
			bt = this->getFuctionAt(xn[0]);
			bpt = this->getDerivativeAt(xn[0]);
			sol[0] = 2*(p.x - bt.x)*(-bpt.x) + 2*(p.y - bt.y)*(-bpt.y);

			if(sol[0] == 0)
			{
				break;
			}

			//move xn to xn-1 and move xn-1 to xn-2
			xn[2] = xn[1];
			xn[1] = xn[0];

			sol[2] = sol[1];
			sol[1] = sol[0];
		}

		if(xn[0] != NAN)
		{
			if(xn[0] >= 0 && xn[0] <= 1)
				return xn[0];
		}
		
		if(solAt0 < solAt1)
			return 0;
		else
			return 1;
	}

	void BezierCurve::clear()
	{
		points.clear();
	}

	size_t BezierCurve::size()
	{
		return points.size();
	}

	std::vector<double> BezierCurve::findTimeForY(double Y, bool removeDuplicates)
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
			for(size_t i=1; i<=size(); i++)
			{
				double nTime = du*i;
				double possibleSolution = MathExt::bisectionMethod(&f, preTime, nTime, 10);
				timeValues.push_back(possibleSolution);
				preTime = nTime;
			}
			break;
		}

		std::vector<double> realTimeValues = std::vector<double>();
		
		for(size_t i=0; i<timeValues.size(); i++)
		{
			double roundedValue = MathExt::roundToDecimal(timeValues[i]);
			if(timeValues[i] == NAN)
				continue;
			if(roundedValue>=0.0 && roundedValue<=1.0)
			{
				bool found = false;
				if(removeDuplicates)
				{
					for(size_t j=0; j<realTimeValues.size(); j++)
					{
						if(realTimeValues[j] == roundedValue)
						{
							found = true;
							break;
						}
					}
				}

				if(!found)
					realTimeValues.push_back(roundedValue);
			}
		}
		
		return realTimeValues;
	}

	std::vector<double> BezierCurve::findTimeForX(double X, bool removeDuplicates)
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
			A = MathExt::roundToDecimal(A, 10);
			B = MathExt::roundToDecimal(B, 10);
			C = MathExt::roundToDecimal(C, 10);
			timeValues = MathExt::solveQuadraticReal(A,B,C);
			break;
		case 4:
			//cubic
			//Ay^3 + By^2 + Cy + D = 0
			A = -points[0].x + 3*points[1].x - 3*points[2].x + points[3].x;
			B = 3*points[0].x - 6*points[1].x + 3*points[2].x;
			C = -3*points[0].x + 3*points[1].x;
			D = points[0].x - X;
			A = MathExt::roundToDecimal(A, 10);
			B = MathExt::roundToDecimal(B, 10);
			C = MathExt::roundToDecimal(C, 10);
			D = MathExt::roundToDecimal(D, 10);
			timeValues = MathExt::solveCubicReal(A,B,C,D);
			break;
		default:
			//attempt to solve each section of the bezier curve separately.
			du = 1.0 / size();
			for(size_t i=1; i<=size(); i++)
			{
				double nTime = du*i;
				double possibleSolution = MathExt::bisectionMethod(&f, preTime, nTime, 10);
				timeValues.push_back(possibleSolution);
				preTime = nTime;
			}
			break;
		}

		std::vector<double> realTimeValues = std::vector<double>();
		
		for(size_t i=0; i<timeValues.size(); i++)
		{
			double roundedValue = MathExt::roundToDecimal(timeValues[i]);
			if(timeValues[i] == NAN)
				continue;
			if(roundedValue>=0.0 && roundedValue<=1.0)
			{
				bool found = false;
				if(removeDuplicates)
				{
					for(size_t j=0; j<realTimeValues.size(); j++)
					{
						if(realTimeValues[j] == roundedValue)
						{
							found = true;
							break;
						}
					}
				}

				if(!found)
					realTimeValues.push_back(roundedValue);
			}
		}

		return realTimeValues;
	}

	std::vector<double> BezierCurve::findTimeForPoint(double x, double y, bool removeDuplicates)
	{
		std::vector<double> xTimes = findTimeForX(x);
		std::vector<double> yTimes = findTimeForY(y);

		//for all matching times, add to the final list.
		std::vector<double> finalTimes;

		for(size_t i=0; i<xTimes.size(); i++)
		{
			for(size_t j=0; j<yTimes.size(); j++)
			{
				if(xTimes[i] == yTimes[j])
				{
					bool found = false;

					if(removeDuplicates)
					{
						for(size_t j=0; j<finalTimes.size(); j++)
						{
							if(finalTimes[j] == xTimes[i])
							{
								found = true;
								break;
							}
						}
					}

					if(!found)
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
		int n = (int)points.size();

		for(int i=0; i<n; i++)
		{
			finalValue += points[i] * (MathExt::binomialCoefficient(n, i) * MathExt::pow(tInverse,(double)n-i) * MathExt::pow(time,(double)i));
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