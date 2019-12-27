#include "Graphics.h"
#include "MathExt.h"
#include <iostream>

Color Graphics::activeColor = { 0, 0, 0, 255 };
Font* Graphics::activeFont = nullptr;
Image* Graphics::activeImage = nullptr;

void Graphics::setImage(Image* img)
{
	activeImage = img;
}

Image* Graphics::getImage()
{
	return activeImage;
}

void Graphics::clearImage(Image* surf)
{
	if (surf == nullptr)
	{
		if (activeImage != nullptr)
		{
			activeImage->setAllPixels(activeColor);
		}
	}
	else
	{
		surf->setAllPixels(activeColor);
	}
}

//works properly now
void Graphics::drawRect(int x, int y, int x2, int y2, bool outline, Image* surf)
{
	//Will pointers for modifying the data
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		int tempWidth = otherImg->getWidth();

		int minX = MathExt::clamp(MathExt::min(x, x2), 0, tempWidth);
		int maxX = MathExt::clamp(MathExt::max(x, x2), 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(MathExt::min(y, y2), 0, tempHeight);
		int maxY = MathExt::clamp(MathExt::max(y, y2), 0, tempHeight);

		Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
		Color* endPoint = otherImg->getPixels() + maxX + ((maxY-1) * tempWidth);

		int offWidth = maxX - minX;
		int addAmount = tempWidth - offWidth;


		int tX = 0;

		if (outline == false)
		{
			while (startPoint < endPoint)
			{
				*startPoint = activeColor;

				startPoint++;
				tX++;

				if (tX >= offWidth)
				{
					tX = 0;
					startPoint += addAmount;
				}
			}
		}
		else
		{
			int tY = minY;
			while (startPoint < endPoint)
			{
				if (tY == minY || tY == maxY)
				{
					*startPoint = activeColor;
				}
				else
				{
					*startPoint = activeColor;
					tX += offWidth-1;
					startPoint += offWidth-1;
					*startPoint = activeColor;
				}

				startPoint++;
				tX++;

				if (tX >= offWidth)
				{
					tX = 0;
					startPoint += addAmount;
					tY++;
				}
			}
		}
	}
}

void Graphics::drawLine(int x1, int y1, int x2, int y2, Image* surf)
{
	//Will pointers for modifying the data
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		//avoid using floats as much as possible

		//determine if it is on the line
		//Using general equation
		//Ax + By = C
		int minX = MathExt::clamp(MathExt::min(x1, x2), 0, otherImg->getWidth());
		int maxX = MathExt::clamp(MathExt::max(x1, x2), 0, otherImg->getWidth());
		int minY = MathExt::clamp(MathExt::min(y1, y2), 0, otherImg->getHeight());
		int maxY = MathExt::clamp(MathExt::max(y1, y2), 0, otherImg->getHeight());

		int xRate = (y2 - y1);
		int yRate = (x1 - x2);
		int constVal = (x1*xRate)+(y1*yRate);

		//for now, keep in for loop
		int x = minX;
		int y = minY;

		do
		{
			do
			{
				int testVal = (x * xRate) + (y * yRate);
				if ( testVal == constVal )
				{
					//is directly on the line
					otherImg->setPixel(x, y, activeColor);
				}
				else
				{
					//find how far the testVal is from the actual value
					//assume a stroke width of 1 for now
					//additional stroke width will be handled by a different function
					if (MathExt::abs(testVal - constVal) - 1 == 0)
					{
						//consider it valid for now.
						otherImg->setPixel(x, y, activeColor);
					}
					else
					{
						break;
					}
				}
				x++;
			}
			while (x < maxX);
			y++;
		}
		while (y < maxY);

	}
}

