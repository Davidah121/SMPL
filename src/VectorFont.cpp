#include "VectorFont.h"
#include "StringTools.h"

namespace glib
{
        
    const Class VectorFont::globalClass = Class("VectorFont", {&Font::globalClass});

    VectorFont::VectorFont() : Font()
    {
		setClass(globalClass);
    }

    VectorFont::~VectorFont()
    {
        
    }

    VectorGraphic* VectorFont::getGraphic(int index)
    {
        return fontSprite.getGraphic(index);
    }

    VectorSprite* VectorFont::getVectorSprite()
    {
        return &fontSprite;
    }

    Image* VectorFont::getImage(int index)
    {
        double scaleValue = (double)this->getFontSize() / this->getOriginalFontSize();
        
        Image* glyphImage = this->cachedGlyphs.getData(index);
        if(glyphImage == nullptr)
        {
            //create new image and add to cachedGlyphs
            VectorGraphic* vecGlyph = fontSprite.getGraphic(index);

            if(vecGlyph != nullptr)
            {
                //can be added.
                Image* newGlyphImg = new Image((int)(baseWidth*scaleValue), (int)(baseHeight*scaleValue));
                vecGlyph->setTransform(MathExt::scale2D(scaleValue, scaleValue));
                vecGlyph->draw(newGlyphImg);

                cachedGlyphs.addData(index, newGlyphImg);
                return newGlyphImg;
            }
        }
        else
        {
            if(this->getFontSize() == glyphImage->getHeight())
            {
                return glyphImage;
            }
            else
            {
                //Should be resized
                //create new image and add to cachedGlyphs
                VectorGraphic* vecGlyph = fontSprite.getGraphic(index);

                if(vecGlyph != nullptr)
                {
                    Image* newGlyphImg = new Image((int)(baseWidth*scaleValue), (int)(baseHeight*scaleValue));
                    //can be added.
                    vecGlyph->setTransform(MathExt::scale2D(scaleValue, scaleValue));
                    vecGlyph->draw(newGlyphImg);

                    cachedGlyphs.addData(index, newGlyphImg);
                    return newGlyphImg;
                }
            }

        }

        return nullptr;
    }

    bool VectorFont::load(File file)
    {
        if(file.getExtension() == ".svg")
        {
            return loadSVGFont(file);
        }
        else if(file.getExtension() == ".ttf" || file.getExtension() == ".otf")
        {
            return loadOTFFont(file);
        }
        return false;
    }

    bool VectorFont::loadSVGFont(File file)
    {
        SimpleXml f = SimpleXml();
        if(!f.load(file))
        {
            return false;
        }

        for(XmlNode* n : f.nodes)
        {
            if(StringTools::equalsIgnoreCase<char>(n->title, "svg"))
            {
                XmlNode* cNode;
                for(int index=0; index<n->childNodes.size(); index++)
                {
                    cNode = n->childNodes[index];
                    if( StringTools::equalsIgnoreCase<char>(cNode->title, "defs") )
                    {
                        for(XmlNode* q : cNode->childNodes)
                        {
                            if( StringTools::equalsIgnoreCase<char>(q->title, "font") )
                            {
                                cNode = q;
                                break;
                            }
                        }
                        break;
                    }
                }

                if( StringTools::equalsIgnoreCase<char>(cNode->title, "font") )
                {
                    for(XmlAttribute a : cNode->attributes)
                    {
                        if( StringTools::equalsIgnoreCase<char>(a.name, "horiz-adv-x") )
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
                    widthAttrib.value = "0";
                    XmlAttribute heightAttrib = XmlAttribute();
                    heightAttrib.name = "height";
                    heightAttrib.value = "0";
                    
                    XmlAttribute transform = XmlAttribute();
                    transform.name = "transform";
                    transform.value = "translate(0,0) scale(1,-1)";
                    
                    parentNode.attributes.push_back( widthAttrib );
                    parentNode.attributes.push_back( heightAttrib );
                    
                    //process font information
                    for(int i=0; i<cNode->childNodes.size(); i++)
                    {
                        XmlNode* fontChild = cNode->childNodes[i];
                        if(StringTools::equalsIgnoreCase<char>(fontChild->title, "glyph") || StringTools::equalsIgnoreCase<char>(fontChild->title, "missing-glyph"))
                        {
                            FontCharInfo fc;
                            fc.x=0;
                            fc.y=0;
                            fc.xOffset = 0;
                            fc.yOffset = 0;
                            fc.width = width;
                            fc.height = height;
                            fc.horizAdv = baseHorizontalAdvance;
                            fc.unicodeValue = -1;   //assuming missing-glyph

                            for(XmlAttribute& a : fontChild->attributes)
                            {
                                if(StringTools::equalsIgnoreCase<char>(a.name, "unicode"))
                                {
                                    auto stuff = StringTools::utf8ToIntString(a.value);
                                    if(stuff.size() > 0)
                                        fc.unicodeValue = stuff.front();

                                }
                                else if(StringTools::equalsIgnoreCase<char>(a.name, "horiz-adv-x"))
                                {
                                    fc.horizAdv = (int)MathExt::ceil( stod(a.value));
                                }
                            }

                            //turn the xmlNode into a path and pass it into a vectorgrahpic
                            VectorGraphic* fontGraphic = new VectorGraphic();
                            
                            XmlNode parsedNode = XmlNode(*fontChild);
                            parsedNode.title = "path";
                            parsedNode.attributes.push_back(transform);

                            parentNode.childNodes.push_back(&parsedNode);

                            fontGraphic->load(&parentNode);
                            
                            parentNode.childNodes.pop_back();

                            this->charInfoList.push_back(fc);
                            this->fontSprite.addGraphic(fontGraphic);
                            
                        }
                        else if(StringTools::equalsIgnoreCase<char>(fontChild->title, "font-face"))
                        {
                            for(XmlAttribute a : fontChild->attributes)
                            {
                                if( StringTools::equalsIgnoreCase<char>(a.name, "bbox"))
                                {
                                    //Note that minX and minY do not contribute to the viewBox. Just the transform
                                    auto lazyCode = StringTools::splitStringMultipleDeliminators(a.value, " ,");
                                    if(lazyCode.size() == 4)
                                    {
                                        width = MathExt::abs(StringTools::toInt(lazyCode[2]));
                                        height = MathExt::abs(StringTools::toInt(lazyCode[3]));
                                        parentNode.attributes[0].value = lazyCode[2];
                                        parentNode.attributes[1].value = lazyCode[3];

                                        this->originalFontSize = height;
                                        fontSize = height;
                                        baseWidth = width;
                                        baseHeight = height;
                                    }
                                }
                                else if(StringTools::equalsIgnoreCase<char>(a.name, "ascent"))
                                {
                                    transform.value = "translate(0," + a.value + ") scale(1,-1)";
                                }
                            }
                        }
                        
                    }
                }
            }

        }

        return true;
    }

} //NAMESPACE glib END