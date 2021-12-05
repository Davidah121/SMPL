#include "Font.h"
#include "SimpleFile.h"
#include "StringTools.h"
#include "Sort.h"

namespace glib
{
		
	const Class Font::myClass = Class("Font", {&Object::myClass});
	const Class* Font::getClass()
	{
		return &Font::myClass;
	}

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

	FontCharInfo Font::getFontCharInfo(int c)
	{
		int index = getCharIndex(c);

		if (index >= 0 && index < charInfoList.size())
		{
			return charInfoList[index];
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
		return verticalAdv;
	}

	int Font::getWidthOfString(std::string text)
	{
		int totalWidth = 0;
		for(int i=0; i<text.size(); i++)
		{
			FontCharInfo fci = getFontCharInfo(text[i]);
			totalWidth += fci.horizAdv;
		}

		return totalWidth;
	}

	int Font::getWidthOfString(std::wstring text)
	{
		int totalWidth = 0;
		for(int i=0; i<text.size(); i++)
		{
			FontCharInfo fci = getFontCharInfo(text[i]);
			totalWidth += fci.horizAdv;
		}

		return totalWidth;
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