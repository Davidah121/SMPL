#include "SimpleXml.h"
#include "StringTools.h"
#include "MathExt.h"

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
        attributes[StringTools::toLowercase(key)] = value;
    }

    void XmlNode::addAttribute(std::pair<std::string, std::string> p)
    {
        attributes[StringTools::toLowercase(p.first)] = p.second;
    }

    std::pair<std::string, std::string> XmlNode::getAttribute(std::string key)
    {
        std::pair<std::string, std::string> pair;
        auto temp = attributes.find(StringTools::toLowercase(key));
        if(temp != attributes.end())
        {
            pair.first = temp->first;
            pair.second = temp->second;
        }
        return pair;
    }

    bool XmlNode::isEndOfSection()
    {
        return isEnd;
    }

    #pragma endregion

    #pragma region SIMPLE_XML

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


    void SimpleXml::save(File file)
    {
        SimpleFile f = SimpleFile(file, SimpleFile::WRITE);

        if(f.isOpen())
        {
            for(XmlNode* v : nodes)
            {
                saveNode(&f, v);
            }

            f.close();
        }
    }
    
    std::string SimpleXml::convertToString()
    {
        std::string s = "";
        for(XmlNode* v : nodes)
        {
            saveNode(s, v);
        }
        return s;
    }

    void SimpleXml::saveNode(SimpleFile* f, XmlNode* node)
    {
        std::string line = "";

        line += "<";

        //has no child nodes, no attributes, and no value
        if(node->childNodes.size()==0 && node->attributes.size()==0 && node->value=="")
        {
            line+="/";
        }

        line += node->title;

        for(std::pair<std::string, std::string> a : node->attributes)
        {
            line += " ";
            line += a.first;
            line += "=\"";
            line += a.second;
            line += "\"";
        }
        
        //has no child nodes and no value, but has at least one attribute
        if(node->childNodes.size()==0 && node->attributes.size()>0 && node->value=="")
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
            saveNode(f, v);
        }

        if(node->childNodes.size()>0 || node->value!="")
        {
            line = "</";
            line += node->title;
            line += ">";

            f->writeString(line);
            f->writeLineBreak();
        }
    }

    void SimpleXml::saveNode(std::string& s, XmlNode* node)
    {
        std::string line = "";

        line += "<";

        //has no child nodes, no attributes, and no value
        if(node->childNodes.size()==0 && node->attributes.size()==0 && node->value=="")
        {
            line+="/";
        }

        line += node->title;

        for(std::pair<std::string, std::string> a : node->attributes)
        {
            line += " ";
            line += a.first;
            line += "=\"";
            line += a.second;
            line += "\"";
        }
        
        //has no child nodes and no value, but has at least one attribute
        if(node->childNodes.size()==0 && node->attributes.size()>0 && node->value=="")
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
            saveNode(s, v);
        }

        if(node->childNodes.size()>0 || node->value!="")
        {
            line = "</";
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
        shouldParseEscape = parseEscape;
        SimpleFile f = SimpleFile(file, SimpleFile::READ | SimpleFile::UTF8);

        bool parsingNode = false;
        std::string innerNodeText = "";

        XmlNode* parentNode = nullptr;
        bool isRecordingText = false;
        bool hitEnd = true;

        if(f.isOpen())
        {
            std::vector<unsigned char> fileBytes = f.readFullFileAsBytes();
            f.close();
            
            fileBytes = removeCommentsAndInvalidChars(fileBytes);

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

                    innerNodeText = "";
                    isRecordingText = false;
                    if(parentNode==nullptr)
                    {
                        if(!node->isEndOfSection())
                        {
                            nodes.push_back(node);
                            parentNode = node;
                        }
                        else
                        {
                            if(node->attributes.size()>0)
                            {
                                nodes.push_back(node);
                            }
                            parentNode = node->parentNode;
                        }
                    }
                    else
                    {
                        node->parentNode = parentNode;
                        if(!node->isEndOfSection())
                        {
                            parentNode->childNodes.push_back(node);
                            parentNode = node;
                        }
                        else
                        {
                            if(node->title == parentNode->title && node->isEndOfSection() && slashAtFront)
                            {
                                parentNode = parentNode->parentNode;
                            }
                            else
                            {
                                parentNode->childNodes.push_back(node);
                                node->parentNode = parentNode;
                            }
                            
                        }
                    }
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

    bool SimpleXml::loadFromBytes(unsigned char* bytes, size_t size, bool parseEscape)
    {
        shouldParseEscape = parseEscape;

        bool parsingNode = false;
        std::string innerNodeText = "";

        XmlNode* parentNode = nullptr;
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

                    innerNodeText = "";
                    isRecordingText = false;
                    if(parentNode==nullptr)
                    {
                        if(!node->isEndOfSection())
                        {
                            nodes.push_back(node);
                            parentNode = node;
                        }
                        else
                        {
                            if(node->attributes.size()>0)
                            {
                                nodes.push_back(node);
                            }
                            parentNode = node->parentNode;
                        }
                    }
                    else
                    {
                        node->parentNode = parentNode;
                        if(!node->isEndOfSection())
                        {
                            parentNode->childNodes.push_back(node);
                            parentNode = node;
                        }
                        else
                        {
                            if(node->title == parentNode->title && node->isEndOfSection() && slashAtFront)
                            {
                                parentNode = parentNode->parentNode;
                            }
                            else
                            {
                                parentNode->childNodes.push_back(node);
                                node->parentNode = parentNode;
                            }
                            
                        }
                    }
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

        size_t indexOfFirstSpace = line.find_first_of(' ');
        std::string title = line.substr(0, indexOfFirstSpace);

        std::string attribString = line;

        node = new XmlNode();

        if(line[0] == '/' || line[line.size()-1] == '/')
        {
            node->isEnd = true;

            if(line[0] == '/')
                title = line.substr(1,indexOfFirstSpace);
            
            if(line[line.size()-1]=='/')
            {
                attribString.pop_back();
                title = attribString.substr(0, indexOfFirstSpace);
            }
        }
        else if((line[0] == '?' && line[line.size()-1] == '?') || line[0] == '!')
        {
            //xml declaration
            node->isEnd = true;
            title = line.substr(1, indexOfFirstSpace);
            attribString.pop_back();
        }

        node->title = title;
        
        //split with parameters. No split if surrounded by quotation marks. split by space then by equals
        std::pair<std::string, std::string> attrib = std::pair<std::string, std::string>();
        std::string tempTitle = "";
        std::string tempValue = "";
        bool inQuotes = false;
        bool settingValue = false;

        if(indexOfFirstSpace!=SIZE_MAX)
        {
            for(size_t i=indexOfFirstSpace+1; i<attribString.size(); i++)
            {
                bool doThing=true;
                if(attribString[i] == '\"')
                {
                    inQuotes = !inQuotes;
                    doThing = false;
                }

                if((attribString[i] == ' ' && inQuotes == false) || i==attribString.size()-1)
                {
                    if(i==attribString.size() && doThing)
                    {
                        if(!inQuotes)
                        {
                            //invalid
                            delete node;
                            return nullptr;
                        }
                        attrib.second+=attribString[i];
                    }

                    if(attrib.first != "")
                        node->addAttribute(attrib);
                    
                    attrib = std::pair<std::string, std::string>();
                    settingValue = false;
                }
                else if(doThing)
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

        for(std::pair<std::string, std::string> k : n->attributes)
        {
            actualString = "";
            tempString = "";
            proc = false;
            for(char c : k.second)
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
            k.second = actualString;

            //No longer done through references so this is required.
            n->addAttribute(k);
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

    #pragma endregion

} //NAMESPACE glib END