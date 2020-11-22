#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

#pragma region VectorEllipse

const Class* VectorEllipse::myClass = new Class("VectorEllipse", {VectorShape::myClass});
const Class* VectorEllipse::getClass()
{
	return VectorEllipse::myClass;
}

VectorEllipse::VectorEllipse() : VectorShape()
{
}

VectorEllipse::~VectorEllipse()
{
}

void VectorEllipse::draw(Image* buffer, int globalWidth, int globalHeight)
{
	double preX,preY,preRX,preRY;
	preX = cx;
	preY = cy;
	preRX = rx;
	preRY = ry;

	applyTransform();

	//first, calc bounding box
	int x1 = cx-rx;
	int x2 = cx+rx;
	int y1 = cy-ry;
	int y2 = cy+ry;

	x1 = MathExt::clamp(x1, 0, globalWidth);
	y1 = MathExt::clamp(y1, 0, globalHeight);
	x2 = MathExt::clamp(x2, 0, globalWidth);
	y2 = MathExt::clamp(y2, 0, globalHeight);

	double radXSqr = MathExt::sqr(rx);
	double radYSqr = MathExt::sqr(ry);
	double strokeXRad = MathExt::sqr(rx-this->getStrokeWidth());
	double strokeYRad = MathExt::sqr(ry-this->getStrokeWidth());

	for(int j=y1; j<y2; j++)
	{
		for(int i=x1; i<x2; i++)
		{
			double ySide = MathExt::sqr(cy-j) / radYSqr;
			double xSide = MathExt::sqr(cx-i) / radXSqr;

			if(xSide+ySide == 1)
			{

				double yRadSide = MathExt::sqr(cy-j) / strokeXRad;
				double xRadSide = MathExt::sqr(cx-i) / strokeYRad;

				if(xRadSide+yRadSide != 1)
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

	cx = preX;
	cy = preY;
	rx = preRX;
	ry = preRY;
}

void VectorEllipse::drawStroke(Image* buffer, int globalWidth, int globalHeight)
{
	
}

void VectorEllipse::setX(double x)
{
	this->cx = x;
}

double VectorEllipse::getX()
{
	return cx;
}

void VectorEllipse::setY(double y)
{
	this->cy = y;
}

double VectorEllipse::getY()
{
	return cy;
}

void VectorEllipse::setXRadius(double r)
{
	this->rx = r;
}

double VectorEllipse::getXRadius()
{
	return this->rx;
}

void VectorEllipse::setYRadius(double r)
{
	this->ry = r;
}

double VectorEllipse::getYRadius()
{
	return this->ry;
}

void VectorEllipse::applyTransform()
{
	Vec3f pos = Vec3f(cx, cy, 1.0);
	Vec3f r = Vec3f(rx, ry, 0.0);
	
	Vec3f newPos = getTransform() * pos;
	Vec3f newR = getTransform() * r;

	cx = newPos.x;
	cy = newPos.y;

	//deal with radius later
}
#pragma endregion