#pragma once
#include <vector>
#include "Vec2f.h"
#include "Object.h"

class BezierCurve : public Object
{
public:
	BezierCurve();
	BezierCurve(const BezierCurve& o);
	~BezierCurve();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void addPoint(Vec2f p);
	Vec2f getPoint(int index);
	Vec2f getFuctionAt(double time);
	void clear();

private:
	Vec2f blendPoints(int i, double time);
	std::vector<Vec2f> points = std::vector<Vec2f>();
};

