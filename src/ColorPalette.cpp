#include "ColorPalette.h"
#include "MathExt.h"
#include "Sort.h"
#include "StringTools.h"
#include "SimpleFile.h"
#include "LCG.h"
#include "System.h"
#include "ColorSpaceConverter.h"

namespace glib
{

	#pragma region ClassStuff
	const Class ColorPalette::myClass = Class("ColorPalette", {&Object::myClass});
	const Class* ColorPalette::getClass()
	{
		return &ColorPalette::myClass;
	}
	#pragma endregion

	struct ColorNode
	{
		Color c;
		int arrayIndex;
	};

	ColorPalette::ColorPalette()
	{
	}

	ColorPalette::~ColorPalette()
	{
		paletteArr.clear();
	}

	ColorPalette::ColorPalette(const ColorPalette& other)
	{
		copy(other);
	}

	void ColorPalette::operator=(const ColorPalette& other)
	{
		copy(other);
	}

	void ColorPalette::copy(const ColorPalette& other)
	{

		uniquePalette = other.uniquePalette;
		
		paletteArr.clear();
		delete paletteTree;
		paletteTree = new KDTree<unsigned char>(3);

		for(Color c : other.paletteArr)
		{
			addNewColor(c);
		}
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

					temp.addNewColor(c);
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
		//find exists, assume sorted
		ColorNode* k = new ColorNode();
		k->c = c;
		k->arrayIndex = (int)paletteArr.size();

		bool canAdd = true;

		if(uniquePalette)
			canAdd = paletteTree->addUnique( (unsigned char*)k );
		else
			paletteTree->add( (unsigned char*)k );
		
		if (canAdd)
		{
			paletteArr.push_back(c);
		}
		else
		{
			delete k;
		}
		
	}

	Color ColorPalette::getColor(int index)
	{
		if(index >= 0 && index < paletteArr.size())
			return paletteArr[index];
		else
		{
			#ifdef USE_EXCEPTIONS
			throw OutOfBoundsError;
			#endif

			return Color();
		}
	}

	Color* ColorPalette::getColorRef(int index)
	{
		if(index >= 0 && index < paletteArr.size())
			return &paletteArr[index];
		else
		{
			#ifdef USE_EXCEPTIONS
			throw OutOfBoundsError;
			#endif

			return nullptr;
		}
	}

	int ColorPalette::getColorIndex(Color c)
	{
		KDTreeNode<unsigned char> node = paletteTree->search( (unsigned char*)&c );

		if(node.data != nullptr)
		{
			ColorNode* k = (ColorNode*)node.data;
			return k->arrayIndex;
		}
		else
		{
			return -1;
		}
	}

	int ColorPalette::getSize()
	{
		return paletteArr.size();
	}

	std::vector<Color>& ColorPalette::getPalette()
	{
		return paletteArr;
	}

	void ColorPalette::setPalette(std::vector<Color> p)
	{
		clearPalette();
		for(int i=0; i<p.size(); i++)
		{
			addNewColor(p[i]);
		}
	}

	void ColorPalette::clearPalette()
	{
		delete paletteTree;
		paletteTree = new KDTree<unsigned char>(3);
		paletteArr.clear();
	}

	void ColorPalette::setPaletteMode(bool uniqueOnly)
	{
		uniquePalette = uniqueOnly;
	}

	Color ColorPalette::getClosestColor(Color c)
	{
		return paletteArr[getClosestColorIndex(c)];
	}

	int ColorPalette::getClosestColorIndex(Color c)
	{
		int index = 0;
		/*
		//For testing purposes with KDTrees
		int index2 = 0;
		
		unsigned int distance = 0xFFFFFFFF;
		for(int i=0; i<paletteArr.size(); i++)
		{
			int redDis = (int)paletteArr[i].red - (int)c.red;
			int greenDis = (int)paletteArr[i].green - (int)c.green;
			int blueDis = (int)paletteArr[i].blue - (int)c.blue;

			unsigned int distance2 = MathExt::sqr(redDis) + MathExt::sqr(greenDis) + MathExt::sqr(blueDis);

			if(distance2 < distance)
			{
				distance = distance2;
				index2 = i;
				if(distance2==0)
					break;
			}
		}
		*/
		
		KDTreeNode<unsigned char> node = paletteTree->searchNearest((unsigned char*)&c);
		ColorNode* k = (ColorNode*)node.data;
		if(k!=nullptr)
			index = k->arrayIndex;
		
		return index;
	}

