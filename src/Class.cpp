#include "Class.h"
#include <iostream>

#pragma region CLASS

namespace glib
{
	RootClass::RootClass(std::string name, std::unordered_set<std::string> parentClassNames)
	{
		this->name = name;
		this->parentClassesByName = parentClassNames;
		if(name != "")
		{
			ClassMaster::addClass(this);
		}
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

	std::unordered_set<std::string> RootClass::getListOfParents() const
	{
		return parentClassesByName;
	}


	Class::Class(const RootClass& rootClass)
	{
		this->classID = rootClass.getID();
	}
	
	Class::Class()
	{
		
	}

	Class::~Class()
	{

	}

	std::string Class::getClassName() const
	{
		const RootClass* rootClass = ClassMaster::getRootClass(classID);
		if(rootClass != nullptr)
			return rootClass->getName();
		return "";
	}

	bool Class::operator==(const Class other) const
	{
		return classID == other.classID;
	}
	
	bool Class::operator!=(const Class other) const
	{
		return classID != other.classID;
	}

	bool Class::operator==(const RootClass& other) const
	{
		return classID == other.getID();
	}
	
	bool Class::operator!=(const RootClass& other) const
	{
		return classID != other.getID();
	}
	
	int Class::getClassID()
	{
		return classID;
	}
	
	const std::unordered_set<std::string> Class::getParentClasses()
	{
		const RootClass* rootClass = ClassMaster::getRootClass(classID);
		if(rootClass != nullptr)
			return rootClass->getListOfParents();
		return {};
	}

	const RootClass* Class::getRootClass()
	{
		return ClassMaster::getRootClass(classID);
	}

	#pragma endregion

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