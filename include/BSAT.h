#pragma once

#include <iostream>
#include <vector>
#include "BinarySearchTree.h"
#include "LinkedList.h"

namespace glib
{

	class BSAT : private BinarySearchTree<uint32_t>
	{
		public:
			BSAT();
			~BSAT();

			BinaryTreeNode<RBNode<uint32_t>>* push(unsigned char v);
			void remove(BinaryTreeNode<RBNode<uint32_t>>* n);
			
			uint32_t searchIndex(std::string s);
			uint32_t searchIndex(std::vector<unsigned char> s);
			uint32_t searchIndex(unsigned char* s, size_t size);

			BRS<RBNode<uint32_t>> binaryRangeSearch(unsigned char v, int offset, BRS<RBNode<uint32_t>> existingRange = {});
			BinaryTree<RBNode<uint32_t>>* getRawTree();
			std::vector<unsigned char>& getBuffer();

		private:
			BinaryTreeNode<RBNode<uint32_t>>* add(uint32_t data);
			std::vector<unsigned char> buffer; //data buffer

	};
}