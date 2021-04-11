#pragma once

#include<vector>
#include<fstream>
#include "Object.h"

class SimpleFile : public Object
{
public:
	static const char READ = 0x00;
	static const char WRITE = 0x01;
	static const char WRITE_APPEND = 0x02;

	static const char LINE_BREAK_1 = 13;
	static const char LINE_BREAK_2 = 10;

	static const char ASCII = 0x00;
	static const char WIDECHAR = 0x10;
	static const char UTF8 = 0x20;
	

	SimpleFile(std::wstring filename, char type);
	SimpleFile(std::string filename, char type);

	~SimpleFile();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	//Read functions
	char readByte();
	int readInt();
	wchar_t readWideChar();
	std::string readString();
	std::wstring readWideString();

	int readUTF8Char();

	std::vector<std::string> readFullFileString();
	std::vector<std::wstring> readFullFileStringWide();

	std::vector<unsigned char> readFullFileAsBytes();

	//Write functions
	void writeByte(char c);
	void writeWideChar(wchar_t c);

	void writeBytes(unsigned char* data, int size);

	void writeString(std::string line);
	void writeWideString(std::wstring line);

	void writeLineBreak();

	//File functions
	bool isOpen();
	void close();
	bool isEndOfFile();
	std::wstring getFileName();

	int getSize();
	int getBytesLeft();

private:
	void init(std::wstring filename, char type);
	int getChar();

	std::wstring wideFileName;
	char type=0;
	char dataType=0;
	std::fstream* file;
	int size;
	
};

