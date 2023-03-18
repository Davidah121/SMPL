#include "SimpleXml.h"
#include "StringTools.h"

namespace glib
{
    #pragma region XML_NODE

    XmlNode::XmlNode()
    {
        
    }

    XmlNode::~XmlNode()
    {
        for(XmlNode* child : childNodes)
        {
            delete child;
        }
        childNodes.clear();
        attributes.clear();
    }


    XmlNode::XmlNode(const XmlNode &other)
    {
        this->title = other.title;
        this->value = other.value;
        this->attributes = other.attributes;
        this->isEnd = other.isEnd;
        this->parentNode = other.parentNode;
        
        for(XmlNode* cNode : other.childNodes)
        {
            this->childNodes.push_back( new XmlNode(*cNode) );
        }
    }

    void XmlNode::addAttribute(std::string key, std::string value)
    {
        attributes.add(StringTools::toLowercase(key), value);
    }

    void XmlNode::addAttribute(std::pair<std::string, std::string> p)
    {
        attributes.add(StringTools::toLowercase(p.first), p.second);
    }

    void XmlNode::addAttribute(HashPair<std::string, std::string> p)
    {
        attributes.add(StringTools::toLowercase(p.key), p.data);
    }

    HashPair<std::string, std::string>* XmlNode::getAttribute(std::string key)
    {
        return attributes.get(key);
    }

    std::vector<XmlNode*> XmlNode::getNodesPattern(std::vector<std::string>& nameOrder, size_t offset)
    {
        std::vector<XmlNode*> result;
        getNodesInternal(nameOrder, offset, result);
        return result;
    }

    void XmlNode::getNodesInternal(std::vector<std::string>& nameOrder, size_t offset, std::vector<XmlNode*>& results)
    {
        if(offset == nameOrder.size())
        {
            results.push_back(this);
            return;
        }
        if(offset < nameOrder.size())
        {
            //attempt to find nameOrder[offset] in the nameToIndexMap
            auto allStuff = nameToIndexMap.getAll();
            std::vector<HashPair<std::string, size_t>*> it = nameToIndexMap.getAll(nameOrder[offset]);
            for(HashPair<std::string, size_t>* c : it)
            {
                if(c != nullptr)
                {
                    childNodes[c->data]->getNodesInternal(nameOrder, offset+1, results);
                }
            }
        }
    }

    std::vector<XmlNode*>& XmlNode::getChildNodes()
    {
        return childNodes;
    }

    SimpleHashMap<std::string, std::string>& XmlNode::getRawAttributes()
    {
        return attributes;
    }

    void XmlNode::addChild(XmlNode* n)
    {
        if(n != nullptr)
        {
            nameToIndexMap.add(n->getTitle(), childNodes.size());
            childNodes.push_back(n);
        }
    }

    std::string XmlNode::getTitle()
    {
        return title;
    }

    std::string XmlNode::getValue()
    {
        return value;
    }

    void XmlNode::setTitle(std::string s)
    {
        title = StringTools::toLowercase(s);
    }
    void XmlNode::setValue(std::string s)
    {
        value = s;
    }

    bool XmlNode::isEndOfSection()
    {
        return isEnd;
    }

    #pragma endregion

    #pragma region SIMPLE_XML

    
    const std::vector<std::string> SimpleXml::knownVoidTags = {"area", "base", "br", "col", "command", "embed", "hr", "img", "input", "keygen", "link", "meta", "param", "source", "track", "wbr"};

    SimpleXml::SimpleXml()
    {

    }

    SimpleXml::SimpleXml(File f)
    {
        load(f);
    }

    SimpleXml::~SimpleXml()
    {
        dispose();
    }

    void SimpleXml::dispose()
    {
        for(XmlNode* n : nodes)
        {
            if(n!=nullptr)
                delete n;
        }
        nodes.clear();
    }

