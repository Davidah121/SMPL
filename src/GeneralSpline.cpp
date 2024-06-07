#include "GeneralSpline.h"

namespace smpl
{

	GeneralSpline::GeneralSpline()
	{

	}

	GeneralSpline::~GeneralSpline()
	{

	}

	void GeneralSpline::addEndPoint(Vec2f p)
	{
		if(firstPoint)
		{
			BezierCurve b = BezierCurve();
			if(curves.size() > 0)
				b.addPoint( curves.back().getPoint( curves.back().size()-1 ) );
			else
				firstPoint = true;
			
			b.addPoint( p );
			curves.push_back(b);
		}
		else
		{
			curves.back().addPoint( p );
			firstPoint = false;
		}
	}

	void GeneralSpline::addControlPoint(size_t index, Vec2f p)
	{
		if(index < curves.size())
		{

			if(index == curves.size()-1)
			{
				//add directly to the end
				curves[index].addPoint(p);
			}
			else
			{
				//add behind the last point
				curves[index].insertPoint( curves[index].size()-1, p);
			}
		}
	}

	BezierCurve* GeneralSpline::getCurve(size_t index)
	{
		if(index < curves.size())
		{
			return &curves[index];
		}
		return nullptr;
	}

	size_t GeneralSpline::size()
	{
		return curves.size();
	}
}