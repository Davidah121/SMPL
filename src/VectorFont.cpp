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

    Image* VectorFont::getImage(size_t index)
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
        // SimpleXml f = SimpleXml();
        // if(!f.load(file))
        // {
        //     return false;
        // }

        // for(XmlNode* n : f.getNodes())
        // {
        //     if(StringTools::equalsIgnoreCase<char>(n->getTitle(), "svg"))
        //     {
        //         XmlNode* cNode = nullptr;
        //         for(int index=0; index<n->getChildNodes().size(); index++)
        //         {
        //             cNode = n->getChildNodes()[index];
        //             if( StringTools::equalsIgnoreCase<char>(cNode->getTitle(), "defs") )
        //             {
        //                 for(XmlNode* q : cNode->getChildNodes())
        //                 {
        //                     if( StringTools::equalsIgnoreCase<char>(q->getTitle(), "font") )
        //                     {
        //                         cNode = q;
        //                         break;
        //                     }
        //                 }
        //                 break;
        //             }
        //         }

        //         if( cNode == nullptr )
        //         {
        //             continue;
        //         }

        //         if( StringTools::equalsIgnoreCase<char>(cNode->getTitle(), "font") )
        //         {
        //             auto temp = cNode->getAttribute("horiz-adv-x");
        //             if(temp != nullptr)
        //                 baseHorizontalAdvance = (int)MathExt::ceil( stod(temp->data) );
                    
        //             int width = 0;
        //             int height = 0;
                    
        //             XmlNode parentNode = XmlNode();
        //             parentNode.setTitle("svg");
                    
        //             std::string transformValue = "translate(0,0) scale(1,-1)";
        //             parentNode.addAttribute("width", "0");
        //             parentNode.addAttribute("height", "0");
                    
                    
                    
        //             //process font information
        //             for(int i=0; i<cNode->getChildNodes().size(); i++)
        //             {
        //                 XmlNode* fontChild = cNode->getChildNodes()[i];
        //                 if(StringTools::equalsIgnoreCase<char>(fontChild->getTitle(), "glyph") || StringTools::equalsIgnoreCase<char>(fontChild->getTitle(), "missing-glyph"))
        //                 {
        //                     FontCharInfo fc = {};
        //                     fc.x=0;
        //                     fc.y=0;
        //                     fc.xOffset = 0;
        //                     fc.yOffset = 0;
        //                     fc.width = width;
        //                     fc.height = height;
        //                     fc.horizAdv = baseHorizontalAdvance;
        //                     fc.unicodeValue = -1;   //assuming missing-glyph

        //                     temp = fontChild->getAttribute("unicode");
        //                     if(temp != nullptr)
        //                     {
        //                         auto stuff = StringTools::utf8ToIntString(temp->data);
        //                         if(stuff.size() > 0)
        //                             fc.unicodeValue = stuff.front();
        //                     }

        //                     temp = fontChild->getAttribute("horiz-adv-x");
        //                     if(temp != nullptr)
        //                     {
        //                         fc.horizAdv = (int)MathExt::ceil( stod(temp->data) );
        //                     }

        //                     //turn the xmlNode into a path and pass it into a vectorgrahpic
        //                     VectorGraphic* fontGraphic = new VectorGraphic();
                            
        //                     XmlNode parsedNode = XmlNode(*fontChild);
        //                     parsedNode.setTitle("path");
        //                     parsedNode.addAttribute("transform", transformValue);

        //                     parentNode.addChild(&parsedNode);

        //                     fontGraphic->load(&parentNode);
                            
        //                     parentNode.getChildNodes().pop_back(); // Why this way??? Add a remove Node to avoid this or something

        //                     this->charInfoList.push_back(fc);
        //                     this->fontSprite.addGraphic(fontGraphic);
                            
        //                 }
        //                 else if(StringTools::equalsIgnoreCase<char>(fontChild->getTitle(), "font-face"))
        //                 {
        //                     temp = fontChild->getAttribute("bbox");
        //                     if(temp != nullptr)
        //                     {
        //                         //Note that minX and minY do not contribute to the viewBox. Just the transform
        //                         auto lazyCode = StringTools::splitStringMultipleDeliminators(temp->data, " ,");
        //                         if(lazyCode.size() == 4)
        //                         {
        //                             width = MathExt::abs(StringTools::toInt(lazyCode[2]));
        //                             height = MathExt::abs(StringTools::toInt(lazyCode[3]));
        //                             parentNode.addAttribute("width", lazyCode[2]);
        //                             parentNode.addAttribute("height", lazyCode[3]);

        //                             this->originalFontSize = height;
        //                             fontSize = height;
        //                             baseWidth = width;
        //                             baseHeight = height;
        //                         }
        //                     }

        //                     temp = fontChild->getAttribute("ascent");
        //                     if(temp != nullptr)
        //                     {
        //                         transformValue = "translate(0," + temp->data + ") scale(1,-1)";
        //                     }
                            
        //                 }
                        
        //             }
        //         }
        //     }

        // }

        return true;
    }

} //NAMESPACE glib END