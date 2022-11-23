#pragma once
#include <vector>
#include <functional>
#include "MathExt.h"
#include "Object.h"
#include "BezierCurve.h"

namespace glib
{
	class Shape : public Object
	{
	public:
		/**
		 * @brief Constructs a generic shape object for Collision.
		 * 		Note that this class should be extended using subclasses.
		 * 		Contains general information such as position, scale, and rotation.
		 */
		Shape();

		/**
		 * @brief Destroy the Shape object
		 * 
		 */
		~Shape();

		//Object and Class Stuff
		static const Class globalClass;

		/**
		 * @brief Set the Position of the shape.
		 * 
		 * @param pos
		 * 		A 3D vector representing position. If the shape is 2D, the z axis can be ignored. 
		 */
		void setPosition(Vec3f pos);

		/**
		 * @brief Get the Position of the shape.
		 * 
		 * @return Vec3f 
		 */
		Vec3f getPosition();

		/**
		 * @brief Set the Scale of the shape.
		 * 		Note that shome shapes may not factor in the scale.
		 * 
		 * @param sca 
		 * 		A 3D vector representing scale. If the shape is 2D, the z axis can be ignored. 
		 * 		
		 */
		void setScale(Vec3f sca);

		/**
		 * @brief Get the Scale of the shape.
		 * 
		 * @return Vec3f 
		 */
		Vec3f getScale();

		/**
		 * @brief Set the Rotation of the shape.
		 * 		Note that some shapes may not factor in the rotation.
		 * 
		 * @param rot 
		 * 		A 3D vector representing rotation. If the shape is 2D, the y and z axis can be ignored. 
		 * 
		 */
		void setRotation(Vec3f rot);

		/**
		 * @brief Get the Rotation of the shape.
		 * 
		 * @return Vec3f 
		 */
		Vec3f getRotation();

		/**
		 * @brief A virtual function to create either a bounding circle or bounding sphere.
		 * 		Useful for eliminating collisions.
		 * 
		 * @return double 
		 */
		virtual double generateBoundingRadius();
		
	protected:
		Vec3f position = Vec3f();
		Vec3f scale = Vec3f(1,1,1);
		Vec3f rotation = Vec3f();

		virtual void onTransformChanged();
	};

	class CombinationShape : public Shape
	{
	public:
		/**
		 * @brief Construct a new Combination Shape object
		 * 		Contains a list of shapes that make up the current shape.
		 * 		If either shape collides with something, it is considered a collision.
		 * 
		 * 		Note that this object also contains a position, scale, and rotation. 
		 * 		These will be applied before the shapes transforms are.
		 * 
		 */
		CombinationShape();

		/**
		 * @brief Destroy the Combination Shape object
		 * 
		 */
		~CombinationShape();

		//Object and Class Stuff
		static const Class globalClass;
		
		/**
		 * @brief Adds a shape to the list.
		 * 
		 * @param s 
		 * 		A valid shape.
		 */
		void addShape(Shape s);

		/**
		 * @brief Gets a Shape from the list.
		 * 
		 * @param i 
		 * 		The index of the shape.
		 * @return Shape 
		 */
		Shape getShape(int i);

		/**
		 * @brief Returns the amount of shapes in the list.
		 * 
		 * @return int 
		 */
		int size();

	protected:
		void onTransformChanged();

	private:
		std::vector<Shape> shapes = std::vector<Shape>();
	};

	#pragma region SHAPES_2D

	class Point2D : public Shape
	{
	public:
		/**
		 * @brief Construct a new Point2D object
		 * 		Default position is (0,0)
		 * 
		 */
		Point2D();

		/**
		 * @brief Construct a new Point2D object
		 * 
		 * @param x
		 * 		The x position
		 * @param y
		 * 		The y position
		 */
		Point2D(double x, double y);

		/**
		 * @brief Construct a new Point2D object
		 * 
		 * @param pos
		 * 		The position to set the point at.
		 */
		Point2D(Vec2f pos);

		/**
		 * @brief Destroy the Point2D object
		 * 
		 */
		~Point2D();

		//Object and Class Stuff
		static const Class globalClass;

