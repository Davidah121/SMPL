#include "SimpleFile.h"
#include "StringTools.h"
#include "SimpleDir.h"

#ifdef _WIN32
#define ftell64(file) _ftelli64(file)
#else
#define ftell64(file) ftello64(file)
#endif

namespace smpl
{
	SimpleFile::SimpleFile()
	{

	}

	SimpleFile::SimpleFile(File file, char type)
	{
		init(file.getFullFileName(), type);
	}

	SimpleFile::SimpleFile(SimpleFile&& other) noexcept
	{
		filename = other.filename;
		type = other.type;
		dataType = other.dataType;
		size = other.size;
		cFile = other.cFile;
		other.cFile = nullptr;
	}
    void SimpleFile::operator=(SimpleFile&& other) noexcept
	{
		filename = other.filename;
		type = other.type;
		dataType = other.dataType;
		size = other.size;
		cFile = other.cFile;
		other.cFile = nullptr;
	}

	void SimpleFile::init(std::string filename, char type)
	{
		this->type = type&0x0F;
		dataType = type&0xF0;
		size = 0;
		switch (this->type)
		{
		case SimpleFile::READ:
			
			//try to open
			cFile = fopen(filename.c_str(), "rb");
			this->filename = filename;
			if(cFile)
			{
				fseek(cFile, 0, SEEK_END);
				size = ftell64(cFile);
				rewind(cFile);
				if(dataType == WIDECHAR)
				{
					int c1 = fgetc(cFile);
					int c2 = fgetc(cFile);
					if(c1 != 0xFE || c2 != 0xFF)
					{
						//no utf-16 header. Put back the bytes
						fseek(cFile, -2, SEEK_CUR);
					}
				}
				else if(dataType == UTF8)
				{
					int c1 = fgetc(cFile);
					int c2 = fgetc(cFile);
					int c3 = fgetc(cFile);
					if(c1 != 0xEF || c2 != 0xBB || c3 != 0xBF)
					{
						//no utf-8 header. Put back the bytes
						fseek(cFile, -3, SEEK_CUR);
					}
				}
			}
			else
			{
				#ifdef USE_EXCEPTIONS
					//not open.  could be file access or the file may not exist.
					throw SimpleFile::FileOpenErrorException();
				#endif
			}
			
			break;
		case SimpleFile::WRITE:
			//
			cFile = fopen(filename.c_str(), "wb");
			this->filename = filename;

			#ifdef USE_EXCEPTIONS
			if(!this->file->is_open())
			{
				//not open. could be file access or the file may not exist.
				throw SimpleFile::FileOpenErrorException();
			}
			#endif

			break;
		case SimpleFile::WRITE_APPEND:
			//
			cFile = fopen(filename.c_str(), "ab");
			this->filename = filename;
			if(cFile)
			{
				size = ftell64(cFile);
			}
			else
			{
				#ifdef USE_EXCEPTIONS
				//not open. could be file access or the file may not exist.
				throw SimpleFile::FileOpenErrorException();
				#endif
			}

			break;
		default:
			break;
		}
	}

	void SimpleFile::init(FILE* filePointer, std::string name, char type)
	{
		this->type = type&0x0F;
		dataType = type&0xF0;
		size = 0;
		cFile = filePointer;
		this->filename = name;
	}

	FILE* SimpleFile::getFilePointer()
	{
		return cFile;
	}

	SimpleFile::~SimpleFile()
	{
		if(isOpen())
		{
			close();
		}
	}

	int SimpleFile::readInt()
	{
		if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
		{
			return fgetc(cFile);
		}
		else
		{
			//File is not opened for reading

			#ifdef USE_EXCEPTIONS

			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}
		return 0;
	}

	char SimpleFile::readByte()
	{
		if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
		{
			return fgetc(cFile);
		}
		else
		{
			//File is not opened for reading
			#ifdef USE_EXCEPTIONS

			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}
		return '\0';
	}

	char SimpleFile::peekByte()
	{
		if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
		{
			int c = fgetc(cFile);
			ungetc(c, cFile);
			return c;
		}
		else
		{
			//File is not opened for reading
			#ifdef USE_EXCEPTIONS

			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}
		return '\0';
	}
	
