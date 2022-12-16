#include "InternalGraphicsHeader.h"

namespace glib
{

    #pragma region IMAGE_MANIPULATION
    
	int SimpleGraphics::ditherMatrixSize = 2;

	Image* SimpleGraphics::crop(Image* img, int x1, int y1, int x2, int y2)
	{
		return nullptr;
	}

	Image* SimpleGraphics::crop(Image* img, Shape s)
	{
		return nullptr;
	}

	void SimpleGraphics::replaceColor(Image* img, Color oldColor, Color newColor, bool ignoreAlpha)
	{
		if(img!=nullptr)
		{
			Color* startPix = img->getPixels();
			Color* endPix = startPix + (img->getWidth() * img->getHeight());
			while(startPix < endPix)
			{
				bool same = true;

				if(startPix->red != oldColor.red)
					same = false;
				if(startPix->green != oldColor.green)
					same = false;
				if(startPix->blue != oldColor.blue)
					same = false;
				
				if(!ignoreAlpha)
				{
					if(startPix->alpha != oldColor.alpha)
						same = false;
				}

				if(!same)
				{
					*startPix = newColor;
				}
				startPix++;
			}
		}
	}

	void SimpleGraphics::filterOutColor(Image* img, Color c1)
	{
		if(img != nullptr)
		{
			int* pixs = (int*)img->getPixels();
			int* endPixs = (int*)img->getPixels()+(img->getWidth()*img->getHeight());
			int colAsInt = *((int*)&c1);
			while(pixs < endPixs)
			{
				if(*pixs != colAsInt)
				{
					*pixs = 0;
				}
				pixs++;
			}
		}
	}

	void SimpleGraphics::filterOutColorRange(Image* img, Color c1, Color c2)
	{
		if(img != nullptr)
		{
			Color* pixs = img->getPixels();
			Color* endPixs = img->getPixels()+(img->getWidth()*img->getHeight());
			while(pixs < endPixs)
			{
				if(pixs->red < c1.red || pixs->red > c2.red)
				{
					*pixs = {0, 0, 0, 0};
				}
				if(pixs->green < c1.green || pixs->green > c2.green)
				{
					*pixs = {0, 0, 0, 0};
				}
				if(pixs->blue < c1.blue || pixs->blue > c2.blue)
				{
					*pixs = {0, 0, 0, 0};
				}
				if(pixs->alpha < c1.alpha || pixs->alpha > c2.alpha)
				{
					*pixs = {0, 0, 0, 0};
				}
				pixs++;
			}
		}
	}

	void SimpleGraphics::convertToColorSpace(Image* img, unsigned char colorSpace)
	{
		if(img != nullptr)
		{
			Color* pixs = img->getPixels();
			Color* endPixs = pixs + (img->getWidth()*img->getHeight());

			while(pixs < endPixs)
			{
				*pixs = ColorSpaceConverter::convert(*pixs, colorSpace);
				pixs++;
			}
		}
	}

	void SimpleGraphics::boxBlur(Image* img, int boxSize)
	{
		if(img != nullptr)
		{
			double multVal = 1.0/(boxSize*boxSize);
			Color* pixs = img->getPixels();
			for(int y=0; y<img->getHeight(); y++)
			{
				for(int x=0; x<img->getWidth(); x++)
				{
					//average out the pixels in the box
					double red = 0;
					double green = 0;
					double blue = 0;
					double alpha = 0;
					for(int i=-boxSize; i<boxSize; i++)
					{
						int nY = MathExt::clamp(y+i, 0, img->getHeight()-1);
						for(int j=-boxSize; j<boxSize; j++)
						{
							int nX = MathExt::clamp(x+j, 0, img->getWidth()-1);
							Color c = pixs[nX + nY*img->getWidth()];
							red += c.red * multVal;
							green += c.green * multVal;
							blue += c.blue * multVal;
							alpha += c.alpha * multVal;
						}
					}

					Color blurColor;
					blurColor.red = (unsigned char)red;
					blurColor.green = (unsigned char)green;
					blurColor.blue = (unsigned char)blue;
					blurColor.alpha = (unsigned char)alpha;
					
					pixs[x + y*img->getWidth()] = blurColor;
				}
			}
		}
	}

