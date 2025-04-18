#pragma once

namespace smpl
{

	template<typename T>
	struct LinkNode
	{
		LinkNode* parentNode = nullptr;
		LinkNode* nextNode = nullptr;
		T value;
	};

	template<typename T>
	class LinkedList
	{
	public:
		/**
		 * @brief Construct a new Linked List object
		 * 
		 */
		LinkedList();

		/**
		 * @brief Destroy the Linked List object along with all of the memory it allocated.
		 * 		Does not delete the template values if they are pointers.
		 */
		~LinkedList();

		/**
		 * @brief Construct a new Linked List object from another
		 * 		It copies the other leaving the other intact
		 * 
		 * @param other 
		 */
		LinkedList(const LinkedList<T>& other);
		/**
		 * @brief Copies another LinkedList leaving the both intact
		 * 
		 * @param other 
		 */
		void operator=(const LinkedList<T>& other);

		/**
		 * @brief Construct a new Linked List object.
		 * 		Moves one object into the other. The other object will no longer be valid.
		 * 
		 * @param other 
		 */
		LinkedList(LinkedList<T>&& other) noexcept;
		
		/**
		 * @brief Moves one object into the other. 
		 * 		The other object will no longer be valid.
		 * 
		 * @param other 
		 */
		void operator=(LinkedList<T>&& other) noexcept;

		/**
		 * @brief Adds a new node at the end of the Linked List.
		 * 
		 * @param n 
		 * 		The data to add.
		 */
		void addNode(T n);

		/**
		 * @brief Adds a new node right after the parent node specified.
		 * 		It is assumed that this parent node is apart of the linked list.
		 * 
		 * @param n 
		 * @param parentNode 
		 */
		void addNode(T n, LinkNode<T>* parentNode);

		/**
		 * @brief Removes a node from the list.
		 * 
		 * @param n 
		 * 		The pointer to the node to remove.
		 */
		void removeNode(LinkNode<T>* n);

		/**
		 * @brief Swaps a node with another node.
		 * 		These nodes should both be in the list to avoid memory leaks and other error.
		 * 
		 * @param n1 
		 * 		The first node.
		 * @param n2 
		 * 		The second node.
		 */
		void swapNodes(LinkNode<T>* n1, LinkNode<T>* n2);
		
		void removeFirst();

		/**
		 * @brief Gets the Root Node
		 * 
		 * @return LinkNode<T>* 
		 */
		LinkNode<T>* getRootNode();

		/**
		 * @brief Gets the Last Node
		 * 
		 * @return LinkNode<T>* 
		 */
		LinkNode<T>* getLastNode();

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
		LinkNode<T>* rootNode = nullptr;
		LinkNode<T>* lastNode = nullptr;
	};

	template<typename T>
	inline void LinkedList<T>::removeFirst()
	{
		if(rootNode != nullptr)
		{
			LinkNode<T>* oldRootNode = rootNode;
			rootNode = oldRootNode->nextNode;
			if(rootNode != nullptr)
				rootNode->parentNode = nullptr;
			else
			{
				lastNode = nullptr;
			}
			delete oldRootNode;
		}
	}

	template<typename T>
	inline LinkedList<T>::LinkedList()
	{
	}

	template<typename T>
	inline LinkedList<T>::~LinkedList()
	{
		clear();
	}

	template<typename T>
	LinkedList<T>::LinkedList(const LinkedList<T>& other)
	{
		clear();
		LinkNode<T>* otherNode = other.rootNode;
		while(otherNode != nullptr)
		{
			this->addNode(otherNode->value);
			otherNode = otherNode->nextNode;
		}
	}

	template<typename T>
	void LinkedList<T>::operator=(const LinkedList<T>& other)
	{
		clear();
		LinkNode<T>* otherNode = other.rootNode;
		while(otherNode != nullptr)
		{
			this->addNode(otherNode->value);
			otherNode = otherNode->nextNode;
		}
	}

	template<typename T>
	LinkedList<T>::LinkedList(LinkedList<T>&& other) noexcept
	{
		clear();
		//quick move. A bit unsafe but should be fine.
		rootNode = other.rootNode;
		lastNode = other.lastNode;
		other.rootNode = nullptr;
		other.lastNode = nullptr;
	}
	
	template<typename T>
	void LinkedList<T>::operator=(LinkedList<T>&& other) noexcept
	{
		clear();
		//quick move. A bit unsafe but should be fine.
		rootNode = other.rootNode;
		lastNode = other.lastNode;
		other.rootNode = nullptr;
		other.lastNode = nullptr;
	}

