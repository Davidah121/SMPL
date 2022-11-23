#include "Font.h"
#include "SimpleFile.h"
#include "StringTools.h"
#include "Sort.h"

namespace glib
{
		
	const Class Font::globalClass = Class("Font", {&Object::globalClass});

	Font::Font()
	{
		setClass(globalClass);
	}

	Font::~Font()
	{
	}
	
	void Font::copyFont(Font& f)
	{
		setClass( f.getClass() );
		charInfoList = f.charInfoList;

		maxHorizAdv = f.maxHorizAdv;
		ascent = f.ascent;
		descent = f.descent;
		verticalAdv = f.verticalAdv;
		
		fontSize = f.fontSize;
		originalFontSize = f.originalFontSize;

		sorted = f.sorted;
	}

	FontCharInfo Font::getFontCharInfo(int index)
	{
		if (index >= 0 && index < charInfoList.size())
		{
			double scaleVal = (double)fontSize / originalFontSize;
			FontCharInfo nFCI = charInfoList[index];
			nFCI.x = MathExt::round(nFCI.x * scaleVal);
			nFCI.y = MathExt::round(nFCI.y * scaleVal);
			nFCI.width = MathExt::round(nFCI.width * scaleVal);
			nFCI.height = MathExt::round(nFCI.height * scaleVal);
			nFCI.horizAdv = MathExt::round(nFCI.horizAdv * scaleVal);
			nFCI.xOffset = MathExt::round(nFCI.xOffset * scaleVal);
			nFCI.yOffset = MathExt::round(nFCI.yOffset * scaleVal);
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

	int Font::getCharIndex(int c)
	{
		int index = -1;

		if(sorted)
		{
			//binary search
			int l = 0;
			int r = charInfoList.size()-1;

			int preL, preR;
			preL = l;
			preR = r;

			//check bounds first since they will never occur in the loop
			if(charInfoList[l].unicodeValue == c)
				return l;
			if(charInfoList[r].unicodeValue == c)
				return r;
			
			while(l < r)
			{
				int m = (l+r) / 2;
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
			for (int i = 0; i < charInfoList.size(); i++)
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

	Image* Font::getImage(int index)
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
		return verticalAdv*scaleVal;
	}

	Box2D Font::getBoundingBox(std::string text, int maxWidth, int maxHeight)
	{
		int totalHeight = 0;
		int totalWidth = 0;
		int currWidth = 0;
		int currHeight = 0;

		for(int i=0; i<text.size(); i++)
		{
			int charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);
			if(text[i] == '\n')
			{
				currHeight += this->getVerticalAdvance();
				if(maxHeight >= 0)
				{
					if(currHeight >= maxHeight)
					{
						break;
					}
				}
				continue;
			}

			if(maxWidth >= 0)
			{
				if(currWidth + fci.horizAdv > maxWidth)
				{
					currWidth = 0;
					currHeight += this->getVerticalAdvance();
					if(maxHeight >= 0)
					{
						if(currHeight >= maxHeight)
						{
							break;
						}
					}
				}
			}

			currWidth += fci.horizAdv;
			totalWidth = MathExt::max(totalWidth, currWidth);
		}

		return Box2D(0, 0, totalWidth, currHeight+this->getVerticalAdvance());
	}

	Box2D Font::getBoundingBox(std::wstring text, int maxWidth, int maxHeight)
	{
		int totalHeight = 0;
		int totalWidth = 0;
		int currWidth = 0;
		int currHeight = 0;

		for(int i=0; i<text.size(); i++)
		{
			int charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);
			if(text[i] == L'\n')
			{
				currHeight += this->getVerticalAdvance();
				if(maxHeight >= 0)
				{
					if(currHeight >= maxHeight)
					{
						break;
					}
				}
				continue;
			}
			
			if(maxWidth >= 0)
			{
				if(currWidth + fci.horizAdv > maxWidth)
				{
					currWidth = 0;
					currHeight += this->getVerticalAdvance();
					if(maxHeight >= 0)
					{
						if(currHeight >= maxHeight)
						{
							break;
						}
					}
				}
			}

			currWidth += fci.horizAdv;
			totalWidth = MathExt::max(totalWidth, currWidth);
		}

		return Box2D(0, 0, totalWidth, currHeight+this->getVerticalAdvance());
	}

	Vec2f Font::getCursorLocation(std::string text, size_t charIndex, int maxWidth, int maxHeight)
	{
		int currWidth = 0;
		int currHeight = 0;

		size_t actualSize = MathExt::clamp<size_t>(charIndex, 0, text.size());

		for(int i=0; i<actualSize; i++)
		{
			int charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);
			if(text[i] == '\n')
			{
				currHeight += this->getVerticalAdvance();
				if(maxHeight >= 0)
				{
					if(currHeight >= maxHeight)
					{
						break;
					}
				}
				continue;
			}

			if(maxWidth >= 0)
			{
				if(currWidth + fci.horizAdv > maxWidth)
				{
					currWidth = 0;
					currHeight += this->getVerticalAdvance();
					if(maxHeight >= 0)
					{
						if(currHeight >= maxHeight)
						{
							break;
						}
					}
				}
			}

			currWidth += fci.horizAdv;
		}

		return Vec2f(currWidth, currHeight);
	}

	Vec2f Font::getCursorLocation(std::wstring text, size_t charIndex, int maxWidth, int maxHeight)
	{
		int currWidth = 0;
		int currHeight = 0;

		size_t actualSize = MathExt::clamp<size_t>(charIndex, 0, text.size());

		for(int i=0; i<actualSize; i++)
		{
			int charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);
			if(text[i] == L'\n')
			{
				currHeight += this->getVerticalAdvance();
				if(maxHeight >= 0)
				{
					if(currHeight >= maxHeight)
					{
						break;
					}
				}
				continue;
			}

			if(maxWidth >= 0)
			{
				if(currWidth + fci.horizAdv > maxWidth)
				{
					currWidth = 0;
					currHeight += this->getVerticalAdvance();
					if(maxHeight >= 0)
					{
						if(currHeight >= maxHeight)
						{
							break;
						}
					}
				}
			}

			currWidth += fci.horizAdv;
		}

		return Vec2f(currWidth, currHeight);
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