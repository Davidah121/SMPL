#include "ComplexNumber.h"

namespace smpl
{
	ComplexNumber::ComplexNumber()
	{
	}

	ComplexNumber::ComplexNumber(double real, double imaginary)
	{
		this->real = real;
		this->imaginary = imaginary;
	}
	
	ComplexNumber::~ComplexNumber()
	{
	}

	ComplexNumber ComplexNumber::conjugate() const
	{
		return ComplexNumber(real, -imaginary);
	}
	
	ComplexNumber ComplexNumber::normalize() const
	{
		double len = length();
		if(len != 0)
			return ComplexNumber(real/len, imaginary/len);
		return ComplexNumber();
	}
	
	double ComplexNumber::length() const
	{
		return sqrt(real*real + imaginary*imaginary);
	}

	Vec2f ComplexNumber::toVec2f() const
	{
		return Vec2f(real, imaginary);
	}

	ComplexNumber ComplexNumber::operator+(const ComplexNumber& other) const
	{
		return ComplexNumber(real+other.real, imaginary+other.imaginary);
	}

	ComplexNumber ComplexNumber::operator-(const ComplexNumber& other) const
	{
		return ComplexNumber(real-other.real, imaginary-other.imaginary);
	}

	ComplexNumber ComplexNumber::operator-() const
	{
		return ComplexNumber(-real, -imaginary);
	}

	ComplexNumber ComplexNumber::operator*(const ComplexNumber& other) const
	{
		//(a+bi)*(c+di)
		//(a*c + a*di + bi*c + bi*di)
		//real = a*c - b*d
		//imag = a*d + b*c

		ComplexNumber k = ComplexNumber();
		k.real = (real*other.real) - (imaginary*other.imaginary);
		k.imaginary = (real*other.imaginary) + (imaginary*other.real);

		return k;
	}

	ComplexNumber ComplexNumber::operator*(const double& other) const
	{
		return ComplexNumber(real*other,imaginary*other);
	}

	ComplexNumber ComplexNumber::operator/(const double& other) const
	{
		return ComplexNumber(real/other,imaginary/other);
	}
	
	ComplexNumber ComplexNumber::operator/(const ComplexNumber& other) const
	{
		ComplexNumber numerator = operator*(other.conjugate());
		ComplexNumber denominator = other*other.conjugate();
		//should be left with just a real number in the denominator
		return ComplexNumber(numerator.real / denominator.real, numerator.imaginary / denominator.real);
	}

	void ComplexNumber::operator+=(const ComplexNumber& other)
	{
		real += other.real;
		imaginary += other.imaginary;
	}

	void ComplexNumber::operator-=(const ComplexNumber& other)
	{
		real -= other.real;
		imaginary -= other.imaginary;
	}

	void ComplexNumber::operator*=(const ComplexNumber& other)
	{
		double newReal = (real*other.real) - (imaginary*other.imaginary);
		double newImag = (real*other.imaginary) + (imaginary*other.real);

		real = newReal;
		imaginary = newImag;
	}

	void ComplexNumber::operator*=(double other)
	{
		real*=other;
		imaginary*=other;
	}

	void ComplexNumber::operator/=(double other)
	{
		real/=other;
		imaginary/=other;
	}

	bool ComplexNumber::operator==(const ComplexNumber& other) const
	{
		return (real==other.real && imaginary==other.imaginary);
	}

	bool ComplexNumber::operator!=(const ComplexNumber& other) const
	{
		return (real!=other.real || imaginary!=other.imaginary);
	}
	
	void formatToString(StringStream& stream, const ComplexNumber& v, const std::string& options)
	{
		stream.write('(');
		formatToString(stream, v.real, options);
		stream.write(" + ");
		formatToString(stream, v.imaginary, options);
		stream.write("i)");
	}

} //NAMESPACE glib END