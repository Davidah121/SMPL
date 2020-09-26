#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "Graphics.h"
#include <iostream>
#include "StringTools.h"

void Image::saveGIF(std::string filename)
{

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
