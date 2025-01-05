#include "MathFunction.h"
#include "StringTools.h"
namespace smpl
{

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

    std::string PolynomialMathFunction::toString() const
    {
        std::string output;
        bool firstTerm = true;
        for(int j=size()-1; j>=0; j--)
        {
            if(constants[j] != 0)
            {
                if(!firstTerm)
                    output += " + ";
                if(j == 0)
                    output += StringTools::formatString("%.9f", constants[j]);
                else if(j == 1)
                    output += StringTools::formatString("%.9fx", constants[j]);
                else
                    output += StringTools::formatString("%.9fx^%d", constants[j], j);
                firstTerm = false;
            }
        }
        return output;
    }

    double PolynomialMathFunction::solve(double x) const
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

    double PolynomialMathFunction::getConstant(int index) const
    {
        return constants[index];
    }

    size_t PolynomialMathFunction::size() const
    {
        return constants.size();
    }

    PolynomialMathFunction PolynomialMathFunction::getDerivative() const
    {
        //constants[0] = lowest degree -> C*X^0 -> C
        //constants[n] = highest degree -> C*X^N
        PolynomialMathFunction newFunction;
        newFunction = std::vector<double>(size()-1);
        for(size_t i=1; i<size(); i++)
        {
            newFunction.constants[i-1] = constants[i]*i;
        }
        return newFunction;
    }
    
    double& PolynomialMathFunction::operator[](size_t index)
    {
        return constants[index];
    }

    PolynomialMathFunction PolynomialMathFunction::operator+(const PolynomialMathFunction& g) const
    {
        PolynomialMathFunction newFunction;
        newFunction.constants = std::vector<double>(__max(size(), g.size()));
        size_t minSize = __min(size(), g.size());
        for(size_t i=0; i<minSize; i++)
            newFunction.constants[i] = constants[i] + g.constants[i];
        
        if(size() == newFunction.size())
        {
            for(size_t i=minSize; i<size(); i++)
                newFunction.constants[i] = constants[i];
        }
        else
        {
            for(size_t i=minSize; i<g.size(); i++)
                newFunction.constants[i] = g.constants[i];
        }
        
        return newFunction;
    }
    PolynomialMathFunction PolynomialMathFunction::operator-(const PolynomialMathFunction& g) const
    {
        PolynomialMathFunction newFunction;
        newFunction.constants = std::vector<double>(__max(size(), g.size()));
        size_t minSize = __min(size(), g.size());
        for(size_t i=0; i<minSize; i++)
            newFunction.constants[i] = constants[i] - g.constants[i];
        
        if(size() == newFunction.size())
        {
            for(size_t i=minSize; i<size(); i++)
                newFunction.constants[i] = constants[i];
        }
        else
        {
            for(size_t i=minSize; i<g.size(); i++)
                newFunction.constants[i] = -g.constants[i];
        }
        
        return newFunction;
    }
    PolynomialMathFunction PolynomialMathFunction::operator*(const PolynomialMathFunction& g) const
    {
        //foil
        PolynomialMathFunction newFunction;
        newFunction.constants = std::vector<double>(size()+g.size());
        for(size_t i=0; i<size(); i++)
        {
            for(size_t j=0; j<g.size(); j++)
            {
                //index = i+j
                size_t index = i+j;
                newFunction[index] += constants[i] * g.constants[j];
            }
        }

        //remove leading zeroes
        while (true)
        {
            if(newFunction.constants.size() > 0 && newFunction.constants.back() == 0)
                newFunction.constants.pop_back();
            else
                break;
        }
        return newFunction;
    }
    PolynomialMathFunction PolynomialMathFunction::operator+(double m) const
    {
        PolynomialMathFunction newFunction;
        newFunction.constants = std::vector<double>(size());
        for(size_t i=0; i<size(); i++)
            newFunction.constants[i] = constants[i] + m;
        
        return newFunction;
    }
    PolynomialMathFunction PolynomialMathFunction::operator-(double m) const
    {
        PolynomialMathFunction newFunction;
        newFunction.constants = std::vector<double>(size());
        for(size_t i=0; i<size(); i++)
            newFunction.constants[i] = constants[i] - m;
        
        return newFunction;
    }
    PolynomialMathFunction PolynomialMathFunction::operator*(double m) const
    {
        PolynomialMathFunction newFunction;
        newFunction.constants = std::vector<double>(size());
        for(size_t i=0; i<size(); i++)
            newFunction.constants[i] = constants[i] * m;
        
        return newFunction;
    }
    PolynomialMathFunction PolynomialMathFunction::operator/(double m) const
    {
        PolynomialMathFunction newFunction;
        newFunction.constants = std::vector<double>(size());
        for(size_t i=0; i<size(); i++)
            newFunction.constants[i] = constants[i] / m;
        
        return newFunction;
    }

    #pragma endregion

}