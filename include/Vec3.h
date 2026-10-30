#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "SimpleSerialization.h"
#include "NewStringFormatting.h"
#include "Vec2.h"

namespace smpl
{
	
	template<typename T>
	class DLL_OPTION Vec3;

	typedef Vec3<float> Vec3f;
	typedef Vec3<double> Vec3d;
	typedef Vec3<int> Vec3i;

	template<typename T>
	class DLL_OPTION Vec3
	{
	public:
	
		Vec3() = default;
		Vec3(const Vec3<T>& other) = default;
		Vec3<T>& operator=(const Vec3<T>& other) = default;
		Vec3(Vec3<T>&& other) = default;
		Vec3<T>& operator=(Vec3<T>&& other) = default;
		~Vec3() = default;

		/**
		 * @brief Construct a new Vec3 object
		 * 
		 * @param x 
		 * @param y 
		 * @param z
		 */
		Vec3(T x, T y, T z){this->x = x; this->y = y; this->z = z;}
		Vec3(const Vec2<T>& v, T z = 0){this->x = v.x; this->y = v.y; this->z = z;}
		
		
		/**
		 * @brief Converts the Vec4f object to a Vec2 object by dropping the w value.
		 * 
		 * @return Vec2 
		 */
		Vec2<T> toVec2() const {return Vec2<T>(x, y);}

		/**
		 * @brief Get the Length of the vector
		 * 
		 * @return T 
		 */
		T getLength() const {return sqrt(x*x + y*y + z*z);}

		/**
		 * @brief Gets the normalized version of the vector.
		 * 		Returns a vector with the same direction but a length of 1.
		 * 
		 * @return Vec3<T> 
		 */
		Vec3<T> normalize() const
		{
			T len = getLength();
			return Vec3<T>(x/len, y/len, z/len);
		}

		/**
		 * @brief Computes the dot product between this vector and another
		 * 
		 * @param other 
		 * @return T 
		 */
		T dot(const Vec3<T>& b) const {return x*b.x + b.y*y + z*b.z;}

		/**
		 * @brief Computes the scalar vector projection between 2 vectors
		 * 
		 * @param other 
		 * @return T 
		 */
		T project(const Vec3<T>& b) const {return dot(b.normalize());}

		/**
		 * @brief Reflects this vector across the vector "b"
		 * 
		 * @param other 
		 * @return Vec3<T> 
		 */
		Vec3<T> reflect(const Vec3<T>& b) const
		{
			Vec3<T> n = b.normalize();
			return this->operator-(n*(dot(n)*2));
		}

		/**
		 * @brief Returns a new vector that is the inverse.
		 * 		Returns this : Vec3<T>( -f.y, f.x )
		 * 
		 * @return Vec3 
		 */
		Vec3<T> cross(const Vec3<T>& v2) const
		{
			T cx = y*v2.z - z*v2.y;
			T cy = z*v2.x - x*v2.z;
			T cz = x*v2.y - y*v2.x;
			return Vec3<T>(cx, cy, cz);
		}
		
		Vec3<T> operator-(const Vec3<T>& b) const { return Vec3<T>(x-b.x, y-b.y, z-b.z); }
		Vec3<T> operator+(const Vec3<T>& b) const { return Vec3<T>(x+b.x, y+b.y, z+b.z); }

		void operator+=(const Vec3<T>& b) {x += b.x; y += b.y; z += b.z;}
		void operator-=(const Vec3<T>& b) {x -= b.x; y -= b.y; z -= b.z;}

		void operator*=(T b) { x *= b; y *= b; z *= b;}
		void operator/=(T b) { x /= b; y /= b; z /= b;}

		Vec3<T> operator-() const {return Vec3<T>(-x, -y, -z);}

		Vec3<T> operator*(T value) const { return Vec3<T>(x*value, y*value, z*value); }
		Vec3<T> operator/(T value) const { return Vec3<T>(x/value, y/value, z/value); }
		
		Vec3<T> operator*(const Vec3<T>& b) const {return Vec3<T>(x*b.x, y*b.y, z*b.z);}
		Vec3<T> operator/(const Vec3<T>& b) const {return Vec3<T>(x/b.x, y/b.y, z/b.z);}

		friend Vec3<T> operator*(T value, const Vec3<T>& other)
		{
			return other*value;
		}

		Vec3<T> broadcastAdd(T v) const {return Vec3<T>(x+v, y+v, z+v);}
		Vec3<T> broadcastSubtract(T v) const {return Vec3<T>(x-v, y-v, z-v);}
		Vec3<T> broadcastFunction(std::function<T(T)> func) const
		{
			if(func == nullptr)
				throw std::bad_function_call();
			return Vec3<T>(func(x), func(y), func(z));
		}
		Vec3<T> broadcastFunction(T (*func)(T)) const
		{
			if(func == nullptr)
				throw std::bad_function_call();
			return Vec3<T>(func(x), func(y), func(z));
		}

		bool operator==(const Vec3<T>& b) const {return x==x && y==y && z==z;}
		bool operator!=(const Vec3<T>& b) const {return x!=x || y!=y || z!=z;}
		
		T x = 0;
		T y = 0;
		T z = 0;
	
		SERIALIZE_CLASS_LIGHT(Vec3<T>, x, y)
	};

	template<typename T>
	Vec3<T> min(const Vec3<T>& a, const Vec3<T>& b)
	{
		return Vec3<T>(min<T>(a.x, b.x), min<T>(a.y, b.y), min<T>(a.z, b.z));
	}
	template<typename T>
	Vec3<T> max(const Vec3<T>& a, const Vec3<T>& b)
	{
		return Vec3<T>(max<T>(a.x, b.x), max<T>(a.y, b.y), max<T>(a.z, b.z));
	}

	template<typename T>
	void formatToString(StringStream& stream, const Vec3<T>& v, const std::string& options)
	{
		stream.write('<');
		formatToString(stream, v.x, options);
		stream.write(", ");
		formatToString(stream, v.y, options);
		stream.write(", ");
		formatToString(stream, v.z, options);
		stream.write('>');
	}
} //NAMESPACE glib END