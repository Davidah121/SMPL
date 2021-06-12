#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "StringTools.h"
#include "MathExt.h"
#include "Graphics.h"

#include "ColorSpaceConverter.h"

#include "System.h"

void Image::saveGIF(std::string filename, int paletteSize, bool dither, bool saveAlpha, unsigned char alphaThreshold, bool greyscale)
{
	SimpleFile f = SimpleFile(filename, SimpleFile::WRITE);

	if(!f.isOpen())
		return; //invalid so return from function
	
	std::string gifType = "GIF87a";
	std::string gifHeaderInfo = "";
	ColorPalette tempPalette;
	Image tempImg = Image();
	tempImg.copyImage(this);

	unsigned char* pixs = new unsigned char[width*height];

	Color* nPixels = tempImg.getPixels();

	if(greyscale)
	{
		Color* startPixs = nPixels;
		Color* endPixs = nPixels + (width*height);

		while(startPixs < endPixs)
		{
			Color ycbcr = ColorSpaceConverter::convert(*startPixs, ColorSpaceConverter::RGB_TO_YCBCR);
			*startPixs = {ycbcr.red, ycbcr.red, ycbcr.red, ycbcr.alpha};
		}
	}

	bool containsTransparency = saveAlpha;
	time_t t1, t2;

	t1 = System::getCurrentTimeMillis();
	if(containsTransparency)
	{
		if(tempImg.getPalette().getSize() > 0 && tempImg.getPalette().getSize() < paletteSize)
		{
			tempPalette = tempImg.getPalette();
			tempPalette.setPaletteMode(false);
			tempPalette.addNewColor( {0,0,0,0} );
		}
		else
		{
			//palette too big
			tempPalette = ColorPalette::generateOptimalPalette(nPixels, width*height, paletteSize-1, ColorPalette::MEAN_CUT);
		}
	}
	else
	{
		if(tempImg.getPalette().getSize() > 0 && tempImg.getPalette().getSize() <= paletteSize)
		{
			tempPalette = tempImg.getPalette();
		}
		else
		{
			//palette too big
			tempPalette = ColorPalette::generateOptimalPalette(nPixels, width*height, paletteSize, ColorPalette::MEAN_CUT);
		}
	}
	t2 = System::getCurrentTimeMillis();

	StringTools::println("Time to get palette: %u", t2-t1);

	if(dither)
	{
		tempImg.setPalette(tempPalette);
		Graphics::ditherImage(&tempImg, Graphics::FLOYD_DITHER);
	}

	t1 = System::getCurrentTimeMillis();
	if(containsTransparency)
	{
		for(int i=0; i<width*height; i++)
		{
			if(nPixels[i].alpha <= alphaThreshold)
			{
				pixs[i] = 0xFF;
			}
			else
			{
				pixs[i] = (unsigned char)tempPalette.getClosestColorIndex(nPixels[i]);
			}
		}
	}
	else
	{
		for(int i=0; i<width*height; i++)
		{
			pixs[i] = (unsigned char)tempPalette.getClosestColorIndex(nPixels[i]);
		}
	}

	t2 = System::getCurrentTimeMillis();

	StringTools::println("Time to set palette: %u", t2-t1);

	//width
	gifHeaderInfo += (char)((width) & 0xFF);
	gifHeaderInfo += (char)((width>>8) & 0xFF);

	//height
	gifHeaderInfo += (char)((height) & 0xFF);
	gifHeaderInfo += (char)((height>>8) & 0xFF);

	//packed data
		int paletteSizeP2 = (MathExt::ceil( MathExt::log( (double)paletteSize, 2.0) - 1 ) );
		char c = 0;
		//paletteSize
		c += (char)paletteSizeP2;
		//paletteSorted
		c += (char)(0 << 3);
		//colorRes
		c += (char)(paletteSizeP2 << 4);
		//hasGlobalColorTable
		c += (char)(1 << 7);
	gifHeaderInfo += c;

	//backgroundColorIndex
	gifHeaderInfo += (char)0;

	//aspect ratio
	gifHeaderInfo += (char)0;


	//addPalette
	int padding = (1 << (paletteSizeP2+1)) - tempPalette.getSize();
	
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

	//Graphic Control Extension
	gifHeaderInfo += (char)0x21;
	
	//Label
	gifHeaderInfo += (char)0xF9;

	//BlockSize
	gifHeaderInfo += (char)0x04;

	//PackedField
		//Note that most of this is reserved
		//0x00 = no transparency | 0x01 = has transparency
		gifHeaderInfo += (char)containsTransparency;
	
	//Delay Time
	gifHeaderInfo += (char)0x00;
	gifHeaderInfo += (char)0x00;

	//Transparent Color Index, will always be the last value
	gifHeaderInfo += (char)0xFF;

	//Block Terminator
	gifHeaderInfo += (char)0x00;

	//Image Descriptor
	gifHeaderInfo += (char)0x2C;

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
	gifHeaderInfo += (char)0x00;

	//min code size
	gifHeaderInfo += (char)paletteSizeP2+1;
	
	//compress data
	t1 = System::getCurrentTimeMillis();
	std::vector<unsigned char> compressedData = Compression::compressLZW(pixs, width*height, paletteSizeP2+1);
	t2 = System::getCurrentTimeMillis();

	StringTools::println("Time to compress: %u", t2-t1);

	for(int i=0; i<compressedData.size(); i++)
	{
		if(i % 255 == 0)
		{
			if(compressedData.size() - i >= 255)
			{
				gifHeaderInfo += (char)0xFF;
			}
			else
			{
				gifHeaderInfo += (compressedData.size() - i);
			}
		}

		gifHeaderInfo += compressedData[i];
	}

	//end of image data
	gifHeaderInfo += (char)0x00;

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
	globalColorTable.setPaletteMode(false);

	std::string gifType = "";
	gifType += fileData[0];	gifType += fileData[1];	gifType += fileData[2];
	gifType += fileData[3];	gifType += fileData[4];	gifType += fileData[5];

	bool valid = false; 
	
	if (gifType == "GIF87a" || gifType == "GIF89a")
	{
		valid = true;
	}

	int disposeMethod = 0;
	int nextDisposeMethod = 0;
	int indexOfLastNotDisposed = -1;
	bool hasTransparency = false;
	
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

				globalColorTable.getPalette().push_back(c);
			}

			startIndex += sizeOfGlobalTable * 3;
		}

		int transparentColorIndex = -1;
		int backgroundTile = -1;

		while (startIndex < fileData.size())
		{
			if (fileData[startIndex] == 0x2C)
			{
				Image* tempImage = new Image(globalWidth, globalHeight);
				
				if (images->size() > 0)
				{
					if(disposeMethod!=0)
						tempImage->copyImage(images->back());
				}

				if(disposeMethod==0 || disposeMethod==1)
				{
					indexOfLastNotDisposed = images->size()-1;
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
				localTable.setPaletteMode(false);

				if (hasLocalTable)
				{
					for (int i = 0; i < sizeOfLocalTable; i++)
					{
						int tempIndex = i * 3;
						Color c = { fileData[startIndex + tempIndex], fileData[startIndex + tempIndex + 1], fileData[startIndex + tempIndex + 2], 255 };

						localTable.getPalette().push_back(c);
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
							else
							{
								c.alpha = 255;
							}

							if(c.alpha!=0)
							{
								tempImage->setPixel(x + leftScreenPos, y + topScreenPos, c);
							}
							else
							{
								if(disposeMethod==0)
								{
									c.alpha=255;
									tempImage->setPixel(x + leftScreenPos, y + topScreenPos, c);
								}
								else if(disposeMethod==2)
								{
									Color c;
									
									c = localTable.getColor(backgroundColorIndex);
									if (backgroundColorIndex == transparentColorIndex && hasTransparency)
									{
										c.alpha = 0;
									}
									
									tempImage->setPixel(x + leftScreenPos, y + topScreenPos, c);
								}
								else if(disposeMethod==3)
								{
									if(indexOfLastNotDisposed>0)
									{
										Color c = (images->at(indexOfLastNotDisposed))->getPixel(x + leftScreenPos, y + topScreenPos);
										
										tempImage->setPixel(x + leftScreenPos, y + topScreenPos, c);
									}
								}
							}
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
							else
							{
								c.alpha = 255;
							}

							if(c.alpha!=0)
							{
								tempImage->setPixel(x + leftScreenPos, y + topScreenPos, c);
							}
							else
							{
								if(disposeMethod==0)
								{
									c.alpha=255;
									tempImage->setPixel(x + leftScreenPos, y + topScreenPos, c);
								}
								else if(disposeMethod==2)
								{
									Color c;
									
									c = localTable.getColor(backgroundColorIndex);
									if (backgroundColorIndex == transparentColorIndex && hasTransparency)
									{
										c.alpha = 0;
									}
									
									tempImage->setPixel(x + leftScreenPos, y + topScreenPos, c);
								}
								else if(disposeMethod==3)
								{
									if(indexOfLastNotDisposed>0)
									{
										Color c = (images->at(indexOfLastNotDisposed))->getPixel(x + leftScreenPos, y + topScreenPos);
										
										tempImage->setPixel(x + leftScreenPos, y + topScreenPos, c);
									}
								}
							}
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
				disposeMethod = nextDisposeMethod;
			}
			else if (fileData[startIndex] == 0x21)
			{
				//Control Extension
				//We only care about the graphics control extension
				if (fileData[startIndex+1] == 0xF9)
				{
					//GCE
					int blockSize = fileData[startIndex + 2];
					hasTransparency = (fileData[startIndex + 3] & 0x01) == 1;
					nextDisposeMethod = (fileData[startIndex + 3] >> 2) & 7;

					//DelayTime in 1/100 seconds
					int delayTime = (int)fileData[startIndex + 4] + (((int)fileData[startIndex + 5]) << 8);

					
					transparentColorIndex = fileData[startIndex + 6];
					
					startIndex += 3; //controlExtenstion + type + blockSize
					startIndex += blockSize;

					startIndex++;//block terminator
				}
				else 
				{
					
					int blockSize = fileData[startIndex + 2];

					startIndex += 3; //controlExtenstion + type + blockSize
					startIndex += blockSize;
					
					while(fileData[startIndex]!=0x00)
					{
						//datablock probably
						blockSize = fileData[startIndex];
						startIndex+=blockSize+1;
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
				//error
				break;
			}
		}
	}

	if (amountOfImages != nullptr)
		*amountOfImages = images->size();

	return images->data();
}
