#include "VectorFont.h"
#include "StringTools.h"

const Class* VectorFont::myClass = new Class("VectorFont", {Font::myClass});
const Class* VectorFont::getClass()
{
    return VectorFont::myClass;
}

VectorFont::VectorFont()
{

}

VectorFont::~VectorFont()
{
    
}

bool VectorFont::load(std::wstring filename)
{
    SimpleXml f = SimpleXml();
    if(!f.load(filename))
    {
        return false;
    }

    for(XmlNode* n : f.nodes)
    {
        if(StringTools::equalsIgnoreCase(n->title, L"svg"))
        {
            XmlNode* cNode;
            for(int index=0; index<n->childNodes.size(); index++)
            {
                cNode = n->childNodes[index];
                if( StringTools::equalsIgnoreCase(cNode->title, L"defs") )
                {
                    for(XmlNode* q : cNode->childNodes)
                    {
                        if( StringTools::equalsIgnoreCase(q->title, L"font") )
                        {
                            cNode = q;
                            break;
                        }
                    }
                    break;
                }
            }

            if( StringTools::equalsIgnoreCase(cNode->title, L"font") )
            {
                for(XmlAttribute a : cNode->attributes)
                {
                    if( StringTools::equalsIgnoreCase(a.name, L"horiz-adv-x") )
                    {
                        baseHorizontalAdvance = MathExt::ceil( stod(a.value));
                    }
                    
                }

                int width = 0;
                int height = 0;
                
                XmlNode parentNode = XmlNode();
                parentNode.title = L"svg";
                XmlAttribute widthAttrib = XmlAttribute();
                widthAttrib.name = L"width";
                widthAttrib.value = std::to_wstring(width);
                XmlAttribute heightAttrib = XmlAttribute();
                heightAttrib.name = L"height";
                heightAttrib.value = std::to_wstring(height);
                
                parentNode.attributes.push_back( widthAttrib );
                parentNode.attributes.push_back( heightAttrib );
                
                //process font information
                for(int i=0; i<cNode->childNodes.size(); i++)
                {
                    XmlNode* fontChildren = cNode->childNodes[i];
                    if(StringTools::equalsIgnoreCase(fontChildren->title, L"glyph") || StringTools::equalsIgnoreCase(fontChildren->title, L"missing-glyph"))
                    {
                        FontCharInfo fc;
                        fc.x=0;
                        fc.y=0;
                        fc.width = width;
                        fc.height = height;
                        fc.horizAdv = baseHorizontalAdvance;
                        fc.unicodeValue = -1;   //assuming missing-glyph

                        for(XmlAttribute a : fontChildren->attributes)
                        {
                            if(StringTools::equalsIgnoreCase(a.name, L"unicode"))
                            {
                                //StringTools::println(a.value);

                                if(a.value.front() == '&' && a.value.back() == ';')
                                {
                                    fc.unicodeValue = SimpleXml::parseEscapeString(a.value);
                                }
                                else
                                {
                                    
                                    if(a.value == L"A")
                                    {
                                        StringTools::println(a.value);
                                        StringTools::println(a.name);
                                        StringTools::println("%d",i);
                                        StringTools::println(fontChildren->attributes[0].value);
                                        fc.unicodeValue = 65;
                                    }
                                    else
                                        fc.unicodeValue = (unsigned char) a.value.front();
                                }
                                
                            
                                /**
                                for(int i=3, k=0; i>=0 || k<a.value.size(); i--, k++)
                                {
                                    unicodeVal += a.value[k] << (i*8);
                                }
                                **/
                            }
                            else if(StringTools::equalsIgnoreCase(a.name, L"horiz-adv-x"))
                            {
                                fc.horizAdv = (int)MathExt::ceil( stod(a.value));
                            }
                        }

                        //turn the xmlNode into a path and pass it into a vectorgrahpic
                        VectorGraphic* fontGraphic = new VectorGraphic(width, height);
                        
                        XmlNode parsedNode = XmlNode(*fontChildren);
                        parsedNode.title = L"path";
                        parentNode.childNodes.push_back(&parsedNode);

                        fontGraphic->load(&parentNode);
                        
                        parentNode.childNodes.pop_back();

                        this->charInfoList.push_back(fc);
                        this->fontSprite.addGraphic(fontGraphic);
                        
                    }
                    else if(StringTools::equalsIgnoreCase(fontChildren->title, L"font-face"))
                    {
                        for(XmlAttribute a : fontChildren->attributes)
                        {
                            if( StringTools::equalsIgnoreCase(a.name, L"bbox"))
                            {
                                StringTools::println(a.value);

                                std::vector<std::wstring> split = StringTools::splitStringMultipleDeliminators(a.value, L", ");
                                
                                if(split.size()==4)
                                {
                                    width = (int) MathExt::ceil(stod(split[2]) - stod(split[0]));
                                    height = (int) MathExt::ceil(stod(split[3]) - stod(split[1]));

                                    parentNode.attributes[0].value = std::to_wstring(width);
                                    parentNode.attributes[1].value = std::to_wstring(height);
                                }
                                else
                                {
                                    //something went wrong
                                    StringTools::println("Something is wrong with the split size of a.value: %d", split.size());
                                }
                                
                                
                            }
                        }
                    }
                    
                }
            }
        }

    }

    return true;
}

VectorGraphic* VectorFont::getGraphic(int index)
{
    return fontSprite.getGraphic(index);
}

VectorSprite* VectorFont::getVectorSprite()
{
    return &fontSprite;
}
