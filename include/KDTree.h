#include "BinaryTree.h"

template<typename T>
struct KDTreeNode
{
    T* data;
};

template<typename T>
class KDTree
{
public:
    KDTree(int dimensions);
    ~KDTree();

    void add(T* data);
    bool addUnique(T* data);
    KDTreeNode<T> search(T* data);
    KDTreeNode<T> searchNearest(T* data);
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
    int depth = 0;
    int indexToTest = 0;
    BinaryTreeNode< KDTreeNode<T> >* currentTreeNode = binTree.getRoot();

    if(currentTreeNode==nullptr)
    {
        return {nullptr};
    }

    int minDistance = 1 << 30;
    KDTreeNode<T> returnNode = currentTreeNode->data;

    struct TreeThing
    {
        BinaryTreeNode< KDTreeNode<T> >* node;
        int indexToTest;
    };
    
    //BinaryTreeNode<KDTreeNode<T>>* finalNode = recursiveFunc(currentTreeNode, &minDistance, currentTreeNode, 0);
    BinaryTreeNode<KDTreeNode<T>>* finalNode = searchRecursive(currentTreeNode, data, &minDistance, currentTreeNode, 0, false);

    /*

    std::vector< TreeThing > processThese = std::vector< TreeThing >();

    while(currentTreeNode!=nullptr)
    {
        KDTreeNode<T> oKDNode = currentTreeNode->data;
        indexToTest = depth % dimensions;
        
        processThese.push_back( {currentTreeNode, indexToTest} );

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

    while(true)
    {
        TreeThing cNode = processThese.back();
        processThese.pop_back();

        indexToTest = cNode.indexToTest;
        KDTreeNode<T> oKDNode = cNode.node->data;

        //determine distance
        unsigned int currDis = 0;
        for(int i=0; i<dimensions; i++)
        {
            currDis += MathExt::sqr(data[i] - oKDNode[i]);
        }

        if(currDis < minDistance)
        {
            minDistance = currDis;
            returnNode = oKDNode;
        }

        //check distance to split axis for the child if they exist
        if(cNode.node->leftChild != nullptr)
        {
            KDTreeNode<T> leftSide = cNode.node->leftChild->data;
            int leftIndex = indexToTest - 1;
            if(leftIndex < 0)
            {
                leftIndex = dimensions-1;
            }
        }
        if(cNode.node->rightChild != nullptr)
        {
            KDTreeNode<T> rightSide = cNode.node->rightChild->data;
            int rightIndex = indexToTest - 1;
            if(rightIndex < 0)
            {
                rightIndex = dimensions-1;
            }
        }
    }
    */

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
        if(data[axis] < oKDNode.data[axis])
        {
            leftSide = true;
            //go left
            if(tNode->leftChild != nullptr)
                myVal = searchRecursive(tNode->leftChild, data, minDistance, tNode->leftChild, (axis+1) % dimensions, false);
        }
        else
        {
            rightSide = true;
            //go right
            if(tNode->rightChild != nullptr)
                myVal = searchRecursive(tNode->rightChild, data, minDistance, tNode->rightChild, (axis+1) % dimensions, false);
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
            myVal = searchRecursive(tNode->leftChild, data, minDistance, myVal, tempAxis, true);
        }

        //right side
        if(tNode->rightChild != nullptr && !rightSide)
        {
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
                myVal = searchRecursive(tNode->leftChild, data, minDistance, myVal, tempAxis, true);
            }
        }
        else
        {
            //go right
            if(tNode->rightChild != nullptr && !rightSide)
            {
                myVal = searchRecursive(tNode->rightChild, data, minDistance, myVal, tempAxis, true);
            }
        }
    }
    
    return myVal;
}