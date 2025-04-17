#include "SimpleXml.h"
#include "StringTools.h"

#define PARSING_VALUE false
#define PARSING_NODE true

namespace smpl
{
    #pragma region XML_NODE

    XmlNode::XmlNode()
    {
        
    }

    XmlNode::~XmlNode()
    {
        for(ChildNode& child : childNodes)
        {
            if(child.node != nullptr && child.type == ChildNode::TYPE_NODE)
                delete child.node;
        }
        childNodes.clear();
        attributes.clear();
    }


    XmlNode::XmlNode(const XmlNode &other)
    {
        this->title = other.title;
        this->attributes = other.attributes;
        this->isEnd = other.isEnd;
        this->parentNode = other.parentNode;
        
        for(const ChildNode& cNode : other.childNodes)
        {
            ChildNode n = ChildNode();
            if(cNode.node != nullptr)
                n = ChildNode(cNode.node);
            else
                n = ChildNode(n.value);
                
            this->childNodes.push_back( n );
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

    std::pair<std::string, std::string>* XmlNode::getAttribute(std::string key)
    {
        return attributes.get(key);
    }

    std::vector<XmlNode*> XmlNode::getNodesPattern(const std::vector<std::string>& nameOrder, size_t offset)
    {
        std::vector<XmlNode*> result;
        getNodesInternal(nameOrder, offset, result);
        return result;
    }

    void XmlNode::getNodesInternal(const std::vector<std::string>& nameOrder, size_t offset, std::vector<XmlNode*>& results)
    {
        if(offset == nameOrder.size())
        {
            results.push_back(this);
            return;
        }
        if(offset < nameOrder.size())
        {
            //attempt to find nameOrder[offset] in the nameToIndexMap
            std::vector<std::pair<std::string, size_t>*> it = nameToIndexMap.getAll(nameOrder[offset]);
            for(std::pair<std::string, size_t>* c : it)
            {
                if(c != nullptr)
                {
                    ChildNode n = childNodes[c->second];
                    if( n.type == ChildNode::TYPE_NODE && n.node != nullptr)
                    {
                        n.node->getNodesInternal(nameOrder, offset+1, results);
                    }
                }
            }
        }
    }

    std::vector<ChildNode>& XmlNode::getChildNodes()
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
            childNodes.push_back(ChildNode(n)); //Create a child node using n as the XMLNode
            n->parentNode = this;
        }
    }
    
    void XmlNode::addValue(std::string s)
    {
        //determine if possible to concatenate with the previous one
        if(childNodes.size() > 0)
        {
            if(childNodes.back().type == ChildNode::TYPE_VALUE)
            {
                //concatenate with the previous one
                childNodes.back().value += s;
                return;
            }
        }

        //add new one
        nameToIndexMap.add("", childNodes.size());
        childNodes.push_back(ChildNode(s));
    }

    std::string XmlNode::getTitle()
    {
        return title;
    }

    std::string XmlNode::getValue()
    {
        std::string finalStr = "";
        std::vector<std::pair<std::string, size_t>*> it = nameToIndexMap.getAll("");
        for(std::pair<std::string, size_t>* c : it)
        {
            if(c != nullptr)
            {
                ChildNode n = childNodes[c->second];
                if( n.type == ChildNode::TYPE_VALUE)
                {
                    finalStr += n.value;
                }
            }
        }
        return finalStr;
    }

    void XmlNode::setTitle(std::string s)
    {
        title = StringTools::toLowercase(s);
    }

    bool XmlNode::isEndOfSection()
    {
        return isEnd;
    }

    #pragma endregion

    #pragma region SIMPLE_XML

    //known html tags that are expected to not have an end. (They can not have children so they should be considered closed immediately.)
    const std::vector<std::string> SimpleXml::knownVoidTags = {"area", "base", "br", "col", "command", "embed", "hr", "img", "input", "keygen", "link", "meta", "param", "source", "track", "wbr"};
    
