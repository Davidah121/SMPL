#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "Graphics.h"
#include <iostream>
#include "StringTools.h"

void Image::saveGIF(std::string filename)
{
	SimpleFile f = SimpleFile(filename, SimpleFile::WRITE);

	if(!f.isOpen())
		return; //invalid so return from function
	
	std::string gifType = "GIF87a";
	std::string gifHeaderInfo = "";
	ColorPalette tempPalette = p;
	unsigned char* pixs = new unsigned char[width*height];

	// StringTools::println("Before optimal palette");
	if(p.getSize() == 0 || p.getSize() > 256)
	{
		//no palette, create a palette but no dithering
		tempPalette = ColorPalette::createColorPalette(6,7,6);
		//tempPalette = ColorPalette::generateOptimalPalette(pixels, width*height, 4);
	}
	
	// StringTools::println("Before fixed colors");
	
	for(int i=0; i<width*height; i++)
	{
		if(pixels[i].alpha <= 127)
		{
			pixs[i] = (unsigned char)tempPalette.getClosestColorIndex( {0,0,0,0} );
		}
		else
		{
			pixs[i] = (unsigned char)tempPalette.getClosestColorIndex( pixels[i] );
		}
	}

	//width
	gifHeaderInfo += (char)((width) & 0xFF);
	gifHeaderInfo += (char)((width>>8) & 0xFF);

	//height
	gifHeaderInfo += (char)((height) & 0xFF);
	gifHeaderInfo += (char)((height>>8) & 0xFF);

	//packed data
		int paletteSize = (MathExt::ceil( MathExt::log( (double)tempPalette.getSize(), 2.0) - 1 ) );
		char c = 0;
		//paletteSize
		c += (char)paletteSize;
		//paletteSorted
		c += (char)(0 << 3);
		//colorRes
		c += (char)(paletteSize << 4);
		//hasGlobalColorTable
		c += (char)(1 << 7);
	gifHeaderInfo += c;

	//backgroundColorIndex
	gifHeaderInfo += (char)0;

	//aspect ratio
	gifHeaderInfo += (char)0;


	//addPalette
	int padding = (1 << (paletteSize+1)) - tempPalette.getSize();
	
	for(int i=0; i<tempPalette.getSize(); i++)
	{
		Color c = tempPalette.getColor(i);
		gifHeaderInfo += c.red;
		gifHeaderInfo += c.green;
		gifHeaderInfo += c.blue;
	}

	for(int i=0; i<padding; i++)
	{
		gifHeaderInfo += (char)0;
		gifHeaderInfo += (char)0;
		gifHeaderInfo += (char)0;
	}

	//Image Descriptor
	gifHeaderInfo += 0x2C;

	//x
	gifHeaderInfo += (char)0;
	gifHeaderInfo += (char)0;

	//y
	gifHeaderInfo += (char)0;
	gifHeaderInfo += (char)0;

	//width
	gifHeaderInfo += (char)((width) & 0xFF);
	gifHeaderInfo += (char)((width>>8) & 0xFF);

	//height
	gifHeaderInfo += (char)((height) & 0xFF);
	gifHeaderInfo += (char)((height>>8) & 0xFF);

	//packedInfo
	gifHeaderInfo += (char)0;

	//min code size
	gifHeaderInfo += (char)paletteSize+1;
	
	// StringTools::out << "Size: " << tempPalette.getSize() << StringTools::lineBreak;
	// StringTools::out << "color res: " << paletteSize << StringTools::lineBreak;
	// StringTools::out << "minCodeSize: " << paletteSize+1 << StringTools::lineBreak;
	// StringTools::out << "padding: " << padding << StringTools::lineBreak;
	
	//compress data
	std::vector<unsigned char> compressedData = Compression::compressLZW(pixs, width*height, paletteSize+1);
	
	for(int i=0; i<compressedData.size(); i++)
	{
		if(i % 255 == 0)
		{
			if(compressedData.size() - i >= 255)
			{
				gifHeaderInfo += 0xFF;
			}
			else
			{
				gifHeaderInfo += (compressedData.size() - i);
			}
		}

		gifHeaderInfo += compressedData[i];
	}

	//end of image data
	gifHeaderInfo += (char)0;

	//end of data
	gifHeaderInfo += (char)0x3B;

	f.writeString(gifType);
	f.writeString(gifHeaderInfo);

	f.close();
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

				std::vector<unsigned char> pixValues;
				if(colorRes!=1)
					pixValues = Compression::decompressLZW(valuesToDecompress, localTable.getSize());
				else
					pixValues = Compression::decompressLZW(valuesToDecompress, localTable.getSize()+2);
				
				if (localTable.getSize() != 0)
				{
					if(interlaced==false)
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
					else
					{
						int j = 0;
						int incVal = 8;
						int startY = 0;
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
								//Interlacing Portion
								y+=incVal;
								if(y>=lHeight)
								{
									switch (startY)
									{
									case 0:
										incVal=8;
										startY=4;
										break;
									case 4:
										incVal=4;
										startY=2;
										break;
									case 2:
										incVal=2;
										startY=1;
										break;
									default:
										break;
									}
									y = startY;
								}
								x = 0;
							}
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
