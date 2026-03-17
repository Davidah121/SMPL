#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "Streamable.h"
#include <fstream>
#include <vector>
#include "SimpleHashTable.h"
#include <string>

namespace smpl
{
    class DLL_OPTION JNode;
    class DLL_OPTION JNodeWithChildren;
    class DLL_OPTION JPair;
    class DLL_OPTION JArray;
    class DLL_OPTION JObject;

    struct JSONInvalidRootElement : public std::exception
    {
		const char* what() const noexcept { return "Root element must be a valid Object or Array"; }
    };
    
    struct JSONInvalidNodeType : public std::exception
    {
		const char* what() const noexcept { return "Node was not one of the three valid node types"; }
    };

    struct JSONParsingError : public std::exception
    {
		const char* what() const noexcept { return "Failed to parse the json data. An Object or Array is not ended properly"; }
    };

    class DLL_OPTION JNode
    {
    public:
        static const int TYPE = 0;
        JNode(int type, std::string name = "");
        JNode(const JNode& other);
        virtual ~JNode();
        int getType();
        std::string getName();
        JNode* getParent();
        
        virtual std::string getString(bool last = true, bool prettyPrint = false, std::string extraTabsString = "") = 0;

        
        std::vector<JNode*> getNodesPattern(const std::vector<std::string>& s);

        JNode& operator[](int location);
        JNode& operator[](std::string key);
        size_t size();

    protected:
        friend JNodeWithChildren;
        void setParent(JNode* p);

    private:
        void getNodesPatternInternal(const std::vector<std::string>& s, int offset, std::vector<JNode*>& results);

        std::string name;
        int type = 0;
        JNode* parent = nullptr;
    };

    class DLL_OPTION JNodeWithChildren : public JNode
    {
    public:
        void addNode(JNode* o);
        std::vector<JNode*>& getNodes();

    protected:
        friend JNode;
        std::vector<JNode*> vars;
        SimpleHashMultiMap<std::string, size_t> nameToIndexMap;
    
        JNodeWithChildren(int type, std::string name = "");
        JNodeWithChildren(const JNodeWithChildren& other);
        ~JNodeWithChildren();
    };

    class DLL_OPTION JPair : public JNode
    {
    public:
        static const int TYPE = 1;
        JPair(std::string name = "", std::string value = "");
        JPair(const JPair& other);
        ~JPair();

        std::string getValue();
        std::string escapeInvalidChars();
        void setValue(std::string n);
        virtual std::string getString(bool last = true, bool prettyPrinting = false, std::string extraTabsString = "");
    private:
        void replaceEscapeCodes();
        std::string value;
    };

    class DLL_OPTION JObject : public JNodeWithChildren
    {
    public:
        static const int TYPE = 2;
        JObject(std::string name = "");
        JObject(const JObject& other);
        ~JObject();

        virtual std::string getString(bool last = true, bool prettyPrinting = false, std::string extraTabsString = "");
    };

    class DLL_OPTION JArray : public JNodeWithChildren
    {
    public:
        static const int TYPE = 3;
        JArray(std::string name = "");
        JArray(const JArray& other);
        ~JArray();

        virtual std::string getString(bool last = true, bool prettyPrinting = false, std::string extraTabsString = "");
    };

    class DLL_OPTION SimpleJSON
    {
    public:
        /**
         * @brief Construct a new empty SimpleJSON object.
         *      The root element created is a valid JObject that can be added to.
         *          Note that the root element may be a JArray when loading from other sources.
         * 
         */
        SimpleJSON();

        /**
         * @brief Construct a new SimpleJSON from an existing data stream.
         *      That data stream is assumed to be a valid JSON as a string
         * 
         * @param data 
         */
        SimpleJSON(Streamable<unsigned char>* data);

        /**
         * @brief Construct a new SimpleJSON from a file.
         * 
         * @param filename 
         */
        SimpleJSON(const std::string& filename);

        /**
         * @brief Construct a new SimpleJSON object using an existing complete buffer of data.
         * 
         * @param data 
         * @param size 
         */
        SimpleJSON(unsigned char* data, size_t size);

        /**
         * @brief Copy Constructor for a SimpleJSON object
         *      Makes a deep copy of the json
         * @param other 
         */
        SimpleJSON(const SimpleJSON& other);

        /**
         * @brief Copy Assignment for a SimpleJSON object.
         *      Makes a deep copy of the json
         * @param other 
         */
        void operator=(const SimpleJSON& other);
        
        /**
         * @brief Move Constructor for a SimpleJSON object.
         *      
         * @param other 
         */
        SimpleJSON(SimpleJSON&& other);

        /**
         * @brief Move Assignment for a SimpleJSON object.
         * 
         * @param other 
         */
        void operator=(SimpleJSON&& other);

        ~SimpleJSON();


        JNode* getRootNode();
        
        JNode& operator[](int location);
        JNode& operator[](std::string key);
        size_t size();

        std::string getString(bool prettyPrinting = false);

        std::vector<JNode*> getNodesPattern(const std::vector<std::string>& s);
    private:
        JObject* loadJObject(std::string name);
        JArray* loadJArray(std::string name);
        void loadJObjectOrJArray(JNode* node);
        void initLoad();

        
        bool load(Streamable<unsigned char>* data);
        bool load(std::string filename);
        bool load(unsigned char* data, size_t size);

        bool getNextCharacter(char& c);
        
        Streamable<unsigned char>* datasource = nullptr;
        unsigned char* readBuffer = nullptr;
        size_t readSize = 0;
        size_t readIndex = 0;
        bool continueProcessing = true;

        JNode* rootNode = nullptr; //can be an object or array for some stupid reason
    };
}