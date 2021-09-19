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
         *      Uses a std::wstring to store data.
         * @param filename
         *      The filename or path.
         */
        File(std::wstring filename)
        {
            init( filename );
        }

        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a std::wstring to store data.
         * @param filename
         *      The filename or path.
         */
        template<typename T>
        File(std::basic_string<T> filename)
        {
            init( StringTools::toWideString(filename) );
        }

        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a std::wstring to store data.
         * @param filename
         *      The filename or path.
         */
        File(char* filename)
        {
            std::string s = filename;
            init( StringTools::toWideString(s) );
        }

        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a std::wstring to store data.
         * @param filename
         *      The filename or path.
         */
        File(const char* filename)
        {
            std::string s = filename;
            init( StringTools::toWideString(s) );
        }

        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a std::wstring to store data.
         * @param filename
         *      The filename or path.
         */
        File(const wchar_t* filename)
        {
            init( filename );
        }

        /**
         * @brief Creates a File Object.
         *      Allows easy access to the file name, path, full path with name, and extension.
         *      Uses a std::wstring to store data.
         * @param filename
         *      The filename or path.
         */
        File(wchar_t* filename)
        {
            init( filename );
        }

        /**
         * @brief Destroys a File Object.
         */
        ~File();

        /**
         * @brief Returns just the name of the file.
         *      The extension is not included.
         * @return std::wstring
         */
        std::wstring getFileName();

        /**
         * @brief Returns the full reference to the file.
         *      Has the full path, filename, and extension.
         * @return std::wstring
         */
        std::wstring getFullFileName();

        /**
         * @brief Returns the path to the file.
         * @return std::wstring
         */
        std::wstring getPath();

        /**
         * @brief Returns just the extension of the file.
         * @return std::wstring
         */
        std::wstring getExtension();

    private:
        void init(std::wstring filename);
        
        std::wstring fullFileName;
        int locationOfExtension;
        int locationOfFileName;
        
    };
    
}  //NAMESPACE glib END