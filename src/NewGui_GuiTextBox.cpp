#include "NewGui.h"
#include "Input.h"
#include "System.h"

namespace smpl
{
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

        while(!chars.empty())
        {
            int v = chars.front();
            chars.pop();

            if(v == Input::KEY_LEFT)
                textElement.increaseSelectIndex();
            else if(v == Input::KEY_RIGHT)
                textElement.decreaseSelectIndex();
            else if(v == Input::KEY_BACKSPACE)
            {
                int sIndex = textElement.getHighlightStartIndex();
                int eIndex = textElement.getHighlightEndIndex();
                
                if(sIndex >= 0 && eIndex >= 0 && sIndex < eIndex)
                {
                    std::vector<int> nText;
                    for(int i=0; i<nText.size(); i++)
                    {
                        if(i < sIndex && i > eIndex)
                            nText.push_back(str[i]);
                    }
                    str = nText;

                    std::string finalStr = StringTools::toUTF8String(nText);
                    textElement.setText(finalStr);
                    textElement.setSelectIndex(sIndex);
                }
                else if(sIndex == eIndex)
                {
                    std::vector<int> nText;
                    for(int i=0; i<nText.size(); i++)
                    {
                        if(i != sIndex-1)
                            nText.push_back(str[i]);
                    }
                    str = nText;

                    std::string finalStr = StringTools::toUTF8String(nText);
                    textElement.setText(finalStr);
                    textElement.decreaseSelectIndex();
                }
            }
            else if(v == Input::KEY_DELETE)
            {
                int sIndex = textElement.getHighlightStartIndex();
                int eIndex = textElement.getHighlightEndIndex();
                
                if(sIndex >= 0 && eIndex >= 0 && sIndex < eIndex)
                {
                    std::vector<int> nText;
                    for(int i=0; i<nText.size(); i++)
                    {
                        if(i < sIndex && i > eIndex)
                            nText.push_back(str[i]);
                    }
                    str = nText;

                    std::string finalStr = StringTools::toUTF8String(nText);
                    textElement.setText(finalStr);
                    textElement.setSelectIndex(sIndex);
                }
                else if(sIndex == eIndex)
                {
                    //select index stays the same
                    std::vector<int> nText;
                    for(int i=0; i<nText.size(); i++)
                    {
                        if(i != sIndex)
                            nText.push_back(str[i]);
                    }
                    str = nText;

                    std::string finalStr = StringTools::toUTF8String(nText);
                    textElement.setText(finalStr);
                }
            }
            else if(!Input::getKeyDown(Input::KEY_CONTROL))
            {
                int sIndex = textElement.getHighlightStartIndex();
                int eIndex = textElement.getHighlightEndIndex();
                
                if(sIndex >= 0 && eIndex >= 0 && sIndex <= eIndex)
                {
                    std::vector<int> nText;
                    for(int i=0; i<sIndex; i++)
                    {
                        nText.push_back(str[i]);
                    }
                    nText.push_back(v);

                    for(int i=eIndex; i<str.size(); i++)
                    {
                        nText.push_back(str[i]);
                    }
                    str = nText;

                    std::string finalStr = StringTools::toUTF8String(nText);
                    textElement.setText(finalStr);
                    textElement.increaseSelectIndex();
                }
            }
        }

        //copy is already handled in the text element
        if(Input::getKeyDown(Input::KEY_CONTROL) && Input::getKeyPressed('X'))
        {
            //cut
            int sIndex = textElement.getHighlightStartIndex();
            int eIndex = textElement.getHighlightEndIndex();
            
            if(sIndex >= 0 && eIndex >= 0 && sIndex < eIndex)
            {
                std::vector<int> nText;
                std::vector<int> removedText;
                for(int i=0; i<nText.size(); i++)
                {
                    if(i < sIndex && i > eIndex)
                        nText.push_back(str[i]);
                    else
                        removedText.push_back(str[i]);
                }
                str = nText;

                std::string finalStr = StringTools::toUTF8String(nText);
                textElement.setText(finalStr);
                textElement.setSelectIndex(sIndex);

                std::string copiedText = StringTools::toUTF8String(removedText);
                System::copyToClipboard(copiedText);
            }
        }
        else if(Input::getKeyDown(Input::KEY_CONTROL) && Input::getKeyPressed('V'))
        {
            //paste
            std::string pastedText = System::pasteFromClipboard();
            std::vector<int> pTextAsInts = StringTools::utf8ToIntString(pastedText);

            int sIndex = textElement.getHighlightStartIndex();
            int eIndex = textElement.getHighlightEndIndex();
            
            if(sIndex >= 0 && eIndex >= 0 && sIndex <= eIndex)
            {
                std::vector<int> nText;
                for(int i=0; i<sIndex; i++)
                {
                    nText.push_back(str[i]);
                }

                for(int i=0; i<pTextAsInts.size(); i++)
                {
                    nText.push_back(pTextAsInts[i]);
                }

                for(int i=eIndex; i<str.size(); i++)
                {
                    nText.push_back(str[i]);
                }
                str = nText;

                std::string finalStr = StringTools::toUTF8String(nText);
                textElement.setText(finalStr);
                textElement.increaseSelectIndex();
            }
        }
        
        textEmpty = (str.size() > 0);
        if(textEmpty)
            textElement.setText(defaultText);
    }

    void GuiTextBox::update(SmartMemory<GuiManager> manager)
    {
        //get mouseX and mouseY
        GuiManager* rawGPointer = manager.getPointer();
        int mouseX = 0;
        int mouseY = 0;
        
        if(rawGPointer != nullptr)
        {
            mouseX = rawGPointer->getMouseX();
            mouseY = rawGPointer->getMouseY();

            //check if in focus before doing anything.
            if(rawGPointer->getObjectInFocus( SmartMemory<GuiItem>::createNoDelete(this) ))
            {
                keyboardInput();
            }

            if(isColliding(mouseX, mouseY))
            {
                if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
                {
                    rawGPointer->setObjectInFocus( SmartMemory<GuiItem>::createNoDelete(this) );
                    //clear buffer of characters typed.
                    Input::clearCharactersTyped();
                }
            }
        }

        //Update the layout which will update the text element too.
        GuiLayoutList::update(manager);
    }

    void GuiTextBox::render(SmartMemory<GuiManager> manager)
    {
        //just draw the layout
        GuiLayoutList::render(manager);

        //draw line where at highlight end of text
        //The return rectangle is relative to the start position of the text
        GraphicsInterface::setColor(Color{0,0,0,255});
        GRect cRect = textElement.getCaretBox();
        int x1 = textElement.getTrueX() + cRect.left;
        int x2 = textElement.getTrueX() + cRect.right;
        int y1 = textElement.getTrueY() + cRect.top;
        int y2 = textElement.getTrueY() + cRect.bottom;
        
        GraphicsInterface::drawRect(x1, y1, x2, y2, false);
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