	size_t SimpleFile::readBytes(char* buffer, size_t size)
	{
		if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
		{
			return fread(buffer, sizeof(char), size, cFile);
		}
		else
		{
			//File is not opened for reading
			#ifdef USE_EXCEPTIONS

			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}
		return 0;
	}

	wchar_t toWideChar(unsigned char p, unsigned char p2)
	{
		wchar_t n;

		n = p;
		n = n << 8;
		n += p2;

		return n;
	}

	wchar_t SimpleFile::readWideChar()
	{
		wchar_t c;
		if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
		{
			if(fread(&c, sizeof(wchar_t), 1, cFile) == 2)
				return c;
			else
				return -1;
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			
			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}
		return L'\0';
	}

	int SimpleFile::getChar()
	{
		if(dataType==SimpleFile::ASCII)
			return readByte();
		else if(dataType==SimpleFile::WIDECHAR)
			return readWideChar();
		else if(dataType==SimpleFile::UTF8)
			return readUTF8Char();
		
		return 0;
	}

	std::string SimpleFile::readString()
	{
		std::string p = "";
		int c = '\0';

		if (isOpen() && type == SimpleFile::READ)
		{
			while (!isEndOfFile())
			{
				c = getChar();
				if(isEndOfFile())
				{
					break;
				}

				if (c != LINE_BREAK_1)
				{
					p += c;
				}
				else
				{
					c = getChar();
					if(isEndOfFile())
					{
						break;
					}

					if (c == LINE_BREAK_2)
					{
						//Hit a line break
						break;
					}
					else
					{
						//Hit something else.
						//Fix later I guess.
						p += LINE_BREAK_1;
						p += c;
					}
					
				}
				
			}
		}
		else
		{
			//File is not opened for reading
			#ifdef USE_EXCEPTIONS
			
			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}

		return p;
	}

	std::wstring SimpleFile::readWideString()
	{
		std::wstring p = L"";
		wchar_t c = L'\0';
		// unsigned char p1; //Not used
		// unsigned char p2; //Not used
		wchar_t n;

		if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
		{
			while (!isEndOfFile())
			{
				c = getChar();

				if(isEndOfFile())
				{
					break;
				}

				if (c != LINE_BREAK_1)
				{
					p += c;
				}
				else
				{
					n = getChar();

					if(isEndOfFile())
					{
						break;
					}

					if (n == LINE_BREAK_2)
					{
						//Hit a line break
						break;
					}
					else
					{
						//Hit something else.
						//Fix later I guess.
						p += c;
						p += n;
					}
				}
			}

		}
		else
		{
			//File is not opened for reading
			#ifdef USE_EXCEPTIONS
			
			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}

		return p;
	}

	int SimpleFile::readUTF8Char()
	{
		if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
		{
			std::vector<unsigned char> chars = std::vector<unsigned char>();
			unsigned char c1 = fgetc(cFile);

			if(isEndOfFile())
				return -1;
			
			if(c1>>7 == 0)
			{
				return c1;
			}
			else if( ((c1 >> 5) & 0x01) == 0)
			{
				chars.push_back(c1);
				c1 = fgetc(cFile);
				
				if(isEndOfFile())
					return -1;

				if( (c1 >> 7) == 1 && ((c1 >> 6) & 0x01) == 0)
				{
					//valid utf8
					chars.push_back( c1 );
					return StringTools::utf8ToChar(chars);
				}
				else
				{
					//invalid
					ungetc(c1, cFile);
					return chars[0];
				}
			}
			else if( ((c1 >> 4) & 0x01) == 0)
			{
				chars.push_back(c1);
				chars.push_back( (unsigned char)fgetc(cFile) );
				if(isEndOfFile())
					return -1;

				chars.push_back( (unsigned char)fgetc(cFile) );
				if(isEndOfFile())
					return -1;
				return StringTools::utf8ToChar(chars);
			}
			else if( ((c1 >> 3) & 0x01) == 0)
			{
				chars.push_back(c1);
				chars.push_back( (unsigned char)fgetc(cFile) );
				if(isEndOfFile())
					return -1;

				chars.push_back( (unsigned char)fgetc(cFile) );
				if(isEndOfFile())
					return -1;

				chars.push_back( (unsigned char)fgetc(cFile) );
				if(isEndOfFile())
					return -1;
				return StringTools::utf8ToChar(chars);
			}
			else
			{
				//not utf8
				return c1;
			}
		}
		else
		{
			//File is not opened for reading
			#ifdef USE_EXCEPTIONS
			
			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}
		return 0;
	}

