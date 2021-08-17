#pragma once
#include "BinaryTree.h"

namespace glib
{

    template<typename T>
    struct KDTreeNode
    {
        T* data;
        int splitDimension = -1;
    };

    template<typename T>
    class KDTree
    {
    public:
        KDTree(int dimensions);
        ~KDTree();

        void add(T* data, int split = -1);
        bool addUnique(T* data, int split = -1);
        KDTreeNode<T> search(T* data);
        KDTreeNode<T> searchNearest(T* data);
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
        this->dimensions = dimensions;
    }

    template<typename T>
    inline KDTree<T>::~KDTree()
    {
        cleanUp(binTree.getRoot());
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
    inline void KDTree<T>::add(T* data, int split)
    {

        KDTreeNode<T> kdnode = {data, split%dimensions };

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

                if(oKDNode.splitDimension >=0 )
                {
                    indexToTest = oKDNode.splitDimension;
                }

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
    inline bool KDTree<T>::addUnique(T* data, int split)
    {
        KDTreeNode<T> kdnode = {data, split%dimensions};

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
                
                if(oKDNode.splitDimension >=0 )
                {
                    indexToTest = oKDNode.splitDimension;
                }

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
            
            if(oKDNode.splitDimension >=0 )
            {
                indexToTest = oKDNode.splitDimension;
            }
            
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
        int depth = 0;
        int indexToTest = 0;
        BinaryTreeNode< KDTreeNode<T> >* currentTreeNode = binTree.getRoot();

        if(currentTreeNode==nullptr)
        {
            return {nullptr};
        }

        int minDistance = 1 << 30;
        KDTreeNode<T> returnNode = currentTreeNode->data;
        
        if(returnNode.splitDimension >=0 )
        {
            indexToTest = returnNode.splitDimension;
        }

        BinaryTreeNode<KDTreeNode<T>>* finalNode = searchRecursive(currentTreeNode, data, &minDistance, currentTreeNode, indexToTest, false);

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
                    KDTreeNode<T> childNode = tNode->leftChild->data;
                    if(childNode.splitDimension>=0)
                        nAxis = childNode.splitDimension;
                    
                    myVal = searchRecursive(tNode->leftChild, data, minDistance, tNode->leftChild, nAxis, false);
                }
            }
            else
            {
                rightSide = true;
                //go right
                if(tNode->rightChild != nullptr)
                {
                    KDTreeNode<T> childNode = tNode->rightChild->data;
                    if(childNode.splitDimension>=0)
                        nAxis = childNode.splitDimension;
                    
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
            *minDistance = currDis;
        }
        
        double disToAxis = MathExt::sqr( (double)data[axis] - (double)oKDNode.data[axis] );

        int tempAxis = (axis+1) % dimensions;
        
        if(*minDistance > disToAxis)
        {
            //potentially on both sides

            //left side
            if(tNode->leftChild != nullptr && !leftSide)
            {
                KDTreeNode<T> childNode = tNode->leftChild->data;
                if(childNode.splitDimension>=0)
                    tempAxis = childNode.splitDimension;
                    
                myVal = searchRecursive(tNode->leftChild, data, minDistance, myVal, tempAxis, true);
            }
            
            tempAxis = (axis+1) % dimensions;
            //right side
            if(tNode->rightChild != nullptr && !rightSide)
            {
                KDTreeNode<T> childNode = tNode->rightChild->data;
                if(childNode.splitDimension>=0)
                    tempAxis = childNode.splitDimension;
                
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
                    KDTreeNode<T> childNode = tNode->leftChild->data;
                    if(childNode.splitDimension>=0)
                        tempAxis = childNode.splitDimension;
                
                    myVal = searchRecursive(tNode->leftChild, data, minDistance, myVal, tempAxis, true);
                }
            }
            else
            {
                //go right
                if(tNode->rightChild != nullptr && !rightSide)
                {
                    KDTreeNode<T> childNode = tNode->rightChild->data;
                    if(childNode.splitDimension>=0)
                        tempAxis = childNode.splitDimension;
                
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
            int start;
            int end;
        };

        std::vector< KDTreeNode<T> > tempPalette = binTree.getAllElements();
        std::vector< KDTreeNode<T> > addInOrder = std::vector< KDTreeNode<T> >();
        std::vector<Range> ranges = std::vector<Range>();
        std::vector<Range> ranges2 = std::vector<Range>();

        int sortBy = 0;
        int size = tempPalette.size();

        ranges.push_back({0, size});
        
        while(addInOrder.size() < size)
        {
            for(Range r : ranges)
            {
                int rSize = (r.end - r.start);

                if(rSize > 1)
                {
                    //sort by dimension with the most variance
                    double* means = new double[dimensions];
                    double* variance = new double[dimensions];
                    double mult = 1.0/rSize;
                    
                    //find mean
                    for(int i=r.start; i<r.end; i++)
                    {
                        for(int d=0; d<dimensions; d++)
                        {
                            means[d] += ((KDTreeNode<T>)tempPalette[i]).data[d] * mult;
                        }
                    }

                    //find variance
                    for(int i=r.start; i<r.end; i++)
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

                    int midIndex = (r.start + r.end)/2;

                    KDTreeNode<T> kdt = (KDTreeNode<T>)tempPalette[midIndex];
                    kdt.splitDimension = sortBy;

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
        for(int i=0; i<addInOrder.size(); i++)
        {
            KDTreeNode<T> k = addInOrder[i];
            add( k.data, k.splitDimension );
        }
    }

} //NAMESPACE glib END