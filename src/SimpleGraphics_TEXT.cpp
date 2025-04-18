#include "InternalGraphicsHeader.h"

namespace smpl
{
	void SimpleGraphics::drawText(StringBridge strBridge, int x, int y, Image* surf)
	{
		drawTextLimitsHighlighted(strBridge, x, y, -1, -1, Font::NO_WRAP, -1, -1, Color{0, 0, 0, 0}, surf);
	}

	void SimpleGraphics::drawTextHighlighted(StringBridge strBridge, int x, int y, int highlightStart, int highlightEnd, Color highlightColor, Image* surf)
	{
		drawTextLimitsHighlighted(strBridge, x, y, -1, -1, Font::NO_WRAP, highlightStart, highlightEnd, highlightColor, surf);
	}

	void SimpleGraphics::drawTextLimits(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode, Image* surf)
	{
		drawTextLimitsHighlighted(strBridge, x, y, maxWidth, maxHeight, wrapMode, -1, -1, Color{0, 0, 0, 0}, surf);
	}

	void SimpleGraphics::drawTextLimitsHighlighted(StringBridge strBridge, int x, int y, int maxWidth, int maxHeight, char wrapMode, int highlightStart, int highlightEnd, Color highlightColor, Image* surf)
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
			
			Color oldColor = activeColor;
			std::u32string str = strBridge.getData();
			auto boxes = tFont->getAllCharBoxes(str, maxWidth, wrapMode);
			for(auto boxPair : boxes)
			{
				int charIndex = tFont->getCharIndex(str[boxPair.charIndex]);
				Image* charImg = tFont->getImage(charIndex);
				FontCharInfo fci = tFont->getFontCharInfo(charIndex);
				
				if(charImg == nullptr || boxPair.boundingBox.getWidth() == 0 || boxPair.boundingBox.getHeight() == 0)
				{
					//special case for linebreaks to show highlighted sections
					if(boxPair.charIndex >= highlightStart && boxPair.charIndex < highlightEnd)
					{
						int x1 = x+boxPair.boundingBox.getLeftBound();
						int y1 = y+boxPair.rowStartPosition;
						int highlightWidth = 4;

						SimpleGraphics::setColor(highlightColor);
						drawRect(x1, y1, highlightWidth, tFont->getVerticalAdvance(), false, surf);
						SimpleGraphics::setColor(oldColor);
					}
					
					continue;
				}

				if(boxPair.boundingBox.getRightBound() > maxWidth && maxWidth > 0)
				{
					fci.width = maxWidth - boxPair.boundingBox.getRightBound();
				}
				if(boxPair.boundingBox.getBottomBound() > maxHeight && maxHeight > 0)
				{
					fci.height = maxHeight - boxPair.boundingBox.getBottomBound();
				}

				if(str[boxPair.charIndex] != ' ')
				{
					if(fci.x!=0 || fci.y!=0 || fci.width != charImg->getWidth() || fci.height != charImg->getHeight())
						drawSpritePart(charImg, x+boxPair.boundingBox.getLeftBound(), y+boxPair.boundingBox.getTopBound(), fci.x, fci.y, fci.width, fci.height, otherImg);
					else
						drawSprite(charImg, x+boxPair.boundingBox.getLeftBound(), y+boxPair.boundingBox.getTopBound(), otherImg);
				}

				if(boxPair.charIndex >= highlightStart && boxPair.charIndex < highlightEnd)
				{
					int x1 = x+boxPair.boundingBox.getLeftBound();
					int y1 = y+boxPair.rowStartPosition;
					int highlightWidth = boxPair.boundingBox.getWidth() + fci.horizAdv;

					SimpleGraphics::setColor(highlightColor);
					drawRect(x1, y1, highlightWidth, tFont->getVerticalAdvance(), false, surf);
					SimpleGraphics::setColor(oldColor);
				}
			}
		}
	}
} //NAMESPACE glib END