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
        static std::vector<Grouping> packageMergeAlgorithm(std::vector<Grouping> baseItems, size_t desiredTotal);

    private:

    };
    
} //NAMESPACE glib END