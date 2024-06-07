#include "VectorGraphic.h"
#include "SimpleGraphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

namespace smpl
{
		
	#pragma region VectorPolygon

	const RootClass VectorPolygon::globalClass = CREATE_ROOT_CLASS(VectorPolygon, &VectorShape::globalClass);
    const RootClass* VectorPolygon::getClass()
	{
		return &VectorPolygon::globalClass;
	}

	VectorPolygon::VectorPolygon() : VectorShape()
	{
	}

	VectorPolygon::~VectorPolygon()
	{
	}

	void VectorPolygon::draw(Image* buffer, int globalWidth, int globalHeight)
	{
		//Everything done here has been moved to the SimpleGraphics class
		//Hence, we will use that method which will be optimized
		//separately.
		SimpleGraphics::setColor(getFillColor());
		SimpleGraphics::setFillRule(getFillMethod());

		std::vector<Vec2f> prePoints = points;
		applyTransform();

		SimpleGraphics::drawPolygon(points.data(), points.size(), buffer);
		points = prePoints;
	}

	void VectorPolygon::drawStroke(Image* buffer, int globalWidth, int globalHeight)
	{
		SimpleGraphics::setColor(getStrokeColor());
		SimpleGraphics::setFillRule(SimpleGraphics::FILL_EVEN_ODD);

		std::vector<Vec2f> prePoints = points;
		applyTransform();

		if(getStrokeWidth()==1)
		{
			for(int i=0; i<points.size(); i++)
			{
				if(i<points.size()-1)
				{
					SimpleGraphics::drawLine((int)MathExt::floor(points[i].x), (int)MathExt::floor(points[i].y),
						(int)MathExt::floor(points[i+1].x), (int)MathExt::floor(points[i+1].y), buffer);
				}
				else
				{
					SimpleGraphics::drawLine((int)MathExt::floor(points[i].x), (int)MathExt::floor(points[i].y),
						(int)MathExt::floor(points[0].x), (int)MathExt::floor(points[0].y), buffer);
				}
			}
		}
		else if(getStrokeWidth()>1)
		{
			for(int i=0; i<points.size(); i++)
			{
				if(i<points.size()-1)
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
				else
				{
					Vec2f toEndPoint = Vec2f(points[0].x-points[i].x, points[0].y-points[i].y);
					Vec2f toEndPointScaled = MathExt::normalize(toEndPoint) * getStrokeWidth()/2;
					
					Vec2f newPoint1 = Vec2f( points[i].x-toEndPointScaled.y, points[i].y+toEndPointScaled.x);
					Vec2f newPoint2 = Vec2f( points[i].x+toEndPointScaled.y, points[i].y-toEndPointScaled.x);
					Vec2f newPoint3 = newPoint2 + toEndPoint;
					Vec2f newPoint4 = newPoint1 + toEndPoint;
					
					SimpleGraphics::drawPolygon(new Vec2f[4]{newPoint1, newPoint2, newPoint3, newPoint4}, 4, buffer);
				}
			}
		}

		points = prePoints;
	}

	void VectorPolygon::addPoint(double x, double y)
	{
		points.push_back(Vec2f(x,y));
	}
	void VectorPolygon::addPoint(Vec2f v)
	{
		points.push_back(v);
	}

	Vec2f VectorPolygon::getPoint(int index)
	{
		return points[index];
	}
	double VectorPolygon::getPointX(int index)
	{
		return points[index].x;
	}
	double VectorPolygon::getPointY(int index)
	{
		return points[index].y;
	}

	int VectorPolygon::size()
	{
		return points.size();
	}

	void VectorPolygon::applyTransform()
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