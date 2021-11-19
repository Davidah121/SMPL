#include "InternalGuiManagerHeader.h"

namespace glib
{
    
	#pragma region GUI_TEXTBLOCK_CLASS

	const Class GuiTextBlock::myClass = Class("GuiTextBlock", {&GuiInstance::myClass});
	const Class* GuiTextBlock::getClass()
	{
		return &GuiTextBlock::myClass;
	}

	GuiTextBlock::GuiTextBlock(int x, int y, int width, int height)
	{
		setBaseX(x);
		setBaseY(y);
		this->maxWidth = width;
		this->maxHeight = height;
	}

	GuiTextBlock::~GuiTextBlock()
	{
		
	}

	void GuiTextBlock::update()
	{
		Font* currFont = textFont;
		if(currFont == nullptr)
			currFont = Graphics::getFont();

		int totalWidth = currFont->getWidthOfString(text);

	}

	void GuiTextBlock::render(Image* surf)
	{
		if(surf!=nullptr)
		{
			Font* oldFont = Graphics::getFont();

			if(textFont!=nullptr)
				Graphics::setFont(textFont);

			Graphics::setColor(textColor);

			int actualMaxW = (maxWidth <= 0) ? 0xFFFF : maxWidth;
			int actualMaxH = (maxHeight <= 0) ? 0xFFFF : maxHeight;
			

			Graphics::setClippingRect( Box2D(renderX, renderY, renderX+actualMaxW, renderY+actualMaxH) );
			Graphics::drawTextLimits(text, renderX+offsetX, renderY+offsetY, actualMaxW-offsetX, actualMaxH-offsetY, allowLineBreaks, surf);

			if(shouldHighlight && endHighlight>=0 && startHighlight>=0)
			{
				int actualS, actualE;

				actualS = MathExt::clamp(startHighlight, 0, (int)text.size());
				actualE = MathExt::clamp(endHighlight, 0, (int)text.size());

				if(actualS > actualE)
				{
					int swap = actualS;
					actualS = actualE;
					actualE = swap;
				}
				
				Font* currFont = Graphics::getFont();
				std::string startString, endString;
				startString = text.substr(0, actualS);
				endString = text.substr(actualE, text.size());

				//TODO Change to deal with highlighting text vertically
				int startWidth = currFont->getWidthOfString(startString);
				int endWidth = currFont->getWidthOfString(endString);

				int totalWidth = currFont->getWidthOfString(text);
				
				totalWidth -= (startWidth + endWidth);

				Graphics::setColor(highlightColor);
				int sX = renderX + startWidth;
				Graphics::drawRect(sX+offsetX, renderY+offsetY, sX+totalWidth+offsetX, renderY+offsetY+currFont->getFontSize(), false, surf);
			}
			
			Graphics::resetClippingPlane();
			Graphics::setFont(oldFont);
		}
	}

	void GuiTextBlock::setTextColor(Color c)
	{
		textColor = c;
	}

	Color GuiTextBlock::getTextColor()
	{
		return textColor;
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
	}

	void GuiTextBlock::setFont(Font* f)
	{
		textFont = f;
	}

	Font* GuiTextBlock::getFont()
	{
		return textFont;
	}

	void GuiTextBlock::setMaxWidth(int v)
	{
		maxWidth = v;
	}
	void GuiTextBlock::setMaxHeight(int v)
	{
		maxHeight = v;
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
	}
	Color GuiTextBlock::getHighlightColor()
	{
		return highlightColor;
	}

	void GuiTextBlock::setShouldHighlightText(bool v)
	{
		shouldHighlight = v;
	}
	bool GuiTextBlock::getShouldHighlightText()
	{
		return shouldHighlight;
	}

	void GuiTextBlock::setHighlightStart(int v)
	{
		startHighlight = v;
	}
	int GuiTextBlock::getHighlightStart()
	{
		return startHighlight;
	}
	void GuiTextBlock::setHighlightEnd(int v)
	{
		endHighlight = v;
	}
	int GuiTextBlock::getHighlightEnd()
	{
		return endHighlight;
	}

	void GuiTextBlock::setAllowLineBreaks(bool v)
	{
		allowLineBreaks = v;
	}

	bool GuiTextBlock::getAllowLineBreaks()
	{
		return allowLineBreaks;
	}

	void GuiTextBlock::setOffsetX(int x)
	{
		offsetX = x;
	}

	void GuiTextBlock::setOffsetY(int y)
	{
		offsetY = y;
	}

	#pragma endregion

} //NAMESPACE glib END