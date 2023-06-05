#pragma once
#include "SimpleFile.h"
#include "SimpleHashMap.h"
// #include <unordered_map>

namespace glib
{
    struct ChildNode;
    class XmlNode;
    class SimpleXml;
    struct ChildNode
    {
        static const bool TYPE_VALUE = false;
        static const bool TYPE_NODE = true;
        
        bool type = false;
        std::string value = "";
        XmlNode* node = nullptr;
        ChildNode()
        {

        }

        ChildNode(XmlNode* n)
        {
            type = TYPE_NODE;
            node = n;
            value.clear();
        }
        ChildNode(std::string v)
        {
            type = TYPE_VALUE;
            value = v;
            node = nullptr;
        }
    };

    class XmlNode
    {
    public:
        /**
         * @brief Construct a new XmlNode object
         *      Contains the title of the node, childNodes, attributes for the node, and its parent node.
         * 
         *      If it contains a value, it will be the concatenation of the values of the children nodes that are of TYPE_VALUE.
         *          This way allows children's values to be considered as well and maintain their placement.
         * 
         */
        XmlNode();

        /**
         * @brief Construct a new XmlNode object from another XmlNode object
         * 
         * @param other 
         */

        XmlNode(const XmlNode &other);
        /**
         * @brief Destroy the XmlNode object
         * 
         */
        ~XmlNode();

        void addAttribute(std::string key, std::string value);
        void addAttribute(std::pair<std::string, std::string> p);
        void addAttribute(HashPair<std::string, std::string> p);
        HashPair<std::string, std::string>* getAttribute(std::string key);

        std::string getTitle();
        std::string getValue();
        
        void addChild(XmlNode* n);
        void addValue(std::string s);

        std::vector<XmlNode*> getNodesPattern(std::vector<std::string>& nameOrder, size_t offset=0);
        std::vector<ChildNode>& getChildNodes();
        SimpleHashMap<std::string, std::string>& getRawAttributes();

        void setTitle(std::string s);

    private:
        friend class SimpleXml;

        void getNodesInternal(std::vector<std::string>& nameOrder, size_t offset, std::vector<XmlNode*>& results);
        /**
         * @brief Determines if the node an ending node. Ending nodes are only used when loading data.
         * 
         * @return true 
         * @return false 
         */
        bool isEndOfSection();
        bool isEnd = false;
        
        std::string title;

        SimpleHashMap<std::string, std::string> attributes = SimpleHashMap<std::string, std::string>(true); //Unique only
        std::vector<ChildNode> childNodes = std::vector<ChildNode>();
        XmlNode* parentNode = nullptr;
        SimpleHashMap<std::string, size_t> nameToIndexMap = SimpleHashMap<std::string, size_t>(false); //Multimap
    };

    class SimpleXml
    {
    public:
        static const bool TYPE_XML = false;
        static const bool TYPE_HTML = true;
        static const std::vector<std::string> knownVoidTags;
        static const std::vector<std::string> knownHTMLTags;
        static const std::vector<std::string> knownHTMLBlockTags;
        static const std::vector<std::string> knownHTMLInlineTags;
        
        /**
         * @brief Construct a new SimpleXml object
         *      Contains information about an XML file.
         *      Can be constructed in code to save data or load in data to parse.
         * 
         */
        SimpleXml();

        /**
         * @brief Construct a new SimpleXml object by loading data from a file.
         *      Contains information about an XML file.
         *  
         * @param file
         */
        SimpleXml(File file);

        /**
         * @brief Destroy the SimpleXml object
         * 
         */
        ~SimpleXml();

        /**
         * @brief Loads an XML file.
         * 
         * @param file 
         * @return true 
         * @return false 
         */
        bool load(File file, bool parseEscape = true);

        /**
         * @brief Loads an XML file using the string as text data as opposed to a file.
         *      
         * 
         * @param str 
         * @param parseEscape 
         * @return true 
         * @return false 
         */
        bool loadFromBytes(unsigned char* bytes, size_t size, bool parseEscape = true);
        

        /**
         * @brief Saves the XmlNode data into a file.
         * 
         * @param file 
         * @param includeTabs
         *      Whether to include tabs for better formatting.
         *      Default value is true
         */
        void save(File file, bool includeTabs = true);

        /**
         * @brief Converts all of the XML into a compatible string following the XML format.
         * 
         * @return std::string 
         * @param includeTabs
         *      Whether to include tabs for better formatting.
         *      Default value is true
         */
        std::string convertToString(bool includeTabs = true);

        /**
         * @brief Disposes of the memory used by the object.
         *      All nodes maintain by the object including their children should be considered invalid.
         */
        void dispose();

        /**
         * @brief Parses an escape sequence in XML to the original character it represents.
         * 
         * @param escString 
         * @return int 
         */
        static int parseEscapeString(std::string escString);

        void addNode(XmlNode* n);

        std::vector<XmlNode*> getNodesPattern(std::vector<std::string>& nameOrder);
        std::vector<XmlNode*>& getNodes();

        bool getType();
        bool getValidXml();

    private:
        bool type = TYPE_XML;
        bool validXml = true;

        XmlNode* parseXmlLine(std::string line);

        /**
         * @brief parses all escape strings in the xml node.
         *      Parses the attributes and value nodes for escape sequences. 
         *      Handles the children nodes as well so it should only be called on the root node.
         * 
         * @param n 
         */
        void fixParseOnNode(XmlNode* n);

        void saveNode(SimpleFile* f, XmlNode* node, int tabs);
        void saveNode(std::string& s, XmlNode* node, int tabs);

        bool shouldParseEscape = true;
        std::vector<XmlNode*> nodes = std::vector<XmlNode*>();
        SimpleHashMap<std::string, size_t> nameToIndexMap = SimpleHashMap<std::string, size_t>(false); //Multimap

        std::vector<unsigned char> removeCommentsAndInvalidChars(std::vector<unsigned char> fileBytes);
        std::vector<unsigned char> removeCommentsAndInvalidChars(unsigned char* fileBytes, size_t size);
    };

} //NAMESPACE glib END