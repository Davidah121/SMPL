#include "Font.h"
#include "SimpleFile.h"
#include "StringTools.h"

const Class* Font::myClass = new Class("Font", {Object::myClass});
const Class* Font::getClass()
{
	return Font::myClass;
}

Font::Font(std::string filename)
{
	//Load an .ft file
	//Rough Format
	//Name of font
	//Antialiased | Bold
	//size of font
	//location and name of image
	//-Separator-
	//char number, x, y, width, height
	//......
	//endl;

	//We won't store the font name, and attributes
	int index = filename.find_last_of('/');
	std::string dir = filename.substr(0, index);

	SimpleFile file = SimpleFile(filename, SimpleFile::READ);
	if (file.isOpen())
	{
		std::vector<std::string> fileInfo = file.readFullFileString();
		file.close();
		int fontSize = StringTools::toInt(fileInfo[2]);

		index = fileInfo[3].find_last_not_of("\\");
		std::string imageFile = fileInfo[3].substr(0, index);

		img = Image::loadImage(imageFile, NULL)[0];

		int startIndex = 5;
		while (startIndex < fileInfo.size())
		{
			if (fileInfo[startIndex] == "endl;")
			{
				break;
			}
			else
			{
				std::vector<std::string> splitString = StringTools::splitString(fileInfo[startIndex], ",");
				//char num, x, y, width, height
				charNum.push_back(StringTools::toInt(splitString[0]));
				charX.push_back(StringTools::toInt(splitString[1]));
				charY.push_back(StringTools::toInt(splitString[2]));
				charWidth.push_back(StringTools::toInt(splitString[3]));
				charHeight.push_back(StringTools::toInt(splitString[4]));
			}
			startIndex++;
		}

		//pngs have explicit alpha were gifs have 1 alpha value and bmp
		//generally will not have alpha. when they do, it is explicit
		if ( imageFile.substr(imageFile.size() - 3, 3) != "png" )
		{
			Color* imgPixels = img->getPixels();
			Color* endPixels = img->getPixels() + img->getWidth() * img->getHeight();

			while (imgPixels < endPixels)
			{
				(*imgPixels).alpha = (*imgPixels).red;
				(*imgPixels).red = 255;
				(*imgPixels).green = 255;
				(*imgPixels).blue = 255;

				imgPixels++;
			}
		}
	}
	else
	{
		//not a valid file
	}
}

Font::~Font()
{
	delete img;
}

int Font::getCharX(int c)
{
	int index = getCharIndex(c);

	if (index >= 0 && index < charX.size())
	{
		return charX[index];
	}
	return -1;
}

int Font::getCharY(int c)
{
	int index = getCharIndex(c);

	if (index >= 0 && index < charY.size())
	{
		return charY[index];
	}
	return -1;
}

int Font::getCharWidth(int c)
{
	int index = getCharIndex(c);

	if (index >= 0 && index < charWidth.size())
	{
		return charWidth[index];
	}
	return -1;
}

int Font::getCharHeight(int c)
{
	int index = getCharIndex(c);

	if (index >= 0 && index < charHeight.size())
	{
		return charHeight[index];
	}
	return -1;
}

Image* Font::getImage()
{
	return img;
}

int Font::getCharIndex(int c)
{
	int index = -1;
	for (int i = 0; i < charNum.size(); i++)
	{
		if (charNum[i] == c)
		{
			index = i;
			break;
		}
	}
	return index;
}
