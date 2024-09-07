#pragma once
#include "BuildOptions.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <fcntl.h>

#include <stdarg.h>
#include <initializer_list>

namespace smpl
{

	class DLL_OPTION StringTools
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
			std::wstring finalText;
			for (size_t i = 0; i < text.size(); i++)
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
			std::string finalText;

			for (size_t i = 0; i < text.size(); i++)
			{
				finalText += (char)text[i];
			}

			return finalText;
		}

		/**
		 * @brief Converts a wide string to a valid UTF8 string
		 * 
		 * @param s 
		 * @return std::string 
		 */
		static std::string toUTF8String(std::wstring s)
		{
			std::string finalText;

			for (wchar_t& c : s)
			{
				std::vector<unsigned char> values = StringTools::toUTF8(c);
				for(unsigned char& v : values)
				{
					finalText += (char)v;
				}
			}

			return finalText;
		}

		/**
		 * @brief Converts an int string to a valid UTF8 string
		 * 
		 * @param s 
		 * @return std::string 
		 */
		static std::string toUTF8String(std::vector<int> s)
		{
			std::string finalText;

			for (int& c : s)
			{
				std::vector<unsigned char> values = StringTools::toUTF8(c);
				for(unsigned char& v : values)
				{
					finalText += (char)v;
				}
			}

			return finalText;
		}

		/**
		 * @brief Converts any string to lowercase
		 * 
		 * @tparam T 
		 * @param text 
		 * @return std::basic_string<T>
		 */
		template<typename T>
		static std::basic_string<T> toLowercase(std::basic_string<T> text)
		{
			std::basic_string<T> finalText;
			for (size_t i = 0; i < text.size(); i++)
			{
				finalText += tolower(text[i]);
			}

			return finalText;
		}

		/**
		 * @brief Converts any string to uppercase
		 * 
		 * @tparam T 
		 * @param text 
		 * @return std::basic_string<T>
		 */
		template<typename T>
		static std::basic_string<T> toUppercase(std::basic_string<T> text)
		{
			std::basic_string<T> finalText = "";
			for (size_t i = 0; i < text.size(); i++)
			{
				finalText += toupper(text[i]);
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
		 * @brief Converts the bytes in a std::string or any utf8 compliant string into
		 * 		a set of integers for easier parsing.
		 * 
		 * @param validUTF8String 
		 * @return std::vector<int> 
		 */
		static std::vector<int> utf8ToIntString(std::string validUTF8String);

		/**
		 * @brief Converts the bytes in a std::wstring into
		 * 		a set of integers for easier parsing.
		 * @param str 
		 * @return std::vector<int> 
		 */
		static std::vector<int> wideStringToIntString(std::wstring str);

		/**
		 * @brief Converts a base64 character into a number. Properly handles the url safe cases as well.
		 * 
		 * @param b 
		 * @return int 
		 */
		static int base64CharToNum(unsigned char b);

		/**
		 * @brief Converts a set of bytes into base64 encoding.
		 * 
		 * @param bytes 
		 * @param urlSafe 
		 * 		Replaces select values with others that can be put into a url.
		 * 			Specifically replaces '+' and '/'
		 * @return std::string 
		 */
		static std::string base64Encode(std::vector<unsigned char> bytes, bool urlSafe);

		/**
		 * @brief Converts a set of bytes into base64 encoding.
		 * 
		 * @param bytes 
		 * @param size
		 * @param urlSafe 
		 * 		Replaces select values with others that can be put into a url.
		 * 			Specifically replaces '+' and '/'
		 * @return std::string 
		 */
		static std::string base64Encode(unsigned char* bytes, size_t size, bool urlSafe);

		/**
		 * @brief Converts a base64 encoded string or set of bytes back into the original byte data.
		 * 
		 * @param bytes 
		 * @return std::vector<unsigned char> 
		 */
		static std::vector<unsigned char> base64Decode(std::vector<unsigned char> bytes);

		/**
		 * @brief Converts a base64 encoded string or set of bytes back into the original byte data.
		 * 
		 * @param bytes 
		 * @return std::vector<unsigned char> 
		 */
		static std::vector<unsigned char> base64Decode(std::string bytes);

		/**
		 * @brief Converts a base64 encoded string or set of bytes back into the original byte data.
		 * 
		 * @param bytes 
		 * @param size
		 * @return std::vector<unsigned char> 
		 */
		static std::vector<unsigned char> base64Decode(unsigned char* bytes, size_t size);

		/**
		 * @brief Encodes the data to be URL safe.
		 * 		This replaces spaces and unallowed characters with a hexidecimal value.
		 * 		For the query section, the characters are encoded differently.
		 * 
		 * @param str 
		 * @return std::string 
		 */
		static std::string urlEncode(std::string str);

		/**
		 * @brief Decodes the data from a URL to a normal string.
		 * 		This undoes the Encoded version to add back spaces and other characters.
		 * 
		 * @param str 
		 * @return std::string 
		 */
		static std::string urlDecode(std::string str);

		
		/**
		 * @brief Attempts to translate Environment Variables in
		 * 		the string to be a valid.
		 * 		
		 * @param n 
		 * @return std::string 
		 */
		static std::string translateEnvironmentVariables(std::string n);

		/**
		 * @brief Performs a bitwise left rotate on the data type.
		 * 
		 * @tparam T 
		 * @param n 
		 * @param d 
		 * @return T 
		 */
		template<typename T>
		static T leftRotate(T n, unsigned int d)
		{
			return (n << d) | (n >> ((sizeof(T)*8) - d));
		}

		/**
		 * @brief Performs a bitwise right rotate on the data type.
		 * 
		 * @tparam T 
		 * @param n 
		 * @param d 
		 * @return T 
		 */
		template<typename T>
		static T rightRotate(T n, unsigned int d)
		{
			return (n >> d) | (n << ((sizeof(T)*8) - d));
		}

		/**
		 * @brief Swaps the order of the bytes in a short (16 bits).
		 * 		It is equivalent to a left or right rotate by 8 bits.
		 * @param v 
		 * @return short 
		 */
		static short byteSwap(short v);

		/**
		 * @brief Swaps the order of the bytes in an integer (32 bits).
		 * 		Example:
		 * 			0x11223344 -> 0x44332211
		 * @param v 
		 * @return int 
		 */
		static int byteSwap(int v);

		/**
		 * @brief Swaps the order of the bytes in a size_t or long (64 bits).
		 * 
		 * @param v 
		 * @return size_t 
		 */
		static size_t byteSwap(size_t v);

		/**
		 * @brief Swaps the order of the bytes where the size is 8bit, 16bit, 32bit, or 64bit.
		 * 		All other sizes return the same value. Assumes that the value can be cast into a short, int, and size_t
		 * 
		 * @tparam T 
		 * @param v 
		 * @return T 
		 */
		template<typename T>
		static T byteSwap(T v)
		{
			switch(sizeof(T))
			{
				case 1:
					return v;
				case 2:
					return (T)byteSwap((short)v);
				case 4:
					return (T)byteSwap((int)v);
				case 8:
					return (T)byteSwap((size_t)v);
				default:
					return v;
			}
			return v;
		}
		
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
				for(size_t i=0; i<a.size(); i++)
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
		static std::string toHexString(T value)
		{
			int size = sizeof(T) * 2;
			std::string hexString;
			for(int i=0; i<size+2; i++)
			{
				hexString += '0';
			}

			hexString[0] = '0';
			hexString[1] = 'x';
			
			char* startOfHexValues = &hexString[2];
			unsigned long long convertedValue = (unsigned long long)value;
			
			for(size_t i=0; i<size; i++)
			{
				startOfHexValues[size-i-1] = base10ToBase16((convertedValue >> (i*4)) & 0xF);
			}

			return hexString;
		}

		/**
		 * @brief Converts a string into a size_t. Assumes that the value is unsigned
		 * 		and fits into a size_t.
		 * 
		 * @param v 
		 * @return size_t 
		 */
		static size_t fromHexString(std::string v)
		{
			//check for 0x at the front
			size_t result = 0;
			if(v.size() <= 0)
				return -1;

			for(size_t k=0; k<v.size(); k++)
			{
				if(v[k] == 'x')
				{
					if(v[k-1] == 0)
					{
						result = 0;
					}
				}
				else
				{
					int temp = base16ToBase10(v[k]);
					if(temp != -1)
					{
						result <<= 4;
						result += temp;
					}
					else
					{
						return -1;
					}
				}
			}
			
			return result;
		}

		/**
		 * @brief Converts the data to a Binary string.
		 * 		Expects a list of bytes.
		 * 
		 * @param data 
		 * 		The data to be converted.
		 * @param size
		 * 		The total number of bytes in data.
		 * 			typically using sizeof() is appropriate.
		 * @param bits 
		 * 		How many bits to grab.
		 * @param LMSB 
		 * 		Left Most Significant Bit
		 * 		Default is true
		 * @return std::string 
		 */
		static std::string toBinaryString(char* data, int size, int bits, bool LMSB = true);

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
		 * @brief Removes all white space from the string.
		 * 		There is an option to remove tabs as well.
		 * 
		 * @param originalStr 
		 * @param removeTabs 
		 * @param onlyLeadingAndTrailing
		 * 		Only removes the empty white space at the beginning and ending.
		 * 		Keeps whitespace between data.
		 * @return std::string 
		 */
		static std::string removeWhitespace(std::string originalStr, bool removeTabs = false, bool onlyLeadingAndTrailing = false);

		/**
		 * @brief Removes all white space from the string.
		 * 		There is an option to remove tabs as well.
		 * 
		 * @param originalStr 
		 * @param removeTabs 
		 * @param onlyLeadingAndTrailing
		 * 		Only removes the empty white space at the beginning and ending.
		 * 		Keeps whitespace between data.
		 * @return std::wstring 
		 */
		static std::wstring removeWhitespace(std::wstring originalStr, bool removeTabs = false, bool onlyLeadingAndTrailing = false);
		
		/**
		 * @brief Converts a string into an integer.
		 * 
		 * @param s 
		 * @return int 
		 */
		template<typename T>
		static int toInt(std::basic_string<T> s, bool* err = nullptr)
		{
			int ret = 0;
			try
			{
				ret = std::stoi(s.c_str());
				if(err != nullptr)
					*err = false;
			}
			catch(...)
			{
				if(err != nullptr)
					*err = true;
			}
			
			return ret;
		}

		/**
		 * @brief Converts a string into a long.
		 * 
		 * @param s 
		 * @return long 
		 */
		template<typename T>
		static long toLong(std::basic_string<T> s, bool* err = nullptr)
		{
			long ret = 0;
			try
			{
				ret = std::stol(s.c_str());
				if(err != nullptr)
					*err = false;
			}
			catch(...)
			{
				if(err != nullptr)
					*err = true;
			}
			
			return ret;
		}

		/**
		 * @brief Converts a string into a unsigned long long.
		 * 
		 * @param s 
		 * @return long 
		 */
		template<typename T>
		static unsigned long long toULongLong(std::basic_string<T> s, bool* err = nullptr)
		{
			unsigned long long ret = 0;
			try
			{
				ret = std::stoull(s.c_str());
				if(err != nullptr)
					*err = false;
			}
			catch(...)
			{
				if(err != nullptr)
					*err = true;
			}
			
			return ret;
		}

		/**
		 * @brief Converts a string into a double.
		 * 
		 * @param s 
		 * @return double 
		 */
		template<typename T>
		static double toDouble(std::basic_string<T> s, bool* err = nullptr)
		{
			double ret = 0;
			try
			{
				ret = std::stod(s.c_str());
				if(err != nullptr)
					*err = false;
			}
			catch(...)
			{
				if(err != nullptr)
					*err = true;
			}
			
			return ret;
		}

		/**
		 * @brief Converts a string into a float.
		 * 
		 * @param s 
		 * @return float 
		 */
		template<typename T>
		static float toFloat(std::basic_string<T> s, bool* err = nullptr)
		{
			float ret = 0;
			try
			{
				ret = std::stof(s.c_str());
				if(err != nullptr)
					*err = false;
			}
			catch(...)
			{
				if(err != nullptr)
					*err = true;
			}
			
			return ret;
		}

		/**
		 * @brief Converts the value into a string
		 * 
		 * @param k 
		 * @return std::string 
		 */
		static std::string toString(int k);
		static std::string toString(long k);
		static std::string toString(unsigned int k);
		static std::string toString(unsigned long k);
		static std::string toString(long long k);
		static std::string toString(unsigned long long k);
		static std::string toString(float k);
		static std::string toString(double k);

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
		 * @brief Get a string from the user while hiding the text.
		 * 		Useful if the user is entering sensitive data as it will not
		 * 		show as they are typing nor will it appear in the consoles input history.
		 * 
		 * @param showAsterisk
		 * 		If set to true, Asterisk (*) will be shown as the user types.
		 * 		Otherwise, nothing will be shown.
		 * @return std::string 
		 */
		static std::string getHiddenString(bool showAsterisk);

		/**
		 * @brief Get a wide string from the user while hiding the text.
		 * 		Useful if the user is entering sensitive data as it will not
		 * 		show as they are typing nor will it appear in the consoles input history.
		 * @param showAsterisk
		 * 		If set to true, Asterisk (*) will be shown as the user types.
		 * 		Otherwise, nothing will be shown.
		 * @return std::wstring 
		 */
		static std::wstring getHiddenWideString(bool showAsterisk);

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
		 * 		Only valid characters for numbers are allowed.
		 * 
		 * @return int 
		 */
		static int getInt();

		/**
		 * @brief Gets an float from the console using the users input.
		 * 		Halts until the user presses enter.
		 * 		Only valid characters for numbers are allowed.
		 * 
		 * @return float 
		 */
		static float getFloat();

		/**
		 * @brief Gets a character from the console using the users input.
		 * 		Does not wait for the user to press enter.
		 * 		Halts until a character is entered.
		 * 
		 * 		Note that some keys that are not characters will be treated as a character.
		 * 			Ex: arrow keys, delete, insertion, home
		 * 			A value of 0 or 0xE0 (224) will be returned in that case.
		 * 			Also note that 224 is a valid character in both unicode and the extended ascii table.
		 * 			There is no way to determine if it is an escape sequence without checking for the next
		 * 			input which will block if it was a valid character.
		 * 		
		 * 		Some odd things will happen when moving code to linux.
		 * 
		 * @param noFunctionKeys
		 * 		Does not count function keys when returning data.
		 * 		Will read the first valid character.
		 * 		Default value is true.
		 * @return char 
		 */
		static char getCharLessLock(bool noFunctionKeys = true);

		/**
		 * @brief Gets a wide character from the console using the users input.
		 * 		Does not wait for the user to press enter.
		 * 		Halts until a character is entered.
		 * 
		 * 		Note that some keys that are not characters will be treated as a character.
		 * 			Ex: arrow keys, delete, insertion, home
		 * 			A value of 0 or 0xE0 (224) will be returned in that case.
		 * 			Also note that 224 is a valid character in both unicode and the extended ascii table.
		 * 			There is no way to determine if it is an escape sequence without checking for the next
		 * 			input which will block if it was a valid character.
		 * 
		 * 		Some odd things will happen when moving code to linux.
		 * 
		 * @param noFunctionKeys
		 * 		Does not count function keys when returning data.
		 * 		Will read the first valid character.
		 * 		Default value is true.
		 * @return int 
		 */
		static int getWideCharLessLock(bool noFunctionKeys = true);

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
		 * @param base 
		 * @param baseSize 
		 * @param match 
		 * @param matchSize 
		 * @param index 
		 * 		A pointer to an int that will record the start index of the longest match
		 * @param length 
		 * 		A pointer to an int that will record the length of the longest match
		 */
		static void findLongestMatch(unsigned char* base, int baseSize, unsigned char* match, int matchSize, int* index, int* length);

		/**
		 * @brief Finds the pattern using the naive pattern matching algorithm.
		 * 		If the pattern does not appear in the data but only partially, finds the closest match.
		 * 		Runs in O(N*M).
		 * 
		 * @param base 
		 * @param baseSize 
		 * @param match 
		 * @param matchSize 
		 * @param index 
		 * @param length 
		 */
		static void findLongestMatchNaive(unsigned char* base, int baseSize, unsigned char* match, int matchSize, int* index, int* length);

		/**
		 * @brief Finds the pattern using the KMP matching algorithm.
		 * 		If the pattern does not appear in the data but only partially, finds the closest match.
		 * 		Runs in O(N + M).
		 * 
		 * @param base 
		 * @param baseSize 
		 * @param match 
		 * @param matchSize 
		 * @param index 
		 * @param length 
		 */
		static void findLongestMatchKMP(unsigned char* base, int baseSize, unsigned char* match, int matchSize, int* index, int* length);

		/**
		 * @brief Finds the pattern using a DFA matching algorithm.
		 * 		If the pattern does not appear in the data but only partially, finds the closest match.
		 * 		Runs in O(N + M).
		 * 
		 * @param base 
		 * @param baseSize 
		 * @param match 
		 * @param matchSize 
		 * @param index 
		 * @param length 
		 */
		static void findLongestMatchDFA(unsigned char* base, int baseSize, unsigned char* match, int matchSize, int* index, int* length);

		/**
		 * @brief Finds all patterns using the DFA matching algorithm.
		 * 		Runs in O(N + M).
		 * 
		 * @param base 
		 * @param baseSize 
		 * @param match 
		 * @param matchSize 
		 */
		static std::vector<int> findAllMatchDFA(unsigned char* base, int baseSize, unsigned char* match, int matchSize);

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
			std::string finalString = formatStringInternal( fmt, args);
			va_end(args);

			std::cout << finalString;
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
			std::string finalString = formatStringInternal(fmt, args);
			va_end(args);

			std::cout << finalString << std::endl;
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
			std::string finalString = formatStringInternal( StringTools::toUTF8String(fmt), args);
			va_end(args);

			std::cout << finalString;
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
			std::string finalString = formatStringInternal( StringTools::toUTF8String(fmt), args);
			va_end(args);

			std::cout << finalString << std::endl;
		}

		static const wchar_t lineBreak;

		/**
		 * @brief Clears the console display
		 * 
		 * @param clearScrollBuffer 
		 * 		If set to true, clears the console display and the history of information displayed.
		 * 		Does not clear the history of user input.
		 */
		static void clearConsole(bool clearScrollBuffer);

		/**
		 * @brief Moves the console cursor from its current position.
		 * 
		 * @param horizontal 
		 * 		Positive values move the cursor forward on a line.
		 * 		Negative values move the cursor backwards on a line.
		 * 		Note that the cursor stops moving when at the edge.
		 * @param vertical 
		 * 		Positive values move the cursor down.
		 * 		Negative values move the cursor up.
		 * 		Note that the cursor stops moving when at the edge.
		 * @param absolute
		 * 		If true, the horizontal and vertical values represent the absolute cursor position
		 * 		and not an offset.
		 * 		All negative values are set to 1
		 */
		static void moveConsoleCursor(int horizontal, int vertical, bool absolute = false);

		/**
		 * @brief Clears part of the current line in the console.
		 * 
		 * @param eraseFromCursor 
		 * 		If set to false, Clears the entire line and sets the cursor to the start of the line.
		 * 		Otherwise, clears to the right of the cursor only.
		 */
		static void eraseConsoleLine(bool eraseFromCursor);
		
		/**
		 * @brief Gets a string that sets the color for the console.
		 * 		It can change the text color and background color separately.
		 * 		This method is compliant with Basic ANSI escape codes which only supports
		 * 		8 colors.
		 * 		
		 * @return std::string 
		 */
		static std::string setConsoleColorStringAnsi(int foreground, int background);

		/**
		 * @brief Gets a string that sets the color for the console.
		 * 		Uses a 256 color palette
		 * 
		 * @return std::string 
		 */
		static std::string setConsoleColorString256();
		static std::string setConsoleColorStringTrueColor();
		
		/**
		 * @brief Rerouts the output of the print functions to somewhere else such as a file.
		 * 		Could also be a different console if desired.
		 * 
		 * @param file 
		 */
		static void reroutOutput(std::wstreambuf* file);

		/**
		 * @brief Rerouts the input of the get functions to somewhere else such as a file.
		 * 		Could also be a different console if desired.
		 * 
		 * @param file 
		 */
		static void reroutInput(std::wstreambuf* file);

		/**
		 * @brief Rerouts the error output of the printErr functions to somewhere else such as a file.
		 * 		Could also be a different console if desired.
		 * 
		 * @param file 
		 */
		static void reroutErrorOutput(std::wstreambuf* file);
		
		static void resetOutputInputStreams();

		

	private:

		static std::wstreambuf* inputBuffer;
		static std::wstreambuf* outputBuffer;
		static std::wstreambuf* errorBuffer;

		static std::string formatStringInternal(std::string format, va_list args);
		static std::wstring formatStringInternal(std::wstring format, va_list args);

		/**
		 * @brief Pre Processing for the KMP string matching algorithm.
		 * 
		 * @param array 
		 * @param size 
		 */
		static void longestPrefixSubstring(unsigned char* input, int size, int* output);

		/**
		 * @brief Computes a finite automaton for pattern matching.
		 * 
		 * @param input 
		 * @param size 
		 * @param output 
		 * 		1D array instead of 2D array.
		 * 		Must be of the size [ (inputSize+1)*256 ]
		 */
		static void computeMatchDFA(unsigned char* input, int size, int* output);

		static bool hasInit;
	};

} //NAMESPACE glib END