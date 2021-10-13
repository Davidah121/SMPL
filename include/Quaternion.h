#pragma once
#include "Object.h"
#include "Vec4f.h"

namespace glib
{

    class Quaternion : public Object
    {
    public:
        /**
         * @brief Construct a new Quaternion object
         * 
         */
        Quaternion();

        /**
         * @brief Construct a new Quaternion object.
         *      Takes the form ai + bj + ck + d
         * 
         * @param a 
         * @param b 
         * @param c 
         * @param d 
         */
        Quaternion(double a, double b, double c, double d);

        /**
         * @brief Destroy the Quaternion object
         * 
         */
        ~Quaternion();

        static const Class myClass;
        const Class* getClass();

        Quaternion operator*(double val);
        void operator*=(double val);

        friend Quaternion operator*(double val, Quaternion other)
        {
            return other*val;
        }

        Quaternion operator/(double val);
        void operator/=(double val);

        Quaternion operator*(Quaternion other);
        Quaternion operator+(Quaternion other);
        Quaternion operator-(Quaternion other);

        void operator*=(Quaternion other);
        void operator+=(Quaternion other);
        void operator-=(Quaternion other);
        
        Vec3f operator*(Vec3f other);

        /**
         * @brief Gets the Conjugate of the quaternion
         * 
         * @return Quaternion 
         */
        Quaternion getConjugate();

        /**
         * @brief Gets the Inverse of the quaternion
         * 
         * @return Quaternion 
         */
        Quaternion getInverse();

        /**
         * @brief Gets the Length of the quaternion
         * 
         * @return double 
         */
        double getLength();

        /**
		 * @brief Normalizes the quaternion
		 * 
		 * @return Quaternion
		 */
        Quaternion normalize();

        double a=0;
        double b=0;
        double c=0;
        double d=0;
    };

}