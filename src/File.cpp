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

    std::string File::getFileName() const
    {
        if(locationOfFileName < fullFileName.size())
            return fullFileName.substr(locationOfFileName, locationOfExtension-(locationOfFileName));
        return "";
    }

    std::string File::getFullFileName() const
    {
        return fullFileName;
    }

    std::string File::getPath() const
    {
        std::string path = fullFileName.substr(0, locationOfFileName);
        if(path.empty())
            return "./";
        else
            return path;
    }

    std::string File::getExtension() const
    {
        if(locationOfExtension < fullFileName.size())
            return fullFileName.substr(locationOfExtension, fullFileName.size());
        return "";
    }

    std::string File::getFileNameWithExt() const
    {
        if(locationOfFileName < fullFileName.size())
            return fullFileName.substr(locationOfFileName, fullFileName.size());
        return "";
    }

    bool File::isDirectory() const
    {
        return SimpleDir::isDirectory(fullFileName);
    }

    bool File::isFile() const
    {
        return SimpleDir::isFile(fullFileName);
    }

    bool File::doesExist() const
    {
        return SimpleDir::doesExist(fullFileName);
    }

} //NAMESPACE glib END