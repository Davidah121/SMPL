#pragma once
#include<iostream>
#include<stdio.h>
#include<string.h>
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
			std::wstring finalText;
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
			std::string finalText;

			for (int i = 0; i < text.size(); i++)
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
			for (int i = 0; i < text.size(); i++)
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
			for (int i = 0; i < text.size(); i++)
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