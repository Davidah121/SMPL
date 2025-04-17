#include "NewGui.h"
#include "Input.h"
#include "ResourceManager.h"
#include "System.h"

namespace smpl
{
    #pragma region GUI_MANAGER
	std::unordered_map<std::string, std::function<SmartMemory<GuiItem>(SimpleHashMap<std::string, std::string>&, SmartMemory<GuiManager>)> > GuiManager::elementLoadingFunctions;
	
	void GuiManager::initDefaultLoadFunctions()
	{
		GuiManager::registerLoadFunction("Empty", GuiEmpty::loadFunction);
		GuiManager::registerLoadFunction("Text", GuiText::loadFunction);
		GuiManager::registerLoadFunction("TextBox", GuiTextBox::loadFunction);
		GuiManager::registerLoadFunction("Sprite", GuiSprite::loadFunction);
		GuiManager::registerLoadFunction("Button", GuiButton::loadFunction);
		
		GuiManager::registerLoadFunction("LayoutFixed", GuiLayoutFixed::loadFunction);
		GuiManager::registerLoadFunction("LayoutList", GuiLayoutList::loadFunction);
		GuiManager::registerLoadFunction("LayoutTable", GuiLayoutTable::loadFunction);
		GuiManager::registerLoadFunction("ScrollBar", GuiScrollBar::loadFunction);
		// GuiRectangleButton::registerLoadFunction();
		// GuiTextBlock::registerLoadFunction();
		// GuiTextBox::registerLoadFunction();
		// GuiSprite::registerLoadFunction();
		// GuiContainer::registerLoadFunction();
		// GuiList::registerLoadFunction();
		// GuiGrid::registerLoadFunction();
		// GuiContextMenu::registerLoadFunction();
		// GuiDatePicker::registerLoadFunction();
	}

	void GuiManager::registerLoadFunction(std::string className, std::function<SmartMemory<GuiItem>(SimpleHashMap<std::string, std::string>&, SmartMemory<GuiManager>)> func)
	{
		elementLoadingFunctions[ StringTools::toLowercase(className) ] = func;
	}

	bool GuiManager::loadElement(XmlNode* node, SmartMemory<GuiItem> parent)
	{
		if(node == nullptr)
			return false;

		if(StringTools::equalsIgnoreCase<char>(node->getTitle(), "SpriteResource"))
		{
			//separate processing
			std::pair<std::string, std::string>* idPair = node->getAttribute("id");
			std::pair<std::string, std::string>* srcPair = node->getAttribute("src");
			if(idPair != nullptr && srcPair != nullptr )
			{
				//can probably load stuff
				if(!idPair->second.empty() && !srcPair->second.empty())
				{
					//Unless the file is incorrect, should be in the resource list something
					GuiResourceManager::getResourceManager().addSprite(GraphicsInterface::createSprite(srcPair->second), idPair->second, 1, false);
					return true;
				}
			}
			StringTools::println("ERROR LOADING SPRITE RESOURCE: No 'src' or 'id' specified");
			return false;
		}
		else if(StringTools::equalsIgnoreCase<char>(node->getTitle(), "FontResource"))
		{
			//separate processing
			std::pair<std::string, std::string>* idPair = node->getAttribute("id");
			std::pair<std::string, std::string>* srcPair = node->getAttribute("src");
			if(idPair != nullptr && srcPair != nullptr )
			{
				//can probably load stuff
				if(!idPair->second.empty() && !srcPair->second.empty())
				{
					//Unless the file is incorrect, should be in the resource list something
					GuiResourceManager::getResourceManager().addFont(GraphicsInterface::createFont(srcPair->second), idPair->second, 1, false);
					return true;
				}
			}
			StringTools::println("ERROR LOADING FONT RESOURCE: No 'src' or 'id' specified");
			return false;
		}
		else
		{
			SmartMemory<GuiItem> newIns = nullptr;

			auto it = elementLoadingFunctions.find( node->getTitle() );
			if(it != elementLoadingFunctions.end())
			{
				newIns = it->second(node->getRawAttributes(), SmartMemory<GuiManager>::createNoDelete(this)); //call load function for specific instance
			}

			if(newIns == nullptr)
				return false;
			
			//Add to parent if possible. If not possible to add as a child, add to root layout
			if(parent.getPointer() != nullptr)
			{
				if(parent.getRawPointer()->getType() == GuiItem::TYPE_LAYOUT)
				{
					((GuiLayout*)parent.getRawPointer())->addChild(newIns);
				}
				else
				{
					//add to root layout
					addElement(newIns);
				}
			}
			else
			{
				//add to root layout
				addElement(newIns);
			}
			
			setID(newIns, newIns.getRawPointer()->getNameID());
			addToDisposeList(newIns);

			for(ChildNode& c : node->getChildNodes())
			{
				if(c.type != ChildNode::TYPE_NODE)
					continue;
				
				bool successful = loadElement(c.node, newIns);
				if(!successful)
				{
					StringTools::println("ERROR LOADING NODE: %ls", c.node->getTitle().c_str());
					return false;
				}
			}

			return true;
		}
		
	}

