#include "VectorGraphic.h"
#include "Graphics.h"
#include "Sort.h"
#include "StringTools.h"
#include "BezierCurve.h"
#include "ColorNameConverter.h"

namespace glib
{
		
	#pragma region VectorGraphic

	unsigned char VectorGraphic::AA_LEVEL = 0;

	const Class VectorGraphic::myClass = Class("VectorGraphic", {&Object::myClass});
	const Class* VectorGraphic::getClass()
	{
		return &VectorGraphic::myClass;
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
			if(c.shapes[i]->getClass()->getClassName() == VectorRectangle::myClass.getClassName())
			{
				//rectangle
				VectorRectangle* g = new VectorRectangle(*(VectorRectangle*)c.shapes[i]);
				shapes.push_back(g);
			}
			else if(c.shapes[i]->getClass()->getClassName() == VectorLine::myClass.getClassName())
			{
				//line
				VectorLine* g = new VectorLine(*(VectorLine*)c.shapes[i]);
				shapes.push_back(g);
			}
			else if(c.shapes[i]->getClass()->getClassName() == VectorCircle::myClass.getClassName())
			{
				//circle
				VectorCircle* g = new VectorCircle(*(VectorCircle*)c.shapes[i]);
				shapes.push_back(g);
			}
			else if(c.shapes[i]->getClass()->getClassName() == VectorEllipse::myClass.getClassName())
			{
				//ellipse
				VectorEllipse* g = new VectorEllipse(*(VectorEllipse*)c.shapes[i]);
				shapes.push_back(g);
			}
			else if(c.shapes[i]->getClass()->getClassName() == VectorPolygon::myClass.getClassName())
			{
				//polygon
				VectorPolygon* g = new VectorPolygon(*(VectorPolygon*)c.shapes[i]);
				shapes.push_back(g);
			}
			else if(c.shapes[i]->getClass()->getClassName() == VectorPolyline::myClass.getClassName())
			{
				//polyline
				VectorPolyline* g = new VectorPolyline(*(VectorPolyline*)c.shapes[i]);
				shapes.push_back(g);
			}
			else if(c.shapes[i]->getClass()->getClassName() == VectorPath::myClass.getClassName())
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

			int globalWidth = width;
			int globalHeight = height;

			if(width <=0 || height <= 0)
			{
				globalWidth = buffer->getWidth();
				globalHeight = buffer->getHeight();
			}
			
			for (int i = 0; i < shapes.size(); i++)
			{
				temp = shapes[i]->getTransform();

				finalTransform = globalTransform * temp;

				shapes[i]->setTransform(finalTransform);

				if(shapes[i]->getFillColor().alpha > 0)
					shapes[i]->draw(buffer, globalWidth, globalHeight);

				if(shapes[i]->getStrokeColor().alpha > 0)
					shapes[i]->drawStroke(buffer, globalWidth, globalHeight);

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
					std::vector<std::wstring> split = StringTools::splitStringMultipleDeliminators(attrib.value, L" ,");
					bool percent = false;
					int tempX, tempY, tempWidth, tempHeight;

					if(split.size() == 4)
					{
						tempX = (int)MathExt::ceil(toNumber(StringTools::toCString(split[0]), &percent));
						tempY = (int)MathExt::ceil(toNumber(StringTools::toCString(split[1]), &percent));
						tempWidth = (int)MathExt::ceil(toNumber(StringTools::toCString(split[2]), &percent));
						tempHeight = (int)MathExt::ceil(toNumber(StringTools::toCString(split[3]), &percent));
					}

					if(width == 0)
						width = tempWidth;
					if(height == 0)
						height = tempHeight;
					
					double xScale = (double)width / tempWidth;
					double yScale = (double)height / tempHeight;

					viewBox = MathExt::scale2D(xScale, yScale) * MathExt::translation2D(-tempX, -tempY);
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

	void VectorGraphic::loadNode(XmlNode* parentNode, VectorShape* groupPointer)
	{
		for(XmlNode* childNode : parentNode->childNodes)
		{
			if(childNode==nullptr)
			{
				continue;
			}

			VectorShape* shape = nullptr;

			if(StringTools::equalsIgnoreCase(childNode->title, L"g"))
			{
				//group
				VectorShape groupShape = VectorShape();
				loadTransforms(childNode, &groupShape, groupPointer);
				loadNode(childNode, &groupShape);
			}
			else if(StringTools::equalsIgnoreCase(childNode->title, L"rect"))
			{
				//rectangle
				VectorRectangle* g = new VectorRectangle();
				loadRectangle(childNode, g);
				shape = (VectorShape*)g;
			}
			else if(StringTools::equalsIgnoreCase(childNode->title, L"circle"))
			{
				//circle
				VectorCircle* g = new VectorCircle();
				loadCircle(childNode, g);
				shape = (VectorShape*)g;
			}
			else if(StringTools::equalsIgnoreCase(childNode->title, L"ellipse"))
			{
				//ellipse
				VectorEllipse* g = new VectorEllipse();
				loadEllipse(childNode, g);
				shape = (VectorShape*)g;
			}
			else if(StringTools::equalsIgnoreCase(childNode->title, L"line"))
			{
				//line
				VectorLine* g = new VectorLine();
				loadLine(childNode, g);
				shape = (VectorShape*)g;
			}
			else if(StringTools::equalsIgnoreCase(childNode->title, L"polygon"))
			{
				//polygon
				VectorPolygon* g = new VectorPolygon();
				loadPolygon(childNode, g);
				shape = (VectorShape*)g;
			}
			else if(StringTools::equalsIgnoreCase(childNode->title, L"polyline"))
			{
				//polyline
				VectorPolyline* g = new VectorPolyline();
				loadPolyline(childNode, g);
				shape = (VectorShape*)g;
			}
			else if(StringTools::equalsIgnoreCase(childNode->title, L"path"))
			{
				//path
				VectorPath* g = new VectorPath();
				loadPath(childNode, g);
				shape = (VectorShape*)g;
			}

			if(shape!=nullptr)
			{
				loadTransforms(childNode, shape, groupPointer);
				this->addShape(shape);
			}
		}
	}

	void VectorGraphic::loadTransforms(XmlNode* node, VectorShape* shape, VectorShape* groupPointer)
	{
		bool percentValue = false;
		double value = 0;
		double diagonalLength = 0;

		if(node==nullptr)
		{
			//invalid node pointer
			return;
		}

		if(shape==nullptr)
		{
			//invalid shape pointer
			return;
		}

		if(groupPointer!=nullptr)
		{
			shape->setFillColor( groupPointer->getFillColor() );
			shape->setFillMethod( groupPointer->getFillMethod() );
			shape->setStrokeColor( groupPointer->getStrokeColor() );
			shape->setStrokeWidth( groupPointer->getStrokeWidth() );
			shape->setLineCap( groupPointer->getLineCap() );
			shape->setLineJoin( groupPointer->getLineJoin() );
			shape->setTransform( groupPointer->getTransform() );
		}
		
		for(XmlAttribute& attrib : node->attributes)
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
				Mat3f thisTransform = shape->getTransform();
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

				shape->setTransform(thisTransform);
			}
		}
	}

