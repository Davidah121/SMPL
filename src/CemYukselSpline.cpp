#include "CemYukselSpline.h"
#include "MathExt.h"

namespace smpl
{

	CemYukselSpline::CemYukselSpline(int type)
	{
		setType(type);
	}

	CemYukselSpline::~CemYukselSpline()
	{
		
	}

	void CemYukselSpline::setType(int type)
	{
		this->type = MathExt::clamp(type, 0, 3);
	}

	void CemYukselSpline::addPoint(Vec2f p)
	{
		points.push_back(p);
	}

	Vec2f CemYukselSpline::getPoint(size_t index)
	{
		if(index < points.size())
			return points[index];
		return Vec2f();
	}

	void CemYukselSpline::editPoint(size_t index, Vec2f p)
	{
		if(index < points.size())
			points[index] = p;
	}

	bool CemYukselSpline::insertPoint(size_t index, Vec2f p)
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
	
	void CemYukselSpline::removePoint(size_t index)
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

	Vec2f CemYukselSpline::solve(size_t curveSegment, double weight)
	{
		//defined using [0, pi/2] so scaling is required
		size_t index = 0;
		double theta = weight * (PI/2);
		double cosVSquared = MathExt::sqr(MathExt::cos(theta));
		double sinVSquared = MathExt::sqr(MathExt::sin(theta));
		Vec2f F1, F2;

		if(type == TYPE_BEZIER)
		{
			F1 = bezierFormulation(index, theta + (PI/2));
			F2 = bezierFormulation(index+1, theta);
		}
		else if(type == TYPE_CIRCULAR)
		{
			F1 = circularFormulation(index, theta + (PI/2));
			F2 = circularFormulation(index+1, theta);
		}
		else if(type == TYPE_ELLIPTICAL)
		{
			F1 = ellipticalFormulation(index, theta + (PI/2));
			F2 = ellipticalFormulation(index+1, theta);
		}
		else if(type == TYPE_HYBRID)
		{
			F1 = hybridFormulation(index, theta + (PI/2));
			F2 = hybridFormulation(index+1, theta);
		}

		return cosVSquared*F1 + sinVSquared*F2;
	}

	Vec2f CemYukselSpline::bezierFormulation(size_t index, double weight)
	{
		if(size() == 0)
			return Vec2f();
		
		double t;
		double normalizedWeight = weight / PI;
		Vec2f b0, b1, b2;
		BezierCurve c;

		int indexI = (index-1+size()) % size(); //wraps around to the end if neccessary.
		int indexJ = (index+1) % size(); //wraps around to the beginning if neccessary.

		b0 = points[indexI];
		b2 = points[indexJ];
		
		//solve for t such that t places maximum curvature at the point
		double A, B, C, D;
		Vec2f temp1, temp2;
		temp1 = b0 - points[index];
		temp2 = b2 - points[index];

		A = -MathExt::dot(temp1,temp1);
		B = -MathExt::dot(b0, b2)/3;
		C = -B;
		D = MathExt::dot(temp2,temp2);

		//Can also use Bisection since a solution should be in [0, 1]
		std::vector<double> solutions = MathExt::solveCubicReal(A, B, C, D);
		
		bool wasSet = false;
		for(int i=0; i<solutions.size(); i++)
		{
			if(solutions[i] >= 0.0 && solutions[i] <= 1.0)
			{
				wasSet = true;
				t = solutions[i];
				break;
			}
		}

		if(!wasSet)
			t = 0.5; //Something probably went wrong. Set t to 0.5

		//solve for b1
		b1 = (points[index] - MathExt::sqr(1-t)*b0 - MathExt::sqr(t)*b2) / 2*(1-t)*t;

		c.addPoint(b0);
		c.addPoint(b1);
		c.addPoint(b2);
		
		return c.getFuctionAt(normalizedWeight);
	}

