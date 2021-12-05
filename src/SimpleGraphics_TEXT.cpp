#include "InternalGraphicsHeader.h"

namespace glib
{
	void SimpleGraphics::drawText(std::string str, int x, int y, Image* surf)
	{
		SimpleGraphics::drawText(StringTools::toWideString(str), x, y, surf);
	}

	void SimpleGraphics::drawText(std::wstring str, int x, int y, Image* surf)
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
			for(int i=0; i<str.length(); i++)
			{
				int charIndex = tFont->getCharIndex(str[i]);
				Image* charImg = tFont->getImage(charIndex);
				FontCharInfo fci = tFont->getFontCharInfo(str[i]);
				
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

	void SimpleGraphics::drawTextLimits(std::string str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, Image* surf)
	{
		SimpleGraphics::drawTextLimits(StringTools::toWideString(str), x, y, maxWidth, maxHeight, useLineBreak, surf);
	}

	void SimpleGraphics::drawTextLimits(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, Image* surf)
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
			
			for(int i=0; i<str.length(); i++)
			{
				int charIndex = tFont->getCharIndex(str[i]);
				Image* charImg = tFont->getImage(charIndex);
				FontCharInfo fci = tFont->getFontCharInfo(str[i]);
				
				if(charImg == nullptr && str[i] != '\n')
				{
					continue;
				}
				
				if(str[i] == '\n')
				{
					currX = x;
					currW = 0;

					if(useLineBreak)
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
						break;
					}
					continue;
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

					if(useLineBreak)
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
						break;
					}
				}
			}
		}
	}

	void SimpleGraphics::drawTextLimitsHighlighted(std::wstring str, int x, int y, int maxWidth, int maxHeight, bool useLineBreak, int highlightStart, int highlightEnd, Color highlightColor, Image* surf)
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
			
			for(int i=0; i<str.length(); i++)
			{
				int charIndex = tFont->getCharIndex(str[i]);
				Image* charImg = tFont->getImage(charIndex);
				FontCharInfo fci = tFont->getFontCharInfo(str[i]);
				
				if(charImg == nullptr && str[i] != '\n')
				{
					continue;
				}
				
				if(str[i] == '\n')
				{
					currX = x;
					currW = 0;

					if(useLineBreak)
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
						break;
					}
					continue;
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
					
					if(i >= highlightStart && i <= highlightEnd)
					{
						Color oldColor = activeColor;
						SimpleGraphics::setColor(highlightColor);
						drawRect(currX, currY, currX+fci.horizAdv, currY+tFont->getVerticalAdvance(), false, surf);
						SimpleGraphics::setColor(oldColor);
					}

					currX += fci.horizAdv;
					currW += fci.horizAdv;
				}
				else
				{
					if(i >= highlightStart && i <= highlightEnd)
					{
						Color oldColor = activeColor;
						SimpleGraphics::setColor(highlightColor);
						drawRect(currX, currY, currX+fci.horizAdv, currY+tFont->getVerticalAdvance(), false, surf);
						SimpleGraphics::setColor(oldColor);
					}

					currX += fci.horizAdv;
					currW += fci.horizAdv;
				}

				if(currW >= maxWidth)
				{
					currW = 0;
					currX = x;

					if(useLineBreak)
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
						break;
					}
				}
			}
		}
	}

} //NAMESPACE glib END