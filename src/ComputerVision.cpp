#include "ComputerVision.h"

namespace smpl
{
	std::vector<Vec2f> ComputerVision::calculateGradient(Image* img, unsigned char type)
	{
		std::vector<Vec2f> gradientImage;
		if(img!=nullptr)
		{
			gradientImage = std::vector<Vec2f>(img->getHeight()*img->getWidth());

			for(int y=0; y<img->getHeight(); y++)
			{
				for(int x=0; x<img->getWidth(); x++)
				{
					Vec2f grad;
					Color c1, c2;
					switch(type)
					{
						case RED_CHANNEL:
							//central x difference
							c1 = img->getPixel(x-1, y, true);
							c2 = img->getPixel(x+1, y, true);
							
							grad.x = (double)(c2.red - c1.red)/2.0;

							//central y difference
							c1 = img->getPixel(x, y-1, true);
							c2 = img->getPixel(x, y+1, true);
							
							grad.y = (double)(c2.red - c1.red)/2.0;
							break;
						case GREEN_CHANNEL:
							//central x difference
							c1 = img->getPixel(x-1, y, true);
							c2 = img->getPixel(x+1, y, true);
							
							grad.x = (double)(c2.green - c1.green)/2.0;

							//central y difference
							c1 = img->getPixel(x, y-1, true);
							c2 = img->getPixel(x, y+1, true);
							
							grad.y = (double)(c2.green - c1.green)/2.0;
							break;
						case BLUE_CHANNEL:
							//central x difference
							c1 = img->getPixel(x-1, y, true);
							c2 = img->getPixel(x+1, y, true);
							
							grad.x = (double)(c2.blue - c1.blue)/2.0;

							//central y difference
							c1 = img->getPixel(x, y-1, true);
							c2 = img->getPixel(x, y+1, true);
							
							grad.y = (double)(c2.blue - c1.blue)/2.0;
							break;
						case ALPHA_CHANNEL:
							//central x difference
							c1 = img->getPixel(x-1, y, true);
							c2 = img->getPixel(x+1, y, true);
							
							grad.x = (double)(c2.alpha - c1.alpha)/2.0;

							//central y difference
							c1 = img->getPixel(x, y-1, true);
							c2 = img->getPixel(x, y+1, true);
							
							grad.y = (double)(c2.alpha - c1.alpha)/2.0;
							break;
						default:
							break;
					}
					
					gradientImage.push_back(grad);
				}
			}
		}

		return gradientImage;
	}

	uint64_t ComputerVision::imageHash(Image* img)
	{
		if(img == nullptr)
			return {};
		if(img->getWidth() == 0 || img->getHeight() == 0)
			return {};
		
		int width = 8;
		int height = 8;
		
		double xScale = (double)width / img->getWidth();
		double yScale = (double)height / img->getHeight();
		
		Image* scaledImg = SimpleGraphics::scaleImage(img, xScale, yScale, SimpleGraphics::NEAREST_NEIGHBOR_FILTER);
		Color* pixs = scaledImg->getPixels();

		//convert to grayscale
		std::vector<float> luminanceValues;
		float avg = 0;
		for(int i=0; i<width*height; i++)
		{
			float v = 0 + (0.299*pixs[i].red) + (0.587*pixs[i].green) + (0.114*pixs[i].blue);
			luminanceValues.push_back(v);
			avg += v;
		}
		avg /= width*height;

		delete scaledImg; //No longer need scaled image

		uint64_t resultHash = 0;
		for(int i=0; i<width*height; i++)
		{
			resultHash <<= 1;
			if(luminanceValues[i] > avg)
				resultHash |= 1;
		}

		return resultHash;
	}

