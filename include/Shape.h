#pragma once
#include "MathExt.h"
#include <vector>
#include "Object.h"

class Shape : public Object
{
public:
	Shape();
	~Shape();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void setPosition(Vec3f* posPointer);
	Vec3f* getPosition();

	void setScale(Vec3f* posScale);
	Vec3f* getScale();

	void setRotation(Vec3f* posRotation);
	Vec3f* getRotation();
	
protected:
	Vec3f* position = new Vec3f();
	Vec3f* scale = new Vec3f();
	Vec3f* rotation = new Vec3f();

};

#pragma region SHAPES_2D

class Point2D : public Shape
{
public:
	Point2D(Vec2f pos);
	~Point2D();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
};

class Box2D : public Shape
{
public:
	Box2D(float leftBound, float topBound, float rightBound, float bottomBound);
	~Box2D();
	
	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	void setLeftBound(float lb);
	float getLeftBound();

	void setTopBound(float tb);
	float getTopBound();

	void setRightBound(float rb);
	float getRightBound();

	void setBottomBound(float bb);
	float getBottomBound();

	
private:
	
	float lBound = 0;
	float rBound = 0;
	float tBound = 0;
	float bBound = 0;
};

class Circle : public Shape
{
public:
	Circle(float rad);
	~Circle();

	float getRadius();
	void setRadius(float rad);

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
private:

	float radius = 0;
};

class Line2D : public Shape
{
public:
	Line2D(float x1, float y1, float x2, float y2);
	Line2D(Vec2f p1, Vec2f p2);
	~Line2D();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	
	void setPoint1(Vec2f p);
	void setPoint1(float x, float y);

	void setPoint2(Vec2f p);
	void setPoint2(float x, float y);

	Vec2f getPoint1();
	Vec2f getPoint2();

	float getSlope();
	float getSlopeRelativeY();
	float getYInt();
	float getXInt();

	float getMinX();
	float getMaxX();
	float getMinY();
	float getMaxY();

private:

	Vec2f v1;
	Vec2f v2;
};

class Triangle2D : public Shape
{
public:
	Triangle2D(Vec2f p1, Vec2f p2, Vec2f p3);
	Triangle2D(float x1, float y1, float x2, float y2, float x3, float y3);
	~Triangle2D();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;
	

	void setVertex1(Vec2f p);
	void setVertex1(float x, float y);

	void setVertex2(Vec2f p);
	void setVertex2(float x, float y);

	void setVertex3(Vec2f p);
	void setVertex3(float x, float y);

	Vec2f getVertex1();
	Vec2f getVertex2();
	Vec2f getVertex3();

private:

	Vec2f v1;
	Vec2f v2;
	Vec2f v3;
};

#pragma endregion

#pragma region SHAPES_3D

class Sphere : public Shape
{

};

class Cylinder : public Shape
{

};

class Box3D : public Shape
{

};

class Point3D : public Shape
{

};

class Line3D : public Shape
{

};

class Triangle3D : public Shape
{

};

#pragma endregion

class CollisionMaster
{
public:
	//Returns false if there is no matching function for both shapes
	static bool getCollision(Shape* a, Shape* b);

	//Pre made collision functions

	//2D stuff
	//Point2D
	static bool collisionMethod(Point2D* a, Point2D* b);
	static bool collisionMethod(Point2D* a, Circle* b);
	static bool collisionMethod(Point2D* a, Box2D* b);
	static bool collisionMethod(Point2D* a, Triangle2D* b);
	static bool collisionMethod(Point2D* a, Line2D* b);

	//Box2D
	static bool collisionMethod(Box2D* a, Box2D* b);
	static bool collisionMethod(Box2D* a, Circle* b);
	static bool collisionMethod(Box2D* a, Triangle2D* b);
	static bool collisionMethod(Box2D* a, Line2D* b);

	//Circle
	static bool collisionMethod(Circle* a, Circle* b);
	static bool collisionMethod(Circle* a, Triangle2D* b);
	static bool collisionMethod(Circle* a, Line2D* b);

	//Line2D
	static bool collisionMethod(Line2D* a, Line2D* b);
	static bool collisionMethod(Line2D* a, Triangle2D* b);

	//Triangle2D
	static bool collisionMethod(Triangle2D* a, Triangle2D* b);


	//Used for any additional functions
	static void setExternalCollisionFunction(bool(*extFunction)(Shape* a, Shape* b));
private:
	static bool (*extCollision)(Shape* a, Shape* b);
};