#include "Serializable.h"
#include <string>

namespace glib
{
	std::unordered_map<std::string, std::string> SerializedObject::prettyName;
	std::function<std::string(void*, std::string)> SerializedObject::externalSaver;
	std::function<void(void*, std::string, std::string)> SerializedObject::externalLoader;

	SerializedData::SerializedData()
	{
		
	}

	SerializedData::SerializedData(void* d, std::string name, std::string typeString)
	{
		this->type = SerializedObject::getAlternateName(typeString);
		this->name = name;
		data = d;
		parent = nullptr;
		objType = TYPE_DATA;
	}

	SerializedData::SerializedData(void* d, std::string name, std::string typeString, SerializedObject* parent)
	{
		this->type = SerializedObject::getAlternateName(typeString);
		this->name = name;
		data = d;
		this->parent = parent;
		objType = TYPE_DATA;

		if(parent != nullptr)
		{
			parent->variables.push_back(this);
		}
	}

	SerializedData::SerializedData(SerializedObject* d, std::string name, std::string typeString)
	{
		this->type = SerializedObject::getAlternateName(typeString);
		this->name = name;
		data = d;
		parent = nullptr;
		objType = TYPE_OBJECT;

		d->type = SerializedObject::getAlternateName(typeString);
		d->name = name;
		d->data = d;
		d->objType = TYPE_OBJECT;
	}

	SerializedData::SerializedData(SerializedObject* d, std::string name, std::string typeString, SerializedObject* parent)
	{
		this->type = SerializedObject::getAlternateName(typeString);
		this->name = name;
		data = d;
		this->parent = parent;
		objType = TYPE_OBJECT;

		if(parent != nullptr)
		{
			parent->variables.push_back(d);
		}
		
		d->type = SerializedObject::getAlternateName(typeString);
		d->name = name;
		d->data = d;
		d->objType = TYPE_OBJECT;
		d->parent = parent;
	}

	SerializedData::~SerializedData()
	{

	}

	void* SerializedData::getData()
	{
		return data;
	}

	std::string SerializedData::getName()
	{
		return name;
	}

	std::string SerializedData::getType()
	{
		return type;
	}

	void SerializedObject::init()
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

	SerializedObject::SerializedObject() : SerializedData()
	{
		objType = TYPE_OBJECT;
	}
	SerializedObject::~SerializedObject()
	{

	}

	void SerializedObject::serialize(std::string s)
	{
		XmlNode* thisNode = new XmlNode();
		thisNode->setTitle("SERIALIZER");
		serialize(thisNode);

		SimpleXml xml = SimpleXml(); //Pointers will all be deleted when function exits
		xml.addNode(thisNode);
		xml.save(s);
	}