	template<typename T>
	inline void LinkedList<T>::addNode(T n)
	{
		LinkNode<T>* lNode = getLastNode();

		LinkNode<T>* newNode = new LinkNode<T>;
		newNode->value = n;

		if (lNode != nullptr)
		{
			lNode->nextNode = newNode;
			newNode->parentNode = lNode;

			lastNode = newNode;
		}
		else
		{
			rootNode = newNode;
			lastNode = newNode;
		}
	}

	template<typename T>
	inline void LinkedList<T>::addNode(T n, LinkNode<T>* parentNode)
	{
		if(parentNode == nullptr)
			return;
		if(parentNode == lastNode)
		{
			addNode(n);
			return;
		}

		//set parent of newNode and set its next link
		//set parent's next node to be newNode
		//set newNode's next link parent to newNode
		LinkNode<T>* newNode = new LinkNode<T>;
		newNode->value = n;
		newNode->parentNode = parentNode;
		newNode->nextNode = parentNode->nextNode;
		parentNode->nextNode = newNode;
		newNode->nextNode->parentNode = newNode;
	}

	template<typename T>
	inline void LinkedList<T>::removeNode(LinkNode<T>* n)
	{
		if(n == nullptr)
			return;
		
		LinkNode<T>* parent = n->parentNode;
		LinkNode<T>* next = n->nextNode;

		if (parent == nullptr)
		{
			//n was root
			//next will be new root
			if (next != nullptr)
			{
				rootNode = next;
				next->parentNode = nullptr;

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
				lastNode = n->parentNode;
			}
			
			if (next != nullptr)
			{
				parent->nextNode = next;
				next->parentNode = parent;
			}
			else
			{
				parent->nextNode = nullptr;
			}
		}

	}

	template<typename T>
	inline void LinkedList<T>::swapNodes(LinkNode<T>* n1, LinkNode<T>* n2)
	{
		if(n1!=nullptr && n2!=nullptr)
		{
			if(n1->nextNode == n2)
			{
				//Swap parent relation ship then swap next
				LinkNode<T>* tempParent = n1->parentNode;
				LinkNode<T>* tempNext = n2->nextNode;

				n1->parentNode = n2;
				n1->nextNode = tempNext;

				n2->nextNode = n1;
				n2->parentNode = tempParent;
			}
			else if(n1->parentNode == n2)
			{
				//Swap parent relation ship then swap next
				LinkNode<T>* tempParent = n2->parentNode;
				LinkNode<T>* tempNext = n1->nextNode;

				n1->parentNode = tempParent;
				n1->nextNode = n2;
				
				n2->parentNode = n1;
				n2->nextNode = tempNext;
			}
			else
			{
				//Complete swap
				LinkNode<T>* tempParent = n1->parentNode;
				LinkNode<T>* tempNext = n1->nextNode;

				n1->parentNode = n2->parentNode;
				n1->nextNode = n2->nextNode;

				n2->parentNode = tempParent;
				n2->nextNode = tempNext;
			}

			if(n1->parentNode!=nullptr)
				n1->parentNode->nextNode = n1;
			
			if(n2->nextNode!=nullptr)
				n1->nextNode->parentNode = n1;

			if(n2->parentNode!=nullptr)
				n2->parentNode->nextNode = n2;

			if(n2->nextNode!=nullptr)
				n2->nextNode->parentNode = n2;

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
	inline LinkNode<T>* LinkedList<T>::getRootNode()
	{
		return rootNode;
	}

	template<typename T>
	inline LinkNode<T>* LinkedList<T>::getLastNode()
	{
		return lastNode;
	}

	template<typename T>
	inline int LinkedList<T>::size()
	{
		int i = 0;
		LinkNode<T>* lNode = rootNode;

		if (lNode != nullptr)
		{
			i = 1;
			while (lNode->nextNode != nullptr)
			{
				lNode = lNode->nextNode;
				i++;
			}
		}
		return i;
	}

	template<typename T>
	inline bool LinkedList<T>::empty()
	{
		return (rootNode == nullptr && lastNode == nullptr);
	}

	template<typename T>
	inline void LinkedList<T>::clear()
	{
		LinkNode<T>* lNode = getLastNode();

		if(lNode != nullptr)
		{
			while (lNode->parentNode != nullptr)
			{
				LinkNode<T>* nNode = lNode;
				lNode = nNode->parentNode;

				if(nNode != nullptr)
					delete nNode;
			}
		}

		if(rootNode != nullptr)
			delete rootNode;

		lastNode = nullptr;
		rootNode = nullptr;
	}

} //NAMESPACE smpl END