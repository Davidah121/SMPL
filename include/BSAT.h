#include <iostream>
#include <vector>
#include "BinarySearchTree.h"
#include "LinkedList.h"

//!TODO - Rewrite so that it does not need a special structure. Index should give you everything you need to compare the data.
//!TODO - BinarySearchTree has been changed so that modification is possible through subclasses. Basically, adjust search and add.
namespace glib
{
	struct BSATData;
	class BSAT;

	struct BSATData
	{
		public:

			BSATData(unsigned char* pB, uint32_t startPoint, uint32_t index, uint32_t size);
			~BSATData();
			
			bool operator<(BSATData o);
			bool operator==(BSATData o);

			void reset();
			unsigned char getNext();

			uint32_t getIndex();
		private:
			unsigned char* data;
			uint32_t index = 0;
			
	};

	class BSAT 
	{
		public:

			BSAT(int dictionarySize);
			~BSAT();

			void push(unsigned char v);
			void pop_newest();
			void pop_oldest();
			
			uint32_t searchIndex(std::string s);
			uint32_t searchIndex(std::vector<unsigned char> s);
		private:
			BinarySearchTree<BSATData> bTree = BinarySearchTree<BSATData>(BinarySearchTree<BSATData>::RED_BLACK); //suffix array tree for searching
			LinkedList<BinaryTreeNode<RBNode<BSATData>>*> queue; //queue of nodes added
			std::vector<unsigned char> buffer; //rolling buffer
			uint32_t largestIndex = 0;
			uint32_t currIndex = 0;
	};
}