	void SerializedObject::serialize(XmlNode* parent)
	{
		//save as xml

		XmlNode* thisNode = new XmlNode();
		thisNode->setTitle("SerializedObject");
		thisNode->addAttribute("TYPE", type);
		thisNode->addAttribute("NAME", name);

		for(SerializedData* temp : variables)
		{
			if(temp->objType == TYPE_OBJECT)
			{
				((SerializedObject*)temp)->serialize(thisNode);
			}
			else
			{
				std::string valueString = "";
				XmlNode* nNode = new XmlNode();
				nNode->setTitle("SerializedData");
				nNode->addAttribute("TYPE", temp->getType());
				nNode->addAttribute("NAME", temp->getName());

				if( temp->getType() == typeid(double).name()
				|| temp->getType() == getAlternateName(typeid(double).name()) )
				{
					//save as double
					valueString += std::to_string(*((double*)temp->data));
				}
				else if( temp->getType() == typeid(float).name()
				|| temp->getType() == getAlternateName(typeid(float).name()) )
				{
					//save as float
					valueString += std::to_string(*((float*)temp->data));
				}
				else if(temp->getType() == typeid(char).name()
				|| temp->getType() == getAlternateName(typeid(char).name()) )
				{
					//save as char
					valueString += std::to_string(*((char*)temp->data));
				}
				else if(temp->getType() == typeid(unsigned char).name()
				|| temp->getType() == getAlternateName(typeid(unsigned char).name()) )
				{
					//save as unsigned char
					valueString += std::to_string(*((unsigned char*)temp->data));
				}
				else if(temp->getType() == typeid(short).name()
				|| temp->getType() == getAlternateName(typeid(short).name()) )
				{
					//save as short
					valueString += std::to_string(*((short*)temp->data));
				}
				else if(temp->getType() == typeid(unsigned short).name()
				|| temp->getType() == getAlternateName(typeid(unsigned short).name()) )
				{
					//save as unsigned short
					valueString += std::to_string(*((unsigned short*)temp->data));
				}
				else if(temp->getType() == typeid(int).name()
				|| temp->getType() == getAlternateName(typeid(int).name()) )
				{
					//save as int
					valueString += std::to_string(*((int*)temp->data));
				}
				else if(temp->getType() == typeid(unsigned int).name()
				|| temp->getType() == getAlternateName(typeid(unsigned int).name()) )
				{
					//save as unsigned int
					valueString += std::to_string(*((unsigned int*)temp->data));
				}
				else if(temp->getType() == typeid(long).name()
				|| temp->getType() == getAlternateName(typeid(long).name()) )
				{
					//save as long
					valueString += std::to_string(*((long*)temp->data));
				}
				else if(temp->getType() == typeid(unsigned long).name()
				|| temp->getType() == getAlternateName(typeid(unsigned long).name()) )
				{
					//save as unsigned long
					valueString += std::to_string(*((unsigned long*)temp->data));
				}
				else if(temp->getType() == typeid(long long).name()
				|| temp->getType() == getAlternateName(typeid(long long).name()) )
				{
					//save as long long
					valueString += std::to_string(*((long long*)temp->data));
				}
				else if(temp->getType() == typeid(unsigned long long).name()
				|| temp->getType() == getAlternateName(typeid(unsigned long long).name()) )
				{
					//save as unsigned long long
					valueString += std::to_string(*((unsigned long long*)temp->data));
				}
				else if(temp->getType() == typeid(bool).name()
				|| temp->getType() == getAlternateName(typeid(bool).name()) )
				{
					valueString += std::to_string(*((bool*)temp->data));
				}
				else if(temp->getType() == typeid(std::string).name()
				|| temp->getType() == getAlternateName(typeid(std::string).name()) )
				{
					//save as string
					valueString += ((std::string*)temp->data)->c_str();
				}
				else
				{
					//try external loader
					if(externalSaver != nullptr)
						valueString += externalSaver(temp->data, temp->getType());
				}

				nNode->addAttribute("VALUE", valueString);
				thisNode->addChild(nNode);
			}
		}

		if(parent != nullptr)
		{
			parent->addChild(thisNode);
		}
	}

	void SerializedObject::deserialize(std::string s)
	{
		SimpleXml serData = SimpleXml();
		serData.load(s);

		if(serData.getNodes().size() > 0)
		{
			//Skip Serializer and grab first child.
			auto c = serData.getNodes()[0]->getChildNodes()[0];
			deserialize(c.node);
		}
	}

