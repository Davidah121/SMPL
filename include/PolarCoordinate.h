#pragma once
#include "Vec2f.h"

namespace glib
{

    class PolarCoordinate
    {
    public:
        PolarCoordinate();
        PolarCoordinate(double length, double angle);
        PolarCoordinate(Vec2f point);
        ~PolarCoordinate();

        double getLength();
        double getAngle();
        
    private:
        double length;
        double angle;
    };

} //NAMESPACE glib END