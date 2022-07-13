#pragma once
#include "SimpleFile.h"
#include "StringTools.h"

namespace glib
{
    class IniFile
    {
    private:
        struct KeyData
        {
            std::string key;
            std::string value;
        };
        struct SectionData
        {
            std::string name;
            std::vector<KeyData> keys;
        };

    public:
        IniFile();
        ~IniFile();

        void writeValue(std::string section, std::string key, std::string value);
        void removeSection(std::string section);
        void removeKey(std::string section, std::string key);

        std::string readValue(std::string section, std::string key);
        std::vector<std::string> getKeys(std::string section);
        std::vector<std::string> getSections();

        void save(File f);
        bool load(File f); //TODO: Add support for comments and remove whitespace

    private:
        int findSection(std::string section);
        int findKey(std::string key, int sectionID);
        std::vector<SectionData> data;
    };
}