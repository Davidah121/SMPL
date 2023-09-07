#include "BitmapFont.h"
#include "SimpleFile.h"
#include "StringTools.h"
#include "SimpleXml.h"
#include "SimpleGraphics.h"

namespace glib
{

	const Class BitmapFont::globalClass = Class("BitmapFont", {&Font::globalClass});

	BitmapFont::BitmapFont(File file) : Font()
	{
		setClass(globalClass);
		init( file );
	}

	void BitmapFont::init(File file)
	{
		std::string extension = file.getExtension();
		if(extension == ".ft")
		{
			loadFT(file);
		}
		else if(extension == ".fnt")
		{
			loadFNT(file);
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

	Image* BitmapFont::getImage(size_t index)
	{
		if(index>=0 && index<imgPage.size())
		{
			if(fontSize == originalFontSize)
			{
				return img.getImage( imgPage[index] );
			}
			else
			{
				double scale = fontSize / originalFontSize;
				return SimpleGraphics::scaleImage( img.getImage( imgPage[index] ), scale, scale, SimpleGraphics::NEAREST_NEIGHBOR_FILTER);
			}
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			throw OutOfBoundsError();
			#endif
		}

		return nullptr;
	}

	Sprite* BitmapFont::getSprite()
	{
		return &img;
	}

	std::vector<size_t> BitmapFont::getImagePages()
	{
		return imgPage;
	}

	void BitmapFont::loadFT(File file)
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
		std::string dir = file.getPath();

		std::vector<std::string> fileInfo;

		try
		{
			/* code */
			SimpleFile f = SimpleFile(file, SimpleFile::READ);
			if(f.isOpen())
			{
				fileInfo = f.readFullFileString();
			}
			f.close();
		}
		catch(SimpleFile::FileOpenErrorException& e)
		{
			//could not open file or an error occured while reading.
			std::cerr << e.what() << '\n';
			return;
		}
		
		
		if (fileInfo.size()>0)
		{
			// int fontSize = StringTools::toInt(fileInfo[2]); //Not used

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
			

			size_t startIndex = 5;
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

						this->addChar(fc);
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

			this->sortList(false);

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

	void BitmapFont::loadFNT(File file)
	{
		//note, expects xml format
		SimpleXml xmlData = SimpleXml();
		bool valid = xmlData.load(file);

		std::string path = file.getPath();

		if(valid)
		{
			XmlNode* root = nullptr;

			for(XmlNode* n : xmlData.getNodes())
			{
				if(n->getTitle() == "font")
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

			for(ChildNode& c : root->getChildNodes())
			{
				if(c.type != ChildNode::TYPE_NODE)
					continue;
				XmlNode* n = c.node;
				if(n->getTitle() == "info")
				{
					//Can simplify
					auto temp = n->getAttribute("size");
					if(temp != nullptr)
					{
						fontSize = abs(std::stoi(temp->data));
						originalFontSize = fontSize;
					}
				}
				else if(n->getTitle() == "common")
				{
					auto temp = n->getAttribute("lineheight");
					if(temp != nullptr)
						verticalAdv = abs(std::stoi(temp->data));
					
				}
				else if(n->getTitle() == "pages")
				{
					for(ChildNode& c2 : n->getChildNodes())
					{
						if(c2.type != ChildNode::TYPE_NODE)
							continue;
						XmlNode* n2 = c2.node;

						auto temp = n2->getAttribute("file");
						if(temp != nullptr)
						{
							int imgCount = 0;
							//Is local image so full path is needed
							std::string actualFile = path + '/' + temp->data;

							Image** imgArr = Image::loadImage(actualFile, &imgCount);
							if(imgCount>0)
							{
								img.addImage( imgArr[0] );
							}
							delete[] imgArr;
						}
					}
				}
				else if(n->getTitle() == "chars")
				{
					for(ChildNode& c2 : n->getChildNodes())
					{
						if(c2.type != ChildNode::TYPE_NODE)
							continue;
						XmlNode* n2 = c2.node;
						FontCharInfo fci;
						int page = 0;

						auto attrib = n2->getAttribute("id");
						if(attrib != nullptr)
							fci.unicodeValue = StringTools::toInt(attrib->data);
						
						attrib = n2->getAttribute("x");
						if(attrib != nullptr)
							fci.x = StringTools::toInt(attrib->data);
						
						attrib = n2->getAttribute("y");
						if(attrib != nullptr)
							fci.y = StringTools::toInt(attrib->data);
						
						attrib = n2->getAttribute("width");
						if(attrib != nullptr)
							fci.width = StringTools::toInt(attrib->data);
						
						attrib = n2->getAttribute("height");
						if(attrib != nullptr)
							fci.height = StringTools::toInt(attrib->data);
						
						attrib = n2->getAttribute("xadvance");
						if(attrib != nullptr)
							fci.horizAdv = StringTools::toInt(attrib->data);
						
						attrib = n2->getAttribute("page");
						if(attrib != nullptr)
							page = StringTools::toInt(attrib->data);
						
						attrib = n2->getAttribute("xoffset");
						if(attrib != nullptr)
							fci.xOffset = StringTools::toInt(attrib->data);
						
						attrib = n2->getAttribute("yoffset");
						if(attrib != nullptr)
							fci.yOffset = StringTools::toInt(attrib->data);

						this->addChar(fci);
						imgPage.push_back(page);
					}
				}
			}

			this->sortList(false);
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

} //NAMESPACE glib END