#include "IniFile.h"

namespace glib
{
    IniFile::IniFile()
    {

    }
    IniFile::~IniFile()
    {

    }

    void IniFile::writeValue(std::string section, std::string key, std::string value)
    {
        //find section
        int indexOfSection = findSection(section);
        //find key
        int indexOfKey = findKey(key, indexOfSection);

        if(indexOfSection < 0)
        {
            //add section
            indexOfSection = data.size();
            data.push_back( SectionData{section, {} } );
        }

        if(indexOfKey < 0)
        {
            //add key
            indexOfKey = data[indexOfSection].keys.size();
            data[indexOfSection].keys.push_back( KeyData{} );
        }
        
        data[indexOfSection].keys[indexOfKey] = KeyData{key, value};
    }

    void IniFile::removeSection(std::string section)
    {
        int indexOfSection = findSection(section);
        if(indexOfSection >= 0)
        {
            std::vector<SectionData> nData;
            for(int i=0; i<data.size(); i++)
            {
                if(i != indexOfSection)
                    nData.push_back(data[i]);
            }
            data = nData;
        }
    }

    void IniFile::removeKey(std::string section, std::string key)
    {
        int indexOfSection = findSection(section);
        if(indexOfSection >= 0)
        {
            int indexOfKey = findKey(key, indexOfSection);

            if(indexOfKey >= 0)
            {
                std::vector<KeyData> nData;
                for(int i=0; i<data[indexOfSection].keys.size(); i++)
                {
                    if(i != indexOfKey)
                        nData.push_back(data[indexOfSection].keys[i]);
                }
                data[indexOfSection].keys = nData;
            }
        }
    }
    
    std::string IniFile::readValue(std::string section, std::string key)
    {
        //find section
        int indexOfSection = findSection(section);
        //find key
        int indexOfKey = findKey(key, indexOfSection);

        if(indexOfKey >= 0)
            return data[indexOfSection].keys[indexOfKey].value;
        return "";
    }

    std::vector<std::string> IniFile::getKeys(std::string section)
    {
        //find section
        int indexOfSection = findSection(section);
        
        if(indexOfSection >= 0 && indexOfSection < data.size())
        {
            std::vector<std::string> keyNames;
            for(KeyData& k : data[indexOfSection].keys)
                keyNames.push_back( k.key );

            return keyNames;
        }
        return {};
    }
    std::vector<std::string> IniFile::getSections()
    {
        std::vector<std::string> result;
        for(SectionData& s : data)
        {
            result.push_back( s.name );
        }
        return result;
    }

    
    int IniFile::findSection(std::string section)
    {
        int index = 0;
        while(index < data.size())
        {
            if(StringTools::equalsIgnoreCase(section, data[index].name))
            {
                return index;
            }
            index++;
        }
        return -1;
    }
    int IniFile::findKey(std::string key, int sectionID)
    {
        if(sectionID >= 0 && sectionID < data.size())
        {
            std::vector<KeyData> keysPossible = data[sectionID].keys;
            int index = 0;
            while(index < keysPossible.size())
            {
                if(StringTools::equalsIgnoreCase(key, keysPossible[index].key))
                {
                    return index;
                }
                index++;
            }
        }
        return -1;
    }

    void IniFile::save(File f)
    {
        SimpleFile writeFile = SimpleFile(f, SimpleFile::WRITE | SimpleFile::UTF8);
        if(writeFile.isOpen())
        {
            for(SectionData& s : data)
            {
                writeFile.writeString("["+s.name+"]");
                writeFile.writeLineBreak();

                for(KeyData& k : s.keys)
                {
                    writeFile.writeString(k.key + " = " + k.value);
                    writeFile.writeLineBreak();
                }
            }
        }
        writeFile.close();
    }

    bool IniFile::load(File f)
    {
        SimpleFile inputFile = SimpleFile(f, SimpleFile::READ | SimpleFile::UTF8);
        if(inputFile.isOpen())
        {
            std::vector<std::string> lines = inputFile.readFullFileString();
            inputFile.close();

            int indexOfSection = -1;
            
            for(std::string& l : lines)
            {
                //remove white space
                l = StringTools::removeWhitespace(l, true);
                //remove comments. starts with ';' or '#'
                size_t firstSemi = l.find_first_of(';');
                size_t firstPound = l.find_first_of('#');
                size_t commentStart = (firstSemi < firstPound) ? firstSemi : firstPound;
                
                //Get everything before the comment.
                l = l.substr(0, commentStart);
                
                if(l.size() > 0)
                {
                    if(l.front() == '[' && l.back() == ']')
                    {
                        //should check if the name is already in use
                        std::string sectionName = l.substr(1, l.size()-2);
                        indexOfSection = findSection(sectionName);
                        if(indexOfSection < 0)
                        {
                            indexOfSection = data.size();
                            data.push_back( SectionData{sectionName, {}} );
                        }
                    }

                    size_t equalsIndex = l.find('=');
                    if(equalsIndex != (size_t)-1)
                    {
                        if(indexOfSection < 0)
                        {
                            //error in the file probably
                            continue;
                        }
                        std::string keyName, value;

                        if(equalsIndex > 1)
                        {
                            keyName = l.substr(0, equalsIndex);
                        }
                        else
                        {
                            //error in the file. Key has to be more than 0 letters
                            continue;
                        }

                        if(equalsIndex+1 < l.size())
                        {
                            value = l.substr(equalsIndex+1);
                        }
                        
                        //should check if the key is already in use
                        int indexOfKey = findKey(keyName, indexOfSection);
                        if(indexOfKey < 0)
                        {
                            data[indexOfSection].keys.push_back(KeyData{keyName, value});
                        }
                        else
                        {
                            data[indexOfSection].keys[indexOfKey].value = value;
                        }
                    }
                }
            }
        }
        else
        {
            return false;
        }
        return true;
    }
}