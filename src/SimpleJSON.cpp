#include "SimpleJSON.h"
#include <iostream>
#include "StringTools.h"

namespace smpl
{

	size_t JNodeHash::operator()(const JNode& node) const
	{
		return rapidhash_wrapper(node.getName());
	}
	size_t JNodeHash::operator()(const std::string& name) const
	{
		return rapidhash_wrapper(name);
	}

	bool JNodeEquals::operator()(const JNode& node, const JNode& node2) const
	{
		return node == node2;
	}
	bool JNodeEquals::operator()(const JNode& node, const std::string& name) const
	{
		return node.getName() == name;
	}
	bool JNodeEquals::operator()(const std::string& name, const JNode& node) const
	{
		return node.getName() == name;
	}

	#pragma region JNODE

	JNode::JNode(uint8_t type, const std::string& name)
	{
		this->name = name;
		this->type = type;
	}
	
	JNode::JNode(const JNode& other)
	{
		this->name = other.name;
		this->type = other.type;
		if(this->type == JPair::TYPE)
			new (&value) std::string(other.value);
		else if(this->type == JObject::TYPE)
			new (&set) SimpleHashSet<JNode, JNodeHash, JNodeEquals>(other.set);
		else if(this->type == JArray::TYPE)
			new (&arr) std::vector<JNode>(other.arr);
	}
	JNode& JNode::operator=(const JNode& other)
	{
		this->name = other.name;
		this->type = other.type;
		if(this->type == JPair::TYPE)
			new (&value) std::string(other.value);
		else if(this->type == JObject::TYPE)
			new (&set) SimpleHashSet<JNode, JNodeHash, JNodeEquals>(other.set);
		else if(this->type == JArray::TYPE)
			new (&arr) std::vector<JNode>(other.arr);
		return *this;
	}
	JNode::JNode(JNode&& other) noexcept
	{
		this->name = other.name;
		this->type = other.type;
		if(this->type == JPair::TYPE)
			new (&value) std::string(std::move(other.value));
		else if(this->type == JObject::TYPE)
			new (&set) SimpleHashSet<JNode, JNodeHash, JNodeEquals>(std::move(other.set));
		else if(this->type == JArray::TYPE)
			new (&arr) std::vector<JNode>(std::move(other.arr));
	}
	JNode& JNode::operator=(JNode&& other) noexcept
	{
		this->name = other.name;
		this->type = other.type;
		if(this->type == JPair::TYPE)
			new (&value) std::string(std::move(other.value));
		else if(this->type == JObject::TYPE)
			new (&set) SimpleHashSet<JNode, JNodeHash, JNodeEquals>(std::move(other.set));
		else if(this->type == JArray::TYPE)
			new (&arr) std::vector<JNode>(std::move(other.arr));
		return *this;
	}
	
	JNode::~JNode()
	{
		if(type == JObject::TYPE)
		{
			set.~SimpleHashSet<JNode, JNodeHash, JNodeEquals>();
		}
		else if(type == JArray::TYPE)
		{
			typedef std::vector<JNode> arrType;
			arr.~arrType();
		}
		else
		{
			typedef std::string strType;
			value.~strType();
		}
	}
	uint8_t JNode::getType() const
	{
		return type;
	}
	std::string JNode::getName() const
	{
		return name;
	}
	
	JNode& JNode::operator[](const std::string& key)
	{
		if(type != JObject::TYPE)
			throw std::runtime_error("Failed to find \"" + key + "\" as the current JNode is not an Object");
		return ((JObject*)this)->operator[](key);
	}
	JNode& JNode::operator[](size_t location)
	{
		if(type != JArray::TYPE)
			throw std::runtime_error("Failed to find JNode at index \"" + StringTools::toString(location) + "\" as the current JNode is not an Array");
		return ((JArray*)this)->operator[](location);
	}
	
	std::string JNode::getString(bool last, bool prettyPrint, std::string extraTabsString)
	{
		throw std::runtime_error("Failed to get string. This is a JNode and not a specific JSON type like JPair/JObject/JArray. This is the default function and not the overloaded versions");
	}
	
	JPair& JNode::getPair()
	{
		return (JPair&)*this;
	}
	JObject& JNode::getObject()
	{
		return (JObject&)*this;
	}
	JArray& JNode::getArray()
	{
		return (JArray&)*this;
	}
	
