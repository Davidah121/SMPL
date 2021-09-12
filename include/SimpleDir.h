#pragma once

#include<vector>
#include<filesystem>

#include "GeneralExceptions.h"
#include "Object.h"

namespace glib
{
	
	class SimpleDir : public Object
	{
	public:

		/**
		 * @brief Construct a new SimpleDir object
		 * 		Uses the C++ filesystem 
		 * 		Contains information about a directory and its files.
		 * 
		 * @param directory 
		 */
		SimpleDir(std::string directory);

		/**
		 * @brief Construct a new SimpleDir object
		 * 		Uses the C++ filesystem 
		 * 		Contains information about a directory and its files.
		 * 
		 * @param directory 
		 */
		SimpleDir(std::wstring directory);

		/**
		 * @brief Destroy the SimpleDir object
		 * 
		 */
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

		/**
		 * @brief Updates all of the references for the directory.
		 * 		(Meaning what files and folders are in it if there was some change)
		 * 
		 */
		void update();

		//Reference Stuff

		/**
		 * @brief Gets the Reference Name at the specified index.
		 * 		May be a file or folder.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBounds Error may be thrown.
		 * 
		 * @param index 
		 * @return std::wstring 
		 */
		std::wstring getReferenceName(int index);

		/**
		 * @brief Gets the Reference Full Path at the specified index.
		 * 		May be a file or folder.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBounds Error may be thrown.
		 * 
		 * @param index 
		 * @return std::wstring 
		 */
		std::wstring getReferenceFullPath(int index);

		/**
		 * @brief Returns the size of the file or folder at the index.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBounds Error may be thrown.
		 * @param index 
		 * @return unsigned int 
		 */
		unsigned int fileSize(int index);

		/**
		 * @brief Gets the Last Change Time for the file or folder at the index.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBounds Error may be thrown.
		 * 
		 * @param index 
		 * @return time_t 
		 */
		time_t getLastChangeTime(int index);

		/**
		 * @brief Returns if the reference at the specified index is a directory.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBounds Error may be thrown.
		 * 
		 * @param index 
		 * @return true 
		 * @return false 
		 */
		bool referenceIsDirectory(int index); //Gets whether the reference is a directory or not
		
		/**
		 * @brief Attempts to rename the reference at the index.
		 * 		The function may fail to rename the resource.
		 * 		If USE_EXCEPTIONS is defined, an RenameError may be thrown.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBoundsError may be thrown.
		 * 
		 * @param newFileName 
		 * @param index 
		 */
		void renameResource(std::wstring newFileName, int index);

		/**
		 * @brief Attempts to delete the reference at the index.
		 * 		The function may fail to delete the resource.
		 * 		If USE_EXCEPTIONS is defined, an DeleteError may be thrown.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBoundsError may be thrown.
		 * 
		 * 		Note that deleting in this way will not move the data to the recycle bin.
		 * @param index 
		 */
		void deleteResource(int index);

		/**
		 * @brief Attempts to copy the reference at the index.
		 * 		The function may fail to copy the resource.
		 * 		If USE_EXCEPTIONS is defined, an CopyError may be thrown.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBoundsError may be thrown.
		 * @param newFileName 
		 * @param index 
		 */
		void copyResource(std::wstring newFileName, int index);

		/**
		 * @brief Gets the Reference Location by its name.
		 * 
		 * @param name 
		 * @return int 
		 */
		int getReferenceLocation(std::wstring name);

		//Directory Stuff

		/**
		 * @brief Gets the amount of items in the directory.
		 * 
		 * @return int 
		 */
		int getSize();

		/**
		 * @brief Returns a list of all the folders.
		 * 
		 * @return std::vector<std::wstring> 
		 */
		std::vector<std::wstring> getFolders();

		/**
		 * @brief Returns a list of all the files.
		 * 
		 * @return std::vector<std::wstring> 
		 */
		std::vector<std::wstring> getFiles();

		/**
		 * @brief Determines if the directory specified exists.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool doesExist();

		/**
		 * @brief Creates a Directory with the path that the object was created with.
		 * 
		 */
		void createDirectory();

		/**
		 * @brief Gets the name of the directory.
		 * 		It is the string used to create the object.
		 * 
		 * @return std::wstring 
		 */
		std::wstring getLocation();

	private:
		std::vector<std::filesystem::directory_entry> names = std::vector<std::filesystem::directory_entry>();
		bool exists = false;
		std::wstring location = L"";
	};

}  //NAMESPACE glib END