    void SimpleXml::save(File file, bool includeTabs)
    {
        SimpleFile f = SimpleFile(file, SimpleFile::WRITE);

        if(f.isOpen())
        {
            if(includeTabs)
            {
                for(XmlNode* v : nodes)
                {
                    saveNode(&f, v, 0);
                }
            }
            else
            {
                for(XmlNode* v : nodes)
                {
                    saveNode(&f, v, -1);
                }
            }

            f.close();
        }
    }
    
    std::string SimpleXml::convertToString(bool includeTabs)
    {
        std::string s = "";
        if(includeTabs)
        {
            for(XmlNode* v : nodes)
            {
                saveNode(s, v, 0);
            }
        }
        else
        {
            for(XmlNode* v : nodes)
            {
                saveNode(s, v, -1);
            }
        }
        return s;
    }

    void SimpleXml::saveNode(SimpleFile* f, XmlNode* node, int tabs)
    {
        std::string line = "";
        for(int i=0; i<tabs; i++)
        {
            line += "\t";
        }

        line += "<";

        //has no child nodes, no attributes, and no value
        std::vector<HashPair<std::string, std::string>*> attribsAsArr = node->attributes.getAll();

        if(node->childNodes.size()==0 && attribsAsArr.size()==0 && node->value=="")
        {
            line+="/";
        }

        line += node->title;

        for(HashPair<std::string, std::string>* a : attribsAsArr)
        {
            line += " ";
            line += a->key;
            line += "=\"";
            line += a->data;
            line += "\"";
        }
        
        //has no child nodes and no value, but has at least one attribute
        if(node->childNodes.size()==0 && attribsAsArr.size()>0 && node->value=="")
        {
            line += "/";
        }
        line += ">";

        f->writeString(line);

        if(node->value=="" && node->childNodes.size()>0)
            f->writeLineBreak();
        else if(node->value!="")
            f->writeString(node->value);
        else
            f->writeLineBreak();
        
        
        for (XmlNode* v : node->childNodes)
        {
            if(tabs >= 0)
                saveNode(f, v, tabs+1);
            else
                saveNode(f, v, tabs);
        }

        if(node->childNodes.size()>0 || node->value!="")
        {
            line = "";
            for(int i=0; i<tabs; i++)
            {
                line += "\t";
            }

            line += "</";
            line += node->title;
            line += ">";

            f->writeString(line);
            f->writeLineBreak();
        }
    }

    void SimpleXml::saveNode(std::string& s, XmlNode* node, int tabs)
    {
        std::string line = "";
        for(int i=0; i<tabs; i++)
        {
            line += "\t";
        }

        line += "<";

        std::vector<HashPair<std::string, std::string>*> attribsAsArr = node->attributes.getAll();

        //has no child nodes, no attributes, and no value
        if(node->childNodes.size()==0 && attribsAsArr.size()==0 && node->value=="")
        {
            line+="/";
        }

        line += node->title;

        for(HashPair<std::string, std::string>* a : attribsAsArr)
        {
            line += " ";
            line += a->key;
            line += "=\"";
            line += a->data;
            line += "\"";
        }
        
        //has no child nodes and no value, but has at least one attribute
        if(node->childNodes.size()==0 && attribsAsArr.size()>0 && node->value=="")
        {
            line += "/";
        }
        line += ">";

        s += line;

        if(node->value=="" && node->childNodes.size()>0)
            s += "\n";
        else if(node->value!="")
            s += node->value;
        else
            s += "\n";
        
        
        for (XmlNode* v : node->childNodes)
        {
            if(tabs >= 0)
                saveNode(s, v, tabs+1);
            else
                saveNode(s, v, tabs);
        }

        if(node->childNodes.size()>0 || node->value!="")
        {
            line = "";
            for(int i=0; i<tabs; i++)
            {
                line += "\t";
            }
            line += "</";
            line += node->title;
            line += ">";
            s += line + "\n";
        }
    }

