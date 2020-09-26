#pragma once
#include "Object.h"
#include "Vec4f.h"

class Quaternion : public Object
{
public:
    Quaternion();
    Quaternion(double a, double b, double c, double d);
    ~Quaternion();

    static const Class* myClass;
    const Class* getClass();

    Quaternion operator*(double val);
    void operator*=(double val);

    Quaternion operator/(double val);
    void operator/=(double val);

    Quaternion operator*(Quaternion other);
    Quaternion operator+(Quaternion other);
    Quaternion operator-(Quaternion other);

    void operator*=(Quaternion other);
    void operator+=(Quaternion other);
    void operator-=(Quaternion other);
    
    Vec3f operator*(Vec3f other);

    Quaternion getConjugate();
    Quaternion getInverse();

    double a=0;
    double b=0;
    double c=0;
    double d=0;
};