	void GuiManager::loadElementsFromFile(File f)
	{
		SimpleXml xmlFile = SimpleXml(f);
		
		//Everything must be encapsulated in the tag <SimpleGUI> or something
		XmlNode* parentNode = nullptr;

		for(XmlNode* n : xmlFile.getNodes())
		{
			if(StringTools::equalsIgnoreCase<char>(n->getTitle(), "SimpleGUI"))
			{
				parentNode = n;
				break;
			}
		}

		if(parentNode != nullptr)
		{
			auto wAttrib = parentNode->getAttribute("width");
			auto hAttrib = parentNode->getAttribute("height");

			if(wAttrib != nullptr && hAttrib != nullptr)
			{
				int w = StringTools::toInt(wAttrib->second);
				int h = StringTools::toInt(hAttrib->second);
				resizeImage(w, h);
			}

			for(ChildNode& c : parentNode->getChildNodes())
			{
				if(c.type != ChildNode::TYPE_NODE)
					continue;
				
				bool successful = loadElement(c.node, nullptr);
				if(!successful)
				{
					StringTools::println("ERROR LOADING NODE: %ls", c.node->getTitle().c_str());
				}
			}
		}
		else
		{
			StringTools::println("Expected Root Node SimpleGUI was not found.");
		}
	}

	void GuiManager::loadElementsFromXML(SimpleXml& xmlFile)
	{
		//Everything must be encapsulated in the tag <SimpleGUI> or something
		XmlNode* parentNode = nullptr;

		for(XmlNode* n : xmlFile.getNodes())
		{
			if(StringTools::equalsIgnoreCase<char>(n->getTitle(), "SimpleGUI"))
			{
				parentNode = n;
				break;
			}
		}

		if(parentNode != nullptr)
		{
			auto wAttrib = parentNode->getAttribute("width");
			auto hAttrib = parentNode->getAttribute("height");

			if(wAttrib != nullptr && hAttrib != nullptr)
			{
				int w = StringTools::toInt(wAttrib->second);
				int h = StringTools::toInt(hAttrib->second);
				resizeImage(w, h);
			}
			
			for(ChildNode& c : parentNode->getChildNodes())
			{
				if(c.type != ChildNode::TYPE_NODE)
					continue;
				
				XmlNode* n = c.node;
				bool successful = loadElement(n, nullptr);
				if(!successful)
				{
					StringTools::println("ERROR LOADING NODE: %ls", n->getTitle().c_str());
				}
			}
		}
		else
		{
			StringTools::println("Expected Root Node SimpleGUI was not found.");
		}
	}

