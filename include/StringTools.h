#pragma once
#include<iostream>
#include<io.h>
#include<vector>
#include<fcntl.h>

class StringTools
{
public:
	StringTools();
	~StringTools();

	static void init();

	static wchar_t* toWideString(char* text);
	static wchar_t* toWideString(const char* text);
	static std::wstring toWideString(std::string text);

	static char* toCString(wchar_t* text);
	static char* toCString(const wchar_t* text);
	static std::string toCString(std::wstring text);

	static int stringLength(char* text);
	static int stringLength(wchar_t* text);

	static int stringLength(const char* text);
	static int stringLength(const wchar_t* text);

	static char valueToBase16(char v);

	template<class T>
	static char* toHexString(T value)
	{
		int size = sizeof(T) * 2;

		char* hexString = new char[size+1];
		int maxVal = 4*sizeof(T) - 4;

		unsigned long long convertedValue = (unsigned long long)value;
		
		for(int i=0; i<size; i++)
		{
			hexString[size-i-1] = valueToBase16((convertedValue >> (i*4)) & 0xF);
		}

		hexString[size] = '\0';

		return hexString;
	}

	static std::vector<std::string> splitString(std::string s, const char delim);
	static std::vector<std::string> splitString(std::string s, const char* delim);

	static int toInt(std::string s);
	static long toLong(std::string s);
	static double toDouble(std::string s);
	static float toFloat(std::string s);

	static std::wstring getWideString();
	static std::string getString();

	static char getChar();
	static wchar_t getWideChar();
	static int getInt();

	template<class T>
	static void print(T text)
	{
		if(hasInit)
			StringTools::out << text;
	}

	template<class T>
	static void println(T text)
	{
		if(hasInit)
			StringTools::out << text << StringTools::lineBreak;
	}

	static std::wostream out;
	static std::wistream in;
	static std::wostream err;

	static const wchar_t lineBreak;

	static void reroutOutput(std::wofstream file);

private:
	static bool hasInit;

};

