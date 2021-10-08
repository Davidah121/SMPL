#pragma once
#include "Vec2f.h"
#include "Object.h"

namespace glib
{

	class Line : public Object
	{
	public:
		/**
		 * @brief Construct a new Line object
		 * 		Useful for different math operations.
		 */
		Line();

		/**
		 * @brief Construct a new Line object
		 * 		Useful for different math operations.
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 */
		Line(int x1, int y1, int x2, int y2);

		/**
		 * @brief Construct a new Line object
		 * 		Useful for different math operations.
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 */
		Line(double x1, double y1, double x2, double y2);

		/**
		 * @brief Construct a new Line object
		 * 		Useful for different math operations.
		 * 
		 * @param p1 
		 * @param p2 
		 */
		Line(Vec2f p1, Vec2f p2);

		/**
		 * @brief Destroy the Line object
		 * 
		 */
		~Line();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		/**
		 * @brief Gets the first point of the line.
		 * 
		 * @return Vec2f 
		 */
		Vec2f getPoint1();

		/**
		 * @brief Get the second point of the line.
		 * 
		 * @return Vec2f 
		 */
		Vec2f getPoint2();

		/**
		 * @brief Returns a vector that represent the change from point1 to point2.
		 * 
		 * @return Vec2f 
		 */
		Vec2f getToPoint();
		
		/**
		 * @brief Get the max X between the defined points.
		 * 
		 * @return double 
		 */
		double getMaxX();

		/**
		 * @brief Get the min X between the defined points.
		 * 
		 * @return double 
		 */
		double getMinX();

		/**
		 * @brief Get the min Y between the defined points.
		 * 
		 * @return double 
		 */
		double getMinY();

		/**
		 * @brief Get the max Y between the defined points.
		 * 
		 * @return double 
		 */
		double getMaxY();

		/**
		 * @brief Returns the slope of the line.
		 * 		The slope may be invalid if the line is a vertical line.
		 * @return double 
		 */
		double getSlope();

		/**
		 * @brief Returns the slope of the line relative to the y direction.
		 * 		The slope may be invalid if the line is a horizontal line.
		 * @return double 
		 */
		double getSlopeRelativeY();

		/**
		 * @brief Returns the Y intersection point of the line.
		 * 		May be invalid if the line is a vertical line.
		 * 
		 * @return double 
		 */
		double getYInt();

		/**
		 * @brief Returns the X intersection point of the line.
		 * 		May be invalid if the line is a horizontal line.
		 * 
		 * @return double 
		 */
		double getXInt();

		/**
		 * @brief Solves the line for an X value using a y value.
		 * 		May be invalid if the line is horizontal.
		 * @param y 
		 * 		The y value to check.
		 * @return double 
		 */
		double solveForX(double y);

		/**
		 * @brief Solves the line for an Y value using a x value.
		 * 		May be invalid if the line is vertical.
		 * 
		 * @param x 
		 * 		The x value to check.
		 * @return double 
		 */
		double solveForY(double x);

		/**
		 * @brief Gets a point of intersection with another line.
		 * 		May be invalid if the lines never intersect.
		 * 		If invalid, NAN will be returned.
		 * 
		 * @param other 
		 * 		The other line to check for intersection with.
		 * @return Vec2f 
		 */
		Vec2f getIntersection(Line other);
		
		/**
		 * @brief Get the point of intersection with another line as a time value.
		 * 		The answer, if valid, will describe a point of intersection as
		 * 		P = getPoint1() + getToPoint()*t;
		 * 
		 * 		If invalid, NAN will be returned.
		 * 
		 * @param other 
		 * @return double 
		 */
		double getIntersectionParametric(Line other);

		/**
		 * @brief Get the point on the line as Paramtetric time value.
		 * 		If the point does not lie on the line, NAN is returned.
		 * 
		 * @param x 
		 * @param y 
		 * @return double 
		 */
		double getPointAsParamtetricValue(double x, double y);

		/**
		 * @brief Get the point on the line as Paramtetric time value.
		 * 		If the point does not lie on the line, NAN is returned.
		 * 
		 * @param p 
		 * @return double 
		 */
		double getPointAsParamtetricValue(Vec2f p);


	private:
		void setMinMax();
		void setSlopeStuff();

		double slope = 0;
		double slopeRelY = 0;
		double xInt = 0;
		double yInt = 0;

		Vec2f point1 = Vec2f(0,0);
		Vec2f point2 = Vec2f(0, 0);
		Vec2f toPoint = Vec2f(0, 0);

		double minX = 0, maxX = 0, minY = 0, maxY = 0;

	};

} //NAMESPACE glib END