#pragma once
#include "SimpleFile.h"
#include <unordered_map>

namespace glib
{
    class XmlNode
    {
    public:
        /**
         * @brief Construct a new XmlNode object
         *      Contains the title of the node, a value if it has one, childNodes, attributes for the node, and its parent node.
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
        std::pair<std::string, std::string> getAttribute(std::string key);

        std::string title;
        std::unordered_map<std::string, std::string> attributes = std::unordered_map<std::string, std::string>();
        std::vector<XmlNode*> childNodes = std::vector<XmlNode*>();
        XmlNode* parentNode = nullptr;
        std::string value;
    private:
        friend class SimpleXml;

        /**
         * @brief Determines if the node an ending node. Ending nodes are only used when loading data.
         * 
         * @return true 
         * @return false 
         */
        bool isEndOfSection();
        bool isEnd = false;
    };

    class SimpleXml
    {
    public:
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
         */
        void save(File file);

        /**
         * @brief Converts all of the XML into a compatible string following the XML format.
         * 
         * @return std::string 
         */
        std::string convertToString();

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

        std::vector<XmlNode*> nodes = std::vector<XmlNode*>();
        
    private:
        XmlNode* parseXmlLine(std::string line);

        void fixParseOnNode(XmlNode* n);

        void saveNode(SimpleFile* f, XmlNode* node);
        void saveNode(std::string& s, XmlNode* node);
        void deleteNode(XmlNode* node);

        bool shouldParseEscape = true;

        std::vector<unsigned char> removeCommentsAndInvalidChars(std::vector<unsigned char> fileBytes);
        std::vector<unsigned char> removeCommentsAndInvalidChars(unsigned char* fileBytes, size_t size);
    };

} //NAMESPACE glib END