#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include <iostream>
#include "StringTools.h"

namespace smpl
{

    class DLL_OPTION File
    {
    public:
        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a utf8 string to store data.
         * @param filename
         *      The filename or path.
         */
        File(const std::string& filename)
        {
            init( filename );
        }

        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a utf8 string to store data.
         * @param filename
         *      The filename or path.
         */
        File(const std::wstring& filename)
        {
            init( StringTools::toUTF8String(filename) );
        }

        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a utf8 string to store data.
         * @param filename
         *      The filename or path.
         */
        File(const char* filename)
        {
            std::string s = filename;
            init( s );
        }

        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a utf8 string to store data.
         * @param filename
         *      The filename or path.
         */
        File(const wchar_t* filename)
        {
            init( StringTools::toUTF8String(filename) );
        }

		File();
		
		File(const File& other) = default;
		File& operator=(const File& other) = default;

		File(File&& other) = default;
		File& operator=(File&& other) = default;
        /**
         * @brief Destroys a File Object.
         */
        ~File();

        /**
         * @brief Returns just the name of the file.
         *      The extension is not included.
         * @return std::string
         */
        std::string getFileName() const;

        /**
         * @brief Returns the full reference to the file.
         *      Has the full path, filename, and extension.
         * @return std::string
         */
        std::string getFullFileName() const;

        /**
         * @brief Returns the path to the file.
         * @return std::string
         */
        std::string getPath() const;

        /**
         * @brief Returns just the extension of the file.
         * @return std::string
         */
        std::string getExtension() const;

        /**
         * @brief Returns the filename with the extension.
         * 
         * @return std::string 
         */
        std::string getFileNameWithExt() const;

        /**
         * @brief Returns if the object represents a directory.
         *      This value is cached and found upon creation of the object. May not represent current situations
         * 
         * @return true 
         * @return false 
         */
        bool isDirectory() const;

        /**
         * @brief Returns if the object represents a normal file.
         *      This value is cached and found upon creation of the object. May not represent current situations
         * 
         * @return true 
         * @return false 
         */
        bool isFile() const;

        /**
         * @brief Returns if the object represents something that exists.
         *      This value is cached and found upon creation of the object. May not represent current situations
         * 
         * @return true 
         * @return false 
         */
        bool doesExist() const;

		/**
		 * @brief Gets the cached file size. Size of the file is computed upon creation of the object.
		 * 		May not represent current situations
		 *
		 * @return size_t 
		 */
		size_t getFileSize() const;
		
		/**
		 * @brief Gets the cached Time for the file. That time being the last time the file was modified.
		 *		This time value is calculated upon creation of the object. May not represent current situations
		 * 
		 * @return size_t 
		 */
		size_t getFileTime() const;

    private:
        void init(std::string filename);
        
        std::string fullFileName;
        size_t locationOfExtension;
        size_t locationOfFileName;

		size_t cachedFileSize;
		size_t cachedFileTime;
		bool cachedExistance;
		bool cachedIsFileType;
		bool cachedIsDirectoryType;
    };
    
}  //NAMESPACE glib END