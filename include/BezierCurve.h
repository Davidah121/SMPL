#pragma once
#include "BuildOptions.h"
#include <vector>
#include "Vec2f.h"
#include "SimpleSerialization.h"
#include "GeneralExceptions.h"

namespace smpl
{

	class DLL_OPTION BezierCurve : public SerializedObject
	{
	public:
		/**
		 *	@brief Creates a BezierCurve Object.
		 * 		A Bezier Curve is a parametric curve valid from t=0 to t=1
		 * 		The first and last points are end points and all other points are control points.
		 */
		BezierCurve();
		
		/**
		 * @brief Construct a new Bezier Curve object from a set of points.
		 * 		A Bezier Curve is a parametric curve valid from t=0 to t=1
		 * 		The first and last points are end points and all other points are control points.
		 * 		
		 * @param points 
		 */
		BezierCurve(const std::vector<Vec2f>& points);

		BezierCurve(const BezierCurve& other);
		void operator=(const BezierCurve& other);

		BezierCurve(BezierCurve&& other) noexcept;
		void operator=(BezierCurve&& other) noexcept;
		
		/**
		 * @brief Destroys a BezierCurve Object freeing its memory usage
		 */
		~BezierCurve();

		//Exception Stuff
		struct BlendPointsError : public std::exception
		{
			const char* what() const noexcept { return "ERROR ON BLENDING POINTS"; }
		};

		void operator+=(Vec2f p);
		void operator-=(Vec2f p);
		void operator*=(float x);
		void operator/=(float x);
		
		BezierCurve operator+(Vec2f p);
		BezierCurve operator-(Vec2f p);
		BezierCurve operator*(float x);
		BezierCurve operator/(float x);
		
		/**
		 * @brief Adds a point to the list of points for the Bezier Curve.
		 * 		Note that duplicate points are allowed.
		 * @param p
		 * 		A Vec2f containing the point.
		 */
		void addPoint(Vec2f p);

		/**
		 * @brief Adds a point to the list of points for the Bezier Curve.
		 * 		Note that duplicate points are allowed.
		 * @param x
		 * 		A double representing the x value for the point
		 * @param y
		 * 		A double representing the y value for the point
		 */
		void addPoint(double x, double y);

		/**
		 * @brief Inserts a new point at the specified index. This allows you to add
		 * 		control points where desired and to insert a new start or end point.
		 * 
		 * @param index 
		 * @param p 
		 * @return true 
		 * @return false 
		 */
		bool insertPoint(size_t index, Vec2f p);

		/**
		 * @brief Inserts a new point at the specified index. This allows you to add
		 * 		control points where desired and to insert a new start or end point.
		 * 
		 * @param index 
		 * @param x 
		 * @param y 
		 * @return true 
		 * @return false 
		 */
		bool insertPoint(size_t index, double x, double y);

		/**
		 * @brief Removes the point at the specified index.
		 * 
		 * @param index 
		 */
		void removePoint(size_t index);

		/**
		 * @brief Sets the point at the specified index
		 * 
		 * @param index 
		 * @param p 
		 */
		void setPoint(size_t index, Vec2f p);

		/**
		 * @brief Sets the point at the specified index
		 * 
		 * @param index 
		 * @param x 
		 * @param y 
		 */
		void setPoint(size_t index, double x, double y);
		
		/**
		 * @brief Gets a point from the Bezier Curve.
		 * @param index
		 * 		The index of the point in the list.
		 * @return Vec2f
		 * 		returns the point as a Vec2f if index was valid.
		 * 		If index was invalid and USE_EXCEPTIONS is defined, an
		 * 		OutOfBoundsError will be thrown.
		 * 		Otherwise, the default Vec2f is returned.
		 */
		Vec2f getPoint(size_t index);

		/**
		 * @brief Get the Points used by the Bezier Curve
		 * 
		 * @return std::vector<Vec2f>& 
		 */
		std::vector<Vec2f>& getPoints();

		/**
		 * @brief Subdivides the Bezier Curve from [0, 1] into 2 separate Bezier Curves.
		 * 
		 * @param t
		 * 		The time value to split at.
		 * 
		 * @return std::vector<BezierCurve> 
		 */
		std::vector<BezierCurve> subdivide(double t);

		/**
		 * @brief Extracts the segment from [a, b] as a new Bezier Curve.
		 * 
		 * @param a
		 * 		The start time value. Must be between [0, 1]
		 * @param b
		 * 		The end time value. Must be between [0, 1]
		 * @return BezierCurve
		 */
		BezierCurve extract(double a, double b);

		/**
		 * @brief Gets an Axis Aligned Bounding Box for the Bezier Curve.
		 * 		If successful, it returns a std::vector<Vec2f> of size 2 where the first point
		 * 		represents the top left of the box and the second point represents the bottom right of the box.
		 * 
		 * @return std::vector<Vec2f> 
		 */
		std::vector<Vec2f> getBoundingBox();

