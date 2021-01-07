#include "ColorPalette.h"
#include "MathExt.h"
#include "Sort.h"
#include "StringTools.h"
#include "FrequencyTable.h"
#include "SimpleFile.h"

#pragma region ClassStuff
const Class* ColorPalette::myClass = new Class("ColorPalette", {Object::myClass});
const Class* ColorPalette::getClass()
{
	return ColorPalette::myClass;
}
#pragma endregion

ColorPalette::ColorPalette()
{
}


ColorPalette::~ColorPalette()
{
	palette.clear();
}

ColorPalette::ColorPalette(const ColorPalette& other)
{
	palette = std::vector<Color>(other.palette);
}

ColorPalette ColorPalette::createColorPalette(int reds, int greens, int blues)
{
	ColorPalette temp = ColorPalette();
	
	double rInc = 0;
	double gInc = 0;
	double bInc = 0;

	if(reds>1)
		rInc = 255.0 / reds;
	else
	{
		if(reds==1)
			rInc = 255;
	}
	
	if(greens>1)
		gInc = 255.0 / greens;
	else
	{
		if(greens==1)
			gInc = 255;
	}

	if(blues>1)
		bInc = 255.0 / blues;
	else
	{
		if(blues==1)
			bInc = 255;
	}

	int r=0;
	int g=0;
	int b=0;
	do
	{
		g=0;
		do
		{
			b=0;
			do
			{
				Color c = { (unsigned char)(rInc * r),
							(unsigned char)(gInc * g),
							(unsigned char)(bInc * b),
							255 };

				temp.palette.push_back(c);
				b++;
			} while (b<blues);
			g++;
		} while (g<greens);
		r++;
	} while (r<reds);
	
	return temp;
}

void ColorPalette::addNewColor(Color c)
{
	bool canAdd = true;

	for (int i = 0; i < palette.size(); i++)
	{
		if (memcmp(&c, &palette[i], sizeof(Color)) == 0)
		{
			canAdd = false;
			break;
		}
	}

	if (canAdd)
	{
		palette.push_back(c);
	}
}

Color ColorPalette::getColor(int index)
{
	return palette[index];
}

Color* ColorPalette::getColorRef(int index)
{
	return &palette[index];
}

int ColorPalette::getSize()
{
	return palette.size();
}

std::vector<Color> ColorPalette::getPalette()
{
	return palette;
}

std::vector<Color>* ColorPalette::getPaletteRef()
{
	return &palette;
}

void ColorPalette::clearPalette()
{
	palette.clear();
}

Color ColorPalette::getClosestColor(Color c)
{
	return palette[getClosestColorIndex(c)];
}

int ColorPalette::getClosestColorIndex(Color c)
{
	Vec3f disVec = Vec3f(999,999,999);
	Vec3f disVec2 = Vec3f();

	double distance = MathExt::vecLength(disVec);
	int index = 0;

	for(int i=0; i<palette.size(); i++)
	{
		disVec2.x = (int)palette[i].red - c.red;
		disVec2.y = (int)palette[i].green - c.green;
		disVec2.z = (int)palette[i].blue - c.blue;

		double distance2 = MathExt::vecLength(disVec2);

		if(distance2 < distance)
		{
			disVec = disVec2;
			distance = distance2;
			index = i;
		}
	}

	return index;
}

ColorPalette ColorPalette::generateOptimalPalette(Color* colorArray, int size, int colors)
{
	ColorPalette temp = ColorPalette();
	ColorPalette f = ColorPalette();
	for(int i=0; i<size; i++)
	{
		if(colorArray[i].alpha>0)
		{
			Color v = colorArray[i];
			v.alpha = 0;

			f.addNewColor(v);
		}
	}
	
	StringTools::out << f.getSize() << StringTools::lineBreak;
	if(f.getSize()<=colors)
	{
		return f;
	}
	else
	{
		//create 6,7,6 palette. Find closest color in palette. Sort by distance.
		//some sort of priority based on furthest distance
	}
	

	return temp;
}