#include "NewGui.h"
#include "Input.h"
#include "System.h"
#include "GraphicsExt.h"

namespace smpl
{
    GuiText::GuiText() : GuiContent()
    {
    }
    GuiText::~GuiText()
    {

    }

    void GuiText::setText(std::string s)
    {
        text = s;
        setShouldRender();
    }

    std::string GuiText::getText()
    {
        return text;
    }

    void GuiText::setFontColor(Color c)
    {
        fontColor = c;
        setShouldRender();
    }

    void GuiText::setHighlightColor(Color c)
    {
        highlightColor = c;
        setShouldRender();
    }
    
    Color GuiText::getFontColor()
    {
        return fontColor;
    }
    Color GuiText::getHighlightColor()
    {
        return highlightColor;
    }

    void GuiText::setSelectable(bool s)
    {
        isSelectable = s;
    }

    int GuiText::getHighlightStartIndex()
    {
        return highlightStartIndex;
    }

    int GuiText::getHighlightEndIndex()
    {
        return highlightEndIndex;
    }

    void GuiText::setWrapMode(char w)
    {
        wrapMode = w;
        if(w == Font::NO_WRAP)
        {
            maxWidth = -1;
            maxHeight = -1;
        }
        setShouldRender();
    }
    
    char GuiText::getWrapMode()
    {
        return wrapMode;
    }

    void GuiText::increaseSelectIndex()
    {
        if(highlightStartIndex != highlightEndIndex)
            highlightStartIndex = highlightEndIndex;
        else
        {
            highlightStartIndex++;
            if(highlightStartIndex > text.size())
                highlightStartIndex = text.size();
            highlightEndIndex = highlightStartIndex;
        }
    }
    void GuiText::decreaseSelectIndex()
    {
        if(highlightStartIndex != highlightEndIndex)
            highlightEndIndex = highlightStartIndex;
        else
        {
            highlightStartIndex--;
            if(highlightStartIndex < 0)
                highlightStartIndex = 0;
            highlightEndIndex = highlightStartIndex;
        }
    }

    void GuiText::setSelectIndex(int index)
    {
        if(index < 0)
        {
            highlightStartIndex = text.size();
            highlightEndIndex = text.size();
        }
        else
        {
            highlightStartIndex = index;
            highlightEndIndex = index;
        }
    }
    void GuiText::setSelectIndex(int startIndex, int endIndex)
    {
        highlightStartIndex = startIndex;
        highlightEndIndex = endIndex;
    }
    void GuiText::setCaretSide(bool v)
    {
        caretOnRight = v;
    }
    
    bool GuiText::getCaretSide()
    {
        return caretOnRight;
    }
    
    int GuiText::getFinalWidthOfText()
    {
        return finalWidthOfText;
    }
    int GuiText::getFinalHeightOfText()
    {
        return finalHeightOfText;
    }

    GRect GuiText::getCaretBox()
    {
        FontInterface* f = GraphicsInterface::getFont();
        if(f != nullptr)
        {
            int indexSearchingFor = highlightStartIndex;
            if(caretOnRight)
                indexSearchingFor = highlightEndIndex;
            
            if(f->getFont() == nullptr)
                return {0, 0, 0, 0};
            
            Box2D b = f->getFont()->getCaretBox(UnicodeStringBridge(text), finalWidthOfText, wrapMode, indexSearchingFor);
            return {(int32_t)b.getLeftBound(), (int32_t)b.getTopBound(), (int32_t)b.getRightBound(), (int32_t)b.getBottomBound()};
        }
        return {0,0,0,0};
    }

    void GuiText::layoutUpdate(int offX, int offY, int maximumWidth, int maximumHeight)
    {
        //update width and height of text. Do wrapping if necessary
        x = offX;
        y = offY;
        
        FontInterface* f = GraphicsInterface::getFont();
        if(f == nullptr)
            return;
        if(f->getFont() == nullptr)
            return;
        
        if(wrapMode == Font::NO_WRAP)
        {
            //max width and height are just the fonts bounds
            finalWidthOfText = -1;
            finalHeightOfText = -1;
            
            Box2D bounds = f->getFont()->getBoundingBox(text, -1, Font::NO_WRAP);
            width = bounds.getWidth();
            height = bounds.getHeight();
        }
        else
        {
            //max width and height are just the fonts bounds
            if(maxWidth > -1)
                finalWidthOfText = __min(maxWidth, maximumWidth);
            else
                finalWidthOfText = maximumWidth;
            if(maxHeight > -1)
                finalHeightOfText = __min(maxHeight, maximumHeight);
            else
                finalHeightOfText = maximumHeight;
            
            Box2D bounds = f->getFont()->getBoundingBox(text, finalWidthOfText, Font::WORD_WRAP);
            width = bounds.getWidth();
            height = bounds.getHeight();
        }
    }

