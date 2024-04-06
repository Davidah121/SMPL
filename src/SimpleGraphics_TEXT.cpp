#include "InternalGraphicsHeader.h"

namespace smpl
{
	void SimpleGraphics::drawText(std::string str, int x, int y, Image* surf)
	{
		SimpleGraphics::drawText(StringTools::utf8ToIntString(str), x, y, surf);
	}

	void SimpleGraphics::drawText(std::wstring str, int x, int y, Image* surf)
	{
		SimpleGraphics::drawText(StringTools::wideStringToIntString(str), x, y, surf);
	}

	void SimpleGraphics::drawText(std::vector<int> str, int x, int y, Image* surf)
	{
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		Font* tFont = getFont();

		if (otherImg != nullptr && tFont != nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			int currX = x;
			int currY = y;
			for(size_t i=0; i<str.size(); i++)
			{
				int charIndex = tFont->getCharIndex(str[i]);
				Image* charImg = tFont->getImage(charIndex);
				FontCharInfo fci = tFont->getFontCharInfo(charIndex);
				
				if(charImg == nullptr && str[i] != '\n')
				{
					continue;
				}

				if(str[i] == '\n')
				{
					currX = x;
					currY += tFont->getVerticalAdvance();
					continue;
				}

				if(str[i] == ' ')
				{
					currX += fci.horizAdv;
					continue;
				}

				if(fci.x!=0 || fci.y!=0 || fci.width != charImg->getWidth() || fci.height != charImg->getHeight())
					drawSpritePart(charImg, currX+fci.xOffset, currY+fci.yOffset, fci.x, fci.y, fci.width, fci.height, otherImg);
				else
					drawSprite(charImg, currX+fci.xOffset, currY+fci.yOffset, otherImg);
				
				currX += fci.horizAdv;
			}
		}
	}

	void SimpleGraphics::drawTextHighlighted(std::string str, int x, int y, int highlightStart, int highlightEnd, Color highlightColor, Image* surf)
	{
		SimpleGraphics::drawTextHighlighted(StringTools::utf8ToIntString(str), x, y, highlightStart, highlightEnd, highlightColor, surf);
	}

	void SimpleGraphics::drawTextHighlighted(std::wstring str, int x, int y, int highlightStart, int highlightEnd, Color highlightColor, Image* surf)
	{
		SimpleGraphics::drawTextHighlighted(StringTools::wideStringToIntString(str), x, y, highlightStart, highlightEnd, highlightColor, surf);
	}

	void SimpleGraphics::drawTextHighlighted(std::vector<int> str, int x, int y, int highlightStart, int highlightEnd, Color highlightColor, Image* surf)
	{
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		Font* tFont = getFont();

		if (otherImg != nullptr && tFont != nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			int currX = x;
			int currY = y;
			for(size_t i=0; i<str.size(); i++)
			{
				int charIndex = tFont->getCharIndex(str[i]);
				Image* charImg = tFont->getImage(charIndex);
				FontCharInfo fci = tFont->getFontCharInfo(charIndex);
				
				if(charImg == nullptr && str[i] != '\n')
				{
					continue;
				}

				if(str[i] == '\n')
				{
					currX = x;
					currY += tFont->getVerticalAdvance();
					continue;
				}

				if(str[i] != ' ')
				{
					if(fci.x!=0 || fci.y!=0 || fci.width != charImg->getWidth() || fci.height != charImg->getHeight())
						drawSpritePart(charImg, currX+fci.xOffset, currY+fci.yOffset, fci.x, fci.y, fci.width, fci.height, otherImg);
					else
						drawSprite(charImg, currX+fci.xOffset, currY+fci.yOffset, otherImg);
				}

				if(i >= highlightStart && i < highlightEnd)
				{
					Color oldColor = activeColor;
					SimpleGraphics::setColor(highlightColor);
					drawRect(currX, currY, currX+fci.horizAdv, currY+tFont->getVerticalAdvance(), false, surf);
					SimpleGraphics::setColor(oldColor);
				}

				currX += fci.horizAdv;
			}
		}
	}

