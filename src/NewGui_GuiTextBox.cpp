#include "NewGui.h"
#include "Input.h"
#include "System.h"

namespace smpl
{
    GuiTextBox::GuiTextBox() : GuiScrollBar()
    {
        setPadding({5, 5, 5, 5});
        setBorder({1, 1, 1, 1});
        setBackgroundColor({128, 128, 128, 255});
        addChild( SmartMemory<GuiItem>::createNoDelete(&textElement) );
        addChild( SmartMemory<GuiItem>::createNoDelete(&cursorElement) );
        
        cursorElement.setVisible(true);
        textElement.setSelectable(true);

        textElement.setFocusable(false);
        cursorElement.setFocusable(false);
    }

    GuiTextBox::~GuiTextBox()
    {

    }

    void GuiTextBox::addChild(SmartMemory<GuiItem> item)
    {
        GuiScrollBar::addChild(item);
    }

    void GuiTextBox::removeChild(SmartMemory<GuiItem> item)
    {
        GuiScrollBar::removeChild(item);
    }

    GuiText& GuiTextBox::getTextElement()
    {
        return textElement;
    }

    void GuiTextBox::setAllowLineBreaks(bool lb)
    {
        allowLineBreaks = lb;
    }
    
    void GuiTextBox::setDefaultText(std::string s)
    {
        defaultText = s;
    }

    std::string GuiTextBox::getText()
    {
        if(textEmpty)
            return "";
        else
            return textElement.getText();
    }
    
    void GuiTextBox::setTextEmpty(bool v)
    {
        if(textEmpty != v)
        {
            if(v)
            {
                oldFontColor = textElement.getFontColor();
                Color newFontColor = Color{oldFontColor.red, oldFontColor.green, oldFontColor.blue, (unsigned char)(oldFontColor.alpha/2)};
                textElement.setFontColor(newFontColor);
                textElement.setText(defaultText);
                textElement.setSelectIndex(0, 0);
            }
            else
            {
                textElement.setFontColor(oldFontColor);
            }
        }
        textEmpty = v;
    }

