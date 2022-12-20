#pragma once
#include "BinaryTree.h"
#include "MathExt.h"
#include "Sort.h"
#include <cstring>

namespace glib
{

    template<typename T>
    struct KDTreeNode
    {
        T* data;
    };

    template<typename T>
    class KDTree
    {
    public:
        /**
         * @brief Construct a new KDTree object
         *      A K Dimensional tree stores data in a BinaryTree.
         *      The items are placed based on the dimension value.
         *      The class expectes the data to be an array of values of atleast the size of the dimension.
         * 
         * @param dimensions 
         *      The dimension of the KDTree.
         */
        KDTree(int dimensions);

        /**
         * @brief Destroy the KDTree object
         * 
         */
        ~KDTree();

        /**
         * @brief Adds data to the KDTree.
         * 
         * @param data 
         *      The data to add.
         *      It is expected to be an array of at least the dimension size.
         */
        void add(T* data);

        /**
         * @brief Adds data to the KDTree if it does not already exist.
         * 
         * @param data 
         *      The data to add.
         *      It is expected to be an array of at least the dimension size.
         * @return bool
         *      Returns true if successful.
         */
        bool addUnique(T* data);

        /**
         * @brief Searches for the data in the tree.
         * 
         * @param data 
         *      The data to search for.
         *      It is expected to be an array of at least the dimension size.
         * @return KDTreeNode<T> 
         */
        KDTreeNode<T> search(T* data);

        /**
         * @brief Searches for the data in the tree.
         *      If not found, finds the closest thing to it.
         *      Average run time is O(LogN)
         * 
         *      Note that the current implementation is not as good for achieving the average run time of O(LogN)
         * 
         * @param data 
         *      The data to search for.
         *      It is expected to be an array of at least the dimension size.
         * @return KDTreeNode<T> 
         */
        KDTreeNode<T> searchNearest(T* data);

        /**
         * @brief Balances the KDTree to better achieve the average run time of O(LogN)
         * 
         */
        void balance();
    private:
        BinaryTreeNode<KDTreeNode<T>>* searchRecursive(BinaryTreeNode<KDTreeNode<T>>* tNode, T* data, int* minDistance, BinaryTreeNode<KDTreeNode<T>>* returnVal, int axis, bool backwards);
        void cleanUp(BinaryTreeNode<KDTreeNode<T>>* node);

        int dimensions = 0;
        BinaryTree< KDTreeNode<T> > binTree;
    };

    template<typename T>
    inline KDTree<T>::KDTree(int dimensions)
    {
        //max function

        this->dimensions = (dimensions < 0) ? 0 : dimensions;
    }

    template<typename T>
    inline KDTree<T>::~KDTree()
    {
        cleanUp(binTree.getRoot());
        binTree.~BinaryTree();
    }

    template<typename T>
    inline void KDTree<T>::cleanUp(BinaryTreeNode<KDTreeNode<T>>* node)
    {
        if(node!=nullptr)
        {
            KDTreeNode<T> n = (KDTreeNode<T>)node->data;
            delete[] n.data;

            if(node->leftChild != nullptr)
                cleanUp(node->leftChild);
            
            if(node->rightChild != nullptr)
                cleanUp(node->rightChild);
        }
    }

    template<typename T>
    inline void KDTree<T>::add(T* data)
    {

        KDTreeNode<T> kdnode = {data};

        BinaryTreeNode< KDTreeNode<T> >* node = new BinaryTreeNode< KDTreeNode<T> >();
        node->data = kdnode;

        if(binTree.getRoot() == nullptr)
        {
            binTree.setRootNode(node);
        }
        else
        {
            int depth = 0;
            int indexToTest = 0;
            BinaryTreeNode< KDTreeNode<T> >* currentTreeNode = binTree.getRoot();
            
            while(currentTreeNode!=nullptr)
            {
                KDTreeNode<T> oKDNode = currentTreeNode->data;
                indexToTest = depth % dimensions;

                if(kdnode.data[indexToTest] < oKDNode.data[indexToTest])
                {
                    //go left
                    if(currentTreeNode->leftChild == nullptr)
                    {
                        binTree.setLeftNode(currentTreeNode, node);
                        break;
                    }
                    else
                    {
                        currentTreeNode = currentTreeNode->leftChild;
                    }
                }
                else
                {
                    //go right
                    if(currentTreeNode->rightChild == nullptr)
                    {
                        binTree.setRightNode(currentTreeNode, node);
                        break;
                    }
                    else
                    {
                        currentTreeNode = currentTreeNode->rightChild;
                    }
                }

                depth++;
            }
        }
    }

