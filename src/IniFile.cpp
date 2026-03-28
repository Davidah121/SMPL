#include "IniFile.h"
#include "StringTools.h"

namespace smpl
{
	IniFile::IniFile()
	{

	}
	IniFile::~IniFile()
	{

	}

	void IniFile::writeValue(const std::string& section, const std::string& key, const std::string& value)
	{
		//find section
		std::string lowercaseSection = StringTools::toLowercase(section);
		std::string lowercaseKey = StringTools::toLowercase(key);
		
		auto sectionIterator = sectionTable.find(lowercaseSection);
		
		if(sectionIterator == sectionTable.end())
		{
			//add section. SpecialCaseHere
			sectionIterator = sectionTable.insert( IniSectionData(lowercaseSection) );
		}
		
		//always add the key. Will replace upon discovering it exists already.
		sectionIterator->insert(IniKeyData(lowercaseKey, value));
	}

	void IniFile::removeSection(const std::string& section)
	{
		sectionTable.erase(StringTools::toLowercase(section));
	}

	void IniFile::removeKey(const std::string& section, const std::string& key)
	{
		auto it = sectionTable.find(StringTools::toLowercase(section));
		if(it != sectionTable.end())
		{
			it->erase(StringTools::toLowercase(key));
		}
	}

	
	SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData>::Iterator IniFile::begin()
	{
		return sectionTable.begin();
	}
	SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData>::Iterator IniFile::end()
	{
		return sectionTable.end();
	}

	SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData>::Iterator IniFile::find(const std::string& sectionID)
	{
		return sectionTable.find(StringTools::toLowercase(sectionID));
	}
	IniSectionData& IniFile::operator[](const std::string& sectionID)
	{
		auto it = find(sectionID);
		if(it == end())
			throw std::runtime_error("SECTION \"" + sectionID + "\" DOES NOT EXIST");
		return *it;
	}
	SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData>& IniFile::getSections()
	{
		return sectionTable;
	}


	void IniFile::save(File f)
	{
		SimpleFile writeFile = SimpleFile(f, SimpleFile::WRITE | SimpleFile::UTF8);
		if(writeFile.isOpen())
		{
			for(IniSectionData& s : sectionTable)
			{
				writeFile.writeString("["+s.getName()+"]");
				writeFile.writeLineBreak();

				for(IniKeyData& k : s)
				{
					writeFile.writeString(k.getName() + " = " + k.value);
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

			SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData>::Iterator sectionIterator;
			
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
						//Insert always - should automatically replace the data at that point too.
						std::string sectionName = l.substr(1, l.size()-2);
						sectionIterator = sectionTable.insertOrReplace(IniSectionData(sectionName));
					}

					size_t equalsIndex = l.find('=');
					if(equalsIndex != (size_t)-1)
					{
						if(sectionIterator != end())
						{
							//error in the file probably
							continue;
						}
						std::string keyName, value;

						if(equalsIndex >= 1)
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
						
						//Insert always - should automatically replace the data at that point too.
						sectionIterator->insert(IniKeyData(keyName, value));
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