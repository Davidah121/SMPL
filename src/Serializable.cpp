#include "Serializable.h"
#include "SimpleXml.h"
#include <string>

namespace smpl
{
	std::unordered_map<std::string, std::string> SerializedData::prettyName;

	SerializedData::SerializedData()
	{
		
	}

	SerializedData::SerializedData(void* d, std::string name, std::string typeString, size_t size)
	{
		init(d, name, typeString, size);
		objType = TYPE_DATA;
	}

	SerializedData::SerializedData(SerializedObject* d, std::string name, std::string typeString, size_t size)
	{
		init(d, name, typeString, size);
		objType = TYPE_OBJECT;
	}

	void SerializedData::init(void* d, std::string name, std::string typeString, size_t size)
	{
		this->type = SerializedData::getAlternateName(typeString);
		this->name = name;
		this->size = size;
		data = d;
	}

	SerializedData::~SerializedData()
	{

	}

	void* SerializedData::getData()
	{
		return data;
	}

	std::string SerializedData::getType()
	{
		return type;
	}

	std::string SerializedData::getName()
	{
		return name;
	}
	
	size_t SerializedData::getNumberOfElements()
	{
		return size;
	}
	
	bool SerializedData::getObjectType()
	{
		return objType;
	}
	
	void SerializedData::addPrettyName(std::string className, std::string prettyName)
	{
		SerializedData::prettyName[prettyName] = className;
		SerializedData::prettyName[className] = prettyName;
	}

	std::string SerializedData::getAlternateName(std::string className)
	{
		auto altNameIt = SerializedData::prettyName.find(className);
		std::string altType = className;

		if(altNameIt != SerializedData::prettyName.end())
		{
			altType = altNameIt->second;
		}
		return altType;
	}


	const RootClass SerializedObject::globalClass = CREATE_ROOT_CLASS(SerializedObject, &Object::globalClass);

	SerializedObject::SerializedObject()
	{
	}
	SerializedObject::~SerializedObject()
	{

	}

	// std::unordered_map<std::string, SerializedData> SerializedObject::getVariables()
	// {
	// 	return {};
	// }

}