		/**
		 * @brief Generates the bounding radius for the point.
		 * 		Returns the value 0 because it is a point.
		 * 
		 * @return double 
		 */
		double generateBoundingRadius();
		
	protected:
		void onTransformChanged();
	};

	class Box2D : public Shape
	{
	public:
		/**
		 * @brief Construct a new Box2D object
		 * 		It represents an Axis Aligned Bounding Box (AABB) so rotation does not
		 * 		affect the shape.
		 * 		Default bounds are all 0.
		 * 		
		 */
		Box2D();

		/**
		 * @brief Construct a new Box2D object
		 * 		It represents an Axis Aligned Bounding Box (AABB) so rotation does not
		 * 		affect the shape.
		 * 
		 * @param leftBound 
		 * @param topBound 
		 * @param rightBound 
		 * @param bottomBound 
		 */
		Box2D(double leftBound, double topBound, double rightBound, double bottomBound);
		~Box2D();
		
		//Object and Class Stuff
		static const Class globalClass;
		
		/**
		 * @brief Set the Left Bound of the AABB
		 * 
		 * @param lb 
		 */
		void setLeftBound(double lb);

		/**
		 * @brief Get the Left Bound of the AABB
		 * 
		 * @return double 
		 */
		double getLeftBound();

		/**
		 * @brief Set the Top Bound of the AABB
		 * 
		 * @param tb 
		 */
		void setTopBound(double tb);

		/**
		 * @brief Get the Top Bound of the AABB
		 * 
		 * @return double 
		 */
		double getTopBound();

		/**
		 * @brief Set the Right Bound of the AABB
		 * 
		 * @param tb 
		 */
		void setRightBound(double rb);

		/**
		 * @brief Get the Right Bound of the AABB
		 * 
		 * @return double 
		 */
		double getRightBound();

		/**
		 * @brief Set the Bottom Bound of the AABB
		 * 
		 * @param tb 
		 */
		void setBottomBound(double bb);

		/**
		 * @brief Get the Bottom Bound of the AABB
		 * 
		 * @return double 
		 */
		double getBottomBound();

		/**
		 * @brief Gets the Width of the AABB
		 * 
		 * @return double 
		 */
		double getWidth();

		/**
		 * @brief Gets the Height of the AABB
		 * 
		 * @return double 
		 */
		double getHeight();

		/**
		 * @brief Generates a bounding radius for the AABB.
		 * 		(Implement later)
		 * @return double 
		 */
		double generateBoundingRadius();

		bool operator==(Box2D other);
		bool operator!=(Box2D other);
		
	protected:
		void onTransformChanged();

	private:
		Vec2f topLeft;
		Vec2f bottomRight;

		Vec2f baseTopLeft;
		Vec2f baseBottomRight;
		
	};

	class Circle : public Shape
	{
	public:
		/**
		 * @brief Construct a new Circle object
		 * 		A Circle is separate from an Ellipse so scale will not affect this shape.
		 * 		Rotation will not affect the shape either.
		 * 		Default radius is 0.
		 * 
		 */
		Circle();

		/**
		 * @brief Construct a new Circle object
		 * 		A Circle is separate from an Ellipse so scale will not affect this shape.
		 * 		Rotation will not affect the shape either.
		 * 
		 * @param rad 
		 */
		Circle(double rad);

		/**
		 * @brief Destroy the Circle object
		 * 
		 */
		~Circle();

		/**
		 * @brief Get the Radius of the Circle
		 * 
		 * @return double 
		 */
		double getRadius();

		/**
		 * @brief Set the Radius of the Circle
		 * 
		 * @param rad 
		 */
		void setRadius(double rad);

		//Object and Class Stuff
		static const Class globalClass;

		/**
		 * @brief Generates the bounding radius for the circle.
		 * 		Returns the radius of the circle.
		 * 
		 * @return double 
		 */
		double generateBoundingRadius();
		
	protected:
		void onTransformChanged();

	private:
		double radius = 0;
		double baseRadius;
	};

	class Ellipse : public Shape
	{
	public:
		/**
		 * @brief Construct a new Ellipse object
		 * 		All transforms effect this shape. (Not yet implemented)
		 * 		Default xRadius and yRadius are 0.
		 * 
		 */
		Ellipse();