    void GuiText::update(SmartMemory<GuiManager> manager)
    {
        int oldSelectStart = highlightStartIndex;
        int oldSelectEnd = highlightEndIndex;
        
        if(isSelectable)
        {
            //get mouseX and mouseY
            int mouseX = 0;
            int mouseY = 0;

            if(manager.getPointer() != nullptr)
            {
                mouseX = manager.getRawPointer()->getMouseX();
                mouseY = manager.getRawPointer()->getMouseY();
            }

            //do selection code. highlight start and end can be the same.
            //makes textbox code a bit easier.
        
            if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
            {
                if(isColliding(mouseX, mouseY))
                {
                    selecting = true;
                    FontInterface* f = GraphicsInterface::getFont();
                    size_t sIndex = f->getFont()->getSelectIndex(text, finalWidthOfText, wrapMode, mouseX-getTrueX(), mouseY-getTrueY());

                    if(sIndex != SIZE_MAX)
                    {
                        highlightStartIndex = sIndex;
                        highlightEndIndex = sIndex;
                    }
                    else
                    {
                        //something went wrong. should never occur
                        highlightStartIndex = text.size();
                        highlightEndIndex = text.size();
                    }
                    startSelectIndex = highlightStartIndex;
                }
                else
                {
                    highlightStartIndex = 0;
                    highlightEndIndex = 0;
                }
            }

            if(Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
            {
                selecting = false;
            }

            if(selecting)
            {
                FontInterface* f = GraphicsInterface::getFont();
                if(f == nullptr)
                    return;
                if(f->getFont() == nullptr)
                    return;
                size_t sIndex = f->getFont()->getSelectIndex(UnicodeStringBridge(text).getData(), finalWidthOfText, wrapMode, mouseX-getTrueX(), mouseY-getTrueY());

                //set to endIndex
                if(sIndex != SIZE_MAX)
                {
                    if(sIndex < startSelectIndex)
                    {
                        highlightStartIndex = sIndex;
                        highlightEndIndex = startSelectIndex;
                        setCaretSide(false);
                    }
                    else
                    {
                        highlightEndIndex = sIndex;
                        highlightStartIndex = startSelectIndex;
                        setCaretSide(true);
                    }
                }
                else
                {
                    highlightEndIndex = text.size();
                    highlightStartIndex = startSelectIndex;
                }
            }

            if(highlightStartIndex < highlightEndIndex)
            {
                if(Input::getKeyDown(Input::KEY_CONTROL) && Input::getKeyPressed('C'))
                {
                    std::string selectedText = text.substr(highlightStartIndex, highlightEndIndex-highlightStartIndex);
                    System::copyToClipboard(selectedText);
                }
            }
        }
        else
        {
            highlightStartIndex = 0;
            highlightEndIndex = 0;
        }

        if(oldSelectStart != highlightStartIndex || oldSelectEnd != highlightEndIndex)
        {
            setShouldRender();
        }
    }
    
    void GuiText::render(SmartMemory<GuiManager> manager)
    {
        GraphicsInterface::setColor(fontColor);

        if(isSelectable)
        {
            GraphicsInterface::drawTextLimitsHighlighted(text, getTrueX(), getTrueY(), finalWidthOfText, finalHeightOfText, wrapMode, highlightStartIndex, highlightEndIndex, highlightColor);
        }
        else
        {
            GraphicsInterface::drawTextLimits(text, getTrueX(), getTrueY(), finalWidthOfText, finalHeightOfText, wrapMode);
        }
    }

    void GuiText::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiContent::loadDataFromXML(attribs, manager);
        
        auto pair = attribs.get("text");
        if(pair != nullptr)
            text = pair->second;
        attribs.remove(pair);

        pair = attribs.get("font-color");
        if(pair != nullptr)
            fontColor = loadColor(pair->second);
        attribs.remove(pair);

        pair = attribs.get("highlight-color");
        if(pair != nullptr)
            highlightColor = loadColor(pair->second);
        attribs.remove(pair);

        pair = attribs.get("max-width");
        if(pair != nullptr)
            if(loadValueFromAttrib(maxWidth, pair->second))
                maxWidth = -1;
        attribs.remove(pair);

        pair = attribs.get("max-height");
        if(pair != nullptr)
            if(loadValueFromAttrib(maxHeight, pair->second))
                maxHeight = -1;
        attribs.remove(pair);

        pair = attribs.get("selectable");
        if(pair != nullptr)
            isSelectable = (pair->second == "true");
        attribs.remove(pair);

        pair = attribs.get("wrap-mode");
        if(pair != nullptr)
        {
            if(pair->second == "character")
                wrapMode = Font::CHARACTER_WRAP;
            else if(pair->second == "word")
                wrapMode = Font::WORD_WRAP;
            else
                wrapMode = Font::NO_WRAP;
        }
        attribs.remove(pair);
    }

    SmartMemory<GuiItem> GuiText::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiText* ins = new GuiText();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}