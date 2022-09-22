#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_TEXTBOX_CLASS

	const Class GuiTextBox::globalClass = Class("GuiTextBox", {&GuiInstance::globalClass});

	GuiTextBox::GuiTextBox(int x, int y, int width, int height) : GuiInstance()
	{
		setClass(globalClass);
		setBaseX(x);
		setBaseY(y);
		this->width = width;
		this->height = height;
		onEnterPressedFunction = nullptr;
		onKeyPressFunction = nullptr;

		textElement = GuiTextBlock(0, 0, width, height);
		textElement.setShouldHighlightText(true);
		addChild( &textElement );
		boundingBox = Box2D(x, y, x+width, y+height);
	}

	GuiTextBox::GuiTextBox(const GuiTextBox& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiTextBox::operator=(const GuiTextBox& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiTextBox::copy(const GuiTextBox& other)
	{
		setClass(globalClass);
		onEnterPressedFunction = other.onEnterPressedFunction;
		onKeyPressFunction = other.onKeyPressFunction;

		width = other.width;
		height = other.height;

		backgroundColor = other.backgroundColor;
		outlineColor = other.outlineColor;
		focusOutlineColor = other.focusOutlineColor;

		textElement = other.textElement;

		removeChild((GuiInstance*)&other.textElement);
		addChild( &textElement );
	}

	GuiTextBox::~GuiTextBox()
	{
	}

	void GuiTextBox::setOnEnterPressedFunction(std::function<void(GuiInstance*)> func)
	{
		onEnterPressedFunction = func;
	}

	void GuiTextBox::setOnKeyPressFunction(std::function<void(GuiInstance*)> func)
	{
		onKeyPressFunction = func;
	}

	void GuiTextBox::setBackgroundColor(Color c)
	{
		backgroundColor = c;
		setShouldRedraw(true);
	}

	void GuiTextBox::setOutlineColor(Color c)
	{
		outlineColor = c;
		setShouldRedraw(true);
	}

	void GuiTextBox::setFocusOutlineColor(Color c)
	{
		focusOutlineColor = c;
		setShouldRedraw(true);
	}

	void GuiTextBox::setCursorBlinkColor(Color c)
	{
		cursorBlinkColor = c;
		setShouldRedraw(true);
	}

	void GuiTextBox::setCursorBlinkTimer(int timeInFrames)
	{
		cursorBlinkMaxTime = timeInFrames;
	}

	void GuiTextBox::setCursorWidth(int w)
	{
		cursorWidth = w;
		setShouldRedraw(true);
	}

	void GuiTextBox::render()
	{
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
		
		//draw a rectangle
		graphicsInterface->setColor(backgroundColor);
		graphicsInterface->drawRect(x, y, x + width, y + height, false);

		if (getFocus() == false)
			graphicsInterface->setColor(outlineColor);
		else
			graphicsInterface->setColor(focusOutlineColor);
		
		graphicsInterface->drawRect(x, y, x + width, y + height, true);

		if(getFocus())
		{
			if(cursorBlink)
			{
				//Font Stuff
				graphicsInterface->setColor(cursorBlinkColor);
				std::string testText = textElement.getTextRef();
				GuiFontInterface* fInt = (textElement.getFont() != nullptr) ? textElement.getFont() : graphicsInterface->getFont();
				Font* f = fInt->getFont();

				Vec2f cursorPos = f->getCursorLocation( testText, cursorLocation, -1, textElement.getMaxHeight() );

				cursorPos.x += textElement.getBaseX() + xOffsetForText;
				cursorPos.y += textElement.getBaseY() + yOffsetForText;
				
				graphicsInterface->drawRect(x+cursorPos.x, y+cursorPos.y, x+cursorPos.x+cursorWidth, y+cursorPos.y+f->getFontSize(), false);
			}
		}

	}

	void GuiTextBox::keyInput()
	{
		if (getFocus())
		{
			if (Input::getKeyChanged() && Input::getKeyPressed(Input::KEY_ENTER)!=true )
			{
				//General Key
				if (onKeyPressFunction != nullptr)
				{
					onKeyPressFunction(this);
				}
			}

			if(Input::getKeyDown(Input::KEY_CONTROL))
			{
				if( Input::getKeyPressed('V') )
				{
					//PASTE
					std::string pasteText = System::pasteFromClipboard();

					int minSelect = MathExt::min(selectStart, selectEnd);
					int maxSelect = MathExt::max(selectStart, selectEnd);
					
					std::string prefix = textElement.getText().substr(0, minSelect);
					std::string suffix = textElement.getText().substr(maxSelect);

					textElement.setText( (prefix + pasteText) + suffix );
					cursorLocation = minSelect+pasteText.size();
					selectStart = cursorLocation;
					selectEnd = cursorLocation;
				}
				else if( Input::getKeyPressed('C') )
				{
					//COPY
					int minSelect = MathExt::min(selectStart, selectEnd);
					int maxSelect = MathExt::max(selectStart, selectEnd);
					std::string cpyText = textElement.getText().substr(minSelect, maxSelect);

					System::copyToClipboard(cpyText);
				}
				else if( Input::getKeyPressed('X') )
				{
					//CUT
					int minSelect = MathExt::min(selectStart, selectEnd);
					int maxSelect = MathExt::max(selectStart, selectEnd);
					std::string cpyText = textElement.getText().substr(minSelect, maxSelect);

					System::copyToClipboard(cpyText);

					std::string prefix = textElement.getText().substr(0, minSelect);
					std::string suffix = textElement.getText().substr(maxSelect);

					textElement.setText(prefix+suffix);
					cursorLocation = minSelect;
					selectStart = cursorLocation;
					selectEnd = cursorLocation;
				}
			}

			std::queue<int> charBuffer = Input::getCharactersTyped();
			
			while(charBuffer.size() > 0)
			{
				int inputChar = charBuffer.front();
				charBuffer.pop();

				if (inputChar == Input::KEY_ENTER)
				{
					//Enter key
					if (onEnterPressedFunction != nullptr)
					{
						onEnterPressedFunction(this);
					}
					if(getAllowLineBreaks())
					{
						int value = '\n';

						int minSelect = MathExt::min(selectStart, selectEnd);
						int maxSelect = MathExt::max(selectStart, selectEnd);
						
						std::string prefix = textElement.getText().substr(0, minSelect);
						std::string suffix = textElement.getText().substr(maxSelect);

						textElement.setText( (prefix + (char)value) + suffix );
						cursorLocation = minSelect+1;
						selectStart = cursorLocation;
						selectEnd = cursorLocation;
					}
				}
				else if (inputChar == Input::KEY_LEFT)
				{
					//Move cursor back
					cursorLocation = MathExt::clamp(cursorLocation-1, 0, (int)textElement.getTextRef().size());
					selectStart = cursorLocation;
					selectEnd = cursorLocation;
				}
				else if (inputChar == Input::KEY_RIGHT)
				{
					//Move cursor forward
					cursorLocation = MathExt::clamp(cursorLocation+1, 0, (int)textElement.getTextRef().size());
					selectStart = cursorLocation;
					selectEnd = cursorLocation;
				}
				else if(inputChar == Input::KEY_UP)
				{
					//TODO - CHANGE TO BE BASED AROUND BOUNDING BOX
					//Find distance from linebreak. Move to that distance on line above
					int currLineStartLoc = 0;
					int previousLineStartLoc = 0;
					std::string temp = textElement.getText();

					for(int i=cursorLocation-1; i>0; i--)
					{
						if(temp[i] == '\n')
						{
							if(currLineStartLoc == 0)
								currLineStartLoc = i+1;
							else
							{
								previousLineStartLoc = i+1;
								break;
							}
						}
					}

					cursorLocation = previousLineStartLoc + MathExt::clamp(cursorLocation-currLineStartLoc, 0, (currLineStartLoc-previousLineStartLoc-1));
					cursorLocation = MathExt::clamp(cursorLocation, 0, (int)textElement.getTextRef().size());
					selectStart = cursorLocation;
					selectEnd = cursorLocation;
				}
				else if(inputChar == Input::KEY_DOWN)
				{
					//TODO - CHANGE TO BE BASED AROUND BOUNDING BOX
					//Find distance from linebreak. Move to that distance on line below
					std::string temp = textElement.getText();
					int nextLineStartLoc = temp.size();
					int nextLineEndLoc = temp.size();
					int previousLineStartLoc = 0;

					for(int i=cursorLocation-1; i>0; i--)
					{
						if(temp[i] == '\n')
						{
							previousLineStartLoc = i+1;
							break;
						}
					}
					for(int i=cursorLocation; i<temp.size(); i++)
					{
						if(temp[i] == '\n')
						{
							if(nextLineStartLoc == temp.size())
								nextLineStartLoc = i+1;
							else
							{
								nextLineEndLoc = i;
								break;
							}
						}
					}
					
					cursorLocation = nextLineStartLoc + MathExt::clamp(cursorLocation-previousLineStartLoc, 0, nextLineEndLoc-nextLineStartLoc);
					cursorLocation = MathExt::clamp(cursorLocation, 0, (int)textElement.getTextRef().size());
					selectStart = cursorLocation;
					selectEnd = cursorLocation;
				}
				else if( inputChar == Input::KEY_BACKSPACE)
				{
					if(textElement.getTextRef().size() > 0)
					{
						int minSelect = MathExt::min(selectStart, selectEnd);
						int maxSelect = MathExt::max(selectStart, selectEnd);
						if(minSelect==maxSelect)
							minSelect--;
						
						if(minSelect >= 0)
						{
							std::string prefix = textElement.getText().substr(0, minSelect);
							std::string suffix = textElement.getText().substr(maxSelect);

							textElement.setText(prefix+suffix);

							cursorLocation = minSelect;
							selectStart = cursorLocation;
							selectEnd = cursorLocation;
						}
					}
				}
				else if( inputChar == Input::KEY_DELETE)
				{
					if(textElement.getTextRef().size() > 0)
					{
						int minSelect = MathExt::min(selectStart, selectEnd);
						int maxSelect = MathExt::max(selectStart, selectEnd);
						if(minSelect==maxSelect)
							maxSelect++;

						if(maxSelect <= textElement.getTextRef().size())
						{
							std::string prefix = textElement.getText().substr(0, minSelect);
							std::string suffix = textElement.getText().substr(maxSelect);

							textElement.setText(prefix+suffix);

							cursorLocation = minSelect;
							selectStart = cursorLocation;
							selectEnd = cursorLocation;
						}
					}
				}
				else
				{
					if(inputChar >= ' ')
					{
						std::vector<unsigned char> values = StringTools::toUTF8(inputChar);

						int minSelect = MathExt::min(selectStart, selectEnd);
						int maxSelect = MathExt::max(selectStart, selectEnd);
						
						std::string prefix = textElement.getText().substr(0, minSelect);
						std::string suffix = textElement.getText().substr(maxSelect);

						for(int i=0; i<values.size(); i++)
						{
							prefix += values[i];
						}
						textElement.setText( prefix + suffix );
						cursorLocation = minSelect+1;
						selectStart = cursorLocation;
						selectEnd = cursorLocation;
					}
				}
			}
		}
	}

	void GuiTextBox::mouseInput()
	{
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
		GuiFontInterface* fInt = (textElement.getFont() != nullptr) ? textElement.getFont() : graphicsInterface->getFont();
		Font* f = fInt->getFont();
		std::string temp = textElement.getText();

		int mouseX;
		int mouseY;
		
		if(getManager()!=nullptr)
		{
			mouseX = getManager()->getMouseX();
			mouseY = getManager()->getMouseY();
		}
		
		if (Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
		{
			selectEnd = 0;
			selectStart = 0;
			cursorLocation = 0;

			if (mouseX >= x && mouseX <= x + width)
			{
				if (mouseY >= y && mouseY <= y + height)
				{
					if(!getFocus())
						setShouldRedraw(true);
					
					Input::clearCharactersTyped();
					setFocus(true);
					hold = true;
					bool found = false;
					//find start location
					int xStartLoc = x + textElement.getBaseX();
					int wid = 0;
					int preXAdv = 0;
					int yStartLoc = y + textElement.getBaseY();

					if(mouseY < yStartLoc)
					{
						cursorLocation = 0;
						selectStart = 0;
						selectEnd = 0;
					}
					else
					{
						for(int i=0; i<temp.size(); i++)
						{
							bool passedMaxWidth = false;
							if(textElement.getAllowTextWrap())
							{
								passedMaxWidth = (wid >= textElement.getMaxWidth());
							}

							if(temp[i] == '\n' || passedMaxWidth)
							{
								if(mouseY >= yStartLoc && mouseY < yStartLoc+f->getVerticalAdvance())
								{
									int xS = xStartLoc - (int)MathExt::floor((double)preXAdv/2);
									if(mouseX >= xS)
									{
										cursorLocation = i;
										selectStart = i;
										selectEnd = i;
										found = true;
										break;
									}
								}
								xStartLoc = x + textElement.getBaseX();
								wid = 0;
								preXAdv = 0;
								yStartLoc += f->getVerticalAdvance();
							}
							else
							{
								int charIndexInFont = f->getCharIndex(temp[i]);
								FontCharInfo fci = f->getFontCharInfo(charIndexInFont);
								int xS = xStartLoc - (int)MathExt::floor((double)preXAdv/2);
								int xE = xStartLoc + (int)MathExt::ceil((double)fci.horizAdv/2);
								if(mouseY >= yStartLoc && mouseY < yStartLoc+f->getVerticalAdvance())
								{
									if(mouseX >= xS && mouseX < xE)
									{
										cursorLocation = i;
										selectStart = i;
										selectEnd = i;
										found = true;
										break;
									}
								}
								xStartLoc += fci.horizAdv;
								wid += fci.horizAdv;
								preXAdv = fci.horizAdv;
							}
						}

						if(!found)
						{
							if(mouseY >= yStartLoc)
							{
								cursorLocation = temp.size();
								selectStart = cursorLocation;
								selectEnd = cursorLocation;
							}
						}
					}

				}
				else
				{
					if(getFocus())
						setShouldRedraw(true);
					setFocus(false);
				}
			}
			else
			{
				if(getFocus())
					setShouldRedraw(true);
				setFocus(false);
			}
		}

		if (Input::getMouseUp(Input::LEFT_MOUSE_BUTTON))
		{
			hold = false;
		}

		if(hold)
		{
			//find start location
			bool found = false;
			int xStartLoc = x + textElement.getBaseX();
			int wid = 0;
			int preXAdv = 0;
			int yStartLoc = y + textElement.getBaseY();

			if(mouseY < yStartLoc)
			{
				cursorLocation = 0;
				selectEnd = 0;
			}
			else
			{
				for(int i=0; i<temp.size(); i++)
				{
					bool passedMaxWidth = false;
					if(textElement.getAllowTextWrap())
					{
						passedMaxWidth = (wid >= textElement.getMaxWidth());
					}

					if(temp[i] == '\n' || passedMaxWidth)
					{
						if(mouseY >= yStartLoc && mouseY < yStartLoc+f->getVerticalAdvance())
						{
							int xS = xStartLoc - (int)MathExt::floor((double)preXAdv/2);
							if(mouseX >= xS)
							{
								cursorLocation = i;
								selectEnd = i;
								found = true;
								break;
							}
						}
						xStartLoc = x + textElement.getBaseX();
						wid = 0;
						preXAdv = 0;
						yStartLoc += f->getVerticalAdvance();
					}
					else
					{
						int charIndexInFont = f->getCharIndex(temp[i]);
						FontCharInfo fci = f->getFontCharInfo(charIndexInFont);
						int xS = xStartLoc - (int)MathExt::floor((double)preXAdv/2);
						int xE = xStartLoc + (int)MathExt::ceil((double)fci.horizAdv/2);
						if(mouseY >= yStartLoc && mouseY < yStartLoc+f->getVerticalAdvance())
						{
							if(mouseX >= xS && mouseX < xE)
							{
								cursorLocation = i;
								selectEnd = i;
								found = true;
								break;
							}
						}
						xStartLoc += fci.horizAdv;
						wid += fci.horizAdv;
						preXAdv = fci.horizAdv;
					}
				}

				if(!found)
				{
					if(mouseY >= yStartLoc)
					{
						cursorLocation = temp.size();
						selectEnd = cursorLocation;
					}
				}
			}
		}
	}

	void GuiTextBox::selectionCleanup()
	{
		GuiGraphicsInterface* graphicsInterface = this->getManager()->getGraphicsInterface();
		GuiFontInterface* fInt = (textElement.getFont() != nullptr) ? textElement.getFont() : graphicsInterface->getFont();
		std::string tempText = textElement.getText();
		Font* f = fInt->getFont();
		
		textElement.setHighlightStart(selectStart);
		textElement.setHighlightEnd(selectEnd);

		if(startStringIndex>=cursorLocation)
		{
			startStringIndex = cursorLocation-1;
		}

		if(startStringIndex<0)
			startStringIndex = 0;
		
		int xCursorPos, yCursorPos;
		xCursorPos = 0;
		yCursorPos = 0;
		for(int i=0; i<cursorLocation; i++)
		{
			if(tempText[i] == '\n')
			{
				xCursorPos = 0;
				yCursorPos += f->getVerticalAdvance();
			}
			else
			{
				int charIndexInFont = f->getCharIndex(tempText[i]);
				FontCharInfo fci = f->getFontCharInfo(charIndexInFont);
				xCursorPos += fci.horizAdv;
			}

			if(textElement.getAllowTextWrap())
			{
				if(xCursorPos >= textElement.getMaxWidth())
				{
					xCursorPos = 0;
					yCursorPos += f->getVerticalAdvance();
				}
			}
		}

		xCursorPos += textElement.getBaseX()*2;
		yCursorPos += textElement.getBaseY()*2;

		xOffsetForText = MathExt::min(width - xCursorPos, 0);
		yOffsetForText = MathExt::min(height - yCursorPos, 0);

		textElement.setOffsetX(xOffsetForText);
		textElement.setOffsetY(yOffsetForText);
	}

	void GuiTextBox::update()
	{
		int prevCursorLoc = cursorLocation;
		selectEnd = MathExt::clamp(selectEnd, 0, (int)textElement.getTextRef().size());
		selectStart = MathExt::clamp(selectStart, 0, (int)textElement.getTextRef().size());
		cursorLocation = MathExt::clamp(cursorLocation, 0, (int)textElement.getTextRef().size());
		startStringIndex = MathExt::clamp(startStringIndex, 0, (int)textElement.getTextRef().size());
		
		bool previousFocus = getFocus();

		keyInput();
		mouseInput();
		selectionCleanup();

		if(getFocus())
		{
			cursorBlinkTimer++;
			if(cursorBlinkTimer>=cursorBlinkMaxTime)
			{
				cursorBlinkTimer = 0;
				cursorBlink = !cursorBlink;
				setShouldRedraw(true);
			}
		}
		else
		{
			if(previousFocus != getFocus() || cursorBlink)
			{
				setShouldRedraw(true);
				cursorBlink = false;
			}
		}

		if(prevCursorLoc != cursorLocation)
			setShouldRedraw(true);
		
		boundingBox = Box2D(x, y, x+width, y+height);
	}

	GuiTextBlock* GuiTextBox::getTextBlockElement()
	{
		return &textElement;
	}

	void GuiTextBox::setWidth(int v)
	{
		width = v;
		setShouldRedraw(true);
	}
	void GuiTextBox::setHeight(int v)
	{
		height = v;
		setShouldRedraw(true);
	}
	int GuiTextBox::getWidth()
	{
		return width;
	}
	int GuiTextBox::getHeight()
	{
		return height;
	}

	void GuiTextBox::setAllowLineBreaks(bool v)
	{
		allowLineBreaks = v;
	}
	bool GuiTextBox::getAllowLineBreaks()
	{
		return allowLineBreaks;
	}

	void GuiTextBox::solveBoundingBox()
	{
		boundingBox = Box2D(x, y, x+width, y+height);
	}

	void GuiTextBox::loadDataFromXML(std::unordered_map<std::string, std::string>& attributes, GuiGraphicsInterface* inter)
	{
		GuiInstance::loadDataFromXML(attributes, inter);

		std::vector<std::string> possibleNames = { "width", "height", "cursorblinktimer", "cursorwidth", "backgroundcolor", "outlinecolor", "focusoutlinecolor", "cursorblinkcolor", "allowlinebreaks"};

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attributes.find(possibleNames[i]);
			if(it != attributes.end())
			{
				if(possibleNames[i] == "width")
				{
					this->width = std::abs(StringTools::toInt(it->second));
				}
				else if(possibleNames[i] == "height")
				{
					this->height = std::abs(StringTools::toInt(it->second));
				}
				else if(possibleNames[i] == "cursorblinktimer")
				{
					this->setCursorBlinkTimer(StringTools::toInt(it->second));
				}
				else if(possibleNames[i] == "cursorwidth")
				{
					this->cursorWidth = std::abs(StringTools::toInt(it->second));
				}
				else if(possibleNames[i] == "backgroundcolor")
				{
					//define as color name or rgba
					this->backgroundColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == "outlinecolor")
				{
					//define as color name or rgba
					this->outlineColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == "focusoutlinecolor")
				{
					//define as color name or rgba
					this->focusOutlineColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == "cursorblinkcolor")
				{
					//define as color name or rgba
					this->cursorBlinkColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == "allowlinebreaks")
				{
					this->allowLineBreaks = StringTools::equalsIgnoreCase<char>(it->second, "true");
				}

				attributes.erase(possibleNames[i]);
			}
		}
		
		boundingBox = Box2D(x, y, x+width, y+height);

		textElement.loadDataFromXML(attributes, inter);
		
		if(textElement.getMaxWidth() == 0)
			textElement.setMaxWidth(width - textElement.getBaseX()*2);

		if(textElement.getMaxHeight() == 0)
			textElement.setMaxHeight(height - textElement.getBaseY()*2);
	}

	GuiInstance* GuiTextBox::loadFunction(std::unordered_map<std::string, std::string>& attributes, GuiGraphicsInterface* inter)
	{
		GuiTextBox* ins = new GuiTextBox(0, 0, 0, 0);
		ins->loadDataFromXML(attributes, inter);

		return ins;
	}

	void GuiTextBox::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiTextBox", GuiTextBox::loadFunction);
	}

	#pragma endregion

} //NAMESPACE glib END