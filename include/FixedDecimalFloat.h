#pragma once
#include <cstdint>

namespace smpl
{

    struct Saturated_FP16
    {
        int16_t a;
        Saturated_FP16() {};
        Saturated_FP16(const float b)
        { 
            if(b>=1)
                a=0x7FFF;
            else if(b<=-1)
                a=0x8000;
            else
                a = (int16_t)(b*0x7FFF);
        }
        Saturated_FP16(const double b)
        { 
            if(b>=1)
                a=0x7FFF;
            else if(b<=-1)
                a=0x8000;
            else
                a = (int16_t)(b*0x7FFF);
        }
        Saturated_FP16(const Saturated_FP16& other) { a = other.a; }
        void operator=(const Saturated_FP16& other) { a = other.a; }
        void operator=(const float& b)
        { 
            if(b>=1)
                a=0x7FFF;
            else if(b<=-1)
                a=0x8000;
            else
                a = (int16_t)(b*0x7FFF);
        }
        void operator=(const double& b)
        { 
            if(b>=1)
                a=0x7FFF;
            else if(b<=-1)
                a=0x8000;
            else
                a = (int16_t)(b*0x7FFF);
        }

        void operator+=(const float b) { a = operator+(b).a; }
        void operator+=(const double b) { a = operator+(b).a; }
        void operator+=(const Saturated_FP16 b) { a = operator+(b).a; }
        void operator-=(const float b) { a = operator-(b).a; }
        void operator-=(const double b) { a = operator-(b).a; }
        void operator-=(const Saturated_FP16 b) { a = operator-(b).a; }
        void operator*=(const float b) { a = operator*(b).a; }
        void operator*=(const double b) { a = operator*(b).a; }
        void operator*=(const Saturated_FP16 b) { a = operator*(b).a; }
        void operator/=(const float b) { a = operator/(b).a; }
        void operator/=(const double b) { a = operator/(b).a; }
        void operator/=(const Saturated_FP16 b) { a = operator/(b).a; }

        Saturated_FP16 operator-() { return -((double)*this); }

        bool operator==(const Saturated_FP16 b) { return a == b.a; };
        bool operator!=(const Saturated_FP16 b) { return a != b.a; };
        bool operator<(const Saturated_FP16 b) { return a < b.a; };
        bool operator>(const Saturated_FP16 b) { return a > b.a; };
        bool operator<=(const Saturated_FP16 b) { return a <= b.a; };
        bool operator>=(const Saturated_FP16 b) { return a >= b.a; };

        friend double operator*(double value, Saturated_FP16 other) { return value*(double)other; }
        friend float operator*(float value, Saturated_FP16 other) { return value*(float)other; }
        friend double operator/(double value, Saturated_FP16 other) { return value/(double)other; }
        friend float operator/(float value, Saturated_FP16 other) { return value/(float)other; }
        friend double operator+(double value, Saturated_FP16 other) { return value+(double)other; }
        friend float operator+(float value, Saturated_FP16 other) { return value+(float)other; }
        friend double operator-(double value, Saturated_FP16 other) { return value-(double)other; }
        friend float operator-(float value, Saturated_FP16 other) { return value-(float)other; }

        Saturated_FP16 operator+(const float b)
        {
            Saturated_FP16 res;
            res.a = saturatedAdd(a, (int16_t)(b*0x7FFF));
            return res;
        }

        Saturated_FP16 operator+(const double b)
        {
            Saturated_FP16 res;
            res.a = saturatedAdd(a, (int16_t)(b*0x7FFF));
            return res;
        }
        
        Saturated_FP16 operator+(const Saturated_FP16 b)
        {
            Saturated_FP16 res;
            res.a = saturatedAdd(a, b.a);
            return res;
        }

        Saturated_FP16 operator-(const float b)
        {
            Saturated_FP16 res;
            res.a = saturatedSubtraction(a, (int16_t)(b*0x7FFF));
            return res;
        }