	void ColorPalette::reBalance()
	{
		paletteTree->balance();
	}

	ColorPalette ColorPalette::generateOptimalPalette(Color* colorArray, int size, int colors, unsigned char type, bool convertToLab, bool uniqueOnly)
	{
		ColorPalette temp = ColorPalette();
		if(colorArray == nullptr)
		{
			return temp;
		}

		if(size <= 0)
		{
			return temp;
		}

		if(colors <= 0)
		{
			return temp;
		}

		if(type != MEAN_CUT || type != MEDIAN_CUT || type != K_MEANS)
		{
			return temp;
		}

		if(uniqueOnly)
		{
			ColorPalette f = ColorPalette();
			f.setPaletteMode(true);
			for(int i=0; i<size; i++)
			{
				if(colorArray[i].alpha>127)
				{
					Color v = colorArray[i];
					v.alpha = 255;

					f.addNewColor(v);
				}
			}
			
			if(f.getSize()<=colors)
			{
				return f;
			}
			else
			{
				if(!convertToLab)
				{
					std::vector<Color> p;
					std::vector<Color> pColors = std::vector<Color>(f.getSize());
					for(int i=0; i<f.getSize(); i++)
					{
						pColors[i] = f.getColor(i);
					}

					switch(type)
					{
						case MEAN_CUT:
							p = meanCut(pColors, colors);
							break;
						case MEDIAN_CUT:
							p = medianCut(pColors, colors);
							break;
						case K_MEANS:
							p = kMeans(pColors, colors, 10);
							break;
						default:
							p = meanCut(pColors, colors);
							break;
					}

					for(Color c : p)
					{
						temp.addNewColor(c);
					}
				}
				else
				{
					std::vector<std::vector<Vec3f>> p;
					std::vector<Vec3f> pColors = std::vector<Vec3f>(f.getSize());
					for(int i=0; i<f.getSize(); i++)
					{
						Color c = f.getColor(i);
						Vec3f beforeConvert = Vec3f(c.red, c.green, c.blue);
						Vec3f afterConvert = ColorSpaceConverter::convert(beforeConvert, ColorSpaceConverter::RGB_TO_LAB);

						pColors[i] = afterConvert;
					}

					switch(type)
					{
						case MEAN_CUT:
							p = MathExt::meanCut(pColors, colors, true);
							break;
						case MEDIAN_CUT:
							p = MathExt::medianCut(pColors, colors, true);
							break;
						case K_MEANS:
							p = MathExt::kMeans(pColors, colors, 10, true);
							break;
						default:
							p = MathExt::meanCut(pColors, colors, true);
							break;
					}
					
					for(std::vector<Vec3f> g : p)
					{
						Color c;
						Vec3f afterConvert = ColorSpaceConverter::convert(g[0], ColorSpaceConverter::LAB_TO_RGB);

						c.red = (unsigned char)MathExt::clamp(afterConvert.x, 0.0, 255.0);
						c.green = (unsigned char)MathExt::clamp(afterConvert.y, 0.0, 255.0);
						c.blue = (unsigned char)MathExt::clamp(afterConvert.z, 0.0, 255.0);
						
						temp.addNewColor(c);
					}
				}
			}
		}
		else
		{
			temp.setPaletteMode(false);
			if(!convertToLab)
			{
				std::vector<Color> p;
				std::vector<Color> pColors = std::vector<Color>(size);
				for(int i=0; i<size; i++)
				{
					pColors[i] = colorArray[i];
				}

				switch(type)
				{
					case MEAN_CUT:
						p = meanCut(pColors, colors);
						break;
					case MEDIAN_CUT:
						p = medianCut(pColors, colors);
						break;
					case K_MEANS:
						p = kMeans(pColors, colors, 10);
						break;
					default:
						p = meanCut(pColors, colors);
						break;
				}

				for(Color c : p)
				{
					temp.addNewColor(c);
				}
			}
			else
			{
				std::vector<std::vector<Vec3f>> p;
				std::vector<Vec3f> pColors = std::vector<Vec3f>(size);
				for(int i=0; i<size; i++)
				{
					Color c = colorArray[i];
					Vec3f beforeConvert = Vec3f(c.red, c.green, c.blue);
					Vec3f afterConvert = ColorSpaceConverter::convert(beforeConvert, ColorSpaceConverter::RGB_TO_LAB);
					
					pColors[i] = afterConvert;
				}

				switch(type)
				{
					case MEAN_CUT:
						p = MathExt::meanCut(pColors, colors, true);
						break;
					case MEDIAN_CUT:
						p = MathExt::medianCut(pColors, colors, true);
						break;
					case K_MEANS:
						p = MathExt::kMeans(pColors, colors, 10, true);
						break;
					default:
						p = MathExt::meanCut(pColors, colors, true);
						break;
				}
				
				for(std::vector<Vec3f> g : p)
				{
					Color c;
					Vec3f afterConvert = ColorSpaceConverter::convert(g[0], ColorSpaceConverter::LAB_TO_RGB);

					c.red = (unsigned char)MathExt::clamp(afterConvert.x, 0.0, 255.0);
					c.green = (unsigned char)MathExt::clamp(afterConvert.y, 0.0, 255.0);
					c.blue = (unsigned char)MathExt::clamp(afterConvert.z, 0.0, 255.0);
					
					temp.addNewColor(c);
				}
			}
		}

		return temp;
	}