	std::vector<std::string> SimpleFile::readFullFileString()
	{
		std::vector<std::string> info = std::vector<std::string>();

		if(isOpen() && type == SimpleFile::READ)
		{
			while (!isEndOfFile())
			{
				info.push_back(readString());
			}
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			
			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}

		return info;
	}

	std::vector<std::wstring> SimpleFile::readFullFileStringWide()
	{
		std::vector<std::wstring> info = std::vector<std::wstring>();

		// Sometimes contains a header that reads FF FE. 
		// unsigned char c1 = file->get();
		// unsigned char c2 = file->get();

		if(isOpen() && type == SimpleFile::READ)
		{
			while (!isEndOfFile())
			{
				//read
				info.push_back(readWideString());
			}
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			
			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}


		return info;
	}

	std::vector<unsigned char> SimpleFile::readFullFileAsBytes()
	{
		std::vector<unsigned char> info = std::vector<unsigned char>();

		if(isOpen() && type == SimpleFile::READ)
		{
			while (true)
			{
				//read
				unsigned char c = fgetc(cFile);

				if(!feof(cFile))
					info.push_back(c);
				else
					break;
			}
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			
			//not open for reading. could be file access or the file may not exist.
			throw SimpleFile::FileReadException();

			#endif
		}

		return info;
	}

	void SimpleFile::writeByte(char c)
	{
		if (isOpen())
		{
			if (type == WRITE || type == WRITE_APPEND)
			{
				fputc(c, cFile);
			}
			else
			{
				//File is not opened for writing
				#ifdef USE_EXCEPTIONS
				
				throw SimpleFile::FileWriteException();

				#endif
			}
		}
		else
		{
			//File is not opened
			#ifdef USE_EXCEPTIONS

			//not open for writing. could be file access or the file may not exist.
			throw SimpleFile::FileWriteException();

			#endif
		}
	}

	void SimpleFile::writeWideChar(wchar_t c)
	{
		if (isOpen())
		{
			if (type == WRITE || type == WRITE_APPEND)
			{
				fwrite(&c, sizeof(wchar_t), 1, cFile);
			}
			else
			{
				//File is not opened for writing
				#ifdef USE_EXCEPTIONS
				
				throw SimpleFile::FileWriteException();

				#endif
			}
		}
		else
		{
			//File is not opened
			#ifdef USE_EXCEPTIONS

			//not open for writing. could be file access or the file may not exist.
			throw SimpleFile::FileWriteException();

			#endif
		}
	}

	void SimpleFile::writeUTF8Char(int c)
	{
		if (isOpen())
		{
			if (type == WRITE || type == WRITE_APPEND)
			{
				std::vector<unsigned char> bytes = StringTools::toUTF8(c);
				fwrite(bytes.data(), sizeof(unsigned char), bytes.size(), cFile);
			}
			else
			{
				//File is not opened for writing
				#ifdef USE_EXCEPTIONS
				
				throw SimpleFile::FileWriteException();

				#endif
			}
		}
		else
		{
			//File is not opened
			#ifdef USE_EXCEPTIONS

			//not open for writing. could be file access or the file may not exist.
			throw SimpleFile::FileWriteException();

			#endif
		}
	}

	void SimpleFile::writeBytes(unsigned char* data, size_t size)
	{
		if (isOpen())
		{
			if (type == WRITE || type == WRITE_APPEND)
			{
				fwrite(data, sizeof(unsigned char), size, cFile);
			}
			else
			{
				//File is not opened for writing
				#ifdef USE_EXCEPTIONS
				
				throw SimpleFile::FileWriteException();

				#endif
			}
		}
		else
		{
			//File is not opened
			#ifdef USE_EXCEPTIONS

			//not open for writing. could be file access or the file may not exist.
			throw SimpleFile::FileWriteException();

			#endif
		}
	}

