#include "SimpleFile.h"
#include "StringTools.h"

char* const SimpleFile::LINE_BREAK = new char[2]{ (char)13, (char)10 };
char const SimpleFile::READ = 0;
char const SimpleFile::WRITE = 1;
char const SimpleFile::WRITE_APPEND = 2;

const Class* SimpleFile::myClass = new Class("SimpleFile", {Object::myClass});
const Class* SimpleFile::getClass()
{
	return SimpleFile::myClass;
}

SimpleFile::SimpleFile(std::wstring filename, char type)
{
	this->type = type;
	this->size = 0;

	switch (type)
	{
	case SimpleFile::READ:
		//
		this->file = new std::fstream(filename, std::fstream::in | std::fstream::ate | std::fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new std::fstream(filename, std::fstream::in | std::fstream::binary);

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

		this->file = new std::fstream(filename, std::fstream::in | std::fstream::binary);
		this->wideFileName = filename;
		break;
	}

}

SimpleFile::SimpleFile(std::string filename, char type)
{
	this->type = type;
	this->size = 0;

	switch (type)
	{
	case SimpleFile::READ:
		//
		this->file = new std::fstream(filename, std::fstream::in | std::fstream::ate | std::fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new std::fstream(filename, std::fstream::in | std::fstream::binary);

		this->wideFileName = StringTools::toWideString(filename);
		break;
	case SimpleFile::WRITE:
		//
		this->file = new std::fstream(filename, std::fstream::out | std::fstream::binary);
		this->wideFileName = StringTools::toWideString(filename);
		break;
	case SimpleFile::WRITE_APPEND:
		//
		this->file = new std::fstream(filename, std::fstream::in | std::fstream::ate | std::fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new std::fstream(filename, std::fstream::out | std::fstream::app | std::fstream::binary);
		this->wideFileName = StringTools::toWideString(filename);
		break;
	default:
		this->file = new std::fstream(filename, std::fstream::in | std::fstream::ate | std::fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new std::fstream(filename, std::fstream::in | std::fstream::binary);
		this->wideFileName = StringTools::toWideString(filename);
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
		p2 = file->get();
		p1 = file->get();

		return toWideChar(p1, p2);
	}
	return L'\0';
}

const char * SimpleFile::readLine()
{
	std::string p = "";
	char c = '\0';

	if (isOpen() && type == SimpleFile::READ && !isEndOfFile())
	{
		while (!isEndOfFile())
		{
			c = file->get();

			if (c != LINE_BREAK[0])
			{
				p += c;
			}
			else
			{
				if (!isEndOfFile())
				{
					c = file->get();
					if (c == LINE_BREAK[1])
					{
						//Hit a line break
						break;
					}
					else
					{
						//Hit something else.
						//Fix later I guess.
						p += 13;
						p += c;
					}
				}
				else
				{
					p += c;
				}
			}
		}

		return p.c_str();
	}
	else
	{
		//File is not opened for reading
	}

	return nullptr;
}

const wchar_t * SimpleFile::readWideLine()
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
			p2 = file->get();
			p1 = file->get();
			c = toWideChar(p1, p2);

			if (c != LINE_BREAK[0])
			{
				p += c;
			}
			else
			{
				if (!isEndOfFile())
				{
					p2 = file->get();
					p1 = file->get();
					n = toWideChar(p1, p2);

					if (n == LINE_BREAK[1])
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
				else
				{
					p += c;
				}
			}
		}

		return p.c_str();
	}
	else
	{
		//File is not opened for reading
	}

	return nullptr;
}

std::string SimpleFile::readString()
{
	std::string p = "";
	char c = '\0';

	if (isOpen() && type == SimpleFile::READ)
	{
		while (!isEndOfFile())
		{
			c = file->get();

			if(!isEndOfFile())
			{
				if (c != LINE_BREAK[0])
				{
					p += c;
				}
				else
				{
					c = file->get();
					if(!isEndOfFile())
					{
						if (c == LINE_BREAK[1])
						{
							//Hit a line break
							break;
						}
						else
						{
							//Hit something else.
							//Fix later I guess.
							p += 13;
							p += c;
						}
					}
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
			p2 = file->get();
			p1 = file->get();
			c = toWideChar(p1, p2);

			if (c != LINE_BREAK[0])
			{
				p += c;
			}
			else
			{
				if (!isEndOfFile())
				{
					p2 = file->get();
					p1 = file->get();
					n = toWideChar(p1, p2);

					if (n == LINE_BREAK[1])
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
				else
				{
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

std::vector<const char*> SimpleFile::readFullFile()
{
	std::vector<const char*> info = std::vector<const char*>();

	while (!isEndOfFile())
	{
		info.push_back(readLine());
	}

	return info;
}

std::vector<const wchar_t*> SimpleFile::readFullFileWide()
{
	//Read the first two bytes to determine if the file is a unicode file
	std::vector<const wchar_t*> info = std::vector<const wchar_t*>();

	unsigned char c1 = file->get();
	unsigned char c2 = file->get();

	if (c1 == 0xFF && c2 == 0xFE)
	{
		//Is unicode, proceed to read
		while (!isEndOfFile())
		{
			//read
			info.push_back(readWideLine());
		}
	}

	return std::vector<const wchar_t*>();
}

std::vector<std::string> SimpleFile::readFullFileString()
{
	std::vector<std::string> info = std::vector<std::string>();

	while (!isEndOfFile())
	{
		info.push_back(readString());
	}

	return info;
}

std::vector<std::wstring> SimpleFile::readFullFileStringWide()
{
	//Read the first two bytes to determine if the file is a unicode file
	std::vector<std::wstring> info = std::vector<std::wstring>();

	unsigned char c1 = file->get();
	unsigned char c2 = file->get();

	if (c1 == 0xFF && c2 == 0xFE)
	{
		//Is unicode, proceed to read
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

	int i=0;
	while (i < size)
	{
		//read
		info.push_back(file->get());
		i++;
	}

	return info;
}

void SimpleFile::writeByte(char c)
{
	if (isOpen())
	{
		if (type == WRITE || type == WRITE_APPEND)
		{
			file->put(c);
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
			file->put(c);
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

void SimpleFile::writeLine(char * line)
{
	if (isOpen())
	{
		if (type == WRITE || type == WRITE_APPEND)
		{
			for (int i = 0; i < strlen(line); i++)
			{
				file->put(line[i]);
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

void SimpleFile::writeWideLine(wchar_t * line)
{
	if (isOpen())
	{
		if (type == WRITE || type == WRITE_APPEND)
		{
			for (int i = 0; i < wcslen(line); i++)
			{
				file->put(line[i]);
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
			for (int i = 0; i < size; i++)
			{
				file->put(data[i]);
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
			for (int i = 0; i < line.size(); i++)
			{
				file->put(line[i]);
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
			for (int i = 0; i < line.size(); i++)
			{
				file->put(line[i]);
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
			file->put(LINE_BREAK[0]);
			file->put(LINE_BREAK[1]);
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
