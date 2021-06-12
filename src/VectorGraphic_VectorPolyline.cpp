#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

#pragma region VectorPolyline

const Class VectorPolyline::myClass = Class("VectorPolyline", {&VectorShape::myClass});
const Class* VectorPolyline::getClass()
{
	return &VectorPolyline::myClass;
}

VectorPolyline::VectorPolyline() : VectorShape()
{
}

VectorPolyline::~VectorPolyline()
{
}

void VectorPolyline::draw(Image* buffer, int globalWidth, int globalHeight)
{
	//Everything done here has been moved to the Graphics class
	//Hence, we will use that method which will be optimized
	//separately.
	//The difference here will be in how we draw the stroke.
	Graphics::setColor(getFillColor());
	Graphics::setFillRule(getFillMethod());

	std::vector<Vec2f> prePoints = points;
	applyTransform();

	buffer->drawPolygon(points.data(), points.size());

	if(getStrokeWidth()==1)
	{
		Graphics::setColor(getStrokeColor());
		Graphics::setFillRule(Graphics::FILL_EVEN_ODD);
		for(int i=0; i<points.size()-1; i++)
		{
			buffer->drawLine(points[i].x, points[i].y, points[i+1].x, points[i+1].y);
		}
	}
	else if(getStrokeWidth()>1)
	{
		Graphics::setColor(getStrokeColor());
		Graphics::setFillRule(Graphics::FILL_EVEN_ODD);
		for(int i=0; i<points.size()-1; i++)
		{
			//create the 4 points
			Vec2f toEndPoint = Vec2f(points[i+1].x-points[i].x, points[i+1].y-points[i].y);
			Vec2f toEndPointScaled = MathExt::normalize(toEndPoint) * getStrokeWidth()/2;
			
			Vec2f newPoint1 = Vec2f( points[i].x-toEndPointScaled.y, points[i].y+toEndPointScaled.x);
			Vec2f newPoint2 = Vec2f( points[i].x+toEndPointScaled.y, points[i].y-toEndPointScaled.x);
			Vec2f newPoint3 = newPoint2 + toEndPoint;
			Vec2f newPoint4 = newPoint1 + toEndPoint;
			
			buffer->drawPolygon(new Vec2f[4]{newPoint1, newPoint2, newPoint3, newPoint4}, 4);
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