#pragma once
#include <vector>
#include "BinarySet.h"

namespace glib
{
	template<typename T>
	class BinarySearchTree;

	template<typename T>
	struct BinaryTreeNode
	{
		BinaryTreeNode<T>* leftChild = nullptr;
		BinaryTreeNode<T>* rightChild = nullptr;
		T data;
		BinaryTreeNode<T>* parent = nullptr; //Do this later
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
		/**
		 * @brief Creates a template BinaryTree object.
		 * 		Uses the template BinaryTreeNode structure for each node.
		 */
		BinaryTree();

		/**
		 * @brief Destroys the BinaryTree Object and cleans up all memory.
		 * 		Note that it deletes all BinaryTreeNode pointers but not the data they store.
		 * 		This should be done by the user or pointers should be avoided unless they are
		 * 		smart pointers.
		 */
		~BinaryTree();

		/**
		 * @brief Sets the root node of the tree to the specified node pointer.
		 * 		Note that it will delete the old root and its subtree.
		 * 
		 * @param node
		 * 		The node to become the new root node.
		 */
		void setRootNode(BinaryTreeNode<T>* node);

		/**
		 * @brief Sets the left node of the specified parent node.
		 * 		Note that it will delete the old child and its subtree.
		 * 
		 * 
		 * @param parent
		 * 		The node to have its left child node modified.
		 * @param leftChild
		 * 		The node to set as the left child.
		 */
		void setLeftNode(BinaryTreeNode<T>* parent, BinaryTreeNode<T>* leftChild);

		/**
		 * @brief Sets the right node of the specified parent node.
		 * 		Note that it will delete the old child and its subtree.
		 * 
		 * @param parent
		 * 		The node to have its right child node modified.
		 * @param rightChild
		 * 		The node to set as the right child.
		 */
		void setRightNode(BinaryTreeNode<T>* parent, BinaryTreeNode<T>* rightChild);

		/**
		 * @brief Traverses the tree to get a BinaryTreeNode using a BinarySet.
		 * 		Each bit is used to traverse the tree where 0 = goLeft | 1 = goRight
		 * @param bin
		 * 		The BinarySet to use for traversal. Starts at index 0.
		 * @return BinaryTreeNode<T>*
		 * 		Returns a pointer to the BinaryTreeNode found. If no node was found,
		 * 		returns a nullptr.
		 */
		BinaryTreeNode<T>* traverse(BinarySet bin);

		/**
		 * @brief Gets traversal information for the nodes in the tree.
		 * 		A BinarySet is used to keep track of the path taken to get to
		 * 		the node.
		 * @param leafOnly
		 * 		Set to only get leaf nodes.
		 * @return std::vector<TraverseInfo<T>>
		 * 		Returns a list of all traversal data. Each contains the node and the path.
		 */
		std::vector<TraverseInfo<T>> getTraversalInformation(bool leafOnly = false);

		/**
		 * @brief Returns the root of the tree.
		 * @return BinaryTreeNode<T>*
		 * 		Can be nullptr
		 */
		BinaryTreeNode<T>* getRoot();

		/**
		 * @brief Returns the amount of nodes in the tree.
		 * 		Includes all nodes. Not just leaf nodes.
		 * @return int
		 */
		int getSize();

		/**
		 * @brief Returns the height of the tree.
		 * @return int
		 */
		int getHeight();

		/**
		 * @brief Returns all of the data in all of the nodes as a list.
		 * @return std::vector<T>
		 */
		std::vector<T> getAllElements();

		/**
		 * @brief Rotate elements in the binary tree left around an element.
		 * 		Useful in Binary Search Trees.
		 * 
		 * @param n 
		 */
		void leftRotate(BinaryTreeNode<T>* n);

		/**
		 * @brief Rotate elements in the binary tree right around an element.
		 * 		Useful in Binary Search Trees.
		 * 
		 * @param n 
		 */
		void rightRotate(BinaryTreeNode<T>* n);

		/**
		 * @brief Swaps the position of node A and B. Both must be valid nodes.
		 * 
		 * @param A 
		 * @param B 
		 */
		void swapNodes(BinaryTreeNode<T>* A, BinaryTreeNode<T>* B);

	private:

		BinaryTreeNode<T>* rootNode = nullptr;
		void cleanUp(BinaryTreeNode<T>* node);
		int traverseCount(BinaryTreeNode<T>* node);
		void getElementsRecursive(std::vector<T>* data, BinaryTreeNode<T>* startNode);