		/**
		 * @brief Solves the BezierCurve for the given time. Note that any time
		 * 		value can be used even if outside of the bounds [0,1]
		 * @param time
		 * 		The time value to solve for.
		 * @return Vec2f
		 * 		returns the point as a Vec2f.
		 */
		Vec2f getFuctionAt(double time);

		/**
		 * @brief Solves the derivative of BezierCurve for the given time. Note that any time
		 * 		value can be used even if outside of the bounds [0,1]
		 * @param time
		 * 		The time value to solve for.
		 * @return Vec2f
		 * 		returns the derivative as a Vec2f.
		 */
		Vec2f getDerivativeAt(double time);

		/**
		 * @brief Solves the second derivative of the curve at the given time. Just like for the 1st derivative,
		 * 		any time value can be used even if outside the bounds [0,1]
		 * 
		 * @param time 
		 * @return Vec2f 
		 */
		Vec2f getSecondDerivativeAt(double time);

		/**
		 * @brief Solves for the simple derivative of BezierCurve for the given time.
		 * 		Note that any time value can be used even if outside of the bounds [0,1].
		 * 		
		 * 		The simple derivative is just the next point minus the previous point where time
		 * 		determines which points to use. This is useful if you just need to know if the function
		 * 		is increasing at the time given instead of the actual derivative.
		 * @param time
		 * 		The time value to solve for.
		 * @return Vec2f
		 * 		returns the simple derivative as a Vec2f.
		 */
		Vec2f getSimpleDerivativeAt(double time);

		/**
		 * @brief Solves for the Arc Length of the Bezier curve from 0 to the given time.
		 * 		Note for all bezier curves above degree 2 (not a line) the arc length is
		 * 		approximated using Simpson's rule.
		 * 		The Bezier curve is divided into segments and each segement is approximated and
		 * 		added together to get the total arc length.
		 * @param time
		 * 		The end time to measure arc length to. 
		 * @return double
		 * 		returns the measured arc length.
		 */
		double getArcLengthAt(double time);

		/**
		 * @brief Solves for the Arc Length of the Bezier curve from start time to end time.
		 * 		Note for all bezier curves above degree 2 (not a line) the arc length is
		 * 		approximated using Simpson's rule.
		 * 		The Bezier curve is divided into segments and each segement is approximated and
		 * 		added together to get the total arc length.
		 * @param startTime
		 * 		The time to begin measuring arc length from.
		 * @param endTime
		 * 		The end time to measure arc length to. 
		 * @return double
		 * 		returns the measured arc length.
		 */
		double getArcLengthAt(double startTime, double endTime);

		/**
		 * @brief Finds a time t in the range [0, 1] such that
		 * 		the bezier curve at that point results in the smallest distance
		 * 		to p. 
		 * 			i.e. length( b(t) - p ) is the smallest possible
		 * 		In other words, find the closest point on the bezier curve to p.
		 * 		This uses Newton's method to approximate the answer with a default of a maximum of 5 iterations.
		 * 
		 * 		Runtime = O(3N * Iterations) where N = total number of points
		 * @param p 
		 * @param maxIterations
		 * 		The maximum number of iterations of Newton's method to use.
		 * 		Default value is 10.
		 * @return double 
		 */
		double findTimeForMinDis(Vec2f p, unsigned int maxIterations = 5);

		/**
		 * @brief Finds a time t in the range [0, 1] such that
		 * 		the bezier curve at that point results in the smallest distance
		 * 		to p. 
		 * 			i.e. length( b(t) - p ) is the smallest possible
		 * 		In other words, find the closest point on the bezier curve to p.
		 * 		This uses Newton's method to approximate the answer with a default of a maximum of 5 iterations.
		 * 
		 * 		Runtime = O(3N * Iterations) where N = total number of points
		 * @param x 
		 * @param y 
		 * @param maxIterations
		 * 		The maximum number of iterations of Newton's method to use.
		 * 		Default value is 5.
		 * @return double 
		 */
		double findTimeForMinDis(double x, double y, unsigned int maxIterations = 5);

		/**
		 * @brief Clears all points from the BezierCurve's list of points
		 */
		void clear();

		/**
		 * @brief Returns the amount of points the bezier curve has.
		 * @return size_t
		 */
		size_t size();

		/**
		 * @brief Solves the function all times where the solution has the specified y value.
		 * 		Currently only works for BezierCurves with 4 or less points.
		 * 
		 * 	@param y
		 * 		The y value to solve for.
		 * @return std::vector<double>
		 * 		returns a list of all solutions. Can return an empty list if there is no
		 * 		solution.
		 */
		std::vector<double> findTimeForY(double y, bool removeDuplicates = false);