    /**
     * No Error Checking currently.
     * Currently Adding error checking and failures.
     * --May Have done that but I can't remember
     */
    bool SimpleXml::load(File file, bool parseEscape)
    {
        type = TYPE_XML;
        SimpleFile f = SimpleFile(file, SimpleFile::READ | SimpleFile::UTF8);
        if(f.isOpen())
        {
            std::vector<unsigned char> fileBytes = f.readFullFileAsBytes();
            f.close();

            if(file.getExtension() == ".html" || file.getExtension() == ".htm")
                type = TYPE_HTML;
            
            return SimpleXml::loadFromBytes(fileBytes.data(), fileBytes.size(), parseEscape);
        }
        return false;
    }

    bool SimpleXml::loadFromBytes(unsigned char* bytes, size_t size, bool parseEscape)
    {
        shouldParseEscape = parseEscape;

        bool parsingNode = false;
        std::string innerNodeText = "";
        bool previousHasClosingTag = false;

        XmlNode* parentNode = nullptr;
        XmlNode* lastNodeParsed = nullptr;
        bool isRecordingText = false;
        bool hitEnd = true;

        if(bytes != nullptr)
        {
            std::vector<unsigned char> fileBytes = removeCommentsAndInvalidChars(bytes, size);

            for(unsigned char byte : fileBytes)
            {
                if(byte=='<' && hitEnd==true)
                {
                    //set value of node
                    if(parentNode!=nullptr)
                    {
                        parentNode->value = innerNodeText;
                    }
                    innerNodeText = "";
                    isRecordingText = false;
                    parsingNode = true;
                    hitEnd=false;
                }
                else if(byte=='>' && hitEnd==false)
                {
                    parsingNode = false;
                    hitEnd=true;
                    
                    XmlNode* node = parseXmlLine(innerNodeText);

                    if(node==nullptr)
                    {
                        dispose();
                        return false;
                    }

                    bool slashAtFront = innerNodeText[0] == '/';
                    bool slashAtEnd = innerNodeText.back() == '/';

                    innerNodeText = "";
                    isRecordingText = false;
                    if(parentNode==nullptr)
                    {
                        if(!node->isEndOfSection())
                        {
                            addNode(node);
                            parentNode = node;
                        }
                        else
                        {
                            if(node->attributes.getSize()>0)
                            {
                                addNode(node);
                            }
                            parentNode = node->parentNode;
                        }
                    }
                    else
                    {
                        node->parentNode = parentNode;
                        if(!node->isEndOfSection())
                        {
                            parentNode->addChild(node);
                            parentNode = node;
                        }
                        else
                        {
                            if(node->title == parentNode->title && slashAtFront)
                            {
                                //valid closing tag. Contains nothing and can be discarded.
                                delete node;
                                node = nullptr;

                                parentNode = parentNode->parentNode;
                            }
                            else
                            {
                                if(slashAtFront)
                                {
                                    //error of some sort. closing tag that does not correspond to the parent.
                                    //delete and continue parsing
                                    if(type == TYPE_HTML)
                                    {
                                        //may be a void tag
                                        if(lastNodeParsed != nullptr)
                                        {
                                            if(lastNodeParsed->getTitle() != node->getTitle())
                                            {
                                                validXml = false;
                                            }
                                            //otherwise valid xml. Still delete the node though.
                                        }
                                    }
                                    else
                                    {
                                        validXml = false;
                                    }

                                    delete node;
                                    node = nullptr;
                                }
                                else
                                {
                                    //valid close but contains stuff
                                    parentNode->addChild(node);
                                    node->parentNode = parentNode;

                                    if(lastNodeParsed != nullptr)
                                    {
                                        if(!previousHasClosingTag && lastNodeParsed->isEndOfSection())
                                        {
                                            //the previous may have been a void tag
                                            validXml = false;
                                        }
                                    }
                                }
                            }
                            
                        }
                    }

                    lastNodeParsed = node;
                    if(node != nullptr)
                        previousHasClosingTag = (slashAtEnd || slashAtFront);
                }
                else
                {
                    if(isRecordingText)
                    {
                        innerNodeText += byte;
                    }
                    else
                    {
                        if(parsingNode)
                        {
                            if( StringTools::isAlphaNumerial(byte, true, false) || byte == '/' || byte == '?' || byte == '!')
                            {
                                isRecordingText=true;
                                innerNodeText += byte;
                            }
                            else
                            {
                                //error has occurred
                                //Must a number, letter, /, or _ at the start or ? or !
                                dispose();
                                return false;
                            }
                        }
                        else
                        {
                            if(byte > 32)
                            {
                                //valid
                                isRecordingText=true;
                                innerNodeText += byte;
                            }
                        }
                    }
                    
                }
            }

            if(shouldParseEscape)
            {
                for(XmlNode* node : nodes)
                {
                    fixParseOnNode(node);
                }
            }

        }
        else
        {
            //StringTools::out << "Could not open file" << StringTools::lineBreak;
            return false;
        }

        return true;
        
    }

