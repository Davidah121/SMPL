#include "BitmapFont.h"
#include "SimpleFile.h"
#include "StringTools.h"

const Class* BitmapFont::myClass = new Class("BitmapFont", {Object::myClass});
const Class* BitmapFont::getClass()
{
	return BitmapFont::myClass;
}

BitmapFont::BitmapFont(std::string filename)
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
				FontCharInfo fc;
				fc.unicodeValue = StringTools::toInt(splitString[0]);
				fc.x = StringTools::toInt(splitString[1]);
				fc.y = StringTools::toInt(splitString[2]);
				fc.width = StringTools::toInt(splitString[3]);
				fc.height = StringTools::toInt(splitString[4]);
				charInfoList.push_back(fc);
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

BitmapFont::~BitmapFont()
{
	delete img;
}

Image* BitmapFont::getImage(int index)
{
	return img;
}