		/**
		 * @brief Construct a new Ellipse object
		 * 		All transforms effect this shape. (Not yet implemented)
		 * 
		 * @param xRad 
		 * @param yRad 
		 */
		Ellipse(double xRad, double yRad);

		/**
		 * @brief Destroy the Ellipse object
		 * 
		 */
		~Ellipse();

		/**
		 * @brief Gets the X radius of the Ellipse.
		 * 
		 * @return double 
		 */
		double getXRadius();

		/**
		 * @brief Sets the X radius of the Ellipse.
		 * 
		 * @param rad 
		 */
		void setXRadius(double rad);

		/**
		 * @brief Gets the Y radius of the Ellipse.
		 * 
		 * @return double 
		 */
		double getYRadius();

		/**
		 * @brief Sets the Y radius of the Ellipse.
		 * 
		 * @param rad 
		 */
		void setYRadius(double rad);

		//Object and Class Stuff
		static const Class globalClass;
		
		/**
		 * @brief Generates the bounding radius of the Ellipse.
		 * 
		 * @return double 
		 */
		double generateBoundingRadius();
	
	protected:
		void onTransformChanged();

	private:
		double xRadius = 0;
		double yRadius = 0;

		double baseXRadius = 0;
		double baseYRadius = 0;
	};

	class Line2D : public Shape
	{
	public:
		/**
		 * @brief Construct a new Line2D object
		 * 		All transforms affect this shape. (Not implemented)
		 * 		Default line is from (0,0) to (0,0)
		 * 
		 */
		Line2D();

		/**
		 * @brief Construct a new Line2D object
		 * 		All transforms affect this shape. (Not implemented)
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 */
		Line2D(double x1, double y1, double x2, double y2);

		/**
		 * @brief Construct a new Line2D object
		 * 		All transforms affect this shape. (Not implemented)
		 * 
		 * @param p1 
		 * @param p2 
		 */
		Line2D(Vec2f p1, Vec2f p2);

		/**
		 * @brief Destroy the Line2D object
		 * 
		 */
		~Line2D();

		//Object and Class Stuff
		static const Class globalClass;
		
		/**
		 * @brief Sets the first point of the line
		 * 
		 * @param p 
		 */
		void setPoint1(Vec2f p);

		/**
		 * @brief Sets the first point of the line
		 * 
		 * @param x 
		 * @param y 
		 */
		void setPoint1(double x, double y);

		/**
		 * @brief Sets the second point of the line
		 * 
		 * @param p 
		 */
		void setPoint2(Vec2f p);

		/**
		 * @brief Sets the second point of the line
		 * 
		 * @param x 
		 * @param y 
		 */
		void setPoint2(double x, double y);

		/**
		 * @brief Gets the first point of the line
		 * 
		 * @return Vec2f 
		 */
		Vec2f getPoint1();

		/**
		 * @brief Gets the second point of the line
		 * 
		 * @return Vec2f 
		 */
		Vec2f getPoint2();

		/**
		 * @brief Gets the Line that is represented by the 2 points.
		 * 
		 * @return Line 
		 */
		Line getLine();

		/**
		 * @brief Generates a bounding radius for the Line.
		 * 
		 * @return double 
		 */
		double generateBoundingRadius();

	protected:
		void onTransformChanged();

	private:
		Line baseL;
		Line l;
	};

	class Triangle2D : public Shape
	{
	public:
		/**
		 * @brief Construct a new Triangle2D object
		 * 		All transforms affect this shape.
		 * 		Default position are all (0,0)
		 * 
		 */
		Triangle2D();

		/**
		 * @brief Construct a new Triangle2D object
		 * 		All transforms affect this shape.
		 * 
		 * @param p1 
		 * @param p2 
		 * @param p3 
		 */
		Triangle2D(Vec2f p1, Vec2f p2, Vec2f p3);

		/**
		 * @brief Construct a new Triangle2D object
		 * 		All transforms affect this shape.
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 * @param x3 
		 * @param y3 
		 */
		Triangle2D(double x1, double y1, double x2, double y2, double x3, double y3);

		/**
		 * @brief Destroy the Triangle2D object
		 * 
		 */
		~Triangle2D();

		//Object and Class Stuff
		static const Class globalClass;
		
