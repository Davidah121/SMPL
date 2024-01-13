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
         *      Takes the form xi + yj + zk + w
         * 
         * @param x 
         * @param y 
         * @param z 
         * @param w 
         */
        Quaternion(double x, double y, double z, double w);

        /**
         * @brief Destroy the Quaternion object
         * 
         */
        ~Quaternion();

        static const RootClass globalClass;

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

        double x=0;
        double y=0;
        double z=0;
        double w=0;
    };

}