    template<typename T>
    inline bool KDTree<T>::addUnique(T* data)
    {
        KDTreeNode<T> kdnode = {data};

        BinaryTreeNode< KDTreeNode<T> >* node = new BinaryTreeNode< KDTreeNode<T> >();
        node->data = kdnode;

        if(binTree.getRoot() == nullptr)
        {
            binTree.setRootNode(node);
            return true;
        }
        else
        {
            int depth = 0;
            int indexToTest = 0;
            BinaryTreeNode< KDTreeNode<T> >* currentTreeNode = binTree.getRoot();
            
            while(currentTreeNode!=nullptr)
            {
                KDTreeNode<T> oKDNode = currentTreeNode->data;
                indexToTest = depth % dimensions;

                bool same = std::memcmp(data, oKDNode.data, dimensions*sizeof(T)) == 0;

                if(same)
                {
                    return false;
                }

                if(kdnode.data[indexToTest] < oKDNode.data[indexToTest])
                {
                    //go left
                    if(currentTreeNode->leftChild == nullptr)
                    {
                        binTree.setLeftNode(currentTreeNode, node);
                        return true;
                    }
                    else
                    {
                        currentTreeNode = currentTreeNode->leftChild;
                    }
                }
                else
                {
                    //go right
                    if(currentTreeNode->rightChild == nullptr)
                    {
                        binTree.setRightNode(currentTreeNode, node);
                        return true;
                    }
                    else
                    {
                        currentTreeNode = currentTreeNode->rightChild;
                    }
                }

                depth++;
            }
        }

        return false;
    }

    template<typename T>
    inline KDTreeNode<T> KDTree<T>::search(T* data)
    {
        int depth = 0;
        int indexToTest = 0;
        BinaryTreeNode< KDTreeNode<T> >* currentTreeNode = binTree.getRoot();
        
        while(currentTreeNode!=nullptr)
        {
            KDTreeNode<T> oKDNode = currentTreeNode->data;
            indexToTest = depth % dimensions;
            
            bool same = std::memcmp(data, oKDNode.data, dimensions*sizeof(T)) == 0;

            if(same)
            {
                return oKDNode;
            }

            if(data[indexToTest] < oKDNode.data[indexToTest])
            {
                //go left
                currentTreeNode = currentTreeNode->leftChild;
            }
            else
            {
                //go right
                currentTreeNode = currentTreeNode->rightChild;
            }

            depth++;
        }

        return {nullptr};
    }

    template<typename T>
    inline KDTreeNode<T> KDTree<T>::searchNearest(T* data)
    {
        // int depth = 0; //Not used
        int indexToTest = 0;
        BinaryTreeNode< KDTreeNode<T> >* currentTreeNode = binTree.getRoot();

        if(currentTreeNode==nullptr)
        {
            return {nullptr};
        }

        int minDistance = 1 << 30;
        // KDTreeNode<T> returnNode = currentTreeNode->data; //Not used

        BinaryTreeNode<KDTreeNode<T>>* finalNode = this->searchRecursive(currentTreeNode, data, &minDistance, currentTreeNode, indexToTest, false);

        return {finalNode->data};
    }

    template<typename T>
    inline BinaryTreeNode<KDTreeNode<T>>* KDTree<T>::searchRecursive(BinaryTreeNode<KDTreeNode<T>>* tNode, T* data, int* minDistance, BinaryTreeNode<KDTreeNode<T>>* returnVal, int axis, bool backwards)
    {
        BinaryTreeNode<KDTreeNode<T>>* myVal = returnVal;
        KDTreeNode<T> oKDNode = tNode->data;

        bool leftSide = false;
        bool rightSide = false;

        if(backwards == false)
        {
            int nAxis = (axis+1) % dimensions;

            if(data[axis] < oKDNode.data[axis])
            {
                leftSide = true;
                //go left
                if(tNode->leftChild != nullptr)
                {
                    // KDTreeNode<T> childNode = tNode->leftChild->data;
                    
                    myVal = searchRecursive(tNode->leftChild, data, minDistance, tNode->leftChild, nAxis, false);
                }
            }
            else
            {
                rightSide = true;
                //go right
                if(tNode->rightChild != nullptr)
                {
                    // KDTreeNode<T> childNode = tNode->rightChild->data;
                    
                    myVal = searchRecursive(tNode->rightChild, data, minDistance, tNode->rightChild, nAxis, false);
                }
            }
        }

        //now we go backwards
        //update min dis

        double currDis = 0;
        for(int i=0; i<this->dimensions; i++)
        {
            currDis += MathExt::sqr((double)data[i] - (double)oKDNode.data[i]);
        }

        if(currDis < *minDistance)
        {
            myVal = tNode;
            *minDistance = (int)currDis;
        }
        
        double disToAxis = MathExt::sqr( (double)data[axis] - (double)oKDNode.data[axis] );

        int tempAxis = (axis+1) % dimensions;
        
        if(*minDistance > disToAxis)
        {
            //potentially on both sides

            //left side
            if(tNode->leftChild != nullptr && !leftSide)
            {
                // KDTreeNode<T> childNode = tNode->leftChild->data;
                    
                myVal = searchRecursive(tNode->leftChild, data, minDistance, myVal, tempAxis, true);
            }
            
            tempAxis = (axis+1) % dimensions;
            //right side
            if(tNode->rightChild != nullptr && !rightSide)
            {
                // KDTreeNode<T> childNode = tNode->rightChild->data;
                
                myVal = searchRecursive(tNode->rightChild, data, minDistance, myVal, tempAxis, true);
            }
        }
        else
        {
            //potentially only on one side
            //traverse down like normal I think
                
            if(data[axis] < oKDNode.data[axis])
            {
                //go left
                if(tNode->leftChild != nullptr && !leftSide)
                {
                    // KDTreeNode<T> childNode = tNode->leftChild->data;
                
                    myVal = searchRecursive(tNode->leftChild, data, minDistance, myVal, tempAxis, true);
                }
            }
            else
            {
                //go right
                if(tNode->rightChild != nullptr && !rightSide)
                {
                    // KDTreeNode<T> childNode = tNode->rightChild->data;
                
                    myVal = searchRecursive(tNode->rightChild, data, minDistance, myVal, tempAxis, true);
                }
            }
        }
        
        return myVal;
    }

