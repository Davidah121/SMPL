#pragma once
#include "BuildOptions.h"
#include "BezierCurve.h"

/*
 * 	Implementation described here:
 * 		http://www.cemyuksel.com/research/interpolating_splines/
 */

namespace smpl
{
	class DLL_OPTION CemYukselSpline
	{
	public:
		static const int TYPE_BEZIER = 0;
		static const int TYPE_CIRCULAR = 1;
		static const int TYPE_ELLIPTICAL = 2;
		static const int TYPE_HYBRID = 3;

		/**
		 * @brief Construct a new Cem Yuksel Spline.
		 * 		Guaranteed C2 Continuity with local support.
		 * 		Research described and demoed here: 
		 * 			http://www.cemyuksel.com/research/interpolating_splines/
		 * 
		 * @param type 
		 */
		CemYukselSpline(int type);

		/**
		 * @brief Destroy the Cem Yuksel Spline
		 * 
		 */
		~CemYukselSpline();

		/**
		 * @brief Sets the Type of the spline.
		 * 
		 * @param type 
		 */
		void setType(int type);

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
		 * @brief Gets the number of points in the spline.
		 * 
		 * @return size_t 
		 */
		size_t size();

		/**
		 * @brief Gets the number of curve segments in the spline
		 * 
		 * @return size_t 
		 */
		size_t curveSize();
	private:

		Vec2f bezierFormulation(size_t index, double weight);
		Vec2f circularFormulation(size_t index, double weight);
		Vec2f ellipticalFormulation(size_t index, double weight);
		Vec2f hybridFormulation(size_t index, double weight);
		
		std::vector<Vec2f> points = std::vector<Vec2f>();
		int type = 0;
	};
}