		/**
		 * @brief Sets the first vertex of the triangle
		 * 
		 * @param p 
		 */
		void setVertex1(Vec2f p);

		/**
		 * @brief Sets the first vertex of the triangle
		 * 
		 * @param x 
		 * @param y 
		 */
		void setVertex1(double x, double y);

		/**
		 * @brief Sets the second vertex of the triangle
		 * 
		 * @param p 
		 */
		void setVertex2(Vec2f p);

		/**
		 * @brief Sets the second vertex of the triangle
		 * 
		 * @param x 
		 * @param y 
		 */
		void setVertex2(double x, double y);

		/**
		 * @brief Sets the third vertex of the triangle
		 * 
		 * @param p 
		 */
		void setVertex3(Vec2f p);

		/**
		 * @brief Sets the third vertex of the triangle
		 * 
		 * @param x 
		 * @param y 
		 */
		void setVertex3(double x, double y);

		/**
		 * @brief Gets the first vertex of the triangle after transforms
		 * 
		 * @return Vec2f 
		 */
		Vec2f getVertex1();

		/**
		 * @brief Gets the first vertex of the triangle after transforms
		 * 
		 * @return Vec2f 
		 */
		Vec2f getVertex2();

		/**
		 * @brief Gets the first vertex of the triangle after transforms
		 * 
		 * @return Vec2f 
		 */
		Vec2f getVertex3();

		/**
		 * @brief Get the Center Position of the triangle after transforms
		 * 
		 * @return Vec2f 
		 */
		Vec2f getCenterPosition();

		/**
		 * @brief Generates a bounding radius for the triangle.
		 * 
		 * @return double 
		 */
		double generateBoundingRadius();
	
	protected:
		void onTransformChanged(); //TODO

	private:
		Vec2f v1;
		Vec2f v2;
		Vec2f v3;
	};

	class Polygon2D : public Shape
	{
	public:
		/**
		 * @brief Construct a new Polygon2D object
		 * 		All transforms affect this shape.
		 * 		Maintains a list of points that make up the shape.
		 * 		Collision methods exist only for convex polygons.
		 * 
		 */
		Polygon2D();

		/**
		 * @brief Destroy the Polygon2D object
		 * 
		 */
		~Polygon2D();

		//Object and Class Stuff
		static const Class globalClass;

		/**
		 * @brief Adds a vertex to the polygon.
		 * 		The center point will be updated as well as whether it is convex or concave.
		 * 
		 * @param p 
		 */
		void addVertex(Vec2f p);

		/**
		 * @brief Gets a vertex from the polygon at the specified index.
		 * 
		 * @param index 
		 * @return Vec2f 
		 */
		Vec2f getVertex(int index);

		/**
		 * @brief Gets the Center Point of the polygon.
		 * 
		 * @return Vec2f 
		 */
		Vec2f getCenterPoint();

		/**
		 * @brief Clears the list of vertices
		 * 
		 */
		void clear();

		/**
		 * @brief Returns the amount of vertices the polygon has.
		 * 
		 * @return size_t 
		 */
		size_t size();

		/**
		 * @brief Returns whether the polygon is convex or concave.
		 * 		If the shape is concave, none of the pre-made collision methods will work.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getConvex();

		/**
		 * @brief Generates a bounding radius for the polygon.
		 * 
		 * @return double 
		 */
		double generateBoundingRadius();

		/**
		 * @brief Generates a 2D polygon that approximates a circle.
		 * 
		 * @param radius 
		 * 		The radius of the circle
		 * @param n 
		 * 		The amount of samples from the circle to create the polygon
		 * @return Polygon2D 
		 */
		static Polygon2D approximateCircle(double radius, int n);

		/**
		 * @brief Generates a 2D polygon that approximates a ellipse.
		 * 
		 * @param xRadius 
		 * 		The x radius of the ellipse
		 * @param yRadius 
		 * 		The y radius of the ellipse
		 * @param n 
		 * 		The amount of samples from the ellipse to create the polygon
		 * @return Polygon2D 
		 */
		static Polygon2D approximateEllipse(double xRadius, double yRadius, int n);