	std::vector<Color> ColorPalette::meanCut(std::vector<Color> uniqueColors, int colors)
	{
		struct ColorBoxInfo
		{
			std::vector<Color> colors = std::vector<Color>();
			Vec3f error = Vec3f();
			Vec3f averageColor = Vec3f();
			bool beenSet = false;
		};

		std::vector< ColorBoxInfo > boxes = std::vector< ColorBoxInfo >();
		boxes.push_back( ColorBoxInfo() );

		boxes[0].colors = uniqueColors;

		while(true)
		{
			for(ColorBoxInfo& f : boxes)
			{
				if(!f.beenSet)
				{
					//first, find mean color
					Vec3f meanColor = Vec3f();
					double meanMult = 1.0/f.colors.size();
					for(int i=0; i<f.colors.size(); i++)
					{
						Color v = f.colors[i];
						meanColor.x += (double)v.red * meanMult;
						meanColor.y += (double)v.green * meanMult;
						meanColor.z += (double)v.blue * meanMult;
					}

					//then find error^2 for x, y and z.
					Vec3f error = Vec3f();
					for(int i=0; i<f.colors.size(); i++)
					{
						Color v = f.colors[i];
						error.x += MathExt::sqr(meanColor.x - v.red);
						error.y += MathExt::sqr(meanColor.y - v.green);
						error.z += MathExt::sqr(meanColor.z - v.blue);
					}

					f.averageColor = meanColor;
					f.error = error;
					f.beenSet = true;
				}
			}

			if(boxes.size() >= colors)
			{
				break;
			}

			int indexOfMostError = 0;
			double mostError = 0;

			for(int i=0; i<boxes.size(); i++)
			{
				Vec3f errVec = boxes[i].error;
				double sumError = errVec.x + errVec.y + errVec.z;

				if(sumError > mostError)
				{
					mostError = sumError;
					indexOfMostError = i;
				}
			}
			
			if(boxes[indexOfMostError].colors.size() <= 1)
			{
				//can't split
				break;
			}

			ColorBoxInfo box = boxes[indexOfMostError];

			Vec3f error = box.error;
			Vec3f avg = box.averageColor;
			
			std::vector<Color> split1 = std::vector<Color>();
			std::vector<Color> split2 = std::vector<Color>();
			
			//split the box along the part with the most error
			if(error.x > error.y)
			{
				if(error.x > error.z)
				{
					//split among x
					for(int i=0; i<box.colors.size(); i++)
					{
						Color c = box.colors[i];
						if((double)c.red < avg.x)
						{
							split1.push_back(c);
						}
						else
						{
							split2.push_back(c);
						}
					}
				}
				else
				{
					//split among z
					for(int i=0; i<box.colors.size(); i++)
					{
						Color c = box.colors[i];
						if((double)c.blue < avg.z)
						{
							split1.push_back(c);
						}
						else
						{
							split2.push_back(c);
						}
					}
				}
			}
			else
			{
				if(error.y > error.z)
				{
					//split among y
					for(int i=0; i<box.colors.size(); i++)
					{
						Color c = box.colors[i];
						if((double)c.green < avg.y)
						{
							split1.push_back(c);
						}
						else
						{
							split2.push_back(c);
						}
					}
				}
				else
				{
					//split among z
					for(int i=0; i<box.colors.size(); i++)
					{
						Color c = box.colors[i];
						if((double)c.blue < avg.z)
						{
							split1.push_back(c);
						}
						else
						{
							split2.push_back(c);
						}
					}
				}
			}

			boxes[indexOfMostError].colors = split1;
			boxes[indexOfMostError].beenSet = false;

			boxes.push_back( ColorBoxInfo() );
			boxes.back().colors = split2;

			//after split, you have 2 boxes. repeat algorithm on all boxes.
			//Split on the box with the most error which is the sum of all of the boxes x,y,z error
			//Insures that you end up with the amount of boxes you need to get the appropriate amount of colors
			//Average the colors on each box to get the final paletteArr.
		}

		std::vector<Color> finalColors = std::vector<Color>();

		for(ColorBoxInfo b : boxes)
		{
			//avg color for each box has already been found
			Vec3f meanColor = b.averageColor;
			//round
			double cr = MathExt::clamp(MathExt::round(meanColor.x), 0.0, 255.0);
			double cg = MathExt::clamp(MathExt::round(meanColor.y), 0.0, 255.0);
			double cb = MathExt::clamp(MathExt::round(meanColor.z), 0.0, 255.0);

			finalColors.push_back( {(unsigned char)cr, (unsigned char)cg, (unsigned char)cb, 255} );
		}

		return finalColors;
	}

