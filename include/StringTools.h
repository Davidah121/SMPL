#pragma once
#include<iostream>
#include<io.h>
#include<vector>
#include<fcntl.h>
#include "BinarySet.h"

#include <stdarg.h>
#include <initializer_list>

namespace glib
{

	class StringTools
	{
	public:
		/**
		 * @brief Initializes the StringTools class.
		 * 		Specifically, it sets up the console to accept wstring data properly.
		 * 		Not required if the console is not needed.
		 * 
		 * 		None of the wrapper functions to get input from the console will work if this function is not called. 
		 * 			Ex: getInt() or getString()
		 */
		static void init();

		/**
		 * @brief Converts any string to a std::wstring
		 * 
		 * @tparam T 
		 * @param text 
		 * @return std::wstring 
		 */
		template<typename T>
		static std::wstring toWideString(std::basic_string<T> text)
		{
			std::wstring finalText = L"";
			for (int i = 0; i < text.size(); i++)
			{
				finalText += (wchar_t)text[i];
			}

			return finalText;
		}

		/**
		 * @brief Converts any string to a std::string
		 * 
		 * @tparam T 
		 * @param text 
		 * @return std::string 
		 */
		template<typename T>
		static std::string toCString(std::basic_string<T> text)
		{
			std::string finalText = "";
			for (int i = 0; i < text.size(); i++)
			{
				finalText += (char)text[i];
			}

			return finalText;
		}

		/**
		 * @brief Converts a character to a valid UTF8 character.
		 * 		May use more than one byte for one UTF8 character.
		 * 
		 * @param c 
		 * @return std::vector<unsigned char> 
		 */
		static std::vector<unsigned char> toUTF8(int c);

		/**
		 * @brief Converts the bytes in a valid UTF8 character to an integer representation.
		 * 
		 * @param utf8Char 
		 * @return int 
		 */
		static int utf8ToChar(std::vector<unsigned char> utf8Char);

		/**
		 * @brief Returns the string length of a character array.
		 * 
		 * @param text 
		 * @return int 
		 */
		static int stringLength(char* text);

		/**
		 * @brief Returns the string length of a character array.
		 * 
		 * @param text 
		 * @return int 
		 */
		static int stringLength(const char* text);

		/**
		 * @brief Returns the string length of a wide character array.
		 * 
		 * @param text 
		 * @return int 
		 */
		static int stringLength(wchar_t* text);

		/**
		 * @brief Returns the string length of a wide character array.
		 * 
		 * @param text 
		 * @return int 
		 */
		static int stringLength(const wchar_t* text);

		/**
		 * @brief Returns if the strings are equal to each other ignoring the case of the character.
		 * 
		 * @tparam T 
		 * @param a 
		 * @param b 
		 * @return true 
		 * @return false 
		 */
		template<typename T>
		static bool equalsIgnoreCase(std::basic_string<T> a, std::basic_string<T> b)
		{
			if(a.size() == b.size())
			{
				for(int i=0; i<a.size(); i++)
				{
					if( toupper(a[i]) != toupper(b[i]))
						return false;
				}
			}
			else
			{
				return false;
			}
			
			return true;
		}

		/**
		 * @brief Returns if the character is apart of the alphabet or is a number.
		 * 		Special characters like '-' and '_' can be added to the check.
		 * 		Only checks in the ASCII space.
		 * 
		 * @param v 
		 * @param underScoreAllowed 
		 * 		Default is false
		 * @param dashAllowed 
		 * 		Default is false
		 * @return true 
		 * @return false 
		 */
		static bool isAlphaNumerial(char v, bool underScoreAllowed = false, bool dashAllowed = false);

		/**
		 * @brief Returns if the number is valid character in the ASCII space.
		 * 
		 * @param v 
		 * @return true 
		 * @return false 
		 */
		static bool isValidChar(int v);
		
		/**
		 * @brief Converts the base10 value to its base16 value represented as a character.
		 * 		Ex:
		 * 		0 -> '0'
		 * 		2 -> '2'
		 * 		10 -> 'A'
		 * 		15 -> 'F'
		 * @param v 
		 * 		Valid values are from [0,15]
		 * @return char 
		 */
		static char base10ToBase16(char v);

		/**
		 * @brief Converts the base16 value represented as a character to its base10 value represented as an integer.
		 * 		Ex:
		 * 		'0' -> 0
		 * 		'2' -> 2
		 * 		'A' -> 10
		 * 		'F' -> 15
		 * @param v 
		 * 		Valid values are from ['0', '9'] and ['A', 'F']
		 * @return int 
		 * 		Returns -1 if unsuccessful
		 */
		static int base16ToBase10(char v);