	Vec2f CemYukselSpline::circularFormulation(size_t index, double weight)
	{
		//Find the circle that fits all 3 points p[i-1] p[i] and p[i+1]
		float normalizedWeight = weight / PI;
		int indexI = (index-1+size()) % size(); //wraps around to the end if neccessary.
		int indexJ = (index+1) % size(); //wraps around to the beginning if neccessary.

		Vec3f circleInfo = MathExt::fitCircleToTriangle(points[indexI], points[index], points[indexJ], false);

		//Just need the angle to p[i-1] and p[i+1]
		Vec2f tempV = points[indexI] - circleInfo.toVec2f();
		Vec2f tempV2 = points[indexJ] - circleInfo.toVec2f();
		double angle1 = MathExt::arctan2( tempV.y, tempV.x );
		double angle2 = MathExt::arctan2( tempV2.y, tempV2.x );
		
		//blend between the angles. Note that we want the small angle between them not the large angle.
		Vec2f range = MathExt::angleRange(angle1, angle2, true);
		
		Vec2f finalPoint = MathExt::lengthDir(circleInfo.z, MathExt::lerp(range.x, range.y, normalizedWeight)) + circleInfo.toVec2f();
		return finalPoint;
	}

	Vec2f CemYukselSpline::ellipticalFormulation(size_t index, double weight)
	{
		//Fit an ellipse to all 3 points p[i-1] p[i] and p[i+1]
		//Solve such that p[i] is on the primary axis
		//Solve such that either p[i+1] or p[i-1] is on the secondary axis depending on which is further from p[i]
		double normalizedWeight = weight / PI;
		int indexI = (index-1+size()) % size(); //wraps around to the end if neccessary.
		int indexJ = (index+1) % size(); //wraps around to the beginning if neccessary.

		Vec2f longerP, circleMid, otherP, mainP;
		double circleRadius;

		mainP = points[index];
		if( (points[indexJ] - mainP).getLength() > (points[indexI] - mainP).getLength() )
		{
			longerP = points[indexJ];
			otherP = points[indexI];
		}
		else
		{
			longerP = points[indexI];
			otherP = points[indexJ];
		}
		circleMid = (longerP+mainP)/2;
		circleRadius = (longerP-mainP).getLength();
		

		//Note that to find the center of ellipse, we find a circle that fits between p[i] and the longer connected point.
		//We treat those 2 points as directly across from each other such that |p[i] - longerP| is the diameter and the center is (p[i]+longerP)/2
		//The ellipse position must be on the circle so we find the position such that the other point is also on the ellipse.
		//To reduce our solution even further from the 4 possible points, we define it such that the other point is not on the same arc and
		//p[i] is still the primary axis (longest). This reduces it such that we only have one solution.
		//We can reduce the range of solutions and use bisection to solve.

		double r1,r2,r3,r4;
		Vec2f temp = mainP - circleMid;
		r1 = MathExt::arctan2(temp.y, temp.x); //arc range
		r2 = r1 + PI; //arc range
		r3 = r1 + PI/2; //axis range
		r4 = r1 + 3*PI/2; //axis range
		
		//reduce to final range before solving using bisection
		//either r1, r3 || r1, r4 || r2, r3 || r2, r4
		//check at r3

		Vec2f r1p = circleMid + MathExt::lengthDir(circleRadius, r1);
		Vec2f r3p = circleMid + MathExt::lengthDir(circleRadius, r3);
		
		bool s1, s2;
		double startRange, endRange;
		if((mainP - r1p).getLength() > (longerP - r1p).getLength())
		{
			s1 = true;
			startRange = r1;
		}
		else
		{
			s1 = false;
			startRange = r2;
		}

		//note that the direction to the point tells you which side to use
		Vec2f toP2 = otherP - circleMid;
		double dirToP2 = MathExt::arctan2(toP2.y, toP2.x);
		if(dirToP2 >= r1 && dirToP2 <= r2)
		{
			s2 = true;
			endRange = r3;
		}
		else
		{
			s2 = false;
			endRange = r4;
		}
		
		if(startRange > endRange)
		{
			//swap.
			double temp = startRange;
			startRange = endRange;
			endRange = temp;
		}
		
		GeneralMathFunction f = GeneralMathFunction();
		f.setFunction([circleMid, circleRadius, mainP, longerP ](double t) -> double{
			//solve ellipse 
			return 0.0;
		});

		return Vec2f(); //TODO: FINISH LATER
	}

	Vec2f CemYukselSpline::hybridFormulation(size_t index, double weight)
	{
		return Vec2f(); //TODO: FINISH LATER
	}

	size_t CemYukselSpline::size()
	{
		return points.size();
	}

	size_t CemYukselSpline::curveSize()
	{
		if(points.size() > 1)
			return points.size()-1;
		else
			return 0;
	}

}