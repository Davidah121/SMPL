#include "BSAT.h"

namespace glib
{

	BSATData::BSATData(unsigned char* pB, uint32_t startPoint, uint32_t index, uint32_t size)
	{
		this->data = pB;
		this->size = size;
		this->start = startPoint;
		this->next = startPoint;
		this->index = index;
	}

	BSATData::~BSATData()
	{

	}

	void BSATData::reset()
	{
		next = start;
	}

	unsigned char BSATData::getNext()
	{
		unsigned char c = 0;
		c = data[next];
		next++;
		if(next >= size)
			next = 0;
	
		return c;
	}
	
	bool BSATData::operator<(BSATData o)
	{
		reset();
		o.reset();

		uint32_t minSize = __min(size, o.size);
		for(int i=0; i<minSize; i++)
		{
			unsigned char c1 = getNext();
			unsigned char c2 = o.getNext();
			if(c1 < c2)
				return true;
			if(c1 > c2)
				return false;
		}
		return false;
	}
	bool BSATData::operator==(BSATData o)
	{
		reset();
		o.reset();

		uint32_t minSize = __min(size, o.size);
		for(int i=0; i<minSize; i++)
		{
			unsigned char c1 = getNext();
			unsigned char c2 = o.getNext();
			if(c1 != c2)
				return false;
		}
		return true;
	}
	

	BSAT::BSAT(int dictionarySize)
	{
		buffer = std::vector<unsigned char>(dictionarySize);
	}
	BSAT::~BSAT()
	{

	}

	void BSAT::push(unsigned char v)
	{
		//add to tree
		//special case since only need to check first character
		
		BSATData v = BSATData(buffer.data(), currIndex, largestIndex, buffer.size());
		buffer[currIndex] = v;
		currIndex = currIndex+1 % buffer.size();
		largestIndex++;

		// bTree.
		//add new node to queue
	}

	void BSAT::pop_newest()
	{
		auto n = queue.getLastNode();
		bTree.remove(n->value);
		queue.removeNode( n );
	}

	void BSAT::pop_oldest()
	{
		auto n = queue.getRootNode();
		bTree.remove(n->value);
		queue.removeNode( n );
	}

	uint32_t BSAT::searchIndex(std::string s)
	{

	}

	uint32_t BSAT::searchIndex(std::vector<unsigned char> s)
	{

	}
};