void Graphics::drawCircle(int x, int y, int radius, bool outline, Image* surf)
{
	//Will pointers for modifying the data
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		int tempWidth = otherImg->getWidth();

		int minX = MathExt::clamp(MathExt::min(x-radius, x+radius), 0, tempWidth);
		int maxX = MathExt::clamp(MathExt::max(x-radius, x+radius), 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(MathExt::min(y-radius, y+radius), 0, tempHeight);
		int maxY = MathExt::clamp(MathExt::max(y-radius, y+radius), 0, tempHeight);

		Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
		Color* endPoint = otherImg->getPixels() + maxX + (maxY * tempWidth);

		int offWidth = maxX - minX;
		int addAmount = tempWidth - offWidth;

		int tX = minX;
		int tY = minY;
		double radSqr = MathExt::sqr(radius);

		if (outline == false)
		{
			while (startPoint < endPoint)
			{
				double dis = MathExt::sqr(tX + 0.5 - x) + MathExt::sqr(tY + 0.5 - y);

				if (dis <= radSqr)
				{
					*startPoint = activeColor;
				}
				startPoint++;
				tX++;

				if (tX >= maxX)
				{
					tX = minX;
					tY++;
					startPoint += addAmount;
				}
			}
		}
		else
		{
			while (startPoint < endPoint)
			{
				int dis = MathExt::sqr(tX + 0.5 - x) + MathExt::sqr(tY + 0.5 - y);

				if (dis <= radSqr)
				{
					*startPoint = activeColor;
				}
				startPoint++;
				tX++;

				if (tX >= maxX)
				{
					tX = minX;
					tY++;
					startPoint += addAmount;
				}
			}
		}
	}
}

//Works properly now
void Graphics::drawSprite(Image* img, int x, int y, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		int tempWidth = otherImg->getWidth();

		int minX = MathExt::clamp(x, 0, tempWidth);
		int maxX = MathExt::clamp(x+img->getWidth(), 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(y, 0, tempHeight);
		int maxY = MathExt::clamp(y+img->getHeight(), 0, tempHeight);

		Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
		Color* endPoint = otherImg->getPixels() + maxX + ((maxY-1) * tempWidth);
		
		int offWidth = maxX - minX;
		int addAmount = tempWidth - offWidth;

		Color* otherStartPoint = img->getPixels();
		int otherAddAmount = 0;

		int tX = 0;

		while (startPoint < endPoint)
		{
			*startPoint = *otherStartPoint;

			startPoint++;
			otherStartPoint++;
			tX++;

			if (tX >= offWidth)
			{
				tX = 0;
				startPoint += addAmount;
				otherStartPoint += otherAddAmount;
			}
		}
	}
}

//Test to see if it works properly later
void Graphics::drawSpritePart(Image* img, int x, int y, int imgX, int imgY, int imgW, int imgH, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		int tempWidth = otherImg->getWidth();

		int minX = MathExt::clamp(x, 0, tempWidth);
		int maxX = MathExt::clamp(x + imgW, 0, tempWidth);

		int tempHeight = otherImg->getHeight();

		int minY = MathExt::clamp(y, 0, tempHeight);
		int maxY = MathExt::clamp(y + imgH, 0, tempHeight);

		Color* startPoint = otherImg->getPixels() + minX + (minY * tempWidth);
		Color* endPoint = otherImg->getPixels() + maxX + ((maxY - 1) * tempWidth);

		int offWidth = maxX - minX;
		int addAmount = tempWidth - offWidth;

		Color* otherStartPoint = img->getPixels() + imgX + (imgY*img->getWidth());
		
		int otherAddAmount = img->getWidth() - imgW;

		int tX = 0;

		while (startPoint < endPoint)
		{
			*startPoint = *otherStartPoint;

			startPoint++;
			otherStartPoint++;
			tX++;

			if (tX >= offWidth)
			{
				tX = 0;
				startPoint += addAmount;
				otherStartPoint += otherAddAmount;
			}
		}
	}
}

void Graphics::drawText(std::string str, int x, int y, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
	}
}

void Graphics::setColor(Color c)
{
	activeColor = c;
}

Color Graphics::getColor()
{
	return activeColor;
}

void Graphics::setFont(Font* f)
{
	activeFont = f;
}

Font* Graphics::getFont()
{
	return activeFont;
}
