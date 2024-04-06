#pragma once
#include "StringTools.h"
#include <vector>

namespace smpl
{

	template<typename T>
	struct TreeNode
	{
		std::vector<TreeNode<T>*> children;
		T data;
	};

	template<typename T>
	class Tree
	{
	public:
		/**
		 * @brief Creates a template Tree object.
		 * 		Uses the template TreeNode structure for each node.
		 * 		This is a general purpose version of the BinaryTree class.
		 */
		Tree();

		/**
		 * @brief Destroys the Tree Object and cleans up all memory.
		 * 		Note that it deletes all TreeNode pointers but not the data they store.
		 * 		This should be done by the user or pointers should be avoided unless they are
		 * 		smart pointers.
		 */
		~Tree();

		/**
		 * @brief Sets the root node of the tree to the specified node pointer.
		 * @param node
		 * 		The node to become the new root node.
		 */
		void setRootNode(TreeNode<T>* node);

		/**
		 * @brief Adds a new child tree node to the parent.
		 * 		It gets added on to the end of the parent's children array.
		 * 
		 * @param parent 
		 * @param child 
		 */
		void addChildNode(TreeNode<T>* parent, TreeNode<T>* child);

		/**
		 * @brief Sets the Child Node of the parent at a the specified index.
		 * 
		 * @param parent 
		 * @param child 
		 * @param index 
		 */
		void setChildNode(TreeNode<T>* parent, TreeNode<T>* child, int index);

		/**
		 * @brief Returns the root of the tree.
		 * @return TreeNode<T>*
		 * 		Can be nullptr
		 */
		TreeNode<T>* getRoot();

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

	private:
		TreeNode<T>* rootNode = nullptr;
		void cleanUp(TreeNode<T>* node);
		int traverseCount(TreeNode<T>* node);
		void getElementsRecursive(std::vector<T>* data, TreeNode<T>* startNode);

		int getHeightRecursive(TreeNode<T>* node, int currMaxHeight);
	};

	template<typename T>
	inline Tree<T>::Tree()
	{

	}

	template<typename T>
	inline Tree<T>::~Tree()
	{
		cleanUp(rootNode);
		rootNode = nullptr;
	}

	template<typename T>
	inline void Tree<T>::setRootNode(TreeNode<T>* parent)
	{
		if (rootNode != parent)
			cleanUp(rootNode);

		rootNode = parent;
	}

	template<typename T>
	inline void Tree<T>::addChildNode(TreeNode<T>* parent, TreeNode<T>* child)
	{
		if(parent != nullptr)
		{
			parent->children.push_back(child);
		}
	}

	template<typename T>
	inline void Tree<T>::setChildNode(TreeNode<T>* parent, TreeNode<T>* child, int index)
	{
		if(parent != nullptr)
		{
			if(parent->children.size() > index)
			{
				if(parent->children[index] != nullptr)
				{
					//cleanup first
					cleanUp(parent->children[index]);
				}
				
				parent->children[index] = child;
			}
		}
	}

	template<typename T>
	inline TreeNode<T>* Tree<T>::getRoot()
	{
		return rootNode;
	}
	
	template<typename T>
	inline int Tree<T>::getSize()
	{
		return traverseCount(rootNode);
	}

	template<typename T>
	inline int Tree<T>::getHeight()
	{
		return getHeightRecursive(rootNode, 0);
	}

	template<typename T>
	inline int Tree<T>::getHeightRecursive(TreeNode<T>* node, int currMaxHeight)
	{
		if(node != nullptr)
		{
			int nHeight = currMaxHeight+1;
			int bestHeight = nHeight;

			for(int i=0; i<node->children.size(); i++)
			{
				int tempHeight = getHeightRecursive(node->children[i], nHeight);
				if(tempHeight > bestHeight)
				{
					bestHeight = tempHeight;
				}
			}

			return bestHeight;
		}
		else
		{
			return currMaxHeight;
		}
	}

	template<typename T>
	inline void Tree<T>::cleanUp(TreeNode<T>* currentNode)
	{
		if (currentNode != nullptr)
		{
			for(int i=0; i<currentNode->children.size(); i++)
			{
				cleanUp(currentNode->children[i]);
			}
			
			delete currentNode;
		}
	}

	template<typename T>
	inline int Tree<T>::traverseCount(TreeNode<T>* node)
	{
		if (node != nullptr)
		{
			int v = 1;//count itself

			for(int i=0; i<node->children.size(); i++)
			{
				v += traverseCount(node->children[i]);
			}

			return v;
		}
		return 0;
	}

	template<typename T>
	inline std::vector<T> Tree<T>::getAllElements()
	{
		std::vector<T> arr = std::vector<T>();

		getElementsRecursive(&arr, rootNode);

		return arr;
	}

	template<typename T>
	inline void Tree<T>::getElementsRecursive(std::vector<T>* arr, TreeNode<T>* node)
	{
		if(node!=nullptr)
		{
			arr->push_back(node->data);
			for(int i=0; i<node->children.size(); i++)
			{
				getElementsRecursive(arr, node->children[i]);
			}
		}
	}

} //NAMESPACE glib END