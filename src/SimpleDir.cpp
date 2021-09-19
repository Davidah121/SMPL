#include "SimpleDir.h"
#include "StringTools.h"
#include <iomanip>

namespace glib
{

	const Class SimpleDir::myClass = Class("SimpleDir", {&Object::myClass});
	const Class* SimpleDir::getClass()
	{
		return &SimpleDir::myClass;
	}

	/*
		Creates an object that will hold information about a directory
		such as the folders and files in it.
	*/
	SimpleDir::SimpleDir(File directory)
	{
		std::wstring dir = directory.getFullFileName();
		try
		{
			exists = std::filesystem::is_directory(dir);
			location = dir;

			if (exists == true)
			{
				update();
			}
		}
		catch (...)
		{
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::ExistanceError();
			#endif
		}
	}

	/*
		This deconstructor does not do anything.
	*/
	SimpleDir::~SimpleDir()
	{
	}

	/*
		Updates all of the references in this object to reflect any changes
		in the directory.
	*/
	void SimpleDir::update()
	{
		names.clear();
		for (auto& entry: std::filesystem::directory_iterator(location))
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
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif

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
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif
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
			return (unsigned int)std::filesystem::file_size(names[index]);
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif
			return -1;
		}
	}

	/*
		Returns a time_t that represents the last time it was changed.
	*/
	time_t SimpleDir::getLastChangeTime(int index)
	{
		if (index >= 0 && index < names.size())
		{
			std::filesystem::file_time_type ftime = std::filesystem::last_write_time(names[index].path());
			
			auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
				ftime - 
				std::filesystem::file_time_type::clock::now() +
				std::chrono::system_clock::now()
			);

			return std::chrono::system_clock::to_time_t(sctp);
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif
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
			return std::filesystem::is_directory( names[index].path() );
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif
			return false;
		}
	}

	/*
		Renames a resource at the specified location.
		Changes the name to the new name specified.
		The new name should not contain the path.
	*/
	void SimpleDir::renameResource(std::wstring newName, int index)
	{
		if (index >= 0 && index < names.size())
		{
			std::wstring temp = getLocation();
			temp += L"\\";
			temp += newName;

			try
			{
				std::filesystem::rename(names[index], temp);
			}
			catch (...)
			{
				#ifdef USE_EXCEPTIONS
				throw SimpleDir::RenameError();
				#endif
			}
		}
		else
		{
			//error
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif
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
					std::filesystem::remove(names[index]); //Just a file
				}
				else
				{
					std::filesystem::remove_all(names[index]); //A directory
				}
			}
			catch(...)
			{
				#ifdef USE_EXCEPTIONS
				throw SimpleDir::DeleteError();
				#endif
			}
		}
		else
		{
			//error
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif
		}
	}

	/*
		Copies the resource at the specified index to another file 
		with the specified name. The name can contain a path. If the directory
		ends with a / or \, then it will use the current name of the file.
		Otherwise, it will use the name and file extension you specify.
	*/
	void SimpleDir::copyResource(std::wstring newName, int index)
	{
		if (index >= 0 && index < names.size())
		{
			std::wstring oth = L"";
			oth += newName;
			
			if (oth.at(oth.size() - 1) == L'\\')
			{
				//Is folder
				oth += getReferenceName(index);
			}
			
			try
			{
				std::filesystem::copy(names[index], oth);
			}
			catch (...)
			{
				#ifdef USE_EXCEPTIONS
				throw SimpleDir::CopyError();
				#endif
			}
		}
		else
		{
			//error
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif
		}
	}

	/*
		Attempts to get a reference with the specified name.
		Returns -1 if the reference does not exist.
	*/
	int SimpleDir::getReferenceLocation(std::wstring name)
	{
		int index = -1;
		for (int i = 0; i < names.size(); i++)
		{
			if ( name == getReferenceName(i) )
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
	std::vector<std::wstring> SimpleDir::getFolders()
	{
		std::vector<std::wstring> dirs = std::vector<std::wstring>();

		for (int i = 0; i < getSize(); i++)
		{
			if (referenceIsDirectory(i))
			{
				dirs.push_back(getReferenceFullPath(i));
			}
		}

		return dirs;
	}

	/*
		Gets all of the files in this directory and returns them as
		a vector of wide character pointers.
	*/
	std::vector<std::wstring> SimpleDir::getFiles()
	{
		std::vector<std::wstring> files = std::vector<std::wstring>();

		for (int i = 0; i < getSize(); i++)
		{
			if (!referenceIsDirectory(i))
			{
				files.push_back(getReferenceFullPath(i));
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
			std::filesystem::create_directory(location);
	}

	/*
		Returns a string that represents the location of the directory
	*/
	std::wstring SimpleDir::getLocation()
	{
		return location;
	}

} //NAMESPACE glib END