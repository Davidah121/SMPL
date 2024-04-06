#include "PolarCoordinate.h"
#include "MathExt.h"

namespace smpl
{
    
    const RootClass PolarCoordinate::globalClass = RootClass("PolarCoordinate", {"SerializedObject"});

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

    std::unordered_map<std::string, SerializedData> PolarCoordinate::getSerializedVariables()
	{
		return {SERIALIZE_MAP(length), SERIALIZE_MAP(angle)};
	}

    double PolarCoordinate::getLength()
    {
        return length;
    }

    double PolarCoordinate::getAngle()
    {
        return angle;
    }

} //NAMESPACE glib END