        Saturated_FP16 operator-(const double b)
        {
            Saturated_FP16 res;
            res.a = saturatedSubtraction(a, (int16_t)(b*0x7FFF));
            return res;
        }

        Saturated_FP16 operator-(const Saturated_FP16 b)
        {
            Saturated_FP16 res;
            res.a = saturatedSubtraction(a, b.a);
            return res;
        }

        Saturated_FP16 operator*(const float b)
        {
            return ((float)*this) * b;
        }

        Saturated_FP16 operator*(const double b)
        {
            return ((double)*this) * b;
        }

        Saturated_FP16 operator*(const Saturated_FP16 b)
        {
            return ((float)*this) * ((float)b);
        }

        Saturated_FP16 operator/(const float b)
        {
            return ((float)*this) / b;
        }

        Saturated_FP16 operator/(const double b)
        {
            return ((double)*this) / b;
        }

        Saturated_FP16 operator/(const Saturated_FP16 b)
        {
            return ((float)*this) / ((float)b);
        }

        operator float() const
        {
            return (a&0x8000) ? ((float)(a & 0x7FFF) / 0x7FFF)-1.0 : (float)(a & 0x7FFF) / 0x7FFF;
        }
        operator double() const
        {
            return (a&0x8000) ? ((double)(a & 0x7FFF) / 0x7FFF)-1.0 : (double)(a & 0x7FFF) / 0x7FFF;
        }
    private:
        //same as MathExt::saturatedSubtraction but no dependency
        static int16_t saturatedAdd(int16_t v1, int16_t v2)
        {
            //https://locklessinc.com/articles/sat_arithmetic/
            uint16_t a = v1;
            uint16_t b = v2;
            uint16_t res = a + b;
    
            a = (a>>15) + INT16_MAX; //overflow stuff
            if( (int16_t)((a^b) | ~(b^res)) >= 0) //force cmovns
                res = a;
            return res;
        }
        static int16_t saturatedSubtraction(int16_t v1, int16_t v2)
        {
            //https://locklessinc.com/articles/sat_arithmetic/
            uint16_t a = v1;
            uint16_t b = v2;
            uint16_t res = a - b;
    
            a = (a>>15) + INT16_MAX; //overflow stuff
            if( (int16_t)((a^b) & (a^res)) < 0) //force cmovns
                res = a;
            return res;
        }
    };

    struct Saturated_UFP16
    {
        uint16_t a;
        Saturated_UFP16() {};
        Saturated_UFP16(const float b) { a = (b<1.0) ? (uint16_t)(b*0xFFFF) : 0xFFFF; }
        Saturated_UFP16(const double b) { a = (b<1.0) ? (uint16_t)(b*0xFFFF) : 0xFFFF; }
        Saturated_UFP16(const Saturated_UFP16& other) { a = other.a; }
        void operator=(const Saturated_UFP16& other) { a = other.a; }
        void operator=(const float& b) { a = (b<1.0) ? (uint16_t)(b*0xFFFF) : 0xFFFF; }
        void operator=(const double& b) { a = (b<1.0) ? (uint16_t)(b*0xFFFF) : 0xFFFF; }

        void operator+=(const float b) { a = operator+(b).a; }
        void operator+=(const double b) { a = operator+(b).a; }
        void operator+=(const Saturated_UFP16 b) { a = operator+(b).a; }
        void operator-=(const float b) { a = operator-(b).a; }
        void operator-=(const double b) { a = operator-(b).a; }
        void operator-=(const Saturated_UFP16 b) { a = operator-(b).a; }
        void operator*=(const float b) { a = operator*(b).a; }
        void operator*=(const double b) { a = operator*(b).a; }
        void operator*=(const Saturated_UFP16 b) { a = operator*(b).a; }
        void operator/=(const float b) { a = operator/(b).a; }
        void operator/=(const double b) { a = operator/(b).a; }
        void operator/=(const Saturated_UFP16 b) { a = operator/(b).a; }