    std::vector<unsigned char> SimpleXml::removeCommentsAndInvalidChars(std::vector<unsigned char> fileBytes)
    {
        return removeCommentsAndInvalidChars(fileBytes.data(), fileBytes.size());
    }

    std::vector<unsigned char> SimpleXml::removeCommentsAndInvalidChars(unsigned char* fileBytes, size_t size)
    {
        bool inComment = false;
        bool potentialCommentStuff = false;
        std::vector<unsigned char> newBytes = std::vector<unsigned char>();
        std::string potentialBytes = "";

        for(size_t i=0; i<size; i++)
        {
            if(!inComment)
            {
                if(!potentialCommentStuff)
                {
                    if(fileBytes[i]=='<')
                    {
                        potentialBytes += '<';
                        potentialCommentStuff = true;
                    }
                    else
                    {
                        if(fileBytes[i] >= 32)
                            newBytes.push_back(fileBytes[i]);
                        else if(fileBytes[i]==0x0D || fileBytes[i]==0x0A)
                            newBytes.push_back(' ');
                    }
                }
                else
                {
                    if(fileBytes[i] >= 32)
                        potentialBytes += fileBytes[i];
                    else if(fileBytes[i]==0x0D || fileBytes[i]==0x0A)
                        potentialBytes += ' ';

                    if(potentialBytes.size() == 4)
                    {
                        if(potentialBytes == "<!--")
                        {
                            inComment = true;
                        }
                        else
                        {
                            newBytes.push_back(potentialBytes[0]);
                            newBytes.push_back(potentialBytes[1]);
                            newBytes.push_back(potentialBytes[2]);
                            newBytes.push_back(potentialBytes[3]);
                        }

                        potentialBytes = "";
                        potentialCommentStuff=false;
                    }
                }
            }
            else
            {
                //in comment, check for -->
                if(!potentialCommentStuff)
                {
                    if(fileBytes[i] == '-')
                    {
                        potentialBytes += '-';
                        potentialCommentStuff = true;
                    }
                }
                else
                {
                    if(fileBytes[i] >= 32)
                        potentialBytes += fileBytes[i];
                    else if(fileBytes[i]==0x0D || fileBytes[i]==0x0A)
                        potentialBytes += ' ';
                    
                    

                    if(potentialBytes.size()==3)
                    {
                        if(potentialBytes == "-->")
                        {
                            inComment = false;
                        }
                        
                        potentialBytes = "";
                        potentialCommentStuff = false;
                    }
                }
            }
            
        }

        if(!inComment)
        {
            for(size_t i=0; i<potentialBytes.size(); i++)
            {
                newBytes.push_back( potentialBytes[i] );
            }
        }

        return newBytes;
    }

