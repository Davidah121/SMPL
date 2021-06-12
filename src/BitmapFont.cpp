#include "BitmapFont.h"
#include "SimpleFile.h"
#include "StringTools.h"
#include "SimpleXml.h"

const Class BitmapFont::myClass = Class("BitmapFont", {&Font::myClass});
const Class* BitmapFont::getClass()
{
	return &BitmapFont::myClass;
}

BitmapFont::BitmapFont(std::string filename) : Font()
{
	size_t lastDot = filename.find_last_of('.');

	std::string extension = filename.substr(lastDot, filename.size()-lastDot);
	if(extension == ".ft")
	{
		loadFT(filename);
	}
	else if(extension == ".fnt")
	{
		loadFNT(filename);
	}
	else
	{
		//error
		#ifdef USE_EXCEPTIONS
		throw BitmapFont::InvalidFileFormat;
		#endif
	}
}

BitmapFont::~BitmapFont()
{
}

Image* BitmapFont::getImage(int index)
{
	if(index>=0 && index<imgPage.size())
		return img.getImage( imgPage[index] );
	else
	{
		#ifdef USE_EXCEPTIONS
		throw OutOfBoundsError();
		#endif
	}

	return nullptr;
}

void BitmapFont::loadFT(std::string filename)
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

	std::vector<std::string> fileInfo;

	try
	{
		/* code */
		SimpleFile file = SimpleFile(filename, SimpleFile::READ);
		if(file.isOpen())
		{
			fileInfo = file.readFullFileString();
		}
		file.close();
	}
	catch(SimpleFile::FileOpenErrorException& e)
	{
		//could not open file or an error occured while reading.
		std::cerr << e.what() << '\n';
		return;
	}
	
	
	if (fileInfo.size()>0)
	{
		int fontSize = StringTools::toInt(fileInfo[2]);
		std::string imageFile = dir + "/" + fileInfo[3];
		int amountOfImages = 0;
		Image** tempImgPointer = Image::loadImage(imageFile, &amountOfImages);
		if(amountOfImages>0)
		{
			img.dispose();
			img.addImage(tempImgPointer[0]);
		}
		else
		{
			//StringTools::println("%s was not loaded", imageFile.c_str());
			delete[] tempImgPointer;
			return;
		}
		
		delete[] tempImgPointer;
		

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
				if(splitString.size() == 5)
				{
					//char num, x, y, width, height
					FontCharInfo fc;
					fc.unicodeValue = StringTools::toInt(splitString[0]);
					fc.x = StringTools::toInt(splitString[1]);
					fc.y = StringTools::toInt(splitString[2]);
					fc.width = StringTools::toInt(splitString[3]);
					fc.height = StringTools::toInt(splitString[4]);
					fc.horizAdv = fc.width-1;
					fc.xOffset = 0;
					fc.yOffset = 0;
					charInfoList.push_back(fc);
					imgPage.push_back(0);
				}
				else
				{
					//invalid split size
					#ifdef USE_EXCEPTIONS
					throw BitmapFont::InvalidFileFormat();
					#endif
					break;
					//StringTools::println("Invalid string: %s| with size: %d", fileInfo[startIndex].c_str(), fileInfo.size());
				}
				
			}
			startIndex++;
		}

		//pngs have explicit alpha, gifs have 1 alpha value
		//and bmp will not have alpha. Treat the intensity as the alpha.
		//should change the format to YCaCb to do this later so colors still work.
		if ( imageFile.substr(imageFile.size() - 3, 3) != "png" && imageFile.substr(imageFile.size() - 3, 3) != "gif")
		{
			Image* tImg = img.getImage(0);
			Color* imgPixels = tImg->getPixels();
			Color* endPixels = tImg->getPixels() + tImg->getWidth() * tImg->getHeight();

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

}

void BitmapFont::loadFNT(std::string filename)
{
	//note, expects xml format
	SimpleXml file = SimpleXml();
	bool valid = file.load(filename);

	int index = filename.find_last_of('/');
	int index2 = filename.find_last_of('\\');
	int finalIndex = 0;

	if(index < filename.size() && index2 < filename.size())
	{
		finalIndex = MathExt::max(index, index2);
	}
	else if(index < filename.size())
	{
		finalIndex = index;
	}
	else
	{
		finalIndex = index2;
	}

	std::string path = filename.substr(0, finalIndex+1);

	if(valid)
	{
		XmlNode* root = nullptr;
		for(XmlNode* n : file.nodes)
		{
			if(n->title == L"font")
			{
				root = n;
				break;
			}
		}

		if(root==nullptr)
		{
			//invalid
			#ifdef USE_EXCEPTIONS
			throw InvalidFileFormat();
			#endif
			return;
		}

		for(XmlNode* n : root->childNodes)
		{
			if(n->title == L"info")
			{
				for(XmlAttribute attrib : n->attributes)
				{
					if(attrib.name == L"size")
					{
						fontSize = std::stoi(attrib.value);
					}
				}
			}
			else if(n->title == L"common")
			{
				for(XmlAttribute attrib : n->attributes)
				{
					if(attrib.name == L"lineHeight")
					{
						verticalAdv = std::stoi(attrib.value);
					}
				}
			}
			else if(n->title == L"pages")
			{
				for(XmlNode* n2 : n->childNodes)
				{
					for(XmlAttribute attrib : n2->attributes)
					{
						if(attrib.name == L"file")
						{
							int imgCount = 0;
							//Is local image so full path is needed
							std::wstring actualFile = StringTools::toWideString(path) + attrib.value;

							Image** imgArr = Image::loadImage(actualFile, &imgCount);
							if(imgCount>0)
							{
								img.addImage( imgArr[0] );
							}
							delete[] imgArr;
						}
					}
				}
			}
			else if(n->title == L"chars")
			{
				for(XmlNode* n2 : n->childNodes)
				{
					FontCharInfo fci;
					int page = 0;

					for(XmlAttribute attrib : n2->attributes)
					{
						if(attrib.name == L"id")
						{
							fci.unicodeValue = StringTools::toInt(attrib.value);
						}
						else if(attrib.name == L"x")
						{
							fci.x = StringTools::toInt(attrib.value);
						}
						else if(attrib.name == L"y")
						{
							fci.y = StringTools::toInt(attrib.value);
						}
						else if(attrib.name == L"width")
						{
							fci.width = StringTools::toInt(attrib.value);
						}
						else if(attrib.name == L"height")
						{
							fci.height = StringTools::toInt(attrib.value);
						}
						else if(attrib.name == L"xadvance")
						{
							fci.horizAdv = StringTools::toInt(attrib.value);
						}
						else if(attrib.name == L"page")
						{
							page = StringTools::toInt(attrib.value);
						}
						else if(attrib.name == L"xoffset")
						{
							fci.xOffset = StringTools::toInt(attrib.value);
						}
						else if(attrib.name == L"yoffset")
						{
							fci.yOffset = StringTools::toInt(attrib.value);
						}
					}

					charInfoList.push_back(fci);
					imgPage.push_back(page);
				}
			}
		}
	}
	else
	{
		//invalid
		#ifdef USE_EXCEPTIONS
		throw InvalidFileFormat();
		#endif
		return;
	}
}
