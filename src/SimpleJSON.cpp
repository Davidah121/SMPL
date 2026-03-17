#include "SimpleJSON.h"
#include <iostream>
#include "StringTools.h"

namespace smpl
{

    #pragma region JNODE

    JNode::JNode(int type, std::string name)
    {
        this->name = name;
        this->type = type;
    }
    
    JNode::JNode(const JNode& other)
    {
        this->name = other.name;
        this->type = other.type;
    }

    JNode::~JNode()
    {
        
    }
    int JNode::getType()
    {
        return type;
    }
    std::string JNode::getName()
    {
        return name;
    }
    JNode* JNode::getParent()
    {
        return parent;
    }
    
    void JNode::setParent(JNode* p)
    {
        parent = p;
    }

    std::vector<JNode*> JNode::getNodesPattern(const std::vector<std::string>& s)
    {
        std::vector<JNode*> nodes;
        getNodesPatternInternal(s, 0, nodes);
        return nodes;
    }

    void JNode::getNodesPatternInternal(const std::vector<std::string>& s, int offset, std::vector<JNode*>& results)
    {
        if(offset == s.size())
        {
            results.push_back(this);
        }
        else if(offset < s.size())
        {
            if(type != JPair::TYPE)
            {
                JNodeWithChildren* obj = (JNodeWithChildren*)this;
				auto pairIt = obj->nameToIndexMap.find(s[offset]);

                for(auto p=pairIt; p!=obj->nameToIndexMap.end(); ++p)
                {
                    obj->vars[p->second]->getNodesPatternInternal(s, offset+1, results);
                }
            }
        }
    }


    JNode& JNode::operator[](int location)
    {
        if(type != JPair::TYPE)
        {
			if(location < ((JNodeWithChildren*)this)->vars.size())
            	return *((JNodeWithChildren*)this)->vars[location];
            throw std::out_of_range("Index: " + std::to_string(location) + " is out of range.");
        }
        throw std::runtime_error("JNode is a Pair and does not have children.");
    }
    JNode& JNode::operator[](std::string key)
    {
        if(type != JPair::TYPE)
        {
            JNodeWithChildren* castedThis = ((JNodeWithChildren*)this);
            auto pairIt = castedThis->nameToIndexMap.find(key);
            if(pairIt != castedThis->nameToIndexMap.end())
                return *(castedThis->vars[pairIt->second]);
            throw std::runtime_error("Failed to find node with key: " + key);
        }
        throw std::runtime_error("JNode is a Pair and does not have children.");
    }
    size_t JNode::size()
    {
        if(type != JPair::TYPE)
            return ((JNodeWithChildren*)this)->vars.size();
        return 0;
    }

    #pragma endregion

    
    JNodeWithChildren::JNodeWithChildren(int type, std::string name) : JNode(type, name)
    {

    }
    
    JNodeWithChildren::JNodeWithChildren(const JNodeWithChildren& other) : JNode(other)
    {
        nameToIndexMap = other.nameToIndexMap;
        for(JNode* node : other.vars)
        {
            if(node != nullptr)
            {
                JNode* newNode = nullptr;
                if(node->getType() == JPair::TYPE)
                    newNode = new JPair(*(JPair*)node);
                else if(node->getType() == JArray::TYPE)
                    newNode = new JArray(*(JArray*)node);
                else if(node->getType() == JObject::TYPE)
                    newNode = new JObject(*(JObject*)node);
                if(newNode != nullptr)
                {
                    vars.push_back(newNode);
                    newNode->setParent(this);
                }
            }
        }
    }
    JNodeWithChildren::~JNodeWithChildren()
    {
        for(int i=0; i<vars.size(); i++)
        {
            if(vars[i]!=nullptr)
                delete vars[i];
        }
    }
    
    void JNodeWithChildren::addNode(JNode* o)
    {
        if(o != nullptr)
        {
            std::pair<std::string, size_t> p;
            p.first = o->getName();
            p.second = vars.size();
            nameToIndexMap.insert( p );

            //always add new entry. It is a multimap so all can be referenced/obtained.
            vars.push_back(o);
            o->setParent(this);
        }
    }

    std::vector<JNode*>& JNodeWithChildren::getNodes()
    {
        return vars;
    }

    #pragma region JPAIR

    JPair::JPair(std::string name, std::string value) : JNode(1, name)
    {
        this->value = value;
        replaceEscapeCodes();
    }
    JPair::JPair(const JPair& other) : JNode(other)
    {
        value = other.value;
    }
    JPair::~JPair()
    {

    }

    std::string JPair::getValue()
    {
        return value;
    }
    void JPair::setValue(std::string n)
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
        size_t i=0;

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

    JObject::JObject(std::string name) : JNodeWithChildren(2, name)
    {

    }
    
    JObject::JObject(const JObject& other) : JNodeWithChildren(other)
    {
    }

