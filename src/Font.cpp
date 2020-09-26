#include "Font.h"
#include "SimpleFile.h"
#include "StringTools.h"

const Class* Font::myClass = new Class("Font", {Object::myClass});
const Class* Font::getClass()
{
	return Font::myClass;
}

Font::Font()
{
}

Font::~Font()
{
}

FontCharInfo Font::getFontCharInfo(int c)
{
	int index = getCharIndex(c);

	if (index >= 0 && index < charInfoList.size())
	{
		return charInfoList[index];
	}
	return {};
}

std::vector<FontCharInfo> Font::getListOfFontCharInfo()
{
	return charInfoList;
}

int Font::getCharIndex(int c)
{
	int index = -1;
	for (int i = 0; i < charInfoList.size(); i++)
	{
		if (charInfoList[i].unicodeValue == c)
		{
			index = i;
			break;
		}
	}
	return index;
}
