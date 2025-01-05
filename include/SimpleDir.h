#pragma once
#include "BuildOptions.h"
#include <vector>
#include <filesystem>

#include "File.h"
#include "GeneralExceptions.h"

namespace smpl
{
	
	class DLL_OPTION SimpleDir
	{
	public:

		/**
		 * @brief Construct a new SimpleDir object
		 * 		Uses the C++ filesystem 
		 * 		Contains information about a directory and its files.
		 * 		All strings are utf8
		 * 
		 * @param directory 
		 */
		SimpleDir(File directory);

		/**
		 * @brief Destroy the SimpleDir object
		 * 
		 */
		~SimpleDir();

		//Exceptions
		struct ExistanceError : public std::exception
		{
			const char* what() const noexcept { return "File or Directory could not be found."; }
		};
		struct CopyError : public std::exception
		{
			const char* what() const noexcept { return "File or Directory could not be copied."; }
		};
		struct DeleteError : public std::exception
		{
			const char* what() const noexcept { return "File or Directory could not be deleted."; }
		};
		struct RenameError : public std::exception
		{
			const char* what() const noexcept { return "File or Directory could not be renamed."; }
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
		 * @return std::string 
		 */
		std::string getReferenceName(size_t index);

		/**
		 * @brief Gets the Reference Full Path at the specified index.
		 * 		May be a file or folder.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBounds Error may be thrown.
		 * 
		 * @param index 
		 * @return std::string 
		 */
		std::string getReferenceFullPath(size_t index);

		/**
		 * @brief Gets the Reference Full Path for the specified reference
		 * 		May be a file or folder.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBounds Error may be thrown.
		 * 
		 * @param n 
		 * @return std::string 
		 */
		static std::string getReferenceFullPath(std::string n);

		/**
		 * @brief Get the size of the file or folder
		 * 		If USE_EXCEPTIONS is defined, an OutOfBounds Error may be thrown.
		 * @param index 
		 * @return size_t 
		 */
		static uintmax_t getReferenceSize(std::string name);

		/**
		 * @brief Gets the Last Change Time for the file or folder
		 * 		If USE_EXCEPTIONS is defined, an OutOfBounds Error may be thrown.
		 * 
		 * @param index 
		 * @return time_t 
		 */
		static time_t getLastChangeTime(std::string name);
		
		/**
		 * @brief Attempts to rename the reference.
		 * 		The function may fail to rename the resource.
		 * 		If USE_EXCEPTIONS is defined, an RenameError may be thrown.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBoundsError may be thrown.
		 * 
		 * @param newFileName 
		 * @param index 
		 */
		static void renameResource(std::string oldFileName, std::string newFileName);

		/**
		 * @brief Attempts to delete the reference.
		 * 		The function may fail to delete the resource.
		 * 		If USE_EXCEPTIONS is defined, an DeleteError may be thrown.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBoundsError may be thrown.
		 * 
		 * 		Note that deleting in this way will not move the data to the recycle bin.
		 * @param index 
		 */
		static void deleteResource(std::string filename);

		/**
		 * @brief Attempts to copy the reference at the index.
		 * 		The function may fail to copy the resource.
		 * 		If USE_EXCEPTIONS is defined, an CopyError may be thrown.
		 * 		If USE_EXCEPTIONS is defined, an OutOfBoundsError may be thrown.
		 * @param newFileName 
		 * @param index 
		 */
		static void copyResource(std::string oldFileName, std::string newFileName);

		//Directory Stuff

		/**
		 * @brief Gets the amount of items in the directory.
		 * 
		 * @return size_t 
		 */
		size_t getSize();

		/**
		 * @brief Returns a list of all the folders in this directory.
		 * 		Returns utf8 strings
		 * 		Does not return the path.
		 * 
		 * @return std::vector<std::string> 
		 */
		std::vector<std::string> getFolders();

		/**
		 * @brief Returns a list of all the files in this directory.
		 * 		Returns utf8 strings
		 * 		Does not return the path.
		 * 
		 * @return std::vector<std::string> 
		 */
		std::vector<std::string> getFiles();

		/**
		 * @brief Gets the raw Directory Entries that were found for this directory.
		 * 
		 * @return std::vector<std::filesystem::directory_entry> 
		 */
		std::vector<std::filesystem::directory_entry> getEntries();

		/**
		 * @brief Creates a Directory with the path that the object was created with.
		 * 
		 */
		static void createDirectory(std::string n);

		/**
		 * @brief Gets the name of the directory.
		 * 		It is the string used to create the object.
		 * 		Returns a utf8 string
		 * 
		 * @return std::string 
		 */
		std::string getLocation();

		/**
		 * @brief Returns if the input is a directory.
		 * 		If it does not exist, this returns false.
		 * 
		 * @param n 
		 * @return true 
		 * @return false 
		 */
		static bool isDirectory(std::string n);

		/**
		 * @brief Returns if the input is a normal file.
		 * 		If it does not exist, this returns false.
		 * 
		 * @param n 
		 * @return true 
		 * @return false 
		 */
		static bool isFile(std::string n);
		
		/**
		 * @brief Returns if the input does exist.
		 * 
		 * @param n 
		 * @return true 
		 * @return false 
		 */
		static bool doesExist(std::string n);

	private:
		static uintmax_t getFileSize(std::filesystem::directory_entry f);
		static uintmax_t getFolderSize(std::filesystem::directory_entry f);

		std::vector<std::filesystem::directory_entry> names = std::vector<std::filesystem::directory_entry>();
		std::string location = "";
	};

}  //NAMESPACE glib END