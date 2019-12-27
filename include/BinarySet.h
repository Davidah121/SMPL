#pragma once
#include <vector>
#include "Object.h"

class BinarySet : public Object
{
public:
	BinarySet();
	~BinarySet();
	
	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void add(bool v);
	void add(char v, int amountOfBits = 8, int offset = 0, bool reverse = false);
	void add(unsigned char v, int amountOfBits = 8, int offset = 0, bool reverse = false);
	void add(short v, int amountOfBits = 16, int offset = 0, bool reverse = false);
	void add(unsigned short v, int amountOfBits = 16, int offset = 0, bool reverse = false);
	void add(int v, int amountOfBits = 32, int offset = 0, bool reverse = false);
	void add(unsigned int v, int amountOfBits = 32, int offset = 0, bool reverse = false);
	void add(long v, int amountOfBits = 64, int offset = 0, bool reverse = false);
	void add(unsigned long v, int amountOfBits = 64, int offset = 0, bool reverse = false);

	bool getBit(int index);
	std::vector<unsigned char> toBytes(bool reverse=false);

	int size();
	void printVals(bool reverse=false);

	void clear();

private:
	std::vector<bool> set = std::vector<bool>();
};

