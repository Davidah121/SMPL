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

        if(v1 == SIZE_MAX)
        {
            //must be v2 or 0
            if(v2 == SIZE_MAX)
            {
                //must be v1
                locationOfFileName = 0;
            }
            else
            {
                //the max of the 2
                locationOfFileName = v2;
            }
        }
        else
        {
            if(v2 == SIZE_MAX)
            {
                //must be v1
                locationOfFileName = v1;
            }
            else
            {
                //the max of the 2
                locationOfFileName = (v1 > v2) ? v1 : v2;
            }
        }

        fullFileName = filename;
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
        std::wstring path = fullFileName.substr(0, locationOfFileName);
        if(path.empty())
            return L"./";
        else
            return path;
    }

    std::wstring File::getExtension()
    {
        return fullFileName.substr(locationOfExtension, fullFileName.size()-locationOfExtension);
    }

    std::wstring File::getFileNameWithExt()
    {
        return fullFileName.substr(locationOfFileName, fullFileName.size());
    }

} //NAMESPACE glib END