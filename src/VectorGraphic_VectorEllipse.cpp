#include "VectorGraphic.h"
#include "SimpleGraphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

namespace smpl
{
		
	#pragma region VectorEllipse

	const RootClass VectorEllipse::globalClass = CREATE_ROOT_CLASS(VectorEllipse, &VectorShape::globalClass);
    const RootClass* VectorEllipse::getClass()
	{
		return &VectorEllipse::globalClass;
	}

	VectorEllipse::VectorEllipse() : VectorShape()
	{
	}

	VectorEllipse::~VectorEllipse()
	{
	}

	void VectorEllipse::draw(Image* buffer, int globalWidth, int globalHeight)
	{

		if(Mat3f::getIdentity() != getTransform())
		{
			//A interesting note for transforms is that it does not have to
			//change into a path. An Ellipse could also be used in some situations
			//and translation does not require a change. Skew, and in some situations,
			//rotation and scale require a change
			
			//Note that this function draws a path version of the shape
			drawTransformed(buffer, globalWidth, globalHeight);
			return;
		}

		double preX,preY,preRX,preRY;
		preX = cx;
		preY = cy;
		preRX = rx;
		preRY = ry;

		//first, calc bounding box
		int x1 = (int)MathExt::round(cx-rx);
		int x2 = (int)MathExt::round(cx+rx);
		int y1 = (int)MathExt::round(cy-ry);
		int y2 = (int)MathExt::round(cy+ry);

		x1 = MathExt::clamp(x1, 0, globalWidth);
		y1 = MathExt::clamp(y1, 0, globalHeight);
		x2 = MathExt::clamp(x2, 0, globalWidth);
		y2 = MathExt::clamp(y2, 0, globalHeight);

		double radXSqr = MathExt::sqr(rx);
		double radYSqr = MathExt::sqr(ry);
		// double strokeXRad = MathExt::sqr(rx-this->getStrokeWidth()); //Not used
		// double strokeYRad = MathExt::sqr(ry-this->getStrokeWidth()); //Not used

		for(int j=y1; j<y2; j++)
		{
			for(int i=x1; i<x2; i++)
			{
				double ySide = MathExt::sqr(cy-j) / radYSqr;
				double xSide = MathExt::sqr(cx-i) / radXSqr;

				if(xSide+ySide <= 1)
				{

					// double yRadSide = MathExt::sqr(cy-j) / strokeXRad;
					// double xRadSide = MathExt::sqr(cx-i) / strokeYRad;

					// if(xRadSide+yRadSide != 1)
					// {
					// 	//stroke
					// 	buffer->drawPixel(i, j, getStrokeColor());
					// }
					// else
					// {
						//fill
						SimpleGraphics::drawPixel(i, j, getFillColor(), buffer);
					// }
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
		if(Mat3f::getIdentity() != getTransform())
		{
			//A interesting note for transforms is that it does not have to
			//change into a path. An Ellipse could also be used in some situations
			//and translation does not require a change. Skew, and in some situations,
			//rotation and scale require a change
			
			//Note that this function draws a path version of the shape
			drawStrokeTransformed(buffer, globalWidth, globalHeight);
			return;
		}
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
		//Does nothing
	}

	void VectorEllipse::drawTransformed(Image* buffer, int globalWidth, int globalHeight)
	{
		//convert to path then transform
		VectorPath p = VectorPath();

		p.setFillColor(getFillColor());
		p.setFillMethod(getFillMethod());
		p.setStrokeWidth(getStrokeWidth());
		p.setStrokeColor(getStrokeColor());
		p.setTransform(getTransform());
		p.setLineCap(getLineCap());
		p.setLineJoin(getLineJoin());

		//
		p.addMoveTo(cx-rx, cy);
		p.addArcTo(rx, ry, 0, false, false, cx+rx, cy);
		p.addArcTo(rx, ry, 0, false, false, cx-rx, cy);
		p.addClosePath();

		p.draw(buffer, globalWidth, globalHeight);
	}

	void VectorEllipse::drawStrokeTransformed(Image* buffer, int globalWidth, int globalHeight)
	{
		//convert to path then transform
		VectorPath p = VectorPath();

		p.setFillColor(getFillColor());
		p.setFillMethod(getFillMethod());
		p.setStrokeWidth(getStrokeWidth());
		p.setStrokeColor(getStrokeColor());
		p.setTransform(getTransform());
		p.setLineCap(getLineCap());
		p.setLineJoin(getLineJoin());

		//
		p.addMoveTo(cx-rx, cy);
		p.addArcTo(rx, ry, 0, false, false, cx+rx, cy);
		p.addArcTo(rx, ry, 0, false, false, cx-rx, cy);
		p.addClosePath();

		p.drawStroke(buffer, globalWidth, globalHeight);
	}
	#pragma endregion

} //NAMESPACE glib END