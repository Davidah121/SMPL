#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "MathExt.h"
#include "Sort.h"

namespace smpl
{
    struct Grouping
    {
        bool valid = true;
        size_t width = 0;
        size_t weight = 0;
        std::vector<int> ids = std::vector<int>();
    };

    class DLL_OPTION Algorithms
    {
    public:
        /**
		 * @brief The Package Merge Algorithm is a subset of the knapsack problem that can be solved in sudo linear time.
         *      This implementation is not the fastest since it requires sorting.
         *      It is primarily used to solve for length limited huffman codes.
		 * @param baseItems 
		 * 		A vector of Grouping structs. Width is the value and weight is the cost of using the item.
         *      Width must be a power of 2.
         *      valid is an internal tool and should be left as true.
         *      unique ids should be set by the user. This allows the user to get the selected items. Only one should be set per item.
         * @param desiredTotal
         *      The desired total to hit.
         * 
         * @return std::vector<Grouping>
         *      The algorithm returns a list of a single Grouping if successful.
         *      The Grouping contains the total weight and the ids of all items used.
		 */
        static std::vector<Grouping> packageMergeAlgorithm(std::vector<Grouping> baseItems, size_t desiredTotal);
        
        /**
         * @brief Attempts to find the value in the sorted array in O(LogN)
         *      using the Binary Search Algorithm
         *      Utilizes templates so operator< and operator== must be defined
         * 
         * @tparam T 
         * @param data 
         * @param size 
         * @param value 
         * @return size_t 
         */
        template<typename T>
        static size_t binarySearch(T* data, size_t size, T value);

        // /**
        //  * @brief Attempts to find a range of indicies that satisfies the desired value in O(LogN)
        //  *      utilizing the Binary Search Algorithm with extra steps
        //  *      Utilizes templates so operator< and operator== must be defined
        //  * 
        //  * @tparam T 
        //  * @param data 
        //  * @param size 
        //  * @param min 
        //  * @param max 
        //  * @return std::pair<size_t, size_t> 
        //  */
        // template<typename T>
        // static std::pair<size_t, size_t> binaryRangeSearch(T* data, size_t size, T value);
    private:

    };
    
    template<typename T>
    inline size_t Algorithms::binarySearch(T* data, size_t size, T value)
    {
        size_t startIndex = 0;
        size_t endIndex = size-1;

        if(data[startIndex] == value)
            return startIndex;
        if(data[endIndex] == value)
            return endIndex;

        while(endIndex - startIndex > 1)
        {
            size_t midPoint = (startIndex+endIndex)/2;
            if(data[midPoint] == value)
                return midPoint;
            if(data[midPoint] < value)
                endIndex = midPoint;
            else
                startIndex = midPoint;
        }

        return SIZE_MAX;
    }

} //NAMESPACE smpl END

