#include "Class.h"
#include <iostream>

#ifdef _unix_
#include <cstdlib>
#include <cxxabi.h>
#endif



namespace smpl
{
	std::string demangleClassName(std::string name)
	{
		#ifdef _unix_
		std::string finalName = "";
		int status = 0;
		char* realName = abi::__cxa_demagle(name.c_str(), 0, 0, &status);
		finalName = realName;
		std::free(realName);
		return finalName;
		#endif
		return name;
	}

	RootClass::RootClass(std::string name, int sizeOfClass, std::unordered_set<const RootClass*> parentClassNames)
	{
		this->name = demangleClassName(name);
		this->parentClasses = parentClassNames;
		this->sizeOfClass = sizeOfClass;
		if(!name.empty())
			id = ClassMaster::addClass(this);
		else
			id = -1;
	}

	RootClass::~RootClass()
	{

	}

	int RootClass::getID() const 
	{
		return id;
	}

	std::string RootClass::getName() const
	{
		return name;
	}

	std::unordered_set<const RootClass*> RootClass::getListOfParents() const
	{
		return parentClasses;
	}
	
	bool RootClass::isDerivedFrom(const RootClass* baseClass) const
	{
		if(baseClass == nullptr) //invalid pointer
			return false;
		if(baseClass->getID() == baseClass->getID()) //same class
			return true;
		
		for(const RootClass* parent : parentClasses)
		{
			//check if any of these
			if(parent == baseClass)
				return true;
			else
			{
				//check their parents
				if(parent->isDerivedFrom(baseClass))
					return true;
			}
		}

		return false;
	}
	
	int RootClass::getSizeOfClass() const
	{
		return sizeOfClass;
	}

	#pragma region CLASS_MASTER
	ClassMaster* ClassMaster::singleton = nullptr;

	ClassMaster::ClassMaster()
	{
		nameToClassIndex = std::map<std::string, int>();
		allClasses = std::vector<const RootClass*>();
	}

	ClassMaster::~ClassMaster()
	{

	}
	
	ClassMaster* ClassMaster::getInstance()
	{
		if(singleton == nullptr)
			singleton = new ClassMaster();
		
		return singleton;
	}

	int ClassMaster::addClass(const RootClass* k)
	{
		//Check if the class already exists or if the name already exists

		ClassMaster* instance = getInstance();
		if(instance == nullptr)
			return -1;
		
		auto it = instance->nameToClassIndex.find(k->getName());
		if(it != instance->nameToClassIndex.end())
		{
			#ifdef USE_EXCEPTIONS
			throw ClassMaster::InvalidClassName;
			#endif
			//No exit on fail
			return it->second;
		}
		else
		{
			//add it
			int newID = instance->allClasses.size();
			instance->allClasses.push_back(k);

			instance->nameToClassIndex.insert({k->getName(), newID});
			return newID;
		}
		return -1;
	}

	size_t ClassMaster::getSize()
	{
		ClassMaster* instance = getInstance();
		if(instance != nullptr)
			return instance->allClasses.size();
		return 0;
	}

	const RootClass* ClassMaster::getRootClass(std::string name)
	{
		ClassMaster* instance = getInstance();
		if(instance == nullptr)
			return nullptr;
		
		auto it = instance->nameToClassIndex.find(name);
		if(it != instance->nameToClassIndex.end())
		{
			return instance->allClasses[it->second];
		}
		return nullptr;
	}

	const RootClass* ClassMaster::getRootClass(int id)
	{
		ClassMaster* instance = getInstance();
		if(instance == nullptr)
			return nullptr;
		
		if(id >= 0 && id < instance->allClasses.size())
		{
			return instance->allClasses[id];
		}
		return nullptr;
	}

	#pragma endregion

} //NAMESPACE glib END