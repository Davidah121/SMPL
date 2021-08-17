#pragma once
#include <iostream>

namespace glib
{

    class File
    {
    public:
        File(std::wstring filename);
        File(std::string filename);
        ~File();

        std::wstring getFileName();
        std::wstring getFullFileName();
        std::wstring getPath();
        std::wstring getExtension();

    private:
        void init(std::wstring filename);
        std::wstring fullFileName;
        int locationOfExtension;
        int locationOfFileName;
        
    };
    
}  //NAMESPACE glib END