    void GuiTextBox::keyboardInput(std::u32string& str)
    {
        bool textChanged = false;
        std::queue<int> chars = Input::getCharactersTyped();
        FontInterface* fiPtr = GraphicsInterface::getFont();
        Font* fPtr = nullptr;
        if(fiPtr != nullptr)
            fPtr = fiPtr->getFont();
        
        //make modifications to str then set once at the end
        int oldSelectionStart = textElement.getHighlightStartIndex();
        int oldSelectionEnd = textElement.getHighlightEndIndex();
        
        int selectionStart = textElement.getHighlightStartIndex();
        int selectionEnd = textElement.getHighlightEndIndex();

        while(!chars.empty())
        {
            if(selectionStart < 0)
                selectionStart = 0;
            if(selectionEnd < 0)
                selectionEnd = 0;

            int v = chars.front();
            chars.pop();
            
            if(v == Input::KEY_LEFT)
            {
                if(Input::getKeyDown(Input::KEY_SHIFT))
                {
                    //selecting too
                    if(textElement.getCaretSide() && selectionEnd!=selectionStart) //caret on the right. move selection back
                    {
                        selectionEnd--;
                        if(selectionEnd <= selectionStart)
                            textElement.setCaretSide(false);
                    }
                    else
                    {
                        if(selectionStart > 0)
                            selectionStart--;
                        textElement.setCaretSide(false);
                    }
                }
                else
                {
                    if(selectionStart > 0 && selectionStart == selectionEnd)
                        selectionStart--;

                    selectionEnd = selectionStart;
                    textElement.setCaretSide(false);
                }

                
                setShouldRender();
                blink = true;
                counter = 500;
            }
            else if(v == Input::KEY_RIGHT)
            {
                if(Input::getKeyDown(Input::KEY_SHIFT))
                {
                    //selecting too
                    if(!textElement.getCaretSide() && selectionEnd!=selectionStart) //caret on the left. move selection back
                    {
                        selectionStart++;
                        if(selectionStart >= selectionEnd)
                            textElement.setCaretSide(true);
                    }
                    else
                    {
                        if(selectionEnd < str.size())
                        selectionEnd++;
                        textElement.setCaretSide(true);
                    }
                }
                else
                {
                    if(selectionEnd < str.size() && selectionStart == selectionEnd)
                    selectionEnd++;
                    selectionStart = selectionEnd;
                    textElement.setCaretSide(true);
                }

                setShouldRender();
                blink = true;
                counter = 500;
            }
            else if(v == Input::KEY_UP)
            {
                int nextSelection = fPtr->moveSelectionUp(str, textElement.getFinalWidthOfText(), textElement.getWrapMode(), selectionStart);
                if(nextSelection < 0)
                    nextSelection = 0;
                selectionStart = nextSelection;
                if(!Input::getKeyDown(Input::KEY_SHIFT))
                {
                    selectionEnd = nextSelection;
                }
                textElement.setCaretSide(false);
                setShouldRender();
                blink = true;
                counter = 500;
            }
            else if(v == Input::KEY_DOWN)
            {
                
                int nextSelection = fPtr->moveSelectionDown(str, textElement.getFinalWidthOfText(), textElement.getWrapMode(), selectionStart);
                if(nextSelection >= str.size())
                    nextSelection = str.size();
                if(nextSelection == 0)
                    nextSelection = str.size();
                
                selectionEnd = nextSelection;
                if(!Input::getKeyDown(Input::KEY_SHIFT))
                {
                    selectionStart = nextSelection;
                }
                textElement.setCaretSide(true);
                setShouldRender();
                blink = true;
                counter = 500;
            }
            else if(v == Input::KEY_BACKSPACE)
            {
                //remove everything between s
                if(selectionStart < selectionEnd)
                {
                    std::u32string nText;
                    for(int i=0; i<str.size(); i++)
                    {
                        if(i < selectionStart || i > selectionEnd-1)
                            nText += str[i];
                    }
                    str = nText;
                    selectionEnd = selectionStart;
                    textChanged = true;
                }
                else if(selectionStart == selectionEnd)
                {
                    std::u32string nText;
                    for(int i=0; i<str.size(); i++)
                    {
                        if(i != selectionStart-1)
                            nText += str[i];
                    }
                    str = nText;
                    selectionStart--;
                    selectionEnd--;
                    textChanged = true;
                }
                if(selectionStart < 0)
                {
                    selectionStart = 0;
                    selectionEnd = 0;
                }
            }
            else if(v == Input::KEY_DELETE)
            {
                if(selectionStart < selectionEnd)
                {
                    std::u32string nText;
                    for(int i=0; i<str.size(); i++)
                    {
                        if(i < selectionStart || i > selectionEnd-1)
                            nText += str[i];
                    }
                    str = nText;
                    selectionEnd = selectionStart;
                    textChanged = true;
                }
                else if(selectionStart == selectionEnd)
                {
                    //select index stays the same
                    std::u32string nText;
                    for(int i=0; i<str.size(); i++)
                    {
                        if(i != selectionStart)
                            nText += str[i];
                    }
                    str = nText;
                    textChanged = true;
                }
            }
            else if(!Input::getKeyDown(Input::KEY_CONTROL))
            {
                if(v >= ' ' || (v == '\n' && allowLineBreaks) || (v == '\r' && allowLineBreaks))
                {
                    if(selectionStart <= selectionEnd)
                    {
                        //equivalent to a delete and insert
                        std::u32string nText;
                        for(int i=0; i<selectionStart; i++)
                        {
                            nText += str[i];
                        }
                        
                        if(v == '\r')
                            nText += '\n';
                        else
                            nText += v;
                        
                        for(int i=selectionEnd; i<str.size(); i++)
                        {
                            nText += str[i];
                        }
                        selectionStart++;
                        selectionEnd = selectionStart;
                        str = nText;
                        textChanged = true;
                    }
                }
            }
        }

        //copy is already handled in the text element
        if(Input::getKeyDown(Input::KEY_CONTROL) && Input::getKeyPressed('X'))
        {
            //cut
            if(selectionStart < selectionEnd) //at least 1 character is selected
            {
                std::u32string nText;
                std::u32string removedText;
                int i=0;
                for(; i<selectionStart; i++)
                {
                    nText.push_back(str[i]);
                }
                for(; i<selectionEnd; i++)
                {
                    removedText.push_back(str[i]);
                }
                for(; i<str.size(); i++)
                {
                    nText.push_back(str[i]);
                }
                str = nText;
                selectionEnd = selectionStart;

                std::string copiedText = StringTools::toUTF8String(removedText);
                System::copyToClipboard(copiedText);
                textChanged = true;
            }
        }
        else if(Input::getKeyDown(Input::KEY_CONTROL) && Input::getKeyPressed('V'))
        {
            //paste
            std::string pastedText = System::pasteFromClipboard();
            std::u32string pTextAsInts = StringTools::utf8ToIntString(pastedText);

            if(selectionStart <= selectionEnd)
            {
                //equivalent to a delete and insert
                std::u32string nText;
                for(int i=0; i<selectionStart; i++)
                {
                    nText.push_back(str[i]);
                }
                
                for(int v : pTextAsInts)
                    nText.push_back(v);
                
                for(int i=selectionEnd; i<str.size(); i++)
                {
                    nText.push_back(str[i]);
                }
                selectionStart += pTextAsInts.size();
                selectionEnd = selectionStart;
                str = nText;
                textChanged = true;
            }
        }
        
        setTextEmpty(str.empty());
        if(!textEmpty)
        {
            if(textChanged)
            {
                std::string finalStr = StringTools::toUTF8String(str);
                textElement.setText(finalStr);
                shouldUpdateCursor = true;
            }
        }
        if(oldSelectionStart != selectionStart || oldSelectionEnd != selectionEnd)
            shouldUpdateCursor = true;
        
        textElement.setSelectIndex(selectionStart, selectionEnd);
    }

