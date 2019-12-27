#include "ColorPalette.h"

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
	
	int rInc = 0;
	int gInc = 0;
	int bInc = 0;

	if(reds!=0)
		rInc = 255 / reds;

	if(greens!=0)
		gInc = 255 / greens;

	if(blues!=0)
		bInc = 255 / blues;

	int r = 0;
	do
	{
		int g = 0;
		do
		{
			int b = 0;
			do
			{

				Color c = { (unsigned char)(rInc * r),
							(unsigned char)(gInc * g),
							(unsigned char)(bInc * b),
							255 };

				temp.palette.push_back(c);

				b++;
			} while (b < blues);

			g++;
		} while (g < greens);

		r++;
	} while (r < reds);

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

ColorPalette ColorPalette::generateOptimalPalette(Color* colorArray, int size, int colors)
{
	//First, use median cut to reduce the amount of colors
	//then remove the colors with the minimum amount of distance to the colors next to it
	//if you need to reduce even further.

	ColorPalette temp = ColorPalette();
	
	int subDivisions = (int)ceil(log2(colors));
	int absoluteAmount = 2 << subDivisions;

	std::vector<Color*> arrays = std::vector<Color*>();
	std::vector<int> sizes = std::vector<int>();

	//first subDivision
	Color* firstSort = medianCutColors(colorArray, size);

	arrays.push_back(&firstSort[size / 2]);
	arrays.push_back(&firstSort[0]);
	sizes.push_back(size / 2);
	sizes.push_back(size / 2 + size % 2);

	for (int i = 0; i < subDivisions-1; i++)
	{
		//subdivide the arrays into more arrays
		std::vector<Color*> moreArrays = std::vector<Color*>();
		std::vector<int> moreSizes = std::vector<int>();

		for (int j = 0; j < arrays.size(); j++)
		{
			Color* newSort = medianCutColors(arrays[j], sizes[j]);

			moreArrays.push_back(&newSort[0]);
			moreArrays.push_back(&newSort[sizes[j] / 2]);

			sizes.push_back(sizes[j] / 2);
			sizes.push_back(sizes[j] / 2 + sizes[j] % 2);
		}

		//Clear the old vectors and fill it with the new ones
		for (int j = 0; j < arrays.size(); j++)
		{
			delete[] arrays[j];
		}
		arrays.clear();
		sizes.clear();

		arrays = moreArrays;
		sizes = moreSizes;
	}

	//After all the subdivisions, find the mean of each subdivision and fill the palette
	//For now, assume that we are asking in powers of 2.
	for (int i = 0; i < arrays.size(); i++)
	{
		int reds = 0;
		int greens = 0;
		int blues = 0;

		for (int j = 0; j < sizes[i]; j++)
		{
			reds += arrays[i][j].red;
			greens += arrays[i][j].green;
			blues += arrays[i][j].blue;
		}

		reds /= sizes[i];
		greens /= sizes[i];
		blues /= sizes[i];

		Color c = { (unsigned char)reds, (unsigned char)greens, (unsigned char)blues, 255 };
		temp.palette.push_back(c);
	}
	return temp;
}

Color* ColorPalette::medianCutColors(Color* colorArray, int size)
{
	//First, determine which has the greatest range.
	//Do not include alpha in this.

	int minRed = 255;
	int maxRed = 0;
	int minGreen = 255;
	int maxGreen = 0;
	int minBlue = 255;
	int maxBlue = 0;

	//Find maxRange between colors
	for (int i = 0; i < size; i++)
	{
		minRed = (colorArray[i].red < minRed) ? colorArray[i].red : minRed;
		maxRed = (colorArray[i].red > minRed) ? colorArray[i].red : maxRed;

		minGreen = (colorArray[i].green < minGreen) ? colorArray[i].green : minGreen;
		maxGreen = (colorArray[i].green > minGreen) ? colorArray[i].green : maxGreen;

		minBlue = (colorArray[i].blue < minBlue) ? colorArray[i].blue : minGreen;
		maxBlue = (colorArray[i].blue > minBlue) ? colorArray[i].blue : maxGreen;
	}

	int sort1 = 0;//red
	int sort2 = 0;//green
	int sort3 = 0;//blue

	if ((maxBlue - minBlue) > (maxGreen - minGreen))
	{
		if ((maxBlue - minBlue) > (maxRed - minRed))
		{
			if ((maxGreen - minGreen) > (maxRed - minRed))
			{
				//sort blue, green, red
				sort1 = 0;
				sort2 = 1;
				sort3 = 2;
			}
			else
			{
				//sort blue, red, green
				sort1 = 1;
				sort2 = 0;
				sort3 = 2;
			}
		}
		else
		{
			//sort red, blue, green
			sort1 = 2;
			sort2 = 0;
			sort3 = 1;
		}
	}
	else
	{
		if ((maxBlue - minBlue) > (maxRed - minRed))
		{
			//sort green, blue, red
			sort1 = 0;
			sort2 = 2;
			sort3 = 1;
		}
		else
		{
			if ((maxGreen - minGreen) > (maxRed - minRed))
			{
				//sort green, red, blue
				sort1 = 1;
				sort2 = 2;
				sort3 = 0;
			}
			else
			{
				//sort red, green, blue
				sort1 = 2;
				sort2 = 1;
				sort3 = 0;
			}
			
		}
	}

	Color* sortedArray = new Color[size];

	int vSize = 0;
	//Now Sort into a new Array
	//We will use selection sort even though it is very inefficient
	//since it is simple and easy in this case

	for (int i = 0; i < size; i++)
	{
		sortedArray[i] = colorArray[i];

		int r = ((int)colorArray[i].red << (8 * sort1));
		int g = ((int)colorArray[i].green << (8 * sort2));
		int b = ((int)colorArray[i].blue << (8 * sort3));

		int v = r + g + b;

		for (int j = i-1; j >=0 ; j--)
		{
			int r2 = ((int)sortedArray[j].red << (8 * sort1));
			int g2 = ((int)sortedArray[j].green << (8 * sort2));
			int b2 = ((int)sortedArray[j].blue << (8 * sort3));

			int v2 = r + g + b;

			if (v < v2)
			{
				//swap
				Color temp = sortedArray[i];
				sortedArray[i] = sortedArray[j];
				sortedArray[j] = temp;
			}
			else
			{
				break;
			}
		}
	}

	return sortedArray;
}
