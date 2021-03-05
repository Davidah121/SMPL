#include "Graphics.h"
#include "MathExt.h"
#include <iostream>
#include "Sort.h"
#include "StringTools.h"

#pragma region DRAWING_FUNCTIONS

Color Graphics::activeColor = { 0, 0, 0, 255 };
BitmapFont* Graphics::activeFont = nullptr;
Image* Graphics::activeImage = nullptr;
unsigned char Graphics::compositeRule = Graphics::COMPOSITE_SRC_OVER;
unsigned char Graphics::blendMode = Graphics::BLEND_NORMAL;
bool Graphics::fillRule = Graphics::FILL_EVEN_ODD;
bool Graphics::antiAliasing = false;

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

void Graphics::drawPixel(double x, double y, Color c, Image* surf)
{
	//convert into four separate pixels
	Vec2f p1 = Vec2f(floor(x), floor(y));
	Vec2f p2 = Vec2f(ceil(x), ceil(y));

	float xAlpha = 1.0f - (x-p1.x);
	float yAlpha = 1.0f - (y-p1.y);

	Color c1, c2, c3, c4;
	c1 = c;
	c1.alpha = (unsigned char) (c1.alpha * xAlpha * yAlpha);

	c2 = c;
	c2.alpha = (unsigned char) (c2.alpha * (1-xAlpha) * yAlpha);
	
	c3 = c;
	c3.alpha = (unsigned char) (c3.alpha * (1-xAlpha) * (1-yAlpha));
	
	c4 = c;
	c4.alpha = (unsigned char) (c4.alpha * xAlpha * (1-yAlpha));
	
	drawPixel( (int)p1.x, (int)p1.y, c1, surf);
	drawPixel( (int)p2.x, (int)p1.y, c2, surf);
	drawPixel( (int)p2.x, (int)p2.y, c3, surf);
	drawPixel( (int)p1.x, (int)p2.y, c4, surf);
}

void Graphics::drawPixel(int x, int y, Color c, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	float Fa = 0;
	float Fb = 0;

	int red = 0;
	int green = 0;
	int blue = 0;
	int alpha = 0;

	//other stuff to
	float tAlpha1 = (float)c.alpha/255;
	Color otherColor = otherImg->getPixel(x,y);
	float tAlpha2 = (float)otherColor.alpha/255;

	if (otherImg != nullptr)
	{
		switch(compositeRule)
		{
			case COMPOSITE_CLEAR:
				Fa=0;
				Fb=0;
				break;
			case COMPOSITE_COPY:
				Fa = 1;
				Fb = 0;
				break;
			case COMPOSITE_DEST:
				Fa = 0;
				Fb = 1;
				break;
			case COMPOSITE_SRC_OVER:
				Fa = 1;
				Fb = 1-tAlpha1;
				break;
			case COMPOSITE_DEST_OVER:
				Fa = 1-tAlpha2;
				Fb = 1;
				break;
			case COMPOSITE_SRC_IN:
				Fa = tAlpha2;
				Fb = 0;
				break;
			case COMPOSITE_DEST_IN:
				Fa = 0;
				Fb = tAlpha1;
				break;
			case COMPOSITE_SRC_OUT:
				Fa = 1-tAlpha2;
				Fb = 0;
				break;
			case COMPOSITE_DEST_OUT:
				Fa = 0;
				Fb = 1-tAlpha1;
				break;
			case COMPOSITE_SRC_ATOP:
				Fa = tAlpha2;
				Fb = 1-tAlpha1;
				break;
			case COMPOSITE_DEST_ATOP:
				Fa = 1-tAlpha2;
				Fb = tAlpha1;
				break;
			case COMPOSITE_XOR:
				Fa = 1-tAlpha2;
				Fb = 1-tAlpha1;
				break;
			case COMPOSITE_LIGHTER:
				Fa = 1;
				Fb = 1;
				break;
			default:
				Fa = 0;
				Fb = 0;
				break;
		}

		red = (tAlpha1 * c.red * Fa) + (tAlpha2 * otherColor.red * Fb);
		green = (tAlpha1 * c.green * Fa) + (tAlpha2 * otherColor.green * Fb);
		blue = (tAlpha1 * c.blue * Fa) + (tAlpha2 * otherColor.blue * Fb);
		alpha = (c.alpha * Fa) + (otherColor.alpha * Fb);

		unsigned char redB = (unsigned char)MathExt::min(red, 255);
		unsigned char greenB = (unsigned char)MathExt::min(green, 255);
		unsigned char blueB = (unsigned char)MathExt::min(blue, 255);
		unsigned char alphaB = (unsigned char)MathExt::min(alpha, 255);

		otherImg->setPixel(x,y,{redB,greenB,blueB,alphaB});
	}
}

