#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "Vec2f.h"
#include "SimpleSerialization.h"

namespace smpl
{

	class DLL_OPTION Line : public SerializedObject
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

		Line operator+(const Vec2f& other) const
		{
			return Line(point1 + other, point2 + other);
		}
		Line operator-(const Vec2f& other) const
		{
			return Line(point1 - other, point2 - other);
		}
		friend Line operator+(const Vec2f& other, const Line& l)
		{
			return l + other;
		}
		friend Line operator-(const Vec2f& other, const Line& l)
		{
			return l - other;
		}

		/**
		 * @brief Gets the first point of the line.
		 * 
		 * @return Vec2f 
		 */
		Vec2f getPoint1() const;

		/**
		 * @brief Get the second point of the line.
		 * 
		 * @return Vec2f 
		 */
		Vec2f getPoint2() const;

		/**
		 * @brief Gets the Mid Point of the line
		 * 
		 * @return Vec2f 
		 */
		Vec2f getMidPoint() const;

		/**
		 * @brief Returns a vector that represent the change from point1 to point2.
		 * 
		 * @return Vec2f 
		 */
		Vec2f getToPoint() const;
		
		/**
		 * @brief Get the max X between the defined points.
		 * 
		 * @return double 
		 */
		double getMaxX() const;

		/**
		 * @brief Get the min X between the defined points.
		 * 
		 * @return double 
		 */
		double getMinX() const;

		/**
		 * @brief Get the min Y between the defined points.
		 * 
		 * @return double 
		 */
		double getMinY() const;

		/**
		 * @brief Get the max Y between the defined points.
		 * 
		 * @return double 
		 */
		double getMaxY() const;

		/**
		 * @brief Returns the slope of the line.
		 * 		The slope may be invalid if the line is a vertical line.
		 * @return double 
		 */
		double getSlope() const;

		/**
		 * @brief Returns the slope of the line relative to the y direction.
		 * 		The slope may be invalid if the line is a horizontal line.
		 * @return double 
		 */
		double getSlopeRelativeY() const;

		/**
		 * @brief Returns the Y intersection point of the line.
		 * 		May be invalid if the line is a vertical line.
		 * 
		 * @return double 
		 */
		double getYInt() const;

		/**
		 * @brief Returns the X intersection point of the line.
		 * 		May be invalid if the line is a horizontal line.
		 * 
		 * @return double 
		 */
		double getXInt() const;

		/**
		 * @brief Solves the line for an X value using a y value.
		 * 		May be invalid if the line is horizontal.
		 * @param y 
		 * 		The y value to check.
		 * @return double 
		 */
		double solveForX(double y) const;

		/**
		 * @brief Solves the line for an Y value using a x value.
		 * 		May be invalid if the line is vertical.
		 * 
		 * @param x 
		 * 		The x value to check.
		 * @return double 
		 */
		double solveForY(double x) const;

		/**
		 * @brief Gets a point of intersection with another line.
		 * 		May be invalid if the lines never intersect.
		 * 
		 * @param other 
		 * 		The other line to check for intersection with.
		 * @return bool 
		 */
		bool getIntersection(const Line& other, Vec2f& output) const;
		
		/**
		 * @brief Get the point of intersection with another line as a time value.
		 * 		The answer, if valid, will describe a point of intersection as
		 * 		P = getPoint1() + getToPoint()*t;
		 * 
		 * @param other 
		 * @return bool 
		 */
		bool getIntersectionParametric(const Line& other, double& timePoint) const;

		/**
		 * @brief Get the point on the line as Paramtetric time value.
		 * 		If the point does not lie on the line, NAN is returned.
		 * 
		 * @param x 
		 * @param y 
		 * @return double 
		 */
		double getPointAsParametricValue(double x, double y) const;

		/**
		 * @brief Get the point on the line as Paramtetric time value.
		 * 		If the point does not lie on the line, NAN is returned.
		 * 
		 * @param p 
		 * @return double 
		 */
		double getPointAsParametricValue(const Vec2f& p) const;

		/**
		 * @brief Gets the line that is Perpendicular to the current line and crosses
		 * 		the mid point.
		 * 
		 * @return Line 
		 */
		Line getPerpendicularBisector() const;

		/**
		 * @brief Projects a point onto this line and returns that point
		 * 
		 * @param p 
		 * @return Vec2f 
		 */
		Vec2f projectOntoLine(const Vec2f& p) const;

		/**
		 * @brief Projects a point onto this line and returns the parametric value needed to get to that point
		 * 
		 * @param p 
		 * @return double 
		 */
		double projectOntoLineParametric(const Vec2f& p) const;


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
		double length = 0;

		double minX = 0, maxX = 0, minY = 0, maxY = 0;

		SERIALIZE_CLASS(point1, point2)
	};

} //NAMESPACE smpl END