    JObject::~JObject()
    {
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
        for(int i=0; i<vars.size(); i++)
        {
            JNode* o = vars[i];
            bool lastOne = (i == vars.size()-1);
            result += o->getString(lastOne, prettyPrinting, nextExtraTabStr);
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

    #pragma region JARRAY

    JArray::JArray(std::string name) : JNodeWithChildren(3, name)
    {

    }
    
    JArray::JArray(const JArray& other) : JNodeWithChildren(other)
    {
    }

    JArray::~JArray()
    {
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
        for(int i=0; i<vars.size(); i++)
        {
            JNode* o = vars[i];
            bool lastOne = (i == vars.size()-1);
            result += o->getString(lastOne, prettyPrinting, nextExtraTabStr);
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

    SimpleJSON::SimpleJSON()
    {
        rootNode = new JObject();
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
        if(other.rootNode != nullptr)
        {
            if(other.rootNode->getType() == JObject::TYPE)
                rootNode = new JObject(*(JObject*)other.rootNode);
            else if(other.rootNode->getType() == JArray::TYPE)
                rootNode = new JArray(*(JArray*)other.rootNode);
            else
                throw JSONInvalidRootElement();
        }
        else
            throw JSONInvalidRootElement();
    }

    void SimpleJSON::operator=(const SimpleJSON& other)
    {
        if(other.rootNode != nullptr)
        {
            if(other.rootNode->getType() == JObject::TYPE)
                rootNode = new JObject(*(JObject*)other.rootNode);
            else if(other.rootNode->getType() == JArray::TYPE)
                rootNode = new JArray(*(JArray*)other.rootNode);
            else
                throw JSONInvalidRootElement();
        }
        else
            throw JSONInvalidRootElement();
    }
    
    SimpleJSON::SimpleJSON(SimpleJSON&& other)
    {
        rootNode = other.rootNode;
        other.rootNode = nullptr;
    }

    void SimpleJSON::operator=(SimpleJSON&& other)
    {
        rootNode = other.rootNode;
        other.rootNode = nullptr;
    }

    SimpleJSON::~SimpleJSON()
    {
        if(rootNode != nullptr)
            delete rootNode;
        rootNode = nullptr;
    }

    std::string SimpleJSON::getString(bool prettyPrinting)
    {
        return rootNode->getString(true, prettyPrinting, "");
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

    bool SimpleJSON::load(std::string filename)
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

    JNode* SimpleJSON::getRootNode()
    {
        return rootNode;
    }
    
    JNode& SimpleJSON::operator[](int location)
    {
        if(rootNode != nullptr)
            return rootNode->operator[](location);
        throw std::runtime_error("JSON Root Node is a nullptr.");
    }
    JNode& SimpleJSON::operator[](std::string key)
    {
        if(rootNode != nullptr)
            return rootNode->operator[](key);
        throw std::runtime_error("JSON Root Node is a nullptr.");
    }
    size_t SimpleJSON::size()
    {
        return rootNode->size();
    }

    std::vector<JNode*> SimpleJSON::getNodesPattern(const std::vector<std::string>& s)
    {
        if(rootNode != nullptr)
            return rootNode->getNodesPattern(s);
        
        return {};
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
                    rootNode = loadJObject(temp);
                    break;
                }
                else if(c == '[')
                {
                    //begin of array
                    //Note that only one root node is allowed. I think???
                    rootNode = loadJArray(temp);
                    break;
                }
            }
            lastChar = c;
        }

        delete readBuffer;
        readBuffer = nullptr;
    }

    JObject* SimpleJSON::loadJObject(std::string name)
    {
        JObject* newObject = new JObject(name);
        loadJObjectOrJArray(newObject);
        return newObject;
    }

    JArray* SimpleJSON::loadJArray(std::string name)
    {
        JArray* newArray = new JArray(name);
        loadJObjectOrJArray(newArray);
        return newArray;
    }

    
    void SimpleJSON::loadJObjectOrJArray(JNode* node)
    {
        if(node == nullptr)
            return;
        if(node->getType() != JArray::TYPE && node->getType() != JObject::TYPE)
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
                        JPair* pair = new JPair(temp, temp2);
                        if(node->getType() == JArray::TYPE)
                            ((JArray*)node)->addNode(pair);
                        else
                            ((JObject*)node)->addNode(pair);
                    }

                    separatorHit = false;
                    temp = "";
                    temp2 = "";
                }
                else if(c == '{')
                {
                    //begin of object
                    if(node->getType() == JArray::TYPE)
                        ((JArray*)node)->addNode(loadJObject(temp));
                    else
                        ((JObject*)node)->addNode(loadJObject(temp));
                    separatorHit = false;
                    temp = "";
                    temp2 = "";
                }
                else if(c == '[')
                {
                    //begin of array
                    if(node->getType() == JArray::TYPE)
                        ((JArray*)node)->addNode(loadJArray(temp));
                    else
                        ((JObject*)node)->addNode(loadJArray(temp));
                    separatorHit = false;
                    temp = "";
                    temp2 = "";
                }
                else if(c == ']' && node->getType() == JArray::TYPE)
                {
                    //end of array
                    validEnd = true;
                    break;
                }
                else if(c == '}' && node->getType() == JObject::TYPE)
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
            JPair* pair = new JPair(temp, temp2);
            if(node->getType() == JArray::TYPE)
                ((JArray*)node)->addNode(pair);
            else
                ((JObject*)node)->addNode(pair);
        }
    }

    #pragma endregion
    
}