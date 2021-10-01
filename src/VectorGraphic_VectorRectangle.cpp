#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

namespace glib
{
		
	#pragma region VectorRectangle

	const Class VectorRectangle::myClass = Class("VectorRectangle", {&VectorShape::myClass});
	const Class* VectorRectangle::getClass()
	{
		return &VectorRectangle::myClass;
	}

	VectorRectangle::VectorRectangle() : VectorShape()
	{
	}

	VectorRectangle::~VectorRectangle()
	{
	}

	void VectorRectangle::draw(Image* buffer, int globalWidth, int globalHeight)
	{
		if(Mat3f::getIdentity() != getTransform() || (rx!=0 && ry!=0))
		{
			//Note that this function draws a path version of the shape
			drawTransformed(buffer, globalWidth, globalHeight);
			return;
		}

		//Same regardless of anti aliasing status.

		//first, calc bounding box
		int x1 = MathExt::min(x, x + width);
		int x2 = MathExt::max(x, x + width);
		int y1 = MathExt::min(y, y + height);
		int y2 = MathExt::max(y, y + height);

		x1 = MathExt::clamp(x1, 0, globalWidth);
		y1 = MathExt::clamp(y1, 0, globalHeight);
		x2 = MathExt::clamp(x2, 0, globalWidth);
		y2 = MathExt::clamp(y2, 0, globalHeight);

		for (int ty = y1; ty <= y2; ty++)
		{
			for (int tx = x1; tx <= x2; tx++)
			{
				//Have to determine if we are on the stroke
				bool onStroke = false;

				if ((x2 - tx) >= 0 && (x2 - tx) < getStrokeWidth())
				{
					//likely in the correct space
					//setColor(x, y, strokeColor);
					onStroke=true;
				}
				else if ((tx - x1) >= 0 && (tx - x1) < getStrokeWidth())
				{
					//likely in the correct space
					//setColor(x, y, strokeColor);
					onStroke=true;
				}
				if ((y2 - ty) >= 0 && (y2 - ty) < getStrokeWidth())
				{
					//likely in the correct space
					//setColor(x, y, strokeColor);
					onStroke=true;
				}
				else if ((ty - y1) >= 0 && (ty - y1) < getStrokeWidth())
				{
					//likely in the correct space
					//setColor(x, y, strokeColor);
					onStroke=true;
				}

				if(!onStroke)
				{
					buffer->drawPixel(tx, ty, getFillColor());
				}
				else
				{
					buffer->drawPixel(tx, ty, getStrokeColor());
				}
				
			}

		}
	}

	void VectorRectangle::drawStroke(Image* buffer, int globalWidth, int globalHeight)
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

	void VectorRectangle::setX(double x)
	{
		this->x = x;
	}

	double VectorRectangle::getX()
	{
		return x;
	}

	void VectorRectangle::setY(double y)
	{
		this->y = y;
	}

	double VectorRectangle::getY()
	{
		return y;
	}

	void VectorRectangle::setRX(double rx)
	{
		this->rx = rx;
	}

	double VectorRectangle::getRX()
	{
		return rx;
	}

	void VectorRectangle::setRY(double ry)
	{
		this->ry = ry;
	}

	double VectorRectangle::getRY()
	{
		return ry;
	}

	void VectorRectangle::setWidth(double width)
	{
		this->width = width;
	}

	double VectorRectangle::getWidth()
	{
		return width;
	}

	void VectorRectangle::setHeight(double height)
	{
		this->height = height;
	}

	double VectorRectangle::getHeight()
	{
		return height;
	}

	void VectorRectangle::applyTransform()
	{
		//Not used.
	}

	void VectorRectangle::drawTransformed(Image* buffer, int globalWidth, int globalHeight)
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
		if(rx == 0 && ry == 0)
		{
			p.addMoveTo(x,y);
			p.addHorizontalTo(x+width);
			p.addVerticalTo(y+height);
			p.addHorizontalTo(x);
			p.addClosePath();
		}
		else
		{
			p.addMoveTo(x+rx, y);
			p.addHorizontalTo(x+width-rx);
			p.addArcToRel(rx,ry,0,0,1,rx,ry);
			p.addVerticalTo(y+height-ry);
			p.addArcToRel(rx,ry,0,0,1,-rx,ry);
			p.addHorizontalTo(x+rx);
			p.addArcToRel(rx,ry,0,0,1,-rx,-ry);
			p.addVerticalTo(y+ry);
			p.addArcToRel(rx,ry,0,0,1,rx,-ry);
			
			p.addClosePath();
		}

		p.draw(buffer, globalWidth, globalHeight);
	}

	void VectorRectangle::drawStrokeTransformed(Image* buffer, int globalWidth, int globalHeight)
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
		if(rx == 0 && ry == 0)
		{
			p.addMoveTo(x,y);
			p.addHorizontalTo(x+width);
			p.addVerticalTo(y+height);
			p.addHorizontalTo(x);
			p.addClosePath();
		}
		else
		{
			p.addMoveTo(x+rx, y);
			p.addHorizontalTo(x+width-rx);
			p.addArcToRel(rx,ry,0,0,1,rx,ry);
			p.addVerticalTo(y+height-ry);
			p.addArcToRel(rx,ry,0,0,1,-rx,ry);
			p.addHorizontalTo(x+rx);
			p.addArcToRel(rx,ry,0,0,1,-rx,-ry);
			p.addVerticalTo(y+ry);
			p.addArcToRel(rx,ry,0,0,1,rx,-ry);
			
			p.addClosePath();
		}

		p.drawStroke(buffer, globalWidth, globalHeight);
	}

	#pragma endregion

} //NAMESPACE glib END