#include "Font.h"
#include "SimpleFile.h"
#include "StringTools.h"

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

void Font::setFontTransform(Mat4f mat)
{
	fontTransform = mat;
}

Mat4f Font::getFontTransform()
{
	return fontTransform;
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