	Image* SimpleGraphics::gaussianBlur(Image* img, int kernelRadius, double sigma)
	{
		double actualSigma = sigma;
		if(sigma <= 0)
		{
			actualSigma = 0.5*kernelRadius;
		}

		if(kernelRadius == 0)
		{
			Image* newImg = new Image(img->getWidth(), img->getHeight());
			img->copyImage(newImg);
			return newImg;
		}

		int n = 2*kernelRadius + 1;
		Matrix verticalMat = Matrix(1, n);
		Matrix horizontalMat = Matrix(n, 1);

		double variance = MathExt::sqr(actualSigma);
		double A = 1.0/(MathExt::sqrt(2*PI*variance));

		double sigX = -kernelRadius;

		for(int i=0; i<n; i++)
		{
			double B = exp( -MathExt::sqr(sigX)/(2*variance) );
			
			verticalMat[i][0] = A*B;
			horizontalMat[0][i] = A*B;
			sigX++;
		}

		Matrix k = verticalMat*horizontalMat;
		
		double sum = 0;
		for(int j=0; j<n; j++)
		{
			for(int i=0; i<n; i++)
			{
				sum += k[j][i];
			}
		}

		k *= 1.0/sum;
		
		Image* result = new Image(img->getWidth(), img->getHeight());

		for(int y=0; y<result->getHeight(); y++)
		{
			for(int x=0; x<result->getWidth(); x++)
			{
				Vec3f kernelResult;

				for(int j=0; j<n; j++)
				{
					int actualY = y+j-kernelRadius;
					for(int i=0; i<n; i++)
					{
						int actualX = x+i-kernelRadius;
						Color c = img->getPixel(actualX, actualY, true);
						kernelResult += k[j][i] * Vec3f(c.red, c.green, c.blue);
					}
				}

				result->setPixel( x, y, {
					(unsigned char)MathExt::round(kernelResult.x),
					(unsigned char)MathExt::round(kernelResult.y),
					(unsigned char)MathExt::round(kernelResult.z),
					255
				});
			}
		}

		return result;
	}

	Image* SimpleGraphics::cannyEdgeFilter(Image* img)
	{
		return nullptr;
	}

	Image* SimpleGraphics::sobelEdgeFilter(Image* img)
	{
		return nullptr;
	}

