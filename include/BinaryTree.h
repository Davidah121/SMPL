#pragma once

template<typename T>
struct BinaryTreeNode
{
	BinaryTreeNode<T>* leftChild = nullptr;
	BinaryTreeNode<T>* rightChild = nullptr;
	T data;
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

	BinaryTreeNode<T>* traverse(BinaryTreeNode<T>* startNode, int value, int length, bool reverse=false);

	BinaryTreeNode<T>* getRoot();

	int getSize();

private:
	BinaryTreeNode<T>* rootNode = nullptr;
	void cleanUp(BinaryTreeNode<T>* node);
	int traverseCount(BinaryTreeNode<T>* node);
};

template<typename T>
inline BinaryTree<T>::BinaryTree()
{

}

template<typename T>
inline BinaryTree<T>::~BinaryTree()
{
	cleanUp(rootNode);
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
inline BinaryTreeNode<T>* BinaryTree<T>::traverse(BinaryTreeNode<T>* startNode, int value, int length, bool reverse)
{
	BinaryTreeNode<T>* currNode = startNode;
	int currLength = 0;

	int maxLength = length;
	if(maxLength<0)
	{
		maxLength = 0;
	}
	if(maxLength>32)
	{
		maxLength = 32;
	}
	
	while(currNode!=nullptr && currLength<maxLength)
	{
		bool side=false;

		if(reverse)
			side = ((value >> (currLength)) & 0x01);
		else
			side = ((value >> (31-currLength)) & 0x01);
		
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
		currLength++;
	}
	
	return currNode;
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
			v += traverse(node->leftChild);
		}

		if (node->rightChild != nullptr)
		{
			v += traverse(node->rightChild);
		}

		return v;
	}
	return 0;
}
