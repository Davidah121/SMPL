#include "SimpleXml.h"
#include "StringTools.h"

#pragma region XML_ATTRIBUTE

XmlAttribute::XmlAttribute()
{

}

XmlAttribute::~XmlAttribute()
{

}

#pragma endregion


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

bool XmlNode::isEndOfSection()
{
    return isEnd;
}

#pragma endregion

#pragma region SIMPLE_XML

SimpleXml::SimpleXml()
{

}

SimpleXml::~SimpleXml()
{
    dispose();
}

void SimpleXml::dispose()
{
    for(XmlNode* n : nodes)
    {
        delete n;
    }
    nodes.clear();
}


void SimpleXml::save(std::string filename)
{
    SimpleFile f = SimpleFile(filename, SimpleFile::WRITE);

    if(f.isOpen())
    {
        for(XmlNode* v : nodes)
        {
            saveNode(&f, v);
        }

        f.close();
    }
}

void SimpleXml::saveNode(SimpleFile* f, XmlNode* node)
{
    std::string line = "";

    line += "<";
    if(node->childNodes.size()==0 && node->attributes.size()==0)
    {
        line+="/";
    }

    line += node->title;

    for(XmlAttribute a : node->attributes)
    {
        line += " ";
        line += a.name;
        line += "=\"";
        line += a.value;
        line += "\"";
    }
    
    if(node->childNodes.size()==0 && node->attributes.size()>0)
    {
        line += "/";
    }
    line += ">";

    f->writeString(line);
    f->writeLineBreak();

    for (XmlNode* v : node->childNodes)
    {
        saveNode(f, v);
    }

    if(node->childNodes.size()!=0)
    {
        line = "</";
        line += node->title;
        line += ">";

        f->writeString(line);
        f->writeLineBreak();
    }
}

/**
 * No Error Checking currently.
 * Currently Adding error checking and failures
 */
bool SimpleXml::load(std::string filename)
{
    SimpleFile f = SimpleFile(filename, SimpleFile::READ);

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
                        if(node->title == parentNode->title && node->isEndOfSection())
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
                        if( StringTools::isAlphaNumerial(byte, true, false) || byte == '/')
                        {
                            isRecordingText=true;
                            innerNodeText += byte;
                        }
                        else
                        {
                            //error has occurred
                            //Must a number, letter, /, or _ at the start
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

    }
    else
    {
        StringTools::out << "Could not open file" << StringTools::lineBreak;
        return false;
    }

    return true;
    
}

std::vector<unsigned char> SimpleXml::removeCommentsAndInvalidChars(std::vector<unsigned char> fileBytes)
{
    bool inComment = false;
    bool potentialCommentStuff = false;
    std::vector<unsigned char> newBytes = std::vector<unsigned char>();
    std::string potentialBytes = "";

    for(int i=0; i<fileBytes.size(); i++)
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
                }
            }
            else
            {
                if(fileBytes[i] >= 32)
                    potentialBytes += fileBytes[i];

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
        for(int i=0; i<potentialBytes.size(); i++)
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

    node->title = title;
    
    //split with parameters. No split if surrounded by quotation marks. split by space then by equals
    XmlAttribute attrib = XmlAttribute();
    std::string tempTitle = "";
    std::string tempValue = "";
    bool inQuotes = false;
    bool settingValue = false;

    if(indexOfFirstSpace!=SIZE_MAX)
    {
        for(int i=indexOfFirstSpace+1; i<attribString.size(); i++)
        {
            bool doThing=true;
            if(attribString[i] == '\"')
            {
                inQuotes = !inQuotes;
                doThing = false;
            }

            if(attribString[i] == ' ' && inQuotes == false || i==attribString.size()-1)
            {
                if(i==attribString.size() && doThing)
                {
                    if(!inQuotes)
                    {
                        //invalid
                        delete node;
                        return nullptr;
                    }
                    attrib.value+=attribString[i];
                }

                if(attrib.name != "")
                    node->attributes.push_back(attrib);
                
                attrib = XmlAttribute();
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
                        attrib.value += attribString[i];
                    }
                    else
                    {
                        attrib.name += attribString[i];
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
                        attrib.value += attribString[i];
                    }
                    else
                    {
                        attrib.name += attribString[i];
                    }
                }
            }
        }
    }
    
    return node;
}

#pragma endregion