#include "PolarCoordinate.h"
#include "MathExt.h"

PolarCoordinate::PolarCoordinate()
{

}

PolarCoordinate::PolarCoordinate(double length, double angle)
{
    this->length = length;
    this->angle = angle;
}

PolarCoordinate::PolarCoordinate(Vec2f coord)
{
    length = MathExt::vecLength(coord);
    angle = MathExt::dirToPoint(0, 0, coord.x, coord.y);
}

PolarCoordinate::~PolarCoordinate()
{
    
}

double PolarCoordinate::getLength()
{
    return length;
}

double PolarCoordinate::getAngle()
{
    return angle;
}