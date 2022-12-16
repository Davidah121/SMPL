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

	Box2D Font::getBoundingBox(std::string text, int maxWidth, int maxHeight)
	{
		int totalWidth = 0;
		int currWidth = 0;
		int currHeight = 0;
		
		for(size_t i=0; i<text.size(); i++)
		{
			//Oddly, skipping over invalid characters messes up the calculations
			//Likely due to how the width and heights are calculated.
			//TODO - FIX SO THAT SKIPPING INVALID CHARACTER INDICES WORKS

			size_t charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);
			
			int newWidth = currWidth + fci.width;
			if( (newWidth > maxWidth && maxWidth > 0) || text[i] == '\n')
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

				if(text[i] == '\n')
				{
					continue;
				}
			}

			//Width may have changed
			newWidth = currWidth + fci.width;
			totalWidth = MathExt::max(totalWidth, newWidth);

			currWidth += fci.horizAdv;
			
			if(i == text.size()-1)
				totalWidth = MathExt::max(totalWidth, newWidth);
		}
		
		totalWidth = MathExt::max(totalWidth, currWidth);

		return Box2D(0, 0, totalWidth, currHeight+this->getVerticalAdvance());
	}

	Box2D Font::getBoundingBox(std::wstring text, int maxWidth, int maxHeight)
	{
		int totalWidth = 0;
		int currWidth = 0;
		int currHeight = 0;

		for(size_t i=0; i<text.size(); i++)
		{
			//Oddly, skipping over invalid characters messes up the calculations
			//Likely due to how the width and heights are calculated.
			//TODO - FIX SO THAT SKIPPING INVALID CHARACTER INDICES WORKS

			size_t charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);
			
			int newWidth = currWidth + fci.width;
			if( (newWidth > maxWidth && maxWidth > 0) || text[i] == L'\n')
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

				if(text[i] == L'\n')
				{
					continue;
				}
			}

			//Width may have changed
			newWidth = currWidth + fci.width;
			totalWidth = MathExt::max(totalWidth, newWidth);

			currWidth += fci.horizAdv;
			
			if(i == text.size()-1)
				totalWidth = MathExt::max(totalWidth, newWidth);
		}
		
		totalWidth = MathExt::max(totalWidth, currWidth);

		return Box2D(0, 0, totalWidth, currHeight+this->getVerticalAdvance());
	}

	size_t Font::getSelectIndex(std::string text, int maxWidth, int x, int y)
	{
		int currWidth = 0;
		int currHeight = 0;

		if(x < 0 || y < 0)
			return SIZE_MAX;

		size_t currIndex = 0;

		for(size_t i=0; i<text.size(); i++)
		{
			size_t charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);

			//y strictly less than currHeight even with the vertical advance. Can quit early
			if(y < currHeight)
				break;
			
			int newWidth = currWidth + fci.width;

			if( (newWidth > maxWidth && maxWidth > 0) || text[i] == '\n')
			{
				currWidth = 0;
				currHeight += this->getVerticalAdvance();
				if(text[i] == '\n')
				{
					continue;
				}
			}
			
			//currWidth may have updated
			newWidth = currWidth + fci.width;
			
			//case 1. X > newWidth
			if(newWidth <= x && y < currHeight + this->getVerticalAdvance())
				currIndex = i+1;
			
			//case 2. Y > currHeight
			if(y > currHeight + this->getVerticalAdvance())
				currIndex = i+1;
			
			//adjust stuff.
			currWidth += fci.horizAdv;
		}

		return currIndex;
	}

	size_t Font::getSelectIndex(std::wstring text, int maxWidth, int x, int y)
	{
		int currWidth = 0;
		int currHeight = 0;

		if(x < 0 || y < 0)
			return SIZE_MAX;

		size_t currIndex = 0;

		for(size_t i=0; i<text.size(); i++)
		{
			size_t charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);

			//y strictly less than currHeight even with the vertical advance. Can quit early
			if(y + this->getVerticalAdvance() < currHeight)
				break;
			
			int newWidth = currWidth + fci.width;

			if( (newWidth >= maxWidth && maxWidth > 0) || text[i] == L'\n')
			{
				currWidth = 0;
				currHeight += this->getVerticalAdvance();
				if(text[i] == '\n')
				{
					continue;
				}
			}
			
			//currWidth may have updated
			newWidth = currWidth + fci.width;
			
			//case 1. X > newWidth
			if(newWidth < x && y < currHeight + this->getVerticalAdvance())
				currIndex = i+1;
			
			//case 2. Y > currHeight
			if(y > currHeight + this->getVerticalAdvance())
				currIndex = i+1;
			
			//adjust stuff.
			currWidth += fci.horizAdv;
		}

		return currIndex;
	}

	Vec2f Font::getCursorLocation(std::string text, size_t charIndex, int maxWidth)
	{
		int currWidth = 0;
		int currHeight = 0;
		
		size_t actualSize = MathExt::clamp<size_t>(charIndex, 0, text.size());

		for(size_t i=0; i<actualSize; i++)
		{
			size_t charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);

			int newWidth = currWidth + fci.width;
			if((newWidth >= maxWidth && maxWidth > 0) || text[i] == '\n')
			{
				currWidth = 0;
				currHeight += this->getVerticalAdvance();
				
				if(text[i] == '\n')
				{
					continue;
				}
			}
			
			currWidth += fci.horizAdv;
		}

		return Vec2f(currWidth, currHeight);
	}

	Vec2f Font::getCursorLocation(std::wstring text, size_t charIndex, int maxWidth)
	{
		int currWidth = 0;
		int currHeight = 0;
		
		size_t actualSize = MathExt::clamp<size_t>(charIndex, 0, text.size());

		for(size_t i=0; i<actualSize; i++)
		{
			size_t charIndex = this->getCharIndex(text[i]);
			FontCharInfo fci = this->getFontCharInfo(charIndex);

			int newWidth = currWidth + fci.width;
			if((newWidth >= maxWidth && maxWidth > 0) || text[i] == L'\n')
			{
				currWidth = 0;
				currHeight += this->getVerticalAdvance();
				
				if(text[i] == L'\n')
				{
					continue;
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