    //A bunch of html tags.
    const std::vector<std::string> SimpleXml::knownHTMLTags = {"!DOCTYPE", "a", "abbr", "acronym", "address", "applet", "area", "article", "aside", "audio", "b", "base", "basefont", "bdi", "bdo",
                                                                "big", "blockquote", "body", "br", "button", "canvas", "caption", "center", "cite", "code", "col", "colgroup", "data", "datalist",
                                                                "dd", "del", "details", "dfn", "dialog", "dir", "div", "dl", "dt", "em", "embed", "fieldset", "figcaption", "figure", "font", "footer",
                                                                "form", "frame", "frameset", "h1", "h2", "h3", "h4", "h5", "h6", "head", "header", "hr", "html", "i", "iframe", "img", "input", "ins",
                                                                "kbd", "label", "legend", "li", "link", "main", "map", "mark", "meta", "meter", "nav", "noframes", "noscript", "object", "ol", "optgroup",
                                                                "option", "output", "p", "param", "picture", "pre", "progress", "q", "rp", "rt", "ruby", "s", "samp", "script", "section", "select", "small",
                                                                "source", "span", "strike", "strong", "style", "sub", "summary", "sup", "svg", "table", "tbody", "td", "template", "textarea", "tfoot", "th",
                                                                "thead", "time", "title", "tr", "track", "tt", "u", "ul", "var", "video", "wbr"};
    
    //Block elements for html. These close when another element is encoutered if they haven't been closed yet.
    const std::vector<std::string> SimpleXml::knownHTMLBlockTags = {};
    
    //Block elements for html. These close when another element is encoutered if they haven't been closed yet.
    const std::vector<std::string> SimpleXml::knownHTMLInlineTags = {};
    
    SimpleXml::SimpleXml()
    {

    }

    SimpleXml::SimpleXml(File f)
    {
        if(!load(f))
            validXml = false;
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
        std::string tabStr = "";
        for(int i=0; i<tabs; i++)
        {
            tabStr += "\t";
        }
        line += "<";

        //has no child nodes, no attributes, and no value
        std::vector<std::vector<std::pair<std::string, std::string>*>> attribsAsArr = node->attributes.getRawData();

        if(node->childNodes.size()==0 && attribsAsArr.size()==0)
        {
            line+="/";
        }

        line += node->title;

        for(std::vector<std::pair<std::string, std::string>*>& bucket : attribsAsArr)
        {
            for(std::pair<std::string, std::string>* a : bucket)
            {
                line += " ";
                line += a->first;
                line += "=\"";
                line += a->second;
                line += "\"";
            }
        }
        
        //has no child nodes and no value, but has at least one attribute.
        if(node->childNodes.size()==0 && attribsAsArr.size()>0)
        {
            line += "/";
        }
        line += ">";

        f->writeString(tabStr);
        f->writeString(line);
        f->writeLineBreak();

        for (ChildNode& c : node->childNodes)
        {
            if( c.type == ChildNode::TYPE_VALUE )
            {
                f->writeString(tabStr);
                f->writeString(c.value);
                f->writeLineBreak();
            }
            else if( c.type == ChildNode::TYPE_NODE )
            {
                if(tabs >= 0)
                    saveNode(f, c.node, tabs+1);
                else
                    saveNode(f, c.node, tabs);
            }
        }

        if(node->childNodes.size()>0)
        {
            line = "";
            line += "</";
            line += node->title;
            line += ">";
            f->writeString(tabStr);
            f->writeString(line);
            f->writeLineBreak();
        }
    }

