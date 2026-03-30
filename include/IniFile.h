#pragma once
#include "BuildOptions.h"
#include "SimpleHashTable_Hash.h"
#include "StandardTypes.h"
#include "SimpleFile.h"
#include "StringTools.h"
#include "SimpleHashTable.h"
#include <stdexcept>
#include <string>

namespace smpl
{
	class DLL_OPTION IniFile;
	
	class IniKeyData
	{
	public:
		IniKeyData(const std::string& name, const std::string& value)
		{
			this->name = StringTools::toLowercase(name);
			this->value = value;
		}
		IniKeyData(const IniKeyData& other) = default;
		IniKeyData& operator=(const IniKeyData& other) = default;
		IniKeyData(IniKeyData&& other) = default;
		IniKeyData& operator=(IniKeyData&& other) = default;

		const std::string& getName() const
		{
			return name;
		}
		std::string value;

	private:
		std::string name;
	};
	
	struct CompareIniKeyData
	{
		using is_transparent = void;
		bool operator()(const IniKeyData& v, const IniKeyData& v2) const
		{
			return v.getName() == v2.getName();
		}
		
		bool operator()(const std::string& v, const IniKeyData& v2) const
		{
			return v == v2.getName();
		}
		
		bool operator()(const IniKeyData& v, const std::string& v2) const
		{
			return v.getName() == v2;
		}
	};

	template<>
	struct RapidHash<IniKeyData> {
		using is_transparent = void;
		size_t operator()(const IniKeyData& p) const noexcept
		{
			return RapidHash<std::string>{}(p.getName());
		}
		size_t operator()(const std::string& p) const noexcept
		{
			return RapidHash<std::string>{}(p);
		}
	};

	class IniSectionData
	{
	public:
		
		IniSectionData(const std::string& name)
		{
			this->name = name;
		}
		IniSectionData(const IniSectionData& other) = default;
		IniSectionData& operator=(const IniSectionData& other) = default;
		IniSectionData(IniSectionData&& other) = default;
		IniSectionData& operator=(IniSectionData&& other) = default;

		const std::string& getName() const
		{
			return name;
		}
		void insert(const IniKeyData& v)
		{
			keys.insertOrReplace(v);
		}
		// void insert(IniKeyData&& v)
		// {
		// 	keys.insertOrReplace(v);
		// }
		void erase(const std::string& v)
		{
			keys.erase(v);
		}

		SimpleHashSet<IniKeyData, RapidHash<IniKeyData>, CompareIniKeyData>::Iterator find(const std::string& keyID)
		{
			return keys.find(StringTools::toLowercase(keyID));
		}
		
		SimpleHashSet<IniKeyData, RapidHash<IniKeyData>, CompareIniKeyData>::ConstIterator find(const std::string& keyID) const
		{
			return keys.find(StringTools::toLowercase(keyID));
		}

		SimpleHashSet<IniKeyData, RapidHash<IniKeyData>, CompareIniKeyData>::Iterator begin()
		{
			return keys.begin();
		}
		
		SimpleHashSet<IniKeyData, RapidHash<IniKeyData>, CompareIniKeyData>::Iterator end()
		{
			return keys.end();
		}

		SimpleHashSet<IniKeyData, RapidHash<IniKeyData>, CompareIniKeyData>::ConstIterator begin() const
		{
			return keys.cbegin();
		}
		
		SimpleHashSet<IniKeyData, RapidHash<IniKeyData>, CompareIniKeyData>::ConstIterator end() const
		{
			return keys.cend();
		}

		IniKeyData& operator[](const std::string& keyID)
		{
			auto it = find(keyID);
			if(it == keys.end())
				throw std::runtime_error("SECTION \"" + name + "\" DOES NOT HAVE THE KEY \"" + keyID + "\"");
			return *it;
		}
		const IniKeyData& operator[](const std::string& keyID) const
		{
			auto it = find(keyID);
			if(it == keys.end())
				throw std::runtime_error("SECTION \"" + name + "\" DOES NOT HAVE THE KEY \"" + keyID + "\"");
			return *it;
		}
	private:
		std::string name;
		SimpleHashSet<IniKeyData, RapidHash<IniKeyData>, CompareIniKeyData> keys;
	};

	struct CompareIniSectionData
	{
		using is_transparent = void;
		bool operator()(const IniSectionData& v, const IniSectionData& v2) const
		{
			return v.getName() == v2.getName();
		}
		
		bool operator()(const std::string& v, const IniSectionData& v2) const
		{
			return v == v2.getName();
		}
		
		bool operator()(const IniSectionData& v, const std::string& v2) const
		{
			return v.getName() == v2;
		}
	};
	template<>
	struct RapidHash<IniSectionData> {
		using is_transparent = void;
		size_t operator()(const IniSectionData& p) const noexcept
		{
			return RapidHash<std::string>{}(p.getName());
		}
		
		size_t operator()(const std::string& p) const noexcept
		{
			return RapidHash<std::string>{}(p);
		}
	};

    class DLL_OPTION IniFile
    {
    private:

    public:
        IniFile();
        ~IniFile();

		IniFile(const IniFile& other) = default;
		IniFile& operator=(const IniFile& other) = default;
		IniFile(IniFile&& other) = default;
		IniFile& operator=(IniFile&& other) = default;

        void writeValue(const std::string& section, const std::string& key, const std::string& value);
        void removeSection(const std::string& section);
        void removeKey(const std::string& section, const std::string& key);

		SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData>::Iterator begin();
		SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData>::Iterator end();
		
		
		SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData>::Iterator find(const std::string& sectionID);
		IniSectionData& operator[](const std::string& sectionID);
        SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData>& getSections();

        void save(File f);
        bool load(File f); //TODO: Add support for comments and remove whitespace

    private:
        SimpleHashSet<IniSectionData, RapidHash<IniSectionData>, CompareIniSectionData> sectionTable;
    };
	

}
