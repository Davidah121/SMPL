#include "SimpleDir.h"
#include "StringTools.h"
#include <iomanip>

namespace smpl
{
	/*
		Creates an object that will hold information about a directory
		such as the folders and files in it.
	*/
	SimpleDir::SimpleDir(File directory)
	{
		std::string dir = directory.getFullFileName();
		#ifdef __unix__
			//In order for this to work in linux, replace all '\' with '/'. Yes, Linux sucks.
			for(int i=0; i<dir.size(); i++)
			{
				if(dir[i] == '\\')
					dir[i] = '/';
			}
		#endif
		if(dir.size() > 0)
		{
			if(dir.back() != '/' && dir.back() != '\\')
				dir += '/';
			else if(dir.back() == '\\')
				dir.back() = '/'; //For linux compatibility
		}

		try
		{
			location = dir;
			update();
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
	std::string SimpleDir::getReferenceName(size_t index)
	{
		if (index < names.size())
		{
			File tempF = getReferenceFullPath(index);
			return tempF.getFileNameWithExt();
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif

			return "";
		}
	}

	std::string SimpleDir::getReferenceFullPath(size_t index)
	{
		if (index < names.size())
		{
			auto p = names[index].path();
			std::string s;
			try
			{
				s = std::filesystem::canonical( p ).u8string();
			}
			catch(const std::exception& e)
			{
				//error. Likely access denied but can't print errors. Causes another fatal error.
				//Shouldn't keep it but it will be kept.
				s = p.u8string();
			}
			return s;
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::OutOfBoundsError();
			#endif
			return "";
		}
	}

	std::string SimpleDir::getReferenceFullPath(std::string n)
	{
		std::filesystem::path p = n;
		std::string s;
		try
		{
			s = std::filesystem::canonical( p ).u8string();
		}
		catch(const std::exception& e)
		{
			//error. Likely access denied but can't print errors. Causes another fatal error.
			//Shouldn't keep it but it will be kept.
			s = p.u8string();
		}
		return s;
	}

	/*
		Gets the file size of a reference at the index if possible.
		Returns an size_t.
	*/
	uintmax_t SimpleDir::getReferenceSize(std::string name)
	{
		if(SimpleDir::doesExist(name))
		{
			std::filesystem::directory_entry dirEntry = std::filesystem::directory_entry(name);
			if(dirEntry.is_directory())
				return getFolderSize(dirEntry);
			else
				return getFileSize(dirEntry);
		}
		return 0;
	}
	
	uintmax_t SimpleDir::getFileSize(std::filesystem::directory_entry f)
	{
		return std::filesystem::file_size(f);
	}

	uintmax_t SimpleDir::getFolderSize(std::filesystem::directory_entry f)
	{
		uintmax_t totalSize = 0;
		for (auto& entry: std::filesystem::directory_iterator(f))
		{
			if(entry.is_directory())
			{
				totalSize += getFolderSize(entry);
			}
			else
			{
				totalSize += getFileSize(entry);
			}
		}
		return totalSize;
	}

	/*
		Returns a time_t that represents the last time it was changed.
	*/
	time_t SimpleDir::getLastChangeTime(std::string name)
	{
		if(SimpleDir::doesExist(name))
		{
			std::filesystem::file_time_type ftime = std::filesystem::last_write_time(name);
			
			auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
				ftime - 
				std::filesystem::file_time_type::clock::now() +
				std::chrono::system_clock::now()
			);

			return std::chrono::system_clock::to_time_t(sctp);
		}
		return 0;
	}

	/*
		Renames a resource at the specified location.
		Changes the name to the new name specified.
		The new name should not contain the path.
	*/
	void SimpleDir::renameResource(std::string oldName, std::string newName)
	{
		try
		{
			std::filesystem::rename(oldName, newName);
		}
		catch (...)
		{
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::RenameError();
			#endif
		}
	}

	/*
		Attempts to delete a resource at the specified location.
	*/
	void SimpleDir::deleteResource(std::string n)
	{
		try
		{
			if (std::filesystem::is_directory(n))
			{
				std::filesystem::remove_all(n); //A directory
			}
			else
			{
				std::filesystem::remove(n); //Just a file
			}
		}
		catch(...)
		{
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::DeleteError();
			#endif
		}
	}

	/*
		Copies the resource at the specified index to another file 
		with the specified name. The name can contain a path. If the directory
		ends with a / or \, then it will use the current name of the file.
		Otherwise, it will use the name and file extension you specify.
	*/
	void SimpleDir::copyResource(std::string oldName, std::string newName)
	{
		try
		{
			std::filesystem::copy(oldName, newName, std::filesystem::copy_options::overwrite_existing);
		}
		catch(std::exception e)
		{
			StringTools::println("%s", e.what());
			#ifdef USE_EXCEPTIONS
			throw SimpleDir::CopyError();
			#endif
		}
	}

	/*
		Gets the amount of references that are stored in this object.
	*/
	size_t SimpleDir::getSize()
	{
		return names.size();
	}

	/*
		Gets all of the folders in this directory and returns them as
		a vector of wide character pointers.
	*/
	std::vector<std::string> SimpleDir::getFolders()
	{
		std::vector<std::string> dirs = std::vector<std::string>();

		for (size_t i = 0; i < getSize(); i++)
		{
			if (names[i].is_directory())
			{
				dirs.push_back(names[i].path().filename().u8string());
			}
		}

		return dirs;
	}

	/*
		Gets all of the files in this directory and returns them as
		a vector of wide character pointers.
	*/
	std::vector<std::string> SimpleDir::getFiles()
	{
		std::vector<std::string> files = std::vector<std::string>();

		for (size_t i = 0; i < getSize(); i++)
		{
			if (names[i].is_regular_file())
			{
				files.push_back(names[i].path().filename().u8string());
			}
		}

		return files;
	}

	std::vector<std::filesystem::directory_entry> SimpleDir::getEntries()
	{
		return names;
	}

	/*
		Creates a directory
	*/
	void SimpleDir::createDirectory(std::string n)
	{
		if(!doesExist(n))
			std::filesystem::create_directory(n);
	}

	/*
		Returns a string that represents the location of the directory
	*/
	std::string SimpleDir::getLocation()
	{
		return location;
	}

	bool SimpleDir::isDirectory(std::string n)
	{
		return std::filesystem::is_directory(n);
	}

	bool SimpleDir::isFile(std::string n)
	{
		return std::filesystem::is_regular_file(n);
	}
	
	bool SimpleDir::doesExist(std::string n)
	{
		return std::filesystem::exists(n);
	}

} //NAMESPACE glib END