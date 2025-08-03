#include <catch_amalgamated.hpp>
#include "MathExt.h"
#include "BezierCurve.h"
#include "StringTools.h"

//Note that all solutions where found to 3 decimals of accuracy so the Epsilon is relatively large.
//Should get solutions to more decimal places to better unit test for small errors.
#define BEZIER_EPSILON 1e-3

TEST_CASE("Testing of the BezierCurve class", "[BezierCurve]")
{
	smpl::BezierCurve mainCurve = smpl::BezierCurve();
	mainCurve.addPoint(smpl::Vec2f(0, 0));
	mainCurve.addPoint(smpl::Vec2f(1, 0));
	mainCurve.addPoint(smpl::Vec2f(1, 1));
	
	
	SECTION("Testing Copy Construction")
	{
		smpl::BezierCurve secondCurve = mainCurve;

		REQUIRE( mainCurve.size() == 3 );
		REQUIRE( mainCurve.size() == secondCurve.size() );
		REQUIRE( ( mainCurve.getPoint(0) == secondCurve.getPoint(0) ) );
		REQUIRE( ( mainCurve.getPoint(1) == secondCurve.getPoint(1) ) );
		REQUIRE( ( mainCurve.getPoint(2) == secondCurve.getPoint(2) ) );
	}

	SECTION("Testing Evaluation, Derivative")
	{
		SECTION("EVAL")
		{
			
			smpl::Vec2f actualV1 = smpl::MathExt::sqr(1-0) * smpl::Vec2f(0, 0) + 2*0*(1-0)*smpl::Vec2f(1, 0) + smpl::MathExt::sqr(0)*smpl::Vec2f(1, 1);
			smpl::Vec2f actualV2 = smpl::MathExt::sqr(1-0.25) * smpl::Vec2f(0, 0) + 2*0.25*(1-0.25)*smpl::Vec2f(1, 0) + smpl::MathExt::sqr(0.25)*smpl::Vec2f(1, 1);
			smpl::Vec2f actualV3 = smpl::MathExt::sqr(1-0.5) * smpl::Vec2f(0, 0) + 2*0.5*(1-0.5)*smpl::Vec2f(1, 0) + smpl::MathExt::sqr(0.5)*smpl::Vec2f(1, 1);
			smpl::Vec2f actualV4 = smpl::MathExt::sqr(1-0.75) * smpl::Vec2f(0, 0) + 2*0.75*(1-0.75)*smpl::Vec2f(1, 0) + smpl::MathExt::sqr(0.75)*smpl::Vec2f(1, 1);
			smpl::Vec2f actualV5 = smpl::MathExt::sqr(1-1) * smpl::Vec2f(0, 0) + 2*1*(1-1)*smpl::Vec2f(1, 0) + smpl::MathExt::sqr(1)*smpl::Vec2f(1, 1);
			
			smpl::Vec2f v1 = mainCurve.getFunctionAt(0) - actualV1;
			smpl::Vec2f v2 = mainCurve.getFunctionAt(0.25) - actualV2;
			smpl::Vec2f v3 = mainCurve.getFunctionAt(0.5) - actualV3;
			smpl::Vec2f v4 = mainCurve.getFunctionAt(0.75) - actualV4;
			smpl::Vec2f v5 = mainCurve.getFunctionAt(1) - actualV5;

			v1.x = smpl::MathExt::roundToDecimal(abs(v1.x), 3);	v2.x = smpl::MathExt::roundToDecimal(abs(v2.x), 3);	v3.x = smpl::MathExt::roundToDecimal(abs(v3.x), 3);	v4.x = smpl::MathExt::roundToDecimal(abs(v4.x), 3);	v5.x = smpl::MathExt::roundToDecimal(abs(v5.x), 3);
			v1.y = smpl::MathExt::roundToDecimal(abs(v1.y), 3);	v2.y = smpl::MathExt::roundToDecimal(abs(v2.y), 3);	v3.y = smpl::MathExt::roundToDecimal(abs(v3.y), 3);	v4.y = smpl::MathExt::roundToDecimal(abs(v4.y), 3);	v5.y = smpl::MathExt::roundToDecimal(abs(v5.y), 3);

			REQUIRE( (v1.x <= BEZIER_EPSILON) );
			REQUIRE( (v1.y <= BEZIER_EPSILON) );
			REQUIRE( (v2.x <= BEZIER_EPSILON) );
			REQUIRE( (v2.y <= BEZIER_EPSILON) );
			REQUIRE( (v3.x <= BEZIER_EPSILON) );
			REQUIRE( (v3.y <= BEZIER_EPSILON) );
			REQUIRE( (v4.x <= BEZIER_EPSILON) );
			REQUIRE( (v4.y <= BEZIER_EPSILON) );
			REQUIRE( (v5.x <= BEZIER_EPSILON) );
			REQUIRE( (v5.y <= BEZIER_EPSILON) );
		}
		SECTION("DERIVATIVE")
		{
			smpl::Vec2f v1 = mainCurve.getDerivativeAt(0) - smpl::Vec2f(2,0);
			smpl::Vec2f v2 = mainCurve.getDerivativeAt(0.25) - smpl::Vec2f(1.5,0.5);
			smpl::Vec2f v3 = mainCurve.getDerivativeAt(0.5) - smpl::Vec2f(1.0,1.0);
			smpl::Vec2f v4 = mainCurve.getDerivativeAt(0.75) - smpl::Vec2f(0.5,1.5);
			smpl::Vec2f v5 = mainCurve.getDerivativeAt(1) - smpl::Vec2f(0,2);

			v1.x = abs(v1.x);	v2.x = abs(v2.x);	v3.x = abs(v3.x);	v4.x = abs(v4.x);	v5.x = abs(v5.x);
			v1.y = abs(v1.y);	v2.y = abs(v2.y);	v3.y = abs(v3.y);	v4.y = abs(v4.y);	v5.y = abs(v5.y);
			
			REQUIRE( (v1.x <= BEZIER_EPSILON) );
			REQUIRE( (v1.y <= BEZIER_EPSILON) );
			REQUIRE( (v2.x <= BEZIER_EPSILON) );
			REQUIRE( (v2.y <= BEZIER_EPSILON) );
			REQUIRE( (v3.x <= BEZIER_EPSILON) );
			REQUIRE( (v3.y <= BEZIER_EPSILON) );
			REQUIRE( (v4.x <= BEZIER_EPSILON) );
			REQUIRE( (v4.y <= BEZIER_EPSILON) );
			REQUIRE( (v5.x <= BEZIER_EPSILON) );
			REQUIRE( (v5.y <= BEZIER_EPSILON) );
		}
	}

	SECTION("Testing for Specific Solution")
	{
		std::vector<double> solveXTimes = mainCurve.findTimeForX(0.85);
		std::vector<double> solveYTimes = mainCurve.findTimeForY(0.85);
		std::vector<double> solveXYTimes = mainCurve.findTimeForPoint(0.75, 0.25);
		
		REQUIRE( solveXTimes.size() == 1 );
		REQUIRE( solveYTimes.size() == 1 );
		REQUIRE( solveXYTimes.size() == 1 );

		REQUIRE( smpl::MathExt::roundToDecimal(solveXTimes[0], 4) - 0.6125 <= BEZIER_EPSILON );
		REQUIRE( smpl::MathExt::roundToDecimal(solveYTimes[0], 4) - 0.922 <= BEZIER_EPSILON );
		REQUIRE( smpl::MathExt::roundToDecimal(solveXYTimes[0], 4) - 0.5 <= BEZIER_EPSILON );
	}

	SECTION("Testing Subdivide, Extract, and BoundingBox")
	{
		SECTION("SUBDIVIDE")
		{
			std::vector<smpl::BezierCurve> splitCurves = mainCurve.subdivide(0.5);
			
			REQUIRE( splitCurves.size() == 2);
			REQUIRE( splitCurves[0].size() == 3 );
			REQUIRE( splitCurves[1].size() == 3 );

			//Test
			REQUIRE( (splitCurves[0].getPoint(0) == smpl::Vec2f(0, 0)) );
			REQUIRE( (splitCurves[0].getPoint(1) == smpl::Vec2f(0.5, 0)) );
			REQUIRE( (splitCurves[0].getPoint(2) == smpl::Vec2f(0.75, 0.25)) );

			REQUIRE( (splitCurves[1].getPoint(0) == smpl::Vec2f(0.75, 0.25)) );
			REQUIRE( (splitCurves[1].getPoint(1) == smpl::Vec2f(1, 0.5)) );
			REQUIRE( (splitCurves[1].getPoint(2) == smpl::Vec2f(1, 1)) );
		}

		SECTION("EXTRACT")
		{
			smpl::BezierCurve nCurve = mainCurve.extract(0.0, 0.5);
			
			REQUIRE( nCurve.size() == 3);

			//Test
			REQUIRE( (nCurve.getPoint(0) == smpl::Vec2f(0, 0)) );
			REQUIRE( (nCurve.getPoint(1) == smpl::Vec2f(0.5, 0)) );
			REQUIRE( (nCurve.getPoint(2) == smpl::Vec2f(0.75, 0.25)) );
		}

		SECTION("BOUNDING BOX")
		{
			std::vector<smpl::Vec2f> corners = mainCurve.getBoundingBox();

			REQUIRE( corners.size() == 2 );
			REQUIRE( (corners[0] == smpl::Vec2f(0, 0)) );
			REQUIRE( (corners[1] == smpl::Vec2f(1, 1)) );
		}
	}
}