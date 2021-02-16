#pragma once
#include<stdarg.h>
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

	static bool equalsIgnoreCase(std::string a, std::string b);
	static bool equalsIgnoreCase(std::wstring a, std::wstring b);
	static bool isAlphaNumerial(char v, bool underScoreAllowed = false, bool dashAllowed = false);

	static char charToBase16(char v);
	static int base16ToBase10(char v);

	template<class T>
	static char* toHexString(T value)
	{
		int size = sizeof(T) * 2;

		char* hexString = new char[size+1];
		int maxVal = 4*sizeof(T) - 4;

		unsigned long long convertedValue = (unsigned long long)value;
		
		for(int i=0; i<size; i++)
		{
			hexString[size-i-1] = charToBase16((convertedValue >> (i*4)) & 0xF);
		}

		hexString[size] = '\0';

		return hexString;
	}

	template<class T>
	static char* toBinaryString(T value, int bits, bool LMSB = true)
	{
		int size = sizeof(T);
		char* binString = new char[bits+1];

		BinarySet b = BinarySet();
		b.setBitOrder(LMSB);
		b.setValues((char*)&value, size);

		for(int i=0; i<bits; i++)
		{
			binString[bits-i-1] = (b.getBit(i)==false) ? '0':'1';
		}
		binString[bits] = '\0';

		return binString;
	}

	static std::vector<std::string> splitString(std::string s, const char delim, bool removeEmpty=true);
	static std::vector<std::string> splitString(std::string s, const char* delim, bool removeEmpty=true);
	static std::vector<std::string> splitStringMultipleDeliminators(std::string s, const char* delim, bool removeEmpty=true);
	
	static std::vector<std::wstring> splitString(std::wstring s, const wchar_t delim, bool removeEmpty=true);
	static std::vector<std::wstring> splitString(std::wstring s, const wchar_t* delim, bool removeEmpty=true);
	static std::vector<std::wstring> splitStringMultipleDeliminators(std::wstring s, const wchar_t* delim, bool removeEmpty=true);

	static int toInt(std::string s);
	static long toLong(std::string s);
	static double toDouble(std::string s);
	static float toFloat(std::string s);

	static std::wstring getWideString();
	static std::string getString();

	static char getChar();
	static wchar_t getWideChar();
	static int getInt();

	static void findLongestMatch(std::string base, std::string match, int* length, int* index);
	static void findLongestMatch(char* base, int sizeOfBase, char* match, int sizeOfMatch, int* length, int* index);

	static void print(std::string text, ...)
	{
		va_list args;
		va_start(args, text);
		vprintf(text.c_str(), args);
		va_end(args);
	}

	static void println(std::string text, ...)
	{
		va_list args;
		va_start(args, text);
		vprintf((text+'\n').c_str(), args);
		va_end(args);
	}
	
	static void print(const char* text, ...)
	{
		va_list args;
		va_start(args, text);
		vprintf(text, args);
		va_end(args);
	}

	static void println(const char* text, ...)
	{
		std::string k = text;
		k+='\n';

		va_list args;
		va_start(args, text);
		vprintf(k.c_str(), args);
		va_end(args);
	}

	static void print(std::wstring text, ...)
	{
		va_list args;
		va_start(args, text);
		vwprintf(text.c_str(), args);
		va_end(args);
	}

	static void println(std::wstring text, ...)
	{
		va_list args;
		va_start(args, text);
		vwprintf((text+lineBreak).c_str(), args);
		va_end(args);
	}
	
	static void print(const wchar_t* text, ...)
	{
		va_list args;
		va_start(args, text);
		vwprintf(text, args);
		va_end(args);
	}

	static void println(const wchar_t* text, ...)
	{
		std::wstring k = text;
		k+=lineBreak;
		
		va_list args;
		va_start(args, text);
		vwprintf(k.c_str(), args);
		va_end(args);
	}

	static const wchar_t lineBreak;

	static void reroutOutput(std::wofstream file);

private:
	static bool hasInit;

};

