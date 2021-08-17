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
		this->width = width;
		this->height = height;
	}

	GuiTextBlock::~GuiTextBlock()
	{
		
	}

	void GuiTextBlock::update()
	{
		
	}

	void GuiTextBlock::render(Image* surf)
	{
		if(surf!=nullptr)
		{
			Font* oldFont = Graphics::getFont();

			if(textFont!=nullptr)
				Graphics::setFont(textFont);

			Graphics::setColor(textColor);

			Graphics::setClippingRect( Box2D(renderX, renderY, renderX+width, renderY+height) );
			Graphics::drawTextLimits(text, renderX+offsetX, renderY+offsetY, width-offsetX, height-offsetY, allowLineBreaks, surf);

			if(shouldHighlight && endHighlight>0 && startHighlight>0)
			{
				int actualS, actualE;

				actualS = MathExt::clamp(startHighlight, 0, text.size());
				actualE = MathExt::clamp(endHighlight, 0, text.size());

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

	void GuiTextBlock::setWidth(int v)
	{
		width = v;
	}
	void GuiTextBlock::setHeight(int v)
	{
		height = v;
	}

	int GuiTextBlock::getWidth()
	{
		return width;
	}
	int GuiTextBlock::getHeight()
	{
		return height;
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