    XmlNode* SimpleXml::parseXmlLine(std::string line)
    {
        //first, get the title. Will always be first and separated by a space from everything else
        XmlNode* node;
        bool checkDoctype = false;

        size_t indexOfFirstSpace = line.find_first_of(' ');
        std::string title = line.substr(0, indexOfFirstSpace);

        std::string attribString = line.substr(indexOfFirstSpace+1);

        node = new XmlNode();

        if(line[0] == '/' || line[line.size()-1] == '/')
        {
            node->isEnd = true;

            if(line[0] == '/')
                title = line.substr(1,indexOfFirstSpace);
            
            if(line[line.size()-1]=='/')
            {
                attribString.pop_back(); //remove the slash at the end for easier parsing
            }
        }
        else if((line[0] == '?' && line[line.size()-1] == '?') || line[0] == '!')
        {
            //xml declaration
            node->isEnd = true;
        }

        node->setTitle(title);

        if(type == TYPE_HTML)
        {
            //extra work since it could be a void tag where there is no end marker
            if(!node->isEnd)
            {
                std::string tempTitle = node->getTitle();
                for(int i=0; i<knownVoidTags.size(); i++)
                {
                    if(tempTitle == knownVoidTags[i])
                    {
                        node->isEnd = true;
                        break;
                    }
                }
            }
        }

        if(node->getTitle() == "!doctype")
        {
            checkDoctype = true;
        }
        
        //split with parameters. No split if surrounded by quotation marks. split by space then by equals
        std::pair<std::string, std::string> attrib = std::pair<std::string, std::string>();
        std::string tempTitle = "";
        std::string tempValue = "";
        bool inQuotes = false;
        bool settingValue = false;

        if(indexOfFirstSpace!=SIZE_MAX)
        {
            for(size_t i=0; i<attribString.size(); i++)
            {
                bool parseValues=true;
                if(attribString[i] == '\"')
                {
                    inQuotes = !inQuotes;
                    parseValues = false;
                }

                if((attribString[i] == ' ' && inQuotes == false) || i==attribString.size()-1)
                {
                    if(parseValues)
                    {
                        //still parsing values
                        //possible cases: no value, malformed value string
                        
                        if(inQuotes)
                        {
                            //value but haven't ended quote yet. Malformed error.
                            //invalid
                            delete node;
                            return nullptr;
                        }
                        if(!settingValue)
                            attrib.first+=attribString[i]; //Case 1. No value
                    }

                    if(attrib.first != "")
                        node->addAttribute(attrib);
                    
                    attrib = std::pair<std::string, std::string>();
                    settingValue = false;
                }
                else if(parseValues)
                {
                    if(!inQuotes)
                    {
                        if(attribString[i] == '=')
                        {
                            settingValue = true;
                        }
                        else if(settingValue)
                        {
                            if(!inQuotes)
                            {
                                //invalid
                                delete node;
                                return nullptr;
                            }
                            attrib.second += attribString[i];
                        }
                        else
                        {
                            attrib.first += attribString[i];
                        }
                    }
                    else
                    {
                        if(settingValue)
                        {
                            if(!inQuotes)
                            {
                                //invalid
                                delete node;
                                return nullptr;
                            }
                            attrib.second += attribString[i];
                        }
                        else
                        {
                            attrib.first += attribString[i];
                        }
                    }
                }
            }
        }
        
        if(checkDoctype)
        {
            //check if html
            if(node->getAttribute("html") != nullptr)
                type = TYPE_HTML;
        }

        return node;
    }