	// std::vector<JNode*> JNode::getNodesPattern(const std::vector<std::string>& s)
	// {
	// 	std::vector<JNode*> nodes;
	// 	getNodesPatternInternal(s, 0, nodes);
	// 	return nodes;
	// }

	// void JNode::getNodesPatternInternal(const std::vector<std::string>& s, int offset, std::vector<JNode*>& results)
	// {
	//     if(offset == s.size())
	//     {
	//         results.push_back(this);
	//     }
	//     else if(offset < s.size())
	//     {
	//         if(type != JPair::TYPE)
	//         {
	//             JNodeWithChildren* obj = (JNodeWithChildren*)this;
	// 			auto pairIt = obj->nameToIndexMap.find(s[offset]);

	//             for(auto p=pairIt; p!=obj->nameToIndexMap.end(); ++p)
	//             {
	//                 obj->vars[p->second]->getNodesPatternInternal(s, offset+1, results);
	//             }
	//         }
	//     }
	// }


	bool JNode::operator==(const std::string& name) const
	{
		return this->name == name;
	}
	bool JNode::operator!=(const std::string& name) const
	{
		return this->name != name;
	}
	
	bool JNode::operator==(const JNode& node) const
	{
		return type == node.type && name == node.name;
	}
	bool JNode::operator!=(const JNode& node) const
	{
		return type != node.type || name != node.name;
	}

	// JNode& JNode::operator[](int location)
	// {
	//     if(type != JPair::TYPE)
	//     {
	// 		if(location < ((JNodeWithChildren*)this)->vars.size())
	//         	return *((JNodeWithChildren*)this)->vars[location];
	//         throw std::out_of_range("Index: " + std::to_string(location) + " is out of range.");
	//     }
	//     throw std::runtime_error("JNode is a Pair and does not have children.");
	// }
	// JNode& JNode::operator[](std::string key)
	// {
	//     if(type != JPair::TYPE)
	//     {
	//         JNodeWithChildren* castedThis = ((JNodeWithChildren*)this);
	//         auto pairIt = castedThis->nameToIndexMap.find(key);
	//         if(pairIt != castedThis->nameToIndexMap.end())
	//             return *(castedThis->vars[pairIt->second]);
	//         throw std::runtime_error("Failed to find node with key: " + key);
	//     }
	//     throw std::runtime_error("JNode is a Pair and does not have children.");
	// }
	// size_t JNode::size()
	// {
	//     if(type != JPair::TYPE)
	//         return ((JNodeWithChildren*)this)->vars.size();
	//     return 0;
	// }

	#pragma endregion
	
	#pragma region JPAIR

	JPair::JPair(const std::string& name, const std::string& value) : JNode(JPair::TYPE, name)
	{
		//Using a unnamed union means doing this and manually calling destructor.
		//destructor is handled in JNode itself.
		new (&this->value) std::string(value); //new placement.
		replaceEscapeCodes();
	}

	std::string JPair::getValue()
	{
		return value;
	}
	void JPair::setValue(const std::string& n)
	{
		value = n;
		replaceEscapeCodes();
	}

	
	std::string JPair::escapeInvalidChars()
	{
		//all values under 32 (space) are invalid in the value part when converting to a valid JSON string. escape those values (note that escaping linebreaks and tabs should be \n and \t respectively)
		std::string outputString = "";
		for(char& c : value)
		{
			if(c < 32)
			{
				//escape these
				if(c == '\n')
					outputString += "\\n";
				else if(c == '\t')
					outputString += "\\t";
				else if(c == '\r')
					outputString += "\\r";
				else
					outputString += "\\u" + StringTools::toHexString((short)c);
			}
			else
				outputString += c;
		}
		return outputString;
	}