	std::vector<Color> ColorPalette::medianCut(std::vector<Color> colorArray, int colors)
	{
		std::vector<int> endPos = std::vector<int>();
		std::vector<Color> sortArray = std::vector<Color>(colorArray);

		endPos.push_back(colorArray.size());

		while(endPos.size() < colors)
		{
			int currentSize = endPos.size();
			for(int i=0; i<currentSize; i++)
			{
				int start = 0;
				int end = endPos[i];
				if(i!=0)
				{
					start = endPos[i-1];
				}

				//sort by most variance
				
				Vec3f mean = Vec3f();
				Vec3f variance = Vec3f();

				double meanMult = 1.0 / (end-start);
				
				for(int v=start; v<end; v++)
				{
					mean.x += meanMult * sortArray[v].red;
					mean.y += meanMult * sortArray[v].green;
					mean.z += meanMult * sortArray[v].blue;
				}

				for(int v=start; v<end; v++)
				{
					variance.x = MathExt::sqr( mean.x - sortArray[v].red);
					variance.y = MathExt::sqr( mean.y - sortArray[v].green);
					variance.z = MathExt::sqr( mean.z - sortArray[v].blue);
				}

				int sortBy = 2;
				if(variance.x > variance.y && variance.x > variance.z)
				{
					sortBy = 0;
				}
				else if(variance.y > variance.x && variance.y > variance.z)
				{
					sortBy = 1;
				}
				
				Sort::mergeSort<Color>(sortArray.data()+start, end-start, [sortBy](Color a, Color b) -> bool{
					unsigned char v = ((unsigned char*)&a)[sortBy];
					unsigned char v2 = ((unsigned char*)&b)[sortBy];

					return v<v2;
				});
			}

			std::vector<int> newEndPos = std::vector<int>();

			newEndPos.push_back(endPos[0]/2);
			newEndPos.push_back(endPos[0]);

			for(int i=1; i<currentSize; i++)
			{
				int midIndex = (endPos[i-1] + endPos[i])/2;
				
				newEndPos.push_back(midIndex);
				newEndPos.push_back(endPos[i]);
			}

			endPos = newEndPos;
		}

		//average out each sections for the final set of colors
		std::vector<Color> finalColors = std::vector<Color>();
		for(int i=0; i<endPos.size(); i++)
		{
			int start = 0;
			int end = endPos[i];
			if(i!=0)
			{
				start = endPos[i-1];
			}

			Vec3f avgVal = Vec3f();
			int divVal = end-start;
			for(int k=start; k<end; k++)
			{
				Color c = sortArray[k];
				avgVal.x += c.red;
				avgVal.y += c.green;
				avgVal.z += c.blue;
			}

			avgVal/=divVal;

			//round
			double cr = MathExt::clamp(MathExt::round(avgVal.x), 0.0, 255.0);
			double cg = MathExt::clamp(MathExt::round(avgVal.y), 0.0, 255.0);
			double cb = MathExt::clamp(MathExt::round(avgVal.z), 0.0, 255.0);

			finalColors.push_back( {(unsigned char)cr, (unsigned char)cg, (unsigned char)cb, 255} );
		}

		return finalColors;
	}

