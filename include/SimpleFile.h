#pragma once

#include<vector>
#include<fstream>
#include "Object.h"

using namespace std;

class SimpleFile : public Object
{
public:
	static char const READ;
	static char const WRITE;
	static char const WRITE_APPEND;

	static char* const LINE_BREAK;

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
	string readString();
	wstring readWideString();

	vector<const char*> readFullFile();
	vector<const wchar_t*> readFullFileWide();

	vector<string> readFullFileString();
	vector<wstring> readFullFileStringWide();

	vector<unsigned char> readFullFileAsBytes();

	//Write functions
	void writeByte(char c);
	void writeWideChar(wchar_t c); //Unicode capable
	void writeLine(char* line);
	void writeWideLine(wchar_t* line); //Unicode capable

	void writeBytes(unsigned char* data, int size);

	void writeString(string line);
	void writeWideString(wstring line); //Unicode capable

	void writeLineBreak();

	//File functions
	bool isOpen();
	void close();
	bool isEndOfFile();
	wstring getFileName();

	int getSize();
	int getBytesLeft();

private:
	std::wstring wideFileName; //Unicode capable
	char type;
	fstream* file;
	int size;
	
};

