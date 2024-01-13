#include "ComputerVision.h"

namespace glib
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

	Matrix ComputerVision::convolution(Image* baseImage, Matrix* kernel, int colorChannel)
	{
		if(baseImage == nullptr || kernel == nullptr)
			return Matrix();
		
		Matrix output = Matrix(baseImage->getHeight(), baseImage->getWidth());
		int* baseImageDataArr = (int*)baseImage->getPixels();
		double* kernelDataArr = kernel->getData();

		int baseImageRows = baseImage->getHeight();
		
		int kernelCols = kernel->getCols();
		int kernelRows = kernel->getRows();

		int kernelColsDiv2 = kernel->getCols()/2;
		int kernelRowsDiv2 = kernel->getRows()/2;

		for(int r=0; r<output.getRows(); r++)
		{
			int minY = MathExt::min(r-kernelRowsDiv2, 0);
			int maxY = MathExt::max(r+kernelRowsDiv2, output.getRows()-1);

			int kernelYMin = kernelRowsDiv2 - r;
			if(r >= kernelRowsDiv2)
				kernelYMin = 0;
			else
				kernelYMin = MathExt::abs(kernelYMin);
			
			int kernelYMax = (output.getRows() - r)+kernelRowsDiv2;
			if(kernelYMax >= kernelRows)
				kernelYMax = kernelRows;
			
			
			for(int c=0; c<output.getCols(); c++)
			{
				int minX = MathExt::min(c-kernelColsDiv2, 0);
				int maxX = MathExt::max(c+kernelColsDiv2, output.getCols()-1);

				int kernelXMin = kernelColsDiv2 - c;
				if(c >= kernelColsDiv2)
					kernelXMin = 0;
				else
					kernelXMin = MathExt::abs(kernelXMin);
				
				int kernelXMax = (output.getCols() - c)+kernelColsDiv2;
				if(kernelXMax >= kernelCols)
					kernelXMax = kernelCols;
			}
		}
		
		// for(int r=0; r<output.getRows(); r++)
		// {
		// 	for(int c=0; c<output.getCols(); c++)
		// 	{
		// 		double sum = 0;

		// 		for(int y=-kernelColsDiv2, kernelY=kernelCols-1; y<=kernelColsDiv2; y++, kernelY--)
		// 		{
		// 			if(y+r < baseImage->getHeight() && y+r >= 0)
		// 			{
		// 				for(int x=-kernelColsDiv2, kernelX=kernelRows-1; x<=kernelColsDiv2; x++, kernelX--)
		// 				{
		// 					if(x+c < baseImage->getWidth() && x+c >= 0)
		// 					{
		// 						unsigned char* pixelAsByteArr = (unsigned char*)&baseImageDataArr[(r+y)*baseImageRows + (c+x)];
		// 						double actualColorValue = ((double)pixelAsByteArr[colorChannel]) / 255.0;
		// 						sum += actualColorValue * kernelDataArr[(kernelY)*kernelRows + (kernelX)];
		// 					}
		// 				}
		// 			}
		// 		}

		// 		output[r][c] = sum;
		// 	}
		// }
		return output;
	}

	Matrix ComputerVision::convolutionNormalized(Image* baseImage, Matrix* kernel, int colorChannel)
	{
		if(baseImage == nullptr || kernel == nullptr)
			return Matrix();
		
		Matrix output = Matrix(baseImage->getHeight(), baseImage->getWidth());
		int* baseImageDataArr = (int*)baseImage->getPixels();
		double* kernelDataArr = kernel->getData();

		int baseImageRows = baseImage->getHeight();
		
		int kernelCols = kernel->getCols();
		int kernelRows = kernel->getRows();
		
		int kernelColsDiv2 = kernel->getCols()/2;
		int kernelRowsDiv2 = kernel->getRows()/2;

		double kernelEnergy = 0;
		for(int i=0; i<kernel->getRows()*kernel->getCols(); i++)
		{
			kernelEnergy += kernelDataArr[i]*kernelDataArr[i];
		}
		kernelEnergy = MathExt::sqrt(kernelEnergy);
		
		for(int r=0; r<output.getRows(); r++)
		{
			for(int c=0; c<output.getCols(); c++)
			{
				double sum = 0;
				double baseImageEnergySum = 0;

				for(int y=-kernelColsDiv2, kernelY=kernelRows-1; y<=kernelColsDiv2; y++, kernelY--)
				{
					if(y+r < baseImage->getHeight() && y+r >= 0)
					{
						for(int x=-kernelColsDiv2, kernelX=kernelCols-1; x<=kernelColsDiv2; x++, kernelX--)
						{
							if(x+c < baseImage->getWidth() && x+c >= 0)
							{
								unsigned char* pixelAsByteArr = (unsigned char*)&baseImageDataArr[(r+y)*baseImageRows + (c+x)];
								double actualColorValue = ((double)pixelAsByteArr[colorChannel]) / 255.0;
								baseImageEnergySum += (actualColorValue * actualColorValue);
								
								sum += actualColorValue * kernelDataArr[(kernelY)*kernelRows + (kernelX)];
							}
						}
					}
				}

				baseImageEnergySum = MathExt::sqrt(baseImageEnergySum);
				double energyWeight = baseImageEnergySum*kernelEnergy;
				
				if(energyWeight == 0)
					output[r][c] = 0; //either all of the image in the area is 0 or all of the kernel is 0. So everything should be 0 anyway.
				else
					output[r][c] = sum / energyWeight;
			}
		}
		return output;
	}
	
	Matrix ComputerVision::crossCorrelation(Image* baseImage, Image* kernel, int colorChannel)
	{
		if(baseImage == nullptr || kernel == nullptr)
			return Matrix();
		
		Matrix output = Matrix(baseImage->getHeight(), baseImage->getWidth());
		int* baseImageDataArr = (int*)baseImage->getPixels();
		int* kernelDataArr = (int*)kernel->getPixels();

		int baseImageRows = baseImage->getHeight();
		int kernelRows = kernel->getHeight();
		
		int kernelColsDiv2 = kernel->getWidth()/2;
		int kernelRowsDiv2 = kernel->getHeight()/2;

		double kernelEnergy = 0;
		for(int i=0; i<kernel->getWidth()*kernel->getHeight(); i++)
		{
			unsigned char* kernelDataAsByteArr = (unsigned char*)&kernelDataArr[i];
			kernelEnergy += MathExt::sqr((double)kernelDataAsByteArr[colorChannel] / 255.0);
		}
		kernelEnergy = MathExt::sqrt(kernelEnergy);
		
		for(int r=0; r<output.getRows(); r++)
		{
			for(int c=0; c<output.getCols(); c++)
			{
				double sum = 0;
				double baseImageEnergySum = 0;

				for(int y=-kernelRowsDiv2, kernelY=0; kernelY<kernel->getHeight(); y++, kernelY++)
				{
					if(y+r < baseImage->getHeight() && y+r >= 0)
					{
						for(int x=-kernelColsDiv2, kernelX=0; kernelX<kernel->getWidth(); x++, kernelX++)
						{
							if(x+c < baseImage->getWidth() && x+c >= 0)
							{
								unsigned char* pixelAsByteArr = (unsigned char*)&baseImageDataArr[(r+y)*baseImage->getWidth() + (c+x)];
								double actualColorValue = ((double)pixelAsByteArr[colorChannel]) / 255.0;
								baseImageEnergySum += (actualColorValue * actualColorValue);

								unsigned char* kernelDataAsByteArr = (unsigned char*)&kernelDataArr[(kernelY)*kernel->getWidth() + (kernelX)];
								double actualKernelColorValue = ((double)kernelDataAsByteArr[colorChannel]) / 255.0;
								
								sum += actualColorValue * actualKernelColorValue;
							}
						}
					}
				}
				baseImageEnergySum = MathExt::sqrt(baseImageEnergySum);
				double energyWeight = baseImageEnergySum*kernelEnergy;
				
				if(energyWeight == 0)
					output[r][c] = 0; //either all of the image in the area is 0 or all of the kernel is 0. So everything should be 0 anyway.
				else
					output[r][c] = sum / energyWeight;
			}
		}
		return output;
	}
}