		/**
		 * @brief Solves the function all times where the solution has the specified x value.
		 * 		Currently only works for BezierCurves with 4 or less points.
		 * 
		 * 	@param x
		 * 		The x value to solve for.
		 * @return std::vector<double>
		 * 		returns a list of all solutions. Can return an empty list if there is no
		 * 		solution.
		 */
		std::vector<double> findTimeForX(double x, bool removeDuplicates = false);

		/**
		 * @brief Solves the function all times where the solution has the specified x,y value.
		 * 		Currently only works for BezierCurves with 4 or less points.
		 * 
		 * @param x 
		 * @param y 
		 * @return std::vector<double> 
		 * 		returns a list of all solutions. Can return an empty list if there is no
		 * 		solution.
		 */
		std::vector<double> findTimeForPoint(double x, double y, bool removeDuplicates = false);

		/**
		 * @brief Finds the times at which this bezier curve intersects with the other bezier curve in space.
		 * 		Approximates using an iterative method.
		 * 		Tolerance refers to the smallest area of the bounding box for both curves needed to confirm collision.
		 * 
		 * @param other 
		 * @param tolerance 
		 * @return std::vector<double> 
		 */
		std::vector<double> findIntersectionPoints(BezierCurve& other, double tolerance);

		/**
		 * @brief Finds all points where the curve intersects itself.
		 * 		Approximates using an iterative method.
		 * 		The curve is subdivided and intersection is checked between the split parts.
		 * 		Tolerance refers to the smallest area of the bounding box for both curves needed to confirm collision.
		 * 
		 * @param tolerance 
		 * @return std::vector<double> 
		 */
		std::vector<double> findSelfIntersection(double tolerance); //TODO


		/**
		 * @brief Approximates a Circle using a series of bezier curves.
		 * 		If cubic bezier curves are selected, 4 Cubic Bezier curves are used for approximation.
		 * 		If quadratic bezier curves are selected, 8 Quadratic Bezier curves are used for approximation.
		 * 
		 * @param radius 
		 * 		The radius of the Circle.
		 * @param cubic
		 * 		If set to true, cubic bezier curves are used to approximate the circle.
		 * 		Otherwise, quadratic bezier curves are used.
		 * 		Default value is true.
		 * @return std::vector<BezierCurve> 
		 */
		static std::vector<BezierCurve> approximateCircle(double radius, double x=0, double y=0, bool cubic=true);

		/**
		 * @brief Approximates a Ellipse using a series of bezier curves.
		 * 		If cubic bezier curves are selected, 4 Cubic Bezier curves are used for approximation.
		 * 		If quadratic bezier curves are selected, 8 Quadratic Bezier curves are used for approximation.
		 * 
		 * @param xRadius 
		 * 		The x radius of the Ellipse.
		 * @param yRadius 
		 * 		The y radius of the Ellipse.
		 * @param cubic
		 * 		If set to true, cubic bezier curves are used to approximate the Ellipse.
		 * 		Otherwise, quadratic bezier curves are used.
		 * 		Default value is true.
		 * @return std::vector<BezierCurve> 
		 */
		static std::vector<BezierCurve> approximateEllipse(double xRadius, double yRadius, double x=0, double y=0, bool cubic=true);

		/**
		 * @brief Approximates an Arc using a bezier curve.
		 * 
		 * @param xRadius 
		 * 		The x radius of the Arc.
		 * @param yRadius 
		 * 		The y radius of the Arc.
		 * @param startAngle 
		 * 		The start angle for the arc.
		 * 		Radians expected.
		 * @param endAngle 
		 * 		The end angle for the arc.
		 * 		Radians expected.
		 * @param cubic
		 * 		If set to true, cubic bezier curves are used to approximate the Arc.
		 * 		Otherwise, quadratic bezier curves are used.
		 * 		Default value is true.
		 * @return BezierCurve
		 */
		static BezierCurve approximateArc(double xRadius, double yRadius, double startAngle, double endAngle, double x=0, double y=0, bool cubic=true);
		
		
	private:
		static void findIntersectionPoints(BezierCurve c1, BezierCurve c2, double tolerance, std::pair<double, double> timePoints, std::vector<double>& output);
		Vec2f blendPointsRecursive(int start, int end, double time);
		Vec2f blendPointsExplicit(double time);
		Vec2f blendPointsDerivativeRecursive(int start, int end, double time);
		Vec2f blendPointsSecondDerivativeRecursive(int start, int end, double time);
		bool checkForPotentialSelfIntersection();
		void subdivideTillNoIntersection(BezierCurve c, double t1, double t2, std::vector<BezierCurve>& outputCurves, std::vector<std::pair<double, double>>& outputTimes);
	

		std::vector<Vec2f> points = std::vector<Vec2f>();
		SERIALIZE_CLASS(points)
	};

} //NAMESPACE smpl END