    void SimpleXml::saveNode(std::string& s, XmlNode* node, int tabs)
    {
        std::string line = "";
        std::string tabStr = "";
        for(int i=0; i<tabs; i++)
        {
            tabStr += "\t";
        }

        line += "<";

        //has no child nodes, no attributes, and no value
        std::vector<std::vector<std::pair<std::string, std::string>*>> attribsAsArr = node->attributes.getRawData();
        
        if(node->childNodes.size()==0 && attribsAsArr.size()==0)
        {
            line+="/";
        }

        line += node->title;

        for(std::vector<std::pair<std::string, std::string>*>& bucket : attribsAsArr)
        {
            for(std::pair<std::string, std::string>* a : bucket)
            {
                line += " ";
                line += a->first;
                line += "=\"";
                line += a->second;
                line += "\"";
            }
        }

        //has no child nodes and no value, but has at least one attribute
        if(node->childNodes.size()==0 && attribsAsArr.size()>0)
        {
            line += "/";
        }
        line += ">";

        s += tabStr;
        s += line;
        s += '\n';

        for (ChildNode& c : node->childNodes)
        {
            if( c.type == ChildNode::TYPE_VALUE )
            {
                s += tabStr;
                s += c.value;
                s += '\n';
            }
            else if( c.type == ChildNode::TYPE_NODE )
            {
                if(tabs >= 0)
                    saveNode(s, c.node, tabs+1);
                else
                    saveNode(s, c.node, tabs);
            }
        }

        if(node->childNodes.size()>0)
        {
            line = "";
            line += "</";
            line += node->title;
            line += ">";
            s += tabStr;
            s += line;
            s += '\n';
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
        //rule set: (assuming no comments and invalid characters)
        //'<' must be followed by a valid character. No space in front of title text.
        //'>' signifies the end of a tag.
        //All text between '>' and the next '<' are considered to be apart of the value of the node. (Can be multiple values)
        //All text between '<' and '>' are considered to be apart of the attributes of the node.
        //All things must be encapsulated in a root parent. (Not going to enforce)

        //Linebreaks and tabs are ignored. Extra spaces are ignored (only the first is counted.)
        //When parsing html, some nodes may not be ended correctly and the script node may contain tags in it.
        //In that case, anything between the script node is considered apart of its value and all spacing should be perserved.

        
        shouldParseEscape = parseEscape;

        XmlNode* parentNode = nullptr;
        XmlNode* lastNodeClosed = nullptr;
        std::string tempValue = "";
        bool inQuotes = false;
        char quoteVal = 0;

        bool potentialProblem = false; //used for html parsing to determine if it is valid xml.
        bool potentialSpace = false;
        bool mode = PARSING_VALUE; //false=parsing value. true=parsing node

        if(bytes == nullptr)
            return false;
        
        std::vector<unsigned char> fileBytes = removeCommentsAndInvalidChars(bytes, size); //Should do inline so that it does not create more memory than necessary.
        for(unsigned char byte : fileBytes)
        {
            if(byte == '<' && mode==PARSING_VALUE)
            {
                //came across a new node potentially
                if(parentNode != nullptr)
                {
                    if(potentialSpace)
                        if(parentNode->childNodes.size() > 0 && tempValue.size() > 0)
                            tempValue = " " + tempValue;

                    if(tempValue != "")
                        parentNode->addValue(tempValue);
                }
                mode = PARSING_NODE;
                tempValue = "";
                potentialSpace = false;
            }
            else if(byte == '<' && mode==PARSING_NODE)
            {
                //problem in normal XML but okay in html for some reason.
                validXml = false;
                if(type == TYPE_HTML)
                {
                    //If inside quotes, ignore
                    if(inQuotes)
                    {
                        tempValue += byte;
                        continue;
                    }

                    if(parentNode != nullptr)
                    {
                        if(parentNode->getTitle() == "script")
                        {
                            //ignore
                            tempValue += byte;
                            continue;
                        }
                    }


                    //everything gathered should go to the parent node as apart of its value. and then we restart
                    tempValue = "<"+tempValue;
                    if(parentNode != nullptr)
                    {
                        if(potentialSpace)
                            if(parentNode->childNodes.size() > 0 && tempValue.size() > 0)
                                tempValue = " " + tempValue;
                                
                        if(tempValue != "")
                            parentNode->addValue(tempValue);
                    }
                    else
                    {
                        //problem not valid xml or html
                        return false;
                    }
                    mode = PARSING_NODE;
                    tempValue = "";
                    potentialSpace = false;
                }
                else
                {
                    //problem
                    return false;
                }
            }
            else if(byte == '>' && mode==PARSING_NODE)
            {
                mode = PARSING_VALUE;
                //check if it was potentially the script title for the parent
                if(type == TYPE_HTML && parentNode != nullptr)
                {
                    if(parentNode->getTitle() == "script" && tempValue != "/script")
                    {
                        //the data is apart of the value.
                        if(tempValue != "")
                            parentNode->addValue("<" + tempValue + "> ");
                        
                        tempValue = "";
                        continue;
                    }
                }

                XmlNode* n = parseXmlLine(tempValue);
                if(n == nullptr)
                {
                    if(type == TYPE_HTML && parentNode != nullptr)
                    {
                        //Considered okay under HTML for some reason. (Web browsers will load it even if it is invalid.)
                        if(tempValue != "")
                            parentNode->addValue("<"+tempValue+">");
                        tempValue = "";
                        continue;
                    }
                    
                    //error when parsing node
                    validXml = false;
                    return false;
                }

                //set the nodes parent if required and stuff.
                //html has tags that can not contain a value so they are considered closed and don't have children but don't have to end with a slash.
                //Results in invalid xml but it is necessary to be able to read them.
                bool slashAtFront = tempValue[0] == '/';
                bool slashAtEnd = tempValue.back() == '/';
                bool endingTag = n->isEndOfSection();

                tempValue = "";

                //One additional note regarding HTML. For HTML, it seems that if certains tags are not closed, they are closed automatically.
                //This results in invalid xml but can result in valid html. It is a bad part of the standard since it becomes ambiguous as too what is a child or not.
                //Block elements all close when another block element is encountered (except in some cases) but divs don't behave this way eventhough they are block elements.
                //Probably other elements like this too. Here, we won't automatically close any tags and just consider it completely invalid. This will likely be fixed later.
                //Only known void tags will automatically be closed.

                if(parentNode == nullptr)
                {
                    //store at the root and if not ending tag. set as new parent
                    addNode(n);
                    if(!endingTag)
                    {
                        parentNode = n;
                    }
                }
                else
                {
                    if(endingTag)
                    {
                        if(parentNode->getTitle() == n->getTitle())
                        {
                            //ending tag for the parent. Set parent to grandparent
                            parentNode = parentNode->parentNode;
                            delete n;
                        }
                        else
                        {
                            if(!slashAtFront)
                            {
                                //if there is no slash at the front. It is a single element.
                                parentNode->addChild(n);

                                //check if it is valid xml since html can have tags with no closing mark. They may be closed later though so keep that in mind.
                                if(!slashAtEnd)
                                {
                                    if(type == TYPE_HTML)
                                    {
                                        potentialProblem = true;
                                        lastNodeClosed = n;
                                    }
                                }
                            }
                            else
                            {
                                if(potentialProblem)
                                {
                                    //check the last problem node to determine if this was suppose to close it.
                                    if(lastNodeClosed != nullptr)
                                    {
                                        if(lastNodeClosed->getTitle() == n->getTitle())
                                        {
                                            //this is okay.
                                            delete n;
                                            continue;
                                        }
                                    }
                                }

                                //error. Ending tag does not line up with the parent.
                                delete n;
                                validXml = false;

                                if(type == TYPE_XML)
                                    return false; //fatal error for XML. For HTML, this is apparently accepted behavior
                            }
                        }
                    }
                    else
                    {
                        parentNode->addChild(n);
                        parentNode = n;
                    }
                }
            }
            else
            {
                if(tempValue.size() == 0 && mode == PARSING_NODE)
                {
                    if( StringTools::isAlphaNumerial(byte, true, false) || byte == '/' || byte == '?' || byte == '!')
                    {
                        tempValue += byte;
                    }
                    else
                    {
                        //error. Only certain characters allowed at the start of the nodes title.
                        validXml = false;
                        return false;
                    }
                }
                else if(byte > 32)
                {
                    //need to factor in escaped quotes
                    if(!inQuotes)
                    {
                        if(byte == '"' || byte == '\'')
                        {
                            inQuotes = true;
                            quoteVal = byte;
                        }
                    }
                    else
                    {
                        if(byte == quoteVal)
                        {
                            inQuotes = false;
                            quoteVal = 0;
                        }
                    }

                    tempValue += byte;
                }
                else
                {
                    //only add a space if the last value was not a space or some other character that would be considered a space.
                    if(inQuotes)
                    {
                        tempValue += ' ';
                    }
                    else
                    {
                        if(tempValue.size() > 0)
                        {
                            if(tempValue.back() > 32)
                            {
                                tempValue += ' ';
                            }
                        }
                        else
                            potentialSpace = true;
                    }
                }
                
            }
        }

        if(potentialProblem)
            validXml = false;

        if(parseEscape)
        {
            for(int i=0; i<nodes.size(); i++) //WHY MORE THAN 1 ROOT ALLOWED? Probably some HTML thing.
                fixParseOnNode(nodes[i]);
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

        //title can not begin with an invalid character. _ or [a-z] or [A-Z]
        char frontChar = node->getTitle().front();
        bool validRange1 = frontChar >= 'a' && frontChar <= 'z';
        bool validRange2 = frontChar >= 'A' && frontChar <= 'Z';
        
        if(frontChar != '_' && !validRange1 && !validRange2 && frontChar != '?' && frontChar != '!')
        {
            //invalid
            delete node;
            return nullptr;
        }

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
        char startQuoteChar = 0;

        if(indexOfFirstSpace!=SIZE_MAX)
        {
            for(size_t i=0; i<attribString.size(); i++)
            {
                bool parseValues=true;
                if(startQuoteChar == 0)
                {
                    if(attribString[i] == '\"' || attribString[i] == '\'')
                    {
                        startQuoteChar = attribString[i];
                        inQuotes = !inQuotes;
                        parseValues = false;
                    }
                }
                else if(attribString[i] == startQuoteChar)
                {
                    startQuoteChar = 0;
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
                        if(!settingValue && attribString[i] != ' ')
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
        if( n == nullptr )
            return;
        if(!shouldParseEscape)
            return;
        
        std::string actualString = "";
        std::string tempString = "";
        bool proc = false;

        std::vector<std::vector<std::pair<std::string, std::string>*>> attribsAsArr = n->attributes.getRawData();


        for(std::vector<std::pair<std::string, std::string>*>& bucket : attribsAsArr)
        {
            for(std::pair<std::string, std::string>* k : bucket)
            {
                actualString = "";
                tempString = "";
                proc = false;
                for(char c : k->second)
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
                k->second = actualString;

                //No longer done through references so this is required.
                // n->addAttribute(k->key, k->data);
            }
        }

        for(ChildNode& child : n->childNodes)
        {
            if(child.type == ChildNode::TYPE_VALUE)
            {
                //repeat for the value of the node if it has one
                actualString = "";
                tempString = "";
                proc = false;
                for(char c : child.value)
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
                child.value = actualString;
            }
            else
            {
                fixParseOnNode(child.node);
            }
        }
        
    }

    std::vector<XmlNode*> SimpleXml::getNodesPattern(const std::vector<std::string>& nameOrder)
    {
        std::vector<XmlNode*> results;
        if(nameOrder.size() > 0)
        {
            std::vector<std::pair<std::string, size_t>*> it = nameToIndexMap.getAll(nameOrder[0]);
            for(std::pair<std::string, size_t>* c : it)
            {
                if(c != nullptr)
                {
                    nodes[c->second]->getNodesInternal(nameOrder, 1, results);
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