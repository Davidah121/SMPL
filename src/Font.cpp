#include "Font.h"
#include "SimpleFile.h"
#include "StringTools.h"
#include "Sort.h"

namespace smpl
{
	
	Font::Font()
	{
	}

	Font::~Font()
	{
	}
	
	void Font::copyFont(Font& f)
	{
		charInfoList = f.charInfoList;

		maxHorizAdv = f.maxHorizAdv;
		ascent = f.ascent;
		descent = f.descent;
		verticalAdv = f.verticalAdv;
		
		fontSize = f.fontSize;
		originalFontSize = f.originalFontSize;

		sorted = f.sorted;
	}

	FontCharInfo Font::getFontCharInfo(size_t index)
	{
		if (index >= 0 && index < charInfoList.size())
		{
			double scaleVal = (double)fontSize / originalFontSize;

			FontCharInfo nFCI = charInfoList[index];
			nFCI.x = (int)MathExt::round(nFCI.x * scaleVal);
			nFCI.y = (int)MathExt::round(nFCI.y * scaleVal);
			nFCI.width = (int)MathExt::round(nFCI.width * scaleVal);
			nFCI.height = (int)MathExt::round(nFCI.height * scaleVal);
			nFCI.horizAdv = (int)MathExt::round(nFCI.horizAdv * scaleVal);
			nFCI.xOffset = (int)MathExt::round(nFCI.xOffset * scaleVal);
			nFCI.yOffset = (int)MathExt::round(nFCI.yOffset * scaleVal);
			return nFCI;
		}
		
		#ifdef USE_EXCEPTIONS
		throw OutOfBoundsError();
		#endif
		return {};
	}

	std::vector<FontCharInfo>& Font::getListOfFontCharInfo()
	{
		return charInfoList;
	}

	size_t Font::getCharIndex(int c)
	{
		size_t index = SIZE_MAX;

		if(sorted)
		{
			//binary search
			size_t l = 0;
			size_t r = charInfoList.size()-1;

			size_t preL, preR;
			preL = l;
			preR = r;

			//check bounds first since they will never occur in the loop
			if(charInfoList[l].unicodeValue == c)
				return l;
			if(charInfoList[r].unicodeValue == c)
				return r;
			
			while(l < r)
			{
				size_t m = (l+r) / 2;
				int uniVal = charInfoList[m].unicodeValue;
				
				if(uniVal == c)
					return m;
				else if( c < uniVal)
					r = m;
				else if( c > uniVal)
					l = m;
				
				if(preL == l && preR == r)
				{
					//not found
					break;
				}
				preL = l;
				preR = r;
			}
		}
		else
		{
			//linear search
			for (size_t i = 0; i < charInfoList.size(); i++)
			{
				if (charInfoList[i].unicodeValue == c)
				{
					index = i;
					break;
				}
			}
		}
		
		
		return index;
	}