    void GuiTextBox::onFocusChanged(SmartMemory<GuiManager> manager, bool changedTo)
    {
        if(changedTo)
        {
            //clear buffer of characters typed.
            Input::clearCharactersTyped();
            blink = true;
            counter = 500;
        }
        else
        {
            blink = false;
            counter = 500;
        }
        shouldUpdateCursor = true;
        updateCursorElementInfo();
        setShouldRender();
    }
    
    void GuiTextBox::updateCursorElementInfo()
    {
        if(shouldUpdateCursor)
        {
            GRect cRect = textElement.getCaretBox();
            cursorElement.setMargin(GRect{cRect.left, cRect.top, 0, 0});
            cursorElement.setMinWidth(cRect.right - cRect.left);
            cursorElement.setMaxWidth(cRect.right - cRect.left);
            cursorElement.setMinHeight(cRect.bottom - cRect.top);
            cursorElement.setMaxHeight(cRect.bottom - cRect.top);
            
            cursorElement.setBackgroundColor(Color{0,0,0,255});

            cursorElement.setVisible(blink);
            shouldUpdateCursor = false;
        }
    }

    void GuiTextBox::update(SmartMemory<GuiManager> manager)
    {
        //Update the layout which will update the text element too.
        GuiScrollBar::update(manager);

        size_t timePassed = System::getCurrentTimeMillis() - startT;
        startT = System::getCurrentTimeMillis();
        
        std::u32string str;
        Font* fptr = nullptr;
        FontInterface* fInterPtr = GraphicsInterface::getFont();
        if(fInterPtr != nullptr)
            fptr = fInterPtr->getFont();
        
        if(fptr == nullptr)
            return;
        
        if(!textEmpty)
            str = UnicodeStringBridge(textElement.getText()).getData();

        // StringTools::println("%d, %d", width, height);
        if(getFocused(manager))
        {
            if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
            {
                int mouseX = manager.getPointer()->getMouseX();
                if(mouseX > textElement.getTrueX() + textElement.width)
                    textElement.setSelectIndex( -1 );
            }
            
            keyboardInput(str);

            counter -= timePassed;
            if(counter < 0)
            {
                blink = !blink;
                counter = 500;
                shouldUpdateCursor = true;
            }
            
            updateCursorElementInfo();
        }
        if(textEmpty)
        {
            textElement.setSelectIndex(0);
        }

    }

    void GuiTextBox::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        //Important that we call the parent first. The other elements are called later
        GuiScrollBar::loadDataFromXML(attribs, manager);
        textElement.loadDataFromXML(attribs, manager);

        auto pair = attribs.get("allow-linebreaks");
        if(pair != nullptr)
            allowLineBreaks = (pair->second == "true");
        attribs.remove(pair);

        pair = attribs.get("default-text");
        if(pair != nullptr)
            defaultText = pair->second;
        attribs.remove(pair);

        textEmpty = false;
        oldFontColor = textElement.getFontColor();
        setTextEmpty(textElement.getText().empty());
    }

    SmartMemory<GuiItem> GuiTextBox::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiTextBox* ins = new GuiTextBox();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}