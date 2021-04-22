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

	template<class T>
	static std::vector<int> longestPrefixSubstring(T* array, int size);

	static void findLongestMatch(std::string base, std::string match, int* index, int* length);

	template<class T>
	static void findLongestMatch(T* base, int baseSize, T* match, int matchSize, int* index, int* length);

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

	template<class T>
	static void KMP(T* base, int baseSize,T* match, int matchSize, int* index, int* length);

	template<class T>
	static void NaivePatternSearch(T* base, int baseSize, T* match, int matchSize, int* index, int* length);

	static bool hasInit;
};

template<class T>
inline std::vector<int> StringTools::longestPrefixSubstring(T* array, int size)
{
	std::vector<int> lps = std::vector<int>(size);

	int m = 0;
	lps[0] = 0;

	for(int pos=1; pos<size; pos++)
	{
		while(m>0 && array[pos] != array[m])
		{
			m = lps[m-1];
		}

		if(array[pos] == array[m])
		{
			m++;
		}

		lps[pos] = m;
	}
	
	for(int i=0; i<size; i++)
	{
		lps[i] -= 1;
	}

	return lps;
}

template<class T>
inline void StringTools::KMP(T* base, int baseSize,T* match, int matchSize, int* index, int* length)
{
	//preprocess match
	std::vector<int> lps = longestPrefixSubstring(match, matchSize);

	int i = 0;
	int j = -1;

	int currMaxLength = 0;
	
	while(i < baseSize)
	{
		if(base[i] == match[j+1])
		{
			j++;
			i++;

			if((j+1)>=currMaxLength)
			{
				currMaxLength = j+1;
				*index = i-currMaxLength;
				*length = currMaxLength;
			}
		}
		else
		{
			if(j>=0)
				j = lps[j];
			else
				i++;
		}

		if(currMaxLength==matchSize)
		{
			//found match
			break;
		}
	}

}

template<class T>
inline void StringTools::NaivePatternSearch(T* base, int baseSize, T* match, int matchSize, int* index, int* length)
{
	if(length!=nullptr && index!=nullptr)
	{
		int maxVal = 0;
		int indexOfMax = 0;

		int x = 0;
		int y = 0;

		int currSize = 0;
		int currStartIndex = -1;

		int nextPossibleIndex = -1;

		T* sB = base;
		T* sM = match;

		char startValue = match[0];
		
		while(x < baseSize)
		{
			if(*sB == *sM)
			{
				if(currStartIndex!=-1)
				{
					if(*sB == startValue)
					{
						nextPossibleIndex = x; 
					}
				}

				if(currStartIndex==-1)
					currStartIndex = x;
				
				currSize++;
				sM++;

				if(currSize >= matchSize)
				{
					maxVal = currSize;
					indexOfMax = currStartIndex;
					break;
				}
			}
			else
			{
				if(currSize >= maxVal)
				{
					maxVal = currSize;
					indexOfMax = currStartIndex;
				}

				if(nextPossibleIndex>0)
				{
					x = nextPossibleIndex;
					sB = base + nextPossibleIndex;
				}

				currSize = 0;
				currStartIndex = -1;
				nextPossibleIndex = -1;

				sM = match;
			}

			sB++;
			x++;
		}
		
		if(currSize >= maxVal)
		{
			maxVal = currSize;
			indexOfMax = currStartIndex;
		}
		
		*length = maxVal;
		*index = indexOfMax;
	}
}

template<class T>
inline void StringTools::findLongestMatch(T* base, int baseSize, T* match, int matchSize, int* index, int* length)
{
	StringTools::KMP(base, baseSize, match, matchSize, index, length);
}