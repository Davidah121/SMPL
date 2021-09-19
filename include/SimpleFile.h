#pragma once

#include<vector>
#include<fstream>
#include "Object.h"
#include<exception>
#include "File.h"

namespace glib
{

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
		
		struct FileOpenErrorException : public std::exception
		{
			const char* what() const throw() { return "Error opening file"; }
		};

		struct FileReadException : public std::exception
		{
			const char* what() const throw() { return "File not opened for reading or another error occured while reading."; }
		};

		struct FileWriteException : public std::exception
		{
			const char* what() const throw() { return "File not opened for writing or another error occured while writing."; }
		};

		/**
		 * @brief Construct a new SimpleFile object
		 * 		The type specifies whether to read or write to a file.
		 * 		The type also specifies whether to use ASCII, UTF8, or UTF16
		 * 
		 * @param file 
		 * @param type 
		 * 		Follows the format ACCESS_TYPE | CHAR_TYPE
		 * 		Valid Access types are:
		 * 			READ
		 * 			WRITE
		 * 			WRITE_APPEND
		 * 		Valid Char types are:
		 * 			ASCII
		 * 			WIDECHAR
		 * 			UTF8
		 */
		SimpleFile(File file, char type);

		/**
		 * @brief Destroy the SimpleFile object
		 * 
		 */
		~SimpleFile();

		//Object and Class Stuff
		const Class* getClass();
		static const Class myClass;

		//Read functions
		/**
		 * @brief Reads a byte of data
		 * 
		 * 		If the file was not opened for reading
		 * 		and USE_EXCEPTIONS is defined, a FileReadException may be thrown.
		 * 
		 * @return char 
		 */
		char readByte();

		/**
		 * @brief Reads an byte of data as an int.
		 * 		Not sure why this is here.
		 *
		 * 		If the file was not opened for reading
		 * 		and USE_EXCEPTIONS is defined, a FileReadException may be thrown.
		 * @return int 
		 */
		int readInt();

		/**
		 * @brief Reads a wide character from the file.
		 * 
		 * 		If the file was not opened for reading
		 * 		and USE_EXCEPTIONS is defined, a FileReadException may be thrown.
		 * @return wchar_t 
		 */
		wchar_t readWideChar();

		/**
		 * @brief Reads a line from the file. Reads until a line break has occured.
		 * 		This function is affected by the character type the file was set as.
		 * 
		 * 		If the file was not opened for reading
		 * 		and USE_EXCEPTIONS is defined, a FileReadException may be thrown.
		 * @return std::string 
		 */
		std::string readString();

		/**
		 * @brief Reads a line from the file. Reads until a line break has occured.
		 * 		This function is affected by the character type the file was set as.
		 * 
		 * 		If the file was not opened for reading
		 * 		and USE_EXCEPTIONS is defined, a FileReadException may be thrown.
		 * @return std::wstring 
		 */
		std::wstring readWideString();

		/**
		 * @brief Reads a UTF8 character from the file.
		 * 
		 * 		If the file was not opened for reading
		 * 		and USE_EXCEPTIONS is defined, a FileReadException may be thrown.
		 * @return int 
		 */
		int readUTF8Char();

		/**
		 * @brief Reads the full file as a series of lines using the readString function.
		 * 		This function is affected by the character type the file was set as.
		 * 
		 * 		If the file was not opened for reading
		 * 		and USE_EXCEPTIONS is defined, a FileReadException may be thrown.
		 * @return std::vector<std::string> 
		 */
		std::vector<std::string> readFullFileString();

		/**
		 * @brief Reads the full file as a series of lines using the readWideString function.
		 * 		This function is affected by the character type the file was set as.
		 * 
		 * 		If the file was not opened for reading
		 * 		and USE_EXCEPTIONS is defined, a FileReadException may be thrown.
		 * @return std::vector<std::wstring> 
		 */
		std::vector<std::wstring> readFullFileStringWide();

		/**
		 * @brief Reads the full file as a series of bytes.
		 * 
		 * 		If the file was not opened for reading
		 * 		and USE_EXCEPTIONS is defined, a FileReadException may be thrown.
		 * @return std::vector<unsigned char> 
		 */
		std::vector<unsigned char> readFullFileAsBytes();

		//Write functions

		/**
		 * @brief Writes a byte to the file
		 * 		This function is affected by the character type the file was set as.
		 * 		
		 * 		If the file was not opened for writing
		 * 		and USE_EXCEPTIONS is defined, a FileWriteException may be thrown.
		 * @param c 
		 */
		void writeByte(char c);

		/**
		 * @brief Writes a wide char to the file
		 * 		This function is affected by the character type the file was set as.
		 * 		
		 * 		If the file was not opened for writing
		 * 		and USE_EXCEPTIONS is defined, a FileWriteException may be thrown.
		 * @param c 
		 */
		void writeWideChar(wchar_t c);

		/**
		 * @brief Writes a series of bytes to the file.
		 * 		This function is affected by the character type the file was set as.
		 * 		
		 * 		If the file was not opened for writing
		 * 		and USE_EXCEPTIONS is defined, a FileWriteException may be thrown.
		 * @param data
		 * 		The array of data.
		 * @param size
		 * 		The size of the data.
		 */
		void writeBytes(unsigned char* data, int size);

		/**
		 * @brief Writes a string to the file. Does not write a line break.
		 * 		This function is affected by the character type the file was set as.
		 * 		
		 * 		If the file was not opened for writing
		 * 		and USE_EXCEPTIONS is defined, a FileWriteException may be thrown.
		 * @param line
		 */
		void writeString(std::string line);

		/**
		 * @brief Writes a wide string to the file. Does not write a line break.
		 * 		This function is affected by the character type the file was set as.
		 * 		
		 * 		If the file was not opened for writing
		 * 		and USE_EXCEPTIONS is defined, a FileWriteException may be thrown.
		 * @param line
		 */
		void writeWideString(std::wstring line);

		/**
		 * @brief Writes a line break to the file.
		 * 		This function is affected by the character type the file was set as.
		 * 		
		 * 		If the file was not opened for writing
		 * 		and USE_EXCEPTIONS is defined, a FileWriteException may be thrown.
		 */
		void writeLineBreak();

		//File functions

		/**
		 * @brief Determines if the file is opened for operations.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool isOpen();

		/**
		 * @brief Closes the file for operations freeing memory and allowing other programs to 
		 * 		access the file.
		 * 
		 */
		void close();

		/**
		 * @brief Determines if the end of the file was reached.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool isEndOfFile();

		/**
		 * @brief Gets the name of the file opened
		 * 
		 * @return std::wstring 
		 */
		std::wstring getFileName();

		/**
		 * @brief Gets the Size of file in bytes.
		 * 
		 * @return int 
		 */
		int getSize();

		/**
		 * @brief Get the amount of bytes left to read in the file.
		 * 		Returns -1 if the file is not opened for reading.
		 * 
		 * @return int 
		 */
		int getBytesLeft();

	private:
		void init(std::wstring filename, char type);
		int getChar();

		std::wstring wideFileName;
		char type=0;
		char dataType=0;
		std::fstream* file = nullptr;
		int size;
		
	};

} //NAMESPACE glib END