#pragma once

#include<vector>
#include<filesystem>
#include "Object.h"

using namespace std;

class SimpleDir : public Object
{
public:
	SimpleDir(wchar_t* directory);
	~SimpleDir();

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	void update(); //Update the references in this directory

	//Reference Stuff
	std::wstring getReferenceName(int index); //Get the name of a thing in the directory
	std::wstring getReferenceFullPath(int index); //Gets the full path to a thing in the directory
	unsigned int fileSize(int index); //Gets the size of the file at that index
	time_t getLastChangeTime(int index); //Gets the last time the reference at that index was updated
	bool referenceIsDirectory(int index); //Gets whether the reference is a directory or not
	
	void renameResource(wchar_t* newName, int index); //
	void deleteResource(int index); //Delete Resouce
	void copyResource(wchar_t* newName, int index); //

	int getReferenceLocation(wchar_t* name);

	//Directory Stuff
	int getSize(); //Returns the amount of items in this directory
	std::vector<wchar_t*> getFolders(); //Returns all of the folders
	std::vector<wchar_t*> getFiles(); //Returns the files with a particular attribute

	bool doesExist(); //Determines if this is currently a directory
	void createDirectory(); //Creates a directory at that location

	wchar_t* getLocation();

private:
	std::vector<filesystem::directory_entry> names = std::vector<filesystem::directory_entry>();
	bool exists = false;
	std::wstring location = L"";
};

