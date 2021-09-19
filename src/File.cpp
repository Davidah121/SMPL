#include "File.h"
#include "StringTools.h"
#include <filesystem>

namespace glib
{

    File::~File()
    {
        
    }

    void File::init(std::wstring filename)
    {
        locationOfExtension = filename.find_last_of(L'.');
        size_t v1 = filename.find_last_of(L'/');
        size_t v2 = filename.find_last_of(L'\\');
        
        if(v1 >= v2 && v1 != -1)
        {
            locationOfFileName = v1;
        }
        else if(v2 >= v1 && v2 != -1)
        {
            locationOfFileName = v2;
        }
        else
        {
            locationOfFileName = 0;
        }

        fullFileName = L"";

        for(wchar_t& c : filename)
        {
            fullFileName += (wchar_t)c;
        }
    }

    std::wstring File::getFileName()
    {
        return fullFileName.substr(locationOfFileName, locationOfExtension-locationOfFileName);
    }

    std::wstring File::getFullFileName()
    {
        return fullFileName;
    }

    std::wstring File::getPath()
    {
        return fullFileName.substr(0, locationOfFileName);
    }

    std::wstring File::getExtension()
    {
        return fullFileName.substr(locationOfExtension, fullFileName.size()-locationOfExtension);
    }

} //NAMESPACE glib END