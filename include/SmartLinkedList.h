#pragma once
#include "SmartMemory.h"

namespace smpl
{

	template<typename T>
	struct SLinkNode
	{
		SmartMemory<SLinkNode<T>> parentNode = nullptr;
		SmartMemory<SLinkNode<T>> nextNode = nullptr;
		T value;
	};

	template<typename T>
	class SmartLinkedList
	{
	public:
		/**
		 * @brief Construct a new Linked List object
		 * 
		 */
		SmartLinkedList();

		/**
		 * @brief Destroy the Linked List object along with all of the memory it allocated.
		 * 		Does not delete the template values if they are pointers.
		 */
		~SmartLinkedList();

		/**
		 * @brief Adds a new node at the end of the Linked List.
		 * 		Returns a Smart Pointer to the node that was added.
		 * @param n 
		 * 		The data to add.
		 * @return SmartMemory<SLinkNode<T>>
		 */
		SmartMemory<SLinkNode<T>> addNode(T n);

		/**
		 * @brief Removes a node from the list.
		 * 		Returns if successful
		 * 
		 * @param n 
		 * 		The pointer to the node to remove.
		 * @return bool
		 */
		bool removeNode(SmartMemory<SLinkNode<T>> n);

		/**
		 * @brief Swaps a node with another node.
		 * 		These nodes should both be in the list to avoid memory leaks and other error.
		 * 
		 * @param n1 
		 * 		The first node.
		 * @param n2 
		 * 		The second node.
		 */
		void swapNodes(SmartMemory<SLinkNode<T>> n1, SmartMemory<SLinkNode<T>> n2);

		/**
		 * @brief Gets the Root Node
		 * 
		 * @return SmartMemory<SLinkNode<T>>&
		 */
		SmartMemory<SLinkNode<T>>& getRootNode();

		/**
		 * @brief Gets the Last Node
		 * 
		 * @return SmartMemory<SLinkNode<T>>&
		 */
		SmartMemory<SLinkNode<T>>& getLastNode();

		/**
		 * @brief Returns the size of the Linked List
		 * 
		 * @return int 
		 */
		int size();

		/**
		 * @brief Returns whether the list is empty.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool empty();

		/**
		 * @brief Clears the data for the Linked List.
		 * 		Does not delete the template values if they are pointers.
		 */
		void clear();
	private:
		SmartMemory<SLinkNode<T>> rootNode = nullptr;
		SmartMemory<SLinkNode<T>> lastNode = nullptr;
	};

	template<typename T>
	inline SmartLinkedList<T>::SmartLinkedList()
	{
	}

	template<typename T>
	inline SmartLinkedList<T>::~SmartLinkedList()
	{
		clear();
	}

	template<typename T>
	inline SmartMemory<SLinkNode<T>> SmartLinkedList<T>::addNode(T n)
	{
		SmartMemory<SLinkNode<T>> lNode = getLastNode();

		SLinkNode<T>* tempNode = new SLinkNode<T>();
		tempNode->value = n;

		SmartMemory<SLinkNode<T>> newNode = SmartMemory<SLinkNode<T>>::createDeleteOnLast(tempNode);

		if (lNode.getPointer() != nullptr)
		{
			lNode.getRawPointer()->nextNode = newNode;
			newNode.getRawPointer()->parentNode = lNode;

			lastNode = newNode;
		}
		else
		{
			rootNode = newNode;
			lastNode = newNode;
		}

		return newNode;
	}

	template<typename T>
	inline bool SmartLinkedList<T>::removeNode(SmartMemory<SLinkNode<T>> n)
	{
		SLinkNode<T>* p = n.getPointer();
		if(p != nullptr)
		{
			//trick to force delete the pointer
			SmartMemory<SLinkNode<T>> forceDelete = SmartMemory<SLinkNode<T>>::createDeleteRights(p);

			SmartMemory<SLinkNode<T>> parent = p->parentNode;
			SmartMemory<SLinkNode<T>> next = p->nextNode;

			if (parent.getPointer() == nullptr)
			{
				//n was root
				//next will be new root
				if (next.getPointer() != nullptr)
				{
					rootNode = next;
					next.getRawPointer()->parentNode = nullptr;

				}
				else
				{
					//blank node
					rootNode = nullptr;
					lastNode = nullptr;
				}
			}
			else
			{
				if(lastNode == n)
				{
					lastNode = p->parentNode;
				}
				
				if (next.getPointer() != nullptr)
				{
					parent.getRawPointer()->nextNode = next;
					next.getRawPointer()->parentNode = parent;
				}
				else
				{
					parent.getRawPointer()->nextNode = nullptr;
				}
			}

			return true;
		}
		return false;
	}

