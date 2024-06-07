#include "Serializable.h"
#include "SimpleXml.h"
#include <string>

namespace smpl
{
	std::unordered_map<std::string, std::function<void(std::vector<std::string>, StreamableList<unsigned char>, SerializedData)>> SerializedData::loadFunctions;

	std::vector<std::string> extractClassAndTemplates(std::string s)
	{
		std::vector<std::string> output;
		std::vector<std::string> splits = StringTools::splitStringMultipleDeliminators(s, ",<>");
		//don't want empty spaces either
		for(std::string arg : splits)
		{
			bool somethingOtherThanSpace = false;
			for(char c : arg)
			{
				if(c != ' ')
				{
					somethingOtherThanSpace = true;
					break;
				}
			}
			if(somethingOtherThanSpace)
				output.push_back(arg);
		}
		
		return output;
	}

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
		this->type = typeString;
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

	void SerializedData::serialize(StreamableList<unsigned char> data)
	{

	}

	void SerializedData::deserialize(StreamableList<unsigned char> data)
	{
		//its stored as the struct Writable Serialized Data is laid out.
		//not all fields are used. Check type first
		WritableSerialzedData objData;

		objData.type = data.get();
		if(objData.type == TYPE_OBJECT)
		{
			if(this->objType != TYPE_OBJECT)
			{
				//throw exception
			}

			//it contains a name (may be empty).
			data.get((unsigned char*)&objData.size, 4);
			for(int i=0; i<objData.size; i++)
			{
				this->name += (char)data.get();
			}

			//for each serialized variable, call deserialize on it.
			//assume order matters
			auto variables = ((SerializedObject*)this)->getSerializedVariables();
			for(auto var : variables)
			{
				var.second.deserialize(data);
			}
		}
		else
		{
			if(this->objType != TYPE_DATA)
			{
				//throw exception
			}
			
			std::vector<std::string> args = extractClassAndTemplates(this->getType());
			std::function<void(std::vector<std::string>, StreamableList<unsigned char>, SerializedData)> loader;
			auto it = loadFunctions.find(args[0]);
			if(it != loadFunctions.end())
			{
				loader = it->second;
				loader(args, data, *this);
			}
			else
			{
				//throw exception
			}
		}
	}

	const RootClass SerializedObject::globalClass = CREATE_ROOT_CLASS(SerializedObject, &Object::globalClass);

	SerializedObject::SerializedObject()
	{
	}
	SerializedObject::~SerializedObject()
	{

	}

}