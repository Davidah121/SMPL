#include "SimpleFile.h"

class XmlAttribute
{
public:
    XmlAttribute();
    ~XmlAttribute();

    std::string name;
    std::string value;
};

class XmlNode
{
public:
    XmlNode();
    ~XmlNode();

    bool isEndOfSection();

    std::string title;
    std::vector<XmlAttribute> attributes = std::vector<XmlAttribute>();
    std::vector<XmlNode*> childNodes = std::vector<XmlNode*>();
    XmlNode* parentNode = nullptr;
    std::string value;
private:
    friend class SimpleXml;
    bool isEnd = false;
};

class SimpleXml
{
public:
    SimpleXml();
    ~SimpleXml();

    bool load(std::string filename);
    void save(std::string filename);

    void dispose();

    std::vector<XmlNode*> nodes = std::vector<XmlNode*>();
private:
    XmlNode* parseXmlLine(std::string line);

    void saveNode(SimpleFile* f, XmlNode* node);
    void deleteNode(XmlNode* node);

    std::vector<unsigned char> removeCommentsAndInvalidChars(std::vector<unsigned char> fileBytes);
};