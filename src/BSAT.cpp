#include "BSAT.h"

namespace glib
{

	BSAT::BSAT() : BinarySearchTree<uint32_t>(BinarySearchTree<uint32_t>::RED_BLACK)
	{
		buffer = std::vector<unsigned char>();
	}

	BSAT::~BSAT()
	{
		//tree and buffer automatically cleared
	}

	BinaryTree<RBNode<uint32_t>>* BSAT::getRawTree()
	{
		return &tree;
	}

	std::vector<unsigned char>& BSAT::getBuffer()
	{
		return buffer;
	}

	BinaryTreeNode<RBNode<uint32_t>>* BSAT::push(unsigned char c)
	{
		buffer.push_back(c);
		return this->add(buffer.size()-1);
	}

	void BSAT::remove(BinaryTreeNode<RBNode<uint32_t>>* n)
	{
		BinarySearchTree<uint32_t>::remove(n);
	}

	uint32_t BSAT::searchIndex(std::string s)
	{
		return searchIndex((unsigned char*)s.data(), s.size());
	}

	uint32_t BSAT::searchIndex(std::vector<unsigned char> s)
	{
		return searchIndex(s.data(), s.size());
	}

	uint32_t BSAT::searchIndex(unsigned char* s, size_t size)
	{
		BinaryTreeNode<RBNode<uint32_t>>* node = tree.getRoot();
		while(node != nullptr)
		{
			uint32_t minSize = (uint32_t)__min(size, buffer.size() - node->data.data);
			int cmpVal = memcmp(s, &buffer[node->data.data], minSize);
			if(cmpVal == 0)
				return node->data.data;
			else if(cmpVal < 0)
				node = node->leftChild;
			else
				node = node->rightChild;
		}
		return UINT_MAX;
	}
	
	BRS<RBNode<uint32_t>> BSAT::binaryRangeSearch(unsigned char v, int offset, BRS<RBNode<uint32_t>> existingRange)
	{
		//Similar to how binaryRangeSearch works for a normal BinarySearchTree
		//Red-Black Balancing does not affect the inorder traversal of the tree so this still works
		BinaryTreeNode<RBNode<uint32_t>>* currNode = existingRange.commonAncestor;
		if(existingRange.commonAncestor == nullptr)
		{
			//no existing range
			currNode = tree.getRoot();
		}

		if(currNode == nullptr)
		{
			return {}; //No data in the tree
		}

		BRS<RBNode<uint32_t>> newRange;

		//First, find the common ancestor
		while(true)
		{
			int cmpVal = 1;
			if(currNode->data.data + offset < buffer.size())
				cmpVal = memcmp(&v, &buffer[currNode->data.data + offset], 1);
			
			if(cmpVal == 0)
			{
				newRange.commonAncestor = currNode;
				break; //Found the common ancestor
			}
			else if(cmpVal > 0)
			{
				//try to go right since we are not within the correct bounds
				if(currNode != existingRange.mostRight)
				{
					if(currNode->rightChild == nullptr)
					{
						//went as far right as possible
						break;
					}
					currNode = currNode->rightChild;
				}
				else
					break; //can't go right any more
			}
			else
			{
				//try to go left since we are not within the correct bounds
				if(currNode != existingRange.mostLeft)
				{
					if(currNode->leftChild == nullptr)
					{
						//went as far left as possible
						break;
					}
					currNode = currNode->leftChild;
				}
				else
					break; //can't go left any more
			}
		}

		if(newRange.commonAncestor == nullptr)
		{
			//could not find a node using the existing range that was in these bounds
			return {};
		}

		//using the new common ancestor, find the most left node
		currNode = newRange.commonAncestor;
		while(true)
		{
			int cmpVal = -1;
			if(currNode->data.data + offset < buffer.size())
				cmpVal = memcmp(&v, &buffer[currNode->data.data + offset], 1);
			
			if(cmpVal == 0)
			{
				//go left if possible
				if(currNode != existingRange.mostLeft && currNode->leftChild != nullptr)
					currNode = currNode->leftChild;
				else
					break;
			}
			else
			{
				//go back one and return that since that must be valid.
				currNode = currNode->parent;
				break;
			}
		}
		newRange.mostLeft = currNode;

		currNode = newRange.commonAncestor;
		while(true)
		{
			int cmpVal = 1;
			if(currNode->data.data + offset < buffer.size())
				cmpVal = memcmp(&v, &buffer[currNode->data.data + offset], 1);
			
			if(cmpVal == 0)
			{
				//go right if possible
				if(currNode != existingRange.mostRight && currNode->rightChild != nullptr)
					currNode = currNode->rightChild;
				else
					break;
			}
			else
			{
				//go back one and return that since that must be valid.
				currNode = currNode->parent;
				break;
			}
		}
		newRange.mostRight = currNode;

		return newRange;
	}

	BinaryTreeNode<RBNode<uint32_t>>* BSAT::add(uint32_t data)
	{
		//create new node
		count++;
		BinaryTreeNode<RBNode<uint32_t>>* selectedNode = tree.getRoot();

		if(selectedNode == nullptr)
		{
			//create new node as the root
			BinaryTreeNode<RBNode<uint32_t>>* p = new BinaryTreeNode<RBNode<uint32_t>>();
			p->data.color = COLOR_BLACK;
			p->data.data = data;

			//Set the BinaryTree stuff
			p->leftChild = nullptr;
			p->rightChild = nullptr;
			tree.setRootNode( p );
			return p;
		}

		int insertMode = 0; //0=do nothing, 1=left, 2=right
		int c1 = buffer[data];
		int c2 = 0;
		while(selectedNode != nullptr)
		{
			//check the first character
			c2 = buffer[selectedNode->data.data];

			// int cmpVal = memcmp(&buffer[data], &buffer[selectedNode->data.data], 1);
			
			//Not ever truly equal since its size is less than the existing data in the list.
			//Consider it less than and go left if it is equal
			if(c1 <= c2)
			{
				//go left
				if(selectedNode->leftChild == nullptr)
				{
					if(c1 == c2)
						printf("Node==%u\n", selectedNode->data.data);
					
					insertMode = 1;
					break;
				}
				selectedNode = selectedNode->leftChild;
			}
			else
			{
				//go right
				if(selectedNode->rightChild == nullptr)
				{
					insertMode = 2;
					break;
				}
				selectedNode = selectedNode->rightChild;
			}
		}

		BinaryTreeNode<RBNode<uint32_t>>* p = nullptr;
		if(insertMode == 0)
		{
			//error of some sort
			count--;
			return nullptr;
		}
		else if(insertMode == 1)
		{
			//insert left
			p = new BinaryTreeNode<RBNode<uint32_t>>();
			//Set the RedBlackNode stuff
			p->data.color = COLOR_RED;
			p->data.data = data;

			//Set the BinaryTree stuff
			p->leftChild = nullptr;
			p->rightChild = nullptr;
			tree.setLeftNode(selectedNode, p);
		}
		else
		{
			//go right
			p = new BinaryTreeNode<RBNode<uint32_t>>();
			//Set the RedBlackNode stuff
			p->data.color = COLOR_RED;
			p->data.data = data;

			//Set the BinaryTree stuff
			p->leftChild = nullptr;
			p->rightChild = nullptr;
			tree.setRightNode(selectedNode, p);
		}

		if(type == RED_BLACK)
		{
			redBlackReBalance(p); //Height can change here
			
			if(tree.getRoot() != nullptr)
				tree.getRoot()->data.color = COLOR_BLACK;
		}

		return p;
	}

}