#pragma once
#include "BuildOptions.h"
#include <vector>
#include <functional>
#include "MathExt.h"
#include "SimpleSerialization.h"
#include "BezierCurve.h"

namespace smpl
{
	class DLL_OPTION Shape : public SerializedObject
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
		virtual ~Shape();

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
		virtual float generateBoundingRadius();
		
	protected:
		Vec3f position = Vec3f(0,0,0);
		Vec3f scale = Vec3f(1,1,1);
		Vec3f rotation = Vec3f(0,0,0);

		virtual void onTransformChanged();

	SERIALIZE_CLASS(position, scale, rotation)
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

	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(shapes)
	};

	#pragma region SHAPES_2D

	class DLL_OPTION Point2D : public Shape
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
		Point2D(float x, float y);

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

		/**
		 * @brief Generates the bounding radius for the point.
		 * 		Returns the value 0 because it is a point.
		 * 
		 * @return float 
		 */
		float generateBoundingRadius();
		
	protected:
		void onTransformChanged();
	
	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS()
	};

	class DLL_OPTION Box2D : public Shape
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
		Box2D(float leftBound, float topBound, float rightBound, float bottomBound);
		~Box2D();
		
		/**
		 * @brief Set the Left Bound of the AABB
		 * 
		 * @param lb 
		 */
		void setLeftBound(float lb);

		/**
		 * @brief Get the Left Bound of the AABB
		 * 
		 * @return float 
		 */
		float getLeftBound();

		/**
		 * @brief Set the Top Bound of the AABB
		 * 
		 * @param tb 
		 */
		void setTopBound(float tb);

		/**
		 * @brief Get the Top Bound of the AABB
		 * 
		 * @return float 
		 */
		float getTopBound();

		/**
		 * @brief Set the Right Bound of the AABB
		 * 
		 * @param tb 
		 */
		void setRightBound(float rb);

		/**
		 * @brief Get the Right Bound of the AABB
		 * 
		 * @return float 
		 */
		float getRightBound();

		/**
		 * @brief Set the Bottom Bound of the AABB
		 * 
		 * @param tb 
		 */
		void setBottomBound(float bb);

		/**
		 * @brief Get the Bottom Bound of the AABB
		 * 
		 * @return float 
		 */
		float getBottomBound();

		/**
		 * @brief Gets the Width of the AABB
		 * 
		 * @return float 
		 */
		float getWidth();

		/**
		 * @brief Gets the Height of the AABB
		 * 
		 * @return float 
		 */
		float getHeight();

		/**
		 * @brief Generates a bounding radius for the AABB.
		 * 		(Implement later)
		 * @return float 
		 */
		float generateBoundingRadius();

		bool operator==(Box2D other);
		bool operator!=(Box2D other);
		
	protected:
		void onTransformChanged();

	private:
		Vec2f topLeft;
		Vec2f bottomRight;

		Vec2f baseTopLeft;
		Vec2f baseBottomRight;
		
	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(baseTopLeft, baseBottomRight)
	};

	class DLL_OPTION Circle : public Shape
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
		Circle(float rad);

		/**
		 * @brief Destroy the Circle object
		 * 
		 */
		~Circle();

		/**
		 * @brief Get the Radius of the Circle
		 * 
		 * @return float 
		 */
		float getRadius();

		/**
		 * @brief Set the Radius of the Circle
		 * 
		 * @param rad 
		 */
		void setRadius(float rad);

		/**
		 * @brief Generates the bounding radius for the circle.
		 * 		Returns the radius of the circle.
		 * 
		 * @return float 
		 */
		float generateBoundingRadius();
		
	protected:
		void onTransformChanged();

	private:
		float radius = 0;
		float baseRadius;
	
	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(baseRadius)
	};

	class DLL_OPTION Ellipse : public Shape
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
		Ellipse(float xRad, float yRad);

		/**
		 * @brief Destroy the Ellipse object
		 * 
		 */
		~Ellipse();

		/**
		 * @brief Gets the X radius of the Ellipse.
		 * 
		 * @return float 
		 */
		float getXRadius();

		/**
		 * @brief Sets the X radius of the Ellipse.
		 * 
		 * @param rad 
		 */
		void setXRadius(float rad);

		/**
		 * @brief Gets the Y radius of the Ellipse.
		 * 
		 * @return float 
		 */
		float getYRadius();

		/**
		 * @brief Sets the Y radius of the Ellipse.
		 * 
		 * @param rad 
		 */
		void setYRadius(float rad);
		
		/**
		 * @brief Generates the bounding radius of the Ellipse.
		 * 
		 * @return float 
		 */
		float generateBoundingRadius();
	
	protected:
		void onTransformChanged();

	private:
		float xRadius = 0;
		float yRadius = 0;
		float baseXRadius = 0;
		float baseYRadius = 0;
		
	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(baseXRadius, baseYRadius)
	};

	class DLL_OPTION Line2D : public Shape
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
		Line2D(float x1, float y1, float x2, float y2);

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
		void setPoint1(float x, float y);

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
		void setPoint2(float x, float y);

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
		 * @return float 
		 */
		float generateBoundingRadius();

	protected:
		void onTransformChanged();

	private:
		Line baseLine;
		Line l;
		
	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(baseLine)
	};

	class DLL_OPTION Triangle2D : public Shape
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
		Triangle2D(float x1, float y1, float x2, float y2, float x3, float y3);

		/**
		 * @brief Destroy the Triangle2D object
		 * 
		 */
		~Triangle2D();
		
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
		void setVertex1(float x, float y);

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
		void setVertex2(float x, float y);

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
		void setVertex3(float x, float y);

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
		 * @return float 
		 */
		float generateBoundingRadius();
	
	protected:
		void onTransformChanged(); //TODO

	private:
		Vec2f v1;
		Vec2f v2;
		Vec2f v3;
		
	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(v1, v2, v3)
	};

	class DLL_OPTION Triangle2DModel : public Shape
	{
	public:
		/**
		 * @brief Construct a new Triangle 2D Model object
		 * 		Consider this a special case of Combination Shape that only contains 2D triangles.
		 * 		Good for converting polygons and other shapes into a simpler shape and either rendering them or performing collision.
		 * 			Note that this model is "uncompressed" meanining it is equivalent to a non-indexed model with a triangle strip so there are duplicate points.
		 * 
		 */
		Triangle2DModel();

		/**
		 * @brief Destroy the Triangle 2D Model object
		 * 
		 */
		~Triangle2DModel();

		/**
		 * @brief Adds a 2D triangle to the list of triangles in the shape.
		 * 
		 * @param tri 
		 */
		void add(Triangle2D tri);

		/**
		 * @brief Gets a 2D triangle at the specified index.
		 * 
		 * @param index 
		 * @return Triangle2D 
		 */
		Triangle2D get(size_t index);

		/**
		 * @brief Get the raw list of triangles
		 * 
		 * @return std::vector<Triangle2D>& 
		 */
		std::vector<Triangle2D>& getTriangles();

		/**
		 * @brief Returns the number of triangles in the model.
		 * 
		 * @return size_t 
		 */
		size_t size();
	private:
		std::vector<Triangle2D> triangleList;
		
	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(triangleList)
	};

	class DLL_OPTION Polygon2D : public Shape
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
		Vec2f getVertex(size_t index);

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
		 * @return float 
		 */
		float generateBoundingRadius();

		/**
		 * @brief Generates a 2D polygon that approximates a circle.
		 * 
		 * @param radius 
		 * 		The radius of the circle
		 * @param n 
		 * 		The amount of samples from the circle to create the polygon
		 * @return Polygon2D 
		 */
		static Polygon2D approximateCircle(float radius, int n);

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
		static Polygon2D approximateEllipse(float xRadius, float yRadius, int n);

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
		static Polygon2D approximateArc(float xRadius, float yRadius, float startAngle, float endAngle, int n);
	
	protected:
		void onTransformChanged(); //TODO

	private:
		void checkConcave();
		std::vector<Vec2f> points;
		Vec2f centerPoint;
		bool isConvex = true;
		
	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(points)
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

	// class DLL_OPTION Sphere : public Shape
	// {

	// };

	// class DLL_OPTION Ellipsoid : public Shape
	// {

	// };

	// class DLL_OPTION Cylinder : public Shape
	// {

	// };

	// class DLL_OPTION Box3D : public Shape
	// {

	// };

	// class DLL_OPTION Point3D : public Shape
	// {

	// };

	// class DLL_OPTION Line3D : public Shape
	// {

	// };

	class DLL_OPTION Triangle3D : public Shape
	{
	public:
		/**
		 * @brief Construct a new Triangle3D object
		 * 		All transforms affect this shape.
		 * 		Default position are all (0,0)
		 * 
		 */
		Triangle3D();

		/**
		 * @brief Construct a new Triangle3D object
		 * 		All transforms affect this shape.
		 * 
		 * @param p1 
		 * @param p2 
		 * @param p3 
		 */
		Triangle3D(Vec3f p1, Vec3f p2, Vec3f p3);

		/**
		 * @brief Construct a new Triangle3D object
		 * 		All transforms affect this shape.
		 * 
		 * @param x1 
		 * @param y1 
		 * @param z1
		 * @param x2 
		 * @param y2 
		 * @param z2
		 * @param x3 
		 * @param y3 
		 * @param z3
		 */
		Triangle3D(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);

		/**
		 * @brief Destroy the Triangle3D object
		 * 
		 */
		~Triangle3D();
		
		/**
		 * @brief Sets the first vertex of the triangle
		 * 
		 * @param p 
		 */
		void setVertex1(Vec3f p);

		/**
		 * @brief Sets the first vertex of the triangle
		 * 
		 * @param x 
		 * @param y 
		 * @param z
		 */
		void setVertex1(float x, float y, float z);

		/**
		 * @brief Sets the second vertex of the triangle
		 * 
		 * @param p 
		 */
		void setVertex2(Vec3f p);

		/**
		 * @brief Sets the second vertex of the triangle
		 * 
		 * @param x 
		 * @param y 
		 * @param z
		 */
		void setVertex2(float x, float y, float z);

		/**
		 * @brief Sets the third vertex of the triangle
		 * 
		 * @param p 
		 */
		void setVertex3(Vec3f p);

		/**
		 * @brief Sets the third vertex of the triangle
		 * 
		 * @param x 
		 * @param y 
		 * @param z
		 */
		void setVertex3(float x, float y, float z);

		/**
		 * @brief Gets the first vertex of the triangle after transforms
		 * 
		 * @return Vec3f 
		 */
		Vec3f getVertex1();

		/**
		 * @brief Gets the first vertex of the triangle after transforms
		 * 
		 * @return Vec3f 
		 */
		Vec3f getVertex2();

		/**
		 * @brief Gets the first vertex of the triangle after transforms
		 * 
		 * @return Vec3f 
		 */
		Vec3f getVertex3();

		/**
		 * @brief Get the Center Position of the triangle after transforms
		 * 
		 * @return Vec3f 
		 */
		Vec3f getCenterPosition();
		
		/**
		 * @brief Gets the Normal vector for the triangle
		 * 
		 * @return Vec3f 
		 */
		Vec3f getNormal();

		/**
		 * @brief Generates a bounding radius for the triangle.
		 * 
		 * @return float 
		 */
		float generateBoundingRadius();
	
	protected:
		void onTransformChanged(); //TODO

	private:
		Vec3f v1;
		Vec3f v2;
		Vec3f v3;

	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(v1, v2, v3)
	};

	class DLL_OPTION Triangle3DModel : public Shape
	{
	public:
		/**
		 * @brief Construct a new Triangle 3D Model object
		 * 		Consider this a special case of Combination Shape that only contains 3D triangles.
		 * 		Good for converting polygons and other shapes into a simpler shape and either rendering them or performing collision.
		 * 			Note that this model is "uncompressed" meanining it is equivalent to a non-indexed model with a triangle strip so there are duplicate points.
		 * 
		 */
		Triangle3DModel();

		/**
		 * @brief Destroy the Triangle 3D Model object
		 * 
		 */
		~Triangle3DModel();

		/**
		 * @brief Adds a 3D triangle to the list of triangles in the shape.
		 * 
		 * @param tri 
		 */
		void add(Triangle3D tri);

		/**
		 * @brief Gets a 3D triangle at the specified index.
		 * 
		 * @param index 
		 * @return Triangle3D 
		 */
		Triangle3D get(size_t index);

		/**
		 * @brief Get the raw list of triangles
		 * 
		 * @return std::vector<Triangle3D>& 
		 */
		std::vector<Triangle3D>& getTriangles();

		/**
		 * @brief Returns the number of triangles in the model.
		 * 
		 * @return size_t 
		 */
		size_t size();
	private:
		std::vector<Triangle3D> triangleList;

	SERIALIZE_SUPER_CLASS(Shape)
	SERIALIZE_CLASS(triangleList)
	};

	// class DLL_OPTION Polygon3D : public Shape
	// {

	// };

	#pragma endregion

	class DLL_OPTION CollisionMaster
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
