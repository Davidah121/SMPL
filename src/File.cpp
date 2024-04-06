#include "File.h"
#include "StringTools.h"
#include "SimpleDir.h"

namespace smpl
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
                //must be v2
                locationOfFileName = v2+1;
            }
        }
        else
        {
            if(v2 == SIZE_MAX)
            {
                //must be v1
                locationOfFileName = v1+1;
            }
            else
            {
                //the max of the 2
                locationOfFileName = (v1 > v2) ? v1+1 : v2+1;
            }
        }

        if(locationOfExtension < locationOfFileName)
        {
            //no extension
            locationOfExtension = SIZE_MAX;
        }

        fullFileName = filename;
    }

    std::string File::getFileName()
    {
        if(locationOfFileName < fullFileName.size())
            return fullFileName.substr(locationOfFileName, locationOfExtension-(locationOfFileName));
        return "";
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
        if(locationOfFileName < fullFileName.size())
            return fullFileName.substr(locationOfFileName, fullFileName.size());
        return "";
    }

    bool File::isDirectory()
    {
        return SimpleDir::isDirectory(fullFileName);
    }

    bool File::isFile()
    {
        return SimpleDir::isFile(fullFileName);
    }

    bool File::doesExist()
    {
        return SimpleDir::doesExist(fullFileName);
    }

} //NAMESPACE glib END