		/**
		 * @brief Generates a 2D polygon that approximates an arc from an ellipse.
		 * 		The center of the ellipse will be included at (0,0)
		 * 
		 * @param xRadius 
		 * 		The x radius of the ellipse
		 * @param yRadius 
		 * 		The y radius of the ellipse
		 * @param startAngle 
		 * 		The start angle for the arc.
		 * 		Radians expected.
		 * @param endAngle 
		 * 		The end angle for the arc.
		 * 		Radians expected.
		 * @param n 
		 * 		The amount of samples from the ellipse to create the polygon
		 * @return Polygon2D 
		 */
		static Polygon2D approximateArc(double xRadius, double yRadius, double startAngle, double endAngle, int n);
	
	protected:
		void onTransformChanged(); //TODO

	private:
		void checkConcave();
		std::vector<Vec2f> points;
		Vec2f centerPoint;
		bool isConvex = true;
	};

	// class CubicBezierCurve2D : public Shape
	// {
	// public:
	// 	CubicBezierCurve2D();
	// 	~CubicBezierCurve2D();

	// 	void setBezierCurve(BezierCurve b);
	// 	BezierCurve& getBezierCurve();

	// private:
	// 	BezierCurve b;
	// };

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

		/**
		 * @brief Returns if there is a collision for the 2 shapes.
		 * 		Finds a valid collision method based on the class names.
		 * 		If no valid collision method is found among the pre-made collision methods,
		 * 		an external function can be defined to handle collision.
		 * 		If the function does not exist or it could not resolve the collision, the function
		 * 		returns false.
		 * 
		 * @param a 
		 * @param b 
		 * @param overrideQuickCheck
		 * 		Uses the bounding radius for the 2 shapes to determine if a collision is possible.
		 * 		Returns false if they don't collide otherwise, continues with normal collision.
		 * 		Can be skipped if the bounding radius values are not correctly generated for any of the specified shapes.
		 * @return true 
		 * @return false 
		 */
		static bool getCollision(Shape* a, Shape* b, bool overrideQuickCheck = false);

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
		static bool collisionMethod(Box2D* a, Triangle2D* b); //TEST
		static bool collisionMethod(Box2D* a, Line2D* b);

		//Circle
		static bool collisionMethod(Circle* a, Circle* b);
		static bool collisionMethod(Circle* a, Ellipse* b);
		static bool collisionMethod(Circle* a, Triangle2D* b);
		static bool collisionMethod(Circle* a, Line2D* b);

		//Line2D
		static bool collisionMethod(Line2D* a, Line2D* b);
		static bool collisionMethod(Line2D* a, Ellipse* b);
		static bool collisionMethod(Line2D* a, Triangle2D* b);

		//Ellipse
		static bool collisionMethod(Ellipse* a, Ellipse* b);
		static bool collisionMethod(Ellipse* a, Triangle2D* b);

		//Triangle2D
		static bool collisionMethod(Triangle2D* a, Triangle2D* b); //TEST

		//Polygon Stuff
		/**
		 * @brief Uses the Separating Axis Theorem to determine if the 2 polygons are colliding.
		 * 		Can determine the point of collision and if either shape is encapsulated in the other.
		 * 
		 * @param a 
		 * @param b 
		 * @return true 
		 * @return false 
		 */
		static bool SeparatingAxisTheorem(Polygon2D* a, Polygon2D* b);

		/**
		 * @brief Uses a method called Alternating Diagonals to determine if the 2 polygons are colliding.
		 * 		More information can be found here: https://www.youtube.com/watch?v=7Ik2vowGcU0
		 * 		Can determine the point of collision easier than SAT and is potentially faster due to earlier termination
		 * 		and simple line to line collisions.
		 * 		By itself, it can not determine if either shape is encapsulated in the other in every case.
		 * 
		 * @param a 
		 * @param b 
		 * @return true 
		 * @return false 
		 */
		static bool AlternatingDiagonals(Polygon2D* a, Polygon2D* b);
		
		static bool collisionMethod(Polygon2D* a, Point2D* b);
		static bool collisionMethod(Polygon2D* a, Circle* b);
		static bool collisionMethod(Polygon2D* a, Ellipse* b);
		
	private:
		static bool bezierCurveCollision(BezierCurve& a, BezierCurve& b, double tolerance=1.0);
	};

}  //NAMESPACE glib END