	GuiManager::GuiManager(unsigned char type)
	{
		GraphicsInterface::setDefaultType(type);
    	GraphicsInterface::setType(type);
		surf = GraphicsInterface::createSurface(320, 240);
		expectedSize = Vec2f(320, 240);
	}

	GuiManager::GuiManager(unsigned char type, int width, int height)
	{
		GraphicsInterface::setDefaultType(type);
    	GraphicsInterface::setType(type);
		surf = GraphicsInterface::createSurface(width, height);
		expectedSize = Vec2f(width, height);
	}

	GuiManager::~GuiManager()
	{
		clear();

		if(surf != nullptr)
			delete surf;
		surf = nullptr;
	}

	void GuiManager::clear()
	{
		//Because we are using smart pointers, any object that should be deleted will be deleted when
		//the smart pointer goes out of scope. This includes when we clear the list of objects to delete.
		//removes the work we need to do.
		
		objectInFocus = nullptr;
		rootLayout = GuiLayoutFixed(); //should be okay
		resetRenderValues();
		objectsByName.clear();
		shouldDelete.clear();
	}

	void GuiManager::addElement(SmartMemory<GuiItem> k)
	{
		if(k.getPointer() != nullptr)
		{
			rootLayout.addChild(k);
		}
	}
	
	void GuiManager::removeElement(SmartMemory<GuiItem> k)
	{
		if(k.getPointer() != nullptr)
		{
			rootLayout.removeChild(k);

			//also remove from name list
			objectsByName.remove(k.getRawPointer()->nameID, k);
			if(k.getDeleteRights())
			{
				//dispose of this object.
				k.~SmartMemory();
			}
		}
	}
	
	void GuiManager::resetRenderValues()
	{
		oldRectsDrawn.clear();
		newRectsDrawn.clear();
		renderCounter = 0;
		currDepthCounter = 0;

        newDrawnArea = {INT_MAX, INT_MAX, INT_MIN, INT_MIN};
        oldDrawnArea = {INT_MAX, INT_MAX, INT_MIN, INT_MIN};
	}

	void GuiManager::updateRenderCounter()
	{
		renderCounter++;
	}

	uint32_t GuiManager::getNextDepthValue()
	{
		return currDepthCounter++;
	}

	void GuiManager::addNewDrawnArea(GRect r)
	{
		newRectsDrawn.push_back(r);
		newDrawnArea.left = MathExt::min(newDrawnArea.left, r.left);
		newDrawnArea.right = MathExt::max(newDrawnArea.right, r.right);
		newDrawnArea.top = MathExt::min(newDrawnArea.top, r.top);
		newDrawnArea.bottom = MathExt::max(newDrawnArea.bottom, r.bottom);
	}

	void GuiManager::addOldDrawnArea(GRect r)
	{
		oldRectsDrawn.push_back(r);
		oldDrawnArea.left = MathExt::min(oldDrawnArea.left, r.left);
		oldDrawnArea.right = MathExt::max(oldDrawnArea.right, r.right);
		oldDrawnArea.top = MathExt::min(oldDrawnArea.top, r.top);
		oldDrawnArea.bottom = MathExt::max(oldDrawnArea.bottom, r.bottom);
	}
	
	GRect GuiManager::getNewDrawnArea()
	{
		return newDrawnArea;
	}

	GRect GuiManager::getOldDrawnArea()
	{
		return oldDrawnArea;
	}

	std::vector<GRect>& GuiManager::getNewDrawingRects()
	{
		return newRectsDrawn;
	}

	std::vector<GRect>& GuiManager::getOldDrawingRects()
	{
		return oldRectsDrawn;
	}

	void GuiManager::updateGuiElements()
	{
		//Assume that pollInput() was already called
		//update root elements
		rootLayout.doLayoutUpdate(0, 0, surf->getWidth(), surf->getHeight());
		rootLayout.doPreUpdate();
		rootLayout.doUpdate( SmartMemory<GuiManager>::createNoDelete(this) );
	}

