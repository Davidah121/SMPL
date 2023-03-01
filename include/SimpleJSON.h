#pragma once
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

namespace glib
{
    class JNode;
    class JPair;
    class JArray;
    class JObject;

    class JNode
    {
    public:
        JNode(int type);
        ~JNode();
        int getType();
        std::string getName();
        void setName(std::string n);

        JNode* getNode(std::vector<std::string>& s, int offset=0);

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
        friend JNode;
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
        friend JNode;
        std::vector<JNode*> vars;
        std::unordered_map<std::string, size_t> nameToIndexMap;
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
        friend JNode;
        std::vector<JNode*> vars;
        std::unordered_map<std::string, size_t> nameToIndexMap;
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
        void load(unsigned char* data, size_t size);

        JObject* getRootNode();

        std::string getString();

        JNode* getNode(std::vector<std::string>& s, int offset=0);
    private:
        JObject* loadJObject(std::fstream& file, std::string name);
        JArray* loadJArray(std::fstream& file, std::string name);

        
        JObject* loadJObject(unsigned char* data, size_t size, size_t& index, std::string name);
        JArray* loadJArray(unsigned char* data, size_t size, size_t& index, std::string name);

        JObject* rootNode = nullptr;
    };
}