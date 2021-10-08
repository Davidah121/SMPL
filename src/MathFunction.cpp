#include "MathFunction.h"

#pragma region MATH_FUNCTION

MathFunction::MathFunction()
{
}

MathFunction::~MathFunction()
{
}

double MathFunction::solve(double x)
{
    return 0;
}

#pragma endregion

#pragma region GENERAL_MATH_FUNCTION

GeneralMathFunction::GeneralMathFunction()
{
}

GeneralMathFunction::~GeneralMathFunction()
{
}

double GeneralMathFunction::solve(double x)
{
    if(func != nullptr)
        return func(x);
    return 0;
}

void GeneralMathFunction::setFunction(std::function<double(double)> f)
{
    func = f;
}

#pragma endregion

#pragma region POLYNOMIAL_MATH_FUNCTION

PolynomialMathFunction::PolynomialMathFunction()
{
}

PolynomialMathFunction::PolynomialMathFunction(std::vector<double> constants)
{
    this->constants = constants;
}

PolynomialMathFunction::~PolynomialMathFunction()
{
}

double PolynomialMathFunction::solve(double x)
{
    if(size() == 0)
        return 0;
    
    double solution = constants[constants.size()-1];
    for(int i=constants.size()-2; i>=0; i--)
    {
        solution = x*solution + constants[i];
    }

    return solution;
}

void PolynomialMathFunction::addConstant(double d)
{
    constants.push_back(d);
}

double PolynomialMathFunction::getConstant(int index)
{
    return constants[index];
}

size_t PolynomialMathFunction::size()
{
    return constants.size();
}

#pragma endregion