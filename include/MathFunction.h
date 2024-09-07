#pragma once
#include "BuildOptions.h"
#include <functional>

namespace smpl
{
    class DLL_OPTION MathFunction
    {
    public:
        MathFunction();
        ~MathFunction();

        double virtual solve(double x);
    private:
    };

    class DLL_OPTION GeneralMathFunction : public MathFunction
    {
    public:
        GeneralMathFunction();
        ~GeneralMathFunction();

        void setFunction(std::function<double(double)> f);
        virtual double solve(double x);
    private:
        std::function<double(double)> func;
    };

    class DLL_OPTION PolynomialMathFunction
    {
    public:
        PolynomialMathFunction();
        PolynomialMathFunction(std::vector<double> constants);
        ~PolynomialMathFunction();

        PolynomialMathFunction getDerivative();
        void addConstant(double d);
        double getConstant(int index);
        size_t size();

        virtual double solve(double x);
    private:
        std::vector<double> constants;
    };

} // namespace smpl