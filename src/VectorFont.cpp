#include "VectorFont.h"
#include "StringTools.h"

namespace smpl
{

	VectorFont::VectorFont() : Font()
	{
	}

	VectorFont::~VectorFont()
	{
		
	}

	VectorGraphic* VectorFont::getGraphic(size_t index)
	{
		return fontSprite.getGraphic(index);
	}

	VectorSprite& VectorFont::getVectorSprite()
	{
		return fontSprite;
	}

	SmartMemory<Image> VectorFont::getImage(const FontCharInfo& fci)
	{
		if(fci.width == 0 || fci.height == 0) //even if the graphic exists, it won't be rendered.
			return nullptr;

		double scaleValue = (double)this->getFontSize() / originalFontSize;
		
		SmartMemory<Image> glyphImage = this->cachedGlyphs.getData(fci.reserved);
		if(glyphImage == nullptr)
		{

			//create new image and add to cachedGlyphs
			VectorGraphic* vecGlyph = fontSprite.getGraphic(fci.reserved);
			
			if(vecGlyph != nullptr)
			{
				//can be added.
				Image* newGlyphImg = new Image(fci.width, fci.height);

				Mat3f oldTransform = vecGlyph->getTransform();
				Mat3f newTransform = MathExt::scale2D(scaleValue, scaleValue)*oldTransform;
				vecGlyph->setTransform(newTransform);

				vecGlyph->draw(newGlyphImg);
				vecGlyph->setTransform(oldTransform);

				uint32_t tempReservedData = fci.reserved;
				cachedGlyphs.addData(std::move(tempReservedData), SmartMemory<Image>::createDeleteRights(newGlyphImg)); //probably should give pure delete rights
				return newGlyphImg;
			}
		}
		else
		{
			if(fci.width == glyphImage->getWidth() && fci.height == glyphImage->getHeight())
			{
				return glyphImage;
			}
			else
			{
				//Should be resized
				//create new image and add to cachedGlyphs
				VectorGraphic* vecGlyph = fontSprite.getGraphic(fci.reserved);

				if(vecGlyph != nullptr)
				{
					//can be added.
					Image* newGlyphImg = new Image(fci.width, fci.height);

					Mat3f oldTransform = vecGlyph->getTransform();
					vecGlyph->setTransform(MathExt::scale2D(scaleValue, scaleValue)*oldTransform);
					vecGlyph->draw(newGlyphImg);
					vecGlyph->setTransform(oldTransform);

					uint32_t tempReservedData = fci.reserved;
					cachedGlyphs.addData(std::move(tempReservedData), SmartMemory<Image>::createDeleteRights(newGlyphImg)); //probably should give pure delete rights
					return newGlyphImg;
				}
			}

		}

		return nullptr;
	}

	bool VectorFont::load(File file)
	{
		if(file.getExtension() == ".svg")
		{
			return loadSVGFont(file);
		}
		else if(file.getExtension() == ".ttf" || file.getExtension() == ".otf")
		{
			return loadOTFFont(file);
		}
		return false;
	}

