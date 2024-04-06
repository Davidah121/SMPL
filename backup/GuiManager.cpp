#include "InternalGuiManagerHeader.h"

namespace smpl
{
	#pragma region GUI_MANAGER
	std::unordered_map<std::string, std::function<GuiInstance*(SimpleHashMap<std::string, std::string>&)> > GuiManager::elementLoadingFunctions;

	const Class GuiManager::globalClass = Class("GuiManager", {&Object::globalClass});

	void GuiManager::initDefaultLoadFunctions()
	{
		GuiRectangleButton::registerLoadFunction();
		GuiTextBlock::registerLoadFunction();
		GuiTextBox::registerLoadFunction();
		GuiSprite::registerLoadFunction();
		GuiContainer::registerLoadFunction();
		GuiList::registerLoadFunction();
		GuiGrid::registerLoadFunction();
		GuiContextMenu::registerLoadFunction();
		GuiDatePicker::registerLoadFunction();
	}

	void GuiManager::registerLoadFunction(std::string className, std::function<GuiInstance*(SimpleHashMap<std::string, std::string>&)> func)
	{
		elementLoadingFunctions[className] = func;
	}

	bool GuiManager::loadElement(XmlNode* node, GuiInstance* parent)
	{
		if(node == nullptr)
			return false;

		if(StringTools::equalsIgnoreCase<char>(node->getTitle(), "SpriteResource"))
		{
			//separate processing
			HashPair<std::string, std::string>* idPair = node->getAttribute("id");
			HashPair<std::string, std::string>* srcPair = node->getAttribute("src");
			if(idPair != nullptr && srcPair != nullptr )
			{
				//can probably load stuff
				if(!idPair->data.empty() && !srcPair->data.empty())
				{
					//Unless the file is incorrect, should be in the resource list something
					GuiResourceManager::getResourceManager().addSprite(GraphicsInterface::createSprite(srcPair->data, graphicsInterfaceMode), idPair->data, false);
					return true;
				}
			}
			StringTools::println("ERROR LOADING SPRITE RESOURCE: No 'src' or 'id' specified");
			return false;
		}
		else if(StringTools::equalsIgnoreCase<char>(node->getTitle(), "FontResource"))
		{
			//separate processing
			HashPair<std::string, std::string>* idPair = node->getAttribute("id");
			HashPair<std::string, std::string>* srcPair = node->getAttribute("src");
			if(idPair != nullptr && srcPair != nullptr )
			{
				//can probably load stuff
				if(!idPair->data.empty() && !srcPair->data.empty())
				{
					//Unless the file is incorrect, should be in the resource list something
					GuiResourceManager::getResourceManager().addFont(GraphicsInterface::createFont(srcPair->data, graphicsInterfaceMode), idPair->data, false);
					return true;
				}
			}
			StringTools::println("ERROR LOADING FONT RESOURCE: No 'src' or 'id' specified");
			return false;
		}
		else
		{
			GuiInstance* thisIns = nullptr;

			auto it = elementLoadingFunctions.find(node->getTitle());
			if(it != elementLoadingFunctions.end())
			{
				thisIns = it->second(node->getRawAttributes()); //call load function for specific instance
				
				if(parent != nullptr)
					parent->addChild(thisIns);
			}

			if(thisIns == nullptr)
				return false;
			
			//add to list
			addElement(thisIns);
			addToDeleteList(thisIns);

			for(ChildNode& c : node->getChildNodes())
			{
				if(c.type != ChildNode::TYPE_NODE)
					continue;

				bool successful = loadElement(c.node, thisIns);
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
				int w = StringTools::toInt(wAttrib->data);
				int h = StringTools::toInt(hAttrib->data);
				resizeImage(w, h);
				setExpectedSize( Vec2f(w,h) );
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
				int w = StringTools::toInt(wAttrib->data);
				int h = StringTools::toInt(hAttrib->data);
				resizeImage(w, h);
				setExpectedSize( Vec2f(w,h) );
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
		setClass(globalClass);
		GraphicsInterface::setDefaultType(type);
		surf = GraphicsInterface::createSurface(320, 240);
		expectedSize = Vec2f(320, 240);

		// surf.setAllPixels(backgroundColor); //Clear to background color
	}

	GuiManager::GuiManager(unsigned char type, int width, int height)
	{
		setClass(globalClass);
		GraphicsInterface::setDefaultType(type);
		surf = GraphicsInterface::createSurface(width, height);
		expectedSize = Vec2f(width, height);

		// surf.setAllPixels(backgroundColor); //Clear to background color
	}

	GuiManager::~GuiManager()
	{
		disposeObjects();

		if(surf != nullptr)
			delete surf;
		surf = nullptr;
	}

	void GuiManager::disposeObjects()
	{
		for(GuiInstance* k : objects)
		{
			deleteElementDelayed(k);
		}

		//Remove the remaining objects allocated that for some reason did not get deleted
		for(auto it=shouldDelete.begin(); it != shouldDelete.end(); it++)
		{
			GuiInstance* pointer = *it;

			pointer->setManager(nullptr); //Prevent circular delete.
			if(pointer != nullptr)
				delete pointer;
			
		}
		
		objects.clear();
		objectsByName.clear();
		shouldDelete.clear();
	}

	void GuiManager::addElement(GuiInstance* k)
	{
		if(k!=nullptr)
		{
			if(k->getManager() == nullptr)
			{
				k->setManager(this);
				objectsByName.add( k->nameID, k);
				for(size_t i=0; i<k->getChildren().size(); i++)
				{
					GuiInstance* c = k->getChildren().at(i);
					if(c->getManager() == nullptr)
					{
						addElement(c);
					}
				}

				if(k->parent == nullptr)
				{
					objects.push_back(k);
				}
			}
			
		}
	}
	
	void GuiManager::addToDeleteList(GuiInstance* k)
	{
		shouldDelete.insert(k);
	}

	void GuiManager::deleteElement(GuiInstance* k)
	{
		if(k == nullptr)
			return;

		if(k->manager == this)
		{
			//Even if the element is not a root element, it is possible for the object
			//to be in the root object list.
			int startIndex = -1;
			int orgSize = getSize();

			for (int i = 0; i < orgSize; i++)
			{
				if (objects[i] == k)
				{
					startIndex = i;
					break;
				}
			}

			if (startIndex >= 0)
			{
				for (int i = startIndex; i < orgSize - 1; i++)
				{
					objects[i] = objects[i + 1];
				}
				objects.pop_back();
			}

			k->manager = nullptr;
			objectsByName.remove(k->nameID, k);
			addToRemovedObjectBox(k);

			//delete children as well
			for(GuiInstance* o : k->getChildren())
			{
				deleteElement(o);
			}

			auto it = shouldDelete.find(k);
			if(it != shouldDelete.end())
			{
				shouldDelete.erase(k);
				delete k;
			}
			
		}
	}

	void GuiManager::deleteElementDelayed(GuiInstance* k)
	{
		if(k == nullptr)
			return;

		if(k->manager == this)
		{
			k->manager = nullptr;
			objectsByName.remove(k->nameID, k);
			addToRemovedObjectBox(k);

			//delete children as well
			for(GuiInstance* o : k->getChildren())
			{
				deleteElementDelayed(o);
			}
		}
	}

	void GuiManager::addToRemovedObjectBox(GuiInstance* k)
	{
		if(k!=nullptr)
		{
			deletedObjectsBox.setLeftBound( MathExt::min(deletedObjectsBox.getLeftBound(), k->previousBoundingBox.getLeftBound()) );
			deletedObjectsBox.setRightBound( MathExt::max(deletedObjectsBox.getRightBound(), k->previousBoundingBox.getRightBound()) );
			deletedObjectsBox.setTopBound( MathExt::min(deletedObjectsBox.getTopBound(), k->previousBoundingBox.getTopBound()) );
			deletedObjectsBox.setBottomBound( MathExt::max(deletedObjectsBox.getBottomBound(), k->previousBoundingBox.getBottomBound()) );
		}
	}

	void GuiManager::fixObjects()
	{
		//Only keep the root elements
		std::vector<GuiInstance*> rootElements;
		
		for(GuiInstance* objs : objects)
		{
			if(objs->getParent() == nullptr)
			{
				//Do not add duplicate
				bool canAdd = true;
				for(size_t i=0; i<rootElements.size(); i++)
				{
					if(objs == rootElements[i])
					{
						canAdd = false;
						break;
					}
				}
				if(canAdd)
					rootElements.push_back( objs );
			}
			else
			{
				GuiInstance* temp = objs->getParent();
				while(true)
				{
					if(temp->getParent() == nullptr)
						break;
					else
						temp = temp->getParent();
				}

				//Do not add duplicate
				bool canAdd = true;
				for(size_t i=0; i<rootElements.size(); i++)
				{
					if(temp == rootElements[i])
					{
						canAdd = false;
						break;
					}
				}
				if(canAdd)
					rootElements.push_back( temp );
			}
		}

		objects = rootElements;
	}

	void GuiManager::updateGuiElements()
	{
		//Assume that pollInput() was already called
		//update root elements
		fixObjects();
		for(size_t i=0; i<objects.size(); i++)
		{
			updateElement( objects[i] );
		}
	}

	void GuiManager::updateElement(GuiInstance* k)
	{
		if(k != nullptr)
		{
			if(k->getActive())
			{
				bool v = k->getPriority() & 0b10;
				if(v)
				{
					//update self then children
					k->baseUpdate();
					k->update();

					for(GuiInstance* childObj : k->getChildrenRef())
					{
						updateElement(childObj);
					}
				}
				else
				{
					//update children then self
					for(GuiInstance* childObj : k->getChildrenRef())
					{
						updateElement(childObj);
					}

					k->baseUpdate();
					k->update();
				}
			}
		}
	}

	void GuiManager::renderElement(GuiInstance* k, int& redrawCount)
	{
		if(k != nullptr)
		{
			//moved out of the getVisible section so that all possible objects are still updated.
			bool v = k->getPriority() & 0b01;
			
			if(!v)
			{
				//render children first
				for(GuiInstance* childObj : k->getChildrenRef())
				{
					renderElement(childObj, redrawCount);
				}
			}

			if(k->getVisible())
			{
				//render self
				bool validNewBox = false;
				bool validPreBox = false;
				bool clip1 = false;
				bool clip2 = false;

				if(newClipBox.getLeftBound() <= newClipBox.getRightBound()
				&& newClipBox.getTopBound() <= newClipBox.getBottomBound())
				{
					validNewBox = true;
				}

				if(preClipBox.getLeftBound() <= preClipBox.getRightBound()
				&& preClipBox.getTopBound() <= preClipBox.getBottomBound())
				{
					validPreBox = true;
				}

				if(k->getStaticScaling())
				{
					GraphicsInterface::enableScaling(false);
				}

				if(!this->invalidImage)
				{
					if(validNewBox)
					{
						if(CollisionMaster::collisionMethod(&newClipBox, &k->boundingBox))
						{
							k->shouldRedraw = true;
							clip1 = true;
							GraphicsInterface::setClippingRect(newClipBox);
						}
					}

					if(validPreBox)
					{
						if(CollisionMaster::collisionMethod(&preClipBox, &k->boundingBox))
						{
							k->shouldRedraw = true;
							clip2 = true;
							GraphicsInterface::setClippingRect(preClipBox);
						}
					}
					
					if(!clip1 && !clip2)
					{
						k->shouldRedraw = false;
					}
				}
				else
				{
					k->shouldRedraw = true;
				}

				if(k->shouldRedraw)
				{
					k->baseRender();
					if(k->getCanvas() == nullptr)
						GraphicsInterface::setBoundSurface(surf);
					else
						GraphicsInterface::setBoundSurface(k->getCanvas());
					
					if(!clip1 && !clip2)
					{
						k->render();
					}
					else
					{
						if(clip1 && !clip2)
						{
							GraphicsInterface::setClippingRect(newClipBox);
							k->render();
						}
						else if(clip2 && !clip1)
						{
							GraphicsInterface::setClippingRect(preClipBox);
							k->render();
						}
						else if(clip1 && clip2)
						{
							//check if either box is completely inside of the other.
							//If so, do not use that box.
							bool box1Col = false;
							bool box2Col = false;

							//Check for overlap. If completely inside, this will be true for preClipBox but if newClipBox is inside preClipBox, it will be false.
							//assumes that leftBound<rightBound and topBound<bottomBound
							if(preClipBox.getLeftBound() >= newClipBox.getLeftBound() && preClipBox.getRightBound() <= newClipBox.getRightBound())
							{
								if(preClipBox.getTopBound() >= newClipBox.getTopBound() && preClipBox.getBottomBound() <= newClipBox.getBottomBound())
								{
									box1Col = true;
								}
							}

							//Check for overlap. If completely inside, this will be true for newClipBox but if preClipBox is inside newClipBox, it will be false.
							if(newClipBox.getLeftBound() >= preClipBox.getLeftBound() && newClipBox.getRightBound() <= preClipBox.getRightBound())
							{
								if(newClipBox.getTopBound() >= preClipBox.getTopBound() && newClipBox.getBottomBound() <= preClipBox.getBottomBound())
								{
									box2Col = true;
								}
							}

							if(!box1Col && !box2Col)
							{
								//neither box fully encapsulates the other.
								//Use both boxes
								GraphicsInterface::setClippingRect(newClipBox);
								k->render();
								GraphicsInterface::setClippingRect(preClipBox);
								k->render();
							}
							else if(box1Col && !box2Col)
							{
								//previousBox is in new box. Only do new box
								GraphicsInterface::setClippingRect(newClipBox);
								k->render();
							}
							else
							{
								//Possible that both are the same
								//newBox is in the previousBox. Only do previous box
								GraphicsInterface::setClippingRect(preClipBox);
								k->render();
							}
						}
					}

					redrawCount++;
					k->previousBoundingBox = k->boundingBox;
				}
			}

			GraphicsInterface::enableScaling(true);

			if(v)
			{
				//render children last
				for(GuiInstance* childObj : k->getChildrenRef())
				{
					renderElement(childObj, redrawCount);
				}
			}
			
			//set things like if it should redraw. This will apply regardless of whether the object is visible or not.
			//If the object is not visible, it just means it should be cleared.
			k->shouldRedraw = false;
		}
	}

	void GuiManager::updateBounds(GuiInstance* obj, int& redrawCount)
	{
		//calculate the bounding render area

		if( (obj->shouldRedraw || this->invalidImage) )
		{
			newClipBox.setLeftBound( MathExt::min(newClipBox.getLeftBound(), obj->boundingBox.getLeftBound()) );
			newClipBox.setRightBound( MathExt::max(newClipBox.getRightBound(), obj->boundingBox.getRightBound()) );
			newClipBox.setTopBound( MathExt::min(newClipBox.getTopBound(), obj->boundingBox.getTopBound()) );
			newClipBox.setBottomBound( MathExt::max(newClipBox.getBottomBound(), obj->boundingBox.getBottomBound()) );
			
			preClipBox.setLeftBound( MathExt::min(preClipBox.getLeftBound(), obj->previousBoundingBox.getLeftBound()) );
			preClipBox.setRightBound( MathExt::max(preClipBox.getRightBound(), obj->previousBoundingBox.getRightBound()) );
			preClipBox.setTopBound( MathExt::min(preClipBox.getTopBound(), obj->previousBoundingBox.getTopBound()) );
			preClipBox.setBottomBound( MathExt::max(preClipBox.getBottomBound(), obj->previousBoundingBox.getBottomBound()) );
			
			redrawCount++;
			obj->setShouldRedraw(true);
		}

		for(GuiInstance* childObj : obj->getChildrenRef())
		{
			if(obj->shouldRedraw)
				childObj->setShouldRedraw(true);
			
			updateBounds(childObj, redrawCount);
		}
	}

	bool GuiManager::renderGuiElements()
	{
		fixObjects();
		newClipBox = GuiInstance::getInvalidBox();
		preClipBox = GuiInstance::getInvalidBox();

		int shouldRedrawCount = 0;
		int redrawCount = 0;

		if(alwaysInvalidate)
		{
			invalidImage = true;
		}
		
		for(GuiInstance* obj : objects)
		{
			updateBounds(obj, shouldRedrawCount);
		}

		//adjust previous clip box by all of the removed objects.
		preClipBox.setLeftBound( MathExt::min(preClipBox.getLeftBound(), deletedObjectsBox.getLeftBound()) );
		preClipBox.setRightBound( MathExt::max(preClipBox.getRightBound(), deletedObjectsBox.getRightBound()) );
		preClipBox.setTopBound( MathExt::min(preClipBox.getTopBound(), deletedObjectsBox.getTopBound()) );
		preClipBox.setBottomBound( MathExt::max(preClipBox.getBottomBound(), deletedObjectsBox.getBottomBound()) );
		
		int width = surf->getWidth();
		int height = surf->getHeight();
		
		GraphicsInterface::setOrthoProjection(width, height);
		GraphicsInterface::resetClippingPlane();
		GraphicsInterface::setColor(backgroundColor);
		GraphicsInterface::setBoundSurface(surf);
		
		GraphicsInterface::setScalingFactor( Vec2f((double)width/expectedSize.x, (double)height/expectedSize.y) );

		if(shouldRedrawCount > 0)
		{
			// StringTools::println("REDRAW: %d", redrawCount);
			// StringTools::println("NewBox: (%.3f, %.3f , %.3f, %.3f)", newClipBox.getLeftBound(), newClipBox.getTopBound(), newClipBox.getRightBound(), newClipBox.getBottomBound());
			// StringTools::println("PreBox: (%.3f, %.3f , %.3f, %.3f)", preClipBox.getLeftBound(), preClipBox.getTopBound(), preClipBox.getRightBound(), preClipBox.getBottomBound());
			
			//Add to redrawCount since you are clearing stuff.
			redrawCount++;
			if(invalidImage)
			{
				GraphicsInterface::clear();
			}
			else
			{
				if(newClipBox.getLeftBound() <= 0 && newClipBox.getRightBound() >= surf->getWidth() 
					&& newClipBox.getTopBound() <= 0 && newClipBox.getBottomBound() >= surf->getHeight())
				{
					GraphicsInterface::clear(); //Clear everything
				}
				else if(preClipBox.getLeftBound() <= 0 && preClipBox.getRightBound() >= surf->getWidth() 
						&& preClipBox.getTopBound() <= 0 && preClipBox.getBottomBound() >= surf->getHeight())
				{
					GraphicsInterface::clear(); //Clear everything
				}
				else
				{
					//Clear the invalid areas

					//check if valid box
					if(newClipBox.getLeftBound() <= newClipBox.getRightBound()
					&& newClipBox.getTopBound() <= newClipBox.getBottomBound())
					{
						GraphicsInterface::drawRect((int)newClipBox.getLeftBound(), (int)newClipBox.getTopBound(), (int)newClipBox.getRightBound(), (int)newClipBox.getBottomBound(), false);
					}
					
					//check if valid box
					if(preClipBox.getLeftBound() <= preClipBox.getRightBound()
					&& preClipBox.getTopBound() <= preClipBox.getBottomBound())
					{
						GraphicsInterface::drawRect((int)preClipBox.getLeftBound(), (int)preClipBox.getTopBound(), (int)preClipBox.getRightBound(), (int)preClipBox.getBottomBound(), false);
					}
				}
			}

		
			for(GuiInstance* obj : objects)
			{
				renderElement(obj, redrawCount);
			}
			
		}
		else
		{
			if(invalidImage)
			{
				GraphicsInterface::setColor(backgroundColor);
				GraphicsInterface::setBoundSurface(surf);
				GraphicsInterface::clear();
				redrawCount++;
			}
			else
			{
				//If the object was removed from the list, it may not have caused other objects to redraw but needs to clear its area.
				//The box will have valid bounds in that case. (Note that it could still have bounds outside of the render area but it won't matter)
				if(deletedObjectsBox.getLeftBound() <= deletedObjectsBox.getRightBound()
				&& deletedObjectsBox.getTopBound() <= deletedObjectsBox.getBottomBound())
				{
					GraphicsInterface::setColor(backgroundColor);
					GraphicsInterface::setBoundSurface(surf);
					GraphicsInterface::drawRect((int)deletedObjectsBox.getLeftBound(), (int)deletedObjectsBox.getTopBound(), (int)deletedObjectsBox.getRightBound(), (int)deletedObjectsBox.getBottomBound(), false);
					redrawCount++;
				}
			}
		}

		GraphicsInterface::resetClippingPlane();
		invalidImage = false;
		deletedObjectsBox = GuiInstance::getInvalidBox();

		if(redrawCount != 0)
		{
			GraphicsInterface::setBoundSurface(surf);
			GraphicsInterface::setColor(Vec4f(1,1,1,1));
			GraphicsInterface::drawToScreen();
			// StringTools::println("RedrawCount: %d", redrawCount);
		}
		
		return redrawCount != 0;
	}

	std::vector<GuiInstance*>& GuiManager::getElements()
	{
		return objects;
	}
	
	std::vector< HashPair<std::string, GuiInstance*>* > GuiManager::getInstancesByName(std::string name)
	{
		return objectsByName.getAll(name);
	}

	SurfaceInterface* GuiManager::getSurface()
	{
		return surf;
	}

	size_t GuiManager::getSize()
	{
		return objects.size();
	}

	void GuiManager::resizeImage(int width, int height)
	{
		bool wasBound = (surf == GraphicsInterface::getBoundSurface());
		
		if(surf->getWidth() != width || surf->getHeight() != height)
		{
			if(surf != nullptr)
			{
				delete surf;
			}
			surf = GraphicsInterface::createSurface(width, height);
		}

		invalidImage = true;
		if(wasBound)
			GraphicsInterface::setBoundSurface(surf);
	}

	void GuiManager::invalidateImage()
	{
		invalidImage = true;
	}
	
	void GuiManager::alwaysInvalidateImage(bool v)
	{
		alwaysInvalidate = v;
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
		double temp = 0;
		double scaling = 1;

		temp = (double)(Input::getMouseX() - getWindowX());
		if(surf != nullptr)
			scaling = expectedSize.x / surf->getWidth();

		return (int)(temp*scaling);
	}
	
	int GuiManager::getMouseY()
	{
		double temp = 0;
		double scaling = 1;
		temp = (double)(Input::getMouseY() - getWindowY());
		if(surf != nullptr)
			scaling = expectedSize.y / surf->getHeight();

		return (int)(temp*scaling);
	}

	bool GuiManager::getFocus()
	{
		return isInFocus;
	}

	void GuiManager::setFocus(bool v)
	{
		isInFocus = v;
	}

	Color GuiManager::getBackgroundColor()
	{
		return backgroundColor;
	}

	void GuiManager::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}
	
	void GuiManager::setExpectedSize(Vec2f s)
	{
		expectedSize = s;
	}

	Vec2f GuiManager::getExpectedSize()
	{
		return expectedSize;
	}
	
	void GuiManager::setGraphicsInterfaceMode(int mode)
	{
		graphicsInterfaceMode = mode;
	}

	int GuiManager::getGraphicsInterfaceMode()
	{
		return graphicsInterfaceMode;
	}

	#pragma endregion

} //NAMESPACE smpl END