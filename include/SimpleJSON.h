#pragma once
#include <fstream>
#include <vector>
#include <string>

class JNode
{
public:
    JNode(int type);
    ~JNode();
    int getType();
    std::string getName();
    void setName(std::string n);
private:
    std::string name;
    int type = 0;
};

class JPair : public JNode
{
public:
    JPair();
    ~JPair();

    std::string getValue();
    void setValue(std::string n);
    std::string getString(bool last = true);

private:
    std::string value;
};

class JObject : public JNode
{
public:
    JObject();
    ~JObject();

    void addNode(JNode* o);
    std::vector<JNode*>& getNodes();
    std::string getString(bool last = true);
private:
    std::vector<JNode*> vars;
};

class JArray : public JNode
{
public:
    JArray();
    ~JArray();

    void addNode(JNode* o);
    std::vector<JNode*>& getNodes();
    std::string getString(bool last = true);
private:
    std::vector<JNode*> vars;
};

class SimpleJSON
{
public:
    static const int TYPE_NODE = 0;
    static const int TYPE_PAIR = 1;
    static const int TYPE_OBJECT = 2;
    static const int TYPE_ARRAY = 3;
    
    SimpleJSON();
    ~SimpleJSON();

    void load(std::string filename);
    JObject* getRootNode();

    std::string getString();
private:
    JObject* loadJObject(std::fstream& file, std::string name);
    JArray* loadJArray(std::fstream& file, std::string name);

    JObject* rootNode = nullptr;
};