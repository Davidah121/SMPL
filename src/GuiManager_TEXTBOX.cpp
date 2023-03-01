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
		boundingBox = Box2D(x, y, x+width, y+height);

		initContextMenu();
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
		cursorBlinkColor = other.cursorBlinkColor;

		textElement = other.textElement;

		allowLineBreaks = other.allowLineBreaks;
		cursorBlinkMaxTime = other.cursorBlinkMaxTime;
		cursorWidth = other.cursorWidth;
		contextMenuEnabled = other.contextMenuEnabled;

		removeChild((GuiInstance*)&other.contextMenu);

		//recreate context menu
		initContextMenu();
	}

	GuiTextBox::~GuiTextBox()
	{
		dispose();
	}

	void GuiTextBox::dispose()
	{
		//delete contextMenuStuff
		for(size_t i=0; i<contextMenuButtons.size(); i++)
		{
			if(contextMenuText[i] != nullptr)
				delete contextMenuText[i];
			if(contextMenuButtons[i] != nullptr)
				delete contextMenuButtons[i];
		}

		contextMenuText.clear();
		contextMenuButtons.clear();
		removeChild(&contextMenu);
	}

	void GuiTextBox::initContextMenu()
	{
		//Delete old context menu data.
		dispose();

		// Adjust so that the text is always the same size regardless of scale.
		// Text Size of 12pt or something.
		contextMenuButtons.push_back( new GuiRectangleButton(0, 0, 96, 24) );
		contextMenuButtons.push_back( new GuiRectangleButton(0, 0, 96, 24) );
		contextMenuButtons.push_back( new GuiRectangleButton(0, 0, 96, 24) );
		
		contextMenuText.push_back( new GuiTextBlock(0, 0, 96, 24) );
		contextMenuText.push_back( new GuiTextBlock(0, 0, 96, 24) );
		contextMenuText.push_back( new GuiTextBlock(0, 0, 96, 24) );

		contextMenuButtons[0]->setOnClickReleaseFunction([this](GuiInstance* ins) -> void{
			this->copy();
		});
		contextMenuButtons[1]->setOnClickReleaseFunction([this](GuiInstance* ins) -> void{
			this->paste();
		});
		contextMenuButtons[2]->setOnClickReleaseFunction([this](GuiInstance* ins) -> void{
			this->cut();
		});
		contextMenuText[0]->setText("Copy");
		contextMenuText[1]->setText("Paste");
		contextMenuText[2]->setText("Cut");

		contextMenuButtons[0]->addChild(contextMenuText[0]);
		contextMenuButtons[1]->addChild(contextMenuText[1]);
		contextMenuButtons[2]->addChild(contextMenuText[2]);
		
		contextMenu.addChild(contextMenuButtons[0]);
		contextMenu.addChild(contextMenuButtons[1]);
		contextMenu.addChild(contextMenuButtons[2]);
		addChild(&contextMenu);
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
		//draw a rectangle
		GuiGraphicsInterface::setColor(backgroundColor);
		GuiGraphicsInterface::drawRect(x, y, x + width, y + height, false);

		if (getFocus() == false)
			GuiGraphicsInterface::setColor(outlineColor);
		else
			GuiGraphicsInterface::setColor(focusOutlineColor);
		
		GuiGraphicsInterface::drawRect(x, y, x + width, y + height, true);

		if(getFocus())
		{
			if(cursorBlink)
			{
				//Font Stuff
				GuiGraphicsInterface::setColor(cursorBlinkColor);
				std::string testText = textElement.getTextRef();

				GuiFontInterface* fInt = textElement.getFont();
				Font* f = fInt->getFont();

				if(f == nullptr)
					return;
				
				Vec2f cursorPos;

				if(textElement.getAllowTextWrap())
					cursorPos = f->getCursorLocation( testText, cursorLocation, textElement.getMaxWidth() );
				else
					cursorPos = f->getCursorLocation( testText, cursorLocation, -1 );

				cursorPos.x += xOffsetForText;
				cursorPos.y += yOffsetForText;
				
				int tx = textElement.getX();
				int ty = textElement.getY();
				
				GuiGraphicsInterface::drawRect((int)MathExt::round(tx+cursorPos.x), (int)MathExt::round(ty+cursorPos.y),
											(int)MathExt::round(tx+cursorPos.x+cursorWidth), (int)MathExt::round(ty+cursorPos.y+f->getVerticalAdvance()), false);
			}
		}
		
		//Create new bounding box to put the text in. Must be bound by the textbox size regardless of the total size of the collection of objects.
		//Collection of objects refering to itself and its children
		Box2D oldBounds = GuiGraphicsInterface::getClippingRect();
		Box2D nBounds = oldBounds;
		nBounds.setLeftBound( MathExt::max((int)nBounds.getLeftBound(), x) );
		nBounds.setTopBound( MathExt::max((int)nBounds.getTopBound(), y) );

		GuiGraphicsInterface::setClippingRect(nBounds);

		textElement.baseRender();
		textElement.render();

		GuiGraphicsInterface::setClippingRect(oldBounds);
	}

	void GuiTextBox::copy()
	{
		int minSelect = MathExt::min(selectStart, selectEnd);
		int maxSelect = MathExt::max(selectStart, selectEnd);
		std::string cpyText = textElement.getText().substr(minSelect, maxSelect);

		System::copyToClipboard(cpyText);
	}

	void GuiTextBox::cut()
	{
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

	void GuiTextBox::paste()
	{
		//Do some additional parsing of the pasted text to remove linebreaks and other invalid characters
		std::string pasteText = System::pasteFromClipboard();
		std::string cleanedText = "";
		for(int i=0; i<pasteText.size(); i++)
		{
			if(pasteText[i] >= 32)
			{
				cleanedText += pasteText[i];
			}
			else if(pasteText[i] == '\n' && allowLineBreaks)
			{
				cleanedText += '\n';
			}
			//should allow tabs too whenever a method to handle them is implemented.
		}
		

		int minSelect = MathExt::min(selectStart, selectEnd);
		int maxSelect = MathExt::max(selectStart, selectEnd);
		
		std::string prefix = textElement.getText().substr(0, minSelect);
		std::string suffix = textElement.getText().substr(maxSelect);

		textElement.setText( (prefix + cleanedText) + suffix );
		cursorLocation = minSelect+cleanedText.size();
		selectStart = cursorLocation;
		selectEnd = cursorLocation;
	}

	std::string GuiTextBox::getSelectedText()
	{
		int minSelect = MathExt::min(selectStart, selectEnd);
		int maxSelect = MathExt::max(selectStart, selectEnd);
		std::string cpyText = textElement.getText().substr(minSelect, maxSelect);

		return cpyText;
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
					paste();
				}
				else if( Input::getKeyPressed('C') )
				{
					//COPY
					copy();
				}
				else if( Input::getKeyPressed('X') )
				{
					//CUT
					cut();
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

					StringTools::println("CursorLocation: %d", cursorLocation);
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
					for(int i=cursorLocation; i<(int)temp.size(); i++)
					{
						if(temp[i] == '\n')
						{
							if(nextLineStartLoc == (int)temp.size())
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

						if(maxSelect <= (int)textElement.getTextRef().size())
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

						for(size_t i=0; i<values.size(); i++)
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
		GuiFontInterface* fInt = textElement.getFont();
		Font* f = fInt->getFont();
		std::string temp = textElement.getText();

		int mouseX;
		int mouseY;
		
		if(getManager()!=nullptr)
		{
			mouseX = getManager()->getMouseX();
			mouseY = getManager()->getMouseY();
		}

		//check if there is a context menu open. If there is, skip. There may not be any context menus
		GuiContextMenuHelper* helper = GuiContextMenuHelper::getContextMenuHelper(getManager());
		if(helper != nullptr)
		{
			//Assume that if visible, there is a context menu open
			if(helper->isMenuShowing())
			{
				if(Input::getMousePressed(Input::LEFT_MOUSE_BUTTON))
				{
					//Continue only if the context menu showed is not owned by this object
					for(size_t i=0; i<getChildrenRef().size(); i++)
					{
						GuiInstance* childIns = getChildrenRef()[i];
						if(childIns->getClass() == GuiContextMenu::globalClass)
						{
							if(((GuiContextMenu*)childIns)->isMenuShowing())
								return;
						}
					}
				}
				else
				{
					return;
				}
			}
		}
		
		if(Input::getMousePressed(Input::RIGHT_MOUSE_BUTTON) || Input::getMousePressed(Input::MIDDLE_MOUSE_BUTTON))
		{
			bool good = false;
			if (mouseX >= x && mouseX <= x + width)
			{
				if (mouseY >= y && mouseY <= y + height)
				{
					good = true;
				}
			}

			if(!good)
			{
				selectEnd = 0;
				selectStart = 0;
				cursorLocation = 0;
				
				if(getFocus())
					setShouldRedraw(true);
				setFocus(false);
			}
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
					bool firstFocus = false;
					if(!getFocus())
					{
						firstFocus = true;
						setShouldRedraw(true);
					}
					
					Input::clearCharactersTyped();
					setFocus(true);
					hold = true;
					
					if(firstFocus)
					{
						selectStart = 0;
					}
					else
					{
						if(f != nullptr)
						{
							//adjust mouse x and y by the text offsets.
							int adjustMouseX = (mouseX - x) - xOffsetForText;
							int adjustMouseY = (mouseY - y) - yOffsetForText;
							size_t tempSelection = SIZE_MAX;

							if(textElement.getAllowTextWrap())
								tempSelection = f->getSelectIndex(textElement.getTextRef(), textElement.getMaxWidth(), adjustMouseX, adjustMouseY);
							else
								tempSelection = f->getSelectIndex(textElement.getTextRef(), -1, adjustMouseX, adjustMouseY);
							
							// StringTools::println("(%d, %d) - %llu", adjustMouseX, adjustMouseY, tempSelection);
							if(tempSelection != SIZE_MAX)
								selectStart = (int)tempSelection;
							else
								selectStart = 0;
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
			//Find the end select location at the mouse cursor if(f != nullptr)
			if(f != nullptr)
			{
				//adjust mouse x and y by the text offsets.
				int adjustMouseX = (mouseX - x) - xOffsetForText;
				int adjustMouseY = (mouseY - y) - yOffsetForText;
				size_t tempSelection = SIZE_MAX;

				if(textElement.getAllowTextWrap())
					tempSelection = f->getSelectIndex(textElement.getTextRef(), textElement.getMaxWidth(), adjustMouseX, adjustMouseY);
				else
					tempSelection = f->getSelectIndex(textElement.getTextRef(), -1, adjustMouseX, adjustMouseY);

				if(tempSelection != SIZE_MAX)
					selectEnd = (int)tempSelection;
				else
					selectEnd = 0;
			}
			// selectEnd = selectStart;
			cursorLocation = selectEnd;
		}

	}

	void GuiTextBox::selectionCleanup()
	{
		GuiFontInterface* fInt = textElement.getFont();
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
		
		Vec2f cursorPos;

		if(f != nullptr)
		{
			if(textElement.getAllowTextWrap())
				cursorPos = f->getCursorLocation(tempText, cursorLocation, textElement.getMaxWidth());
			else
				cursorPos = f->getCursorLocation(tempText, cursorLocation, -1);
		}
		
		cursorPos += Vec2f(xOffsetForText, yOffsetForText);

		//Introduce a buffer to allow the text to move before hitting the edge.
		int rightBuffer = 4 + textElement.getBaseX();
		int bottomBuffer = f->getVerticalAdvance() + textElement.getBaseY();
		if(!textElement.getAllowTextWrap())
		{
			if(cursorPos.x < 0)
			{
				xOffsetForText -= cursorPos.x;
			}
			if(cursorPos.x >= width - rightBuffer)
			{
				xOffsetForText += (width-rightBuffer) - cursorPos.x;
			}
		}

		if(cursorPos.y < 0)
		{
			yOffsetForText += f->getVerticalAdvance();
		}
		if(cursorPos.y >= height-bottomBuffer)
		{
			yOffsetForText -= f->getVerticalAdvance();
		}

		if(cursorLocation == 0)
		{
			xOffsetForText = 0;
			yOffsetForText = 0;
		}

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
		
		textElement.setOffset(&x, &y);
		textElement.baseUpdate();
		textElement.update();
	}

	GuiTextBlock* GuiTextBox::getTextBlockElement()
	{
		return &textElement;
	}

	GuiContextMenu* GuiTextBox::getContextMenu()
	{
		return &contextMenu;
	}

	void GuiTextBox::enableContextMenu(bool v)
	{
		contextMenuEnabled = v;
		if(v)
		{
			contextMenu.hideMenu();
			contextMenu.setShowOnRightClick(false); //Remains in the children list but can not be enabled using right click
		}
		else
		{
			contextMenu.setShowOnRightClick(true); //Remains in the children list but can not be enabled using right click
		}
	}

	bool GuiTextBox::isContextMenuEnabled()
	{
		return contextMenuEnabled;
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

	void GuiTextBox::loadDataFromXML(SimpleHashMap<std::string, std::string>& attributes)
	{
		GuiInstance::loadDataFromXML(attributes);

		std::vector<std::string> possibleNames = { "width", "height", "cursorblinktimer", "cursorwidth", "backgroundcolor", "outlinecolor", "focusoutlinecolor", "cursorblinkcolor", "allowlinebreaks", "enablecontextmenu"};

		for(size_t i=0; i<possibleNames.size(); i++)
		{
			auto it = attributes.get(possibleNames[i]);
			if(it != nullptr)
			{
				if(possibleNames[i] == "width")
				{
					this->width = std::abs(StringTools::toInt(it->data));
				}
				else if(possibleNames[i] == "height")
				{
					this->height = std::abs(StringTools::toInt(it->data));
				}
				else if(possibleNames[i] == "cursorblinktimer")
				{
					this->setCursorBlinkTimer(StringTools::toInt(it->data));
				}
				else if(possibleNames[i] == "cursorwidth")
				{
					this->cursorWidth = std::abs(StringTools::toInt(it->data));
				}
				else if(possibleNames[i] == "backgroundcolor")
				{
					//define as color name or rgba
					this->backgroundColor = ColorNameConverter::NameToColor(it->data);
				}
				else if(possibleNames[i] == "outlinecolor")
				{
					//define as color name or rgba
					this->outlineColor = ColorNameConverter::NameToColor(it->data);
				}
				else if(possibleNames[i] == "focusoutlinecolor")
				{
					//define as color name or rgba
					this->focusOutlineColor = ColorNameConverter::NameToColor(it->data);
				}
				else if(possibleNames[i] == "cursorblinkcolor")
				{
					//define as color name or rgba
					this->cursorBlinkColor = ColorNameConverter::NameToColor(it->data);
				}
				else if(possibleNames[i] == "allowlinebreaks")
				{
					this->allowLineBreaks = StringTools::equalsIgnoreCase<char>(it->data, "true");
				}
				else if(possibleNames[i] == "enablecontextmenu")
				{
					this->enableContextMenu( StringTools::equalsIgnoreCase<char>(it->data, "true") );
				}
				
				attributes.remove(it);
			}
		}
		
		boundingBox = Box2D(x, y, x+width, y+height);

		textElement.loadDataFromXML(attributes);
		
		if(textElement.getMaxWidth() == 0)
			textElement.setMaxWidth(width - textElement.getBaseX()*2 - 1);

		if(textElement.getMaxHeight() == 0)
			textElement.setMaxHeight(height - textElement.getBaseY()*2 - 1);
	}

	GuiInstance* GuiTextBox::loadFunction(SimpleHashMap<std::string, std::string>& attributes)
	{
		GuiTextBox* ins = new GuiTextBox(0, 0, 0, 0);
		ins->loadDataFromXML(attributes);

		return ins;
	}

	void GuiTextBox::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiTextBox", GuiTextBox::loadFunction);
	}

	#pragma endregion

} //NAMESPACE glib END