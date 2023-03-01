#include "SimpleJSON.h"
#include <iostream>

namespace glib
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

    JNode* JNode::getNode(std::vector<std::string>& s, int offset)
    {
        if(type == SimpleJSON::TYPE_OBJECT)
        {
            if(offset < s.size())
            {
                JObject* obj = (JObject*)this;
                auto it = obj->nameToIndexMap.find(s[offset]);
                if(it != obj->nameToIndexMap.end())
                {
                    if(offset == s.size()-1)
                    {
                        return obj->vars[it->second];
                    }
                    else
                    {
                        return obj->vars[it->second]->getNode(s, offset+1);
                    }
                }
            }
        }
        else if(type == SimpleJSON::TYPE_ARRAY)
        {
            if(offset < s.size())
            {
                JArray* obj = (JArray*)this;
                auto it = obj->nameToIndexMap.find(s[offset]);
                if(it != obj->nameToIndexMap.end())
                {
                    if(offset == s.size()-1)
                    {
                        return obj->vars[it->second];
                    }
                    else
                    {
                        return obj->vars[it->second]->getNode(s, offset+1);
                    }
                }
            }
        }

        return nullptr; //Some type that does not have children
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
            //check if it exist already
            auto it = nameToIndexMap.find(o->getName());
            if(it != nameToIndexMap.end())
            {
                //override existing entry
                JNode* oldNode = vars[it->second];
                if(oldNode != nullptr)
                    delete oldNode;
                
                vars[it->second] = o;
            }
            else
            {
                //add new entry
                vars.push_back(o);
                nameToIndexMap[o->getName()] = vars.size()-1;
            }
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
            //check if it exist already
            auto it = nameToIndexMap.find(o->getName());
            if(it != nameToIndexMap.end())
            {
                //override existing entry
                JNode* oldNode = vars[it->second];
                if(oldNode != nullptr)
                    delete oldNode;
                
                vars[it->second] = o;
            }
            else
            {
                //add new entry
                vars.push_back(o);
                nameToIndexMap[o->getName()] = vars.size()-1;
            }
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
        return rootNode->getString(true);
    }

    void SimpleJSON::load(std::string filename)
    {
        std::fstream file = std::fstream(filename, std::fstream::in | std::fstream::binary);
        if(file.is_open())
        {
            std::string temp;
            std::string temp2;
            bool inP = false;
            bool separatorHit = false;
            while(true)
            {
                char c = file.get();
                if(file.eof())
                    break;
                if(c == '\"')
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
                        rootNode = loadJObject(file, temp);
                        break;
                    }
                }
            }
        }
        file.close();
    }

    void SimpleJSON::load(unsigned char* data, size_t size)
    {
        if(data != nullptr && size > 0)
        {
            std::string temp;
            std::string temp2;
            bool inP = false;
            bool separatorHit = false;
            size_t index = 0;
            while(index < size)
            {
                char c = data[index];
                index++;

                if(c == '\"')
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
                        rootNode = loadJObject(data, size, index, temp);
                        break;
                    }
                }
            }
        }
    }

    JObject* SimpleJSON::getRootNode()
    {
        return rootNode;
    }

    JNode* SimpleJSON::getNode(std::vector<std::string>& s, int offset)
    {
        if(rootNode != nullptr)
            return rootNode->getNode(s, offset);
        
        return nullptr;
    }

    JObject* SimpleJSON::loadJObject(std::fstream& file, std::string name)
    {
        JObject* newObject = new JObject();
        newObject->setName(name);
        if(file.is_open())
        {
            std::string temp;
            std::string temp2;
            bool inP = false;
            bool separatorHit = false;
            while(true)
            {
                char c = file.get();
                if(file.eof())
                    break;
                if(c == '\"')
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
                            newObject->addNode(pair);
                        }

                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == '{')
                    {
                        //begin of object
                        newObject->addNode(loadJObject(file, temp));
                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == '}')
                    {
                        //end of object
                        break;
                    }
                    else if(c == '[')
                    {
                        //begin of array
                        newObject->addNode(loadJArray(file, temp));
                        separatorHit = false;
                        temp = "";
                        temp2 = "";
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
            }

            if(!temp.empty())
            {
                //separator between pairs or items
                JPair* pair = new JPair();
                pair->setName(temp);
                pair->setValue(temp2);
                newObject->addNode(pair);
            }
        }

        return newObject;
    }

    JArray* SimpleJSON::loadJArray(std::fstream& file, std::string name)
    {
        JArray* newArray = new JArray();
        newArray->setName(name);
        if(file.is_open())
        {
            std::string temp;
            std::string temp2;
            bool inP = false;
            bool separatorHit = false;
            while(true)
            {
                char c = file.get();
                if(file.eof())
                    break;
                
                if(c == '\"')
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
                            newArray->addNode(pair);
                        }

                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == '{')
                    {
                        //begin of object
                        newArray->addNode(loadJObject(file, temp));
                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == '[')
                    {
                        //begin of array
                        newArray->addNode(loadJArray(file, temp));
                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == ']')
                    {
                        //end of array
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
            }

            if(!temp.empty())
            {
                //separator between pairs or items
                JPair* pair = new JPair();
                pair->setName(temp);
                pair->setValue(temp2);
                newArray->addNode(pair);
            }
        }

        
        return newArray;
    }

    JObject* SimpleJSON::loadJObject(unsigned char* data, size_t size, size_t& index, std::string name)
    {
        JObject* newObject = new JObject();
        newObject->setName(name);
        if(data != nullptr && size > 0)
        {
            std::string temp;
            std::string temp2;
            bool inP = false;
            bool separatorHit = false;
            while(index < size)
            {
                char c = data[index];
                index++;

                if(c == '\"')
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
                            newObject->addNode(pair);
                        }

                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == '{')
                    {
                        //begin of object
                        newObject->addNode(loadJObject(data, size, index, temp));
                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == '}')
                    {
                        //end of object
                        break;
                    }
                    else if(c == '[')
                    {
                        //begin of array
                        newObject->addNode(loadJArray(data, size, index, temp));
                        separatorHit = false;
                        temp = "";
                        temp2 = "";
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
            }

            if(!temp.empty())
            {
                //separator between pairs or items
                JPair* pair = new JPair();
                pair->setName(temp);
                pair->setValue(temp2);
                newObject->addNode(pair);
            }
        }

        return newObject;
    }

    JArray* SimpleJSON::loadJArray(unsigned char* data, size_t size, size_t& index, std::string name)
    {
        JArray* newArray = new JArray();
        newArray->setName(name);
        if(data != nullptr && size > 0)
        {
            std::string temp;
            std::string temp2;
            bool inP = false;
            bool separatorHit = false;
            while(index < size)
            {
                char c = data[index];
                index++;
                
                if(c == '\"')
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
                            newArray->addNode(pair);
                        }

                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == '{')
                    {
                        //begin of object
                        newArray->addNode(loadJObject(data, size, index, temp));
                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == '[')
                    {
                        //begin of array
                        newArray->addNode(loadJArray(data, size, index, temp));
                        separatorHit = false;
                        temp = "";
                        temp2 = "";
                    }
                    else if(c == ']')
                    {
                        //end of array
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
            }

            if(!temp.empty())
            {
                //separator between pairs or items
                JPair* pair = new JPair();
                pair->setName(temp);
                pair->setValue(temp2);
                newArray->addNode(pair);
            }
        }

        
        return newArray;
    }

    #pragma endregion
    
}