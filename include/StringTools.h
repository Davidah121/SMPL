#pragma once
#include<iostream>
#include<io.h>
#include<vector>
#include<fcntl.h>
#include "BinarySet.h"

#include <stdarg.h>
#include <initializer_list>

class StringTools
{
public:
	StringTools();
	~StringTools();

	static void init();

	static std::wstring toWideString(std::string text);

	static std::string toCString(std::wstring text);

	static std::vector<unsigned char> toUTF8(int c);
	static int utf8ToChar(std::vector<unsigned char> utf8Char);

	static int stringLength(char* text);
	static int stringLength(wchar_t* text);

	static int stringLength(const char* text);
	static int stringLength(const wchar_t* text);

	static bool equalsIgnoreCase(std::string a, std::string b);
	static bool equalsIgnoreCase(std::wstring a, std::wstring b);
	static bool isAlphaNumerial(char v, bool underScoreAllowed = false, bool dashAllowed = false);
	static bool isValidChar(int v);
	
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
	static std::vector<std::string> splitString(std::string s, std::string delim, bool removeEmpty=true);
	static std::vector<std::string> splitStringMultipleDeliminators(std::string s, std::string delim, bool removeEmpty=true);
	
	static std::vector<std::wstring> splitString(std::wstring s, const wchar_t delim, bool removeEmpty=true);
	static std::vector<std::wstring> splitString(std::wstring s, std::wstring delim, bool removeEmpty=true);
	static std::vector<std::wstring> splitStringMultipleDeliminators(std::wstring s, std::wstring delim, bool removeEmpty=true);

	static int toInt(std::string s);
	static long toLong(std::string s);
	static double toDouble(std::string s);
	static float toFloat(std::string s);

	static int toInt(std::wstring s);
	static long toLong(std::wstring s);
	static double toDouble(std::wstring s);
	static float toFloat(std::wstring s);

	static std::wstring getWideString();
	static std::string getString();

	static char getChar();
	static wchar_t getWideChar();
	static int getInt();

	static void findLongestMatch(std::string base, std::string match, int* length, int* index);
	static void findLongestMatch(char* base, int sizeOfBase, char* match, int sizeOfMatch, int* length, int* index);

	static std::string formatString(std::string text, ...);
	static std::wstring formatWideString(std::wstring text, ...);

	
	static void print(std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		std::wstring finalString = formatWideStringInternal( StringTools::toWideString(fmt), args);
		va_end(args);

		std::wcout << finalString;
	}

	static void println(std::string fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		std::wstring finalString = formatWideStringInternal( StringTools::toWideString(fmt), args);
		va_end(args);

		std::wcout << finalString << L"\n";
	}

	static void print(std::wstring fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		std::wstring finalString = formatWideStringInternal(fmt, args);
		va_end(args);

		std::wcout << finalString;
	}

	static void println(std::wstring fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		std::wstring finalString = formatWideStringInternal(fmt, args);
		va_end(args);

		std::wcout << finalString << L"\n";
	}

	static const wchar_t lineBreak;

	static void reroutOutput(std::wofstream file);

private:
	static std::string formatStringInternal(std::string text, va_list orgArgs);
	static std::wstring formatWideStringInternal(std::wstring text, va_list orgArgs);

	static bool hasInit;
};