	uint64_t ComputerVision::pHash(Image* img)
	{
		if(img == nullptr)
			return {};
		if(img->getWidth() == 0 || img->getHeight() == 0)
			return {};
		
		int width = 32;
		int height = 32;
		double xScale = (double)width / img->getWidth();
		double yScale = (double)height / img->getHeight();
		
		Image* scaledImg = SimpleGraphics::scaleImage(img, xScale, yScale, SimpleGraphics::NEAREST_NEIGHBOR_FILTER);
		Color* pixs = scaledImg->getPixels();

		//convert to grayscale
		Matrix lumMat = Matrix(32, 32);
		double* dataMatrix = lumMat.getData();
		for(int i=0; i<width*height; i++)
		{
			float v = 0 + (0.299*pixs[i].red) + (0.587*pixs[i].green) + (0.114*pixs[i].blue);
			dataMatrix[i] = v;
		}
		
		delete scaledImg; //No longer need scaled image

		//compute dct of 32x32 grid
		Matrix dctMatrix =  MathExt::fastCosineTransform2D(lumMat, false);
		// Matrix dctMatrix = MathExt::cosineTransform2D(lumMat, false);
		//calculate avg of 8x8 except 0,0
		double avg = 0;
		for(int y=0; y<8; y++)
		{
			for(int x=0; x<8; x++)
			{
				if(x==0 && y==0)
					continue;
				avg += dctMatrix[y][x];
			}
		}
		avg /= 63;

		uint64_t resultHash = 0;
		for(int y=0; y<8; y++)
		{
			for(int x=0; x<8; x++)
			{
				resultHash <<= 1;
				if(dctMatrix[y][x] > avg)
					resultHash |= 1;
			}
		}

		return resultHash;
	}

	uint64_t ComputerVision::dHash(Image* img)
	{
		if(img == nullptr)
			return {};
		if(img->getWidth() == 0 || img->getHeight() == 0)
			return {};
		
		int width = 9;
		int height = 8;
		
		double xScale = (double)width / img->getWidth();
		double yScale = (double)height / img->getHeight();
		
		Image* scaledImg = SimpleGraphics::scaleImage(img, xScale, yScale, SimpleGraphics::NEAREST_NEIGHBOR_FILTER);
		Color* pixs = scaledImg->getPixels();

		//convert to grayscale
		std::vector<float> luminanceValues;
		for(int i=0; i<width*height; i++)
		{
			float v = 0 + (0.299*pixs[i].red) + (0.587*pixs[i].green) + (0.114*pixs[i].blue);
			luminanceValues.push_back(v);
		}

		delete scaledImg; //No longer need scaled image

		uint64_t resultHash = 0;
		for(int y=0; y<height; y++)
		{
			for(int x=1; x<width; x++)
			{
				resultHash <<= 1;
				if(luminanceValues[x + y*width] >= luminanceValues[x-1 + y*width])
					resultHash |= 1;
			}
		}

		return resultHash;
	}
	
	int ComputerVision::hammingDistanceImageHash(uint64_t img1, uint64_t img2)
	{
		//we want the ones that don't match.
		//This is equivalent to MathExt::popcount(~(img1 ^ img2));
		int count = 0;
		for(int i=0; i<64; i++)
		{
			bool v1 = (img1>>i) & 0x01;
			bool v2 = (img2>>i) & 0x01;
			if(v1 != v2)
				count++;
		}
		return count;
	}

	Matrix ComputerVision::readjustIntensity(Matrix* baseImg, double minIntensity, double maxIntensity)
	{
		if(baseImg == nullptr)
			return Matrix();
		
		//find min and max of baseImg
		Matrix readjustedMatrix = Matrix(baseImg->getRows(), baseImg->getCols());
		size_t sizeOfBaseImg = baseImg->getRows() * baseImg->getCols();
		double* baseImgData = baseImg->getData();
		double* baseImgDataEnd = baseImg->getData() + sizeOfBaseImg;

		double minIntensityBaseImg = INFINITY;
		double maxIntensityBaseImg = -INFINITY;
		
		while(baseImgData < baseImgDataEnd)
		{
			minIntensityBaseImg = MathExt::min(*baseImgData, minIntensityBaseImg);
			maxIntensityBaseImg = MathExt::max(*baseImgData, maxIntensityBaseImg);
			baseImgData++;
		}
		
		baseImgData = baseImg->getData();
		double* readjustedMatrixData = readjustedMatrix.getData();

		double desiredRange = maxIntensity - minIntensity;
		double currentRange = maxIntensityBaseImg - minIntensityBaseImg;
		
		while(baseImgData < baseImgDataEnd)
		{
			*readjustedMatrixData = ((*baseImgData - minIntensityBaseImg) * (desiredRange / currentRange)) + minIntensity;
			baseImgData++;
			readjustedMatrixData++;
		}
		
		return readjustedMatrix;
	}