	std::vector<Color> ColorPalette::kMeans(std::vector<Color> colorArray, int colors, int maxIterations)
	{
		struct ColorBoxInfo
		{
			std::vector<Color> colors = std::vector<Color>();
			Vec3f averageColor = Vec3f();
		};

		//pick k means
		std::vector< ColorBoxInfo > groups = std::vector< ColorBoxInfo >();
		
		//pick randomly
		unsigned int currentAmount = 0;
		LCG lcg = LCG( (unsigned int)System::getCurrentTimeNano(), 12354, 0, colorArray.size());

		for(int i=0; i<colors; i++)
		{
			int v = lcg.get();
			ColorBoxInfo b = ColorBoxInfo();
			b.averageColor = Vec3f(colorArray[v].red, colorArray[v].green, colorArray[v].blue);

			groups.push_back( b );
		}

		struct importantData
		{
			double a;
			double b;
			double c;
			int location;
		};

		//do k-means
		for(int i=0; i<maxIterations; i++)
		{
			//make kdtree for speed
			KDTree<double> meanTree = KDTree<double>(3);
			for(int k=0; k<groups.size(); k++)
			{
				importantData* tmp = new importantData();
				tmp->a = groups[k].averageColor.x;
				tmp->b = groups[k].averageColor.y;
				tmp->c = groups[k].averageColor.z;
				tmp->location = k;

				meanTree.add((double*)tmp);
			}

			//group into k groups
			for(int k=0; k<colorArray.size(); k++)
			{
				double* testData = new double[3]{(double)colorArray[k].red, (double)colorArray[k].green, (double)colorArray[k].blue};
				
				KDTreeNode<double> nearest = meanTree.searchNearest(testData);
				importantData* actualData = (importantData*)nearest.data;
				groups[actualData->location].colors.push_back( colorArray[k] );
			}

			//recompute average
			bool same = true;
			for(int j=0; j<groups.size(); j++)
			{
				Vec3f avg = Vec3f();
				double divVal = 1.0 / groups[j].colors.size();

				for(Color c : groups[j].colors)
				{
					avg.x += (double)c.red * divVal;
					avg.y += (double)c.green * divVal;
					avg.z += (double)c.blue * divVal;
				}

				if(groups[j].averageColor != avg)
				{
					same = false;
				}

				groups[j].averageColor = avg;
				groups[j].colors.clear();
			}

			if(same)
			{
				break;
			}
		}

		std::vector<Color> avgColors = std::vector<Color>();
		for(ColorBoxInfo k : groups)
		{
			//round
			double cr = MathExt::clamp(MathExt::round(k.averageColor.x), 0.0, 255.0);
			double cg = MathExt::clamp(MathExt::round(k.averageColor.y), 0.0, 255.0);
			double cb = MathExt::clamp(MathExt::round(k.averageColor.z), 0.0, 255.0);

			avgColors.push_back( {(unsigned char)cr, (unsigned char)cg, (unsigned char)cb, 255} );
		}

		return avgColors;
	}

} //NAMESPACE glib END