	std::vector<FontCharBoxInfo> Font::getAllCharBoxes(StringBridge textBridge, unsigned int maxWidth, char wrapMode)
	{
		size_t lastWordIndex = 0;
		std::vector<FontCharBoxInfo> output;
		int currX = 0;
		int currW = 0;
		int currY = 0;
		int lastChar = 0;
		bool doneWordWrap = false;
		std::u32string text = textBridge.getData();
		for(size_t i=0; i<text.size(); i++)
		{
			if(text[i] == '\n')
			{
				lastChar = '\n';
				doneWordWrap = false;
				lastWordIndex = output.size();
				currX = 0;
				currW = 0;
				currY += getVerticalAdvance();
				continue;
			}

			size_t actualIndex = getCharIndex(text[i]);
			if(actualIndex == SIZE_MAX)
			{
				continue;
			}
			FontCharInfo fci = getFontCharInfo(actualIndex);

			if(lastChar == ' ' && fci.unicodeValue > ' ')
			{
				doneWordWrap = false;
				lastWordIndex = output.size();
			}
			lastChar = fci.unicodeValue;

			if(currW + fci.width >= maxWidth && wrapMode != NO_WRAP)
			{
				if(wrapMode == WORD_WRAP && !doneWordWrap)
				{
					//move the entire word down. If it is still in violation, move the next character down
					int offsetX = output[lastWordIndex].boundingBox.getLeftBound();
					int tempW = 0;
					currY += getVerticalAdvance();
					for(size_t j=lastWordIndex; j<output.size(); j++)
					{
						output[j].boundingBox.setLeftBound(output[j].boundingBox.getLeftBound() - offsetX);
						output[j].boundingBox.setRightBound(output[j].boundingBox.getRightBound() - offsetX);
						output[j].boundingBox.setTopBound(output[j].boundingBox.getTopBound() + getVerticalAdvance());
						output[j].boundingBox.setBottomBound(output[j].boundingBox.getBottomBound() + getVerticalAdvance());
						output[j].rowStartPosition = currY;
						tempW += output[j].boundingBox.getWidth();
					}
					doneWordWrap = true;
				}
				else if(wrapMode == CHARACTER_WRAP || doneWordWrap)
				{
					//simply move the next character over.
					currX = 0;
					currW = 0;
					currY += getVerticalAdvance();
				}
			}

			if(fci.width > 0 && fci.height > 0)
				output.push_back({i, currY, Box2D(currX+fci.xOffset, currY+fci.yOffset, currX+fci.xOffset+fci.width, currY+fci.yOffset+fci.height)});

			currX += fci.horizAdv;
			currW += fci.horizAdv;
		}
		return output;
	}

	Image* Font::getImage(size_t index)
	{
		return nullptr;
	}

	void Font::setFontSize(int value)
	{
		fontSize = value;
	}

	int Font::getFontSize()
	{
		return fontSize;
	}

	int Font::getOriginalFontSize()
	{
		return originalFontSize;
	}

	int Font::getVerticalAdvance()
	{
		double scaleVal = (double)fontSize / originalFontSize;
		return (int)MathExt::round(verticalAdv*scaleVal);
	}

	Box2D Font::getBoundingBox(StringBridge textBridge, unsigned int maxWidth, char wrapMode)
	{
		int totalWidth = 0;
		int totalHeight = 0;

		auto allBoxes = getAllCharBoxes(textBridge, maxWidth, wrapMode);

		for(FontCharBoxInfo& boxPair : allBoxes)
		{
			totalWidth = MathExt::max<int>(boxPair.boundingBox.getRightBound(), totalWidth);
			totalHeight = MathExt::max<int>(boxPair.rowStartPosition + getVerticalAdvance(), totalHeight);
		}
		
		return Box2D(0, 0, totalWidth, totalHeight);
	}

	size_t Font::getSelectIndex(StringBridge textBridge, unsigned int maxWidth, char wrapMode, int x, int y)
	{
		if(x < 0 || y < 0)
			return SIZE_MAX;
		if(x >= maxWidth)
			return SIZE_MAX;

		size_t currIndex = 0;
		auto allBoxes = getAllCharBoxes(textBridge, maxWidth, wrapMode);

		unsigned int topY = 0;
		unsigned int bottomY = getVerticalAdvance();

		size_t potentialSelectIndex = 0;

		for(FontCharBoxInfo& boxPair : allBoxes)
		{
			topY = boxPair.rowStartPosition;
			bottomY = topY + getVerticalAdvance();

			if(y > bottomY || y < topY)
				continue;
			
			if(x >= boxPair.boundingBox.getLeftBound())
				potentialSelectIndex = boxPair.charIndex;
			else
				break;
		}
		
		return potentialSelectIndex;
	}

	void Font::addChar(FontCharInfo a)
	{
		charInfoList.push_back(a);
		sorted = false;
	}

	void Font::sortList(bool insertionSort)
	{
		if(insertionSort)
		{
			sorted = true;
			Sort::insertionSort<FontCharInfo>(charInfoList.data(), charInfoList.size(), [](FontCharInfo a, FontCharInfo b) -> bool{
				return a.unicodeValue < b.unicodeValue;
			});
		}
		else
		{
			sorted = true;
			Sort::mergeSort<FontCharInfo>(charInfoList.data(), charInfoList.size(), [](FontCharInfo a, FontCharInfo b) -> bool{
				return a.unicodeValue < b.unicodeValue;
			});
		}
	}

} //NAMESPACE glib END