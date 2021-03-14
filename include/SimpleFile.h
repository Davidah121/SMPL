#pragma once

#include<vector>
#include<fstream>
#include "Object.h"

class SimpleFile : public Object
{
public:
	static const char READ = 0;
	static const char WRITE = 1;
	static const char WRITE_APPEND = 2;

	static const char LINE_BREAK_1 = 13;
	static const char LINE_BREAK_2 = 10;

	SimpleFile(std::wstring filename, char type);
	SimpleFile(std::string filename, char type);

	~SimpleFile();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	//Read functions
	char readByte();
	int readInt();
	wchar_t readWideChar(); //Unicode capable
	const char* readLine();
	const wchar_t* readWideLine();
	std::string readString();
	std::wstring readWideString();

	std::vector<const char*> readFullFile();
	std::vector<const wchar_t*> readFullFileWide();

	std::vector<std::string> readFullFileString();
	std::vector<std::wstring> readFullFileStringWide();

	std::vector<unsigned char> readFullFileAsBytes();

	//Write functions
	void writeByte(char c);
	void writeWideChar(wchar_t c); //Unicode capable
	void writeLine(char* line);
	void writeWideLine(wchar_t* line); //Unicode capable

	void writeBytes(unsigned char* data, int size);

	void writeString(std::string line);
	void writeWideString(std::wstring line); //Unicode capable

	void writeLineBreak();

	//File functions
	bool isOpen();
	void close();
	bool isEndOfFile();
	std::wstring getFileName();

	int getSize();
	int getBytesLeft();

private:
	std::wstring wideFileName; //Unicode capable
	char type;
	std::fstream* file;
	int size;
	
};