	Matrix ComputerVision::convolution(Image* baseImage, Matrix* kernel, int colorChannel, bool normalize)
	{
		if(baseImage == nullptr)
			return Matrix();
		
		Matrix output = Matrix(baseImage->getHeight(), baseImage->getWidth());
		Color* baseImagePixels = baseImage->getPixels();
		int kernelRowSizeHalf = kernel->getRows()/2;
		int kernelColSizeHalf = kernel->getCols()/2;
		double normalizationFactor = 1.0;

		if(normalize)
		{
			double kernelSum = 0;
			for(int r=0; r<kernel->getRows(); r++)
			{
				for(int c=0; c<kernel->getCols(); c++)
				{
					kernelSum += kernel->getData()[c + r*kernel->getCols()];
				}
			}

			if(kernelSum != 0.0)
				normalizationFactor = 1.0/kernelSum;
		}
		
		for(int r=0; r<output.getRows(); r++)
		{
			for(int c=0; c<output.getCols(); c++)
			{
				double sum = 0;
				for(int kernelY=kernelRowSizeHalf; kernelY >= -kernelRowSizeHalf; kernelY--)
				{
					int newImgY = r + kernelY;
					int actualKernelY = kernelY + kernelRowSizeHalf;
					if(newImgY >= 0 && newImgY < baseImage->getHeight())
					{
						for(int kernelX=kernelColSizeHalf; kernelX >= -kernelColSizeHalf; kernelX--)
						{
							int newImgX = c + kernelX;
							int actualKernelX = kernelX + kernelColSizeHalf;
							if(newImgX >= 0 && newImgX < baseImage->getWidth())
							{
								Color color = baseImagePixels[newImgX + newImgY*baseImage->getWidth()];
								double baseImgValue = ((unsigned char*)&color)[colorChannel];
								//convert to [0 - 1]
								baseImgValue /= 255.0;

								double kernelValue = kernel->getData()[actualKernelX + actualKernelY*kernel->getCols()] * normalizationFactor;
								sum += kernelValue * baseImgValue;
							}
						}
					}
				}

				output[r][c] = sum;
			}
		}
		
		return output;
	}
	
	Matrix ComputerVision::crossCorrelation(Image* baseImage, Image* kernel, int colorChannel, bool normalized)
	{
		if(baseImage == nullptr)
			return Matrix();
		
		Matrix output = Matrix(baseImage->getHeight(), baseImage->getWidth());
		int kernelRowSizeHalf = kernel->getHeight()/2;
		int kernelColSizeHalf = kernel->getWidth()/2;
		
		for(int r=0; r<output.getRows(); r++)
		{
			for(int c=0; c<output.getCols(); c++)
			{
				double sum = 0;
				double kernelEnergy = 0;
				double baseImgEnergy = 0;

				for(int kernelY=-kernelRowSizeHalf; kernelY <= kernelRowSizeHalf; kernelY++)
				{
					int newImgY = r + kernelY;
					int actualKernelY = kernelY + kernelRowSizeHalf;
					if(newImgY >= 0 && newImgY < baseImage->getHeight())
					{
						for(int kernelX=-kernelColSizeHalf; kernelX <= kernelColSizeHalf; kernelX++)
						{
							int newImgX = c + kernelX;
							int actualKernelX = kernelX + kernelColSizeHalf;
							if(newImgX >= 0 && newImgX < baseImage->getWidth())
							{
								unsigned char* c1 = (unsigned char*)&(baseImage->getPixels()[newImgX + newImgY*baseImage->getWidth()]);
								unsigned char* c2 = (unsigned char*)&(kernel->getPixels()[actualKernelX + actualKernelY*kernel->getWidth()]);

								double baseImgValue = (double)c1[colorChannel] / 255.0;
								double kernelMultiplier = (double)c2[colorChannel] / 255.0;

								if(normalized)
								{
									kernelEnergy += MathExt::sqr(kernelMultiplier);
									baseImgEnergy += MathExt::sqr(baseImgValue);
								}
								
								sum += kernelMultiplier * baseImgValue;
							}
						}
					}
				}

				if(normalized)
				{
					double totalEnergy = MathExt::sqrt(kernelEnergy + baseImgEnergy);
					if(totalEnergy != 0)
						output[r][c] = sum / totalEnergy;
					else
						output[r][c] = sum;
				}
				else
				{
					output[r][c] = sum;
				}
			}
		}
		
		return output;
	}
}