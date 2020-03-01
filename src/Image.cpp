#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "Graphics.h"
#include <iostream>
#include "StringTools.h"

#define min(a,b) ((a<b) ? a:b)

const Class* Image::myClass = new Class("Image", {Object::myClass});
const Class* Image::getClass()
{
	return Image::myClass;
}

Image::Image()
{
	width = 0;
	height = 0;
	pixels = nullptr;
	
}

Image::Image(int width, int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width * height];
}

Image::Image(const Image& other)
{
	this->~Image();

	this->width = other.width;
	this->height = other.height;
	pixels = new Color[width * height];

	memcpy(pixels, other.pixels, width * height * sizeof(Color));
	/*
	Color* temp = pixels;
	Color* tempEnd = pixels + (width * height);
	Color* otherTemp = other.pixels;

	while (temp < tempEnd)
	{
		*temp = *otherTemp;
		temp++;
		otherTemp++;
	}
	*/

	p = other.p;
}


Image::~Image()
{
	if(pixels!=nullptr)
		delete[] pixels;

	p.~ColorPalette();
}

int Image::getWidth()
{
	return width;
}

int Image::getHeight()
{
	return height;
}

Color* Image::getPixels()
{
	return pixels;
}

Color Image::getPixel(int x, int y)
{
	if (x >= 0 && x < width)
	{
		if (y >= 0 && y < height)
		{
			return pixels[y * width + x];
		}
	}
	return Color();
}

void Image::setPixel(int x, int y, Color c)
{
	if (x >= 0 && x < width)
	{
		if (y >= 0 && y < height)
		{
			//Add some alpha blending
			//finalRGB = thisRGB * thisAlpha + baseRGB
			//finalAlpha = thisAlpha + baseAlpha

			float blend = (float)c.alpha / 255;
			Color baseCol = pixels[y * width + x];

			int red = ((float)baseCol.red*(1.0-blend) + (float)c.red * blend);
			int green = ((float)baseCol.green * (1.0 - blend) + (float)c.green * blend);
			int blue = ((float)baseCol.blue * (1.0 - blend) + (float)c.blue * blend);
			int alpha = baseCol.alpha + c.alpha;
			
			Color nCol = { (unsigned char)min(red, 255),
							(unsigned char)min(green, 255),
							(unsigned char)min(blue, 255), 
							(unsigned char)min(alpha, 255) };

			pixels[y * width + x] = nCol;
		}
	}
}

void Image::setAllPixels(Color c)
{
	Color* start = pixels;
	Color* end = pixels + (width * height);
	while (start < end)
	{
		*start = c;
		start++;
	}
}

void Image::copyImage(Image* v)
{
	width = v->getWidth();
	height = v->getHeight();
	p = v->getPalette();

	Color* otherPixels = v->getPixels();
	Color* thesePixels = pixels;
	Color* endPixels = pixels + (width * height);

	for (thesePixels = pixels; thesePixels < endPixels; thesePixels++, otherPixels++)
	{
		*thesePixels = *otherPixels;
	}
}

void Image::setPalette(ColorPalette p)
{
	this->p = p;
}

ColorPalette Image::getPalette()
{
	return p;
}

Image** Image::loadImage(std::string filename, int* amountOfImages)
{
	SimpleFile file(filename, SimpleFile::READ);

	if (file.isOpen())
	{
		std::vector<unsigned char> fileData = file.readFullFileAsBytes();

		file.close();

		std::string fileType = filename.substr(filename.size() - 3, 3);
		std::string fileType2 = filename.substr(filename.size() - 4, 4);

		if (fileType == "bmp")
		{
			if (amountOfImages != nullptr)
				*amountOfImages = 1;

			return loadBMP(fileData, amountOfImages);
		}
		else if (fileType == "gif")
		{
			return loadGIF(fileData, amountOfImages);
		}
		else if (fileType == "png")
		{
			return loadPNG(fileData, amountOfImages);
		}
		else if (fileType == "jpg" || fileType2 == "jpeg")
		{
			return loadJPG(fileData, amountOfImages);
		}

		return nullptr;
	}
	else
	{
		if(amountOfImages!=nullptr)
			*amountOfImages = 0;

		return nullptr;
	}
}

