#include "Image.h"
#include <vector>
#include "SimpleFile.h"
#include "Compression.h"
#include "SimpleGraphics.h"
#include <iostream>
#include "StringTools.h"

#include "ColorSpaceConverter.h"

namespace smpl
{
	void Image::saveBMP(File file, unsigned char alphaThreshold, bool greyscale)
	{
		//save header
		//save pixel data
		//return
		
		SimpleFile f = SimpleFile(file, SimpleFile::WRITE);
		if(f.isOpen())
		{
			unsigned char* header = new unsigned char[14];
			unsigned char* infoHeader = new unsigned char[40];

			memset(header, 0, 14);
			memset(infoHeader, 0, 40);
			
			struct FileHeader
			{
				unsigned short type = 0;
				unsigned int size = 0;
				unsigned short reserved1 = 0;
				unsigned short reserved2 = 0;
				unsigned int offset = 0;
			};

			struct FileInfo
			{
				unsigned int size = 0;
				int width = 0;
				int height = 0;
				unsigned short colorPlanes = 0;
				unsigned short bitsPerPixel = 0;
				unsigned int compressionMethod = 0;
				unsigned int imageSize = 0;
				int xResolution = 0;
				int yResolution = 0;
				unsigned int paletteSize = 0;
				unsigned int importantColors = 0;
			};

			//calculate additional bytes on a scanline for padding
			int padding = (width*3) % 4;
			if(padding>0)
			{
				padding = 4-padding;
			}

			header[0] = 0x42; //header
			header[1] = 0x4D; //header

			unsigned int* sizeStart = (unsigned int*)&header[2];
			*sizeStart = 40+14+(unsigned int)((width+padding)*height*3); //size

			header[10] = 54; //data offset

			
			unsigned int* infoAsInt = (unsigned int*)infoHeader;
			infoAsInt[0] = 40; //size
			infoAsInt[1] = width; //width
			infoAsInt[2] = height; //height
			
			unsigned short* infoAsShort = (unsigned short*)&infoHeader[12];
			infoAsShort[0] = 1; //color planes
			infoAsShort[1] = 24; //bits per pixel

			//start from the bottom up
			unsigned char* outPixels = new unsigned char[(width+padding)*height*3];
			int i=0;
			for(int y=height-1; y>=0; y--)
			{
				for(int x=0; x<width; x++)
				{
					Color c = getPixel(x,y);
					if(c.alpha >= alphaThreshold)
					{
						if(!greyscale)
						{
							outPixels[i] = c.blue;
							outPixels[i+1] = c.green;
							outPixels[i+2] = c.red;
						}
						else
						{
							Color ycbcr = ColorSpaceConverter::convert(c, ColorSpaceConverter::RGB_TO_YCBCR);

							outPixels[i] = ycbcr.red;
							outPixels[i+1] = ycbcr.red;
							outPixels[i+2] = ycbcr.red;
						}
					}
					else
					{
						outPixels[i] = 0;
						outPixels[i+1] = 0;
						outPixels[i+2] = 0;
					}
					i+=3;
				}
				for(int x=0; x<padding; x++)
				{
					outPixels[i] = 0;
					i++;
				}
			}

			f.writeBytes(header, 14);
			f.writeBytes(infoHeader, 40);
			f.writeBytes(outPixels, (width+padding)*height*3);
			
			delete[] outPixels;
			delete[] header;
			delete[] infoHeader;
			f.close();
		}
	}

