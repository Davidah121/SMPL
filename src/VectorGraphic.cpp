#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

#pragma region VectorGraphic

const Class* VectorGraphic::myClass = new Class("VectorGraphic", {Object::myClass});
const Class* VectorGraphic::getClass()
{
	return VectorGraphic::myClass;
}

VectorGraphic::VectorGraphic(int width, int height)
{
	this->width = width;
	this->height = height;
	//invalid otherwise and will morph to the image it will be drawn upon.
}

VectorGraphic::VectorGraphic(const VectorGraphic& c)
{
	copy(c);
}

void VectorGraphic::operator=(const VectorGraphic& c)
{
	copy(c);
}

void VectorGraphic::copy(const VectorGraphic& c)
{
	//StringTools::println("Copy Function _ VG");
	this->width = c.width;
	this->height = c.height;

	for(int i=0; i<c.shapes.size(); i++)
	{
		if(c.shapes[i]->getClass()->getClassName() == VectorRectangle::myClass->getClassName())
		{
			//rectangle
			VectorRectangle* g = new VectorRectangle(*(VectorRectangle*)c.shapes[i]);
			shapes.push_back(g);
		}
		else if(c.shapes[i]->getClass()->getClassName() == VectorLine::myClass->getClassName())
		{
			//line
			VectorLine* g = new VectorLine(*(VectorLine*)c.shapes[i]);
			shapes.push_back(g);
		}
		else if(c.shapes[i]->getClass()->getClassName() == VectorCircle::myClass->getClassName())
		{
			//circle
			VectorCircle* g = new VectorCircle(*(VectorCircle*)c.shapes[i]);
			shapes.push_back(g);
		}
		else if(c.shapes[i]->getClass()->getClassName() == VectorEllipse::myClass->getClassName())
		{
			//ellipse
			VectorEllipse* g = new VectorEllipse(*(VectorEllipse*)c.shapes[i]);
			shapes.push_back(g);
		}
		else if(c.shapes[i]->getClass()->getClassName() == VectorPolygon::myClass->getClassName())
		{
			//polygon
			VectorPolygon* g = new VectorPolygon(*(VectorPolygon*)c.shapes[i]);
			shapes.push_back(g);
		}
		else if(c.shapes[i]->getClass()->getClassName() == VectorPolyline::myClass->getClassName())
		{
			//polyline
			VectorPolyline* g = new VectorPolyline(*(VectorPolyline*)c.shapes[i]);
			shapes.push_back(g);
		}
		else if(c.shapes[i]->getClass()->getClassName() == VectorPath::myClass->getClassName())
		{
			//path
			//StringTools::println("PATH");
			VectorPath* g = new VectorPath(*(VectorPath*)c.shapes[i]);
			shapes.push_back(g);
		}
	}
}
VectorGraphic::~VectorGraphic()
{
	clearShapes();
}

void VectorGraphic::addShape(VectorShape* k)
{
	shapes.push_back(k);
}

void VectorGraphic::clearShapes()
{
	shapes.clear();
}

void VectorGraphic::draw(Image* buffer)
{
	if(buffer!=nullptr)
	{
		Mat3f globalTransform = viewBox * transform;
		Mat3f temp = Mat3f::getIdentity();
		Mat3f finalTransform;
		
		for (int i = 0; i < shapes.size(); i++)
		{
			temp = shapes[i]->getTransform();

			finalTransform = globalTransform * temp;

			shapes[i]->setTransform(finalTransform);

			shapes[i]->draw(buffer, this->width, this->height);
			shapes[i]->drawStroke(buffer, this->width, this->height);

			shapes[i]->setTransform(temp);
		}
	}
}

void VectorGraphic::setTransform(Mat3f m)
{
	transform = m;
}

Mat3f VectorGraphic::getTransform()
{
	return transform;
}

void VectorGraphic::setViewBox(Mat3f m)
{
	viewBox = m;
}

Mat3f VectorGraphic::getViewBox()
{
	return viewBox;
}

int VectorGraphic::getWidth()
{
	return width;
}

int VectorGraphic::getHeight()
{
	return height;
}

bool VectorGraphic::save(std::string filename)
{

	return false;
}