Color Graphics::blend(Color src, Color dest)
{
	float Fa = 0;
	float Fb = 0;

	int red = 0;
	int green = 0;
	int blue = 0;
	int alpha = 0;

	//other stuff to
	float tAlpha1 = (float)src.alpha/255;
	float tAlpha2 = (float)dest.alpha/255;

	switch(compositeRule)
	{
		case COMPOSITE_CLEAR:
			Fa=0;
			Fb=0;
			break;
		case COMPOSITE_COPY:
			Fa = 1;
			Fb = 0;
			break;
		case COMPOSITE_DEST:
			Fa = 0;
			Fb = 1;
			break;
		case COMPOSITE_SRC_OVER:
			Fa = 1;
			Fb = 1-tAlpha1;
			break;
		case COMPOSITE_DEST_OVER:
			Fa = 1-tAlpha2;
			Fb = 1;
			break;
		case COMPOSITE_SRC_IN:
			Fa = tAlpha2;
			Fb = 0;
			break;
		case COMPOSITE_DEST_IN:
			Fa = 0;
			Fb = tAlpha1;
			break;
		case COMPOSITE_SRC_OUT:
			Fa = 1-tAlpha2;
			Fb = 0;
			break;
		case COMPOSITE_DEST_OUT:
			Fa = 0;
			Fb = 1-tAlpha1;
			break;
		case COMPOSITE_SRC_ATOP:
			Fa = tAlpha2;
			Fb = 1-tAlpha1;
			break;
		case COMPOSITE_DEST_ATOP:
			Fa = 1-tAlpha2;
			Fb = tAlpha1;
			break;
		case COMPOSITE_XOR:
			Fa = 1-tAlpha2;
			Fb = 1-tAlpha1;
			break;
		case COMPOSITE_LIGHTER:
			Fa = 1;
			Fb = 1;
			break;
		default:
			Fa = 0;
			Fb = 0;
			break;
	}

	red = (tAlpha1 * src.red * Fa) + (tAlpha2 * dest.red * Fb);
	green = (tAlpha1 * src.green * Fa) + (tAlpha2 * dest.green * Fb);
	blue = (tAlpha1 * src.blue * Fa) + (tAlpha2 * dest.blue * Fb);
	alpha = (src.alpha * Fa) + (dest.alpha * Fb);

	unsigned char redB = (unsigned char)MathExt::min(red, 255);
	unsigned char greenB = (unsigned char)MathExt::min(green, 255);
	unsigned char blueB = (unsigned char)MathExt::min(blue, 255);
	unsigned char alphaB = (unsigned char)MathExt::min(alpha, 255);

	return {redB,greenB,blueB,alphaB};

}

