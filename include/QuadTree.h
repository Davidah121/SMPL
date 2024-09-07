#pragma once
//TODO: Implement
#include "Tree.h"
#include "Shape.h"

namespace smpl
{
    struct QuadTreeRect
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

    template<typename T>
    struct QuadTreeNode
    {
        QuadTreeRect rect;
        int depth;
        std::vector<std::pair<QuadTreeRect, T>> dataList;
    };

    template<typename T>
    class QuadTree
    {
    public:
        QuadTree(int x1, int y1, int x2, int y2, int maxLoad = 16, int maxDepth = 8);
        ~QuadTree();

        void insert(int x1, int y1, int x2, int y2, T data);
        void remove(int x1, int y1, int x2, int y2, T data);
        std::vector<QuadTreeNode<T>*> getCollidedNodes(int x1, int y1, int x2, int y2);

        Tree<QuadTreeNode<T>>* getRawTree();
    private:
        bool isColliding(int p1x1, int p1y1, int p1x2, int p1y2, int p2x1, int p2y1, int p2x2, int p2y2);
        void searchRecursive(int x1, int y1, int x2, int y2, TreeNode<QuadTreeNode<T>>* currNode, std::vector<QuadTreeNode<T>*>* results);
        void insertRecursive(int x1, int y1, int x2, int y2, T data, TreeNode<QuadTreeNode<T>>* currNode);
        void removeRecursive(int x1, int y1, int x2, int y2, T data, TreeNode<QuadTreeNode<T>>* currNode);
        void splitNode(TreeNode<QuadTreeNode<T>>* currNode);
        
        Tree<QuadTreeNode<T>> tree = Tree<QuadTreeNode<T>>();

        int maxLoad;
        int maxDepth;
    };

    template<typename T>
    QuadTree<T>::QuadTree(int x1, int y1, int x2, int y2, int maxLoad, int maxDepth)
    {
        this->maxLoad = maxLoad;
        this->maxDepth = maxDepth;

        TreeNode<QuadTreeNode<T>>* rootNode = new TreeNode<QuadTreeNode<T>>();
        rootNode->data.x1 = x1;
        rootNode->data.y1 = y1;
        rootNode->data.x2 = x2;
        rootNode->data.y2 = y2;
        rootNode->data.depth = 0;
        
        tree.setRootNode(rootNode);
    }

    template<typename T>
    QuadTree<T>::~QuadTree()
    {

    }

    template<typename T>
    void QuadTree<T>::insert(int x1, int y1, int x2, int y2, T data)
    {
        insertRecursive(x1, y1, x2, y2, data, tree.getRoot());
    }

    template<typename T>
    void QuadTree<T>::remove(int x1, int y1, int x2, int y2, T data)
    {
        removeRecursive(x1, y1, x2, y2, data, tree.getRoot());
    }

    template<typename T>
    std::vector<QuadTreeNode<T>*> QuadTree<T>::getCollidedNodes(int x1, int y1, int x2, int y2)
    {
        std::vector<QuadTreeNode<T>*> results;
        searchRecursive(x1, y1, x2, y2, tree.getRoot(), &results);
        return results;
    }

    template<typename T>
    Tree<QuadTreeNode<T>>* QuadTree<T>::getRawTree()
    {
        return &tree;
    }

    template<typename T>
    bool QuadTree<T>::isColliding(int p1x1, int p1y1, int p1x2, int p1y2, int p2x1, int p2y1, int p2x2, int p2y2)
    {
        //AABB collision
        if(p1x1 <= p2x2 && p1x2 >= p2x1 && p1y1 <= p2y2 && p1y2 >= p2y1)
            return true;
        if(p2x1 <= p1x2 && p2x2 >= p1x1 && p2y1 <= p1y2 && p2y2 >= p1y1)
            return true;
        return false;
    }