bool VectorGraphic::load(std::string filename)
{
	SimpleXml file = SimpleXml();
	bool valid = file.load(StringTools::toWideString(filename));

	if(valid)
	{
		for(XmlNode* n : file.nodes)
		{
			valid = load(n);
			if(valid)
				break;
		}
			
	}
	else
	{
		//failed to load for some reason
	}
	
	return valid;
}

bool VectorGraphic::load(XmlNode* svgParentNode)
{
	bool valid = svgParentNode!=nullptr;

	if(valid)
		valid = StringTools::equalsIgnoreCase(svgParentNode->title, L"svg");

	if(valid)
	{
		XmlNode* parentNode = svgParentNode;
		
		for(XmlAttribute attrib : parentNode->attributes)
		{
			if(StringTools::equalsIgnoreCase(L"width", attrib.name))
			{
				bool percent = false;
				double value = toNumber(StringTools::toCString(attrib.value), &percent);
				if(percent)
					this->width = (int)MathExt::ceil(this->width * value);
				else
					this->width = (int)MathExt::ceil(value);
				
			}
			else if(StringTools::equalsIgnoreCase(L"height", attrib.name))
			{
				bool percent = false;
				double value = toNumber(StringTools::toCString(attrib.value), &percent);
				if(percent)
					this->height = (int)MathExt::ceil(this->height * value);
				else
					this->height = (int)MathExt::ceil(value);
			}
			else if(StringTools::equalsIgnoreCase(L"viewbox", attrib.name))
			{
				//for now, if width and height have not been defined, set them here
				std::vector<std::wstring> split = StringTools::splitString(attrib.value, L' ');
				bool percent = false;
				if(width==0)
				{
					double value = toNumber(StringTools::toCString(split[2]), &percent);
					this->width = (int)MathExt::ceil(value);
				}
				if(height==0)
				{
					double value = toNumber(StringTools::toCString(split[3]), &percent);
					this->height = (int)MathExt::ceil(value);
				}
			}
		}

		if(height>0 && width>0)
		{
			
		}
		else
		{
			return false;
		}
		
		loadNode(parentNode);
	}

	return valid;
}

