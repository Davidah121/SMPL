#pragma once
#include "Vec2f.h"
#include "Object.h"

class Line : public Object
{
public:
	Line();
	Line(int x1, int y1, int x2, int y2);
	Line(float x1, float y1, float x2, float y2);
	Line(double x1, double y1, double x2, double y2);
	Line(Vec2f p1, Vec2f p2);

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	Vec2f getPoint1();
	Vec2f getPoint2();
	Vec2f getToPoint();
	
	double getMaxX();
	double getMinX();

	double getMinY();
	double getMaxY();

	~Line();

private:
	void setMinMax();

	Vec2f point1 = Vec2f(0,0);
	Vec2f point2 = Vec2f(0, 0);
	Vec2f toPoint = Vec2f(0, 0);

	double minX = 0, maxX = 0, minY = 0, maxY = 0;

};

