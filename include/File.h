#pragma once
#include <iostream>
#include "StringTools.h"

namespace glib
{

    class File
    {
    public:
        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a utf8 string to store data.
         * @param filename
         *      The filename or path.
         */
        File(std::string filename)
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
        File(std::wstring filename)
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
        File(char* filename)
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

        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a utf8 string to store data.
         * @param filename
         *      The filename or path.
         */
        File(wchar_t* filename)
        {
            init( StringTools::toUTF8String(filename) );
        }

        /**
         * @brief Destroys a File Object.
         */
        ~File();

        /**
         * @brief Returns just the name of the file.
         *      The extension is not included.
         * @return std::string
         */
        std::string getFileName();

        /**
         * @brief Returns the full reference to the file.
         *      Has the full path, filename, and extension.
         * @return std::string
         */
        std::string getFullFileName();

        /**
         * @brief Returns the path to the file.
         * @return std::string
         */
        std::string getPath();

        /**
         * @brief Returns just the extension of the file.
         * @return std::string
         */
        std::string getExtension();

        /**
         * @brief Returns the filename with the extension.
         * 
         * @return std::string 
         */
        std::string getFileNameWithExt();

        /**
         * @brief Returns if the object represents a directory.
         *      It is a wrapper around SimpleDir::isDirectory
         * 
         * @return true 
         * @return false 
         */
        bool isDirectory();

        /**
         * @brief Returns if the object represents a normal file.
         *      It is a wrapper around SimpleDir::isFile
         * 
         * @return true 
         * @return false 
         */
        bool isFile();

        /**
         * @brief Returns if the object represents something that exists.
         *      It is a wrapper around SimpleDir::doesExist
         * 
         * @return true 
         * @return false 
         */
        bool doesExist();

    private:
        void init(std::string filename);
        
        std::string fullFileName;
        size_t locationOfExtension;
        size_t locationOfFileName;
        
    };
    
}  //NAMESPACE glib END