	void SerializedObject::deserialize(XmlNode* n)
	{
		//read xml
		if(n == nullptr)
			return;
		
		int varNum = 0;
		//assumes that the data appears in the same order so no checks are done.
		for(ChildNode& c : n->getChildNodes())
		{
			if(c.type != ChildNode::TYPE_NODE)
				continue;
			
			XmlNode* cNodes = c.node;
			if(cNodes->getTitle() == "SerializedObject")
			{
				((SerializedObject*)variables[varNum])->deserialize(cNodes);
			}
			else if(cNodes->getTitle() == "SerializedData")
			{
				auto typeStringPointer = cNodes->getAttribute("TYPE");
				auto valueStringPointer = cNodes->getAttribute("VALUE");

				std::string typeString, valueString;
				
				if(typeStringPointer != nullptr)
					typeString = typeStringPointer->data;
				if(valueStringPointer != nullptr)
					valueString = valueStringPointer->data;
				
				if( typeString == typeid(double).name()
				|| typeString == getAlternateName(typeid(double).name()) )
				{
					*((double*)variables[varNum]->data) = std::stod(valueString);
				}
				else if( typeString == typeid(float).name()
				|| typeString == getAlternateName(typeid(float).name()) )
				{
					*((float*)variables[varNum]->data) = std::stof(valueString);
				}
				else if(typeString == typeid(char).name()
				|| typeString == getAlternateName(typeid(char).name()) )
				{
					*((char*)variables[varNum]->data) = std::stoi(valueString);
				}
				else if(typeString == typeid(unsigned char).name()
				|| typeString == getAlternateName(typeid(unsigned char).name()) )
				{
					*((unsigned char*)variables[varNum]->data) = std::stoi(valueString);
				}
				else if(typeString == typeid(short).name()
				|| typeString == getAlternateName(typeid(short).name()) )
				{
					*((short*)variables[varNum]->data) = std::stoi(valueString);
				}
				else if(typeString == typeid(unsigned short).name()
				|| typeString == getAlternateName(typeid(unsigned short).name()) )
				{
					*((unsigned short*)variables[varNum]->data) = std::stoi(valueString);
				}
				else if(typeString == typeid(int).name()
				|| typeString == getAlternateName(typeid(int).name()) )
				{
					*((int*)variables[varNum]->data) = std::stoi(valueString);
				}
				else if(typeString == typeid(unsigned int).name()
				|| typeString == getAlternateName(typeid(unsigned int).name()) )
				{
					*((unsigned int*)variables[varNum]->data) = std::stoul(valueString); //Go one up for unsigned int
				}
				else if(typeString == typeid(long).name()
				|| typeString == getAlternateName(typeid(long).name()) )
				{
					*((long*)variables[varNum]->data) = std::stol(valueString);
				}
				else if(typeString == typeid(unsigned long).name()
				|| typeString == getAlternateName(typeid(unsigned long).name()) )
				{
					*((unsigned long*)variables[varNum]->data) = std::stoul(valueString); //Go one up for unsigned int
				}
				else if(typeString == typeid(long long).name()
				|| typeString == getAlternateName(typeid(long long).name()) )
				{
					*((long long*)variables[varNum]->data) = std::stoll(valueString);
				}
				else if(typeString == typeid(unsigned long long).name()
				|| typeString == getAlternateName(typeid(unsigned long long).name()) )
				{
					*((unsigned long long*)variables[varNum]->data) = std::stoull(valueString); //Go one up for unsigned int
				}
				else if(typeString == typeid(bool).name()
				|| typeString == getAlternateName(typeid(bool).name()) )
				{
					*((bool*)variables[varNum]->data) = std::stoi(valueString);
				}
				else if(typeString == typeid(std::string).name()
				|| typeString == getAlternateName(typeid(std::string).name()) )
				{
					*((std::string*)variables[varNum]->data) = valueString;
				}
				else
				{
					//pass off to external loader.
					if(externalLoader != nullptr)
						externalLoader(variables[varNum]->data, typeString, valueString);
				}
			}
			varNum++;
		}
	}

	std::vector<SerializedData*>& SerializedObject::getVariables()
	{
		return variables;
	}

	void SerializedObject::addPrettyName(std::string className, std::string prettyName)
	{
		SerializedObject::prettyName[prettyName] = className;
		SerializedObject::prettyName[className] = prettyName;
	}

	std::string SerializedObject::getAlternateName(std::string className)
	{
		auto altNameIt = SerializedObject::prettyName.find(className);
		std::string altType = className;

		if(altNameIt != SerializedObject::prettyName.end())
		{
			altType = altNameIt->second;
		}
		return altType;
	}

	void SerializedObject::setExternalSaver(std::function<std::string(void*, std::string)> extSaver)
	{
		externalSaver = extSaver;
	}

	void SerializedObject::setExternalLoader(std::function<void(void*, std::string, std::string)> extLoader)
	{
		externalLoader = extLoader;
	}

}