Color Graphics::lerp(Color src, Color dest, float lerpVal)
{
	Vec4f v1 = Vec4f(src.red, src.green, src.blue, src.alpha);
	Vec4f v2 = Vec4f(dest.red, dest.green, dest.blue, dest.alpha);
	
	Vec4f v3 = v1*(1.0-lerpVal) + v2*(lerpVal);

	return {(unsigned char)MathExt::clamp(v3.x, 0.0f, 255.0f),
			(unsigned char)MathExt::clamp(v3.y, 0.0f, 255.0f),
			(unsigned char)MathExt::clamp(v3.z, 0.0f, 255.0f),
			(unsigned char)MathExt::clamp(v3.w, 0.0f, 255.0f) };
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

		int actualX = minX;
		int actualY = minY;
		if (outline == false)
		{
			while (startPoint < endPoint)
			{

				*startPoint = blend(activeColor, *startPoint);

				startPoint++;
				tX++;
				actualX++;

				if (tX >= offWidth)
				{
					tX = 0;
					actualX = minX;
					actualY++;
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
					*startPoint = blend(activeColor, *startPoint);
				}
				else
				{
					*startPoint = blend(activeColor, *startPoint);
					tX += offWidth-1;
					startPoint += offWidth-1;
					*startPoint = activeColor;
				}

				startPoint++;
				tX++;
				actualX++;

				if (tX >= offWidth)
				{
					tX = 0;
					actualX=minX;
					startPoint += addAmount;
					tY++;
					actualY++;
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
		
		//determine if it is on the line
		//Using general equation
		//Ax + By = C
		//In order to avoid holes, swap between solving for x or y depending
		//on which has a greater rate of change. AntiAliasing can be added easily
		//in this case.
		int P1 = y1-y2;
		int P2 = x2-x1;
		int con = x1*y2 - x2*y1;

		if(P1!=0 && P2!=0)
		{
			if(MathExt::abs(P1) >= MathExt::abs(P2))
			{
				//VERTICAL BASED
				int minY, maxY;
				if(y1 <= y2)
				{
					minY = MathExt::clamp(y1, 0, otherImg->getHeight()-1);
					maxY = MathExt::clamp(y2, 0, otherImg->getHeight()-1);
				}
				else
				{
					minY = MathExt::clamp(y2, 0, otherImg->getHeight()-1);
					maxY = MathExt::clamp(y1, 0, otherImg->getHeight()-1);
				}
				for(int i=minY; i<=maxY; i++)
				{
					//solve with respect to x
					double val = -(con+P2*i) / (double)P1;
					int actualX = MathExt::round(val);
					otherImg->drawPixel(actualX, i, Graphics::activeColor);
				}
			}
			else
			{
				//HORIZONTAL BASED
				int minX, maxX;
				if(x1 <= x2)
				{
					minX = MathExt::clamp(x1, 0, otherImg->getWidth()-1);
					maxX = MathExt::clamp(x2, 0, otherImg->getWidth()-1);
				}
				else
				{
					minX = MathExt::clamp(x2, 0, otherImg->getWidth()-1);
					maxX = MathExt::clamp(x1, 0, otherImg->getWidth()-1);
				}
				for(int i=minX; i<=maxX; i++)
				{
					//solve with respect to y
					double val = -(con+P1*i) / (double)P2;
					int actualY = MathExt::round(val);
					otherImg->drawPixel(i, actualY, Graphics::activeColor);
				}
			}
		}
		else if(P2==0)
		{
			//VERTICAL LINE
			int minY, maxY;
			if(y1 <= y2)
			{
				minY = MathExt::clamp(y1, 0, otherImg->getHeight()-1);
				maxY = MathExt::clamp(y2, 0, otherImg->getHeight()-1);
			}
			else
			{
				minY = MathExt::clamp(y2, 0, otherImg->getHeight()-1);
				maxY = MathExt::clamp(y1, 0, otherImg->getHeight()-1);
			}

			for(int i=minY; i<=maxY; i++)
			{
				otherImg->drawPixel(x1, i, Graphics::activeColor);
			}
		}
		else if(P1==0)
		{
			//HORIZONTAL LINE
			int minX, maxX;
			if(x1 <= x2)
			{
				minX = MathExt::clamp(x1, 0, otherImg->getWidth()-1);
				maxX = MathExt::clamp(x2, 0, otherImg->getWidth()-1);
			}
			else
			{
				minX = MathExt::clamp(x2, 0, otherImg->getWidth()-1);
				maxX = MathExt::clamp(x1, 0, otherImg->getWidth()-1);
			}

			for(int i=minX; i<=maxX; i++)
			{
				otherImg->drawPixel(i, y1, Graphics::activeColor);
			}
		}

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
					Color c = blend(activeColor, *startPoint);
					*startPoint = c;
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
					Color c = blend(activeColor, *startPoint);
					*startPoint = c;
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

void Graphics::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool outline, Image* surf)
{
	Image* otherImg;
	if(surf==nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;
	
	if(otherImg!=nullptr)
	{
		// Line l1 = Line(x1, y1, x2, y2);
		// Line l2 = Line(x2, y2, x3, y3);
		// Line l3 = Line(x3, y3, x1, y1);


		// int arr[3] = {x1, x2, x3};
		// int minX = MathExt::clamp(MathExt::min( arr, 3 ), 0, otherImg->getWidth());
		// int maxX = MathExt::clamp(MathExt::max( arr, 3 ), 0, otherImg->getWidth());
		
		// arr[0] = y1; arr[1] = y2; arr[2] = y3;
		// int minY = MathExt::clamp(MathExt::min( arr, 3 ), 0, otherImg->getHeight());
		// int maxY = MathExt::clamp(MathExt::max( arr, 3 ), 0, otherImg->getHeight());
		
		// double length1 = MathExt::vecLength(l1.getToPoint());
		// double length2 = MathExt::vecLength(l2.getToPoint());
		// double length3 = MathExt::vecLength(l3.getToPoint());

		// Line constLine;
		// Line sLine;
		// Line eLine;
		// Vec2f pivot = Vec2f();
		// if(length1>length2 && length1>length3)
		// {
		// 	constLine = l1;
		// 	pivot = Vec2f(x3, y3);
		// 	if(y2 < y1)
		// 	{
		// 		sLine = l2;
		// 		eLine = l3;
		// 	}
		// 	else
		// 	{
		// 		sLine = l3;
		// 		eLine = l2;
		// 	}
		// }
		// else if(length2>length1 && length2>length3)
		// {
		// 	constLine = l2;
		// 	pivot = Vec2f(x1, y1);
		// 	if(y3 < y2)
		// 	{
		// 		sLine = l3;
		// 		eLine = l1;
		// 	}
		// 	else
		// 	{
		// 		sLine = l1;
		// 		eLine = l3;
		// 	}
		// }
		// else
		// {
		// 	constLine = l3;
		// 	pivot = Vec2f(x2, y2);
		// 	if(y1 < y3)
		// 	{
		// 		sLine = l1;
		// 		eLine = l2;
		// 	}
		// 	else
		// 	{
		// 		sLine = l2;
		// 		eLine = l1;
		// 	}
		// }

		// for(int y=minY; y<maxY; y++)
		// {
		// 	int cX1 = 0;
		// 	int cX2 = 0;

		// 	if(y<pivot.y)
		// 	{
		// 		cX1 = constLine.solveForX(y);
		// 		cX2 = sLine.solveForX(y);
		// 	}
		// 	else
		// 	{
		// 		cX1 = constLine.solveForX(y);
		// 		cX2 = eLine.solveForX(y);
		// 	}

		// 	if(cX1>cX2)
		// 	{
		// 		int old = cX2;
		// 		cX2 = cX1;
		// 		cX1 = cX2;
		// 	}

		// 	cX1 = MathExt::clamp(cX1, 0, otherImg->getWidth());
		// 	cX2 = MathExt::clamp(cX2, 0, otherImg->getWidth());

		// 	//fill from cX1 to cX2
		// 	for(int x=cX1; x<cX2; x++)
		// 	{
		// 		drawPixel(x, y, activeColor, surf);
		// 	}
		// }

		int arr[3] = {x1, x2, x3};
		int minX = MathExt::clamp(MathExt::min( arr, 3 ), 0, otherImg->getWidth());
		int maxX = MathExt::clamp(MathExt::max( arr, 3 ), 0, otherImg->getWidth());
		
		arr[0] = y1; arr[1] = y2; arr[2] = y3;
		int minY = MathExt::clamp(MathExt::min( arr, 3 ), 0, otherImg->getHeight());
		int maxY = MathExt::clamp(MathExt::max( arr, 3 ), 0, otherImg->getHeight());

		for(int y=minY; y<maxY; y++)
		{
			for(int x=minX; x<maxX; x++)
			{
				double w2 = (y-y3)*(x1-x3) - (x-x3)*(y1-y3);
				w2 /= (y2-y3)*(x1-x3) - (x2-x3)*(y1-y3);

				double w1 = (x - w2*(x2-x3) - x3) / (x1-x3);
				double w3 = 1.0 - w2 - w1;

				if(w1>=0 && w2>=0 && w3>=0)
				{
					drawPixel(x,y,activeColor,otherImg);
				}
			}
		}
		
	}
}

void Graphics::drawTexturedTriangle(Vec4f p1, Vec4f p2, Vec4f p3, Image* texture, Image* surf)
{
	Image* otherImg;
	if(surf==nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;
	
	if(otherImg!=nullptr && texture!=nullptr)
	{
		// Line l1 = Line(p1.x, p1.y, p2.x, p2.y);
		// Line l2 = Line(p2.x, p2.y, p3.x, p3.y);
		// Line l3 = Line(p3.x, p3.y, p1.x, p1.y);


		// int arr[3] = {(int)p1.x, (int)p2.x, (int)p3.x};
		// int minX = MathExt::clamp(MathExt::min( arr, 3 ), 0, otherImg->getWidth());
		// int maxX = MathExt::clamp(MathExt::max( arr, 3 ), 0, otherImg->getWidth());
		
		// arr[0] = (int)p1.y; arr[1] = (int)p2.y; arr[2] = (int)p3.y;
		// int minY = MathExt::clamp(MathExt::min( arr, 3 ), 0, otherImg->getHeight());
		// int maxY = MathExt::clamp(MathExt::max( arr, 3 ), 0, otherImg->getHeight());
		
		// double length1 = MathExt::vecLength(l1.getToPoint());
		// double length2 = MathExt::vecLength(l2.getToPoint());
		// double length3 = MathExt::vecLength(l3.getToPoint());

		// Line constLine;
		// Line sLine;
		// Line eLine;
		// Vec2f pivot = Vec2f();
		// if(length1>length2 && length1>length3)
		// {
		// 	constLine = l1;
		// 	pivot = Vec2f(p3.x, p3.y);
		// 	if(p2.y < p1.y)
		// 	{
		// 		sLine = l2;
		// 		eLine = l3;
		// 	}
		// 	else
		// 	{
		// 		sLine = l3;
		// 		eLine = l2;
		// 	}
		// }
		// else if(length2>length1 && length2>length3)
		// {
		// 	constLine = l2;
		// 	pivot = Vec2f(p1.x, p1.y);
		// 	if(p3.y < p2.y)
		// 	{
		// 		sLine = l3;
		// 		eLine = l1;
		// 	}
		// 	else
		// 	{
		// 		sLine = l1;
		// 		eLine = l3;
		// 	}
		// }
		// else
		// {
		// 	constLine = l3;
		// 	pivot = Vec2f(p2.x, p2.y);
		// 	if(p1.y < p3.y)
		// 	{
		// 		sLine = l1;
		// 		eLine = l2;
		// 	}
		// 	else
		// 	{
		// 		sLine = l2;
		// 		eLine = l1;
		// 	}
		// }

		// for(int y=minY; y<maxY; y++)
		// {
		// 	int cX1 = 0;
		// 	int cX2 = 0;

		// 	if(y<pivot.y)
		// 	{
		// 		cX1 = constLine.solveForX(y);
		// 		cX2 = sLine.solveForX(y);
		// 	}
		// 	else
		// 	{
		// 		cX1 = constLine.solveForX(y);
		// 		cX2 = eLine.solveForX(y);
		// 	}

		// 	if(cX1>cX2)
		// 	{
		// 		int old = cX2;
		// 		cX2 = cX1;
		// 		cX1 = cX2;
		// 	}

		// 	cX1 = MathExt::clamp(cX1, 0, otherImg->getWidth());
		// 	cX2 = MathExt::clamp(cX2, 0, otherImg->getWidth());

		// 	//fill from cX1 to cX2
		// 	for(int x=cX1; x<cX2; x++)
		// 	{
		// 		//adjust texture information to get color
		// 		double w2 = (y-p3.y)*(p1.x-p3.x) - (x-p3.x)*(p1.y-p3.y);
		// 		w2 /= (p2.y-p3.y)*(p1.x-p3.x) - (p2.x-p3.x)*(p1.y-p3.y);

		// 		double w1 = (x - w2*(p2.x-p3.x) - p3.x) / (p1.x-p3.x);
		// 		double w3 = 1.0 - w2 - w1;

		// 		double u = p1.z*w1 + p2.z*w2 + p3.z*w3;
		// 		double v = p1.w*w1 + p2.w*w2 + p3.w*w3;
				
		// 		u *= texture->getWidth()-1;
		// 		v *= texture->getHeight()-1;

		// 		Color c = texture->getPixel(u, v);
				
		// 		drawPixel(x, y, c, surf);
		// 	}
		// }

		int arr[3] = {(int)p1.x, (int)p2.x, (int)p3.x};
		int minX = MathExt::clamp(MathExt::min( arr, 3 ), 0, otherImg->getWidth());
		int maxX = MathExt::clamp(MathExt::max( arr, 3 ), 0, otherImg->getWidth());
		
		arr[0] = (int)p1.y; arr[1] = (int)p2.y; arr[2] = (int)p3.y;
		int minY = MathExt::clamp(MathExt::min( arr, 3 ), 0, otherImg->getHeight());
		int maxY = MathExt::clamp(MathExt::max( arr, 3 ), 0, otherImg->getHeight());

		for(int y=minY; y<maxY; y++)
		{
			for(int x=minX; x<maxX; x++)
			{
				double w2 = (y-p3.y)*(p1.x-p3.x) - (x-p3.x)*(p1.y-p3.y);
				w2 /= (p2.y-p3.y)*(p1.x-p3.x) - (p2.x-p3.x)*(p1.y-p3.y);

				double w1 = (x - w2*(p2.x-p3.x) - p3.x) / (p1.x-p3.x);
				double w3 = 1.0 - w2 - w1;

				if(w1>=0 && w2>=0 && w3>=0)
				{
					double u = p1.z*w1 + p2.z*w2 + p3.z*w3;
					double v = p1.w*w1 + p2.w*w2 + p3.w*w3;
					
					u *= texture->getWidth()-1;
					v *= texture->getHeight()-1;

					Color c = texture->getPixel(u, v);

					drawPixel(x,y,c,otherImg);
				}
			}
		}
	}
}

//Works properly now
void Graphics::drawImage(Image* img, int x, int y, Image* surf)
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

		Vec4f colorMult = Vec4f((double)Graphics::activeColor.red / 255.0, (double)Graphics::activeColor.green / 255.0, (double)Graphics::activeColor.blue / 255.0, (double)Graphics::activeColor.alpha / 255.0);

		while (startPoint < endPoint)
		{
			Color drawC = { (unsigned char) (otherStartPoint->red * colorMult.x),
							(unsigned char) (otherStartPoint->green * colorMult.y),
							(unsigned char) (otherStartPoint->blue * colorMult.z), 
							(unsigned char) (otherStartPoint->alpha * colorMult.w) };
			*startPoint = blend(drawC, *startPoint);

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
		
		Vec4f colorMult = Vec4f((double)Graphics::activeColor.red / 255.0, (double)Graphics::activeColor.green / 255.0, (double)Graphics::activeColor.blue / 255.0, (double)Graphics::activeColor.alpha / 255.0);

		while (startPoint < endPoint)
		{
			Color drawC = { (unsigned char) (otherStartPoint->red * colorMult.x),
							(unsigned char) (otherStartPoint->green * colorMult.y),
							(unsigned char) (otherStartPoint->blue * colorMult.z), 
							(unsigned char) (otherStartPoint->alpha * colorMult.w) };
			*startPoint = blend(drawC, *startPoint);

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

	if (otherImg != nullptr && activeFont != nullptr)
	{
		int currX = x;
		for(int i=0; i<str.length(); i++)
		{
			int charIndex = activeFont->getCharIndex(str[i]);
			Image* charImg = activeFont->getImage(charIndex);
			FontCharInfo fci = activeFont->getFontCharInfo(charIndex);
			
			if(fci.x!=0 || fci.y!=0 || fci.width != charImg->getWidth() || fci.height != charImg->getHeight())
				drawSpritePart(charImg, currX, y, fci.x, fci.y, fci.width, fci.height, otherImg);
			else
				drawSprite(charImg, currX, y, otherImg);
			
			currX += fci.horizAdv;
		}
	}
}

void Graphics::drawPolygon(Vec2f* points, int size, Image* surf)
{
	Image* otherImg;
	if (surf == nullptr)
		otherImg = activeImage;
	else
		otherImg = surf;

	if (otherImg != nullptr)
	{
		//similar in nature to the vector graphic version
		//always connects the last point to the first

		std::vector<Vec2f> actualPoints = std::vector<Vec2f>();
		for(int i=0; i<size; i++)
		{
			actualPoints.push_back(points[i]);
		}
		actualPoints.push_back(points[0]);
		
		int minY = (int)points[0].y;
		int maxY = (int)points[0].y;

		for(int i=0; i<size; i++)
		{
			if((int)points[i].y < minY)
			{
				minY = (int)points[i].y;
			}
			if((int)points[i].y > maxY)
			{
				maxY = (int)points[i].y;
			}
		}

		minY = MathExt::clamp(minY, 0, otherImg->getHeight()-1);
		maxY = MathExt::clamp(maxY, 0, otherImg->getHeight()-1);

		int yDis = 1+maxY-minY;
		struct criticalPoint
		{
			int xValue;
			bool direction;
		};

		std::vector<criticalPoint>* scanLines = new std::vector<criticalPoint>[yDis];

		for(int i=0; i<size; i++)
		{
			//Ax + By + C = 0
			//A=(y1-y2)
			//B=(x2-x1)
			//C=(x1*y2 - x2*y1)
			double A = (actualPoints[i].y - actualPoints[i+1].y);
			double B = (actualPoints[i+1].x - actualPoints[i].x);
			double C = (actualPoints[i].x*actualPoints[i+1].y - actualPoints[i+1].x*actualPoints[i].y);
			bool posDir = true;

			//AFAIK, if the y is going up in some way, the direction is positive
			//otherwise, it is negative.
			if(A<=0)
			{
				posDir = true;
			}
			else
			{
				posDir = false;
			}
			
			if(A!=0)
			{
				//vertical line or some other valid line
				int thisMinY = MathExt::ceil( MathExt::min( actualPoints[i].y, actualPoints[i+1].y));
				int thisMaxY = MathExt::floor( MathExt::max( actualPoints[i].y, actualPoints[i+1].y));

				thisMinY = MathExt::clamp(thisMinY, minY, maxY);
				thisMaxY = MathExt::clamp(thisMaxY, minY, maxY);

				for(int j=thisMinY; j<=thisMaxY; j++)
				{
					int xValue = MathExt::round(-(B*j + C) / A);
					xValue = MathExt::clamp(xValue, 0, otherImg->getWidth());
					
					scanLines[j-minY].push_back({xValue,posDir});
				}
			}
			else
			{
				//horizontal line or invalid
				//do nothing
			}
		}

		//sort the points on the scanlines
		for(int i=0; i<yDis; i++)
		{
			if(scanLines[i].size()>0)
			{
				Sort::insertionSort<criticalPoint>(scanLines[i].data(), scanLines[i].size(), [](criticalPoint a, criticalPoint b) -> bool {return a.xValue<b.xValue;});
				
				//rule, can not be the same if you are filling to it.
				//different for even-odd and non-zero
				std::vector<criticalPoint> newScanLine = std::vector<criticalPoint>();

				if(fillRule == Graphics::FILL_EVEN_ODD)
				{
					for(int j=1; j<scanLines[i].size(); j+=2)
					{
						if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
						{
							newScanLine.push_back(scanLines[i][j-1]);
							newScanLine.push_back(scanLines[i][j]);
						}
						else
						{
							if(scanLines[i][j].direction != scanLines[i][j-1].direction)
							{
								newScanLine.push_back(scanLines[i][j-1]);
								newScanLine.push_back(scanLines[i][j]);
							}
						}
					}
				}
				else
				{
					newScanLine.push_back(scanLines[i][0]);
					for(int j=1; j<scanLines[i].size(); j++)
					{
						if(scanLines[i][j].xValue != scanLines[i][j-1].xValue)
						{
							newScanLine.push_back(scanLines[i][j]);
						}
						else
						{
							if(scanLines[i][j].direction != scanLines[i][j-1].direction)
							{
								newScanLine.push_back(scanLines[i][j]);
							}
						}
					}
				}

				scanLines[i] = newScanLine;
			}
		}

		if(fillRule==Graphics::FILL_EVEN_ODD)
		{
			//even-odd rule. Fill between even to odd indicies
			//not odd to even indicies
			for(int j=0; j<yDis; j++)
			{
				int actualSize = scanLines[j].size();
				if(actualSize%2 == 1)
				{
					actualSize-=2;
				}
				else
				{
					actualSize-=1;
				}
				

				for(int i=0; i<actualSize; i+=2)
				{
					//fill between spots
					int startX = scanLines[j][i].xValue;
					int endX = scanLines[j][i+1].xValue;

					int fillX = startX;

					do
					{
						otherImg->drawPixel(fillX, j+minY, Graphics::activeColor);
						fillX++;
					} while (fillX<=endX);
					
				}
			}
			
		}
		else
		{
			//non-zero rule. Fill when positive only. Not when zero or
			//less.
			for(int j=0; j<yDis; j++)
			{
				int passCounter = 0;
				for(int i=0; i<scanLines[j].size()-1; i++)
				{
					//fill between spots
					int startX = scanLines[j][i].xValue;
					int endX = scanLines[j][i+1].xValue;
					if(scanLines[j][i].direction == true)
					{
						//positive y direction
						passCounter++;
					}
					else
					{
						//negative y direction
						passCounter--;
					}
					
					if(passCounter!=0)
					{
						int fillX = startX;

						do
						{
							otherImg->drawPixel(fillX, j+minY, Graphics::activeColor);
							fillX++;
						} while (fillX<=endX);
						
					}
				}
			}
		}

	}

}

void Graphics::drawModel(Model* model, Image* texture, Image* surf)
{
	Image* otherImg = surf;
	if(otherImg==nullptr)
	{
		otherImg = activeImage;
	}

	if(otherImg!=nullptr)
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

void Graphics::setFont(BitmapFont* f)
{
	activeFont = f;
}

BitmapFont* Graphics::getFont()
{
	return activeFont;
}

void Graphics::setFillRule(bool v)
{
	Graphics::fillRule = v;
}

bool Graphics::getFillRule()
{
	return fillRule;
}

void Graphics::setAntiAliasing(bool v)
{
	Graphics::antiAliasing = v;
}

bool Graphics::getAntiAliasing()
{
	return antiAliasing;
}

void Graphics::setCompositeRule(unsigned char b)
{
	compositeRule = b;
}

unsigned char Graphics::getCompositeRule()
{
	return compositeRule;
}

void Graphics::setBlendMode(unsigned char b)
{
	blendMode = b;
}

unsigned char Graphics::getBlendMode()
{
	return blendMode;
}

#pragma endregion

#pragma region IMAGE_MANIPULATION
int Graphics::ditherMatrixSize = 2;

void Graphics::replaceColor(Image* img, Color oldColor, Color newColor, bool ignoreAlpha)
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

void Graphics::ditherImage(Image* img, unsigned char method)
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

void Graphics::floydSteinburgDithering(Image* img)
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

void Graphics::orderedBayerDithering(Image* img)
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

Matrix Graphics::generateBayerMatrix(Matrix mat, int rowSize)
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

Image* Graphics::scaleImage(Image* img, double xScale, double yScale, unsigned char filterMethod)
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

Image* Graphics::scaleNearestNeighbor(Image* img, double xScale, double yScale)
{
	if(img!=nullptr)
	{
		int nWidth = img->getWidth() * xScale;
		int nHeight = img->getHeight() * yScale;
		Image* sImg = new Image( nWidth, nHeight );

		//position top left of image so that it has an equal amount of distance from edges
		//divide distance by size of image to find offset

		double xOffset = (double)(nWidth - img->getWidth())/ (2*img->getWidth());
		double yOffset = (double)(nHeight - img->getHeight())/ (2*img->getHeight());

		for(int i=0; i<nHeight; i++)
		{
			double yP = ((double)(i-yOffset) / nHeight) * img->getHeight();
			int nearY = MathExt::clamp(MathExt::round(yP), 0.0, (double)img->getHeight()-1);
			
			for(int i2=0; i2<nWidth; i2++)
			{
				double xP = ((double)(i2-xOffset) / nWidth) * img->getWidth();
				int nearX = MathExt::clamp(MathExt::round(xP), 0.0, (double)img->getWidth()-1);
				
				sImg->setPixel(i2, i, img->getPixel(nearX, nearY));
			}
		}

		return sImg;
	}
	return nullptr;
}

Image* Graphics::scaleBilinear(Image* img, double xScale, double yScale)
{
	if(img!=nullptr)
	{
		int nWidth = img->getWidth() * xScale;
		int nHeight = img->getHeight() * yScale;
		
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

			int y1 = MathExt::clamp(MathExt::floor(yP), 0.0, (double)img->getHeight()-1);
			int y2 = MathExt::clamp(MathExt::ceil(yP), 0.0, (double)img->getHeight()-1);
			
			for(int i2=0; i2<nWidth; i2++)
			{
				double xP = ((double)(i2-xOffset) / nWidth) * img->getWidth();
				//xP -= xOffset;
				double xFrac = MathExt::frac(xP);

				int x1 = MathExt::clamp(MathExt::floor(xP), 0.0, (double)img->getWidth()-1);
				int x2 = MathExt::clamp(MathExt::ceil(xP), 0.0, (double)img->getWidth()-1);
				
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

Image* Graphics::scaleBicubic(Image* img, double xScale, double yScale)
{
	if(img!=nullptr)
	{
		int nWidth = img->getWidth() * xScale;
		int nHeight = img->getHeight() * yScale;
		
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
			yPoints[1] = MathExt::clamp(MathExt::floor(yP), 0.0, (double)img->getHeight()-1);
			yPoints[0] = MathExt::clamp(yPoints[1]-1, 0, img->getHeight()-1);
			yPoints[2] = MathExt::clamp(MathExt::ceil(yP), 0.0, (double)img->getHeight()-1);
			yPoints[3] = MathExt::clamp(yPoints[2]+1, 0, img->getHeight()-1);

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
				xPoints[1] = MathExt::clamp(MathExt::floor(xP), 0.0, (double)img->getWidth()-1);
				xPoints[0] = MathExt::clamp(xPoints[1]-1, 0, img->getWidth()-1);
				xPoints[2] = MathExt::clamp(MathExt::ceil(xP), 0.0, (double)img->getWidth()-1);
				xPoints[3] = MathExt::clamp(xPoints[2]+1, 0, img->getWidth()-1);

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
				
				unsigned char red = (unsigned char)MathExt::clamp(finalC.x, 0.0f, 255.0f);
				unsigned char green = (unsigned char)MathExt::clamp(finalC.y, 0.0f, 255.0f);
				unsigned char blue = (unsigned char)MathExt::clamp(finalC.z, 0.0f, 255.0f);
				unsigned char alpha = (unsigned char)MathExt::clamp(finalC.w, 0.0f, 255.0f);
				
				sImg->setPixel(i2, i, {red,green,blue,alpha});
			}
		}

		return sImg;
	}
	return nullptr;
}



#pragma endregion