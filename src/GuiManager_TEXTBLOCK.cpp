#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_TEXTBLOCK_CLASS

	const Class GuiTextBlock::globalClass = Class("GuiTextBlock", {&GuiInstance::globalClass});

	GuiTextBlock::GuiTextBlock(int x, int y, int width, int height) : GuiInstance()
	{
		setClass(globalClass);
		setBaseX(x);
		setBaseY(y);
		this->maxWidth = width;
		this->maxHeight = height;
	}

	GuiTextBlock::GuiTextBlock(const GuiTextBlock& other) : GuiInstance(other)
	{
		copy(other);
	}

	void GuiTextBlock::operator=(const GuiTextBlock& other)
	{
		GuiInstance::copy(other);
		copy(other);
	}

	void GuiTextBlock::copy(const GuiTextBlock& other)
	{
		setClass(globalClass);
		maxWidth = other.maxWidth;
		maxHeight = other.maxHeight;
		shouldHighlight = other.shouldHighlight;
		startHighlight = other.startHighlight;
		endHighlight = other.endHighlight;
		offsetX = other.offsetX;
		offsetY = other.offsetY;
		allowWrapText = other.allowWrapText;
		updateBounds = other.updateBounds;
		defaultString = other.defaultString;
		textColor = other.textColor;
		defaultTextColor = other.defaultTextColor;
		highlightColor = other.highlightColor;
		textFont = other.textFont;
		text = other.text;
	}

	GuiTextBlock::~GuiTextBlock()
	{
	}

	void GuiTextBlock::update()
	{
		int width = boundingBox.getWidth();
		int height = boundingBox.getHeight();

		if(updateBounds)
		{
			GuiFontInterface* fInt = (textFont != nullptr) ? textFont : GuiGraphicsInterface::getFont();

			if(fInt == nullptr)
				return;
			
			Font* currFont = fInt->getFont();
			Box2D tempBoundingBox;

			std::string checkText = text;
			if(checkText.empty())
				checkText = defaultString;
			
			if(allowWrapText)
				tempBoundingBox = currFont->getBoundingBox(checkText, maxWidth, maxHeight);
			else
			{
				tempBoundingBox = currFont->getBoundingBox(checkText, -1, maxHeight);
				if(maxWidth >= 0)
				{
					int tempWidth = tempBoundingBox.getRightBound();
					tempWidth = MathExt::clamp(tempWidth, 0, maxWidth);
					tempBoundingBox.setRightBound( tempBoundingBox.getLeftBound() + tempWidth );
				}
			}

			height = (int)tempBoundingBox.getHeight();
			width = (int)tempBoundingBox.getWidth();
			updateBounds = false;
		}

		if((int)boundingBox.getLeftBound() == x && (int)boundingBox.getRightBound() == x+width)
		{
			if((int)boundingBox.getTopBound() == y && (int)boundingBox.getBottomBound() == y+height)
			{
				//Don't do anything. It may still need to be redrawn for other reasons.
			}
			else
			{
				setShouldRedraw(true);
			}
		}
		else
		{
			setShouldRedraw(true);
		}

		boundingBox.setLeftBound(x);
		boundingBox.setRightBound(x+width);
		boundingBox.setTopBound(y);
		boundingBox.setBottomBound(y+height);

		// setShouldRedraw(true);
	}

	void GuiTextBlock::render()
	{
		GuiFontInterface* fInt = (textFont != nullptr) ? textFont : GuiGraphicsInterface::getFont();
		GuiFontInterface* oldFontInt = GuiGraphicsInterface::getFont();

		Font* currFont = fInt->getFont();

		GuiGraphicsInterface::setFont(fInt);
		GuiGraphicsInterface::setColor(textColor);

		int actualMaxW = (maxWidth < 0) ? 0xFFFF : maxWidth; //65535 will be considered the maximum width. Most images and textures limit size to this.
		int actualMaxH = (maxHeight < 0) ? 0xFFFF : maxHeight; //65535 will be considered the maximum height. Most images and textures limit size to this.

		int minHighlight = MathExt::min(startHighlight, endHighlight);
		int maxHighlight = MathExt::max(startHighlight, endHighlight);
		
		if(!text.empty())
		{
			if(shouldHighlight)
				GuiGraphicsInterface::drawTextLimitsHighlighted(text, x+offsetX, y+offsetY, actualMaxW-offsetX, actualMaxH-offsetY, allowWrapText, minHighlight, maxHighlight, highlightColor);
			else
				GuiGraphicsInterface::drawTextLimits(text, x+offsetX, y+offsetY, actualMaxW-offsetX, actualMaxH-offsetY, allowWrapText);
		}
		else
		{
			GuiGraphicsInterface::setColor(defaultTextColor);
			if(shouldHighlight)
				GuiGraphicsInterface::drawTextLimitsHighlighted(defaultString, x+offsetX, y+offsetY, actualMaxW-offsetX, actualMaxH-offsetY, allowWrapText, minHighlight, maxHighlight, highlightColor);
			else
				GuiGraphicsInterface::drawTextLimits(defaultString, x+offsetX, y+offsetY, actualMaxW-offsetX, actualMaxH-offsetY, allowWrapText);
		}
		
		GuiGraphicsInterface::setFont(oldFontInt);

		// Box2D oldClip = GuiGraphicsInterface::getClippingRect();
		// GuiGraphicsInterface::resetClippingPlane();
		// GuiGraphicsInterface::setColor(Color{255,0,0,255});
		// GuiGraphicsInterface::drawRect(boundingBox.getLeftBound(), boundingBox.getTopBound(), boundingBox.getRightBound(), boundingBox.getBottomBound(), true);
		// GuiGraphicsInterface::setClippingRect(oldClip);
		
	}

	void GuiTextBlock::setTextColor(Color c)
	{
		textColor = c;
	}

	Color GuiTextBlock::getTextColor()
	{
		return textColor;
	}

	void GuiTextBlock::setDefaultTextColor(Color c)
	{
		defaultTextColor = c;
	}

	Color GuiTextBlock::getDefaultTextColor()
	{
		return defaultTextColor;
	}

	std::string GuiTextBlock::getText()
	{
		return text;
	}

	std::string& GuiTextBlock::getTextRef()
	{
		return text;
	}

	void GuiTextBlock::setText(std::string s)
	{
		text = s;
		updateBounds = true;
		setShouldRedraw(true);
		update();
	}

	void GuiTextBlock::setText(std::wstring s)
	{
		text = StringTools::toUTF8String(s);
		updateBounds = true;
		setShouldRedraw(true);
		update();
	}

	
	void GuiTextBlock::setDefaultText(std::string s)
	{
		defaultString = s;
		updateBounds = true;
		setShouldRedraw(true);
		update();
	}
	void GuiTextBlock::setDefaultText(std::wstring s)
	{
		defaultString = StringTools::toUTF8String(s);
		updateBounds = true;
		setShouldRedraw(true);
		update();
	}

	void GuiTextBlock::setFont(GuiFontInterface* f)
	{
		textFont = f;
		updateBounds = true;
		setShouldRedraw(true);
		update();
	}

	GuiFontInterface* GuiTextBlock::getFont()
	{
		return textFont;
	}

	void GuiTextBlock::setMaxWidth(int v)
	{
		if(v != maxWidth)
		{
			setShouldRedraw(true);
			update();
		}
		maxWidth = v;
		updateBounds = true;
	}
	void GuiTextBlock::setMaxHeight(int v)
	{
		if(v != maxHeight)
		{
			setShouldRedraw(true);
			update();
		}
		maxHeight = v;
		updateBounds = true;
	}

	int GuiTextBlock::getMaxWidth()
	{
		return maxWidth;
	}
	int GuiTextBlock::getMaxHeight()
	{
		return maxHeight;
	}

	void GuiTextBlock::setHighlightColor(Color c)
	{
		highlightColor = c;
		setShouldRedraw(true);
	}
	Color GuiTextBlock::getHighlightColor()
	{
		return highlightColor;
	}

	void GuiTextBlock::setShouldHighlightText(bool v)
	{
		if(v != shouldHighlight)
		{
			setShouldRedraw(true);
			update();
		}

		shouldHighlight = v;
	}
	bool GuiTextBlock::getShouldHighlightText()
	{
		return shouldHighlight;
	}

	void GuiTextBlock::setHighlightStart(int v)
	{
		if(v != startHighlight)
		{
			setShouldRedraw(true);
			update();
		}
		
		startHighlight = v;
	}
	int GuiTextBlock::getHighlightStart()
	{
		return startHighlight;
	}
	void GuiTextBlock::setHighlightEnd(int v)
	{
		if(v != endHighlight)
		{
			setShouldRedraw(true);
			update();
		}
		
		endHighlight = v;
	}
	int GuiTextBlock::getHighlightEnd()
	{
		return endHighlight;
	}

	void GuiTextBlock::setAllowTextWrap(bool v)
	{
		if(allowWrapText != v)
		{
			setShouldRedraw(true);
			update();
		}
		allowWrapText = v;
		updateBounds = true;
	}

	bool GuiTextBlock::getAllowTextWrap()
	{
		return allowWrapText;
	}

	void GuiTextBlock::setOffsetX(int x)
	{
		if(x != offsetX)
		{
			setShouldRedraw(true);
			update();
		}
		offsetX = x;
		updateBounds = true;
	}

	void GuiTextBlock::setOffsetY(int y)
	{
		if(y != offsetY)
		{
			setShouldRedraw(true);
			update();
		}
		offsetY = y;
		updateBounds = true;
	}

	void GuiTextBlock::solveBoundingBox()
	{
		//Just call update since it only solves for the new bounding box.
		update();
	}

	void GuiTextBlock::loadDataFromXML(std::unordered_map<std::string, std::string>& attributes)
	{
		GuiInstance::loadDataFromXML(attributes);

		std::vector<std::string> possibleNames = { "maxwidth", "maxheight", "textcolor", "defaulttextcolor", "highlightcolor", "allowhighlight", "allowwraptext", "highlightstart", "highlightend", "textxoffset", "textyoffset", "text", "defaulttext" };

		for(int i=0; i<possibleNames.size(); i++)
		{
			auto it = attributes.find(possibleNames[i]);
			if(it != attributes.end())
			{
				if(possibleNames[i] == "maxwidth")
				{
					this->maxWidth = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == "maxheight")
				{
					this->maxHeight = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == "textcolor")
				{
					//define as color name or rgba
					this->textColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == "defaulttextcolor")
				{
					//define as color name or rgba
					this->defaultTextColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == "highlightcolor")
				{
					//define as color name or rgba
					this->highlightColor = ColorNameConverter::NameToColor(it->second);
				}
				else if(possibleNames[i] == "allowhighlight")
				{
					this->shouldHighlight = StringTools::equalsIgnoreCase<char>(it->second, "true");
				}
				else if(possibleNames[i] == "allowwraptext")
				{
					this->allowWrapText = StringTools::equalsIgnoreCase<char>(it->second, "true");
				}
				else if(possibleNames[i] == "highlightstart")
				{
					this->startHighlight = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == "highlightend")
				{
					this->endHighlight = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == "textxoffset")
				{
					this->baseX = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == "textyoffset")
				{
					this->baseY = StringTools::toInt(it->second);
				}
				else if(possibleNames[i] == "text")
				{
					this->text = it->second;
				}
				else if(possibleNames[i] == "defaulttext")
				{
					this->defaultString = it->second;
				}

				attributes.erase(possibleNames[i]);
			}
		}

		update(); //just updates the bounding box
	}

	GuiInstance* GuiTextBlock::loadFunction(std::unordered_map<std::string, std::string>& attributes)
	{
		GuiTextBlock* ins = new GuiTextBlock(0, 0);
		ins->loadDataFromXML(attributes);

		return ins;
	}

	void GuiTextBlock::registerLoadFunction()
	{
		GuiManager::registerLoadFunction("GuiTextBlock", GuiTextBlock::loadFunction);
	}


	#pragma endregion

} //NAMESPACE glib END