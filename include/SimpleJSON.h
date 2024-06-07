#pragma once
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

namespace smpl
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

        std::vector<JNode*> getNodesPattern(std::vector<std::string> s, int offset=0);

    private:
        void getNodesPatternInternal(std::vector<std::string> s, int offset, std::vector<JNode*>& results);

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
        std::unordered_multimap<std::string, size_t> nameToIndexMap;
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
        std::unordered_multimap<std::string, size_t> nameToIndexMap;
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

        bool load(std::string filename);
        bool load(unsigned char* data, size_t size);

        JNode* getRootNode();

        std::string getString();

        std::vector<JNode*> getNodesPattern(std::vector<std::string> s, int offset=0);
    private:
        JObject* loadJObject(std::string name);
        JArray* loadJArray(std::string name);
        void loadJObjectOrJArray(JNode* node);
        void initLoad();

        bool getNextCharacter(char& c);
        void setDataSource(unsigned char* data, size_t size);
        void setDataSource(std::fstream* file);
        void clearDataSource();

        std::fstream* fileDataSource = nullptr;
        unsigned char* otherDataSource = nullptr;
        size_t otherDataSourceSize = 0;
        size_t otherDataSourceIndex = 0;
        bool continueProcessing = true;

        JNode* rootNode = nullptr; //can be an object or array for some stupid reason
    };
}