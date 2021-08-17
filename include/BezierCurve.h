#pragma once
#include <vector>
#include "Vec2f.h"
#include "Object.h"
#include "GeneralExceptions.h"

namespace glib
{

	class BezierCurve : public Object
	{
	public:
		BezierCurve();
		BezierCurve(const BezierCurve& o);
		~BezierCurve();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		//Exception Stuff
		struct BlendPointsError : public std::exception
		{
			const char* what() noexcept { return "ERROR ON BLENDING POINTS"; }
		};

		void addPoint(Vec2f p);
		Vec2f getPoint(int index);
		Vec2f getFuctionAt(double time);
		Vec2f getDerivativeAt(double time);
		Vec2f getSimpleDerivativeAt(double time);
		double getArcLengthAt(double time);
		double getArcLengthAt(double startTime, double endTime);
		void clear();
		int size();

		std::vector<double> findTimeForY(double y);
		std::vector<double> findTimeForX(double x);

	private:
		Vec2f blendPointsRecursive(int start, int end, double time);
		Vec2f blendPointsExplicit(double time);
		Vec2f blendPointsDerivativeRecursive(int start, int end, double time);

		std::vector<Vec2f> points = std::vector<Vec2f>();
	};

} //NAMESPACE glib END