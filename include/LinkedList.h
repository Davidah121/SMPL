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

	LinkNode<T>* getRootNode();
	LinkNode<T>* getLastNode();

	int size();

	void clear();
private:
	LinkNode<T>* rootNode;
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
	LinkNode<T>* lastNode = getLastNode();

	LinkNode<T>* newNode = new LinkNode<T>;
	newNode->value = n;

	if (lastNode != nullptr)
	{
		lastNode->nextNode = newNode;
		newNode->parentNode = lastNode;
	}
	else
	{
		rootNode = newNode;
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
		}
	}
	else
	{
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
	LinkNode<T>* lastNode = rootNode;
	if (lastNode != nullptr)
	{
		while (lastNode->nextNode != nullptr)
		{
			lastNode = lastNode->nextNode;
		}
	}
	return lastNode;
}

template<typename T>
inline int LinkedList<T>::size()
{
	int i = 0;
	LinkNode<T>* lastNode = rootNode;

	if (lastNode != nullptr)
	{
		i = 1;
		while (lastNode->nextNode != nullptr)
		{
			lastNode = lastNode->nextNode;
			i++;
		}
	}
	return i;
}

template<typename T>
inline void LinkedList<T>::clear()
{
	LinkNode<T>* lastNode = getLastNode();

	while (lastNode->parentNode != nullptr)
	{
		LinkNode<T>* nNode = lastNode;
		lastNode = nNode->parentNode;

		delete nNode;
	}

	delete rootNode;
}
