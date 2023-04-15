#pragma once
#include "BezierCurve.h"

namespace glib
{
	class CatmullRom
	{
	public:
		/**
		 * @brief Construct a new Catmull Rom spline.
		 * 		This spline guarantees C1 continuity and requires at least 2 points.
		 * 			The derivative of the first and last points are calculated by fitting a curve.
		 * 
		 */
		CatmullRom();

		/**
		 * @brief Destroy the Catmull Rom object
		 * 
		 */
		~CatmullRom();

		/**
		 * @brief Adds a new point in the curve.
		 * 
		 * @param p 
		 */
		void addPoint(Vec2f p);

		/**
		 * @brief Gets a Point in the spline.
		 * 		May fail if it is not a valid index.
		 * 			On fail, returns empty Vec2f or throws an exception if possible.
		 * 
		 * @param index 
		 * @return Vec2f 
		 */
		Vec2f getPoint(size_t index);

		/**
		 * @brief Edits a point in the curve.
		 * 
		 * @param index 
		 * @param p 
		 */
		void editPoint(size_t index, Vec2f p);

		/**
		 * @brief Inserts a new point into the curve at the specified index.
		 * 		May fail if it is not possible to insert at the specified index.
		 * @param index 
		 * @param p 
		 * @return true 
		 * @return false 
		 */
		bool insertPoint(size_t index, Vec2f p);

		/**
		 * @brief Removes a point from the curve at the specified index.
		 * 
		 * @param index 
		 */
		void removePoint(size_t index);

		/**
		 * @brief Get the Tension of the curve.
		 * 
		 * @return double 
		 */
		double getTension();

		/**
		 * @brief Set the Tension of the curve.
		 * 		Typically defined in the range [0, 1] and sometimes [-1 , 1] but here
		 * 		it is uncapped.
		 * @param t 
		 */
		void setTension(double t);

		/**
		 * @brief Solves for a point in the Catmull Rom spline with the provided curveSegment and
		 * 		weight value.
		 * 
		 * @param curveSegment 
		 * 		Any valid curve segment.
		 * @param weight 
		 * 		Defined between [0, 1]
		 * @return Vec2f 
		 */
		Vec2f solve(size_t curveSegment, double weight);

		/**
		 * @brief Gets a Curve segment from the Catmull Rom spline that can be rendered.
		 * 
		 * @param index 
		 * @return BezierCurve 
		 */
		BezierCurve getCurve(size_t index);

		/**
		 * @brief Gets the number of points in the Catmull Rom spline.
		 * 
		 * @return size_t 
		 */
		size_t size();

		/**
		 * @brief Gets the number of curve segments in the Catmull Rom spline
		 * 
		 * @return size_t 
		 */
		size_t curveSize();

	private:
		Vec2f getTangent(size_t index);
		std::vector<Vec2f> points = std::vector<Vec2f>();
		double tension = 0;
	};
}