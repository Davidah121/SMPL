#include "MathExt.h"
#include "Sort.h"

namespace glib
{
    struct Grouping
    {
        bool valid = true;
        size_t width = 0;
        size_t weight = 0;
        std::vector<int> ids = std::vector<int>();
    };

    class Algorithms
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
        
    private:

    };

} //NAMESPACE glib END

