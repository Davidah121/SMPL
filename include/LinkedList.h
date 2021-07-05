#pragma once

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
	LinkedList();
	~LinkedList();

	void addNode(T n);
	void addNode(int position, T n);
	void removeNode(int position);
	void removeNode(LinkNode<T>* n);
	LinkNode<T>* getNode(int position);
	void swapNodes(LinkNode<T>* n1, LinkNode<T>* n2);

	LinkNode<T>* getRootNode();
	LinkNode<T>* getLastNode();

	int size();

	void clear();
private:
	LinkNode<T>* rootNode = nullptr;
	LinkNode<T>* lastNode = nullptr;
};

template<typename T>
inline LinkedList<T>::LinkedList()
{
}

template<typename T>
inline LinkedList<T>::~LinkedList()
{
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
inline void LinkedList<T>::addNode(int position, T n)
{
}

template<typename T>
inline void LinkedList<T>::removeNode(int position)
{
}

template<typename T>
inline void LinkedList<T>::removeNode(LinkNode<T>* n)
{
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
inline LinkNode<T>* LinkedList<T>::getNode(int position)
{
	return nullptr;
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
inline void LinkedList<T>::clear()
{
	LinkNode<T>* lNode = getLastNode();

	while (lNode->parentNode != nullptr)
	{
		LinkNode<T>* nNode = lNode;
		lNode = nNode->parentNode;

		delete nNode;
	}

	delete rootNode;

	lastNode = nullptr;
	rootNode = nullptr;
}