	void SimpleFile::writeString(std::string line)
	{
		if (isOpen())
		{
			if (type == WRITE || type == WRITE_APPEND)
			{
				if(dataType == ASCII)
				{
					fwrite(line.c_str(), sizeof(char), line.size(), cFile);
				}
				else if(dataType == WIDECHAR)
				{
					for (size_t i = 0; i < line.size(); i++)
					{
						fputc(0, cFile);
						fputc(line[i], cFile);
					}
				}
				else
				{
					for (size_t i = 0; i < line.size(); i++)
					{
						std::vector<unsigned char> bytes = StringTools::toUTF8(line[i]);
						fwrite(bytes.data(), sizeof(unsigned char), bytes.size(), cFile);
					}
				}
			}
			else
			{
				//File is not opened for writing
				#ifdef USE_EXCEPTIONS
				
				throw SimpleFile::FileWriteException();

				#endif
			}
		}
		else
		{
			//File is not opened
			#ifdef USE_EXCEPTIONS

			//not open for writing. could be file access or the file may not exist.
			throw SimpleFile::FileWriteException();

			#endif
		}
	}

	void SimpleFile::writeWideString(std::wstring line)
	{
		if (isOpen())
		{
			if (type == WRITE || type == WRITE_APPEND)
			{
				if(dataType == ASCII)
				{
					for (size_t i = 0; i < line.size(); i++)
					{
						fputc(line[i] & 0xFF, cFile);
					}
				}
				else if(dataType == WIDECHAR)
				{
					fwrite(line.c_str(), sizeof(wchar_t), line.size(), cFile);
				}
				else
				{
					for (size_t i = 0; i < line.size(); i++)
					{
						std::vector<unsigned char> bytes = StringTools::toUTF8(line[i]);
						fwrite(bytes.data(), sizeof(unsigned char), bytes.size(), cFile);
					}
				}
			}
			else
			{
				//File is not opened for writing
				#ifdef USE_EXCEPTIONS
				
				throw SimpleFile::FileWriteException();

				#endif
			}
		}
		else
		{
			//File is not opened
			#ifdef USE_EXCEPTIONS

			//not open for writing. could be file access or the file may not exist.
			throw SimpleFile::FileWriteException();

			#endif
		}
	}

	void SimpleFile::writeLineBreak()
	{
		if (isOpen())
		{
			if (type == WRITE || type == WRITE_APPEND)
			{
				if(dataType == ASCII || dataType == UTF8)
				{
					fwrite("\r\n", sizeof(char), 2, cFile);
				}
				else
				{
					fwrite(L"\r\n", sizeof(wchar_t), 2, cFile);
				}
			}
			else
			{
				//File is not opened for writing
				#ifdef USE_EXCEPTIONS
				
				throw SimpleFile::FileWriteException();

				#endif
			}
		}
		else
		{
			//File is not opened
			#ifdef USE_EXCEPTIONS

			//not open for writing. could be file access or the file may not exist.
			throw SimpleFile::FileWriteException();

			#endif
		}
	}

	bool SimpleFile::isOpen()
	{
		return cFile != nullptr;
	}

	void SimpleFile::close()
	{
		if(cFile != nullptr)
		{
			fclose(cFile);
			cFile = nullptr;
		}
	}

	bool SimpleFile::isEndOfFile()
	{
		if(cFile != nullptr)
			return feof(cFile);
		
		return true;
	}

	std::string SimpleFile::getFileName()
	{
		return filename;
	}

	size_t SimpleFile::getSize()
	{
		return size;
	}

	size_t SimpleFile::getBytesLeft()
	{
		if(isOpen() && type == READ)
		{
			return (size - ftell64(cFile));
		}
		else
		{
			return 0;
		}
	}
	
	void SimpleFile::seek(size_t index)
	{
		if(isOpen())
			fseek(cFile, index, SEEK_SET);
	}
	
} //NAMESPACE glib END