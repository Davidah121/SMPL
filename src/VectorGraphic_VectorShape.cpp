#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "SimpleXml.h"
#include "ColorNameConverter.h"

#pragma region VectorShape

const Class* VectorShape::myClass = new Class("VectorShape", {Object::myClass});
const Class* VectorShape::getClass()
{
	return VectorShape::myClass;
}

VectorShape::VectorShape()
{
	transform = Mat3f::getIdentity();
}

VectorShape::~VectorShape()
{
	
}

void VectorShape::draw(Image* buffer, int globalWidth, int globalHeight)
{
	StringTools::println("Basic one");
}

void VectorShape::drawStroke(Image* buffer, int globalWidth, int globalHeight)
{
	StringTools::println("Basic one");
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

#pragma endregion