#include "Serializable.h"
#include "SimpleXml.h"
#include <string>

namespace smpl
{
	std::unordered_map<std::string, std::string> SerializedData::prettyName;
	std::function<std::string(void*, std::string)> SerializedData::externalSaver;
	std::function<void(void*, std::string, std::string)> SerializedData::externalLoader;

	void SerializedData::init()
	{
		addPrettyName(typeid(std::string).name(), "class std::string");
		addPrettyName(typeid(char).name(), "int8_t");
		addPrettyName(typeid(unsigned char).name(), "uint8_t");
		addPrettyName(typeid(short).name(), "int16_t");
		addPrettyName(typeid(unsigned short).name(), "uint16_t");
		addPrettyName(typeid(int).name(), "int32_t");
		addPrettyName(typeid(unsigned int).name(), "uint32_t");
		addPrettyName(typeid(unsigned long).name(), "ulong"); //A long may not be 64 bit. This specifies that it is a long without the longer "unsigned long" name
		addPrettyName(typeid(long long).name(), "int64_t");
		addPrettyName(typeid(unsigned long long).name(), "uint64_t");
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


	void SerializedData::serialize(XmlNode* parent)
	{
		//save as xml
		XmlNode* thisNode = new XmlNode();

		if(objType == TYPE_OBJECT)
		{
			thisNode->setTitle("SerializedObject");
			thisNode->addAttribute("TYPE", type);
			thisNode->addAttribute("NAME", name);
			std::unordered_map<std::string, SerializedData> variables = ((SerializedObject*)this)->getSerializedVariables();

			for(std::pair<const std::string, SerializedData>& v : variables)
			{
				v.second.serialize(thisNode);
			}
		}
		else
		{
			std::string valueString = "";
			thisNode->setTitle("SerializedData");
			thisNode->addAttribute("TYPE", type);
			thisNode->addAttribute("NAME", name);

			if( type == typeid(double).name()
			|| type == getAlternateName(typeid(double).name()) )
			{
				//save as double
				valueString += std::to_string(*((double*)data));
			}
			else if( type == typeid(float).name()
			|| type == getAlternateName(typeid(float).name()) )
			{
				//save as float
				valueString += std::to_string(*((float*)data));
			}
			else if(type == typeid(char).name()
			|| type == getAlternateName(typeid(char).name()) )
			{
				//save as char
				valueString += std::to_string(*((char*)data));
			}
			else if(type == typeid(unsigned char).name()
			|| type == getAlternateName(typeid(unsigned char).name()) )
			{
				//save as unsigned char
				valueString += std::to_string(*((unsigned char*)data));
			}
			else if(type == typeid(short).name()
			|| type == getAlternateName(typeid(short).name()) )
			{
				//save as short
				valueString += std::to_string(*((short*)data));
			}
			else if(type == typeid(unsigned short).name()
			|| type == getAlternateName(typeid(unsigned short).name()) )
			{
				//save as unsigned short
				valueString += std::to_string(*((unsigned short*)data));
			}
			else if(type == typeid(int).name()
			|| type == getAlternateName(typeid(int).name()) )
			{
				//save as int
				valueString += std::to_string(*((int*)data));
			}
			else if(type == typeid(unsigned int).name()
			|| type == getAlternateName(typeid(unsigned int).name()) )
			{
				//save as unsigned int
				valueString += std::to_string(*((unsigned int*)data));
			}
			else if(type == typeid(long).name()
			|| type == getAlternateName(typeid(long).name()) )
			{
				//save as long
				valueString += std::to_string(*((long*)data));
			}
			else if(type == typeid(unsigned long).name()
			|| type == getAlternateName(typeid(unsigned long).name()) )
			{
				//save as unsigned long
				valueString += std::to_string(*((unsigned long*)data));
			}
			else if(type == typeid(long long).name()
			|| type == getAlternateName(typeid(long long).name()) )
			{
				//save as long long
				valueString += std::to_string(*((long long*)data));
			}
			else if(type == typeid(unsigned long long).name()
			|| type == getAlternateName(typeid(unsigned long long).name()) )
			{
				//save as unsigned long long
				valueString += std::to_string(*((unsigned long long*)data));
			}
			else if(type == typeid(bool).name()
			|| type == getAlternateName(typeid(bool).name()) )
			{
				valueString += std::to_string(*((bool*)data));
			}
			else if(type == typeid(std::string).name()
			|| type == getAlternateName(typeid(std::string).name()) )
			{
				//save as string
				valueString += ((std::string*)data)->c_str();
			}
			else
			{
				//try external loader
				if(externalSaver != nullptr)
					valueString += externalSaver(data, type);
			}

			thisNode->addAttribute("VALUE", valueString);
		}

		parent->addChild(thisNode);
	}

	void SerializedData::deserialize(XmlNode* n)
	{
		//read xml
		if(n == nullptr)
			return;
		
		if(objType == TYPE_OBJECT)
		{
			std::unordered_map<std::string, SerializedData> variables = ((SerializedObject*)this)->getSerializedVariables();

			///put into an unordered map
			for(ChildNode& c : n->getChildNodes())
			{
				if(c.type != ChildNode::TYPE_NODE)
					continue;
				
				XmlNode* childNode = c.node;
				HashPair<std::string, std::string>* nameAttrib = childNode->getAttribute("NAME");
				SerializedData dataToModify;
				if(nameAttrib != nullptr)
				{
					auto varIterator = variables.find(nameAttrib->data);
					if(varIterator != variables.end())
					{
						dataToModify = varIterator->second;
					}
				}
				else
				{
					//ignore
					continue;
				}
				dataToModify.serialize(childNode);
			}
		}
		else
		{
			HashPair<std::string, std::string>* typeAttrib = n->getAttribute("TYPE");
			HashPair<std::string, std::string>* valueAttrib = n->getAttribute("VALUE");
			
			if(valueAttrib == nullptr && typeAttrib == nullptr)
			{
				//ignore
				return;
			}

			if( typeAttrib->data == typeid(double).name()
			|| typeAttrib->data == getAlternateName(typeid(double).name()) )
			{
				*((double*)data) = std::stod(valueAttrib->data);
			}
			else if( typeAttrib->data == typeid(float).name()
			|| typeAttrib->data == getAlternateName(typeid(float).name()) )
			{
				*((float*)data) = std::stof(valueAttrib->data);
			}
			else if(typeAttrib->data == typeid(char).name()
			|| typeAttrib->data == getAlternateName(typeid(char).name()) )
			{
				*((char*)data) = std::stoi(valueAttrib->data);
			}
			else if(typeAttrib->data == typeid(unsigned char).name()
			|| typeAttrib->data == getAlternateName(typeid(unsigned char).name()) )
			{
				*((unsigned char*)data) = std::stoi(valueAttrib->data);
			}
			else if(typeAttrib->data == typeid(short).name()
			|| typeAttrib->data == getAlternateName(typeid(short).name()) )
			{
				*((short*)data) = std::stoi(valueAttrib->data);
			}
			else if(typeAttrib->data == typeid(unsigned short).name()
			|| typeAttrib->data == getAlternateName(typeid(unsigned short).name()) )
			{
				*((unsigned short*)data) = std::stoi(valueAttrib->data);
			}
			else if(typeAttrib->data == typeid(int).name()
			|| typeAttrib->data == getAlternateName(typeid(int).name()) )
			{
				*((int*)data) = std::stoi(valueAttrib->data);
			}
			else if(typeAttrib->data == typeid(unsigned int).name()
			|| typeAttrib->data == getAlternateName(typeid(unsigned int).name()) )
			{
				*((unsigned int*)data) = std::stoul(valueAttrib->data); //Go one up for unsigned int
			}
			else if(typeAttrib->data == typeid(long).name()
			|| typeAttrib->data == getAlternateName(typeid(long).name()) )
			{
				*((long*)data) = std::stol(valueAttrib->data);
			}
			else if(typeAttrib->data == typeid(unsigned long).name()
			|| typeAttrib->data == getAlternateName(typeid(unsigned long).name()) )
			{
				*((unsigned long*)data) = std::stoul(valueAttrib->data); //Go one up for unsigned int
			}
			else if(typeAttrib->data == typeid(long long).name()
			|| typeAttrib->data == getAlternateName(typeid(long long).name()) )
			{
				*((long long*)data) = std::stoll(valueAttrib->data);
			}
			else if(typeAttrib->data == typeid(unsigned long long).name()
			|| typeAttrib->data == getAlternateName(typeid(unsigned long long).name()) )
			{
				*((unsigned long long*)data) = std::stoull(valueAttrib->data); //Go one up for unsigned int
			}
			else if(typeAttrib->data == typeid(bool).name()
			|| typeAttrib->data == getAlternateName(typeid(bool).name()) )
			{
				*((bool*)data) = std::stoi(valueAttrib->data);
			}
			else if(typeAttrib->data == typeid(std::string).name()
			|| typeAttrib->data == getAlternateName(typeid(std::string).name()) )
			{
				*((std::string*)data) = valueAttrib->data;
			}
			else
			{
				//pass off to external loader.
				if(externalLoader != nullptr)
					externalLoader(data, typeAttrib->data, valueAttrib->data);
			}
		}
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

	void SerializedData::setExternalSaver(std::function<std::string(void*, std::string)> extSaver)
	{
		externalSaver = extSaver;
	}

	void SerializedData::setExternalLoader(std::function<void(void*, std::string, std::string)> extLoader)
	{
		externalLoader = extLoader;
	}

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