        Saturated_UFP16 operator-() { return -((double)*this); }

        bool operator==(const Saturated_UFP16 b) { return a == b.a; };
        bool operator!=(const Saturated_UFP16 b) { return a != b.a; };
        bool operator<(const Saturated_UFP16 b) { return a < b.a; };
        bool operator>(const Saturated_UFP16 b) { return a > b.a; };
        bool operator<=(const Saturated_UFP16 b) { return a <= b.a; };
        bool operator>=(const Saturated_UFP16 b) { return a >= b.a; };

        friend double operator*(double value, Saturated_UFP16 other) { return value*(double)other; }
        friend float operator*(float value, Saturated_UFP16 other) { return value*(float)other; }
        friend double operator/(double value, Saturated_UFP16 other) { return value/(double)other; }
        friend float operator/(float value, Saturated_UFP16 other) { return value/(float)other; }
        friend double operator+(double value, Saturated_UFP16 other) { return value+(double)other; }
        friend float operator+(float value, Saturated_UFP16 other) { return value+(float)other; }
        friend double operator-(double value, Saturated_UFP16 other) { return value-(double)other; }
        friend float operator-(float value, Saturated_UFP16 other) { return value-(float)other; }

        Saturated_UFP16 operator+(const float b)
        {
            Saturated_UFP16 res;
            res.a = saturatedAdd(a, (uint16_t)(b*0xFFFF));
            return res;
        }

        Saturated_UFP16 operator+(const double b)
        {
            Saturated_UFP16 res;
            res.a = saturatedAdd(a, (uint16_t)(b*0xFFFF));
            return res;
        }
        
        Saturated_UFP16 operator+(const Saturated_UFP16 b)
        {
            Saturated_UFP16 res;
            res.a = saturatedAdd(a, b.a);
            return res;
        }

        Saturated_UFP16 operator-(const float b)
        {
            Saturated_UFP16 res;
            res.a = saturatedSubtraction(a, (uint16_t)(b*0xFFFF));
            return res;
        }

        Saturated_UFP16 operator-(const double b)
        {
            Saturated_UFP16 res;
            res.a = saturatedSubtraction(a, (uint16_t)(b*0xFFFF));
            return res;
        }

        Saturated_UFP16 operator-(const Saturated_UFP16 b)
        {
            Saturated_UFP16 res;
            res.a = saturatedSubtraction(a, b.a);
            return res;
        }

        Saturated_UFP16 operator*(const float b)
        {
            return ((float)*this) * b;
        }

        Saturated_UFP16 operator*(const double b)
        {
            return ((double)*this) * b;
        }

        Saturated_UFP16 operator*(const Saturated_UFP16 b)
        {
            return ((float)*this) * ((float)b);
        }

        Saturated_UFP16 operator/(const float b)
        {
            return ((float)*this) / b;
        }

        Saturated_UFP16 operator/(const double b)
        {
            return ((double)*this) / b;
        }

        Saturated_UFP16 operator/(const Saturated_UFP16 b)
        {
            return ((float)*this) / ((float)b);
        }

        operator float() const
        {
            return ((float)a  / 0xFFFF);
        }
        operator double() const
        {
            return ((double)a / 0xFFFF);
        }
        
    private:
        //same as MathExt::saturatedSubtraction but no dependency
        static uint16_t saturatedAdd(uint16_t v1, uint16_t v2)
        {
            //https://locklessinc.com/articles/sat_arithmetic/
            uint16_t c = v1+v2;
            c |= -(c < v1);
            return c;
        }
        static uint16_t saturatedSubtraction(uint16_t v1, uint16_t v2)
        {
            //https://locklessinc.com/articles/sat_arithmetic/
            uint16_t c = v1-v2;
            c &= -(c <= v1);
            return c;
        }
    };
}