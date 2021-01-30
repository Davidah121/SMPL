#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "Graphics.h"
#include <iostream>
#include "StringTools.h"

void Image::savePNG(std::string filename)
{
    
}

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
	p.setPaletteMode(false);
	

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
				
				valid = false;
				break;
			}

			tImg = new Image(width, height);
			
		}
		else if(blockID=="PLTE")
		{
			if(blockSize%3 == 0)
			{
				//std::vector<Color> palette = std::vector<Color>();
				for(int i=0; i<blockSize; i+=3)
				{
					Color c;
					c.red = fileData[location+i];
					c.green = fileData[location+i+1];
					c.blue = fileData[location+i+2];
					c.alpha = 255;

					p.addNewColor(c);
					
					//palette.push_back(c);
				}
				tImg->setPalette(p);
				//p.setPalette( palette );
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

			if(tImg!=nullptr)
				delete tImg;

			if(images!=nullptr)
				delete[] images;
			
			return nullptr;
		}

		std::vector<unsigned char> decompressedData = Compression::decompressDeflate(&compressedData.data()[2], compressedData.size()-2);

		//note that when reading different bit depths, they are packed into bytes
		//instead of taking up an entire byte.
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

		int scanLineBytes = MathExt::ceil(tImg->getWidth() * (bitDepth/8.0));

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
		
		//For interlacing, the amount of values in the scanline is the only thing that changes. It doubles on every even pass
		//The scanlines that are affected are doubled on every odd pass. There can be padding on the scanlines to make it them an even amount.

		//potential equations
		//xGrids = ceil(width/8)
		//yGrids = ceil(height/8)
		//
		//for each grid
		//	do adam7 interlacing
		//	filter based on size of scanline at the time
		//	fill based on adam7 interlacing

		int xGrids = tImg->getWidth();
		int yGrids = tImg->getHeight();
		int pass = 1;
		int scanlinesDone = 0;
		int yGridsNeeded = 0;
		if(interlace)
		{
			xGrids = (int)MathExt::ceil((double)xGrids / 8.0);
			yGrids = (int)MathExt::ceil((double)yGrids / 8.0);
			
			scanLineBytes = (int)MathExt::ceil(xGrids * (bitDepth/8.0));
			yGridsNeeded = yGrids;

			//note that scanLineBytes was changed to a completely new value
			//so adjustments are needed to account for color planes
			//moveBackVal does not need adjustments
			switch(colorType)
			{
				case 2:
					//3 values per pixel
					scanLineBytes*=3;
					break;
				case 4:
					//2 values per pixel
					scanLineBytes*=2;
					break;
				case 6:
					//4 values per pixel
					scanLineBytes*=4;
					break;
				default:
					break;
			}
		}

		while(index < decompressedData.size())
		{
			//read first byte in scanline for the filter method
			int filterMethod = decompressedData[index];
			index++;
			
			#pragma region unfilterScanLine

			if(interlace)
			{
				if(scanlinesDone>=yGridsNeeded)
				{
					scanlinesDone = 0;
					pass++;

					switch (pass)
					{
						case 1:
							scanLineBytes = (int)MathExt::ceil(xGrids * (bitDepth/8.0));
							yGridsNeeded = yGrids;
							break;
						case 2:
							scanLineBytes = (int)MathExt::ceil(xGrids * (bitDepth/8.0));
							yGridsNeeded = yGrids;
							break;
						case 3:
							scanLineBytes = (int)MathExt::ceil(xGrids * (bitDepth/8.0) * 2);
							yGridsNeeded = yGrids;
							break;
						case 4:
							scanLineBytes = (int)MathExt::ceil(xGrids * (bitDepth/8.0) * 2);
							yGridsNeeded = yGrids*2;
							break;
						case 5:
							scanLineBytes = (int)MathExt::ceil(xGrids * (bitDepth/8.0) * 4);
							yGridsNeeded = yGrids*2;
							break;
						case 6:
							scanLineBytes = (int)MathExt::ceil(xGrids * (bitDepth/8.0) * 4);
							yGridsNeeded = yGrids*4;
							break;
						case 7:
							scanLineBytes = (int)MathExt::ceil(xGrids * (bitDepth/8.0) * 8);
							yGridsNeeded = yGrids*4;
							break;
						default:
							scanLineBytes = 0;
							yGridsNeeded = 0;
							break;
					}

					//note that scanLineBytes was changed to a completely new value
					//so adjustments are needed to account for color planes
					//moveBackVal does not need adjustments
					switch(colorType)
					{
						case 2:
							//3 values per pixel
							scanLineBytes*=3;
							break;
						case 4:
							//2 values per pixel
							scanLineBytes*=2;
							break;
						case 6:
							//4 values per pixel
							scanLineBytes*=4;
							break;
						default:
							break;
					}
				}

				scanlinesDone++;
			}

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
					
					p = a+b-c;
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
			
			int interlacedX = 0;
			int interlacedY = 0;
			int incVal = 1;

			if(interlace)
			{
				switch (pass)
				{
				case 1:
					interlacedX = 0;
					incVal = 8;
					interlacedY = (scanlinesDone-1)*8;
					break;
				case 2:
					interlacedX = 4;
					incVal = 8;
					interlacedY = (scanlinesDone-1)*8;
					break;
				case 3:
					interlacedX = 0;
					incVal = 4;
					interlacedY = 4 + (scanlinesDone-1)*8;
					break;
				case 4:
					interlacedX = 2;
					incVal = 4;
					interlacedY = (scanlinesDone-1)*4;
					break;
				case 5:
					interlacedX = 0;
					incVal = 2;
					interlacedY = 2 + (scanlinesDone-1)*4;
					break;
				case 6:
					interlacedX = 1;
					incVal = 2;
					interlacedY = (scanlinesDone-1)*2;
					break;
				case 7:
					interlacedX = 0;
					incVal = 1;
					interlacedY = 1 + (scanlinesDone-1)*2;
					break;
				default:
					break;
				}

			}

			if(colorType==0)
			{
				//Greyscale
					//(R)
					//possible bit depths are 1,2,4,8,16

				if(bitDepth<8)
				{
					BinarySet pixelBits = BinarySet();
					pixelBits.setValues(scanLine.data(), scanLineBytes);

					pixelBits.setBitOrder(BinarySet::RMSB);
					
					//Increase Color Amount
					double ICA = 255.0 / ((2<<(bitDepth-1)) - 1);

					for(int i = 0; i<pixelBits.size(); i+=bitDepth)
					{
						Color c;
						c.red = (unsigned char)(ICA * pixelBits.getBits(i, i + bitDepth, true));
						c.green = c.red;
						c.blue = c.red;
						c.alpha = 255;

						if(!interlace)
						{
							rawPixs[rawIndex] = c;
							rawIndex++;
						}
						else
						{
							tImg->setPixel(interlacedX, interlacedY, c);
							interlacedX += incVal;
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

						if(!interlace)
						{
							rawPixs[rawIndex] = c;
							rawIndex++;
						}
						else
						{
							tImg->setPixel(interlacedX, interlacedY, c);
							interlacedX += incVal;
						}
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

						if(!interlace)
						{
							rawPixs[rawIndex] = c;
							rawIndex++;
						}
						else
						{
							tImg->setPixel(interlacedX, interlacedY, c);
							interlacedX += incVal;
						}
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
					BinarySet pixelBits = BinarySet();
					pixelBits.setValues(scanLine.data(), scanLineBytes);
					pixelBits.setBitOrder(BinarySet::RMSB);
					
					for(int i = 0; i<pixelBits.size(); i+=bitDepth)
					{
						paletteIndex = pixelBits.getBits(i, i+bitDepth, true);

						Color c = p.getColor(paletteIndex);
						if(!interlace)
						{
							rawPixs[rawIndex] = c;
							rawIndex++;
						}
						else
						{
							tImg->setPixel(interlacedX, interlacedY, c);
							interlacedX += incVal;
						}
					}
				}
				else if(bitDepth==8)
				{
					for(int i = 0; i<scanLine.size(); i++)
					{
						paletteIndex = scanLine[i];

						Color c = p.getColor(paletteIndex);
						if(!interlace)
						{
							rawPixs[rawIndex] = c;
							rawIndex++;
						}
						else
						{
							tImg->setPixel(interlacedX, interlacedY, c);
							interlacedX += incVal;
						}
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

						if(!interlace)
						{
							rawPixs[rawIndex] = c;
							rawIndex++;
						}
						else
						{
							tImg->setPixel(interlacedX, interlacedY, c);
							interlacedX += incVal;
						}
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

						if(!interlace)
						{
							rawPixs[rawIndex] = c;
							rawIndex++;
						}
						else
						{
							tImg->setPixel(interlacedX, interlacedY, c);
							interlacedX += incVal;
						}
					}
				}
			}

			#pragma endregion FillImage

			preScanLine = scanLine;
		}
	}

	if(*amountOfImages == 1)
	{
		images = new Image* [1]{tImg};
	}
	
	return images;
	
}
