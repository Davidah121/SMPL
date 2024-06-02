#include "NewGui.h"
#include "Input.h"
#include "System.h"

namespace smpl
{
    const RootClass GuiTextBox::globalClass = CREATE_ROOT_CLASS(GuiTextBox, &GuiLayoutList::globalClass);
    const RootClass* GuiTextBox::getClass()
	{
		return &GuiTextBox::globalClass;
	}

    GuiTextBox::GuiTextBox() : GuiLayoutList()
    {
        setPadding({5, 5, 5, 5});
        setBorder({1, 1, 1, 1});
        setBackgroundColor({128, 128, 128, 255});
        addChild( SmartMemory<GuiItem>::createNoDelete(&textElement) );
        textElement.setSelectable(true);
    }

    GuiTextBox::~GuiTextBox()
    {

    }

    void GuiTextBox::addChild(SmartMemory<GuiItem> item)
    {
        if(children.size() <= 0)
        {
            GuiLayoutList::addChild(item);
        }
    }

    void GuiTextBox::removeChild(SmartMemory<GuiItem> item)
    {
        GuiLayoutList::removeChild(item);
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

    void GuiTextBox::keyboardInput()
    {
        std::queue<int> chars = Input::getCharactersTyped();
        std::vector<int> str;
        if(!textEmpty)
            str = StringTools::utf8ToIntString(textElement.getText());
        
        //make modifications to str then set once at the end
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
            }
            else if(v == Input::KEY_BACKSPACE)
            {
                //remove everything between s
                if(selectionStart < selectionEnd)
                {
                    std::vector<int> nText;
                    for(int i=0; i<str.size(); i++)
                    {
                        if(i < selectionStart || i > selectionEnd-1)
                            nText.push_back(str[i]);
                    }
                    str = nText;
                    selectionEnd = selectionStart;
                }
                else if(selectionStart == selectionEnd)
                {
                    std::vector<int> nText;
                    for(int i=0; i<str.size(); i++)
                    {
                        if(i != selectionStart-1)
                            nText.push_back(str[i]);
                    }
                    str = nText;
                    selectionStart--;
                    selectionEnd--;
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
                    std::vector<int> nText;
                    for(int i=0; i<str.size(); i++)
                    {
                        if(i < selectionStart || i > selectionEnd-1)
                            nText.push_back(str[i]);
                    }
                    str = nText;
                    selectionEnd = selectionStart;
                }
                else if(selectionStart == selectionEnd)
                {
                    //select index stays the same
                    std::vector<int> nText;
                    for(int i=0; i<str.size(); i++)
                    {
                        if(i != selectionStart)
                            nText.push_back(str[i]);
                    }
                    str = nText;
                }
            }
            else if(!Input::getKeyDown(Input::KEY_CONTROL))
            {
                if(v >= ' ' || (v == '\n' && allowLineBreaks))
                {
                    if(selectionStart <= selectionEnd)
                    {
                        //equivalent to a delete and insert
                        std::vector<int> nText;
                        for(int i=0; i<selectionStart; i++)
                        {
                            nText.push_back(str[i]);
                        }
                        nText.push_back(v);
                        for(int i=selectionEnd; i<str.size(); i++)
                        {
                            nText.push_back(str[i]);
                        }
                        selectionStart++;
                        selectionEnd = selectionStart;
                        str = nText;
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
                std::vector<int> nText;
                std::vector<int> removedText;
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
            }
        }
        else if(Input::getKeyDown(Input::KEY_CONTROL) && Input::getKeyPressed('V'))
        {
            //paste
            std::string pastedText = System::pasteFromClipboard();
            std::vector<int> pTextAsInts = StringTools::utf8ToIntString(pastedText);

            if(selectionStart <= selectionEnd)
            {
                // StringTools::println("%d", v);
                //equivalent to a delete and insert
                std::vector<int> nText;
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
            }
        }
        
        textEmpty = !(str.size() > 0);
        if(textEmpty)
        {
            textElement.setText(defaultText);
            selectionStart = 0;
            selectionEnd = 0;
        }
        else
        {
            std::string finalStr = StringTools::toUTF8String(str);
            textElement.setText(finalStr);
        }
        textElement.setSelectIndex(selectionStart, selectionEnd);
    }

    void GuiTextBox::onFocusChanged(SmartMemory<GuiManager> manager, bool changedTo)
    {
        if(changedTo)
        {
            //clear buffer of characters typed.
            Input::clearCharactersTyped();
            blink = true;
            counter = 30;
        }
        else
        {
            blink = false;
            counter = 30;
        }
        setShouldRender();
    }

    void GuiTextBox::update(SmartMemory<GuiManager> manager)
    {
        //Update the layout which will update the text element too.
        GuiLayoutList::update(manager);

        if(getFocused(manager) || textElement.getFocused(manager))
        {
            if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
            {
                int mouseX = manager.getPointer()->getMouseX();
                if(mouseX > textElement.getTrueX() + textElement.width)
                    textElement.setSelectIndex( -1 );
            }
            counter--;
            if(counter < 0)
            {
                blink = !blink;
                counter = 30;
                setShouldRender();
            }
            keyboardInput();
        }
    }

    void GuiTextBox::render(SmartMemory<GuiManager> manager)
    {
        //just draw the layout
        GuiLayoutList::render(manager);

        //draw line where at highlight end of text
        //The return rectangle is relative to the start position of the text
        GraphicsInterface::setColor(Color{0,0,0,255});
        GRect cRect = textElement.getCaretBox();
        if(blink)
        {
            int x1 = textElement.getTrueX() + cRect.left;
            int x2 = textElement.getTrueX() + cRect.right;
            int y1 = textElement.getTrueY() + cRect.top;
            int y2 = textElement.getTrueY() + cRect.bottom;
            
            GraphicsInterface::drawRect(x1, y1, x2, y2, false);
        }
    }

    void GuiTextBox::loadDataFromXML(SimpleHashMap<std::string, std::string>& attribs, SmartMemory<GuiManager> manager)
    {
        //Important that we call the parent first. The other elements are called later
        GuiLayoutList::loadDataFromXML(attribs, manager);
        textElement.loadDataFromXML(attribs, manager);

        auto pair = attribs.get("allow-linebreaks");
        if(pair != nullptr)
            allowLineBreaks = (pair->data == "true");
        attribs.remove(pair);

        pair = attribs.get("default-text");
        if(pair != nullptr)
            defaultText = pair->data;
        attribs.remove(pair);
    }

    SmartMemory<GuiItem> GuiTextBox::loadFunction(SimpleHashMap<std::string, std::string>& attributes, SmartMemory<GuiManager> manager)
    {
        GuiTextBox* ins = new GuiTextBox();
        ins->loadDataFromXML(attributes, manager);
        return SmartMemory<GuiItem>(ins);
    }
}