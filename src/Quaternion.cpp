#include "Quaternion.h"
#include "MathExt.h"

namespace smpl
{

    const RootClass Quaternion::globalClass = RootClass("Quaternion", {"SerializedObject"});

    Quaternion::Quaternion()
    {
		setClass(globalClass);
    }


    Quaternion::Quaternion(double x, double y, double z, double w)
    {
		setClass(globalClass);
        this->x=x;
        this->y=y;
        this->z=z;
        this->w=w;
    }

    Quaternion::~Quaternion()
    {

    }

    std::unordered_map<std::string, SerializedData> Quaternion::getSerializedVariables()
	{
		return {SERIALIZE_MAP(x), SERIALIZE_MAP(y), SERIALIZE_MAP(z), SERIALIZE_MAP(w)};
	}

    Quaternion Quaternion::operator*(double val)
    {
        return Quaternion(val*x, val*y, val*z, val*w);
    }

    void Quaternion::operator*=(double val)
    {
        x*=val;
        y*=val;
        z*=val;
        w*=val;
    }

    Quaternion Quaternion::operator/(double val)
    {
        return Quaternion(val/x, val/y, val/z, val/w);
    }

    void Quaternion::operator/=(double val)
    {
        x/=val;
        y/=val;
        z/=val;
        w/=val;
    }

    Quaternion Quaternion::operator*(Quaternion other)
    {
        Quaternion newQuat = Quaternion();

        newQuat.w = (w*other.w - x*other.x - y*other.y - z*other.z);
        newQuat.x = (w*other.x + x*other.w + y*other.z - z*other.y);
        newQuat.y = (w*other.y - x*other.z + y*other.w + z*other.x);
        newQuat.z = (w*other.z + x*other.y - y*other.x + z*other.w);
        
        return newQuat;
    }

    Quaternion Quaternion::operator+(Quaternion other)
    {
        return Quaternion(x+other.x, y+other.y, z+other.z, w+other.w);
    }

    Quaternion Quaternion::operator-(Quaternion other)
    {
        return Quaternion(x-other.x, y-other.y, z-other.z, w-other.w);
    }

    void Quaternion::operator*=(Quaternion other)
    {
        double newW = (w*other.w - x*other.x - y*other.y - z*other.z);
        double newX = (w*other.x + x*other.w + y*other.z - z*other.y);
        double newY = (w*other.y - x*other.z + y*other.w + z*other.x);
        double newZ = (w*other.z + x*other.y - y*other.x + z*other.w);
        
        x = newX;
        y = newY;
        z = newZ;
        w = newW;
    }

    void Quaternion::operator+=(Quaternion other)
    {
        x+=other.x;
        y+=other.y;
        z+=other.z;
        w+=other.w;
    }

    void Quaternion::operator-=(Quaternion other)
    {
        x-=other.x;
        y-=other.y;
        z-=other.z;
        w-=other.w;
    }

    Vec3f Quaternion::operator*(Vec3f other)
    {
        Quaternion directionQuat = Quaternion(other.x, other.y, other.z, 0);
        Quaternion output = (*(this)) * directionQuat * getConjugate();

        return Vec3f(output.x, output.y, output.z);
    }

    Quaternion Quaternion::getConjugate()
    {
        return Quaternion(-x, -y, -z, w);
    }

    Quaternion Quaternion::getInverse()
    {
        return MathExt::normalize( getConjugate() );
    }

    double Quaternion::getLength()
    {
        return sqrt(x*x + y*y + z*z + w*w);
    }

    Quaternion Quaternion::normalize()
    {
        double l = getLength();
        return Quaternion(x/l, y/l, z/l, w/l);
    }

} //NAMESPACE glib END