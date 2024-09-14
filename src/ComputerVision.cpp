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
		float* dataMatrix = lumMat.getData();
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

	
	Matrix ComputerVision::guassianKernel(int kernelRadius, float sigma)
	{	
		double actualSigma = sigma;
		if(sigma <= 0)
		{
			actualSigma = 0.5*kernelRadius;
		}

		if(kernelRadius <= 0)
		{
			return Matrix();
		}

		int n = 2*kernelRadius + 1;
		Matrix kernel = Matrix(n, n);

		double variance = MathExt::sqr(actualSigma);
		double A = 1.0/(2.0*PI*variance);
		double invDenominator = 1.0 / (2.0*variance);
		double sum = 0.0;

		for(int y=1; y<=n; y++)
		{
			for(int x=1; x<=n; x++)
			{
				double numerator = MathExt::sqr(x-(kernelRadius+1)) + MathExt::sqr(y-(kernelRadius+1));
				double B = exp(-numerator*invDenominator);
				kernel[y-1][x-1] = A*B;

				sum += A*B;
			}
		}
		kernel *= 1.0/sum;
		return kernel;
	}
	
	Matrix ComputerVision::imageToMatrix(Image* img, unsigned char colorChannel)
	{
		if(img == nullptr)
			return Matrix();

		Matrix m = Matrix(img->getHeight(), img->getWidth());
		int offset = MathExt::clamp<int>(colorChannel, 0, 3);
		float* matrixValues = m.getData();
		float* endMatrixValues = m.getData()+(img->getWidth()*img->getHeight());
		unsigned char* colorValues = (unsigned char*)img->getPixels();
		colorValues += offset;

		while(matrixValues != endMatrixValues)
		{
			*matrixValues = ((float)*colorValues) / 255.0;
			matrixValues++;
			colorValues+=4;
		}
		
		return m;
	}
	
	Image* ComputerVision::matrixToImage(Matrix* mat)
	{
		if(mat == nullptr)
			return nullptr;
		Image* img = new Image(mat->getCols(), mat->getRows());
		for(int y=0; y<mat->getRows(); y++)
		{
			for(int x=0; x<mat->getCols(); x++)
			{
				Color c;
				c.red = (unsigned char)MathExt::clamp(mat->operator[](y)[x]*255.0, 0.0, 255.0);
				c.alpha = 255;
				c.green = c.red;
				c.blue = c.red;

				img->getPixels()[x + y*img->getWidth()] = c;
			}
		}
		return img;
	}
	void ComputerVision::matrixToImage(Matrix* mat, Image* img, unsigned char colorChannel)
	{
		if(mat == nullptr || img == nullptr)
			return;
		
		int offset = MathExt::clamp<int>(colorChannel, 0, 3);
		unsigned char* imgPixels = (unsigned char*)img->getPixels();
		imgPixels += offset;

		for(int y=0; y<mat->getRows(); y++)
		{
			for(int x=0; x<mat->getCols(); x++)
			{
				*imgPixels = (unsigned char)MathExt::clamp(mat->operator[](y)[x]*255.0, 0.0, 255.0);
				imgPixels += 4;
			}
		}
	}

	Matrix ComputerVision::thresholding(Image* img, unsigned char threshold, unsigned char colorChannel, bool inverse)
	{
		Matrix m = Matrix(img->getHeight(), img->getWidth());
		int offset = MathExt::clamp<int>(colorChannel, 0, 3);
		float* matrixValues = m.getData();
		float* endMatrixValues = m.getData()+(img->getWidth()*img->getHeight());
		
		unsigned char* colorValues = (unsigned char*)img->getPixels();
		colorValues += offset;
		if(!inverse)
		{
			while(matrixValues != endMatrixValues)
			{
				*matrixValues = (*colorValues >= threshold) ? 1.0 : 0.0;
				matrixValues++;
				colorValues+=4;
			}
		}
		else
		{
			while(matrixValues != endMatrixValues)
			{
				*matrixValues = (*colorValues <= threshold) ? 1.0 : 0.0;
				matrixValues++;
				colorValues+=4;
			}
		}
	
		return m;
	}

	Matrix ComputerVision::thresholding(Matrix* img, float threshold, bool inverse)
	{
		Matrix m = Matrix(img->getRows(), img->getCols());
		float* matrixValues = m.getData();
		float* endMatrixValues = m.getData()+(img->getRows()*img->getCols());
		float* otherMatrixValues = img->getData();

		if(!inverse)
		{
			while(matrixValues != endMatrixValues)
			{
				*matrixValues = (*otherMatrixValues >= threshold) ? 1.0 : 0.0;
				matrixValues++;
				otherMatrixValues++;
			}
		}
		else
		{
			while(matrixValues != endMatrixValues)
			{
				*matrixValues = (*otherMatrixValues <= threshold) ? 1.0 : 0.0;
				matrixValues++;
				otherMatrixValues++;
			}
		}
	
		return m;
	}

	Matrix ComputerVision::adaptiveThresholding(Image* img, unsigned char adjustment, unsigned char radius, unsigned char colorChannel, unsigned char mode, bool inverse)
	{
		Matrix m = Matrix(img->getHeight(), img->getWidth());
		int offset = MathExt::clamp<int>(colorChannel, 0, 3);
		float* matrixValues = m.getData();
		float* endMatrixValues = m.getData()+(img->getWidth()*img->getHeight());
		
		Color* colorValues = img->getPixels();

		Matrix weights;
		if(mode == THRESH_ADAPTIVE_MEAN)
		{
			weights = Matrix(2*radius + 1, 2*radius + 1);
			float weightValue = 1.0 / (weights.getRows()*weights.getCols());
			for(int i=0; i<2*radius+1; i++)
			{
				for(int j=0; j<2*radius+1; j++)
				{
					weights[i][j] = weightValue;
				}
			}
		}
		else if(mode == THRESH_ADAPTIVE_GUASSIAN)
		{
			weights = ComputerVision::guassianKernel(radius, 1.0);
		}

		for(int y=0; y<img->getHeight(); y++)
		{
			for(int x=0; x<img->getWidth(); x++)
			{
				//compute the mean and std dev
				//calculate using a 9x9 kernel (4 pixels left; 4 pixels right)
				float avg = 0;
				int count = 0;
				int x1 = MathExt::clamp(x-radius, 0, img->getWidth()-1);
				int x2 = MathExt::clamp(x+radius, 0, img->getWidth()-1);
				int y1 = MathExt::clamp(y-radius, 0, img->getHeight()-1);
				int y2 = MathExt::clamp(y+radius, 0, img->getHeight()-1);
				for(int i=y1; i<=y2; i++)
				{
					int kernelY = (i-y) + radius;
					for(int j=x1; j<=x2; j++)
					{
						Color c = colorValues[j + i*img->getWidth()];
						unsigned char v = *(((unsigned char*)&c)+offset);
						int kernelX = (j-x) + radius;
						
						avg += v * weights[kernelY][kernelX];
						count++;
					}
				}
				avg -= adjustment;
				
				Color c = colorValues[x + y*img->getWidth()];
				float v = *(((unsigned char*)&c)+offset);

				if(!inverse)
				{
					if(v > avg)
						*matrixValues = 1.0;
				}
				else
				{
					if(v < avg)
						*matrixValues = 1.0;
				}
				matrixValues++;
			}
		}

		return m;
	}

	Matrix ComputerVision::readjustIntensity(Matrix* baseImg, double minIntensity, double maxIntensity)
	{
		if(baseImg == nullptr)
			return Matrix();
		
		//find min and max of baseImg
		Matrix readjustedMatrix = Matrix(baseImg->getRows(), baseImg->getCols());
		size_t sizeOfBaseImg = baseImg->getRows() * baseImg->getCols();
		float* baseImgData = baseImg->getData();
		float* baseImgDataEnd = baseImg->getData() + sizeOfBaseImg;

		double minIntensityBaseImg = *baseImgData;
		double maxIntensityBaseImg = *baseImgData;
		
		while(baseImgData < baseImgDataEnd)
		{
			minIntensityBaseImg = MathExt::min<double>(*baseImgData, minIntensityBaseImg);
			maxIntensityBaseImg = MathExt::max<double>(*baseImgData, maxIntensityBaseImg);
			baseImgData++;
		}
		
		baseImgData = baseImg->getData();
		float* readjustedMatrixData = readjustedMatrix.getData();

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

	Matrix ComputerVision::convolution(Matrix* baseImage, Matrix* kernel)
	{
		if(baseImage == nullptr)
			return Matrix();
		
		Matrix output = Matrix(baseImage->getRows(), baseImage->getCols());

		float* baseImagePixels = baseImage->getData();
		int kernelRowSizeHalf = kernel->getRows()/2;
		int kernelColSizeHalf = kernel->getCols()/2;
		
		for(int r=0; r<output.getRows(); r++)
		{
			for(int c=0; c<output.getCols(); c++)
			{
				double sum = 0;
				for(int kernelY=kernelRowSizeHalf; kernelY >= -kernelRowSizeHalf; kernelY--)
				{
					int newImgY = r + kernelY;
					int actualKernelY = kernelY + kernelRowSizeHalf;
					if(newImgY >= 0 && newImgY < baseImage->getRows())
					{
						for(int kernelX=kernelColSizeHalf; kernelX >= -kernelColSizeHalf; kernelX--)
						{
							int newImgX = c + kernelX;
							int actualKernelX = kernelX + kernelColSizeHalf;
							if(newImgX >= 0 && newImgX < baseImage->getCols())
							{
								float baseImgValue = baseImagePixels[newImgX + newImgY*baseImage->getCols()];
								float kernelValue = kernel->getData()[actualKernelX + actualKernelY*kernel->getCols()];
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
	
	Matrix ComputerVision::crossCorrelation(Matrix* baseImage, Matrix* kernel, bool normalized)
	{
		if(baseImage == nullptr)
			return Matrix();
		
		Matrix output = Matrix(baseImage->getRows(), baseImage->getCols());
		int kernelRowSizeHalf = kernel->getRows()/2;
		int kernelColSizeHalf = kernel->getCols()/2;
		
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
					if(newImgY >= 0 && newImgY < baseImage->getRows())
					{
						for(int kernelX=-kernelColSizeHalf; kernelX <= kernelColSizeHalf; kernelX++)
						{
							int newImgX = c + kernelX;
							int actualKernelX = kernelX + kernelColSizeHalf;
							if(newImgX >= 0 && newImgX < baseImage->getCols())
							{
								float baseImgValue = baseImage->getData()[newImgX + newImgY*baseImage->getRows()];
								float kernelMultiplier = kernel->getData()[actualKernelX + actualKernelY*kernel->getRows()];
								
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

	void removeBoundaryShape(Matrix* m, std::vector<Vec2f>& boundaryShape)
	{
		std::vector<std::vector<std::pair<int, bool>>> criticalPoints = std::vector<std::vector<std::pair<int, bool>>>(m->getRows());
    	std::vector<std::vector<std::pair<int, int>>> horizontalLines = std::vector<std::vector<std::pair<int, int>>>(m->getRows());

		for(int i=0; i<boundaryShape.size(); i++)
		{
			int p1 = i;
			int p2 = (i+1) % boundaryShape.size();
			Vec2f v1 = boundaryShape[p1];
			Vec2f v2 = boundaryShape[p2];
			Vec2f toPoint = v2-v1;
			BezierCurve b;
			b.addPoint(v1);
			b.addPoint(v2);

			if(toPoint.y == 0)
			{
				//special case
				if(v1.y >= 0 && v1.y < criticalPoints.size())
				{
					//line must be going to the right.
					if(v2.x >= v1.x)
						horizontalLines[(int)v1.y].push_back({(int)v1.x, (int)v2.x});
					else
						horizontalLines[(int)v1.y].push_back({(int)v2.x, (int)v1.x});
				}
				continue;
			}

			for(int y=0; y<criticalPoints.size(); y++)
			{
				std::vector<double> timeValues = b.findTimeForY(y);
				for(double& d : timeValues)
				{
					if(d >= 0 && d <= 1)
					{
						Vec2f intersection = b.getFuctionAt(d);
						if(d >= 0 && d <= 1)
							criticalPoints[y].push_back({(int)intersection.x, (toPoint.y > 0)});
					}
				}
			}
		}

		//group up the horizontal lines
		for(int y=0; y<horizontalLines.size(); y++)
		{
			if(horizontalLines[y].size() > 1)
			{
				std::vector<std::pair<int, int>> newLines;
				Sort::mergeSort<std::pair<int, int>>(horizontalLines[y].data(), horizontalLines[y].size(), [](std::pair<int, int> A, std::pair<int, int> B) ->bool{
					if(A.first == B.first)
						return A.second < B.second;
					return A.first < B.first;
				});
				//sorted by first vertex
				std::pair<int, int> mergedLine = horizontalLines[y][0];
				for(int i=1; i<horizontalLines[y].size(); i++)
				{
					std::pair<int, int> nextLine = horizontalLines[y][i];
					//sorted so nextLine.first >= previousLine.first which means it is >= mergedLine.first
					if(nextLine.first <= mergedLine.second)
					{
						mergedLine.second = __max(nextLine.second, mergedLine.second);
					}
					else
					{
						//disjointed horizontal lines. Add mergedLine to newLines and replace mergedLine
						newLines.push_back(mergedLine);
						mergedLine = nextLine;
					}
				}
				newLines.push_back(mergedLine);
				horizontalLines[y] = newLines;
			}
		}
		


		for(int y=0; y<criticalPoints.size(); y++)
		{
			Sort::mergeSort<std::pair<int, bool>>(criticalPoints[y].data(), criticalPoints[y].size(), [](std::pair<int, bool> A, std::pair<int, bool> B) ->bool{
				if(A.first == B.first)
					return A.second < B.second;
				return A.first < B.first;
			});

			std::vector<int> reducedPoints = std::vector<int>();
			bool lastSlope = false;
			for(int k=0; k<criticalPoints[y].size(); k++)
			{
				if(k > 0 && criticalPoints[y][k-1] == criticalPoints[y][k])
					continue; //skip duplicate point
				
				if(reducedPoints.size() > 0)
				{
					bool connected = false;
					for(std::pair<int, int>& potentialMatch : horizontalLines[y])
					{
						if(potentialMatch.first <= criticalPoints[y][k].first)
						{
							if(potentialMatch.second >= reducedPoints.back())
							{
								connected = true;
								break;
							}
						}
					}

					if(connected)
					{
						//special case. Connected by a horizontal line.
						if(lastSlope != criticalPoints[y][k].second)
						{
							//ensure data between them is included too
							if(reducedPoints.size() % 2 == 0)
							{
								reducedPoints.push_back(reducedPoints.back());
								reducedPoints.push_back(criticalPoints[y][k].first);
							}
							reducedPoints.push_back(criticalPoints[y][k].first);
							lastSlope = criticalPoints[y][k].second;
						}
						else
						{
							//update last point if it was supposed to be a maximum
							if(reducedPoints.size() % 2 == 0)
							{
								reducedPoints.back() = criticalPoints[y][k].first;
							}
						}
					}
					else
					{
						reducedPoints.push_back(criticalPoints[y][k].first);
						lastSlope = criticalPoints[y][k].second;
					}
				}
				else
				{
					reducedPoints.push_back(criticalPoints[y][k].first);
					lastSlope = criticalPoints[y][k].second;
				}
			}

			int s = (reducedPoints.size() >> 1) << 1;
			for(int i=0; i<s; i+=2)
			{
				int x1 = reducedPoints[i];
				int x2 = reducedPoints[i+1];
				for(int x=x1; x<=x2; x++)
				{
					m->operator[](y)[x] = 0;
				}
			}
		}
	}
	
	void ComputerVision::mooreNeighborTracing(Matrix* m, int x, int y, int preX, int preY, std::vector<Vec2f>& points)
	{
		//moore-neighbor tracing
		const std::pair<int, int> boundaryPointsClockwise[8] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};

		Vec2f newPoint = Vec2f(x, y);
		Vec2f enterPoint = Vec2f(preX, preY);

		while(true)
		{
			if(newPoint.x < 0 || newPoint.y < 0)
				break;

			//if this point is the first point
			if(newPoint.x == x && newPoint.y == y && points.size() > 0)
				break;
			
			points.push_back(newPoint);

			//find start of the boundary checks
			int tempX = enterPoint.x - newPoint.x;
			int tempY = enterPoint.y - newPoint.y;
			int startP = -1;
			for(int i=0; i<8; i++)
			{
				if(tempX == boundaryPointsClockwise[i].first && tempY == boundaryPointsClockwise[i].second)
				{
					startP = i % 8;
					break;
				}
			}

			if(startP < 0)
				return; //problem
			
			Vec2f nextPotentialPoint = {-1, -1};
			int oldCheckX, oldCheckY;
			for(int i=0; i<8; i++)
			{
				//move around pixel clockwise till a white pixel is found (white being a valid pixel and black being background).
				std::pair<int, int> adjustments = boundaryPointsClockwise[startP];
				int checkX = newPoint.x + adjustments.first;
				int checkY = newPoint.y + adjustments.second;

				//assume thresholding
				//returns 0 if invalid as well
				if(m->get(checkY, checkX) > 0)
				{
					//valid pixel. Part of boundary.
					nextPotentialPoint = Vec2f(checkX, checkY);
					enterPoint = Vec2f(oldCheckX, oldCheckY);
					break;
				}
				
				oldCheckX = checkX;
				oldCheckY = checkY;
				startP = (startP+1) % 8;
			}

			newPoint = nextPotentialPoint;
		}
	}

	std::vector<std::vector<Vec2f>> ComputerVision::findContours(Image* img)
	{
		//just use thresholding. Assume its already grayscale and thresholded.
		std::vector<std::vector<Vec2f>> allShapesFound;
		Matrix thresholdedMat = ComputerVision::imageToMatrix(img, ComputerVision::RED_CHANNEL);

		for(int y=0; y<thresholdedMat.getRows(); y++)
		{
			for(int x=0; x<thresholdedMat.getCols(); x++)
			{
				if(thresholdedMat[y][x] != 0.0)
				{
					std::vector<Vec2f> boundaryPoints;
					mooreNeighborTracing(&thresholdedMat, x, y, x-1, y, boundaryPoints);
					if(!boundaryPoints.empty())
					{
						//found a contour.
						allShapesFound.push_back(boundaryPoints);

						//remove all stuff captured by the contour. Helps to prevent duplicates
						//its a polygon but the lines are really short
						removeBoundaryShape(&thresholdedMat, boundaryPoints);
					}
				}
			}
		}
		return allShapesFound;
	}
}