Image** Image::loadImage(std::wstring filename, int* amountOfImages)
{
	SimpleFile file(filename, SimpleFile::READ);

	if (file.isOpen())
	{
		std::vector<unsigned char> fileData = file.readFullFileAsBytes();

		file.close();

		std::wstring fileType = filename.substr(filename.size() - 3, 3);
		std::wstring fileType2 = filename.substr(filename.size() - 4, 4);

		if (fileType == L"bmp")
		{
			if (amountOfImages != nullptr)
				* amountOfImages = 1;

			return loadBMP(fileData, amountOfImages);
		}
		else if (fileType == L"gif")
		{
			return loadGIF(fileData, amountOfImages);
		}
		else if (fileType == L"png")
		{
			return loadPNG(fileData, amountOfImages);
		}
		else if (fileType == L"jpg" || fileType2 == L"jpeg")
		{
			return loadJPG(fileData, amountOfImages);
		}
	}
	else
	{
		if (amountOfImages != nullptr)
			* amountOfImages = 0;

		return nullptr;
	}

	return nullptr;
}

void Image::clearImage()
{
	Graphics::clearImage(this);
}

void Image::drawRect(int x, int y, int x2, int y2, bool outline)
{
	Graphics::drawRect(x, y, x2, y2, outline, this);
}

void Image::drawLine(int x, int y, int x2, int y2)
{
	Graphics::drawLine(x, y, x2, y2, this);
}

void Image::drawCircle(int x, int y, int radius, bool outline)
{
	Graphics::drawCircle(x, y, radius, outline, this);
}

void Image::drawSprite(Image* img, int x, int y)
{
	Graphics::drawSprite(img, x, y, this);
}

void Image::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH)
{
	Graphics::drawSpritePart(img, x, y, imgX, imgY, imgW, imgH, this);
}

void Image::drawText(std::string str, int x, int y)
{
	Graphics::drawText(str, x, y, this);
}

#pragma region BITMAP_CODE

