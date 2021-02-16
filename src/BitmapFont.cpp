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
		std::string imageFile = dir + "/" + fileInfo[3];
		int amountOfImages = 0;
		Image** tempImgPointer = Image::loadImage(imageFile, &amountOfImages);
		if(amountOfImages>0)
		{
			StringTools::println("Image loaded");
			img = tempImgPointer[0];
		}
		else
		{
			StringTools::println("%s was not loaded", imageFile.c_str());
		}
		

		int startIndex = 5;
		while (startIndex < fileInfo.size())
		{
			if (fileInfo[startIndex] == "endl;")
			{
				StringTools::println("End of file");
				break;
			}
			else
			{
				std::vector<std::string> splitString = StringTools::splitString(fileInfo[startIndex], ",");
				if(splitString.size() == 5)
				{
					//char num, x, y, width, height
					FontCharInfo fc;
					fc.unicodeValue = StringTools::toInt(splitString[0]);
					fc.x = StringTools::toInt(splitString[1]);
					fc.y = StringTools::toInt(splitString[2]);
					fc.width = StringTools::toInt(splitString[3]);
					fc.height = StringTools::toInt(splitString[4]);
					fc.horizAdv = fc.width;
					charInfoList.push_back(fc);
				}
				else
				{
					StringTools::println("Invalid string: %s| with size: %d", fileInfo[startIndex].c_str(), fileInfo.size());
				}
				
			}
			startIndex++;
		}

		//pngs have explicit alpha, gifs have 1 alpha value
		//and bmp will not have alpha. Treat the intensity as the alpha.
		//should change the format to YCaCb to do this later so colors still work.
		if ( imageFile.substr(imageFile.size() - 3, 3) != "png" && imageFile.substr(imageFile.size() - 3, 3) != "gif")
		{
			
			StringTools::println("Not png or gif");
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