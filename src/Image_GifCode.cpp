#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "StringTools.h"
#include "MathExt.h"
#include "SimpleGraphics.h"

#include "ColorSpaceConverter.h"

#include "System.h"

namespace smpl
{
	void Image::saveGIF(File file, int paletteSize, bool dither, bool saveAlpha, unsigned char alphaThreshold)
	{
		SimpleFile f = SimpleFile(file, SimpleFile::WRITE);

		if(!f.isOpen())
			return; //invalid so return from function
		
		std::string gifType = "GIF87a";
		std::string gifHeaderInfo = "";
		ColorPalette tempPalette;
		Image tempImg = Image();
		tempImg.copyImage(this);

		unsigned char* pixs = new unsigned char[width*height];

		Color* nPixels = tempImg.getPixels();

		bool containsTransparency = saveAlpha;
		if(containsTransparency)
		{
			if(tempImg.getPalette().getSize() > 0 && tempImg.getPalette().getSize() < (size_t)paletteSize)
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
			if(tempImg.getPalette().getSize() > 0 && tempImg.getPalette().getSize() <= (size_t)paletteSize)
			{
				tempPalette = tempImg.getPalette();
			}
			else
			{
				//palette too big
				tempPalette = ColorPalette::generateOptimalPalette(nPixels, width*height, paletteSize, ColorPalette::MEAN_CUT);
			}
		}

		if(dither)
		{
			tempImg.setPalette(tempPalette);
			SimpleGraphics::ditherImage(&tempImg, SimpleGraphics::FLOYD_DITHER);
		}

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

		//width
		gifHeaderInfo += (char)((width) & 0xFF);
		gifHeaderInfo += (char)((width>>8) & 0xFF);

		//height
		gifHeaderInfo += (char)((height) & 0xFF);
		gifHeaderInfo += (char)((height>>8) & 0xFF);

		//packed data
			int paletteSizeP2 = (int)(MathExt::ceil( MathExt::log( (double)paletteSize, 2.0) - 1 ) );
			char c = 0;
			//paletteSize
			c += (char)paletteSizeP2;
			//paletteSorted
			c += (char)(0 << 3);
			//colorRes
			c += (char)(7 << 4);
			//hasGlobalColorTable
			c += (char)(1 << 7);
		gifHeaderInfo += c;

		//backgroundColorIndex
		gifHeaderInfo += (char)0xFF;

		//aspect ratio
		gifHeaderInfo += (char)0;


		
		int codeSize = paletteSizeP2+1;
		//addPalette
		int padding = (1 << (codeSize)) - tempPalette.getSize();
		
		for(size_t i=0; i<tempPalette.getSize(); i++)
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
		gifHeaderInfo += (char)codeSize;
		
		//Separate into blocks to make it multi threaded
		//each block should approach the max dictionary size of 4096 entries and try not to exceed it.

		//compress data
		std::vector<unsigned char> compressedData = Compression::compressLZW(pixs, width*height, 1, codeSize);
		
		for(size_t i=0; i<compressedData.size(); i++)
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

	bool Image::saveAGIF(File file, Image** images, int size, int* delayTimePerFrame, bool loops, int paletteSize, bool dither, bool saveAlpha, unsigned char alphaThreshold)
	{
		SimpleFile f = SimpleFile(file, SimpleFile::WRITE);

		if(!f.isOpen())
		{
			return false; //invalid so return from function
		}
		
		std::string gifType = "GIF89a";
		std::string gifHeaderInfo = "";

		//first check if all images have the same size
		int w,h;
		if(images == nullptr || size <= 0)
		{
			return false; //No images
		}
		w = images[0]->getWidth();
		h = images[0]->getHeight();

		for(int i=1; i<size; i++)
		{
			if(w != images[i]->getWidth())
				return false; //Not all the same width
			
			if(h != images[i]->getHeight())
				return false; //Not all the same height
		}

		//width
		gifHeaderInfo += (char)((w) & 0xFF);
		gifHeaderInfo += (char)((w>>8) & 0xFF);

		//height
		gifHeaderInfo += (char)((h) & 0xFF);
		gifHeaderInfo += (char)((h>>8) & 0xFF);

		//packed data
			//colorRes
			char c = 0b01110111;
		gifHeaderInfo += c;

		//backgroundColorIndex
		gifHeaderInfo += (char)0x00;

		//aspect ratio
		gifHeaderInfo += (char)0;

		///Need NETSCAPE to specify looping
		gifHeaderInfo += (char)0x21; //extension code
		gifHeaderInfo += (char)0xFF; //application extension
		gifHeaderInfo += (char)0x0B; //length of application block (11 bytes)
		gifHeaderInfo += "NETSCAPE2.0";
		gifHeaderInfo += (char)0x03;
		gifHeaderInfo += (char)loops; 
		gifHeaderInfo += (char)0x00; //loop infinitely.
		gifHeaderInfo += (char)0x00; //loop infinitely.
		gifHeaderInfo += (char)0x00; //end of block

		f.writeString(gifType);
		f.writeString(gifHeaderInfo);

		std::vector<unsigned char> pixs = std::vector<unsigned char>(w*h);
		Image oldImg;
		Image tempImg;
		for(int i=0; i<size; i++)
		{
			gifHeaderInfo = "";

			//Graphic Control Extension
			gifHeaderInfo += (char)0x21;
			
			//Label
			gifHeaderInfo += (char)0xF9;

			//BlockSize
			gifHeaderInfo += (char)0x04;

			//PackedField
				//Note that most of this is reserved. Includes disposal method too
				//0x09 = has transparency, dispose to background | 0x05 = transparency, do not dispose (Transparency needed to record differences between frames)
				gifHeaderInfo += (saveAlpha) ? 0x09 : 0x05;
			
			//Delay Time
			//Time expected to be in milliseconds (1/1000s) but this needs to be in 1/100s of a second
			int tempTime = (delayTimePerFrame[i]) / 10;
			gifHeaderInfo += (char)(tempTime & 0xFF);
			gifHeaderInfo += (char)((tempTime >> 8) & 0xFF);

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
			gifHeaderInfo += (char)((w) & 0xFF);
			gifHeaderInfo += (char)((w>>8) & 0xFF);

			//height
			gifHeaderInfo += (char)((h) & 0xFF);
			gifHeaderInfo += (char)((h>>8) & 0xFF);

			//packedInfo
			char packed = 0b10000000;
			char codeSize = (int)MathExt::ceil( MathExt::log((double)paletteSize, 2.0) );
			packed |= codeSize-1;
			gifHeaderInfo += packed;

			//create palette
			ColorPalette palette;
			palette = ColorPalette::generateOptimalPalette(images[i]->getPixels(), w*h, paletteSize-1, ColorPalette::MEAN_CUT);
			
			//store palette
			for(int k=0; k<palette.getSize(); k++)
			{
				Color c = palette.getColor(k);
				gifHeaderInfo += c.red;
				gifHeaderInfo += c.green;
				gifHeaderInfo += c.blue;
			}
			//padding for the transparent value. Needed regardless of alpha choice
			gifHeaderInfo += (char)0;
			gifHeaderInfo += (char)0;
			gifHeaderInfo += (char)0;
		
			//min code size
			gifHeaderInfo += (char)codeSize;
			
			//Separate into blocks to make it multi threaded
			//each block should approach the max dictionary size of 4096 entries and try not to exceed it.

			//gotta copy the image, dither if necessary, and replace the pixels with a number in the palette.
			if(i>0)
				oldImg.copyImage(&tempImg);
			
			tempImg.copyImage(images[i]);
			Color* nPixels = tempImg.getPixels();
			if(dither)
			{
				tempImg.setPalette(palette);
				SimpleGraphics::ditherImage(&tempImg, SimpleGraphics::FLOYD_DITHER);
			}

			Color* oldImgPixs = oldImg.getPixels();
			for(int j=0; j<w*h; j++)
			{
				if(i != 0)
				{
					if(nPixels[j] == oldImgPixs[j])
					{
						pixs[j] = 0xFF;
						continue;
					}
				}

				if(nPixels[j].alpha <= alphaThreshold && saveAlpha)
				{
					pixs[j] = 0xFF;
				}
				else
				{
					pixs[j] = (unsigned char)palette.getClosestColorIndex(nPixels[j]);
				}
			}
			
			//compress data
			std::vector<unsigned char> compressedData = Compression::compressLZW(pixs.data(), w*h, 1, codeSize);

			for(size_t i=0; i<compressedData.size(); i++)
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

			f.writeString(gifHeaderInfo); //write everything out
		}

		f.writeByte(0x3B); //end of data

		f.close();
		return true;
	}

	HiResImage** HiResImage::loadGIF(std::vector<unsigned char> fileData, int* amountOfImages, std::vector<int>* extraData)
	{
		std::vector<HiResImage*>* images = new std::vector<HiResImage*>();

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

		if(extraData!=nullptr)
		{
			extraData->clear();
			extraData->push_back(0); //Set to not loop by default
		}
		
		if (valid == true)
		{
			int globalWidth = (int)fileData[6] + ((int)fileData[7] << 8);
			int globalHeight = (int)fileData[8] + ((int)fileData[9] << 8);
			
			bool hasGlobalColorTable = (fileData[10] >> 7) == 1;
			int colorRes = ((fileData[10] >> 4) & 0b00000111) + 1;
			// bool globalTableSorted = ((fileData[10] >> 3) & 0b00000001) == 1; //Not used
			int sizeOfGlobalTable = 2 << (fileData[10] & 0b00000111);

			int backgroundColorIndex = fileData[11];
			// int pixelAspectRatio = fileData[12]; //Not used

			size_t startIndex = 13;
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
			// int backgroundTile = -1; //Not used. Not sure if this should be used???

			while (startIndex < fileData.size())
			{
				if (fileData[startIndex] == 0x2C)
				{
					HiResImage* tempImage = new HiResImage(globalWidth, globalHeight);
					
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
					// bool sorted = ((fileData[startIndex + 9] >> 5) & 0x01) == 1; //Not needed

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

					// int lzwMinCodeSize = fileData[startIndex]; //Not needed. The palette must be the dictionary size. Otherwise, use 1 << lzwCodeSize to get the dictionary size.
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
						pixValues = Compression::decompressLZW(valuesToDecompress.data(), valuesToDecompress.size(), localTable.getSize());
					else
						pixValues = Compression::decompressLZW(valuesToDecompress.data(), valuesToDecompress.size(), localTable.getSize()+2);
					
					if (localTable.getSize() != 0)
					{
						if(interlaced==false)
						{
							size_t j = 0;
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
									tempImage->setPixel(x + leftScreenPos, y + topScreenPos, SimpleGraphics::convertColorToColor4f(c));
								}
								else
								{
									if(disposeMethod==0)
									{
										c.alpha=255;
										tempImage->setPixel(x + leftScreenPos, y + topScreenPos, SimpleGraphics::convertColorToColor4f(c));
									}
									else if(disposeMethod==2)
									{
										Color c;
										
										c = localTable.getColor(backgroundColorIndex);
										if (backgroundColorIndex == transparentColorIndex && hasTransparency)
										{
											c.alpha = 0;
										}
										
										tempImage->setPixel(x + leftScreenPos, y + topScreenPos, SimpleGraphics::convertColorToColor4f(c));
									}
									else if(disposeMethod==3)
									{
										if(indexOfLastNotDisposed>0)
										{
											Color4f c = (images->at(indexOfLastNotDisposed))->getPixel(x + leftScreenPos, y + topScreenPos);
											
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
							size_t j = 0;
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
									tempImage->setPixel(x + leftScreenPos, y + topScreenPos, SimpleGraphics::convertColorToColor4f(c));
								}
								else
								{
									if(disposeMethod==0)
									{
										c.alpha=255;
										tempImage->setPixel(x + leftScreenPos, y + topScreenPos, SimpleGraphics::convertColorToColor4f(c));
									}
									else if(disposeMethod==2)
									{
										Color c;
										
										c = localTable.getColor(backgroundColorIndex);
										if (backgroundColorIndex == transparentColorIndex && hasTransparency)
										{
											c.alpha = 0;
										}
										
										tempImage->setPixel(x + leftScreenPos, y + topScreenPos, SimpleGraphics::convertColorToColor4f(c));
									}
									else if(disposeMethod==3)
									{
										if(indexOfLastNotDisposed>0)
										{
											Color4f c = (images->at(indexOfLastNotDisposed))->getPixel(x + leftScreenPos, y + topScreenPos);
											
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
						
						if(extraData!=nullptr)
						{
							extraData->push_back( delayTime*10 );
						}
						
						transparentColorIndex = fileData[startIndex + 6];
						
						startIndex += 3; //controlExtenstion + type + blockSize
						startIndex += blockSize;

						startIndex++;//block terminator
					}
					else if(fileData[startIndex+1] == 0xFF)
					{
						int blockSize = fileData[startIndex + 2];
						startIndex += 3;
						if(blockSize == 11)
						{
							std::string s = "";
							for(int j=0; j<11; j++)
							{
								s += fileData[startIndex + j];
							}
							if(s == "NETSCAPE2.0")
							{
								startIndex += blockSize;
								//read 4 bytes then terminator
								if(extraData != nullptr)
								{
									extraData->at(0) = fileData[startIndex+1];
								}
								startIndex+=5;
							}
							else
							{
								//Not sure what it is. Skip
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
						else
						{
							//Not sure what it is. Skip
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

} //NAMESPACE glib END