	void SimpleGraphics::drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool allowTextWrap, Image* surf)
	{
		SimpleGraphics::drawTextLimits(StringTools::utf8ToIntString(str), x, y, maxWidth, maxHeight, allowTextWrap, surf);
	}

	void SimpleGraphics::drawTextLimits(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool allowTextWrap, Image* surf)
	{
		SimpleGraphics::drawTextLimits(StringTools::wideStringToIntString(str), x, y, maxWidth, maxHeight, allowTextWrap, surf);
	}

	void SimpleGraphics::drawTextLimits(std::vector<int> str, int x, int y, int maxWidth, int maxHeight, bool allowTextWrap, Image* surf)
	{
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		Font* tFont = getFont();

		if (otherImg != nullptr && tFont != nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			int currX = x;
			int currY = y;
			int currW = 0;
			int currH = 0;
			bool waitingOnLineBreak = false;
			
			for(size_t i=0; i<str.size(); i++)
			{
				int charIndex = tFont->getCharIndex(str[i]);
				Image* charImg = tFont->getImage(charIndex);
				FontCharInfo fci = tFont->getFontCharInfo(charIndex);
				
				if(charImg == nullptr && str[i] != '\n')
				{
					continue;
				}
				
				if(str[i] == '\n')
				{
					waitingOnLineBreak = false;
					currX = x;
					currW = 0;

					currH += tFont->getVerticalAdvance();
					currY += tFont->getVerticalAdvance();

					if(currH >= maxHeight)
					{
						break;
					}
					continue;
				}

				if(waitingOnLineBreak)
				{
					continue;
				}
				
				if(currW + fci.width > maxWidth)
				{
					if(allowTextWrap)
					{
						currX = x;
						currW = 0;
						currH += tFont->getVerticalAdvance();
						currY += tFont->getVerticalAdvance();
						
						if(currH >= maxHeight)
						{
							break;
						}
					}
				}

				if(currW + fci.width > maxWidth)
				{
					fci.width = maxWidth - currW;
				}
				if(currH + fci.height > maxHeight)
				{
					fci.height = maxHeight - currH;
				}

				if(str[i] != ' ')
				{
					if(fci.x!=0 || fci.y!=0 || fci.width != charImg->getWidth() || fci.height != charImg->getHeight())
						drawSpritePart(charImg, currX+fci.xOffset, currY+fci.yOffset, fci.x, fci.y, fci.width, fci.height, otherImg);
					else
						drawSprite(charImg, currX+fci.xOffset, currY+fci.yOffset, otherImg);
					
					currX += fci.horizAdv;
					currW += fci.horizAdv;
				}
				else
				{
					currX += fci.horizAdv;
					currW += fci.horizAdv;
				}

				if(currW >= maxWidth)
				{
					currW = 0;
					currX = x;

					if(allowTextWrap)
					{
						currH += tFont->getVerticalAdvance();
						currY += tFont->getVerticalAdvance();

						if(currH >= maxHeight)
						{
							break;
						}
					}
					else
					{
						waitingOnLineBreak = true;
					}
				}
			}
		}
	}
	
	void SimpleGraphics::drawTextLimitsHighlighted(std::string str, int x, int y, int maxWidth, int maxHeight, bool allowTextWrap, int highlightStart, int highlightEnd, Color highlightColor, Image* surf)
	{
		SimpleGraphics::drawTextLimitsHighlighted(StringTools::utf8ToIntString(str), x, y, maxWidth, maxHeight, allowTextWrap, highlightStart, highlightEnd, highlightColor, surf);
	}
	
	void SimpleGraphics::drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool allowTextWrap, int highlightStart, int highlightEnd, Color highlightColor, Image* surf)
	{
		SimpleGraphics::drawTextLimitsHighlighted(StringTools::wideStringToIntString(str), x, y, maxWidth, maxHeight, allowTextWrap, highlightStart, highlightEnd, highlightColor, surf);
	}

	void SimpleGraphics::drawTextLimitsHighlighted(std::vector<int> str, int x, int y, int maxWidth, int maxHeight, bool allowTextWrap, int highlightStart, int highlightEnd, Color highlightColor, Image* surf)
	{
		Image* otherImg;
		if (surf == nullptr)
			return;
		else
			otherImg = surf;

		Font* tFont = getFont();

		if (otherImg != nullptr && tFont != nullptr)
		{
			if(otherImg->getWidth()<=0 || otherImg->getHeight()<=0)
			{
				return;
			}
			int currX = x;
			int currY = y;
			int currW = 0;
			int currH = 0;
			bool waitingOnLineBreak = false;
			
			for(size_t i=0; i<str.size(); i++)
			{
				int charIndex = tFont->getCharIndex(str[i]);
				Image* charImg = tFont->getImage(charIndex);
				FontCharInfo fci = tFont->getFontCharInfo(charIndex);
				
				if(charImg == nullptr && str[i] != '\n')
				{
					continue;
				}
				
				if(str[i] == '\n')
				{
					waitingOnLineBreak = false;
					currX = x;
					currW = 0;

					currH += tFont->getVerticalAdvance();
					currY += tFont->getVerticalAdvance();

					if(currH >= maxHeight)
					{
						break;
					}
					continue;
				}

				if(waitingOnLineBreak)
				{
					continue;
				}
				
				if(currW + fci.width > maxWidth)
				{
					if(allowTextWrap)
					{
						currX = x;
						currW = 0;
						currH += tFont->getVerticalAdvance();
						currY += tFont->getVerticalAdvance();
						
						if(currH >= maxHeight)
						{
							break;
						}
					}
				}

				if(currW + fci.width > maxWidth)
				{
					fci.width = maxWidth - currW;
				}
				if(currH + fci.height > maxHeight)
				{
					fci.height = maxHeight - currH;
				}

				if(str[i] != ' ')
				{
					if(fci.x!=0 || fci.y!=0 || fci.width != charImg->getWidth() || fci.height != charImg->getHeight())
						drawSpritePart(charImg, currX+fci.xOffset, currY+fci.yOffset, fci.x, fci.y, fci.width, fci.height, otherImg);
					else
						drawSprite(charImg, currX+fci.xOffset, currY+fci.yOffset, otherImg);
				}

				if(i >= highlightStart && i < highlightEnd)
				{
					Color oldColor = activeColor;
					SimpleGraphics::setColor(highlightColor);
					drawRect(currX, currY, currX+fci.horizAdv, currY+tFont->getVerticalAdvance(), false, surf);
					SimpleGraphics::setColor(oldColor);
				}

				currX += fci.horizAdv;
				currW += fci.horizAdv;

				if(currW >= maxWidth)
				{
					currW = 0;
					currX = x;

					if(allowTextWrap)
					{
						currH += tFont->getVerticalAdvance();
						currY += tFont->getVerticalAdvance();

						if(currH >= maxHeight)
						{
							break;
						}
					}
					else
					{
						waitingOnLineBreak = true;
					}
				}
			}
		}
	}

} //NAMESPACE glib END