#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "Graphics.h"

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

Image** Image::loadPNG(std::vector<unsigned char> fileData, int* amountOfImages)
{
	return nullptr;
}

Image** Image::loadJPG(std::vector<unsigned char> fileData, int* amountOfImages)
{
	return nullptr;
}
