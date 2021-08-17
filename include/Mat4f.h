#pragma once
#include "Matrix.h"
#include "Vec4f.h"

namespace glib
{

	class Mat4f : public Matrix
	{
	public:
		Mat4f();
		Mat4f(double v1, double v2, double v3, double v4, 
			double v5, double v6, double v7, double v8,
			double v9, double v10, double v11, double v12, 
			double v13, double v14, double v15, double v16);
		Mat4f(const Mat4f& c);
		void operator=(const Mat4f& o);

		~Mat4f();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		static Mat4f getIdentity();

		double* operator[](int row);

		Mat4f operator+(Mat4f other);
		Mat4f operator-(Mat4f other);
		Mat4f operator*(Mat4f other);

		Mat4f operator*(int other);
		Mat4f operator*(float other);
		Mat4f operator*(double other);

		void operator*=(int other);
		void operator*=(float other);
		void operator*=(double other);

		void operator+=(Mat4f other);
		void operator-=(Mat4f other);

		Vec4f operator*(Vec4f other);

		bool operator==(Mat4f other);
		bool operator!=(Mat4f other);
	};

} //NAMESPACE glib END