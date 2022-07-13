#include "File.h"
#include "StringTools.h"
#include <filesystem>

namespace glib
{

    File::~File()
    {
        
    }

    void File::init(std::string filename)
    {
        locationOfExtension = filename.find_last_of('.');
        size_t v1 = filename.find_last_of('/');
        size_t v2 = filename.find_last_of('\\');

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

    std::string File::getFileName()
    {
        return fullFileName.substr(locationOfFileName+1, locationOfExtension-(locationOfFileName+1));
    }

    std::string File::getFullFileName()
    {
        return fullFileName;
    }

    std::string File::getPath()
    {
        std::string path = fullFileName.substr(0, locationOfFileName);
        if(path.empty())
            return "./";
        else
            return path;
    }

    std::string File::getExtension()
    {
        if(locationOfExtension < fullFileName.size())
            return fullFileName.substr(locationOfExtension, fullFileName.size());
        return "";
    }

    std::string File::getFileNameWithExt()
    {
        if(locationOfFileName+1 < fullFileName.size())
            return fullFileName.substr(locationOfFileName+1, fullFileName.size());
        return "";
    }

} //NAMESPACE glib END