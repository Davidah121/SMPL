#include "VectorGraphic.h"
#include <iostream>

#pragma region VectorShape

const Class* VectorShape::myClass = new Class("VectorShape", {Object::myClass});
const Class* VectorShape::getClass()
{
	return VectorShape::myClass;
}

void VectorShape::draw(Image* buffer)
{
	std::cout << "Basic one" << std::endl;
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

#pragma endregion

#pragma region VectorRectangle

const Class* VectorRectangle::myClass = new Class("VectorRectangle", {VectorShape::myClass});
const Class* VectorRectangle::getClass()
{
	return VectorRectangle::myClass;
}

VectorRectangle::VectorRectangle()
{
}

VectorRectangle::~VectorRectangle()
{
}

void VectorRectangle::draw(Image* buffer)
{
	//first, calc bounding box
	int x1 = MathExt::min(x, x + width);
	int x2 = MathExt::max(x, x + width);
	int y1 = MathExt::min(y, y + height);
	int y2 = MathExt::max(y, y + height);

	for (int y = y1-1; y < y2+1; y++)
	{
		for (int x = x1-1; x < x2+1; x++)
		{
			//Since this is a rectangle, we don't have to worry
				
			if (x >= x1 && x <= x2)
			{
				if (y >= y1 && y <= y2)
				{
					//setColor(x, y, fillColor);

					//Have to determine if we are on the stroke
					if ((x2 - x) >= 0 && (x2 - x) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						buffer->setPixel(x, y, getStrokeColor());
					}
					else if ((x - x1) >= 0 && (x - x1) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						buffer->setPixel(x, y, getStrokeColor());
					}
					else if ((y2 - y) >= 0 && (y2 - y) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						buffer->setPixel(x, y, getStrokeColor());
					}
					else if ((y - y1) >= 0 && (y - y1) < getStrokeWidth())
					{
						//likely in the correct space
						//setColor(x, y, strokeColor);
						buffer->setPixel(x, y, getStrokeColor());
					}
					else
					{
						buffer->setPixel(x, y, getFillColor());
					}
				}
			}
		}
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

#pragma endregion

#pragma region VectorGraphic

const Class* VectorGraphic::myClass = new Class("VectorGraphic", {Object::myClass});
const Class* VectorGraphic::getClass()
{
	return VectorGraphic::myClass;
}

VectorGraphic::VectorGraphic()
{
}

VectorGraphic::~VectorGraphic()
{
}

void VectorGraphic::addShape(VectorShape* k)
{
	shapes.push_back(k);
}

void VectorGraphic::clearShapes()
{
	shapes.clear();
}

void VectorGraphic::draw(Image* buffer)
{
	for (int i = 0; i < shapes.size(); i++)
	{
		shapes[i]->draw(buffer);
	}
}

#pragma endregion


