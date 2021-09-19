#pragma once

namespace glib
{
    
    class LCG
    {
    public:
        /**
         * @brief Construct a new LCG (Linear Congruential Generator) for random numbers.
         *      Generates psudo random numbers using the seed, multiplier, increment, and modulus value.
         *      Fast but not secure due to repeated values. The functions period is based on the parameters.
         * 
         * @param seed 
         *      The starting seed.
         *      Default is 65535
         * @param multiplier 
         *      The multiplier
         *      Default is 48271
         * @param increment 
         *      The increment
         *      Default is 0
         * @param mod 
         *      The modulus value to use.
         *      Default is the maximum of a unsigned integer.
         */
        LCG(unsigned int seed = 0xFFFF, unsigned int multiplier = 48271, unsigned int increment = 0, unsigned int mod = 0xFFFFFFFF)
        {
            this->currentVal = seed;
            this->multiplier = multiplier;
            this->increment = increment;
            this->mod = mod;
        }

        /**
         * @brief Destroy the LCG object
         * 
         */
        ~LCG()
        {
            
        }

        /**
         * @brief Returns the next random number.
         * 
         * @return unsigned int 
         */
        unsigned int get()
        {
            currentVal = ( (currentVal*multiplier) + increment ) % mod;
            return currentVal;
        }

        /**
         * @brief Sets the Seed to a different seed.
         * 
         * @param seed 
         */
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

} //NAMESPACE glib END