		void getTraversalInfoRecursive(std::vector<TraverseInfo<T>>* data, BinaryTreeNode<T>* startNode, BinarySet binPath, bool leafOnly = false);
		int getHeightRecursive(BinaryTreeNode<T>* node, int currMaxHeight);
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

			if(leftChild != nullptr)
				leftChild->parent = parent;
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
			if(rightChild != nullptr)
				rightChild->parent = parent;
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
	inline int BinaryTree<T>::getHeight()
	{
		return getHeightRecursive(rootNode, 0);
	}

	template<typename T>
	inline int BinaryTree<T>::getHeightRecursive(BinaryTreeNode<T>* node, int currMaxHeight)
	{
		if(node != nullptr)
		{
			int nHeight = currMaxHeight+1;

			int leftHeight = nHeight;
			int rightHeight = nHeight;

			leftHeight = getHeightRecursive(node->leftChild, nHeight);
			rightHeight = getHeightRecursive(node->rightChild, nHeight);

			return (leftHeight > rightHeight) ? leftHeight : rightHeight;
		}
		else
		{
			return currMaxHeight;
		}
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

	template<typename T>
	inline void BinaryTree<T>::leftRotate(BinaryTreeNode<T>* A)
	{
		if(A != nullptr)
		{
			BinaryTreeNode<T>* oldParent = A->parent;
			BinaryTreeNode<T>* B = A->rightChild;
			BinaryTreeNode<T>* L = A->leftChild;
			BinaryTreeNode<T>* R = nullptr;
			BinaryTreeNode<T>* S = nullptr;
			
			if(B != nullptr)
			{
				R = B->leftChild;
				S = B->rightChild;
			}

			//manually set so nothing gets deleted
			A->rightChild = R;
			if(R != nullptr)
				R->parent = A;
			A->parent = B;
			if(B != nullptr)
			{
				B->parent = oldParent;
				B->leftChild = A;
			}
			if(oldParent != nullptr)
			{
				if(oldParent->leftChild == A)
					oldParent->leftChild = B;
				else
					oldParent->rightChild = B;
			}

			if(B->parent == nullptr)
				rootNode = B;
		}
	}

	template<typename T>
	inline void BinaryTree<T>::rightRotate(BinaryTreeNode<T>* B)
	{
		if(B != nullptr)
		{
			BinaryTreeNode<T>* oldParent = B->parent;
			BinaryTreeNode<T>* A = B->leftChild;
			BinaryTreeNode<T>* S = B->rightChild;
			BinaryTreeNode<T>* R = nullptr;
			BinaryTreeNode<T>* L = nullptr;
			
			if(A != nullptr)
			{
				L = A->leftChild;
				R = A->rightChild;
			}

			//manually set so nothing gets deleted
			B->leftChild = R;
			if(R != nullptr)
				R->parent = B;
			B->parent = A;
			if(A != nullptr)
			{
				A->parent = oldParent;
				A->rightChild = B;
			}
			
			if(oldParent != nullptr)
			{
				if(oldParent->leftChild == B)
					oldParent->leftChild = A;
				else
					oldParent->rightChild = A;
			}

			if(A->parent == nullptr)
				rootNode = A;
		}
	}

	template<typename T>
	inline void BinaryTree<T>::swapNodes(BinaryTreeNode<T>* A, BinaryTreeNode<T>* B)
	{
		if(A != nullptr && B != nullptr)
		{
			BinaryTreeNode<T>* AParent = A->parent;
			BinaryTreeNode<T>* BParent = B->parent;
			BinaryTreeNode<T>* BSideL = B->leftChild;
			BinaryTreeNode<T>* BSideR = B->rightChild;
			
			B->leftChild = A->leftChild;
			B->rightChild = A->rightChild;
			A->leftChild = BSideL;
			A->rightChild = BSideR;

			A->parent = BParent;
			B->parent = AParent;

			if(AParent != nullptr)
			{
				if(AParent->leftChild == A)
					AParent->leftChild = B;
				else
					AParent->rightChild = B;
			}
			if(BParent != nullptr)
			{
				if(BParent->leftChild == B)
					BParent->leftChild = A;
				else
					BParent->rightChild = A;
			}

			if(A == rootNode)
				rootNode = B;
			else if(B == rootNode)
				rootNode = A;
		}
	}

} //NAMESPACE glib END