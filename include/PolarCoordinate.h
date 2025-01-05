#pragma once
#include "BuildOptions.h"
#include "Vec2f.h"

namespace smpl
{

    class DLL_OPTION PolarCoordinate : SerializedObject
    {
    public:
        /**
         * @brief Construct a new Polar Coordinate object
         * 
         */
        PolarCoordinate();

        /**
         * @brief Construct a new Polar Coordinate object
         * 
         * @param length 
         * @param angle 
         *      The angle is expected to be in radians.
         */
        PolarCoordinate(double length, double angle);

        /**
         * @brief Construct a new Polar Coordinate object
         *      Converts the Cartesian corrdiante to a polar coordinate.
         * 
         * @param point 
         */
        PolarCoordinate(Vec2f point);

        /**
         * @brief Destroy the Polar Coordinate object
         * 
         */
        ~PolarCoordinate();

        /**
         * @brief Gets the Length of the polar coordinate
         * 
         * @return double 
         */
        double getLength();

        /**
         * @brief Gets the Angle of the polar coordinate.
         *      The angle is in radians
         * 
         * @return double 
         */
        double getAngle();
        
    private:
        double length;
        double angle;
        
    SERIALIZE_CLASS(length, angle)
    };

} //NAMESPACE smpl END