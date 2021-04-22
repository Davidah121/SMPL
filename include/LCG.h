#pragma once

class LCG
{
public:
    LCG(unsigned int seed = 0xFFFF, unsigned int multiplier = 48271, unsigned int increment = 0, unsigned int mod = 0xFFFFFFFF)
    {
        this->currentVal = seed;
        this->multiplier = multiplier;
        this->increment = increment;
        this->mod = mod;
    }

    ~LCG()
    {
        
    }

    unsigned int get()
    {
        currentVal = ( (currentVal*multiplier) + increment ) % mod;
        return currentVal;
    }

    void setSeed(unsigned int seed)
    {
        currentVal = seed;
    }

private:
    unsigned int currentVal;
    unsigned int multiplier;
    unsigned int increment;
    unsigned int mod;
};