		/**
		 * @brief Converts the value to a Hexidecimal string.
		 * 
		 * @tparam T 
		 * @param value 
		 * @return char* 
		 */
		template<class T>
		static char* toHexString(T value)
		{
			int size = sizeof(T) * 2;

			char* hexString = new char[size+1];
			int maxVal = 4*sizeof(T) - 4;

			unsigned long long convertedValue = (unsigned long long)value;
			
			for(int i=0; i<size; i++)
			{
				hexString[size-i-1] = base10ToBase16((convertedValue >> (i*4)) & 0xF);
			}

			hexString[size] = '\0';

			return hexString;
		}

		/**
		 * @brief Converts the value to a Binary string.
		 * 
		 * @tparam T 
		 * @param value 
		 * @param bits 
		 * 		How many bits to grab.
		 * @param LMSB 
		 * 		Left Most Significant Bit
		 * 		Default is true
		 * @return char* 
		 */
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

		/**
		 * @brief Splits a string using a single character deliminator.
		 * 
		 * @param s 
		 * @param delim 
		 * @param removeEmpty 
		 * 		Whether to keep empty splits or remove them.
		 * 		Default is true.
		 * @return std::vector<std::string> 
		 */
		static std::vector<std::string> splitString(std::string s, const char delim, bool removeEmpty=true);

		/**
		 * @brief Splits a string using a string as the deliminator.
		 * 
		 * @param s 
		 * @param delim 
		 * @param removeEmpty 
		 * 		Whether to keep empty splits or remove them.
		 * 		Default is true.
		 * @return std::vector<std::string> 
		 */
		static std::vector<std::string> splitString(std::string s, std::string delim, bool removeEmpty=true);

		/**
		 * @brief Splits a string using multiple single character deliminators.
		 * 
		 * @param s 
		 * @param delim 
		 * 		Each character in the string is a separate deliminator
		 * @param removeEmpty 
		 * 		Whether to keep empty splits or remove them.
		 * 		Default is true.
		 * @return std::vector<std::string> 
		 */
		static std::vector<std::string> splitStringMultipleDeliminators(std::string s, std::string delim, bool removeEmpty=true);
		
		/**
		 * @brief Splits a string using a single character deliminator.
		 * 
		 * @param s 
		 * @param delim 
		 * @param removeEmpty 
		 * 		Whether to keep empty splits or remove them.
		 * 		Default is true.
		 * @return std::vector<std::wstring> 
		 */
		static std::vector<std::wstring> splitString(std::wstring s, const wchar_t delim, bool removeEmpty=true);

		/**
		 * @brief Splits a string using a string as the deliminator.
		 * 
		 * @param s 
		 * @param delim 
		 * @param removeEmpty 
		 * 		Whether to keep empty splits or remove them.
		 * 		Default is true.
		 * @return std::vector<std::wstring> 
		 */
		static std::vector<std::wstring> splitString(std::wstring s, std::wstring delim, bool removeEmpty=true);

		/**
		 * @brief Splits a string using multiple single character deliminators.
		 * 
		 * @param s 
		 * @param delim 
		 * @param removeEmpty 
		 * 		Whether to keep empty splits or remove them.
		 * 		Default is true.
		 * @return std::vector<std::wstring> 
		 */
		static std::vector<std::wstring> splitStringMultipleDeliminators(std::wstring s, std::wstring delim, bool removeEmpty=true);

		/**
		 * @brief Converts a string into an integer.
		 * 
		 * @param s 
		 * @return int 
		 */
		static int toInt(std::string s);

		/**
		 * @brief Converts a string into an integer.
		 * 
		 * @param s 
		 * @return int 
		 */
		static int toInt(std::wstring s);

		/**
		 * @brief Converts a string into a long.
		 * 
		 * @param s 
		 * @return long 
		 */
		static long toLong(std::string s);

		/**
		 * @brief Converts a string into a long.
		 * 
		 * @param s 
		 * @return long 
		 */
		static long toLong(std::wstring s);

		/**
		 * @brief Converts a string into a double.
		 * 
		 * @param s 
		 * @return double 
		 */
		static double toDouble(std::string s);

		/**
		 * @brief Converts a string into a double.
		 * 
		 * @param s 
		 * @return double 
		 */
		static double toDouble(std::wstring s);

		/**
		 * @brief Converts a string into a float.
		 * 
		 * @param s 
		 * @return float 
		 */
		static float toFloat(std::string s);

		/**
		 * @brief Converts a string into a float.
		 * 
		 * @param s 
		 * @return float 
		 */
		static float toFloat(std::wstring s);

		/**
		 * @brief Gets a wide string from the console using the users input.
		 * 		Halts until the user presses enter.
		 * 
		 * @return std::wstring 
		 */
		static std::wstring getWideString();