	std::vector<std::vector<Vec2f>> SimpleGraphics::calculateGradient(Image* img, unsigned char type)
	{
		std::vector< std::vector<Vec2f> > gradientImage;
		if(img!=nullptr)
		{
			gradientImage = std::vector< std::vector<Vec2f> >(img->getHeight());

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
					
					gradientImage[y].push_back(grad);
				}
			}
		}

		return gradientImage;
	}

	void SimpleGraphics::ditherImage(Image* img, unsigned char method)
	{
		if(img!=nullptr)
		{
			if(img->getPalette().getSize()>0)
			{
				switch (method)
				{
				case ORDERED_DITHER_BAYER:
					orderedBayerDithering(img);
					break;
				case FLOYD_DITHER:
					floydSteinburgDithering(img);
					break;
				default:
					img->enforcePalette();
					break;
				}
			}
		}
	}

	void SimpleGraphics::floydSteinburgDithering(Image* img)
	{
		int wid = img->getWidth();
		int hei = img->getHeight();
		Vec3f* error = new Vec3f[wid*hei];
		for(int y=0; y<hei; y++)
		{
			for(int x=0; x<wid; x++)
			{
				Color c = img->getPixel(x,y);
				c.red = (unsigned char)MathExt::clamp((int) ((double)c.red + error[x+y*wid].x), 0, 255);
				c.green = (unsigned char)MathExt::clamp((int) ((double)c.green + error[x+y*wid].y), 0, 255);
				c.blue = (unsigned char)MathExt::clamp((int) ((double)c.blue + error[x+y*wid].z), 0, 255);
				
				Color c2 = img->getPalette().getClosestColor( c );
				c2.alpha = c.alpha;
				
				img->setPixel(x,y,c2);
				Vec3f v1 = Vec3f();
				v1.x = (int)c.red - (int)c2.red;
				v1.y = (int)c.green - (int)c2.green;
				v1.z = (int)c.blue - (int)c2.blue;
				
				if(x!=wid-1)
				{
					error[(x + 1) + y*wid] += v1*(7.0/16.0);

					if(y!=hei-1)
						error[(x + 1) + (y+1)*wid] += v1*(1.0/16.0);
				}

				if(y!=hei-1)
				{
					error[x + (y+1)*wid] += v1*(5.0/16.0);

					if(x!=0)
						error[(x - 1) + (y+1)*wid] += v1*(3.0/16.0);
				}

			}
		}

		delete[] error;
	}

	void SimpleGraphics::orderedBayerDithering(Image* img)
	{
		int rows = (int)MathExt::ceil(MathExt::sqrt(img->getPalette().getSize()));
		int size = (int)MathExt::sqr(rows);
		double exp = MathExt::log(size, 2.0);

		Matrix bayerMatrix = generateBayerMatrix(Matrix(), rows);

		bayerMatrix *= 1.0/size;
		double r = 255.0/((double)exp/3.0);

		for(int y=0; y<img->getHeight(); y++)
		{
			for(int x=0; x<img->getWidth(); x++)
			{
				int matX = x%rows;
				int matY = y%rows;

				Color c = img->getPixel(x,y);
				double multVal = bayerMatrix[matY][matX] - 0.5;
				double addVal = r * multVal;

				c.red = (unsigned char)MathExt::clamp((int) ((double)c.red + addVal), 0, 255);
				c.green = (unsigned char)MathExt::clamp((int) ((double)c.green + addVal), 0, 255);
				c.blue = (unsigned char)MathExt::clamp((int) ((double)c.blue + addVal), 0, 255);
				
				Color c2 = img->getPalette().getClosestColor( c );
				
				img->setPixel(x,y,c2);
			}
		}
	}

	Matrix SimpleGraphics::generateBayerMatrix(Matrix mat, int rowSize)
	{
		Matrix mat2;
		if(mat.getCols() == 0 || mat.getRows() == 0)
		{
			mat2 = Matrix(2, 2);
			mat2[0][0] = 0;
			mat2[0][1] = 2;
			mat2[1][0] = 3;
			mat2[1][1] = 1;
		}
		else
		{
			mat2 = Matrix(mat.getRows()*2, mat.getCols()*2);

			int inc = mat.getRows();
			for(int y=0; y<mat.getRows(); y++)
			{
				for(int x=0; x<mat.getCols(); x++)
				{
					mat2[y][x] = 4*mat[y][x] + 0;
					mat2[y][x+inc] = 4*mat[y][x] + 2;
					mat2[y+inc][x] = 4*mat[y][x] + 3;
					mat2[y+inc][x+inc] = 4*mat[y][x] + 1;
				}
			}
		}

		if(mat2.getRows() == rowSize)
		{
			return mat2;
		}
		else
		{
			return generateBayerMatrix(mat2, rowSize);
		}
	}

	Image* SimpleGraphics::scaleImage(Image* img, double xScale, double yScale, unsigned char filterMethod)
	{
		switch(filterMethod)
		{
			case NEAREST_NEIGHBOR_FILTER:
				return scaleNearestNeighbor(img, xScale, yScale);
				break;
			case BILINEAR_FILTER:
				return scaleBilinear(img, xScale, yScale);
				break;
			case BICUBIC_FILTER:
				return scaleBicubic(img, xScale, yScale);
				break;
			default:
				return scaleBilinear(img, xScale, yScale);
				break;
		}

		return nullptr;
	}

	Image* SimpleGraphics::scaleNearestNeighbor(Image* img, double xScale, double yScale)
	{
		if(img!=nullptr)
		{
			int nWidth = (int)MathExt::round(img->getWidth() * xScale);
			int nHeight = (int)MathExt::round(img->getHeight() * yScale);
			Image* sImg = new Image( nWidth, nHeight );

			//position top left of image so that it has an equal amount of distance from edges
			//divide distance by size of image to find offset

			double xOffset = (double)(nWidth - img->getWidth())/ (2*img->getWidth());
			double yOffset = (double)(nHeight - img->getHeight())/ (2*img->getHeight());

			for(int i=0; i<nHeight; i++)
			{
				double yP = ((double)(i-yOffset) / nHeight) * img->getHeight();
				int nearY = (int)MathExt::clamp(MathExt::round(yP), 0.0, (double)img->getHeight()-1);
				
				for(int i2=0; i2<nWidth; i2++)
				{
					double xP = ((double)(i2-xOffset) / nWidth) * img->getWidth();
					int nearX = (int)MathExt::clamp(MathExt::round(xP), 0.0, (double)img->getWidth()-1);
					
					sImg->setPixel(i2, i, img->getPixel(nearX, nearY));
				}
			}

			return sImg;
		}
		return nullptr;
	}

	Image* SimpleGraphics::scaleBilinear(Image* img, double xScale, double yScale)
	{
		if(img!=nullptr)
		{
			int nWidth = (int)MathExt::round(img->getWidth() * xScale);
			int nHeight = (int)MathExt::round(img->getHeight() * yScale);
			
			//position top left of image so that it has an equal amount of distance from edges
			//divide distance by size of image to find offset

			double xOffset = (double)(nWidth - img->getWidth())/ (2*img->getWidth());
			double yOffset = (double)(nHeight - img->getHeight())/ (2*img->getHeight());

			Image* sImg = new Image( nWidth, nHeight );

			for(int i=0; i<nHeight; i++)
			{
				double yP = ((double)(i-yOffset) / nHeight) * img->getHeight();
				//yP -= yOffset;

				double yFrac = MathExt::frac(yP);

				int y1 = (int)MathExt::clamp(MathExt::floor(yP), 0.0, (double)img->getHeight()-1);
				int y2 = (int)MathExt::clamp(MathExt::ceil(yP), 0.0, (double)img->getHeight()-1);
				
				for(int i2=0; i2<nWidth; i2++)
				{
					double xP = ((double)(i2-xOffset) / nWidth) * img->getWidth();
					//xP -= xOffset;
					double xFrac = MathExt::frac(xP);

					int x1 = (int)MathExt::clamp(MathExt::floor(xP), 0.0, (double)img->getWidth()-1);
					int x2 = (int)MathExt::clamp(MathExt::ceil(xP), 0.0, (double)img->getWidth()-1);
					
					Color c1 = lerp( img->getPixel(x1, y1), img->getPixel(x2, y1), xFrac);
					Color c2 = lerp( img->getPixel(x1, y2), img->getPixel(x2, y2), xFrac);
					Color c3 = lerp( c1, c2, yFrac);

					sImg->setPixel(i2, i, c3);
				}
			}

			return sImg;
		}
		return nullptr;
	}

	Image* SimpleGraphics::scaleBicubic(Image* img, double xScale, double yScale)
	{
		if(img!=nullptr)
		{
			int nWidth = (int)MathExt::round(img->getWidth() * xScale);
			int nHeight = (int)MathExt::round(img->getHeight() * yScale);
			
			//position top left of image so that it has an equal amount of distance from edges
			//divide distance by size of image to find offset

			double xOffset = (double)(nWidth - img->getWidth())/ (2*img->getWidth());
			double yOffset = (double)(nHeight - img->getHeight())/ (2*img->getHeight());


			Image* sImg = new Image( nWidth, nHeight );

			for(int i=0; i<nHeight; i++)
			{
				double yP = ((double)(i-yOffset) / nHeight) * img->getHeight();
				double yFrac = MathExt::frac(yP);

				int yPoints[4];
				yPoints[1] = (int)MathExt::clamp(MathExt::floor(yP), 0.0, (double)img->getHeight()-1);
				yPoints[0] = (int)MathExt::clamp(yPoints[1]-1, 0, img->getHeight()-1);
				yPoints[2] = (int)MathExt::clamp(MathExt::ceil(yP), 0.0, (double)img->getHeight()-1);
				yPoints[3] = (int)MathExt::clamp(yPoints[2]+1, 0, img->getHeight()-1);

				if(yPoints[0]<0)
				{
					yPoints[0] = 0;
				}
				if(yPoints[3]>=img->getHeight())
				{
					yPoints[3] = img->getHeight()-1;
				}
				
				for(int i2=0; i2<nWidth; i2++)
				{
					double xP = ((double)(i2-xOffset) / nWidth) * img->getWidth();

					double xFrac = MathExt::frac(xP);

					int xPoints[4];
					xPoints[1] = (int)MathExt::clamp(MathExt::floor(xP), 0.0, (double)img->getWidth()-1);
					xPoints[0] = (int)MathExt::clamp(xPoints[1]-1, 0, img->getWidth()-1);
					xPoints[2] = (int)MathExt::clamp(MathExt::ceil(xP), 0.0, (double)img->getWidth()-1);
					xPoints[3] = (int)MathExt::clamp(xPoints[2]+1, 0, img->getWidth()-1);

					if(xPoints[0]<0)
					{
						xPoints[0] = 0;
					}
					if(xPoints[3]>=img->getWidth())
					{
						xPoints[3] = img->getWidth()-1;
					}
					
					Vec4f arr[16];
					for(int j=0; j<16; j++)
					{
						int xV = j%4;
						int yV = j/4;
						Color c = img->getPixel(xPoints[xV], yPoints[yV]);
						arr[j] = Vec4f(c.red, c.green, c.blue, c.alpha);
					}

					Vec4f polys[4];
					for(int j=0; j<4; j++)
					{
						Vec4f a, b, c, d;
						a = (arr[j*4 + 0]*-0.5) + (arr[j*4 + 1]*1.5) + (arr[j*4 + 2]*-1.5) + (arr[j*4 + 3]*0.5);
						b = (arr[j*4 + 0]) + (arr[j*4 + 1]*-2.5) + (arr[j*4 + 2]*2) + (arr[j*4 + 3]*-0.5);
						c = (arr[j*4 + 0]*-0.5) + (arr[j*4 + 2]*0.5);
						d = arr[j*4 + 1];

						polys[j] = ((a*xFrac + b)*xFrac + c)*xFrac + d;
					}
					
					Vec4f a, b, c, d;
					a = (polys[0]*-0.5) + (polys[1]*1.5) + (polys[2]*-1.5) + (polys[3]*0.5);
					b = (polys[0]) + (polys[1]*-2.5) + (polys[2]*2) + (polys[3]*-0.5);
					c = (polys[0]*-0.5) + (polys[2]*0.5);
					d = polys[1];

					Vec4f finalC = ((a*yFrac + b)*yFrac + c)*yFrac + d;
					
					unsigned char red = (unsigned char)MathExt::clamp(finalC.x, 0.0, 255.0);
					unsigned char green = (unsigned char)MathExt::clamp(finalC.y, 0.0, 255.0);
					unsigned char blue = (unsigned char)MathExt::clamp(finalC.z, 0.0, 255.0);
					unsigned char alpha = (unsigned char)MathExt::clamp(finalC.w, 0.0, 255.0);
					
					sImg->setPixel(i2, i, {red,green,blue,alpha});
				}
			}

			return sImg;
		}
		return nullptr;
	}

	#pragma endregion

} //NAMESPACE glib END