	void clampClipRectangle(GRect& b, int width, int height)
	{
		b.left = MathExt::clamp(b.left, 0, width);
		b.right = MathExt::clamp(b.right, 0, width);
		b.top = MathExt::clamp(b.top, 0, height);
		b.bottom = MathExt::clamp(b.bottom, 0, height);
	}

	void fixInvalidBoxes(GRect& b)
	{
		if(b.left > b.right || b.top > b.bottom) //invalid box
			b = {0, 0, 0, 0, 0};
	}
	
	bool GuiManager::renderGuiElements()
	{
    	size_t t1 = System::getCurrentTimeMicro();
		if(alwaysForceRedraw)
		{
			forceRedraw();
		}
		
		resetRenderValues();

		int width = surf->getWidth();
		int height = surf->getHeight();
		
		// GraphicsInterface::setOrthoProjection(width, height);
		GraphicsInterface::setBoundSurface(surf);
		GraphicsInterface::resetClippingPlane();

		rootLayout.doPreRender( SmartMemory<GuiManager>::createNoDelete(this) );
		clampClipRectangle(oldDrawnArea, width, height);
		clampClipRectangle(newDrawnArea, width, height);

		fixInvalidBoxes(oldDrawnArea);
		fixInvalidBoxes(newDrawnArea);
		
		if(oldDrawnArea == newDrawnArea)
			oldDrawnArea = {0, 0, 0, 0, 0};
		
		//prevent double rendering
		if(shouldForceRedraw)
		{
			oldDrawnArea = {0, 0, 0, 0, 0};
			newDrawnArea = {0, 0, 0xFFFF, 0xFFFF, 0};
		}
		
		//combine clause. If the sum of both areas is greater than if we just combined them into 1 giant box, make 1 giant box
		int minLeftCombined = MathExt::min(oldDrawnArea.left, newDrawnArea.left);
		int minTopCombined = MathExt::min(oldDrawnArea.top, newDrawnArea.top);
		int maxRightCombined = MathExt::max(oldDrawnArea.right, newDrawnArea.right);
		int maxBottomCombined = MathExt::max(oldDrawnArea.bottom, newDrawnArea.bottom);
		
		int combinedArea = (maxRightCombined - minLeftCombined) * (maxBottomCombined - minTopCombined);
		int addedSeparateArea = (oldDrawnArea.right - oldDrawnArea.left)*(oldDrawnArea.bottom - oldDrawnArea.top);
		addedSeparateArea += (newDrawnArea.right - newDrawnArea.left)*(newDrawnArea.bottom - newDrawnArea.top);

		if(combinedArea < addedSeparateArea)
		{
			newDrawnArea = {minLeftCombined, minTopCombined, maxRightCombined, maxBottomCombined, 0};
			oldDrawnArea = {0, 0, 0, 0, 0};
		}

		
		if(renderCounter > 0)
		{
			if(oldDrawnArea != GRect{0, 0, 0, 0, 0})
			{
            	// StringTools::println("\tOLD AREA - (%d, %d, %d, %d)", oldDrawnArea.left, oldDrawnArea.top, oldDrawnArea.right, oldDrawnArea.bottom);
				GraphicsInterface::setClippingRect(Box2D(oldDrawnArea.left, oldDrawnArea.top, oldDrawnArea.right, oldDrawnArea.bottom));
				GraphicsInterface::setColor(backgroundColor);
				GraphicsInterface::drawRect(oldDrawnArea.left, oldDrawnArea.top, oldDrawnArea.right, oldDrawnArea.bottom, false);
				rootLayout.doRender( SmartMemory<GuiManager>::createNoDelete(this) );
			}
			
			if(newDrawnArea != GRect{0, 0, 0, 0, 0})
			{
            	// StringTools::println("NEW AREA - (%d, %d, %d, %d)", newDrawnArea.left, newDrawnArea.top, newDrawnArea.right, newDrawnArea.bottom);
				GraphicsInterface::setClippingRect(Box2D(newDrawnArea.left, newDrawnArea.top, newDrawnArea.right, newDrawnArea.bottom));
				GraphicsInterface::setColor(backgroundColor);
				GraphicsInterface::drawRect(newDrawnArea.left, newDrawnArea.top, newDrawnArea.right, newDrawnArea.bottom, false);
				rootLayout.doRender( SmartMemory<GuiManager>::createNoDelete(this) );
			}

			rootLayout.setFinishedRendering();
		}
		
		GraphicsInterface::resetClippingPlane();
		
		GraphicsInterface::setBoundSurface(surf);
		GraphicsInterface::setColor(Vec4f(1,1,1,1));
		GraphicsInterface::drawToScreen();
		
		shouldForceRedraw = false;
		
		size_t t2 = System::getCurrentTimeMicro();
		// StringTools::println("TIME TAKEN: %llu", t2-t1);
		return true;
	}

