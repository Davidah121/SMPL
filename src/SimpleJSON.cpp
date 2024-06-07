#include "SimpleJSON.h"
#include <iostream>
#include "StringTools.h"

namespace smpl
{

    #pragma region JNODE

    JNode::JNode(int type)
    {
        this->type = type;
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
    void JNode::setName(std::string n)
    {
        name = n;
    }

    std::vector<JNode*> JNode::getNodesPattern(std::vector<std::string> s, int offset)
    {
        std::vector<JNode*> nodes;
        getNodesPatternInternal(s, offset, nodes);
        return nodes;
    }

    void JNode::getNodesPatternInternal(std::vector<std::string> s, int offset, std::vector<JNode*>& results)
    {
        if(offset == s.size())
        {
            results.push_back(this);
        }
        else if(offset < s.size())
        {
            if(type == SimpleJSON::TYPE_OBJECT)
            {
                JObject* obj = (JObject*)this;
                auto pairIt = obj->nameToIndexMap.equal_range(s[offset]);

                for(auto p=pairIt.first; p!=pairIt.second; p++)
                {
                    obj->vars[p->second]->getNodesPatternInternal(s, offset+1, results);
                }
            }
            else if(type == SimpleJSON::TYPE_ARRAY)
            {
                JArray* obj = (JArray*)this;
                auto pairIt = obj->nameToIndexMap.equal_range(s[offset]);

                for(auto p=pairIt.first; p!=pairIt.second; p++)
                {
                    obj->vars[p->second]->getNodesPatternInternal(s, offset+1, results);
                }
            }
        }
    }

    #pragma endregion

    #pragma region JPAIR

    JPair::JPair() : JNode(1)
    {

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
    }
    std::string JPair::getString(bool last)
    {
        std::string result;
        if(getName() != "")
        {
            result += getName() + ": ";
        }
        result += value;
        if(last)
            result += "\n";
        else
            result += ", \n";
        return result;
    }

    #pragma endregion

    #pragma region JOBJECT

    JObject::JObject() : JNode(2)
    {

    }
    JObject::~JObject()
    {
        for(int i=0; i<vars.size(); i++)
        {
            if(vars[i]!=nullptr)
                delete vars[i];
        }
    }

    void JObject::addNode(JNode* o)
    {
        if(o != nullptr)
        {
            std::pair<std::string, size_t> p;
            p.first = o->getName();
            p.second = vars.size();
            nameToIndexMap.insert( p );

            //always add new entry. It is a multimap so all can be referenced/obtained.
            vars.push_back(o);
        }
    }

    std::vector<JNode*>& JObject::getNodes()
    {
        return vars;
    }
    std::string JObject::getString(bool last)
    {
        std::string result;
        if(getName() != "")
        {
            result += getName() + ": ";
        }
        result += "{\n";
        for(int i=0; i<vars.size(); i++)
        {
            JNode* o = vars[i];
            bool lastOne = (i == vars.size()-1);

            if(o->getType() == SimpleJSON::TYPE_PAIR)
            {
                JPair* pair = (JPair*)o;
                result += pair->getString(lastOne);
            }
            else if(o->getType() == SimpleJSON::TYPE_OBJECT)
            {
                JObject* object = (JObject*)o;
                result += object->getString(lastOne);
            }
            else if(o->getType() == SimpleJSON::TYPE_ARRAY)
            {
                JArray* arr = (JArray*)o;
                result += arr->getString(lastOne);
            }
        }
        if(last)
            result += "}\n";
        else
            result += "},\n";
        return result;
    }

    #pragma endregion

    #pragma region JARRAY

    JArray::JArray() : JNode(3)
    {

    }
    JArray::~JArray()
    {
        for(int i=0; i<vars.size(); i++)
        {
            if(vars[i]!=nullptr)
                delete vars[i];
        }
    }

    void JArray::addNode(JNode* o)
    {
        if(o != nullptr)
        {
            std::pair<std::string, size_t> p;
            p.first = o->getName();
            p.second = vars.size();
            nameToIndexMap.insert( p );

            //always add new entry. It is a multimap so all can be referenced/obtained.
            vars.push_back(o);
        }
    }

    std::vector<JNode*>& JArray::getNodes()
    {
        return vars;
    }

    std::string JArray::getString(bool last)
    {
        std::string result;
        if(getName() != "")
        {
            result += getName() + ": ";
        }
        result += "[\n";
        for(int i=0; i<vars.size(); i++)
        {
            JNode* o = vars[i];
            bool lastOne = (i == vars.size()-1);

            if(o->getType() == SimpleJSON::TYPE_PAIR)
            {
                JPair* pair = (JPair*)o;
                result += pair->getString(lastOne);
            }
            else if(o->getType() == SimpleJSON::TYPE_OBJECT)
            {
                JObject* object = (JObject*)o;
                result += object->getString(lastOne);
            }
            else if(o->getType() == SimpleJSON::TYPE_ARRAY)
            {
                JArray* arr = (JArray*)o;
                result += arr->getString(lastOne);
            }
        }

        if(last)
            result += "]\n";
        else
            result += "],\n";
        return result;
    }

    #pragma endregion

    #pragma region SIMPLEJSON

    SimpleJSON::SimpleJSON()
    {

    }
    SimpleJSON::~SimpleJSON()
    {
        if(rootNode != nullptr)
            delete rootNode;
        rootNode = nullptr;
    }

    std::string SimpleJSON::getString()
    {
        if(rootNode->getType() == SimpleJSON::TYPE_ARRAY)
            return ((JArray*)rootNode)->getString(true);
        else if(rootNode->getType() == SimpleJSON::TYPE_OBJECT)
            return ((JObject*)rootNode)->getString(true);
        else
            return "";
    }

    bool SimpleJSON::load(std::string filename)
    {
        std::fstream file = std::fstream(filename, std::fstream::in | std::fstream::binary);
        if(file.is_open())
        {
            setDataSource(&file);
            initLoad();
            file.close();
        }
        else
            return false;
        return true;
    }

    bool SimpleJSON::load(unsigned char* data, size_t size)
    {
        if(data != nullptr && size > 0)
        {
            setDataSource(data, size);
            initLoad();
        }
        else
            return false;
        return true;
    }

    JNode* SimpleJSON::getRootNode()
    {
        return rootNode;
    }

    std::vector<JNode*> SimpleJSON::getNodesPattern(std::vector<std::string> s, int offset)
    {
        if(rootNode != nullptr)
            return rootNode->getNodesPattern(s, offset);
        
        return {};
    }

    bool SimpleJSON::getNextCharacter(char& c)
    {
        if(otherDataSource != nullptr)
        {
            if(otherDataSourceIndex < otherDataSourceSize)
            {
                c = otherDataSource[otherDataSourceIndex];
                otherDataSourceIndex++;
            }
            else
            {
                continueProcessing = false;
                return false;
            }
        }
        else if(fileDataSource != nullptr)
        {
            c = fileDataSource->get();
            if(fileDataSource->eof())
            {
                continueProcessing = false;
                return false;
            }
        }
        else
        {
            continueProcessing = false;
            return false;
        }
        
        return true;
    }

    void SimpleJSON::setDataSource(unsigned char* data, size_t size)
    {
        clearDataSource();
        otherDataSource = data;
        otherDataSourceSize = size;
        continueProcessing = true;
    }

    void SimpleJSON::setDataSource(std::fstream* file)
    {
        clearDataSource();
        fileDataSource = file;
        continueProcessing = true;
    }

    void SimpleJSON::clearDataSource()
    {
        otherDataSource = nullptr;
        fileDataSource = nullptr;
        otherDataSourceSize = 0;
        otherDataSourceIndex = 0;
        continueProcessing = false;
    }

    void SimpleJSON::initLoad()
    {
        std::string temp;
        std::string temp2;
        bool inP = false;
        bool separatorHit = false;
        char lastChar = 0;
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
    }

    JObject* SimpleJSON::loadJObject(std::string name)
    {
        JObject* newObject = new JObject();
        newObject->setName(name);
        loadJObjectOrJArray(newObject);
        return newObject;
    }

    JArray* SimpleJSON::loadJArray(std::string name)
    {
        JArray* newArray = new JArray();
        newArray->setName(name);
        loadJObjectOrJArray(newArray);
        return newArray;
    }

    
    void SimpleJSON::loadJObjectOrJArray(JNode* node)
    {
        if(node == nullptr)
            return;
        if(node->getType() != SimpleJSON::TYPE_ARRAY && node->getType() != SimpleJSON::TYPE_OBJECT)
            return;
        
        std::string temp;
        std::string temp2;
        bool inP = false;
        bool separatorHit = false;
        char lastChar = 0;
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
                        JPair* pair = new JPair();
                        pair->setName(temp);
                        pair->setValue(temp2);
                        if(node->getType() == SimpleJSON::TYPE_ARRAY)
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
                    if(node->getType() == SimpleJSON::TYPE_ARRAY)
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
                    if(node->getType() == SimpleJSON::TYPE_ARRAY)
                        ((JArray*)node)->addNode(loadJArray(temp));
                    else
                        ((JObject*)node)->addNode(loadJArray(temp));
                    separatorHit = false;
                    temp = "";
                    temp2 = "";
                }
                else if(c == ']' && node->getType() == SimpleJSON::TYPE_ARRAY)
                {
                    //end of array
                    break;
                }
                else if(c == '}' && node->getType() == SimpleJSON::TYPE_OBJECT)
                {
                    //end of object
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

        if(!temp.empty())
        {
            //separator between pairs or items
            JPair* pair = new JPair();
            pair->setName(temp);
            pair->setValue(temp2);
            if(node->getType() == SimpleJSON::TYPE_ARRAY)
                ((JArray*)node)->addNode(pair);
            else
                ((JObject*)node)->addNode(pair);
        }
    }

    #pragma endregion
    
}