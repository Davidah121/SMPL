#include "VectorGraphic.h"
#include "SimpleGraphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "SimpleXml.h"
#include "ColorNameConverter.h"

namespace smpl
{
		
	#pragma region VectorShape

	VectorShape::VectorShape()
	{
		transform = Mat3f::getIdentity();
	}

	VectorShape::~VectorShape()
	{
		
	}

	void VectorShape::draw(Image* buffer, int globalWidth, int globalHeight)
	{
	}

	void VectorShape::drawStroke(Image* buffer, int globalWidth, int globalHeight)
	{
	}

	void VectorShape::setFillMethod(bool value)
	{
		fillMethod = value;
	}

	bool VectorShape::getFillMethod()
	{
		return fillMethod;
	}

	void VectorShape::setFillColor(Color k)
	{
		fill = k;
	}

	Color VectorShape::getFillColor()
	{
		return fill;
	}

	void VectorShape::setStrokeColor(Color k)
	{
		strokeColor = k;
	}

	Color VectorShape::getStrokeColor()
	{
		return strokeColor;
	}

	void VectorShape::setStrokeWidth(double k)
	{
		strokeWidth = k;
	}

	double VectorShape::getStrokeWidth()
	{
		return strokeWidth;
	}

	void VectorShape::setLineCap(char c)
	{
		lineCap = c;
	}

	char VectorShape::getLineCap()
	{
		return lineCap;
	}

	void VectorShape::setLineJoin(char c)
	{
		lineJoin = c;
	}

	char VectorShape::getLineJoin()
	{
		return lineJoin;
	}

	void VectorShape::setTransform(Mat3f m)
	{
		transform = m;
	}

	Mat3f VectorShape::getTransform()
	{
		return transform;
	}

	void VectorShape::applyTransform()
	{
	}

	std::pair<Vec2f, Vec2f> VectorShape::getBoundingBox()
	{
		return {};
	}
	
	VectorShape* VectorShape::createNewFromVectorShape(const VectorShape& shape)
	{
		if(shape.getClass() == TypeInfo::get<VectorRectangle>())
			return new VectorRectangle((VectorRectangle&)shape);
		else if(shape.getClass() == TypeInfo::get<VectorCircle>())
			return new VectorCircle((VectorCircle&)shape);
		else if(shape.getClass() == TypeInfo::get<VectorLine>())
			return new VectorLine((VectorLine&)shape);
		else if(shape.getClass() == TypeInfo::get<VectorEllipse>())
			return new VectorEllipse((VectorEllipse&)shape);
		else if(shape.getClass() == TypeInfo::get<VectorPolygon>())
			return new VectorPolygon((VectorPolygon&)shape);
		else if(shape.getClass() == TypeInfo::get<VectorPolyline>())
			return new VectorPolyline((VectorPolyline&)shape);
		else if(shape.getClass() == TypeInfo::get<VectorPath>())
			return new VectorPath((VectorPath&)shape);
		return nullptr;
	}
	#pragma endregion

} //NAMESPACE glib END