#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include <functional>
#include <vector>
#include <string>

#ifndef __min
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef __max
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#endif

namespace smpl
{
	class DLL_OPTION MathFunction
	{
	public:
		MathFunction();
		~MathFunction();

		double virtual solve(double x) const;
	private:
	};

	class DLL_OPTION GeneralMathFunction : public MathFunction
	{
	public:
		GeneralMathFunction();
		GeneralMathFunction(const std::function<double(double)>& f);
		~GeneralMathFunction();

		virtual double solve(double x) const;
	private:
		std::function<double(double)> func;
	};

	class DLL_OPTION PolynomialMathFunction
	{
	public:
		PolynomialMathFunction();
		PolynomialMathFunction(const std::vector<double>& constants);
		~PolynomialMathFunction();
		std::string toString() const;

		void addConstant(double d);
		double getConstant(int index) const;
		size_t size() const;

		virtual double solve(double x) const;

		PolynomialMathFunction getDerivative() const;
		double& operator[](size_t index);
		double operator[](size_t index) const;
		
		PolynomialMathFunction operator+(const PolynomialMathFunction& g) const;
		PolynomialMathFunction operator-(const PolynomialMathFunction& g) const;
		PolynomialMathFunction operator*(const PolynomialMathFunction& g) const;
		PolynomialMathFunction operator+(double m) const;
		PolynomialMathFunction operator-(double m) const;
		PolynomialMathFunction operator*(double m) const;
		PolynomialMathFunction operator/(double m) const;
		
	private:
		std::vector<double> constants;
	};

} // namespace smpl