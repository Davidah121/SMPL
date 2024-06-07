#include "Tree.h"
#include <iostream>

//TODO: Implement
/*
    Notes:
        Its a normal tree since each node can have multiple children (Will limit the number of children to 256 for space concerns)
        The children nodes have a string attached to it representing the remaining parts of the string (not just a single character so its compressed)
        3 main operations:
            Insert
            Delete
            Search
*/
namespace smpl
{
    class RadixTree
    {
    public:
        RadixTree();
        ~RadixTree();

        void insert(std::string s);
        void remove(std::string s);
        bool search(std::string s);

        Tree<std::pair<std::string, bool>>* getRawTree();

    private:
        Tree<std::pair<std::string, bool>> tree = Tree<std::pair<std::string, bool>>();
    };
}