Image** Image::loadBMP(std::vector<unsigned char> fileData, int* amountOfImages)
{
	Image** images = nullptr;
	//Assume that the bitmap is a valid bitmap
	
	int pixStart = ((int)fileData[13] << 24) + ((int)fileData[12]<<16)
		+((int)fileData[11]<<8) + ((int)fileData[10]);

	int width = ((int)fileData[21] << 24) + ((int)fileData[20] << 16)
		+ ((int)fileData[19] << 8) + ((int)fileData[18]);

	int height = ((int)fileData[25] << 24) + ((int)fileData[24] << 16)
		+ ((int)fileData[23] << 8) + ((int)fileData[22]);

	int bpp = ((int)fileData[29] << 8) + ((int)fileData[28]);

	int compressionMethod = ((int)fileData[33] << 24) + ((int)fileData[32] << 16)
		+ ((int)fileData[31] << 8) + ((int)fileData[30]);

	int colorsInPalette = ((int)fileData[49] << 24) + ((int)fileData[48] << 16)
		+ ((int)fileData[47] << 8) + ((int)fileData[46]);


	//Load palette if it exists

	ColorPalette cPalette = ColorPalette();

	if (colorsInPalette > 0)
	{
		int paletteStart = 54;

		for (int i = 0; i < colorsInPalette; i++)
		{
			int refLoc = paletteStart + (4 * i);
			
			cPalette.addNewColor({ (unsigned char)fileData[refLoc + 2], (unsigned char)fileData[refLoc + 1], (unsigned char)fileData[refLoc], (unsigned char)fileData[refLoc + 3] });
		}
	}

	std::vector<unsigned char> uncompressedData = std::vector<unsigned char>();
	unsigned char* pixData = fileData.data() + pixStart;
	int pixsDataSize = fileData.size() - pixStart;

	//Handle Compression if it is RLE
	if (compressionMethod == 1)
	{
		//RLE for 8 bit images
		//This works like normal so you can use the normal method.
		int i = pixStart;

		while (i < fileData.size()-1)
		{
			int run = fileData[i];
			int value = fileData[i + 1];

			for (int k = 0; k < run; k++)
			{
				uncompressedData.push_back(value);
			}

			i += 2;
		}
		
		pixData = uncompressedData.data();
		pixsDataSize = uncompressedData.size();

	}
	else if (compressionMethod == 2)
	{
		//RLE for 4 bit images
		//Have to do a bit of conversion to get the data like you expect.
		int i = pixStart;

		int shift = 4;

		
		while (i < fileData.size()-1)
		{
			int run = fileData[i];
			int value = fileData[i + 1]>>4;

			int k = 0;
			while(k<run)
			{
				if (shift == 4)
				{
					uncompressedData.push_back(value<<4);
					shift = 0;
				}
				else
				{
					uncompressedData[uncompressedData.size() - 1] += value;
					shift = 4;
				}
				k++;
			}
			
			i += 2;
		}

		pixData = uncompressedData.data();
		pixsDataSize = uncompressedData.size();
	}
	else if (compressionMethod == 0)
	{
		//Nothing
	}
	else
	{
		//can't be loaded
		return nullptr;
	}

	//Load pixels
	Image* tImg = new Image(width, height);

	if (bpp == 1)
	{
		//must have a palette. Could need padding
		int padding = (width / 8) % 4;

		int i = 0;
		int x = 0;
		int y = height - 1;

		while (i < pixsDataSize)
		{
			for (int k = 0; k < 8; k++)
			{
				if (x < width)
				{
					int v1 = (pixData[i] >> (7 - i)) & 0b00000001;
					Color setC = cPalette.getColor(v1);
					tImg->pixels[y * width + x] = setC;
					x++;
				}
				else
				{
					break;
				}
			}

			if (x >= width)
			{
				y--;
				i += padding;
				x = 0;
			}
			i += 1;
		}
	}
	else if (bpp == 2)
	{
		//must have a palette. Could need padding
		int padding = (width / 4) % 4;

		int i = 0;
		int x = 0;
		int y = height - 1;

		while (i < pixsDataSize)
		{
			for (int k = 0; k < 4; k++)
			{
				if (x < width)
				{
					int v1 = (pixData[i] >> (6 - k * 2)) & 0b00000011;
					Color setC = cPalette.getColor(v1);
					tImg->pixels[y * width + x] = setC;
					x++;
				}
				else
				{
					break;
				}
			}

			if (x >= width)
			{
				y--;
				i += padding;
				x = 0;
			}
			i += 1;
		}
	}
	else if (bpp == 4)
	{
		//must have a palette. Could need padding
		int padding = (width / 2) % 4;

		int i = 0;
		int x = 0;
		int y = height - 1;

		while (i < pixsDataSize)
		{
			for (int k = 0; k < 2; k++)
			{
				if (x < width)
				{
					int v1 = (pixData[i] >> (4 - k * 4)) & 0b00001111;
					Color setC = cPalette.getColor(v1);
					tImg->pixels[y * width + x] = setC;
					x++;
				}
				else
				{
					break;
				}
			}

			if (x >= width)
			{
				y--;
				i += padding;
				x = 0;
			}
			i += 1;
		}
	}
	else if (bpp == 8)
	{
		//must have a palette. Could need padding
		int padding = width % 4;

		int i = 0;
		int x = 0;
		int y = height - 1;

		while (i < pixsDataSize)
		{
			Color setC = cPalette.getColor(pixData[i]);
			tImg->pixels[y * width + x] = setC;

			x++;
			if (x >= width)
			{
				y--;
				i += padding;
				x = 0;
			}
			i += 1;
		}
	}
	else if (bpp == 16)
	{
		//Less likely. Could need padding.
		//Supports alpha
		//No compression
		//Could have a palette, but not sure.
		int padding = (width * 2) % 4;

		int i = 0;
		int x = 0;
		int y = height - 1;

		while (i < pixsDataSize)
		{
			unsigned char highWord = pixData[i];
			unsigned char lowWord = pixData[i + 1];

			tImg->pixels[y * width + x].blue = (highWord >> 8 & 0xFF);
			tImg->pixels[y * width + x].green = (highWord & 0xFF);
			tImg->pixels[y * width + x].red = (lowWord >> 8 & 0xFF);
			tImg->pixels[y * width + x].alpha = (lowWord & 0xFF);

			x++;
			if (x >= width)
			{
				y--;
				i += padding;
				x = 0;
			}
			i += 2;
		}
	}
	else if (bpp == 24)
	{
		//Normal one. Could need padding
		//No compression
		//No palette
		int padding = (width * 3) % 4;

		int i = 0;
		int x = 0;
		int y = height - 1;

		while (i < pixsDataSize)
		{
			tImg->pixels[y * width + x].blue = pixData[i];
			tImg->pixels[y * width + x].green = pixData[i + 1];
			tImg->pixels[y * width + x].red = pixData[i + 2];

			x++;
			if (x >= width)
			{
				y--;
				i += padding;
				x = 0;
			}
			i += 3;
		}
	}
	else if (bpp == 32)
	{
		//Not likely to ever appear. No padding.
		//Supports alpha.
		//No compression
		//No palette
		int padding = 0;

		int i = 0;
		int x = 0;
		int y = height - 1;

		while (i < pixsDataSize)
		{
			tImg->pixels[y * width + x].blue = pixData[i];
			tImg->pixels[y * width + x].green = pixData[i + 1];
			tImg->pixels[y * width + x].red = pixData[i + 2];
			tImg->pixels[y * width + x].alpha = pixData[i + 3];

			x++;
			if (x >= width)
			{
				y--;
				i += padding;
				x = 0;
			}
			i += 4;
		}
	}

	if(amountOfImages!=nullptr)
		*amountOfImages = 1;

	images = new Image * [*amountOfImages]{ tImg };
	return images;
	
}

