#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

#pragma region VectorRectangle

const Class* VectorRectangle::myClass = new Class("VectorRectangle", {VectorShape::myClass});
const Class* VectorRectangle::getClass()
{
	return VectorRectangle::myClass;
}

VectorRectangle::VectorRectangle() : VectorShape()
{
}

VectorRectangle::~VectorRectangle()
{
}

void VectorRectangle::draw(Image* buffer, int globalWidth, int globalHeight)
{
	//first, calc bounding box
	int x1 = MathExt::min(x, x + width);
	int x2 = MathExt::max(x, x + width);
	int y1 = MathExt::min(y, y + height);
	int y2 = MathExt::max(y, y + height);

	x1 = MathExt::clamp(x1, 0, globalWidth);
	y1 = MathExt::clamp(y1, 0, globalHeight);
	x2 = MathExt::clamp(x2, 0, globalWidth);
	y2 = MathExt::clamp(y2, 0, globalHeight);

	//some rules: for rx and ry, there is a ellipse at each corner that
	//determines if it is in the thing.
	//The min and max x values change based on the y value.
	//They could be the x1 and x2 values but not always.

	int xCenterLeft = x+rx;
	int xCenterRight = x+width-rx;
	int yCenterTop = y+ry;
	int yCenterBottom = y+height-ry;

	for (int ty = y1; ty <= y2; ty++)
	{
		//first valid x on scanline
		int tx1 = x1;
		//last valid x on scanline
		int tx2 = x2;

		if(ty < yCenterTop)
		{
			//top corner
			// ((tx - x)^2 / rx^2) + ((ty - y)^2 / ry^2) = 1
			// (tx - x)^2 = rx^2 * (1 - ((ty - y)^2 / ry^2))
			// x = sqrt( rx^2 * (1 - ((ty - y)^2 / ry^2)) ) + tx

			int temp = MathExt::sqrt( MathExt::sqr(rx) * (1-(MathExt::sqr(ty-yCenterTop)/MathExt::sqr(ry) ) ) );
			
			//left side of rectangle
			tx1 = xCenterLeft - temp;
			//right side of rectangle
			tx2 = xCenterRight + temp;
		}
		else if(ty > yCenterBottom)
		{
			//bottom corner

			int temp = MathExt::sqrt( MathExt::sqr(rx) * (1-(MathExt::sqr(ty-yCenterBottom)/MathExt::sqr(ry) ) ) );
			
			//left side of rectangle
			tx1 = xCenterLeft - temp;
			//right side of rectangle
			tx2 = xCenterRight + temp;
		}

		for (int tx = tx1; tx <= tx2; tx++)
		{
			//first valid y on column
			int ty1 = y1;
			//last valid y on column
			int ty2 = y2;

			if(tx < xCenterLeft)
			{
				//top corner
				int temp = MathExt::sqrt( MathExt::sqr(ry) * (1-(MathExt::sqr(tx-xCenterLeft)/MathExt::sqr(rx) ) ) );
				
				//left side of rectangle
				ty1 = yCenterTop - temp;
				//right side of rectangle
				ty2 = yCenterBottom + temp;
			}
			else if(tx > xCenterRight)
			{
				//bottom corner
				int temp = MathExt::sqrt( MathExt::sqr(ry) * (1-(MathExt::sqr(tx-xCenterRight)/MathExt::sqr(rx) ) ) );
				
				//left side of rectangle
				ty1 = yCenterTop - temp;
				//right side of rectangle
				ty2 = yCenterBottom + temp;
			}
			
			if (tx >= tx1 && tx <= tx2)
			{
				if (ty >= ty1 && ty <= ty2)
				{
					//setColor(x, y, fillColor);

					//Have to determine if we are on the stroke
					bool onStroke = false;

					if ((tx2 - tx) >= 0 && (tx2 - tx) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						onStroke=true;
					}
					else if ((tx - tx1) >= 0 && (tx - tx1) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						onStroke=true;
					}
					if ((ty2 - ty) >= 0 && (ty2 - ty) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						onStroke=true;
					}
					else if ((ty - ty1) >= 0 && (ty - ty1) < getStrokeWidth())
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

	}
}

void VectorRectangle::drawStroke(Image* buffer, int globalWidth, int globalHeight)
{
	
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
	Vec3f pos = getTransform() * Vec3f(x,y,1.0);
	Vec3f scale = getTransform() * Vec3f(width,height,0.0);
	Vec3f rScale = getTransform() * Vec3f(rx,ry,0.0);
	
	//Not correct
	x = pos.x;
	y = pos.y;
	width = scale.x;
	height = scale.y;
}

#pragma endregion