	template<typename T>
	inline void SmartLinkedList<T>::swapNodes(SmartMemory<SLinkNode<T>> n1, SmartMemory<SLinkNode<T>> n2)
	{
		if(n1.getPointer()!=nullptr && n2.getPointer()!=nullptr)
		{
			SLinkNode<T>* n1p = n1.getRawPointer();
			SLinkNode<T>* n2p = n2.getRawPointer();
			
			if(n1p->nextNode == n2)
			{
				//Swap parent relation ship then swap next
				SmartMemory<SLinkNode<T>> tempParent = n1p->parentNode;
				SmartMemory<SLinkNode<T>> tempNext = n2p->nextNode;

				n1p->parentNode = n2;
				n1p->nextNode = tempNext;

				n2p->nextNode = n1;
				n2p->parentNode = tempParent;
			}
			else if(n1->parentNode == n2)
			{
				//Swap parent relation ship then swap next
				SmartMemory<SLinkNode<T>> tempParent = n2p->parentNode;
				SmartMemory<SLinkNode<T>> tempNext = n1p->nextNode;

				n1p->parentNode = tempParent;
				n1p->nextNode = n2;
				
				n2p->parentNode = n1;
				n2p->nextNode = tempNext;
			}
			else
			{
				//Complete swap
				SmartMemory<SLinkNode<T>> tempParent = n1p->parentNode;
				SmartMemory<SLinkNode<T>> tempNext = n1p->nextNode;

				n1p->parentNode = n2p->parentNode;
				n1p->nextNode = n2p->nextNode;

				n2p->parentNode = tempParent;
				n2p->nextNode = tempNext;
			}

			if(n1p->parentNode!=nullptr)
				n1p->parentNode->nextNode = n1;
			
			if(n2p->nextNode!=nullptr)
				n1p->nextNode->parentNode = n1;

			if(n2p->parentNode!=nullptr)
				n2p->parentNode->nextNode = n2;

			if(n2p->nextNode!=nullptr)
				n2p->nextNode->parentNode = n2;

			if(lastNode == n1)
				lastNode = n2;
			else if(lastNode == n2)
				lastNode = n1;

			if(rootNode == n1)
				rootNode = n2;
			else if(rootNode == n2)
				rootNode = n1;
			
		}
	}

	template<typename T>
	inline SmartMemory<SLinkNode<T>>& SmartLinkedList<T>::getRootNode()
	{
		return rootNode;
	}

	template<typename T>
	inline SmartMemory<SLinkNode<T>>& SmartLinkedList<T>::getLastNode()
	{
		return lastNode;
	}

	template<typename T>
	inline int SmartLinkedList<T>::size()
	{
		int i = 0;
		SmartMemory<SLinkNode<T>> lNode = rootNode;

		if (lNode.getPointer() != nullptr)
		{
			i = 1;
			while (lNode.getRawPointer()->nextNode.getPointer() != nullptr)
			{
				lNode = lNode.getRawPointer()->nextNode;
				i++;
			}
		}
		return i;
	}

	template<typename T>
	inline bool SmartLinkedList<T>::empty()
	{
		return (rootNode == nullptr && lastNode == nullptr);
	}

	template<typename T>
	inline void SmartLinkedList<T>::clear()
	{
		//should be able to just delete the first node and last node and remove everything
		//for each node, set every pointer to 0
		SmartMemory<SLinkNode<T>> node = rootNode;
		while(true)
		{
			if(node.getPointer() != nullptr)
			{
				//Trick to force delete the node
				//Force delete memory of the current node. This makes all smart pointers to this node invalid.
				SmartMemory<SLinkNode<T>> thisNode = SmartMemory<SLinkNode<T>>::createDeleteOnLast(node.getRawPointer());

				SmartMemory<SLinkNode<T>> nNode = node.getRawPointer()->nextNode;
				node.getRawPointer()->nextNode = nullptr;
				node.getRawPointer()->parentNode = nullptr;
				node = nNode;
			}
			else
				break;
		}

		lastNode = nullptr;
		rootNode = nullptr;
	}

} //NAMESPACE glib END