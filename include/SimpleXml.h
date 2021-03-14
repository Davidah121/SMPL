#pragma once
#include "SimpleFile.h"

class XmlAttribute
{
public:
    XmlAttribute();
    ~XmlAttribute();

    std::wstring name;
    std::wstring value;
};

class XmlNode
{
public:
    XmlNode();
    ~XmlNode();
    XmlNode(const XmlNode &other);

    bool isEndOfSection();

    std::wstring title;
    std::vector<XmlAttribute> attributes = std::vector<XmlAttribute>();
    std::vector<XmlNode*> childNodes = std::vector<XmlNode*>();
    XmlNode* parentNode = nullptr;
    std::wstring value;
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

    bool load(std::wstring filename);
    void save(std::wstring filename);

    void dispose();

    std::vector<XmlNode*> nodes = std::vector<XmlNode*>();
    static int parseEscapeString(std::wstring escString);
    
private:
    XmlNode* parseXmlLine(std::wstring line);

    void fixParseOnNode(XmlNode* n);

    void saveNode(SimpleFile* f, XmlNode* node);
    void deleteNode(XmlNode* node);

    std::vector<unsigned char> removeCommentsAndInvalidChars(std::vector<unsigned char> fileBytes);
};