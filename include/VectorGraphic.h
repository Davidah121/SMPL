#pragma once
#include<vector>
#include "Image.h"
#include "MathExt.h"

class VectorShape : public Object
{
public:
	static const bool EVEN_ODD_RULE = false;
	static const bool NON_ZERO = true;

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	virtual void draw(Image* buffer);

	void setFillMethod(bool value);
	bool getFillMethod();
	
	void setFillColor(Color k);
	Color getFillColor();

	void setStrokeColor(Color k);
	Color getStrokeColor();

	void setStrokeWidth(double k);
	double getStrokeWidth();

private:
	Color fill;
	Color strokeColor;
	double strokeWidth = 0;
	bool fillMethod = false;
};

class VectorRectangle : public VectorShape
{
public:
	VectorRectangle();
	~VectorRectangle();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void draw(Image* buffer);

	void setX(double x);
	double getX();

	void setY(double y);
	double getY();

	void setRX(double rx);
	double getRX();

	void setRY(double ry);
	double getRY();

	void setWidth(double width);
	double getWidth();

	void setHeight(double height);
	double getHeight();
private:

	double x = 0;
	double y = 0;
	double rx = 0;
	double ry = 0;
	double width = 0;
	double height = 0;
};

class VectorGraphic : public Object
{
public:

	static const unsigned char RECTANGLE = 1;
	static const unsigned char CIRCLE = 2;
	static const unsigned char ELLIPSE = 3;
	static const unsigned char LINE = 4;
	static const unsigned char POLYLINE = 5;
	static const unsigned char POLYGON = 6;
	static const unsigned char PATH = 7;

	VectorGraphic();
	~VectorGraphic();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void addShape(VectorShape* k);
	void clearShapes();

	void draw(Image* buffer);
private:
	std::vector<VectorShape*> shapes = std::vector<VectorShape*>();

	//transforms
	Mat3f transform = Mat3f();
};

