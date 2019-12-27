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
		this->file = new fstream(filename, fstream::in | fstream::ate | fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new fstream(filename, fstream::in | fstream::binary);

		this->wideFileName = filename;
		break;
	case SimpleFile::WRITE:
		//
		this->file = new fstream(filename, fstream::out | fstream::binary);
		this->wideFileName = filename;
		break;
	case SimpleFile::WRITE_APPEND:
		//
		this->file = new fstream(filename, fstream::in | fstream::ate | fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new fstream(filename, fstream::out | fstream::app | fstream::binary);
		this->wideFileName = filename;
		break;
	default:
		this->file = new fstream(filename, fstream::in | fstream::ate | fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new fstream(filename, fstream::in | fstream::binary);
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
		this->file = new fstream(filename, fstream::in | fstream::ate | fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new fstream(filename, fstream::in | fstream::binary);

		this->wideFileName = (wchar_t*)filename.c_str();
		break;
	case SimpleFile::WRITE:
		//
		this->file = new fstream(filename, fstream::out | fstream::binary);
		this->wideFileName = (wchar_t*)filename.c_str();
		break;
	case SimpleFile::WRITE_APPEND:
		//
		this->file = new fstream(filename, fstream::in | fstream::ate | fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new fstream(filename, fstream::out | fstream::app | fstream::binary);
		this->wideFileName = (wchar_t*)filename.c_str();
		break;
	default:
		this->file = new fstream(filename, fstream::in | fstream::ate | fstream::binary);
		size = (int)file->tellg();
		file->close();

		this->file = new fstream(filename, fstream::in | fstream::binary);
		this->wideFileName = (wchar_t*)filename.c_str();
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
		StringTools::out << (int)isOpen() << StringTools::lineBreak;
		StringTools::out << (int)type << StringTools::lineBreak;
		StringTools::out << (int)isEndOfFile() << StringTools::lineBreak;

		StringTools::out << "Is not opened for reading" << StringTools::lineBreak;
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
		StringTools::out << "Is not opened for reading" << StringTools::lineBreak;
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
	string p = "";
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
		StringTools::out << "Is not opened for reading" << StringTools::lineBreak;
	}

	return nullptr;
}

const wchar_t * SimpleFile::readWideLine()
{
	wstring p = L"";
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
		StringTools::out << "Is not opened for reading" << StringTools::lineBreak;
	}

	return nullptr;
}

string SimpleFile::readString()
{
	string p = "";
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
	}
	else
	{
		StringTools::out << "Is not opened for reading" << StringTools::lineBreak;
	}

	return p;
}

wstring SimpleFile::readWideString()
{
	wstring p = L"";
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
		StringTools::out << "Is not opened for reading" << StringTools::lineBreak;
	}

	return p;
}

vector<const char*> SimpleFile::readFullFile()
{
	vector<const char*> info = vector<const char*>();

	while (!isEndOfFile())
	{
		info.push_back(readLine());
	}

	return info;
}

vector<const wchar_t*> SimpleFile::readFullFileWide()
{
	//Read the first two bytes to determine if the file is a unicode file
	vector<const wchar_t*> info = vector<const wchar_t*>();

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

	return vector<const wchar_t*>();
}

vector<string> SimpleFile::readFullFileString()
{
	vector<string> info = vector<string>();

	while (!isEndOfFile())
	{
		info.push_back(readString());
	}

	return info;
}

vector<wstring> SimpleFile::readFullFileStringWide()
{
	//Read the first two bytes to determine if the file is a unicode file
	vector<wstring> info = vector<wstring>();

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

vector<unsigned char> SimpleFile::readFullFileAsBytes()
{
	vector<unsigned char> info = vector<unsigned char>();

	while (!isEndOfFile())
	{
		//read
		info.push_back(file->get());
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
			StringTools::out << "Is not opened for writing" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "File is not opened" << StringTools::lineBreak;
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
			StringTools::out << "Is not opened for writing" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "File is not opened" << StringTools::lineBreak;
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
			StringTools::out << "Is not opened for writing" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "File is not opened" << StringTools::lineBreak;
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
			StringTools::out << "Is not opened for writing" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "File is not opened" << StringTools::lineBreak;
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
			StringTools::out << "Is not opened for writing" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "File is not opened" << StringTools::lineBreak;
	}
}

void SimpleFile::writeString(string line)
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
			StringTools::out << "Is not opened for writing" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "File is not opened" << StringTools::lineBreak;
	}
}

void SimpleFile::writeWideString(wstring line)
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
			StringTools::out << "Is not opened for writing" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "File is not opened" << StringTools::lineBreak;
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
			StringTools::out << "Is not opened for writing" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "File is not opened" << StringTools::lineBreak;
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

wstring SimpleFile::getFileName()
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