	void VectorGraphic::loadRectangle(XmlNode* node, VectorRectangle* shape)
	{
		//rectangle
		double value = 0;
		bool percentValue = false;

		if(shape == nullptr)
		{
			//invalid shape pointer
			return;
		}
		if(node == nullptr)
		{
			//invalid node pointer
			return;
		}
		if(!StringTools::equalsIgnoreCase(node->title, L"rect"))
		{
			//invalid node
			return;
		}

		for(XmlAttribute& attrib : node->attributes)
		{
			if(StringTools::equalsIgnoreCase(attrib.name, L"x"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setX( width*value );
				else
					shape->setX( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"y"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setY( height*value );
				else
					shape->setY( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"rx"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setRX( width*value );
				else
					shape->setRX( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"ry"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setRY( height*value );
				else
					shape->setRY( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"width"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setWidth( width*value );
				else
					shape->setWidth( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"height"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setHeight( height*value );
				else
					shape->setHeight( value );
			}
		}
	}

	void VectorGraphic::loadCircle(XmlNode* node, VectorCircle* shape)
	{
		double value = 0;
		bool percentValue = false;

		if(shape == nullptr)
		{
			//invalid shape pointer
			return;
		}
		if(node == nullptr)
		{
			//invalid node pointer
			return;
		}
		if(!StringTools::equalsIgnoreCase(node->title, L"circle"))
		{
			//invalid node
			return;
		}

		for(XmlAttribute attrib : node->attributes)
		{
			if(StringTools::equalsIgnoreCase(attrib.name, L"cx"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setX( width*value );
				else
					shape->setX( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"cy"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setY( height*value );
				else
					shape->setY( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"r"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setRadius( width*value );
				else
					shape->setRadius( value );
			}
		}
	}

	void VectorGraphic::loadEllipse(XmlNode* node, VectorEllipse* shape)
	{
		double value = 0;
		bool percentValue = false;

		if(shape == nullptr)
		{
			//invalid shape pointer
			return;
		}
		if(node == nullptr)
		{
			//invalid node pointer
			return;
		}
		if(!StringTools::equalsIgnoreCase(node->title, L"ellipse"))
		{
			//invalid node
			return;
		}

		for(XmlAttribute attrib : node->attributes)
		{
			if(StringTools::equalsIgnoreCase(attrib.name, L"cx"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setX( width*value );
				else
					shape->setX( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"cy"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setY( height*value );
				else
					shape->setY( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"rx"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setXRadius( width*value );
				else
					shape->setXRadius( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"ry"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setYRadius( height*value );
				else
					shape->setYRadius( value );
			}
		}
	}

	void VectorGraphic::loadLine(XmlNode* node, VectorLine* shape)
	{
		double value = 0;
		bool percentValue = false;

		if(shape == nullptr)
		{
			//invalid shape pointer
			return;
		}
		if(node == nullptr)
		{
			//invalid node pointer
			return;
		}
		if(!StringTools::equalsIgnoreCase(node->title, L"line"))
		{
			//invalid node
			return;
		}

		for(XmlAttribute attrib : node->attributes)
		{
			if(StringTools::equalsIgnoreCase(attrib.name, L"x1"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setX1( width*value );
				else
					shape->setX1( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"y1"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setY1( height*value );
				else
					shape->setY1( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"x2"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setX2( width*value );
				else
					shape->setX2( value );
			}
			else if(StringTools::equalsIgnoreCase(attrib.name, L"y2"))
			{
				value = toNumber(StringTools::toCString(attrib.value), &percentValue);
				if(percentValue)
					shape->setY2( height*value );
				else
					shape->setY2( value );
			}
		}
	}

	void VectorGraphic::loadPolygon(XmlNode* node, VectorPolygon* shape)
	{
		if(shape == nullptr)
		{
			//invalid shape pointer
			return;
		}
		if(node == nullptr)
		{
			//invalid node pointer
			return;
		}
		if(!StringTools::equalsIgnoreCase(node->title, L"polygon"))
		{
			//invalid node
			return;
		}

		for(XmlAttribute attrib : node->attributes)
		{
			if(StringTools::equalsIgnoreCase(attrib.name, L"points"))
			{
				std::vector<std::wstring> splits = StringTools::splitString(attrib.value, ' ');
				for(std::wstring point : splits)
				{
					std::vector<std::wstring> pointSplit = StringTools::splitString(point, ',');
					if(pointSplit.size()==2)
						shape->addPoint( std::stod(pointSplit[0]), std::stod(pointSplit[1]) );
				}
			}
		}
	}

	void VectorGraphic::loadPolyline(XmlNode* node, VectorPolyline* shape)
	{
		if(shape == nullptr)
		{
			//invalid shape pointer
			return;
		}
		if(node == nullptr)
		{
			//invalid node pointer
			return;
		}
		if(!StringTools::equalsIgnoreCase(node->title, L"polyline"))
		{
			//invalid node
			return;
		}

		for(XmlAttribute attrib : node->attributes)
		{
			if(StringTools::equalsIgnoreCase(attrib.name, L"points"))
			{
				std::vector<std::wstring> splits = StringTools::splitString(attrib.value, ' ');
				for(std::wstring point : splits)
				{
					std::vector<std::wstring> pointSplit = StringTools::splitString(point, ',');
					if(pointSplit.size()==2)
						shape->addPoint( std::stod(pointSplit[0]), std::stod(pointSplit[1]) );
				}
			}
		}
	}

	void VectorGraphic::loadPath(XmlNode* node, VectorPath* shape)
	{
		//path
		double value = 0;
		bool percentValue = false;

		if(shape == nullptr)
		{
			//invalid shape pointer
			return;
		}
		if(node == nullptr)
		{
			//invalid node pointer
			return;
		}
		if(!StringTools::equalsIgnoreCase(node->title, L"path"))
		{
			//invalid node
			return;
		}

		for(XmlAttribute& attrib : node->attributes)
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
							shape->addMoveTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]) );
							numberIndex += 2;
							break;
						case 'm':
							//move to relative
							shape->addMoveToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]) );
							numberIndex += 2;
							break;
						case 'L':
							//Line to
							shape->addLineTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]) );
							numberIndex += 2;
							break;
						case 'l':
							//Line to relative
							shape->addLineToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]) );
							numberIndex += 2;
							break;
						case 'H':
							//Horizontal to
							shape->addHorizontalTo( std::stod(splitNumbers[numberIndex]) );
							numberIndex += 1;
							break;
						case 'h':
							//horizontal to relative
							shape->addHorizontalToRel( std::stod(splitNumbers[numberIndex]) );
							numberIndex += 1;
							break;
						case 'V':
							//Vertical to
							shape->addVerticalTo( std::stod(splitNumbers[numberIndex]) );
							numberIndex += 1;
							break;
						case 'v':
							//vertical to relative
							shape->addVerticalToRel( std::stod(splitNumbers[numberIndex]) );
							numberIndex += 1;
							break;
						case 'Z':
							shape->addClosePath();
							break;
						case 'z':
							shape->addClosePath();
							break;
						case 'Q':
							shape->addQuadTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]));
							numberIndex += 4;
							break;
						case 'q':
							shape->addQuadToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]));
							numberIndex += 4;
							break;
						case 'T':
							shape->addQuadToShort( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]));
							numberIndex += 2;
							break;
						case 't':
							shape->addQuadToShortRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]));
							numberIndex += 2;
							break;
						case 'C':
							shape->addCubicTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]), std::stod(splitNumbers[numberIndex+4]), std::stod(splitNumbers[numberIndex+5]));
							numberIndex += 6;
							break;
						case 'c':
							shape->addCubicToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]), std::stod(splitNumbers[numberIndex+4]), std::stod(splitNumbers[numberIndex+5]));
							numberIndex += 6;
							break;
						case 'S':
							shape->addCubicToShort( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]));
							numberIndex += 4;
							break;
						case 's':
							shape->addCubicToShortRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), std::stod(splitNumbers[numberIndex+3]));
							numberIndex += 4;
							break;
						case 'A':
							shape->addArcTo( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), splitNumbers[numberIndex+3]=="1", splitNumbers[numberIndex+4]=="1", std::stod(splitNumbers[numberIndex+5]), std::stod(splitNumbers[numberIndex+6]));
							numberIndex += 7;
							break;
						case 'a':
							shape->addArcToRel( std::stod(splitNumbers[numberIndex]), std::stod(splitNumbers[numberIndex+1]), std::stod(splitNumbers[numberIndex+2]), splitNumbers[numberIndex+3]=="1", splitNumbers[numberIndex+4]=="1", std::stod(splitNumbers[numberIndex+5]), std::stod(splitNumbers[numberIndex+6]));
							numberIndex += 7;
							break;
						default:
							break;
					}
				}
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

	void VectorGraphic::setAALevel(unsigned char level)
	{
		AA_LEVEL = MathExt::clamp(level, 0, 3);
	}

	unsigned char VectorGraphic::getAALevel()
	{
		return AA_LEVEL;
	}


	#pragma endregion

} //NAMESPACE glib END