    template<class T>
    void KDTree<T>::balance()
    {
        //sort elements by dimension with the most variance
        //dividing the list into halfs selecting the median of each list to add to the list next.
        //takes O(n*log(n)*log(n)) where n is the amount of values

        struct Range
        {
            size_t start;
            size_t end;
        };

        std::vector< KDTreeNode<T> > tempPalette = binTree.getAllElements();
        std::vector< KDTreeNode<T> > addInOrder = std::vector< KDTreeNode<T> >();
        std::vector<Range> ranges = std::vector<Range>();
        std::vector<Range> ranges2 = std::vector<Range>();

        int sortBy = 0;
        size_t size = tempPalette.size();

        ranges.push_back({0, size});
        
        while(addInOrder.size() < size)
        {
            for(Range r : ranges)
            {
                size_t rSize = (r.end - r.start);

                if(rSize > 1)
                {
                    //sort by dimension with the most variance
                    double* means = new double[dimensions];
                    double* variance = new double[dimensions];
                    double mult = 1.0/rSize;
                    
                    //find mean
                    for(size_t i=r.start; i<r.end; i++)
                    {
                        for(int d=0; d<dimensions; d++)
                        {
                            means[d] += ((KDTreeNode<T>)tempPalette[i]).data[d] * mult;
                        }
                    }

                    //find variance
                    for(size_t i=r.start; i<r.end; i++)
                    {
                        for(int d=0; d<dimensions; d++)
                        {
                            double m = (means[d] - ((KDTreeNode<T>)tempPalette[i]).data[d]);
                            variance[d] += m*m;
                        }
                    }

                    //sortby = index of max variance
                    sortBy = 0;
                    double preMax = variance[0];
                    for(int d=0; d<dimensions; d++)
                    {
                        if(variance[d] > preMax)
                        {
                            preMax = variance[d];
                            sortBy = d;
                        }
                    }

                    Sort::mergeSort<KDTreeNode<T>>(tempPalette.data()+r.start, rSize, [sortBy](KDTreeNode<T> a, KDTreeNode<T> b) -> bool{
                        return a.data[sortBy] < b.data[sortBy];
                    });

                    size_t midIndex = (r.start + r.end)/2;

                    KDTreeNode<T> kdt = (KDTreeNode<T>)tempPalette[midIndex];

                    addInOrder.push_back(kdt);

                    ranges2.push_back( {r.start, midIndex-1} );
                    ranges2.push_back( {midIndex+1, r.end} );
                }
                else if (rSize==0)
                {
                    addInOrder.push_back(tempPalette[r.start]);
                }
                else if (rSize==1)
                {
                    addInOrder.push_back(tempPalette[r.start]);
                    if(r.end != size)
                    {
                        addInOrder.push_back(tempPalette[r.end]);
                    }
                }
            }

            ranges = ranges2;
            ranges2.clear();
        }

        //after everything above done
        binTree = BinaryTree< KDTreeNode<T> >();
        for(size_t i=0; i<addInOrder.size(); i++)
        {
            KDTreeNode<T> k = addInOrder[i];
            add( k.data );
        }
    }

} //NAMESPACE glib END