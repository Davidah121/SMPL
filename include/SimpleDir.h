#pragma once

#include<vector>
#include<filesystem>

#include "GeneralExceptions.h"
#include "Object.h"

class SimpleDir : public Object
{
public:
	SimpleDir(std::string directory);
	SimpleDir(std::wstring directory);
	~SimpleDir();

	//Object and Class Stuff
	const Class* getClass();
	static const Class myClass;

	//Exceptions
	struct ExistanceError : public std::exception
	{
		const char* what() noexcept { return "File or Directory could not be found."; }
	};
	struct CopyError : public std::exception
	{
		const char* what() noexcept { return "File or Directory could not be copied."; }
	};
	struct DeleteError : public std::exception
	{
		const char* what() noexcept { return "File or Directory could not be deleted."; }
	};
	struct RenameError : public std::exception
	{
		const char* what() noexcept { return "File or Directory could not be renamed."; }
	};

	void update(); //Update the references in this directory

	//Reference Stuff
	std::wstring getReferenceName(int index); //Get the name of a thing in the directory
	std::wstring getReferenceFullPath(int index); //Gets the full path to a thing in the directory
	unsigned int fileSize(int index); //Gets the size of the file at that index
	time_t getLastChangeTime(int index); //Gets the last time the reference at that index was updated
	bool referenceIsDirectory(int index); //Gets whether the reference is a directory or not
	
	void renameResource(std::wstring newName, int index); //
	void deleteResource(int index); //Delete Resouce
	void copyResource(std::wstring newName, int index); //

	int getReferenceLocation(std::wstring name);

	//Directory Stuff
	int getSize(); //Returns the amount of items in this directory
	std::vector<std::wstring> getFolders(); //Returns all of the folders
	std::vector<std::wstring> getFiles(); //Returns the files with a particular attribute

	bool doesExist(); //Determines if this is currently a directory
	void createDirectory(); //Creates a directory at that location

	std::wstring getLocation();

private:
	std::vector<std::filesystem::directory_entry> names = std::vector<std::filesystem::directory_entry>();
	bool exists = false;
	std::wstring location = L"";
};