	bool VectorFont::loadSVGFont(File file)
	{
		SimpleXml f = SimpleXml();
		if(!f.load(file))
		{
			return false;
		}

		//look for "svg/defs/font"
		std::vector<XmlNode*> rootNodeList = f.getNodesPattern({"svg", "defs", "font"});
		if(rootNodeList.size() <= 0)
			return false;
		
		XmlNode* rootNode = rootNodeList.front();
		auto temp = rootNode->getAttribute("horiz-adv-x");
		if(temp != rootNode->getRawAttributes().end())
			baseHorizontalAdvance = StringTools::toInt(temp->second);
		
		XmlNode parentNode = XmlNode();
		parentNode.setTitle("svg");
		
		std::string transformValue = "scale(1,-1)";
		parentNode.addAttribute("width", "0");
		parentNode.addAttribute("height", "0");

		originalFontSize = 1000;
		verticalAdv = 1000;

		Mat3f viewBoxMat = Mat3f::getIdentity();
		Vec2f viewBoxOffset = Vec2f(0, 0);

		auto childNodes = rootNode->getChildNodes();
		
		auto fontFaceNode = rootNode->getNodesPattern({"font-face"});
		if(!fontFaceNode.empty())
		{
			auto temp = fontFaceNode.front()->getAttribute("bbox");
			if(temp != fontFaceNode.front()->getRawAttributes().end())
			{
				//not only does transformValue need to flip y but it also needs to offset by enough to put it in the valid locations
				std::vector<std::string> split = StringTools::splitStringMultipleDeliminators(temp->second, " ,");
				
				if(split.size() == 4)
				{
					int minX = (int)StringTools::toInt(split[0]);
					int minY = (int)StringTools::toInt(split[1]);
					int tempWidth = (int)StringTools::toInt(split[2]);
					int tempHeight = (int)StringTools::toInt(split[3]);
					
					viewBoxOffset = Vec2f(minX, minY);
					viewBoxMat = MathExt::translation2D(minX, minY);

					int dis = tempHeight - minY;
					transformValue = "translate(0," + StringTools::toString(dis) + ") scale(1,-1)";

					originalFontSize = dis; //the font size is the height of the font. These aren't a closely fit bounding box though
					verticalAdv = dis; //same thing. Just the height of the font
				}
			}

			temp = fontFaceNode.front()->getAttribute("units-per-em");
			if(temp != fontFaceNode.front()->getRawAttributes().end())
			{
				originalFontSize = (int)StringTools::toInt(temp->second);
				verticalAdv = originalFontSize;
				transformValue = "translate(0," + StringTools::toString(originalFontSize) + ") scale(1,-1)";
			}
		}
		
		//process font information
		for(int i=0; i<childNodes.size(); i++)
		{
			if(childNodes[i].type != ChildNode::TYPE_NODE)
				continue;
			
			XmlNode* fontChild = childNodes[i].node;
			if(StringTools::equalsIgnoreCase<char>(fontChild->getTitle(), "glyph") || StringTools::equalsIgnoreCase<char>(fontChild->getTitle(), "missing-glyph"))
			{
				FontCharInfo fc = {};
				fc.x=0;
				fc.y=0;
				fc.xOffset = 0;
				fc.yOffset = 0;
				fc.width = 0;
				fc.height = 0;
				fc.horizAdv = baseHorizontalAdvance;
				fc.unicodeValue = -1;   //assuming missing-glyph

				temp = fontChild->getAttribute("unicode");
				if(temp != fontChild->getRawAttributes().end())
				{
					auto stuff = StringTools::utf8ToIntString(temp->second);
					if(stuff.size() > 0)
						fc.unicodeValue = stuff.front();
				}

				temp = fontChild->getAttribute("horiz-adv-x");
				if(temp != fontChild->getRawAttributes().end())
				{
					fc.horizAdv = (int)MathExt::ceil( stod(temp->second) );
				}

				//turn the xmlNode into a path and pass it into a vectorgrahpic
				VectorGraphic* fontGraphic = new VectorGraphic();
				
				XmlNode parsedNode = XmlNode(*fontChild);
				parsedNode.setTitle("path");
				parsedNode.addAttribute("transform", transformValue);
				parsedNode.addAttribute("fill", "white");

				auto m = parsedNode.getAttribute("d");

				parentNode.addChild(&parsedNode);

				fontGraphic->load(&parentNode);

				std::pair<Vec2f, Vec2f> boundingPoints = fontGraphic->getBoundingBox();
				Vec2f diff = boundingPoints.second - boundingPoints.first;
				
				//set width and height to be the bounding box difference but also add a translate by -x and -y
				fontGraphic->setWidth(ceil(abs(diff.x)));
				fontGraphic->setHeight(ceil(abs(diff.y)));

				if(fc.unicodeValue == 'A')
				{
					StringTools::println("bounding box = %f", boundingPoints);
					StringTools::println("bounding box = %f, %f", boundingPoints.first-viewBoxOffset, boundingPoints.second-viewBoxOffset);
				}

				//generate a new transform
				Mat3f adjustedTransform = MathExt::translation2D(boundingPoints.first-viewBoxOffset) * fontGraphic->getTransform();
				fontGraphic->setTransform(adjustedTransform);
				fc.xOffset = boundingPoints.first.x;
				fc.yOffset = boundingPoints.first.y;
				fc.width = fontGraphic->getWidth();
				fc.height = fontGraphic->getHeight();

				fontGraphic->setViewBox(viewBoxMat);

				parentNode.getChildNodes().pop_back(); // Why this way??? Add a remove Node to avoid this or something

				fc.reserved = this->fontSprite.getSize();
				this->addChar(fc);
				this->fontSprite.addGraphic(fontGraphic);
			}
		}

		return true;
	}

} //NAMESPACE glib END