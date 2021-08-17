#include "Quaternion.h"
#include "MathExt.h"

namespace glib
{

    const Class Quaternion::myClass = Class("Quaternion", {&Object::myClass});
    const Class* Quaternion::getClass()
    {
        return &Quaternion::myClass;
    }

    Quaternion::Quaternion()
    {

    }


    Quaternion::Quaternion(double a, double b, double c, double d)
    {
        this->a=a;
        this->b=b;
        this->c=c;
        this->d=d;
    }

    Quaternion::~Quaternion()
    {

    }

    Quaternion Quaternion::operator*(double val)
    {
        return Quaternion(val*a, val*b, val*c, val*d);
    }

    void Quaternion::operator*=(double val)
    {
        a*=val;
        b*=val;
        c*=val;
        d*=val;
    }

    Quaternion Quaternion::operator/(double val)
    {
        return Quaternion(val/a, val/b, val/c, val/d);
    }

    void Quaternion::operator/=(double val)
    {
        a/=val;
        b/=val;
        c/=val;
        d/=val;
    }

    Quaternion Quaternion::operator*(Quaternion other)
    {
        Quaternion newQuat = Quaternion();
        newQuat.a = (a*other.a - b*other.b - c*other.c - d*other.d);
        newQuat.b = (a*other.b - b*other.a - c*other.d - d*other.c);
        newQuat.c = (a*other.c - b*other.d - c*other.a - d*other.b);
        newQuat.c = (a*other.d - b*other.c - c*other.b - d*other.a);
        return newQuat;
    }

    Quaternion Quaternion::operator+(Quaternion other)
    {
        return Quaternion(a+other.a, b+other.b, c+other.c, d+other.d);
    }

    Quaternion Quaternion::operator-(Quaternion other)
    {
        return Quaternion(a-other.a, b-other.b, c-other.c, d-other.d);
    }

    void Quaternion::operator*=(Quaternion other)
    {
        double newA = (a*other.a - b*other.b - c*other.c - d*other.d);
        double newB = (a*other.b - b*other.a - c*other.d - d*other.c);
        double newC = (a*other.c - b*other.d - c*other.a - d*other.b);
        double newD = (a*other.d - b*other.c - c*other.b - d*other.a);
        
        a = newA;
        b = newB;
        c = newC;
        d = newD;
    }

    void Quaternion::operator+=(Quaternion other)
    {
        a+=other.a;
        b+=other.b;
        c+=other.c;
        d+=other.d;
    }

    void Quaternion::operator-=(Quaternion other)
    {
        a-=other.a;
        b-=other.b;
        c-=other.c;
        d-=other.d;
    }

    Vec3f Quaternion::operator*(Vec3f other)
    {
        Quaternion directionQuat = Quaternion(0, other.x, other.y, other.z);
        Quaternion output = (*(this)) * directionQuat * getConjugate();

        return Vec3f(output.b, output.c, output.d);
    }

    Quaternion Quaternion::getConjugate()
    {
        return Quaternion(a, -b, -c, -d);
    }

    Quaternion Quaternion::getInverse()
    {
        return MathExt::normalize( getConjugate() );
    }

} //NAMESPACE glib END