		/**
		 * @brief Gets a string from the console using the users input.
		 * 		Halts until the user presses enter.
		 * 
		 * @return std::string 
		 */
		static std::string getString();

		/**
		 * @brief Gets a character from the console using the users input.
		 * 		Halts until the user presses enter.
		 * 
		 * @return char 
		 */
		static char getChar();

		/**
		 * @brief Gets a wide character from the console using the users input.
		 * 		Halts until the user presses enter.
		 * 
		 * @return wchar_t 
		 */
		static wchar_t getWideChar();

		/**
		 * @brief Gets an integer from the console using the users input.
		 * 		Halts until the user presses enter.
		 * 
		 * @return int 
		 */
		static int getInt();

		/**
		 * @brief Finds the longest match in the string base.
		 * 
		 * @param base 
		 * @param match 
		 * @param index 
		 * 		A pointer to an int that will record the start index of the longest match
		 * @param length 
		 * 		A pointer to an int that will record the length of the longest match
		 */
		static void findLongestMatch(std::string base, std::string match, int* index, int* length);

		/**
		 * @brief Finds the longest match in the string base.
		 * 
		 * @tparam T 
		 * @param base 
		 * @param baseSize 
		 * @param match 
		 * @param matchSize 
		 * @param index 
		 * 		A pointer to an int that will record the start index of the longest match
		 * @param length 
		 * 		A pointer to an int that will record the length of the longest match
		 */
		template<class T>
		static void findLongestMatch(T* base, int baseSize, T* match, int matchSize, int* index, int* length);

		/**
		 * @brief Formats a string like printf() would but converts std::wstring to std::string to avoid errors.
		 * 		For formating information, check https://www.cplusplus.com/reference/cstdio/printf/
		 * @param text 
		 * @param ... 
		 * 		The list of parameters to use.
		 * @return std::string 
		 */
		static std::string formatString(std::string text, ...);

		/**
		 * @brief Formats a string like wprintf() would but converts std::string to std::wstring to avoid errors.
		 * 		For formating information, check https://www.cplusplus.com/reference/cstdio/printf/
		 * @param text 
		 * @param ... 
		 * 		The list of parameters to use.
		 * @return std::wstring 
		 */
		static std::wstring formatWideString(std::wstring text, ...);

		/**
		 * @brief Converts a keyboard press to the Ascii key it represents. 
		 * 		This is valid in the Windows OS for the English language but may not be valid in other languages and other OS
		 * 
		 * @param keyVal 
		 * @param shiftHeld 
		 * @return int 
		 */
		static int convertKeyToAscii(int keyVal, bool shiftHeld);
		
		/**
		 * @brief Prints information to the console using the same format as printf()
		 * 
		 * @param fmt 
		 * @param ... 
		 */
		static void print(std::string fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			std::wstring finalString = formatWideStringInternal( StringTools::toWideString(fmt), args);
			va_end(args);

			std::wcout << finalString;
		}

		/**
		 * @brief Prints information to the console using the same format as printf().
		 * 		Also prints a line break at the end.
		 * 
		 * @param fmt 
		 * @param ... 
		 */
		static void println(std::string fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			std::wstring finalString = formatWideStringInternal( StringTools::toWideString(fmt), args);
			va_end(args);

			std::wcout << finalString << L"\n";
		}

		/**
		 * @brief Prints information to the console using the same format as printf()
		 * 
		 * @param fmt 
		 * @param ... 
		 */
		static void print(std::wstring fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			std::wstring finalString = formatWideStringInternal(fmt, args);
			va_end(args);

			std::wcout << finalString;
		}

		/**
		 * @brief Prints information to the console using the same format as printf().
		 * 		Also prints a line break at the end.
		 * 
		 * @param fmt 
		 * @param ... 
		 */
		static void println(std::wstring fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			std::wstring finalString = formatWideStringInternal(fmt, args);
			va_end(args);

			std::wcout << finalString << L"\n";
		}

		static const wchar_t lineBreak;

		/**
		 * @brief Rerouts the output of the print functions to somewhere else such as a file.
		 * 		(Not implemented)
		 * 
		 * @param file 
		 */
		static void reroutOutput(std::wofstream file);

		

	private:
		static std::string formatStringInternal(std::string text, va_list orgArgs);
		static std::wstring formatWideStringInternal(std::wstring text, va_list orgArgs);

		/**
		 * @brief Pre Processing for the KMP string matching algorithm.
		 * 
		 * @tparam T 
		 * @param array 
		 * @param size 
		 * @return std::vector<int> 
		 */
		template<class T>
		static std::vector<int> longestPrefixSubstring(T* array, int size);

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

} //NAMESPACE glib END