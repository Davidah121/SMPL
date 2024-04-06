#pragma once
#include<functional>

namespace smpl
{
    class MathFunction
    {
    public:
        MathFunction();
        ~MathFunction();

        double virtual solve(double x);
    private:
    };

    class GeneralMathFunction : public MathFunction
    {
    public:
        GeneralMathFunction();
        ~GeneralMathFunction();

        void setFunction(std::function<double(double)> f);
        double solve(double x);
    private:
        std::function<double(double)> func;
    };

    class PolynomialMathFunction
    {
    public:
        PolynomialMathFunction();
        PolynomialMathFunction(std::vector<double> constants);
        ~PolynomialMathFunction();

        PolynomialMathFunction getDerivative();
        void addConstant(double d);
        double getConstant(int index);
        size_t size();

        double solve(double x);
    private:
        std::vector<double> constants;
    };

} // namespace smpl