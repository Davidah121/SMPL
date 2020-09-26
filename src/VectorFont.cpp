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

bool VectorFont::load(std::string filename)
{
    SimpleXml f = SimpleXml();
    if(!f.load(filename))
    {
        return false;
    }

    for(XmlNode* n : f.nodes)
    {
        if(StringTools::equalsIgnoreCase(n->title, "svg"))
        {
            XmlNode* cNode;
            for(int index=0; index<n->childNodes.size(); index++)
            {
                cNode = n->childNodes[index];
                if( StringTools::equalsIgnoreCase(cNode->title, "defs") )
                {
                    for(XmlNode* q : cNode->childNodes)
                    {
                        if( StringTools::equalsIgnoreCase(q->title, "font") )
                        {
                            cNode = q;
                            break;
                        }
                    }
                    break;
                }
            }

            if( StringTools::equalsIgnoreCase(cNode->title, "font") )
            {
                for(XmlAttribute a : cNode->attributes)
                {
                    if( StringTools::equalsIgnoreCase(a.name, "horiz-adv-x") )
                    {
                        baseHorizontalAdvance = MathExt::ceil( stod(a.value));
                    }
                    
                }

                int width = 0;
                int height = 0;
                
                XmlNode parentNode = XmlNode();
                parentNode.title = "svg";
                XmlAttribute widthAttrib = XmlAttribute();
                widthAttrib.name = "width";
                widthAttrib.value = to_string(width);
                XmlAttribute heightAttrib = XmlAttribute();
                heightAttrib.name = "height";
                heightAttrib.value = to_string(height);
                
                parentNode.attributes.push_back( widthAttrib );
                parentNode.attributes.push_back( heightAttrib );
                
                //process font information
                for(XmlNode* fontChildren : cNode->childNodes)
                {
                    if(StringTools::equalsIgnoreCase(fontChildren->title, "glyph") || StringTools::equalsIgnoreCase(fontChildren->title, "missing-glyph"))
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
                            if(StringTools::equalsIgnoreCase(a.name, "unicode"))
                            {
                                int unicodeVal = a.value.front();
                                /**
                                for(int i=3, k=0; i>=0 || k<a.value.size(); i--, k++)
                                {
                                    unicodeVal += a.value[k] << (i*8);
                                }
                                **/
                                fc.unicodeValue = unicodeVal;
                            }
                            else if(StringTools::equalsIgnoreCase(a.name, "horiz-adv-x"))
                            {
                                fc.horizAdv = (int)MathExt::ceil( stod(a.value));
                            }
                        }

                        //turn the xmlNode into a path and pass it into a vectorgrahpic
                        VectorGraphic* fontGraphic = new VectorGraphic(width, height);
                        
                        XmlNode parsedNode = XmlNode(*fontChildren);
                        parsedNode.title = "path";
                        parentNode.childNodes.push_back(&parsedNode);

                        fontGraphic->load(&parentNode);
                        
                        parentNode.childNodes.pop_back();

                        this->charInfoList.push_back(fc);
                        this->fontSprite.addGraphic(fontGraphic);
                    }
                    else if(StringTools::equalsIgnoreCase(fontChildren->title, "font-face"))
                    {
                        for(XmlAttribute a : fontChildren->attributes)
                        {
                            if( StringTools::equalsIgnoreCase(a.name, "bbox"))
                            {
                                std::vector<std::string> split = StringTools::splitString(a.value, ',');
                                width = (int) MathExt::ceil(stod(split[2]) - stod(split[0]));
                                height = (int) MathExt::ceil(stod(split[3]) - stod(split[1]));

                                parentNode.attributes[0].value = to_string(width);
                                parentNode.attributes[1].value = to_string(height);
                                
                            }
                        }
                    }
                    
                }
            }
        }

    }

    return true;
}

Image* VectorFont::getImage(int index)
{
    return fontSprite.getGraphic(index)->getImageBuffer();
}

VectorSprite* VectorFont::getVectorSprite()
{
    return &fontSprite;
}