	HiResImage** HiResImage::loadBMP(std::vector<unsigned char> fileData, int* amountOfImages)
	{
		HiResImage** images = nullptr;
		//Assume that the bitmap is a valid bitmap
		int ID = ((int)fileData[0] << 8) + fileData[1];
		int size = ((int)fileData[5] << 24) + ((int)fileData[4]<<16)
			+((int)fileData[3]<<8) + ((int)fileData[2]);
		
		//skip the reserved data

		int pixStart = ((int)fileData[13] << 24) + ((int)fileData[12]<<16)
			+((int)fileData[11]<<8) + ((int)fileData[10]);

		int width = ((int)fileData[21] << 24) + ((int)fileData[20] << 16)
			+ ((int)fileData[19] << 8) + ((int)fileData[18]);

		int height = ((int)fileData[25] << 24) + ((int)fileData[24] << 16)
			+ ((int)fileData[23] << 8) + ((int)fileData[22]);

		int bpp = ((int)fileData[29] << 8) + ((int)fileData[28]);

		int compressionMethod = 0;
		int colorsInPalette = 0;
		unsigned int bitFieldsR = 0;
		unsigned int bitFieldsG = 0;
		unsigned int bitFieldsB = 0;
		unsigned int bitFieldsA = 0;

		int shiftAmountR = 32;
		int shiftAmountG = 32;
		int shiftAmountB = 32;
		int shiftAmountA = 32;

		if(size >= 40)
		{
			int compressionMethod = ((int)fileData[33] << 24) + ((int)fileData[32] << 16)
				+ ((int)fileData[31] << 8) + ((int)fileData[30]);

			int colorsInPalette = ((int)fileData[49] << 24) + ((int)fileData[48] << 16)
				+ ((int)fileData[47] << 8) + ((int)fileData[46]);
		}

		if(size >= 52)
		{
			bitFieldsR = ((int)fileData[0x39] << 24) + ((int)fileData[0x38] << 16)
				+ ((int)fileData[0x37] << 8) + ((int)fileData[0x36]);
			bitFieldsG = ((int)fileData[0x3D] << 24) + ((int)fileData[0x3C] << 16)
				+ ((int)fileData[0x3B] << 8) + ((int)fileData[0x3A]);
			bitFieldsB = ((int)fileData[0x41] << 24) + ((int)fileData[0x40] << 16)
				+ ((int)fileData[0x3F] << 8) + ((int)fileData[0x3E]);
		}
		if(size >= 56)
		{
			bitFieldsA = ((int)fileData[0x45] << 24) + ((int)fileData[0x44] << 16)
				+ ((int)fileData[0x43] << 8) + ((int)fileData[0x42]);
		}
		else
		{
			bitFieldsA = 0;
		}

		for(int i=0; i<32; i++)
		{
			if((bitFieldsA >> i) & 0x01)
				shiftAmountA = MathExt::min(shiftAmountA, i);
				
			if((bitFieldsR >> i) & 0x01)
				shiftAmountR = MathExt::min(shiftAmountR, i);
				
			if((bitFieldsG >> i) & 0x01)
				shiftAmountG = MathExt::min(shiftAmountG, i);
				
			if((bitFieldsB >> i) & 0x01)
				shiftAmountB = MathExt::min(shiftAmountB, i);
		}

		//Load palette if it exists

		ColorPalette cPalette = ColorPalette();
		cPalette.setPaletteMode(false);

		if (colorsInPalette > 0)
		{
			int paletteStart = 54;

			for (int i = 0; i < colorsInPalette; i++)
			{
				int refLoc = paletteStart + (4 * i);
				
				cPalette.addNewColor({ (unsigned char)fileData[refLoc + 2], (unsigned char)fileData[refLoc + 1], (unsigned char)fileData[refLoc], 255 });
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
			size_t i = pixStart;

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
			size_t i = pixStart;
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
		else if (compressionMethod == 3 || compressionMethod == 6)
		{
			//if size == 64, it is OS22XBITMAPHEADER which uses more stuff that can't be ignored.
			//It uses Huffman 1D compression which we won't bother with here
			if(size == 64)
				return nullptr;
		}
		else if (compressionMethod == 4)
		{
			//not a bmp. Contains a different image format inside of it. Contains a JPEG
			return nullptr;
		}
		else if (compressionMethod == 5)
		{
			//not a bmp. Contains a different image format inside of it. Contains a PNG
			return nullptr;
		}
		else
		{
			//can't be loaded
			return nullptr;
		}

		//Load pixels
		HiResImage* tImg = new HiResImage(width, height);

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
						Color4f actualColor = Color4f{(double)setC.red / 255, (double)setC.green / 255, (double)setC.blue / 255, (double)setC.alpha / 255};
						tImg->pixels[y * width + x] = actualColor;
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

					if(y<0)
					{
						break;
					}
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
						Color4f actualColor = Color4f{(double)setC.red / 255, (double)setC.green / 255, (double)setC.blue / 255, (double)setC.alpha / 255};
						tImg->pixels[y * width + x] = actualColor;
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

					if(y<0)
					{
						break;
					}
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
						Color4f actualColor = Color4f{(double)setC.red / 255, (double)setC.green / 255, (double)setC.blue / 255, (double)setC.alpha / 255};
						tImg->pixels[y * width + x] = actualColor;
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

					if(y<0)
					{
						break;
					}
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
				Color4f actualColor = Color4f{(double)setC.red / 255, (double)setC.green / 255, (double)setC.blue / 255, (double)setC.alpha / 255};
				tImg->pixels[y * width + x] = actualColor;

				x++;
				if (x >= width)
				{
					y--;
					i += padding;
					x = 0;

					if(y<0)
					{
						break;
					}
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

			if(compressionMethod != 3 && compressionMethod != 6)
			{
				bitFieldsR = 0x00F0;
				bitFieldsG = 0x0F00;
				bitFieldsB = 0xF000;
				bitFieldsA = 0x000F;

				shiftAmountR = 4;
				shiftAmountG = 8;
				shiftAmountB = 12;
				shiftAmountA = 0;
			}
			while (i < pixsDataSize)
			{
				unsigned int colorAsInt = ((unsigned int)pixData[i+1] << 8) + pixData[i];
				Color setC;
				setC.red = (colorAsInt & bitFieldsR) >> shiftAmountR;
				setC.green = (colorAsInt & bitFieldsG) >> shiftAmountG;
				setC.blue = (colorAsInt & bitFieldsB) >> shiftAmountB;
				setC.alpha = (colorAsInt & bitFieldsA) >> shiftAmountA;
				
				Color4f actualColor = Color4f{(double)setC.red / 255, (double)setC.green / 255, (double)setC.blue / 255, (double)setC.alpha / 255};
				tImg->pixels[y * width + x] = actualColor;

				x++;
				if (x >= width)
				{
					y--;
					i += padding;
					x = 0;

					if(y<0)
					{
						break;
					}
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
				Color setC = {pixData[i+2], pixData[i+1], pixData[i], 255};
				Color4f actualColor = Color4f{(double)setC.red / 255, (double)setC.green / 255, (double)setC.blue / 255, (double)setC.alpha / 255};
				tImg->pixels[y * width + x] = actualColor;

				x++;
				if (x >= width)
				{
					y--;
					i += padding;
					x = 0;

					if(y<0)
					{
						break;
					}
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

			if(compressionMethod != 3 && compressionMethod != 6)
			{
				unsigned int bitFieldsR = 0x0000FF00;
				unsigned int bitFieldsG = 0x00FF0000;
				unsigned int bitFieldsB = 0xFF000000;
				unsigned int bitFieldsA = 0x000000FF;

				int shiftAmountR = 8;
				int shiftAmountG = 16;
				int shiftAmountB = 24;
				int shiftAmountA = 0;
			}

			while (i < pixsDataSize)
			{
				unsigned int colorAsInt = ((unsigned int)pixData[i] << 24) + ((unsigned int)pixData[i+1] << 16)
										+ ((unsigned int)pixData[i+2] << 8) + ((unsigned int)pixData[i+3]);
				
				Color setC;
				setC.red = (colorAsInt & bitFieldsR) >> shiftAmountR;
				setC.green = (colorAsInt & bitFieldsG) >> shiftAmountG;
				setC.blue = (colorAsInt & bitFieldsB) >> shiftAmountB;
				setC.alpha = (colorAsInt & bitFieldsA) >> shiftAmountA;

				Color4f actualColor = Color4f{(double)setC.red / 255, (double)setC.green / 255, (double)setC.blue / 255, (double)setC.alpha / 255};
				tImg->pixels[y * width + x] = actualColor;

				x++;
				if (x >= width)
				{
					y--;
					i += padding;
					x = 0;

					if(y<0)
					{
						break;
					}
				}
				i += 4;
			}
		}

		if(amountOfImages!=nullptr)
			*amountOfImages = 1;

		images = new HiResImage*[1]{ tImg };
		return images;
	}

} //NAMESPACE glib END