	void JPair::replaceEscapeCodes()
	{
		//For the purposes of C++ processing, it is unnecessary to keep the escaped characters so replace them with valid utf-8 in the string
		std::string outputString = "";
		size_t i=value.find_first_of('\\');
		if(i == SIZE_MAX)
			return;

		outputString = value.substr(0, i);
		while(i<value.size())
		{
			if(value[i] == '\\')
			{
				if(i+1 < value.size())
				{
					if(value[i+1] == 'n')
					{
						outputString += '\n';
						i++;
					}
					else if(value[i+1] == 't')
					{
						outputString += '\t';
						i++;
					}
					else if(value[i+1] == 'r')
					{
						outputString += '\r';
						i++;
					}
					else if(value[i+1] == 'u')
					{
						if(i+5 < value.size())
						{
							size_t unicodeValue = StringTools::fromHexString(value.substr(i+2, 4));
							std::vector<unsigned char> utfValues = StringTools::toUTF8(unicodeValue);
							for(char c : utfValues)
								outputString += c;
							i += 5;
						}
					}
				}
			}
			else
				outputString += value[i];
			i++;
		}

		value = outputString;
	}
	
	std::string JPair::getString(bool last, bool prettyPrinting, std::string extraTabsString)
	{
		std::string result;
		if(prettyPrinting)
			result = extraTabsString;

		if(getName() != "")
			result += "\"" + getName() + "\": ";
		result += "\"" + escapeInvalidChars() + "\"";

		if(!last)
			result += ", ";
		if(prettyPrinting)
			result += '\n';
		return result;
	}

	#pragma endregion

	#pragma region JOBJECT

	JObject::JObject(const std::string& name) : JNode(JObject::TYPE, name)
	{
		//Using a unnamed union means doing this and manually calling destructor.
		//destructor is handled in JNode itself.
		new (&this->set) SimpleHashSet<JNode, JNodeHash, JNodeEquals>(); //new placement.
	}

	JNode& JObject::operator[](const std::string& key)
	{
		auto it = set.find(key);
		if(it != set.end())
			return *it;
		throw std::runtime_error("Failed to find node with key: " + key);
	}
	
	void JObject::insert(const JNode& node)
	{
		set.insert(node);
	}
	void JObject::insert(JNode&& node)
	{
		set.insert(node);
	}
	void JObject::remove(const std::string& name)
	{
		set.erase(name);
	}
		
	size_t JObject::size()
	{
		return set.size();
	}
	bool JObject::empty()
	{
		return set.empty();
	}
	void JObject::clear()
	{
		set.clear();
	}

	void JObject::shrink()
	{
		set.tightlyFit();
	}

	std::string JObject::getString(bool last, bool prettyPrinting, std::string extraTabsString)
	{
		std::string result;
		std::string nextExtraTabStr = "";
		if(prettyPrinting)
			nextExtraTabStr = extraTabsString + '\t';

		if(prettyPrinting)
			result = extraTabsString;
		
		if(getName() != "")
			result += "\"" + getName() + "\": ";
		result += '{';
		
		if(prettyPrinting)
			result += '\n';
		size_t index = 0;
		for(auto& nodeUnion : set)
		{
			JNode& node = nodeUnion;
			bool lastOne = (index == set.size()-1);
			result += node.getString(lastOne, prettyPrinting, nextExtraTabStr);
			index++;
		}
		if(prettyPrinting)
			result += extraTabsString;
		result += '}';
			
		if(!last)
			result += ',';
		if(prettyPrinting)
			result += '\n';
		return result;
	}

	#pragma endregion

	#pragma region JArray
	JArray::JArray(const std::string& name) : JNode(JArray::TYPE, name)
	{
		//Using a unnamed union means doing this and manually calling destructor.
		//destructor is handled in JNode itself.
		new (&this->arr) std::vector<JNode>(); //new placement.
	}
	
	JNode& JArray::operator[](size_t location)
	{
		return arr[location];
	}
	void JArray::push_back(const JNode& node)
	{
		arr.push_back(node);
	}
	
	JNode& JArray::front()
	{
		return arr.front();
	}
	JNode& JArray::back()
	{
		return arr.back();
	}
	void JArray::push_back(JNode&& node)
	{
		arr.push_back(std::move(node));
	}
	void JArray::pop_back()
	{
		arr.pop_back();
	}
	size_t JArray::size()
	{
		return arr.size();
	}
	bool JArray::empty()
	{
		return arr.empty();
	}
	void JArray::clear()
	{
		arr.clear();
	}

	void JArray::shrink()
	{
		arr.shrink_to_fit();
	}