#pragma endregion

#pragma region GIF_CODE

Image** Image::loadGIF(std::vector<unsigned char> fileData, int* amountOfImages)
{
	std::vector<Image*>* images = new std::vector<Image*>();

	ColorPalette globalColorTable = ColorPalette();

	std::string gifType = "";
	gifType += fileData[0];	gifType += fileData[1];	gifType += fileData[2];
	gifType += fileData[3];	gifType += fileData[4];	gifType += fileData[5];

	bool valid = false; 
	
	if (gifType == "GIF87a" || gifType == "GIF89a")
	{
		valid = true;
	}
	
	if (valid == true)
	{
		int globalWidth = (int)fileData[6] + ((int)fileData[7] << 8);
		int globalHeight = (int)fileData[8] + ((int)fileData[9] << 8);
		
		bool hasGlobalColorTable = (fileData[10] >> 7) == 1;
		int colorRes = ((fileData[10] >> 4) & 0b00000111) + 1;
		bool globalTableSorted = ((fileData[10] >> 3) & 0b00000001) == 1;
		int sizeOfGlobalTable = 2 << (fileData[10] & 0b00000111);

		int backgroundColorIndex = fileData[11];
		int pixelAspectRatio = fileData[12]; //Not used

		int startIndex = 13;
		if (hasGlobalColorTable)
		{

			for (int i = 0; i < sizeOfGlobalTable; i++)
			{
				int tempIndex = i * 3;
				Color c = { fileData[startIndex + tempIndex], fileData[startIndex + tempIndex + 1], fileData[startIndex + tempIndex + 2], 255 };

				globalColorTable.getPaletteRef()->push_back(c);
			}

			startIndex += sizeOfGlobalTable * 3;
		}

		int transparentColorIndex = -1;

		while (startIndex < fileData.size())
		{
			if (fileData[startIndex] == 0x2C)
			{
				Image* tempImage = new Image(globalWidth, globalHeight);

				if (images->size() > 0)
				{
					tempImage->copyImage(images->at(images->size() - 1));
				}
				else
				{
					if (backgroundColorIndex >= 0 && backgroundColorIndex < sizeOfGlobalTable && hasGlobalColorTable==true)
					{
						tempImage->setAllPixels(globalColorTable.getColor(backgroundColorIndex));
					}
				}

				//Image Descriptor
				//We will consider each Image Descriptor seen as a new Image
				//but we will focus on one image at a time.

				//Note that these are for each individual image descriptor
				//The are separate from the global width and height
				//These allow you to specify different blocks at different
				//locations with different color palettes if you need.

				//Not necessarly a new Image
				int leftScreenPos = (int)fileData[startIndex + 1] + (((int)fileData[startIndex + 2]) << 8);
				int topScreenPos = (int)fileData[startIndex + 3] + (((int)fileData[startIndex + 4]) << 8);

				int lWidth = (int)fileData[startIndex + 5] + (((int)fileData[startIndex + 6]) << 8);
				int lHeight = (int)fileData[startIndex + 7] + (((int)fileData[startIndex + 8]) << 8);

				bool hasLocalTable = ((fileData[startIndex + 9] >> 7) & 0x01) == 1;
				bool interlaced = ((fileData[startIndex + 9] >> 6) & 0x01) == 1;
				bool sorted = ((fileData[startIndex + 9] >> 5) & 0x01) == 1;
				//Skipping reserved field
				int sizeOfLocalTable = 2 << (fileData[startIndex + 9] & 0b00000111);

				startIndex += 10;

				ColorPalette localTable = ColorPalette();

				if (hasLocalTable)
				{

					for (int i = 0; i < sizeOfLocalTable; i++)
					{
						int tempIndex = i * 3;
						Color c = { fileData[startIndex + tempIndex], fileData[startIndex + tempIndex + 1], fileData[startIndex + tempIndex + 2], 255 };

						localTable.getPaletteRef()->push_back(c);
					}

					startIndex += sizeOfLocalTable * 3;
				}
				else
				{
					localTable = globalColorTable;
				}

				int lzwMinCodeSize = fileData[startIndex];
				startIndex++;

				//Start reading the sub blocks
				int x = 0;
				int y = 0;
				std::vector<unsigned char> valuesToDecompress = std::vector<unsigned char>();

				while (true)
				{
					int blockSize = fileData[startIndex];
					startIndex++;

					if (blockSize == 0)
					{
						break;
					}
					else
					{
						for (int i = 0; i < blockSize; i++)
						{
							valuesToDecompress.push_back(fileData[startIndex+i]);
						}
					}

					startIndex += blockSize;
				}

				std::vector<unsigned char> pixValues =
					Compression::decompressLZW(valuesToDecompress, localTable.getSize());

				if (localTable.getSize() != 0)
				{
					int j = 0;
					while (j < pixValues.size())
					{
						int paletteIndex = pixValues[j];
						Color c = localTable.getColor(paletteIndex);
						if (paletteIndex == transparentColorIndex)
						{
							c.alpha = 0;
						}

						tempImage->setPixel(x + leftScreenPos, y + topScreenPos, c);
						j++;

						x++;
						if (x >= lWidth)
						{
							y++;
							x = 0;
						}
					}
				}

				images->push_back(tempImage);
				break;
			}
			else if (fileData[startIndex] == 0x21)
			{
				//Control Extension
				//We only care about the graphics control extension
				if (fileData[startIndex+1] == 0xF9)
				{
					//GCE
					
					int blockSize = fileData[startIndex + 2];
					bool hasTransparentColor = (fileData[startIndex + 3] & 0x01) == 1;
					
					//DelayTime in 1/100 seconds
					int delayTime = (int)fileData[startIndex + 4] + (((int)fileData[startIndex + 5]) << 8);

					transparentColorIndex = fileData[startIndex + 6];

					startIndex += 6;

					for (int i = 4; i < blockSize; i++)
					{
						startIndex++;
					}
					startIndex++;
				}
				else if (fileData[startIndex + 1] == 0xFF)
				{
					startIndex += 13;

					while (fileData[startIndex] != 0x00)
					{
						startIndex++;
					}

					startIndex++;
				}
				else 
				{
					while (fileData[startIndex] != 0x00)
					{
						startIndex++;
					}

					startIndex++;
				}
			}
			else if (fileData[startIndex] == 0x3B)
			{
				//End of file
				break;
			}
			else
			{
				startIndex++;
			}
		}
	}

	if (amountOfImages != nullptr)
		*amountOfImages = images->size();

	return images->data();
}