    template<typename T>
    void QuadTree<T>::searchRecursive(int x1, int y1, int x2, int y2, TreeNode<QuadTreeNode<T>>* currNode, std::vector<QuadTreeNode<T>*>* results)
    {
        if(currNode != nullptr)
        {
            if(isColliding(x1, y1, x2, y2, currNode->data.rect.x1, currNode->data.rect.y1, currNode->data.rect.x2, currNode->data.rect.y2))
            {
                if(currNode->children.size() != 0)
                {
                    for(int i=0; i<currNode->children.size(); i++)
                    {
                        searchRecursive(x1, y1, x2, y2, currNode->children[i], results);
                    }
                }
                else
                {
                    //leaf node
                    results->push_back(&currNode->data);
                }
            }
        }
    }

    template<typename T>
    void QuadTree<T>::insertRecursive(int x1, int y1, int x2, int y2, T data, TreeNode<QuadTreeNode<T>>* currNode)
    {
        if(currNode != nullptr)
        {
            if(currNode->children.size() == 0)
            {
                //try to insert into this node
                if(isColliding(x1, y1, x2, y2, currNode->data.rect.x1, currNode->data.rect.y1, currNode->data.rect.x2, currNode->data.rect.y2))
                {
                    currNode->data.children.push_back(data);

                    if(currNode->data.children.size() > maxLoad && currNode->data.depth < maxDepth)
                    {
                        //split into 4 children. Not necessarily equally split but should be close.
                        splitNode(currNode);
                        for(int i=0; i<currNode->data.children.size(); i++)
                        {
                            for(int j=0; j<currNode->children.size(); j++)
                            {
                                insertRecursive(x1, y1, x2, y2, currNode->data.children[i], currNode->children[j]);
                            }
                        }
                    }
                }
            }
            else
            {
                //TODO: Fix this.....
                //try to insert into it's children
                // for(int j=0; j<currNode->children.size(); j++)
                // {
                //     insertRecursive(x1, y1, x2, y2, currNode->data.children[i], currNode->children[j]);
                // }
            }
        }
    }

    template<typename T>
    void QuadTree<T>::removeRecursive(int x1, int y1, int x2, int y2, T data, TreeNode<QuadTreeNode<T>>* currNode)
    {

    }

    template<typename T>
    void QuadTree<T>::splitNode(TreeNode<QuadTreeNode<T>>* currNode)
    {
        if(currNode != nullptr && currNode->children.size() == 0)
        {
            TreeNode<QuadTreeNode<T>>* topLeftBox = new TreeNode<QuadTreeNode<T>>();
            TreeNode<QuadTreeNode<T>>* topRightBox = new TreeNode<QuadTreeNode<T>>();
            TreeNode<QuadTreeNode<T>>* bottomLeftBox = new TreeNode<QuadTreeNode<T>>();
            TreeNode<QuadTreeNode<T>>* bottomRightBox = new TreeNode<QuadTreeNode<T>>();

            int midX = (currNode->data.rect.x1 + currNode->data.rect.x2)/2;
            int midY = (currNode->data.rect.y1 + currNode->data.rect.y2)/2;
            
            topLeftBox->data.depth = currNode->data.depth+1;
            topRightBox->data.depth = currNode->data.depth+1;
            bottomLeftBox->data.depth = currNode->data.depth+1;
            bottomRightBox->data.depth = currNode->data.depth+1;

            topLeftBox->data.rect = currNode->data.rect;
            topRightBox->data.rect = currNode->data.rect;
            bottomLeftBox->data.rect = currNode->data.rect;
            bottomRightBox->data.rect = currNode->data.rect;

            topLeftBox->data.rect.x2 = midX;
            topLeftBox->data.rect.y2 = midY;

            topRightBox->data.rect.x1 = midX;
            topRightBox->data.rect.y2 = midY;

            bottomLeftBox->data.rect.x2 = midX;
            bottomLeftBox->data.rect.y1 = midY;

            bottomRightBox->data.rect.x1 = midX;
            bottomRightBox->data.rect.y1 = midY;
            
            currNode->children.push_back(topLeftBox);
            currNode->children.push_back(topRightBox);
            currNode->children.push_back(bottomLeftBox);
            currNode->children.push_back(bottomRightBox);
        }
    }
}