void VectorGraphic::loadNode(XmlNode* parentNode)
{
	Mat3f groupTransform = Mat3f::getIdentity();
	VectorShape groupShape = VectorShape();

	if(parentNode->title == L"g")
	{
		//group. Probably has a transform or color

		for(XmlAttribute attrib : parentNode->attributes)
		{
			double value = 0;
			bool percentValue = false;
			if(StringTools::equalsIgnoreCase(attrib.name, L"fill"))
			{
				Color c = toColor(StringTools::toCString(attrib.value));
				groupShape.setFillColor( c );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"fill-opacity"))
			{
				Color c = groupShape.getFillColor();
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				c.alpha = (unsigned char)(255*value);
				groupShape.setFillColor( c );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"fill-rule"))
			{
				if(StringTools::equalsIgnoreCase(attrib.value, L"nonzero"))
				{
					groupShape.setFillMethod(VectorShape::NON_ZERO);
				}
				else
				{
					groupShape.setFillMethod(VectorShape::EVEN_ODD_RULE);
				}
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke"))
			{
				Color c = toColor(StringTools::toCString(attrib.value));
				groupShape.setStrokeColor( c );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-opacity"))
			{
				Color c = groupShape.getStrokeColor();
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				c.alpha = (unsigned char)(255*value);
				groupShape.setStrokeColor( c );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-width"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					groupShape.setStrokeWidth( width* value );
				else
					groupShape.setStrokeWidth( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-linecap"))
			{
				if(attrib.value==L"butt")
				{
					groupShape.setLineCap(VectorShape::LINE_CAP_BUTT);
				}
				else if(attrib.value==L"square")
				{
					groupShape.setLineCap(VectorShape::LINE_CAP_SQUARE);
				}
				else if(attrib.value==L"round")
				{
					groupShape.setLineCap(VectorShape::LINE_CAP_ROUND);
				}
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-linejoin"))
			{
				if(attrib.value==L"arcs")
				{
					//groupShape.setLineCap(NULL);
				}
				else if(attrib.value==L"bevel")
				{
					groupShape.setLineJoin(VectorShape::LINE_JOIN_BEVEL);
				}
				else if(attrib.value==L"miter")
				{
					groupShape.setLineJoin(VectorShape::LINE_JOIN_MITER);
				}
				else if(attrib.value==L"miter-clip")
				{
					//groupShape.setLineCap(NULL);
				}
				else if(attrib.value==L"round")
				{
					groupShape.setLineJoin(VectorShape::LINE_JOIN_ROUND);
				}
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-dasharray"))
			{
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"transform"))
			{
				Mat3f thisTransform = Mat3f::getIdentity();
				std::vector<std::wstring> splitString = StringTools::splitString(attrib.value, ' ');
				for(std::wstring transformName : splitString)
				{
					int indexOfArgs = transformName.find('(', 0);
					std::wstring subName = transformName.substr(0, indexOfArgs);
					std::wstring subArgs = transformName.substr(indexOfArgs+1, transformName.length()-indexOfArgs-2);

					std::vector<std::wstring> args = StringTools::splitString(subArgs, ',');

					if(subName == L"translate")
					{
						switch(args.size())
						{
							case 1:
								thisTransform = MathExt::translation2D( std::stod(args[0]), 0) * thisTransform;
								break;
							case 2:
								thisTransform = MathExt::translation2D( std::stod(args[0]), std::stod(args[1])) * thisTransform;
								break;
							default:
								break;
						}
					}
					else if(subName == L"scale")
					{
						switch(args.size())
						{
							case 1:
								thisTransform = MathExt::scale2D( std::stod(args[0]), 1) * thisTransform;
								break;
							case 2:
								thisTransform = MathExt::scale2D( std::stod(args[0]), std::stod(args[1])) * thisTransform;
								break;
							default:
								break;
						}
					}
					else if(subName == L"rotate")
					{
						switch(args.size())
						{
							case 1:
								thisTransform = MathExt::rotation2D( std::stod(args[0]), 0, 0) * thisTransform;
								break;
							case 3:
								thisTransform = MathExt::rotation2D( std::stod(args[0]), std::stod(args[1]), std::stod(args[2])) * thisTransform;
								break;
							default:
								break;
						}
					}
					else if(subName == L"skewX")
					{
						if(args.size() == 1)
						{
							thisTransform = MathExt::scale2D( std::stod(args[0]), 0) * thisTransform;
						}
					}
					else if(subName == L"skewY")
					{
						if(args.size() == 1)
						{
							thisTransform = MathExt::skew2D( 0, std::stod(args[0])) * thisTransform;
						}
					}
					else if(subName == L"matrix")
					{
						if(args.size() == 6)
						{
							Mat3f m = Mat3f::getIdentity();
							m[0][0] = std::stod(args[0]);
							m[0][1] = std::stod(args[2]);
							m[0][2] = std::stod(args[4]);

							m[1][0] = std::stod(args[1]);
							m[1][1] = std::stod(args[3]);
							m[1][2] = std::stod(args[5]);

							thisTransform = m * thisTransform;
						}
					}
				}
				groupShape.setTransform(thisTransform);
			}
		}
	}

	for(XmlNode* childNode : parentNode->childNodes)
	{
		if(childNode==nullptr)
		{
			continue;
		}

		VectorShape* shape = nullptr;
		bool percentValue = false;
		double value = 0;
		double diagonalLength = 0;
		if(StringTools::equalsIgnoreCase(childNode->title, L"g"))
		{
			//group
			loadNode(childNode);
		}
		else if(StringTools::equalsIgnoreCase(childNode->title, L"rect"))
		{
			//rectangle
			VectorRectangle* g = new VectorRectangle();

			for(XmlAttribute attrib : childNode->attributes)
			{
				if(StringTools::equalsIgnoreCase(attrib.name, L"x"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setX( width*value );
					else
						g->setX( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"y"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setY( height*value );
					else
						g->setY( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"rx"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setRX( width*value );
					else
						g->setRX( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"ry"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setRY( height*value );
					else
						g->setRY( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"width"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setWidth( width*value );
					else
						g->setWidth( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"height"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setHeight( height*value );
					else
						g->setHeight( value );
				}
			}

			shape = (VectorShape*)g;
		}
		else if(StringTools::equalsIgnoreCase(childNode->title, L"circle"))
		{
			//circle
			VectorCircle* g = new VectorCircle();

			for(XmlAttribute attrib : childNode->attributes)
			{
				if(StringTools::equalsIgnoreCase(attrib.name, L"cx"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setX( width*value );
					else
						g->setX( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"cy"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setY( height*value );
					else
						g->setY( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"r"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setRadius( width*value );
					else
						g->setRadius( value );
				}
			}

			shape = (VectorShape*)g;
		}
		else if(StringTools::equalsIgnoreCase(childNode->title, L"ellipse"))
		{
			//ellipse
			VectorEllipse* g = new VectorEllipse();

			for(XmlAttribute attrib : childNode->attributes)
			{
				if(StringTools::equalsIgnoreCase(attrib.name, L"cx"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setX( width*value );
					else
						g->setX( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"cy"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setY( height*value );
					else
						g->setY( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"rx"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setXRadius( width*value );
					else
						g->setXRadius( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"ry"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setYRadius( height*value );
					else
						g->setYRadius( value );
				}
			}

			shape = (VectorShape*)g;
		}
		else if(StringTools::equalsIgnoreCase(childNode->title, L"line"))
		{
			//line
			VectorLine* g = new VectorLine();

			for(XmlAttribute attrib : childNode->attributes)
			{
				if(StringTools::equalsIgnoreCase(attrib.name, L"x1"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setX1( width*value );
					else
						g->setX1( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"y1"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setY1( height*value );
					else
						g->setY1( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"x2"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setX2( width*value );
					else
						g->setX2( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"y2"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						g->setY2( height*value );
					else
						g->setY2( value );
				}
			}

			shape = (VectorShape*)g;
		}
		else if(StringTools::equalsIgnoreCase(childNode->title, L"polygon"))
		{
			//polygon
			VectorPolygon* g = new VectorPolygon();
			for(XmlAttribute attrib : childNode->attributes)
			{
				if(StringTools::equalsIgnoreCase(attrib.name, L"points"))
				{
					std::vector<std::wstring> splits = StringTools::splitString(attrib.value, ' ');
					for(std::wstring point : splits)
					{
						std::vector<std::wstring> pointSplit = StringTools::splitString(point, ',');
						if(pointSplit.size()==2)
							g->addPoint( std::stod(pointSplit[0]), std::stod(pointSplit[1]) );
					}
				}
			}
			shape = (VectorShape*)g;
		}
		else if(StringTools::equalsIgnoreCase(childNode->title, L"polyline"))
		{
			//polyline
			VectorPolyline* g = new VectorPolyline();
			for(XmlAttribute attrib : childNode->attributes)
			{
				if(StringTools::equalsIgnoreCase(attrib.name, L"points"))
				{
					std::vector<std::wstring> splits = StringTools::splitString(attrib.value, ' ');
					for(std::wstring point : splits)
					{
						std::vector<std::wstring> pointSplit = StringTools::splitString(point, ',');
						if(pointSplit.size()==2)
							g->addPoint( std::stod(pointSplit[0]), std::stod(pointSplit[1]) );
					}
				}
			}
			shape = (VectorShape*)g;
		}
		else if(StringTools::equalsIgnoreCase(childNode->title, L"path"))
		{
			//path
			VectorPath* g = new VectorPath();
			for(XmlAttribute attrib : childNode->attributes)
			{
				if(StringTools::equalsIgnoreCase(attrib.name, L"d"))
				{
					//Method: Separate Letters from values
					//separate letters first
					std::vector<char> instructions = std::vector<char>();
					std::string numbers = "";
					int argNum = 0;
					int parsedArgs = 0;
					bool rel = false;
					for(char c : attrib.value)
					{
						if(c>=65 && c<=90 || c>=97 && c<=127)
						{
							instructions.push_back(c);

							if(c <= 90)
							{
								rel = false;
							}
							else
							{
								rel = true;
							}
							
							switch (std::tolower(c))
							{
							case 'm':
								argNum = 2;
								break;
							case 'l':
								argNum = 2;
								break;
							case 'h':
								argNum = 1;
								break;
							case 'v':
								argNum = 1;
								break;
							case 'z':
								argNum = 0;
								break;
							case 'c':
								argNum = 6;
								break;
							case 's':
								argNum = 4;
								break;
							case 'q':
								argNum = 4;
								break;
							case 't':
								argNum = 2;
								break;
							case 'a':
								argNum = 7;
								break;
							default:
								argNum = -1;
								break;
							}
							parsedArgs = 0;
							numbers += ' ';
						}
						else if(c>=32)
						{
							if(parsedArgs>=argNum && c!=' ')
							{
								parsedArgs = 0;
								if(rel)
									instructions.push_back( 'l' );
								else
									instructions.push_back( 'L' );
								argNum = 2;
							}

							if( (c==' ' || c==',' || c=='-') && numbers.back()!= ' ')
							{
								if(c=='-')
								{
									if(numbers.back()>='0' && numbers.back()<='9')
									{
										numbers += ' ';
										parsedArgs++;
									}
								}
								else
								{
									parsedArgs++;
								}
							}

							numbers += c;
						}
					}
					
					std::vector<std::string> splitNumbers = StringTools::splitStringMultipleDeliminators(numbers, " ,");
					
					int numberIndex = 0;
					for(char c : instructions)
					{
						switch (c)
						{
							case 'M':
								//move to
								g->addMoveTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]) );
								numberIndex += 2;
								break;
							case 'm':
								//move to relative
								g->addMoveToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]) );
								numberIndex += 2;
								break;
							case 'L':
								//Line to
								g->addLineTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]) );
								numberIndex += 2;
								break;
							case 'l':
								//Line to relative
								g->addLineToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]) );
								numberIndex += 2;
								break;
							case 'H':
								//Horizontal to
								g->addHorizontalTo( std::stod(splitNumbers[numberIndex]) );
								numberIndex += 1;
								break;
							case 'h':
								//horizontal to relative
								g->addHorizontalToRel( std::stod(splitNumbers[numberIndex]) );
								numberIndex += 1;
								break;
							case 'V':
								//Vertical to
								g->addVerticalTo( std::stod(splitNumbers[numberIndex]) );
								numberIndex += 1;
								break;
							case 'v':
								//vertical to relative
								g->addVerticalToRel( std::stod(splitNumbers[numberIndex]) );
								numberIndex += 1;
								break;
							case 'Z':
								g->addClosePath();
								break;
							case 'z':
								g->addClosePath();
								break;
							case 'Q':
								g->addQuadTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]));
								numberIndex += 4;
								break;
							case 'q':
								g->addQuadToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]));
								numberIndex += 4;
								break;
							case 'T':
								g->addQuadToShort( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]));
								numberIndex += 2;
								break;
							case 't':
								g->addQuadToShortRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]));
								numberIndex += 2;
								break;
							case 'C':
								g->addCubicTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]), std::stod(splitNumbers[numberIndex+4]), std::stod(splitNumbers[numberIndex+5]));
								numberIndex += 6;
								break;
							case 'c':
								g->addCubicToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]), std::stod(splitNumbers[numberIndex+4]), std::stod(splitNumbers[numberIndex+5]));
								numberIndex += 6;
								break;
							case 'S':
								g->addCubicToShort( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]));
								numberIndex += 4;
								break;
							case 's':
								g->addCubicToShortRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]));
								numberIndex += 4;
								break;
							case 'A':
								g->addArcTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), splitNumbers[numberIndex+3]=="1", splitNumbers[numberIndex+4]=="1", std::stod(splitNumbers[numberIndex+5]), std::stod(splitNumbers[numberIndex+6]));
								numberIndex += 7;
								break;
							case 'a':
								g->addArcToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), splitNumbers[numberIndex+3]=="1", splitNumbers[numberIndex+4]=="1", std::stod(splitNumbers[numberIndex+5]), std::stod(splitNumbers[numberIndex+6]));
								numberIndex += 7;
								break;
							default:
								break;
						}
					}
				}
			}

			shape = (VectorShape*)g;
		}

		if(shape!=nullptr)
		{
			shape->setFillColor( groupShape.getFillColor() );
			shape->setFillMethod( groupShape.getFillMethod() );
			shape->setStrokeColor( groupShape.getStrokeColor() );
			shape->setStrokeWidth( groupShape.getStrokeWidth() );
			shape->setLineCap( groupShape.getLineCap() );
			shape->setLineJoin( groupShape.getLineJoin() );
			shape->setTransform( groupShape.getTransform() );
			
			for(XmlAttribute attrib : childNode->attributes)
			{
				if(StringTools::equalsIgnoreCase(attrib.name, L"fill"))
				{
					Color c = toColor(StringTools::toCString(attrib.value));
					shape->setFillColor( c );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"fill-opacity"))
				{
					Color c = shape->getFillColor();
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					c.alpha = (unsigned char)(255*value);
					shape->setFillColor( c );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"fill-rule"))
				{
					if(StringTools::equalsIgnoreCase(attrib.value, L"nonzero"))
					{
						shape->setFillMethod(VectorShape::NON_ZERO);
					}
					else
					{
						shape->setFillMethod(VectorShape::EVEN_ODD_RULE);
					}
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke"))
				{
					Color c = toColor(StringTools::toCString(attrib.value));
					shape->setStrokeColor( c );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-opacity"))
				{
					Color c = shape->getStrokeColor();
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					c.alpha = (unsigned char)(255*value);
					shape->setStrokeColor( c );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-width"))
				{
					value = toNumber(StringTools::toCString(attrib.value), &percentValue);
					if(percentValue)
						shape->setStrokeWidth( width* value );
					else
						shape->setStrokeWidth( value );
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-linecap"))
				{
					if(attrib.value==L"butt")
					{
						shape->setLineCap(VectorShape::LINE_CAP_BUTT);
					}
					else if(attrib.value==L"square")
					{
						shape->setLineCap(VectorShape::LINE_CAP_SQUARE);
					}
					else if(attrib.value==L"round")
					{
						shape->setLineCap(VectorShape::LINE_CAP_ROUND);
					}
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-linejoin"))
				{
					if(attrib.value==L"arcs")
					{
						//shape->setLineCap(NULL);
					}
					else if(attrib.value==L"bevel")
					{
						shape->setLineJoin(VectorShape::LINE_JOIN_BEVEL);
					}
					else if(attrib.value==L"miter")
					{
						shape->setLineJoin(VectorShape::LINE_JOIN_MITER);
					}
					else if(attrib.value==L"miter-clip")
					{
						//shape->setLineCap(NULL);
					}
					else if(attrib.value==L"round")
					{
						shape->setLineJoin(VectorShape::LINE_JOIN_ROUND);
					}
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"stroke-dasharray"))
				{
				}
				else if(StringTools::equalsIgnoreCase(attrib.name, L"transform"))
				{
					Mat3f thisTransform = Mat3f::getIdentity();
					std::vector<std::wstring> splitString = StringTools::splitString(attrib.value, ' ');
					for(std::wstring transformName : splitString)
					{
						int indexOfArgs = transformName.find('(', 0);
						std::wstring subName = transformName.substr(0, indexOfArgs);
						std::wstring subArgs = transformName.substr(indexOfArgs+1, transformName.length()-indexOfArgs-2);

						std::vector<std::wstring> args = StringTools::splitString(subArgs, ',');

						if(subName == L"translate")
						{
							switch(args.size())
							{
								case 1:
									thisTransform = MathExt::translation2D( std::stod(args[0]), 0) * thisTransform;
									break;
								case 2:
									thisTransform = MathExt::translation2D( std::stod(args[0]), std::stod(args[1])) * thisTransform;
									break;
								default:
									break;
							}
						}
						else if(subName == L"scale")
						{
							switch(args.size())
							{
								case 1:
									thisTransform = MathExt::scale2D( std::stod(args[0]), 1) * thisTransform;
									break;
								case 2:
									thisTransform = MathExt::scale2D( std::stod(args[0]), std::stod(args[1])) * thisTransform;
									break;
								default:
									break;
							}
						}
						else if(subName == L"rotate")
						{
							switch(args.size())
							{
								case 1:
									thisTransform = MathExt::rotation2D( std::stod(args[0]), 0, 0) * thisTransform;
									break;
								case 3:
									thisTransform = MathExt::rotation2D( std::stod(args[0]), std::stod(args[1]), std::stod(args[2])) * thisTransform;
									break;
								default:
									break;
							}
						}
						else if(subName == L"skewX")
						{
							if(args.size() == 1)
							{
								thisTransform = MathExt::scale2D( std::stod(args[0]), 0) * thisTransform;
							}
						}
						else if(subName == L"skewY")
						{
							if(args.size() == 1)
							{
								thisTransform = MathExt::skew2D( 0, std::stod(args[0])) * thisTransform;
							}
						}
						else if(subName == L"matrix")
						{
							if(args.size() == 6)
							{
								Mat3f m = Mat3f::getIdentity();
								m[0][0] = std::stod(args[0]);
								m[0][1] = std::stod(args[2]);
								m[0][2] = std::stod(args[4]);

								m[1][0] = std::stod(args[1]);
								m[1][1] = std::stod(args[3]);
								m[1][2] = std::stod(args[5]);

								thisTransform = m * thisTransform;
							}
						}
					}

					shape->setTransform( groupShape.getTransform() * thisTransform);
				}
			}

			this->addShape(shape);
		}
	}
}

