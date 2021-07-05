#pragma once
#include "StringTools.h"
#include <vector>
#include "BinarySet.h"

template<typename T>
struct BinaryTreeNode
{
	BinaryTreeNode<T>* leftChild = nullptr;
	BinaryTreeNode<T>* rightChild = nullptr;
	T data;
};

template<typename T>
struct TraverseInfo
{
	BinarySet path = BinarySet();
	BinaryTreeNode<T>* node = nullptr;
};

template<typename T>
class BinaryTree
{
public:
	BinaryTree();
	~BinaryTree();

	void setRootNode(BinaryTreeNode<T>* node);
	void setLeftNode(BinaryTreeNode<T>* parent, BinaryTreeNode<T>* leftChild);
	void setRightNode(BinaryTreeNode<T>* parent, BinaryTreeNode<T>* rightChild);

	BinaryTreeNode<T>* traverse(BinarySet bin);

	std::vector<TraverseInfo<T>> getTraversalInformation(bool leafOnly = false);

	BinaryTreeNode<T>* getRoot();

	int getSize();

	std::vector<T> getAllElements();

private:
	BinaryTreeNode<T>* rootNode = nullptr;
	void cleanUp(BinaryTreeNode<T>* node);
	int traverseCount(BinaryTreeNode<T>* node);
	void getElementsRecursive(std::vector<T>* data, BinaryTreeNode<T>* startNode);

	void getTraversalInfoRecursive(std::vector<TraverseInfo<T>>* data, BinaryTreeNode<T>* startNode, BinarySet binPath, bool leafOnly = false);

};

template<typename T>
inline BinaryTree<T>::BinaryTree()
{

}

template<typename T>
inline BinaryTree<T>::~BinaryTree()
{
	cleanUp(rootNode);
	rootNode = nullptr;
}


template<typename T>
inline BinaryTreeNode<T>* BinaryTree<T>::getRoot()
{
	return rootNode;
}

template<typename T>
inline void BinaryTree<T>::setLeftNode(BinaryTreeNode<T>* parent, BinaryTreeNode<T>* leftChild)
{
	if (parent != nullptr)
	{
		if (parent->leftChild != leftChild)
		{
			//cleanup first
			cleanUp(parent->leftChild);
		}

		parent->leftChild = leftChild;
	}
}

template<typename T>
inline void BinaryTree<T>::setRightNode(BinaryTreeNode<T>* parent, BinaryTreeNode<T>* rightChild)
{
	if (parent != nullptr)
	{
		if (parent->rightChild != rightChild)
		{
			//cleanup first
			cleanUp(parent->rightChild);
		}

		parent->rightChild = rightChild;
	}
}

template<typename T>
inline void BinaryTree<T>::setRootNode(BinaryTreeNode<T>* parent)
{
	if (rootNode != parent)
		cleanUp(rootNode);

	rootNode = parent;
}

template<typename T>
inline BinaryTreeNode<T>* BinaryTree<T>::traverse(BinarySet bin)
{
	BinaryTreeNode<T>* currNode = rootNode;
	
	int i=0;
	while(currNode!=nullptr && i < bin.size())
	{
		bool side = bin.getBit(i)==1;
		
		if(side == true)
		{
			//right side
			currNode = currNode->rightChild;
		}
		else
		{
			//left side
			currNode = currNode->leftChild;
		}
		i++;
	}
	
	return currNode;
}

template<typename T>
inline std::vector<TraverseInfo<T>> BinaryTree<T>::getTraversalInformation(bool leafOnly)
{
	std::vector<TraverseInfo<T>> arr = std::vector<TraverseInfo<T>>();

	getTraversalInfoRecursive(&arr, rootNode, BinarySet(), leafOnly);
	return arr;
}

template<typename T>
inline int BinaryTree<T>::getSize()
{
	return traverseCount(rootNode);
}

template<typename T>
inline void BinaryTree<T>::cleanUp(BinaryTreeNode<T>* currentNode)
{
	if (currentNode != nullptr)
	{
		if (currentNode->leftChild != nullptr)
		{
			//clearLeft side first

			cleanUp(currentNode->leftChild);

			//delete currentNode->leftChild;
		}

		if (currentNode->rightChild != nullptr)
		{
			//clearRight side first

			cleanUp(currentNode->rightChild);

			//delete currentNode->rightChild;
		}

		delete currentNode;
	}
}

template<typename T>
inline int BinaryTree<T>::traverseCount(BinaryTreeNode<T>* node)
{
	if (node != nullptr)
	{
		int v = 1;//count itself

		if (node->leftChild != nullptr)
		{
			v += traverseCount(node->leftChild);
		}

		if (node->rightChild != nullptr)
		{
			v += traverseCount(node->rightChild);
		}

		return v;
	}
	return 0;
}

template<typename T>
inline std::vector<T> BinaryTree<T>::getAllElements()
{
	std::vector<T> arr = std::vector<T>();

	getElementsRecursive(&arr, rootNode);

	return arr;
}

template<typename T>
inline void BinaryTree<T>::getElementsRecursive(std::vector<T>* arr, BinaryTreeNode<T>* node)
{
	if(node!=nullptr)
	{
		arr->push_back(node->data);
		getElementsRecursive(arr, node->leftChild);
		getElementsRecursive(arr, node->rightChild);
	}
}

template<typename T>
inline void BinaryTree<T>::getTraversalInfoRecursive(std::vector<TraverseInfo<T>>* data, BinaryTreeNode<T>* node, BinarySet binPath, bool leafOnly)
{
	if(node!=nullptr)
	{
		if(leafOnly)
		{
			if(node->leftChild == nullptr && node->rightChild == nullptr)
			{
				data->push_back( {binPath, node} );
			}
		}
		else
		{
			data->push_back( {binPath, node} );
		}

		BinarySet m1 = binPath;
		BinarySet m2 = binPath;

		m1.add(false);
		m2.add(true);
		
		getTraversalInfoRecursive(data, node->leftChild, m1, leafOnly);
		getTraversalInfoRecursive(data, node->rightChild, m2, leafOnly);
	}
}
