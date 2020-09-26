#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

#pragma region VectorCircle

const Class* VectorCircle::myClass = new Class("VectorCircle", {VectorShape::myClass});
const Class* VectorCircle::getClass()
{
	return VectorCircle::myClass;
}

VectorCircle::VectorCircle() : VectorShape()
{
}

VectorCircle::~VectorCircle()
{
}

void VectorCircle::draw(Image* buffer, int globalWidth, int globalHeight)
{
	//first, calc bounding box
	int x1 = cx-radius;
	int x2 = cx+radius;
	int y1 = cy-radius;
	int y2 = cy+radius;

	x1 = MathExt::clamp(x1, 0, globalWidth);
	y1 = MathExt::clamp(y1, 0, globalHeight);
	x2 = MathExt::clamp(x2, 0, globalWidth);
	y2 = MathExt::clamp(y2, 0, globalHeight);

	double radSqr = MathExt::sqr(radius);
	double strokeRad = MathExt::sqr(radius-this->getStrokeWidth());

	for(int j=y1; j<y2; j++)
	{
		for(int i=x1; i<x2; i++)
		{
			double thisPointDis = MathExt::sqr(i-cx) + MathExt::sqr(j-cy);
			if(thisPointDis<radSqr)
			{
				if(thisPointDis>strokeRad)
				{
					//stroke
					buffer->drawPixel(i, j, getStrokeColor());
				}
				else
				{
					//fill
					buffer->drawPixel(i, j, getFillColor());
				}
				
			}
		}
	}
}

void VectorCircle::drawStroke(Image* buffer, int globalWidth, int globalHeight)
{
	
}

void VectorCircle::setX(double x)
{
	this->cx = x;
}

double VectorCircle::getX()
{
	return cx;
}

void VectorCircle::setY(double y)
{
	this->cy = y;
}

double VectorCircle::getY()
{
	return cy;
}

void VectorCircle::setRadius(double r)
{
	this->radius = r;
}

double VectorCircle::getRadius()
{
	return this->radius;
}

#pragma endregion