XmlNode* VectorGraphic::writeAsXmlNode()
{
	return nullptr;
}

double VectorGraphic::toNumber(std::string value, bool* percentage)
{
	//could be a percentage, number, or number with a unit (Ignored)
	//parsing stops when invalid character is found so substrings are not
	//used here.
	if(value[ value.size()-1 ] == '%')
	{
		//percentage
		*percentage = true;
		return std::stod(value) / 100.0;
	}
	else
	{
		//normal number. May have a unit associated with it.
		*percentage = false;
		return std::stod(value);
	}
	
	return 0;
}

Color VectorGraphic::toColor(std::string value)
{
	//could be 	rgb(number, number, number),
	//			hex value
	//			color name
	Color c = {0,0,0,255};
	if(value[0] == '#')
	{
		//hex value
		c.red = (unsigned char)( StringTools::base16ToBase10(value[1])*16 + StringTools::base16ToBase10(value[2]) );
		c.green = (unsigned char)( StringTools::base16ToBase10(value[3])*16 + StringTools::base16ToBase10(value[4]) );
		c.blue = (unsigned char)( StringTools::base16ToBase10(value[5])*16 + StringTools::base16ToBase10(value[6]) );
	}
	else
	{
		std::string first4 = value.substr(0, 4);

		if( StringTools::equalsIgnoreCase(first4, "rgb(") && value[value.size()-1] == ')' )
		{
			//rgb value
			std::string actualValues = value.substr(4, value.size() - 5);
			std::vector<std::string> splitNumbers = StringTools::splitString(actualValues, ',');
			if(splitNumbers.size()==3)
			{
				bool isPercent = false;
				double value = VectorGraphic::toNumber(splitNumbers[0], &isPercent);
				if(isPercent)
					c.red = (unsigned char) MathExt::clamp((255.0 * value), 0.0, 255.0);
				else
					c.red = (unsigned char) MathExt::clamp(value, 0.0, 255.0);
				
				value = VectorGraphic::toNumber(splitNumbers[1], &isPercent);
				if(isPercent)
					c.green = (unsigned char) MathExt::clamp((255.0 * value), 0.0, 255.0);
				else
					c.green = (unsigned char) MathExt::clamp(value, 0.0, 255.0);
				
				value = VectorGraphic::toNumber(splitNumbers[2], &isPercent);
				if(isPercent)
					c.blue = (unsigned char) MathExt::clamp((255.0 * value), 0.0, 255.0);
				else
					c.blue = (unsigned char) MathExt::clamp(value, 0.0, 255.0);
			}
		}
		else
		{
			//color name
			c = ColorNameConverter::NameToColor(value);
		}
	}
	
	return c;
}
#pragma endregion


