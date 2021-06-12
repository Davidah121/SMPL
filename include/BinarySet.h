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
	static const Class myClass;

	//CONSTANTS
	static const bool LMSB = true; //Left bit is the most significant in a byte
	static const bool RMSB = false; //Right bit is the most significant in a byte

	void add(bool v);
	void add(char v, int amountOfBits = 8, int offset = 0);
	void add(unsigned char v, int amountOfBits = 8, int offset = 0);
	void add(short v, int amountOfBits = 16, int offset = 0);
	void add(unsigned short v, int amountOfBits = 16, int offset = 0);
	void add(int v, int amountOfBits = 32, int offset = 0);
	void add(unsigned int v, int amountOfBits = 32, int offset = 0);
	void add(long v, int amountOfBits = 64, int offset = 0);
	void add(unsigned long v, int amountOfBits = 64, int offset = 0);

	void add(char* v, int size);
	void add(unsigned char* v, int size);

	void setValues(char* v, int size);
	void setValues(unsigned char* v, int size);

	bool getBit(int index);
	int getBits(int indexStart, int indexEnd, bool reverse = false);

	void setBit(bool value, int index);
	
	std::vector<unsigned char> toBytes();

	int size();
	void clear();

	void setBitOrder(bool bitOrder);
	void setAddBitOrder(bool order);

private:
	std::vector<unsigned char> set = std::vector<unsigned char>();
	bool MSB = true;
	bool addBitOrder = LMSB;
	long bitNumber = 0;
};

