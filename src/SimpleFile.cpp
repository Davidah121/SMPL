#include "SimpleFile.h"
#include "StringTools.h"

const Class* SimpleFile::myClass = new Class("SimpleFile", {Object::myClass});
const Class* SimpleFile::getClass()
{
	return SimpleFile::myClass;
}

SimpleFile::SimpleFile(std::wstring filename, char type)
{
	init(filename, type);
}

SimpleFile::SimpleFile(std::string filename, char type)
{
	init( StringTools::toWideString(filename), type);
}

void SimpleFile::init(std::wstring filename, char type)
{
	this->type = type&0x0F;
	this->dataType = type&0xF0;
	this->size = 0;

	switch (this->type)
	{
	case SimpleFile::READ:
		//
		this->file = new std::fstream(filename, std::fstream::in | std::fstream::ate | std::fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new std::fstream(filename, std::fstream::in | std::fstream::binary);

		//remove signifier
		if(dataType==WIDECHAR)
		{
			int c1 = file->get();
			int c2 = file->get();

			if(c1 == 0xFE && c2 == 0xFF)
			{
				//utf-16 file
			}
			else
			{
				//no header
				file->putback(c2);
				file->putback(c1);
			}
		}
		else if(dataType==UTF8)
		{
			int c1 = file->get();
			int c2 = file->get();
			int c3 = file->get();

			if(c1 == 0xEF && c2 == 0xBB && c3 == 0xBF)
			{
				//utf-8 file
			}
			else
			{
				//no header
				file->putback(c3);
				file->putback(c2);
				file->putback(c1);
			}
		}
		this->wideFileName = filename;
		break;
	case SimpleFile::WRITE:
		//
		this->file = new std::fstream(filename, std::fstream::out | std::fstream::binary);
		this->wideFileName = filename;
		break;
	case SimpleFile::WRITE_APPEND:
		//
		this->file = new std::fstream(filename, std::fstream::in | std::fstream::ate | std::fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new std::fstream(filename, std::fstream::out | std::fstream::app | std::fstream::binary);
		this->wideFileName = filename;
		break;
	default:
		this->file = new std::fstream(filename, std::fstream::in | std::fstream::ate | std::fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->wideFileName = filename;
		break;
	}
}

SimpleFile::~SimpleFile()
{
}

int SimpleFile::readInt()
{
	if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
	{
		return file->get();
	}
	else
	{
		//File is not opened for reading
	}
	return 0;
}

char SimpleFile::readByte()
{
	if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
	{
		return file->get();
	}
	else
	{
		//File is not opened for reading
	}
	return '\0';
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
	unsigned char p1;
	unsigned char p2;
	if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
	{
		p1 = file->get();
		p2 = file->get();

		return toWideChar(p1, p2);
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
	}

	return p;
}

std::wstring SimpleFile::readWideString()
{
	std::wstring p = L"";
	wchar_t c = L'\0';
	unsigned char p1;
	unsigned char p2;
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
	}

	return p;
}

int SimpleFile::readUTF8Char()
{
	if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
	{
		std::vector<unsigned char> chars = std::vector<unsigned char>();
		unsigned char c1 = file->get();
		
		if(c1>>7 == 0)
		{
			return c1;
		}
		else if( ((c1 >> 5) & 0x01) == 0)
		{
			chars.push_back(c1);
			c1 = file->get();
			if( (c1 >> 7) == 1 && ((c1 >> 6) & 0x01) == 0)
			{
				//valid utf8
				chars.push_back( c1 );
				return StringTools::utf8ToChar(chars);
			}
			else
			{
				//invalid
				file->putback(c1);
				return chars[0];
			}
		}
		else if( ((c1 >> 4) & 0x01) == 0)
		{
			chars.push_back(c1);
			chars.push_back( (unsigned char)file->get() );
			chars.push_back( (unsigned char)file->get() );
			return StringTools::utf8ToChar(chars);
		}
		else if( ((c1 >> 3) & 0x01) == 0)
		{
			chars.push_back(c1);
			chars.push_back( (unsigned char)file->get() );
			chars.push_back( (unsigned char)file->get() );
			chars.push_back( (unsigned char)file->get() );
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
	}
	return 0;
}

std::vector<std::string> SimpleFile::readFullFileString()
{
	std::vector<std::string> info = std::vector<std::string>();

	if(isOpen())
	{
		while (!isEndOfFile())
		{
			info.push_back(readString());
		}
	}

	return info;
}

std::vector<std::wstring> SimpleFile::readFullFileStringWide()
{
	std::vector<std::wstring> info = std::vector<std::wstring>();

	// Sometimes contains a header that reads FF FE. 
	// unsigned char c1 = file->get();
	// unsigned char c2 = file->get();

	if(isOpen())
	{
		while (!isEndOfFile())
		{
			//read
			info.push_back(readWideString());
		}
	}


	return info;
}

std::vector<unsigned char> SimpleFile::readFullFileAsBytes()
{
	std::vector<unsigned char> info = std::vector<unsigned char>();

	if(isOpen())
	{
		int i=0;
		while (i < size)
		{
			//read
			info.push_back(file->get());
			i++;
		}
	}

	return info;
}

void SimpleFile::writeByte(char c)
{
	if (isOpen())
	{
		if (type == WRITE || type == WRITE_APPEND)
		{
			if(dataType == ASCII)
			{
				file->put(c);
			}
			else if(dataType == WIDECHAR)
			{
				file->put(0);
				file->put(c);
			}
			else
			{
				std::vector<unsigned char> bytes = StringTools::toUTF8(c);
				file->write((char*)bytes.data(), bytes.size());
			}
		}
		else
		{
			//File is not opened for writing
		}
	}
	else
	{
		//File is not opened
	}
}

void SimpleFile::writeWideChar(wchar_t c)
{
	if (isOpen())
	{
		if (type == WRITE || type == WRITE_APPEND)
		{
			if(dataType == ASCII)
			{
				file->put(c);
			}
			else if(dataType == WIDECHAR)
			{
				unsigned char c1 = c>>8;
				unsigned char c2 = c & 0xFF;
				file->put(c1);
				file->put(c2);
			}
			else
			{
				std::vector<unsigned char> bytes = StringTools::toUTF8(c);
				file->write((char*)bytes.data(), bytes.size());
			}
		}
		else
		{
			//File is not opened for writing
		}
	}
	else
	{
		//File is not opened
	}
}

void SimpleFile::writeBytes(unsigned char* data, int size)
{
	if (isOpen())
	{
		if (type == WRITE || type == WRITE_APPEND)
		{
			if(dataType == ASCII)
			{
				file->write((char*)data, size);
			}
			else if(dataType == WIDECHAR)
			{
				for(int i=0; i<size; i++)
				{
					file->put( 0 );
					file->put( data[i] );
				}
			}
			else
			{
				for(int i=0; i<size; i++)
				{
					std::vector<unsigned char> bytes = StringTools::toUTF8(data[i]);
					file->write((char*)bytes.data(), bytes.size());
				}
			}
		}
		else
		{
			//File is not opened for writing
		}
	}
	else
	{
		//File is not opened
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
				file->write(line.c_str(), line.size());
			}
			else if(dataType == WIDECHAR)
			{
				for (int i = 0; i < line.size(); i++)
				{
					file->put(0);
					file->put(line[i]);
				}
			}
			else
			{
				for (int i = 0; i < line.size(); i++)
				{
					std::vector<unsigned char> bytes = StringTools::toUTF8(line[i]);
					file->write((char*)bytes.data(), bytes.size());
				}
			}
		}
		else
		{
			//File is not opened for writing
		}
	}
	else
	{
		//File is not opened
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
				for (int i = 0; i < line.size(); i++)
				{
					file->put(line[i]);
				}
			}
			else if(dataType == WIDECHAR)
			{
				for (int i = 0; i < line.size(); i++)
				{
					unsigned char c1 = line[i]>>8;
					unsigned char c2 = line[i] & 0xFF;
					file->put(c1);
					file->put(c2);
				}
			}
			else
			{
				for (int i = 0; i < line.size(); i++)
				{
					std::vector<unsigned char> bytes = StringTools::toUTF8(line[i]);
					file->write((char*)bytes.data(), bytes.size());
				}
			}
		}
		else
		{
			//File is not opened for writing
		}
	}
	else
	{
		//File is not opened
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
				file->put(LINE_BREAK_1);
				file->put(LINE_BREAK_2);
			}
			else
			{
				file->put(0);
				file->put(LINE_BREAK_1);
				
				file->put(0);
				file->put(LINE_BREAK_2);
			}
		}
		else
		{
			//File is not opened for writing
		}
	}
	else
	{
		//File is not opened
	}
}

bool SimpleFile::isOpen()
{
	return file->is_open();
}

void SimpleFile::close()
{
	file->close();
}

bool SimpleFile::isEndOfFile()
{
	return file->eof();
}

std::wstring SimpleFile::getFileName()
{
	return wideFileName;
}

int SimpleFile::getSize()
{
	return size;
}

int SimpleFile::getBytesLeft()
{
	return (int)(size - file->tellg());
}
