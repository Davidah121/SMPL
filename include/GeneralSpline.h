#pragma once
#include "BezierCurve.h"

namespace smpl
{
	class GeneralSpline
	{
	public:
		/**
		 * @brief Construct a new General Spline object.
		 * 		Each curve is connected to each other by the end points.
		 * 		Without adding additional control points, it mimics a polyline.
		 * 		This method does not guarantee any continuity other than C0.
		 * 
		 */
		GeneralSpline();

		/**
		 * @brief Destroy the General Spline object
		 * 
		 */
		~GeneralSpline();

		/**
		 * @brief Adds a new end point. This will in turn add a new Bezier Curve
		 * 		to the spline.
		 * 
		 * @param p 
		 */
		void addEndPoint(Vec2f p);

		/**
		 * @brief Adds a new control point to the bezier curve with the specified index.
		 * 
		 * @param index 
		 * @param p 
		 */
		void addControlPoint(size_t index, Vec2f p);

		/**
		 * @brief Gets a reference to a curve with the specified index.
		 * 		This allows direct modification of the curve if desired.
		 * 		It may be possible that the curve here becomes invalid due to the data structure resizing.
		 * 			Assume that adding an end point or calling the destructor deletes this pointer.
		 * 
		 * @param index 
		 * @return BezierCurve* 
		 */
		BezierCurve* getCurve(size_t index);

		/**
		 * @brief Returns the number of curves in the General Spline. 
		 * 
		 * @return size_t 
		 */
		size_t size();
	private:
		std::vector<BezierCurve> curves;
		bool firstPoint = false;
	};
}