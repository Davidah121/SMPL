#include "CatmullRom.h"
#include "MathExt.h"

namespace smpl
{

	CatmullRom::CatmullRom()
	{

	}

	CatmullRom::~CatmullRom()
	{

	}

	void CatmullRom::addPoint(Vec2f p)
	{
		points.push_back(p);
	}

	Vec2f CatmullRom::getPoint(size_t index)
	{
		if(index < points.size())
			return points[index];
		return Vec2f();
	}

	void CatmullRom::editPoint(size_t index, Vec2f p)
	{
		if(index < points.size())
			points[index] = p;
	}

	bool CatmullRom::insertPoint(size_t index, Vec2f p)
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
	
	void CatmullRom::removePoint(size_t index)
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

	double CatmullRom::getTension()
	{
		return tension;
	}

	void CatmullRom::setTension(double t)
	{
		tension = t;
	}
	
	Vec2f CatmullRom::solve(size_t curveSegment, double weight)
	{
		BezierCurve b = getCurve(curveSegment);
		return b.getFuctionAt(weight);
	}
	
	BezierCurve CatmullRom::getCurve(size_t index)
	{
		if(index < curveSize())
		{
			//special case.
			if(curveSize() == 1)
			{
				//Just a line
				BezierCurve b;
				b.addPoint(points[0]);
				b.addPoint(points[1]);
				return b;
			}
			else
			{
				Vec2f t0 = getTangent(index);
				Vec2f t1 = getTangent(index+1);
				
				BezierCurve b;
				b.addPoint(points[index]);
				b.addPoint(points[index] + t0);
				b.addPoint(points[index+1] - t1);
				b.addPoint(points[index+1]);

				return b;
			}
		}

		return BezierCurve();
	}

	Vec2f CatmullRom::getTangent(size_t index)
	{
		// if(index < size())
		// {
		// 	//special case
		// 	if(size() == 2)
		// 	{
		// 		//is a line
		// 		return (1.0-tension) * 0.5 * (points[1] - points[0]);
		// 	}
		// 	else if(index == 0)
		// 	{
		// 		//create polynomial or something and get derivative.
		// 		//could create a bezier curve from the points and get the derivative too.
		// 		PolynomialMathFunction f = MathExt::fitPolynomial({ points[index], points[index+1], points[index+2] });
		// 		PolynomialMathFunction derivative = f.getDerivative();
		// 		double yder = derivative.solve(points[index].x);

		// 		Vec2f potentialDerivative = Vec2f((points[index+1]-points[index]).x, yder);
		// 		return (1.0-tension) * 0.5 * potentialDerivative;
		// 	}
		// 	else if(index == size()-1)
		// 	{
		// 		//create polynomial or something and get derivative.
		// 		PolynomialMathFunction f = MathExt::fitPolynomial({ points[index], points[index-1], points[index-2] });
		// 		PolynomialMathFunction derivative = f.getDerivative();
		// 		double yder = derivative.solve(points[index].x);

		// 		Vec2f potentialDerivative = Vec2f((points[index]-points[index-1]).x, yder);
		// 		return potentialDerivative;
		// 	}
		// 	else
		// 	{
		// 		return (1.0-tension) * 0.5 * (points[index+1] - points[index-1]);
		// 	}
		// }
		return Vec2f();
	}

	size_t CatmullRom::size()
	{
		return points.size();
	}
	
	size_t CatmullRom::curveSize()
	{
		if(points.size() > 1)
			return points.size()-1;
		else
			return 0;
	}
}