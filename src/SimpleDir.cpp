#include "SimpleDir.h"
#include "StringTools.h"

const Class* SimpleDir::myClass = new Class("SimpleDir", {Object::myClass});
const Class* SimpleDir::getClass()
{
	return SimpleDir::myClass;
}

/*
	Creates an object that will hold information about a directory
	such as the folders and files in it.
*/
SimpleDir::SimpleDir(wchar_t* directory)
{
	try
	{
		exists = filesystem::is_directory(directory);
		location = directory;

		if (exists == true)
		{
			update();
		}
	}
	catch (...)
	{
		StringTools::out << "Failed to initialize." << StringTools::lineBreak;
		StringTools::out << "An invalid character may have been used." << StringTools::lineBreak;

	}
}

/*
	This deconstructor does not do anything.
*/
SimpleDir::~SimpleDir()
{
	std::wcout << L"Destroying" << L'\n';
}

/*
	Updates all of the references in this object to reflect any changes
	in the directory.
*/
void SimpleDir::update()
{
	names.clear();
	for (auto& entry: filesystem::directory_iterator(location))
	{
		names.push_back(entry);
	}
}

/*
	Gets the name of a reference at the index if possible.
*/
std::wstring SimpleDir::getReferenceName(int index)
{
	if (index >= 0 && index < names.size())
	{
		std::wstring temp = getReferenceFullPath(index);

		int pos = temp.find_last_of(L'\\', temp.size());

		return temp.substr(pos + 1, temp.size());
	}
	else
	{
		StringTools::out << "Out of bounds error" << StringTools::lineBreak;
		return L"";
	}
}

std::wstring SimpleDir::getReferenceFullPath(int index)
{
	if (index >= 0 && index < names.size())
	{
		return names[index].path().c_str();
	}
	else
	{
		StringTools::out << "Out of bounds error" << StringTools::lineBreak;
		return L"";
	}
}

/*
	Gets the file size of a reference at the index if possible.
	Returns an unsigned int.
*/
unsigned int SimpleDir::fileSize(int index)
{
	if (index >= 0 && index < names.size())
	{
		return (unsigned int)filesystem::file_size(names[index]);
	}
	else
	{
		StringTools::out << "Out of bounds error" << StringTools::lineBreak;
		return 0;
	}
	return 0;
}

/*
	Returns a time_t that represents the last time it was changed.
*/
time_t SimpleDir::getLastChangeTime(int index)
{
	if (index >= 0 && index < names.size())
	{
		filesystem::file_time_type ftime = filesystem::last_write_time(names[index].path());
		
		std::time_t t = std::chrono::duration_cast<std::chrono::seconds>(ftime.time_since_epoch()).count();
		
		return t;
	}
	else
	{
		StringTools::out << "Out of bounds error" << StringTools::lineBreak;
		return 0;
	}
}

/*
	Determines if the particular reference at the specified index
	is another directory or not if possible.
*/
bool SimpleDir::referenceIsDirectory(int index)
{
	if (index >= 0 && index < names.size())
	{
		return filesystem::is_directory( names[index].path() );
	}
	else
	{
		StringTools::out << "Out of bounds error" << StringTools::lineBreak;
		return false;
	}
}

/*
	Renames a resource at the specified location.
	Changes the name to the new name specified.
	The new name should not contain the path.
*/
void SimpleDir::renameResource(wchar_t * newName, int index)
{
	if (index >= 0 && index < names.size())
	{
		std::wstring temp = getLocation();
		temp += L"\\";
		temp += newName;

		try
		{
			filesystem::rename(names[index], temp);
		}
		catch (...)
		{
			StringTools::out << "Error on Rename" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "Out of bounds error" << StringTools::lineBreak;
	}
}

/*
	Attempts to delete a resource at the specified location.
*/
void SimpleDir::deleteResource(int index)
{
	if (index >= 0 && index < names.size())
	{
		try
		{
			if (!this->referenceIsDirectory(index))
			{
				filesystem::remove(names[index]); //Just a file
			}
			else
			{
				filesystem::remove_all(names[index]); //A directory
			}
		}
		catch(...)
		{
			StringTools::out << "Error on delete" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "Out of bounds error" << StringTools::lineBreak;
	}
}

/*
	Copies the resource at the specified index to another file 
	with the specified name. The name can contain a path. If the directory
	ends with a / or \, then it will use the current name of the file.
	Otherwise, it will use the name and file extension you specify.
*/
void SimpleDir::copyResource(wchar_t * newName, int index)
{
	if (index >= 0 && index < names.size())
	{
		std::wstring oth = L"";
		oth += newName;
		
		if (oth.at(oth.size() - 1) == L'\\')
		{
			StringTools::out << "IS FOLDER" << StringTools::lineBreak;
			oth += getReferenceName(index);
		}
		
		try
		{
			filesystem::copy(names[index], oth);
		}
		catch (...)
		{
			StringTools::out << "Error on Copy" << StringTools::lineBreak;
		}
	}
	else
	{
		StringTools::out << "Out of bounds error" << StringTools::lineBreak;
	}
}

/*
	Attempts to get a reference with the specified name.
	Returns -1 if the reference does not exist.
*/
int SimpleDir::getReferenceLocation(wchar_t * name)
{
	int index = -1;
	for (int i = 0; i < names.size(); i++)
	{
		if ( std::wcscmp(name, getReferenceName(i).c_str() ) == 0 )
		{
			index = i;
			break;
		}
	}

	return index;
}

/*
	Gets the amount of references that are stored in this object.
*/
int SimpleDir::getSize()
{
	return names.size();
}

/*
	Gets all of the folders in this directory and returns them as
	a vector of wide character pointers.
*/
std::vector<wchar_t*> SimpleDir::getFolders()
{
	std::vector<wchar_t*> dirs = std::vector<wchar_t*>();

	for (int i = 0; i < getSize(); i++)
	{
		if (referenceIsDirectory(i))
		{
			dirs.push_back((wchar_t*)getReferenceFullPath(i).c_str());
		}
	}

	return dirs;
}

/*
	Gets all of the files in this directory and returns them as
	a vector of wide character pointers.
*/
std::vector<wchar_t*> SimpleDir::getFiles()
{
	std::vector<wchar_t*> files = std::vector<wchar_t*>();

	for (int i = 0; i < getSize(); i++)
	{
		if (!referenceIsDirectory(i))
		{
			files.push_back((wchar_t*)getReferenceFullPath(i).c_str());
		}
	}

	return files;
}

/*
	Determines whether or not this directory exists.
*/
bool SimpleDir::doesExist()
{
	return exists;
}

/*
	Creates a directory at this location if it does not exists.
*/
void SimpleDir::createDirectory()
{
	if(!doesExist())
		filesystem::create_directory(location);
}

/*
	Returns a string that represents the location of the directory
*/
wchar_t* SimpleDir::getLocation()
{
	return (wchar_t*)location.c_str();
}