    int SimpleXml::parseEscapeString(std::string escString)
    {
        //Format: &----;
        
        if(escString.front()=='&' && escString.back()==';')
        {
            //valid format
            bool num = false;
            bool hex = false;
            std::string infoToParse = "";
            for(size_t i=1; i<escString.size()-1; i++)
            {
                if(escString[i] == '#')
                {
                    num = true;
                    continue;
                }

                if(num)
                {
                    if(escString[i] == 'x')
                    {
                        hex = true;
                        continue;
                    }
                }
                
                infoToParse.push_back(escString[i]);
            }

            if(num)
            {
                if(hex)
                {
                    //hex value
                    int charVal = 0;
                    for(size_t i=0; i<infoToParse.size(); i++)
                    {
                        charVal <<= 4;
                        charVal += StringTools::base16ToBase10(infoToParse[i]);
                    }

                    //charVal could be any unicode value. Change later.
                    return charVal;
                }
                else
                {
                    //decimal number
                    return std::stoi( infoToParse );
                }
            }
            else
            {
                //name
                //only XML predefined names
                if(infoToParse == "quot")
                {
                    return '"';
                }
                else if(infoToParse == "amp")
                {
                    return '&';
                }
                else if(infoToParse == "apos")
                {
                    return '\'';
                }
                else if(infoToParse == "lt")
                {
                    return '<';
                }
                else if(infoToParse == "gt")
                {
                    return '>';
                }
            }
            
        }

        return '\0';
    }

    void SimpleXml::fixParseOnNode(XmlNode* n)
    {
        std::string actualString = "";
        std::string tempString = "";
        bool proc = false;

        std::vector<HashPair<std::string, std::string>*> attribsAsArr = n->attributes.getAll();

        for(HashPair<std::string, std::string>* k : attribsAsArr)
        {
            actualString = "";
            tempString = "";
            proc = false;
            for(char c : k->data)
            {
                if(!proc)
                {
                    if(c != '&')
                    {
                        actualString += c;
                    }
                    else
                    {
                        tempString += c;
                        proc=true;
                    }
                }
                else
                {
                    if(c==';')
                    {
                        tempString += ';';
                        int t = parseEscapeString(tempString);
                        std::vector<unsigned char> asUTFSet = StringTools::toUTF8(t);

                        for(unsigned char& c : asUTFSet)
                        {
                            actualString += c;
                        }
                        proc=false;
                        tempString = "";
                    }
                    else
                    {
                        tempString += c;
                    }
                }
            }
            actualString += tempString;
            k->data = actualString;

            //No longer done through references so this is required.
            // n->addAttribute(k->key, k->data);
        }


        //repeat for the value of the node if it has one
        actualString = "";
        tempString = "";
        proc = false;
        for(char c : n->value)
        {
            if(!proc)
            {
                if(c != '&')
                {
                    actualString += c;
                }
                else
                {
                    tempString += c;
                    proc=true;
                }
            }
            else
            {
                if(c==';')
                {
                    tempString += ';';
                    int t = parseEscapeString(tempString);
                    std::vector<unsigned char> asUTFSet = StringTools::toUTF8(t);

                    for(unsigned char& c : asUTFSet)
                    {
                        actualString += c;
                    }
                    proc=false;
                    tempString = "";
                }
                else
                {
                    tempString += c;
                }
            }
        }
        actualString += tempString;
        n->value = actualString;
        
        if(shouldParseEscape)
        {
            for(XmlNode* q : n->childNodes)
            {
                fixParseOnNode(q);
            }
        }
    }

    std::vector<XmlNode*> SimpleXml::getNodesPattern(std::vector<std::string>& nameOrder)
    {
        std::vector<XmlNode*> results;
        if(nameOrder.size() > 0)
        {
            std::vector<HashPair<std::string, size_t>*> it = nameToIndexMap.getAll(nameOrder[0]);
            for(HashPair<std::string, size_t>* c : it)
            {
                if(c != nullptr)
                {
                    nodes[c->data]->getNodesInternal(nameOrder, 1, results);
                }
            }
        }
        return results;
    }
    std::vector<XmlNode*>& SimpleXml::getNodes()
    {
        return nodes;
    }
    
    void SimpleXml::addNode(XmlNode* n)
    {
        if( n != nullptr )
        {
            nameToIndexMap.add(n->getTitle(), nodes.size());
            nodes.push_back(n);
        }
    }

    bool SimpleXml::getType()
    {
        return type;
    }

    bool SimpleXml::getValidXml()
    {
        return validXml;
    }

    #pragma endregion

} //NAMESPACE glib END