	std::string JArray::getString(bool last, bool prettyPrinting, std::string extraTabsString)
	{
		std::string result;
		std::string nextExtraTabStr = "";
		if(prettyPrinting)
			nextExtraTabStr = extraTabsString + '\t';

		if(prettyPrinting)
			result = extraTabsString;
		
		if(getName() != "")
			result += "\"" + getName() + "\": ";
		result += '[';
		
		if(prettyPrinting)
			result += '\n';
		for(size_t i=0; i<set.getInternalData().size(); i++)
		{
			JNode& o = set.getInternalData()[i];
			bool lastOne = (i == set.getInternalData().size()-1);
			result += o.getString(lastOne, prettyPrinting, nextExtraTabStr); //because this is a reference, it SHOULD pick the correct one
		}
		if(prettyPrinting)
			result += extraTabsString;
		result += ']';
			
		if(!last)
			result += ',';
		if(prettyPrinting)
			result += '\n';
		return result;
	}

	#pragma endregion

	#pragma region SIMPLEJSON

	SimpleJSON::SimpleJSON(bool arrayRoot)
	{
		if(arrayRoot == true)
			rootNode = JArray();
	}
	
	SimpleJSON::SimpleJSON(Streamable<unsigned char>* data)
	{
		if(!load(data))
		{
			throw std::runtime_error("Failed to load JSON");
		}
	}
	SimpleJSON::SimpleJSON(const std::string& filename)
	{
		if(!load(filename))
		{
			throw std::runtime_error("Failed to load JSON");
		}
	}
	SimpleJSON::SimpleJSON(unsigned char* data, size_t size)
	{
		if(!load(data, size))
		{
			throw std::runtime_error("Failed to load JSON");
		}
	}
	
	SimpleJSON::SimpleJSON(const SimpleJSON& other)
	{
		if(other.rootNode.getType() != JObject::TYPE || other.rootNode.getType() != JArray::TYPE)
			throw JSONInvalidRootElement();
		else
			rootNode = other.rootNode;
	}

	SimpleJSON& SimpleJSON::operator=(const SimpleJSON& other)
	{
		if(other.rootNode.getType() != JObject::TYPE || other.rootNode.getType() != JArray::TYPE)
			throw JSONInvalidRootElement();
		else
			rootNode = other.rootNode;
		return *this;
	}
	
	SimpleJSON::SimpleJSON(SimpleJSON&& other) noexcept
	{
		rootNode = std::move(other.rootNode);
	}

	SimpleJSON& SimpleJSON::operator=(SimpleJSON&& other) noexcept
	{
		rootNode = std::move(other.rootNode);
		return *this;
	}

	SimpleJSON::~SimpleJSON()
	{
	}

	bool SimpleJSON::load(Streamable<unsigned char>* data)
	{
		datasource = data;
		bool isValid = datasource->isValid();
		if(isValid)
		{
			initLoad();
		}
		datasource = nullptr;
		return isValid;
	}

	bool SimpleJSON::load(const std::string& filename)
	{
		datasource = new StreamableFile(filename, SimpleFile::READ);
		bool isValid = datasource->isValid();
		if(isValid)
		{
			initLoad();
		}
		delete datasource;
		return isValid;
	}

	bool SimpleJSON::load(unsigned char* data, size_t size)
	{
		datasource = new StreamableArray<unsigned char>(data, size, false);
		bool isValid = datasource->isValid();

		if(isValid)
		{
			initLoad();
		}
		delete datasource;
		return isValid;
	}

	JNode& SimpleJSON::getRootNode()
	{
		return (JNode&)rootNode;
	}

	bool SimpleJSON::getNextCharacter(char& c)
	{
		if(readIndex >= readSize)
		{
			readSize = datasource->read(readBuffer, 4096);
			readIndex = 0;
			if(readSize == 0)
			{
				continueProcessing = false;
				return false;
			}
		}

		if(readIndex < readSize)
		{
			c = readBuffer[readIndex];
			readIndex++;
			return true;
		}

		continueProcessing = false;
		return false;
	}

