#include "NewGui.h"
#include "Input.h"
#include "System.h"
#include "GraphicsExt.h"

namespace smpl
{
    const RootClass GuiText::globalClass = CREATE_ROOT_CLASS(GuiText, &GuiContent::globalClass);
    const RootClass* GuiText::getClass()
	{
		return &GuiText::globalClass;
	}

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

    void GuiText::setColor(Color c)
    {
        fontColor = c;
        setShouldRender();
    }

    void GuiText::setHighlightColor(Color c)
    {
        highlightColor = c;
        setShouldRender();
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

    void GuiText::setCanWrap(bool w)
    {
        canWrap = w;
        if(w == false)
        {
            maxWidth = -1;
            maxHeight = -1;
        }
        setShouldRender();
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

    GRect GuiText::getCaretBox()
    {
        FontInterface* f = GraphicsInterface::getFont();
        if(f != nullptr)
        {
            Vec2f v;
            if(!caretOnRight)
                v = f->getFont()->getCursorLocation(text, highlightStartIndex, maxWidth);
            else
                v = f->getFont()->getCursorLocation(text, highlightEndIndex, maxWidth);
            return {(int32_t)v.x, (int32_t)v.y, (int32_t)v.x+1, (int32_t)v.y + f->getFont()->getVerticalAdvance()};
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
        
        if(canWrap == false)
        {
            //max width and height are just the fonts bounds
            Box2D bounds = f->getFont()->getBoundingBox(text, -1, -1);
            width = bounds.getWidth();
            height = bounds.getHeight();
        }
        else
        {
            //max width and height are just the fonts bounds
            Box2D bounds = f->getFont()->getBoundingBox(text, maximumWidth, maximumHeight);
            width = bounds.getWidth();
            height = bounds.getHeight();
            maxWidth = maximumWidth;
            maxHeight = maximumHeight;
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
                    size_t sIndex = f->getFont()->getSelectIndex(text, maxWidth, mouseX-getTrueX(), mouseY-getTrueY());

                    if(sIndex != SIZE_MAX)
                    {
                        highlightStartIndex = sIndex;
                        highlightEndIndex = sIndex;
                    }
                    else
                    {
                        //something went wrong. should never occur
                        highlightStartIndex = text.size()-1;
                        highlightEndIndex = text.size()-1;
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
                size_t sIndex = f->getFont()->getSelectIndex(text, maxWidth,  mouseX-getTrueX(), mouseY-getTrueY());

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
                    highlightEndIndex = text.size()-1;
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
            setShouldRender();
    }

    void GuiText::render(SmartMemory<GuiManager> manager)
    {
        //assume that trueX and trueY have been properly updated.
        int actualMaxWidth = maxWidth;
        int actualMaxHeight = maxHeight;
        if(GraphicsInterface::getBoundSurface() != nullptr)
        {
            if(actualMaxWidth < 0)
                actualMaxWidth = GraphicsInterface::getBoundSurface()->getWidth();
            if(actualMaxHeight < 0)
                actualMaxHeight = GraphicsInterface::getBoundSurface()->getHeight();
        }

        GraphicsInterface::setColor(fontColor);
        if(isSelectable)
        {
            GraphicsInterface::drawTextLimitsHighlighted(text, getTrueX(), getTrueY(), actualMaxWidth, actualMaxHeight, canWrap, highlightStartIndex, highlightEndIndex, highlightColor);
        }
        else
        {
            GraphicsInterface::drawTextLimits(text, getTrueX(), getTrueY(), actualMaxWidth, actualMaxHeight, canWrap);
        }
    }

    void GuiText::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        GuiContent::loadDataFromXML(attribs, manager);
        
        auto pair = attribs.get("text");
        if(pair != nullptr)
            text = pair->data;
        attribs.remove(pair);

        pair = attribs.get("font-color");
        if(pair != nullptr)
            fontColor = loadColor(pair->data);
        attribs.remove(pair);

        pair = attribs.get("highlight-color");
        if(pair != nullptr)
            highlightColor = loadColor(pair->data);
        attribs.remove(pair);

        pair = attribs.get("max-width");
        if(pair != nullptr)
            if(loadValueFromAttrib(maxWidth, pair->data))
                maxWidth = -1;
        attribs.remove(pair);

        pair = attribs.get("max-height");
        if(pair != nullptr)
            if(loadValueFromAttrib(maxHeight, pair->data))
                maxHeight = -1;
        attribs.remove(pair);

        pair = attribs.get("selectable");
        if(pair != nullptr)
            isSelectable = (pair->data == "true");
        attribs.remove(pair);

        pair = attribs.get("can-wrap");
        if(pair != nullptr)
            canWrap = (pair->data == "true");
        attribs.remove(pair);
    }

    SmartMemory<GuiItem> GuiText::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiText* ins = new GuiText();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}