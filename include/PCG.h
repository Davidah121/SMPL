#pragma once
#include <iostream>

class PCG
{
public:
    PCG(uint64_t seed = 0x4d595df4d0f33173, uint64_t mulitplier = 6364136223846793005u, uint64_t increment = 1442695040888963407u)
    {
        this->seed = seed;
        this->multiplier = mulitplier;
        this->increment = increment;

        seed+=increment;
        get();
    }
    
    ~PCG()
    {

    }

    unsigned int get()
    {
        uint64_t preSeed = seed;
        unsigned int count = preSeed >> 59;
        
        seed = preSeed*multiplier + increment;
        preSeed ^= preSeed >> 18;

        return rotr32((unsigned int)(preSeed>>27), count);
    }
private:
    unsigned int rotr32(unsigned int x, unsigned int r)
    {
        return x >> r | x << (-r & 31);
    }
    uint64_t seed = 0;
    uint64_t multiplier = 0;
    uint64_t increment = 0;
};