#pragma endregion

#pragma region PNG_CODE

Image** Image::loadPNG(std::vector<unsigned char> fileData, int* amountOfImages)
{
	Image** images = nullptr;
	
	int fileSize = fileData.size();
	int location = 0;
	//the bytes 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a must appear first
	unsigned char headerCheck[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};

	bool valid = true;
	for(int i=0; i<8; i++)
	{
		valid = (fileData[i+location] == headerCheck[i]);
	}
	location+=8;

	if(valid==false)
	{
		return nullptr;
	}

	Image* tImg = nullptr;
	ColorPalette p = ColorPalette();
	

	if(amountOfImages!=nullptr)
		*amountOfImages = 0;

	int bitDepth = 0;
	int colorType = 0;
	int filterType = 0;
	bool interlace = false;
	Color transparentColor = {0, 0, 0, 255};

	int x=0;
	int y=0;

	std::vector<unsigned char> compressedData;

	//passed header check, now we read blocks
	while(location < fileSize)
	{
		//read 4 bytes for the size of a block
		unsigned int blockSize = 0;
		blockSize = (int)(fileData[location]<<24) + (int)(fileData[location+1]<<16) + (int)(fileData[location+2]<<8) + (int)fileData[location+3];
		location+=4;

		//read 4 bytes for the id of a block
		std::string blockID = {(char)fileData[location], (char)fileData[location+1], (char)fileData[location+2], (char)fileData[location+3]};
		location+=4;
		
		StringTools::out << StringTools::toWideString(blockID) << std::endl;
		//read n bytes for the data
		if(blockID=="IHDR")
		{
			int width = (int)(fileData[location]<<24) + (int)(fileData[location+1]<<16) + (int)(fileData[location+2]<<8) + (int)fileData[location+3];
			int height = (int)(fileData[location+4]<<24) + (int)(fileData[location+5]<<16) + (int)(fileData[location+6]<<8) + (int)fileData[location+7];
			bitDepth = fileData[location+8];
			colorType = fileData[location+9];
			int compressionMethod = fileData[location+10];
			int filterMethod = fileData[location+11];
			interlace = fileData[location+12] == 1;

			if(compressionMethod!=0 || filterMethod!=0)
			{
				//invalid
				valid = false;
				break;
			}

			if(bitDepth==16)
			{
				//invalid for now
				//Doesn't support 16 bitdepth. Maybe later using ColorHiDef
				std::cout << "Does not support 16 bit depth images" << std::endl;
				valid = false;
				break;
			}

			tImg = new Image(width, height);
			
		}
		else if(blockID=="PLTE")
		{
			if(blockSize%3 == 0)
			{
				p.clearPalette();
				std::vector<Color>* palette = p.getPaletteRef();
				for(int i=0; i<blockSize; i+=3)
				{
					Color c;
					c.red = fileData[location+i];
					c.green = fileData[location+i+1];
					c.blue = fileData[location+i+2];
					c.alpha = 255;

					palette->push_back(c);
				}
			}
			else
			{
				//invalid
				valid = false;
				break;
			}
		}
		else if(blockID=="IDAT")
		{
			//Concatinate the IDAT data into one block.
			for(int i=0; i<blockSize; i++)
			{
				compressedData.push_back(fileData[location+i]);
			}
			//skip the ADLER32 bytes
		}
		else if(blockID=="IEND")
		{
			if(amountOfImages!=nullptr)
				*amountOfImages = 1;
			break;
		}
		else if(blockID=="tRNS")
		{
			if(colorType == 0)
			{
				int val = fileData[location+1];
				transparentColor.red = val;
				transparentColor.green = val;
				transparentColor.blue = val;
				transparentColor.alpha = 0;
			}
			else if(colorType == 2)
			{
				transparentColor.red = fileData[location+1];
				transparentColor.green = fileData[location+3];
				transparentColor.blue = fileData[location+5];
				transparentColor.alpha = 0;
			}
			else if(colorType == 3)
			{
				if(p.getSize()<=0)
				{
					//invalid
					valid=false;
					break;
				}
				else
				{
					for(int i=0; i<blockSize; i++)
					{
						p.getColorRef(i)->alpha = fileData[location+i];
					}
				}
			}
		}
		else
		{
			
		}
		
		//move forward by the blocksize
		location+=blockSize;
		
		//read 4 bytes for the checksum
		//gonna skip for now
		location+=4;
	}

	//didn't fail the other necessary parts
	if(valid)
	{
		//Pretty sure it is always a zlib stream
		//read first 2 bytes
		unsigned char CMF = compressedData[0];
		unsigned char FLG = compressedData[1]; //not necessary
		int compressionMethod = CMF & 0b00001111;
		if(compressionMethod!=8)
		{
			//invalid, must be deflate compression method
			valid = false;
			if(amountOfImages!=nullptr)
				*amountOfImages=0;
			delete tImg;
			return nullptr;
		}

		StringTools::out << "Before decompression" << StringTools::lineBreak;
		std::vector<unsigned char> decompressedData = Compression::decompressDeflate(&compressedData.data()[2], compressedData.size()-2);

		StringTools::out << "Decompressed data size: " << decompressedData.size() << StringTools::lineBreak;
		StringTools::out << "BitDepth: " << bitDepth << ", " << "ColorType: " << colorType << StringTools::lineBreak;
		
		//note that when reading different bit depths, they are correctly represented.
		//meaning that the values are packed into bytes correctly.
		//Example, 32 pixels on a scanline with a bit depth of 1 has 4 bytes.

		//defilter the scanline
		int x = 0;
		int y = 0;

		int index = 0;
		int rawIndex = 0;

		int moveBackVal = 1;
		if(bitDepth>8)
		{
			
			moveBackVal = bitDepth/8;
		}
		int scanLineBytes = (tImg->getWidth() * (8.0/bitDepth));

		switch(colorType)
		{
			case 2:
				//3 values per pixel
				moveBackVal*=3;
				scanLineBytes*=3;
				break;
			case 4:
				//2 values per pixel
				moveBackVal*=2;
				scanLineBytes*=2;
				break;
			case 6:
				//4 values per pixel
				moveBackVal*=4;
				scanLineBytes*=4;
				break;
			default:
				break;
		}

		std::vector<unsigned char> preScanLine = std::vector<unsigned char>(scanLineBytes);
		std::vector<unsigned char> scanLine = std::vector<unsigned char>(scanLineBytes);
		
		int imageSize = tImg->getWidth() * tImg->getHeight();

		while(index < decompressedData.size())
		{
			//read first byte in scanline for the filter method
			int filterMethod = decompressedData[index];
			index++;
			
			//StringTools::out << "FilterMethod: " <<filterMethod<<StringTools::lineBreak;

			#pragma region unfilterScanLine

			if(filterMethod==0)
			{
				//do nothing
				while(x < scanLineBytes)
				{
					scanLine[x] = decompressedData[index];
					x++;
					index++;
				}
			}
			else if(filterMethod==1)
			{
				//Subtractive
				//the raw byte is equal to the previous + the current
				while(x < scanLineBytes)
				{
					if(x<moveBackVal)
					{
						scanLine[x] = decompressedData[index];
					}
					else
					{
						scanLine[x] = scanLine[x-moveBackVal] + decompressedData[index];
					}
					x++;
					index++;
				}
			}
			else if(filterMethod==2)
			{
				//Upward
				//the raw byte is equal to the previous upward + the current
				while(x < scanLineBytes)
				{
					if(y==0)
					{
						scanLine[x] = decompressedData[index];
					}
					else
					{
						scanLine[x] = preScanLine[x] + decompressedData[index];
					}
					x++;
					index++;
				}
			}
			else if(filterMethod==3)
			{
				//Average
				//the raw byte is equal to the currentByte + (previous unfiltered + upward unfiltered)/2
				while(x < scanLineBytes)
				{
					if(x<moveBackVal)
					{
						if(y==0)
							scanLine[x] = decompressedData[index];
						else
							scanLine[x] = decompressedData[index] + (preScanLine[x])/2;
					}
					else
					{
						if(y==0)
							scanLine[x] = decompressedData[index] + (scanLine[x-moveBackVal])/2;
						else
							scanLine[x] = decompressedData[index] + (scanLine[x-moveBackVal] + preScanLine[x])/2;
					}
					x++;
					index++;
				}
			}
			else if(filterMethod==4)
			{
				//Paeth
				//the raw byte is equal to the currentByte + PaethPredictor(previous unfiltered, upward unfiltered, up back unfiltered)
				while(x < scanLineBytes)
				{
					int p, a, b, c;
					int pa, pb, pc;

					if(x>=moveBackVal)
						a = scanLine[x-moveBackVal];
					else
						a = 0;
					
					if(y!=0)
						b = preScanLine[x];
					else
						b = 0;

					if(x>=moveBackVal && y!=0)
						c = preScanLine[x-moveBackVal];
					else
						c = 0;
					
					p = a+b+c;
					pa = std::abs(p-a);
					pb = std::abs(p-b);
					pc = std::abs(p-c);
					
					if(pa <= pb && pa <= pc)
					{
						scanLine[x] = decompressedData[index] + a;
					}
					else if(pb <= pc)
					{
						scanLine[x] = decompressedData[index] + b;
					}
					else
					{
						scanLine[x] = decompressedData[index] + c;
					}
					x++;
					index++;
				}
			}

			x=0;
			y++;
			#pragma endregion unfilterScanLine

			#pragma region FillImage

			//Take the unfiltered scanline and put into image
			//For now, only when not interlaced
			Color* rawPixs = tImg->getPixels();
			if(interlace==false)
			{
				if(colorType==0)
				{
					//Greyscale
						//(R)
						//possible bit depths are 1,2,4,8,16

					if(bitDepth<8)
					{
						//Increase Color Amount
						double ICA = 255.0/bitDepth;

						for(int i = 0; i<scanLine.size(); i++)
						{
							for(int bit=0; bit<8; bit+=bitDepth)
							{
								Color c;
								c.red = (unsigned char)(ICA * (scanLine[i]>>(7-bit) & bitDepth) );
								c.green = c.red;
								c.blue = c.red;
								c.alpha = 255;

								rawPixs[rawIndex] = c;
								rawIndex++;
							}
						}
					}
					else if(bitDepth==8)
					{
						for(int i = 0; i<scanLine.size(); i++)
						{
							Color c;
							c.red = scanLine[i];
							c.green = c.red;
							c.blue = c.red;
							c.alpha = 255;

							rawPixs[rawIndex] = c;
							rawIndex++;
						}
					}
				}
				else if(colorType==2)
				{
					//TrueColor
						//(R,G,B)
						//Possible bit depth value are 8, 16
					if(bitDepth==8)
					{
						for(int i = 0; i<scanLine.size(); i+=3)
						{
							Color c;
							c.red = scanLine[i];
							c.green = scanLine[i+1];
							c.blue = scanLine[i+2];
							c.alpha = 255;

							rawPixs[rawIndex] = c;
							rawIndex++;
						}
					}
					
				}
				else if(colorType==3)
				{
					//Palette
						//(P)
						//Possible bit depth value are 1, 2, 4, 8
					int paletteIndex = 0;
					if(bitDepth<8)
					{
						for(int i = 0; i<scanLine.size(); i++)
						{
							for(int bit=0; bit<8; bit+=bitDepth)
							{
								paletteIndex = (scanLine[i]>>(7-bit) & bitDepth);

								Color c = p.getColor(paletteIndex);
								rawPixs[rawIndex] = c;
								rawIndex++;
							}
						}
					}
					else if(bitDepth==8)
					{
						for(int i = 0; i<scanLine.size(); i++)
						{
							paletteIndex = scanLine[i];

							Color c = p.getColor(paletteIndex);
							rawPixs[rawIndex] = c;
							rawIndex++;
						}
					}
				}
				else if(colorType==4)
				{
					//GreyScale With Alpha
						//(R,A)
						//Possible bit depth value are 8, 16
					if(bitDepth==8)
					{
						for(int i = 0; i<scanLine.size(); i+=2)
						{
							Color c;
							c.red = scanLine[i];
							c.green = c.red;
							c.blue = c.red;
							c.alpha = scanLine[i+1];

							rawPixs[rawIndex] = c;
							rawIndex++;
						}
					}
				}
				else if(colorType==6)
				{
					//Truecolor With Alpha
						//(R,G,B,A)
						//Possible bit depth value are 8, 16
					if(bitDepth==8)
					{
						for(int i = 0; i<scanLine.size(); i+=4)
						{
							Color c;
							c.red = scanLine[i];
							c.green = scanLine[i+1];
							c.blue = scanLine[i+2];
							c.alpha = scanLine[i+3];

							rawPixs[rawIndex] = c;
							rawIndex++;
						}
					}
				}

			}
			else {

			}
			#pragma endregion FillImage

			preScanLine = scanLine;
		}
	}

	if(*amountOfImages == 1)
	{
		images = new Image* [*amountOfImages];
		images[0] = tImg;
	}
	
	StringTools::println("FINISHED");
	return images;
	
}

#pragma endregion

Image** Image::loadJPG(std::vector<unsigned char> fileData, int* amountOfImages)
{
	return nullptr;
}