	GuiLayoutFixed& GuiManager::getRootLayout()
	{
		return rootLayout;
	}
	
	std::vector< std::pair<std::string, SmartMemory<GuiItem>>* > GuiManager::getItemsByName(std::string name)
	{
		return objectsByName.getAll(name);
	}

	SurfaceInterface* GuiManager::getSurface()
	{
		return surf;
	}

	void GuiManager::resizeImage(int width, int height)
	{
		StringTools::println("RESIZE");
		bool wasBound = (surf == GraphicsInterface::getBoundSurface());
		
		if(surf != nullptr)
		{
			if(surf->getWidth() != width || surf->getHeight() != height)
			{
				delete surf;
				surf = GraphicsInterface::createSurface(width, height);
				forceRedraw();
			}
		}
		else
		{
			surf = GraphicsInterface::createSurface(width, height);
			forceRedraw();
		}

		if(wasBound)
			GraphicsInterface::setBoundSurface(surf);
	}

	void GuiManager::forceRedraw()
	{
		shouldForceRedraw = true;
	}

	bool GuiManager::getMustRedraw()
	{
		return shouldForceRedraw;
	}

	void GuiManager::alwaysRedraw(bool v)
	{
		alwaysForceRedraw = v;
	}

	void GuiManager::setWindowX(int v)
	{
		windowX = v;
	}

	void GuiManager::setWindowY(int v)
	{
		windowY = v;
	}

	int GuiManager::getWindowX()
	{
		return windowX;
	}

	int GuiManager::getWindowY()
	{
		return windowY;
	}

	int GuiManager::getMouseX()
	{
		return Input::getMouseX() - getWindowX();
	}
	
	int GuiManager::getMouseY()
	{
		return Input::getMouseY() - getWindowY();
	}

	bool GuiManager::getFocus()
	{
		return isInFocus;
	}

	void GuiManager::setFocus(bool v)
	{
		isInFocus = v;
	}
	
	int GuiManager::getGraphicsInterfaceMode()
	{
		return graphicsInterfaceMode;
	}

	void GuiManager::setID(SmartMemory<GuiItem> k, std::string s)
	{
		objectsByName.add(s, k);
	}

	void GuiManager::addToDisposeList(SmartMemory<GuiItem> k)
	{
		shouldDelete.push_back(SmartMemory<GuiItem>::createDeleteRights(k.getRawPointer(), 1));
	}

	void GuiManager::setObjectInFocus(SmartMemory<GuiItem> k)
	{
		objectInFocus = k;
	}

	bool GuiManager::getObjectInFocus(SmartMemory<GuiItem> k)
	{
		return objectInFocus.getPointer() == k.getPointer();
	}
	
	SmartMemory<GuiItem> GuiManager::getFocusObject()
	{
		return objectInFocus;
	}
	
    void GuiManager::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}


	#pragma endregion
};