	void SimpleJSON::initLoad()
	{
		std::string temp;
		std::string temp2;
		bool inP = false;
		bool separatorHit = false;
		char lastChar = 0;

		readBuffer = new unsigned char[4096];
		readSize = 0;
		readIndex = 0;

		while(continueProcessing)
		{
			char c;
			if(!getNextCharacter(c))
				break;
			if(c == '\"' && lastChar != '\\')
			{
				inP = !inP;
				continue;
			}

			if(!inP)
			{
				if(c == ':')
				{
					//separator between name and whatever comes after
					separatorHit = true;
				}
				else if(c == ',')
				{
					//separator between pairs
					separatorHit = false;
					temp = "";
					temp2 = "";
				}
				else if(c == '{')
				{
					//begin of object
					//Note that only one root node is allowed. I think???
					loadJObject(nullptr, temp);
					break;
				}
				else if(c == '[')
				{
					//begin of array
					//Note that only one root node is allowed. I think???
					loadJArray(nullptr, temp);
					break;
				}
			}
			lastChar = c;
		}

		delete readBuffer;
		readBuffer = nullptr;
	}

	void SimpleJSON::loadJObject(JNode* parent, const std::string& name)
	{
		JObject newObject = JObject(name);
		loadJObjectOrJArray(newObject);
		newObject.shrink();
		if(parent == nullptr)
		{
			rootNode = std::move(newObject);
		}
		else
		{
			if(parent->getType() == JObject::TYPE)
				((JObject*)parent)->insert(std::move(newObject));
			else if(parent->getType() == JArray::TYPE)
				((JArray*)parent)->push_back(std::move(newObject));
			else
			 	throw std::runtime_error("Error parsing JSON. Can't add an object or array to a pair");
		}
	}

	void SimpleJSON::loadJArray(JNode* parent, const std::string& name)
	{
		JArray newArray = JArray(name);
		loadJObjectOrJArray(newArray);
		newArray.shrink();
		if(parent == nullptr)
		{
			rootNode = std::move(newArray);
		}
		else
		{
			if(parent->getType() == JObject::TYPE)
				((JObject*)parent)->insert(std::move(newArray));
			else if(parent->getType() == JArray::TYPE)
				((JArray*)parent)->push_back(std::move(newArray));
			else
			 	throw std::runtime_error("Error parsing JSON. Can't add an object or array to a pair");
		}
	}

	
	void SimpleJSON::loadJObjectOrJArray(JNode& node)
	{
		if(node.getType() != JArray::TYPE && node.getType() != JObject::TYPE)
			return;
		
		std::string temp;
		std::string temp2;
		bool inP = false;
		bool separatorHit = false;
		char lastChar = 0;
		bool validEnd = false;
		while(continueProcessing)
		{
			char c;
			if(!getNextCharacter(c))
				break;
			
			if(c == '\"' && lastChar != '\\')
			{
				inP = !inP;
				continue;
			}

			if(!inP)
			{
				if(c == ':')
				{
					//separator between name and whatever comes after
					separatorHit = true;
				}
				else if(c == ',')
				{
					if(!temp.empty())
					{
						//separator between pairs or items
						JPair pair = JPair(temp, temp2);
						if(node.getType() == JArray::TYPE)
							((JArray&)node).push_back(std::move(pair));
						else
							((JObject&)node).insert(std::move(pair));
					}

					separatorHit = false;
					temp = "";
					temp2 = "";
				}
				else if(c == '{')
				{
					//begin of object
					loadJObject(&node, temp);
					separatorHit = false;
					temp = "";
					temp2 = "";
				}
				else if(c == '[')
				{
					//begin of array
					loadJArray(&node, temp);
					separatorHit = false;
					temp = "";
					temp2 = "";
				}
				else if(c == ']' && node.getType() == JArray::TYPE)
				{
					//end of array
					validEnd = true;
					break;
				}
				else if(c == '}' && node.getType() == JObject::TYPE)
				{
					//end of object
					validEnd = true;
					break;
				}
				else
				{
					if(c>' ')
					{
						if(!separatorHit)
							temp += c;
						else
							temp2 += c;
					}
				}
			}
			else
			{
				if(!separatorHit)
					temp += c;
				else
					temp2 += c;
			}
			lastChar = c;
		}

		if(validEnd != true)
		{
			throw JSONParsingError();
		}

		if(!temp.empty())
		{
			//separator between pairs or items
			JPair pair = JPair(temp, temp2);
			if(node.getType() == JArray::TYPE)
				((JArray&)node).push_back(std::move(pair));
			else
				((JObject&)node).insert(std::move(pair));
		}
	}

	#pragma endregion
	
}