#pragma once
#include <iostream>

namespace smpl
{

    class PCG
    {
    public:
        /**
         * @brief Construct a new PCG Random Number Generator
         *      Produces much more secure random numbers with speed on par with LCG.
         *      Check here for more information https://www.pcg-random.org/
         * @param seed 
         *      Default is 0x4d595df4d0f33173
         * @param mulitplier 
         *      Default is 6364136223846793005u
         * @param increment 
         *      Default is 1442695040888963407u
         */
        PCG(uint64_t seed = 0x4d595df4d0f33173, uint64_t mulitplier = 6364136223846793005u, uint64_t increment = 1442695040888963407u)
        {
            this->seed = seed;
            this->multiplier = mulitplier;
            this->increment = increment;

            seed+=increment;
            get();
        }
        
        /**
         * @brief Destroy the PCG object
         * 
         */
        ~PCG()
        {

        }

        /**
         * @brief Gets the next random number. Truncated to an unsigned int however,
         *      the calculations are done using a uint64_t.
         * 
         * @return unsigned int 
         */
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
            return x >> r | x << (negateNumber(r) & 31);
            //return x >> r | x << (-r & 31);
        }

        unsigned int negateNumber(unsigned int x)
        {
            return 1U + ~x;
        }

        uint64_t seed = 0;
        uint64_t multiplier = 0;
        uint64_t increment = 0;
    };

} //NAMESPACE smpl END