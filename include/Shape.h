#pragma once
#include <vector>
#include "MathExt.h"
#include "Object.h"

namespace glib
{
	
	class Shape : public Object
	{
	public:
		Shape();
		~Shape();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void setPosition(Vec3f posPointer);
		Vec3f getPosition();

		void setScale(Vec3f posScale);
		Vec3f getScale();

		void setRotation(Vec3f posRotation);
		Vec3f getRotation();

		virtual void transform();
		
	protected:
		Vec3f position = Vec3f();
		Vec3f scale = Vec3f();
		Vec3f rotation = Vec3f();
	};

	class CombinationShape : public Shape
	{
	public:
		CombinationShape();
		~CombinationShape();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		void addShape(Shape s);
		Shape getShape(int i);
		int size();

	private:
		std::vector<Shape> shapes = std::vector<Shape>();
	};

	#pragma region SHAPES_2D

	class Point2D : public Shape
	{
	public:
		Point2D();
		Point2D(Vec2f pos);
		~Point2D();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
	};

	class Box2D : public Shape
	{
	public:
		Box2D();
		Box2D(double leftBound, double topBound, double rightBound, double bottomBound);
		~Box2D();
		
		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		void setLeftBound(double lb);
		double getLeftBound();

		void setTopBound(double tb);
		double getTopBound();

		void setRightBound(double rb);
		double getRightBound();

		void setBottomBound(double bb);
		double getBottomBound();

		
	private:
		double lBound = 0;
		double rBound = 0;
		double tBound = 0;
		double bBound = 0;
	};

	class Circle : public Shape
	{
	public:
		Circle();
		Circle(double rad);
		~Circle();

		double getRadius();
		void setRadius(double rad);

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
	private:
		double radius = 0;
	};

	class Ellipse : public Shape
	{
	public:
		Ellipse();
		Ellipse(double xRad, double yRad);
		~Ellipse();

		double getXRadius();
		void setXRadius(double rad);
		double getYRadius();
		void setYRadius(double rad);

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
	private:
		double xRadius = 0;
		double yRadius = 0;

	};

	class Line2D : public Shape
	{
	public:
		Line2D();
		Line2D(double x1, double y1, double x2, double y2);
		Line2D(Vec2f p1, Vec2f p2);
		~Line2D();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		
		void setPoint1(Vec2f p);
		void setPoint1(double x, double y);

		void setPoint2(Vec2f p);
		void setPoint2(double x, double y);

		Vec2f getPoint1();
		Vec2f getPoint2();

		double getSlope();
		double getSlopeRelativeY();
		double getYInt();
		double getXInt();

		double getMinX();
		double getMaxX();
		double getMinY();
		double getMaxY();

	private:

		Vec2f v1;
		Vec2f v2;
	};

	class Triangle2D : public Shape
	{
	public:
		Triangle2D();
		Triangle2D(Vec2f p1, Vec2f p2, Vec2f p3);
		Triangle2D(double x1, double y1, double x2, double y2, double x3, double y3);
		~Triangle2D();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;
		

		void setVertex1(Vec2f p);
		void setVertex1(double x, double y);

		void setVertex2(Vec2f p);
		void setVertex2(double x, double y);

		void setVertex3(Vec2f p);
		void setVertex3(double x, double y);

		Vec2f getVertex1();
		Vec2f getVertex2();
		Vec2f getVertex3();

	private:

		Vec2f v1;
		Vec2f v2;
		Vec2f v3;
	};

	class Polygon2D : public Shape
	{
	public:
		Polygon2D();
		~Polygon2D();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		void addVertex(Vec2f p);
		Vec2f getVertex(int index);
		Vec2f getCenterPoint();
		void clear();
		size_t size();

		bool getConvex();
	private:
		void checkConcave();
		std::vector<Vec2f> points;
		Vec2f centerPoint;
		bool isConvex = true;
	};

	#pragma endregion

	#pragma region SHAPES_3D

	class Sphere : public Shape
	{

	};

	class Ellipsoid : public Shape
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

	class Polygon3D : public Shape
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
		static bool collisionMethod(Point2D* a, Ellipse* b);
		static bool collisionMethod(Point2D* a, Box2D* b);
		static bool collisionMethod(Point2D* a, Triangle2D* b);
		static bool collisionMethod(Point2D* a, Line2D* b);

		//Box2D
		static bool collisionMethod(Box2D* a, Box2D* b);
		static bool collisionMethod(Box2D* a, Circle* b);
		static bool collisionMethod(Box2D* a, Ellipse* b);
		static bool collisionMethod(Box2D* a, Triangle2D* b); //TODO - MATH DONE
		static bool collisionMethod(Box2D* a, Line2D* b);

		//Circle
		static bool collisionMethod(Circle* a, Circle* b);
		static bool collisionMethod(Circle* a, Ellipse* b); //TODO
		static bool collisionMethod(Circle* a, Triangle2D* b); //TODO
		static bool collisionMethod(Circle* a, Line2D* b);

		//Line2D
		static bool collisionMethod(Line2D* a, Line2D* b);
		static bool collisionMethod(Line2D* a, Ellipse* b); //TODO
		static bool collisionMethod(Line2D* a, Triangle2D* b); //TODO

		//Ellipse
		static bool collisionMethod(Ellipse* a, Ellipse* b); //TODO
		static bool collisionMethod(Ellipse* a, Triangle2D* b); //TODO

		//Triangle2D
		static bool collisionMethod(Triangle2D* a, Triangle2D* b); //TODO - MATH DONE

		//Polygon Stuff
		static bool SeparatingAxisTheorem(Polygon2D* a, Polygon2D* b);
		static bool AlternatingDiagonals(Polygon2D* a, Polygon2D* b);
		static bool collisionMethod(Polygon2D* a, Point2D* b);
		static bool collisionMethod(Polygon2D* a, Circle* b);
		static bool collisionMethod(Polygon2D* a, Ellipse* b);

		//Used for any additional functions
		static void setExternalCollisionFunction(bool(*extFunction)(Shape* a, Shape* b));
	private:
		static bool (*extCollision)(Shape* a, Shape* b);
	};

}  //NAMESPACE glib END
