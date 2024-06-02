#include "VectorGraphic.h"
#include "SimpleGraphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

namespace smpl
{
		
	#pragma region VectorPolyline

	const RootClass VectorPolyline::globalClass = CREATE_ROOT_CLASS(VectorPolyline, &VectorShape::globalClass);
    const RootClass* VectorPolyline::getClass()
	{
		return &VectorPolyline::globalClass;
	}

	VectorPolyline::VectorPolyline() : VectorShape()
	{
	}

	VectorPolyline::~VectorPolyline()
	{
	}

	void VectorPolyline::draw(Image* buffer, int globalWidth, int globalHeight)
	{
		//Everything done here has been moved to the SimpleGraphics class
		//Hence, we will use that method which will be optimized
		//separately.
		//The difference here will be in how we draw the stroke.
		SimpleGraphics::setColor(getFillColor());
		SimpleGraphics::setFillRule(getFillMethod());

		std::vector<Vec2f> prePoints = points;
		applyTransform();

		SimpleGraphics::drawPolygon(points.data(), points.size(), buffer);

		if(getStrokeWidth()==1)
		{
			SimpleGraphics::setColor(getStrokeColor());
			SimpleGraphics::setFillRule(SimpleGraphics::FILL_EVEN_ODD);
			for(int i=0; i<points.size()-1; i++)
			{
				SimpleGraphics::drawLine((int)MathExt::floor(points[i].x), (int)MathExt::floor(points[i].y),
					(int)MathExt::floor(points[i+1].x), (int)MathExt::floor(points[i+1].y), buffer);
			}
		}
		else if(getStrokeWidth()>1)
		{
			SimpleGraphics::setColor(getStrokeColor());
			SimpleGraphics::setFillRule(SimpleGraphics::FILL_EVEN_ODD);
			for(int i=0; i<points.size()-1; i++)
			{
				//create the 4 points
				Vec2f toEndPoint = Vec2f(points[i+1].x-points[i].x, points[i+1].y-points[i].y);
				Vec2f toEndPointScaled = MathExt::normalize(toEndPoint) * getStrokeWidth()/2;
				
				Vec2f newPoint1 = Vec2f( points[i].x-toEndPointScaled.y, points[i].y+toEndPointScaled.x);
				Vec2f newPoint2 = Vec2f( points[i].x+toEndPointScaled.y, points[i].y-toEndPointScaled.x);
				Vec2f newPoint3 = newPoint2 + toEndPoint;
				Vec2f newPoint4 = newPoint1 + toEndPoint;
				
				SimpleGraphics::drawPolygon(new Vec2f[4]{newPoint1, newPoint2, newPoint3, newPoint4}, 4, buffer);
			}
		}
		
		points = prePoints;
	}

	void VectorPolyline::drawStroke(Image* buffer, int globalWidth, int globalHeight)
	{
		
	}

	void VectorPolyline::addPoint(double x, double y)
	{
		points.push_back(Vec2f(x,y));
	}
	void VectorPolyline::addPoint(Vec2f v)
	{
		points.push_back(v);
	}

	Vec2f VectorPolyline::getPoint(int index)
	{
		return points[index];
	}
	double VectorPolyline::getPointX(int index)
	{
		return points[index].x;
	}
	double VectorPolyline::getPointY(int index)
	{
		return points[index].y;
	}

	int VectorPolyline::size()
	{
		return points.size();
	}

	void VectorPolyline::applyTransform()
	{
		for(int i=0; i<points.size(); i++)
		{
			Vec3f pos = Vec3f(points[i].x, points[i].y, 1.0);
			Vec3f transPos = getTransform() * pos;
			points[i] = Vec2f(transPos.x, transPos.y);
		}
	}

	#pragma endregion
	
} //NAMESPACE glib END