#include "InternalGuiManagerHeader.h"

namespace glib
{
	#pragma region GUI_MANAGER
	std::unordered_map<std::wstring, std::function<GuiInstance*(std::unordered_map<std::wstring, std::wstring>&, GuiGraphicsInterface* inter)> > GuiManager::elementLoadingFunctions;

	const Class GuiManager::myClass = Class("GuiManager", {&Object::myClass});
	const Class* GuiManager::getClass()
	{
		return &GuiManager::myClass;
	}


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
	}

	void GuiManager::registerLoadFunction(std::wstring className, std::function<GuiInstance*(std::unordered_map<std::wstring, std::wstring>&, GuiGraphicsInterface* inter)> func)
	{
		elementLoadingFunctions[className] = func;
	}

	bool GuiManager::loadElement(XmlNode* node, GuiInstance* parent)
	{
		if(node != nullptr)
		{
			GuiInstance* parentObj = parent;
			GuiInstance* thisIns = nullptr;

			auto it = elementLoadingFunctions.find(node->title);
			if(it != elementLoadingFunctions.end())
			{
				std::unordered_map<std::wstring, std::wstring> map;
				for(XmlAttribute& attrib : node->attributes)
				{
					map[ StringTools::toLowercase(attrib.name) ] = attrib.value;
				}

				thisIns = it->second(map, &graphicsInterface); //call load function for specific instance
				
				if(parent != nullptr)
					parent->addChild(thisIns);
			}

			if(thisIns == nullptr)
				return false;
			
			//add to list
			addElement(thisIns);
			addToDeleteList(thisIns);
			
			
			for(XmlNode* n : node->childNodes)
			{
				bool successful = loadElement(n, thisIns);
				if(!successful)
				{
					StringTools::println("ERROR LOADING NODE: %ls", n->title.c_str());
					return false;
				}
			}

			return true;
		}

		return false;
	}

	void GuiManager::loadElementsFromFile(File f)
	{
		SimpleXml xmlFile = SimpleXml(f);
		
		//Everything must be encapsulated in the tag <SimpleGUI> or something
		XmlNode* parentNode = nullptr;

		for(XmlNode* n : xmlFile.nodes)
		{
			if(StringTools::equalsIgnoreCase<wchar_t>(n->title, L"SimpleGUI"))
			{
				parentNode = n;
				break;
			}
		}

		if(parentNode != nullptr)
		{
			for(XmlNode* n : parentNode->childNodes)
			{
				bool successful = loadElement(n, nullptr);
				if(!successful)
				{
					StringTools::println("ERROR LOADING NODE: %ls", n->title.c_str());
				}
			}
		}
	}

	GuiManager::GuiManager(unsigned char type)
	{
		graphicsInterface = GuiGraphicsInterface(type);
		surf = graphicsInterface.createSurface(320, 240);

		// surf.setAllPixels(backgroundColor); //Clear to background color
	}

	GuiManager::GuiManager(unsigned char type, int width, int height)
	{
		graphicsInterface = GuiGraphicsInterface(type);
		surf = graphicsInterface.createSurface(width, height);

		// surf.setAllPixels(backgroundColor); //Clear to background color
	}

	GuiManager::~GuiManager()
	{
		for(GuiInstance* ins : objects)
		{
			if(ins!=nullptr)
				ins->manager = nullptr;
		}

		for(auto it=shouldDelete.begin(); it != shouldDelete.end(); it++)
		{
			GuiInstance* pointer = *it;
			delete pointer;
		}
		
		objects.clear();
		objectsByName.clear();
		shouldDelete.clear();

		if(surf != nullptr)
			delete surf;
		surf = nullptr;
	}

	void GuiManager::addElement(GuiInstance* k)
	{
		if(k!=nullptr)
		{
			if(k->getManager() == nullptr)
			{
				k->setManager(this);
				objects.push_back(k);
				objectsByName.add( k->nameID, k);

				for(int i=0; i<k->getChildren().size(); i++)
				{
					GuiInstance* c = k->getChildren().at(i);
					if(c->getManager() == nullptr)
					{
						addElement(c);
					}
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
		if(k->manager == this)
		{
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

	void GuiManager::sortElements()
	{
		//assume mostly sorted
		Sort::insertionSort<GuiInstance*>(objects.data(), objects.size(), [](GuiInstance* a, GuiInstance* b)->bool{
			return a->getPriority() < b->getPriority();
		});
	}

	void GuiManager::updateGuiElements()
	{
		//Assume that pollInput() was already called
		sortElements();

		//remove objects that are not active or whose parents aren't active
		std::vector<GuiInstance*> currActiveObjects = std::vector<GuiInstance*>();
		for(int i=0; i<objects.size(); i++)
		{
			GuiInstance* obj = objects[i];

			if (obj != nullptr)
			{
				if(obj->getActive())
				{
					currActiveObjects.push_back(obj);
				}
			}
		}

		//UPDATE
		for(GuiInstance* obj : currActiveObjects)
		{
			obj->baseUpdate();
			obj->update();
		}

	}

	bool GuiManager::renderGuiElements()
	{
		std::vector<GuiInstance*> currVisibleObjects = std::vector<GuiInstance*>();

		unsigned int minX = 0xFFFFFFFF;
		unsigned int maxX = 0;
		unsigned int minY = 0xFFFFFFFF;
		unsigned int maxY = 0;

		unsigned int preMinX = 0xFFFFFFFF;
		unsigned int preMaxX = 0;
		unsigned int preMinY = 0xFFFFFFFF;
		unsigned int preMaxY = 0;

		int redrawCount = 0;

		if(alwaysInvalidate)
		{
			invalidImage = true;
		}

		for(int i=0; i<objects.size(); i++)
		{
			if (objects[i] != nullptr)
			{
				if(objects[i]->getVisible())
				{
					currVisibleObjects.push_back(objects[i]);

					if(invalidImage)
					{
						redrawCount++;
						objects[i]->shouldRedraw = true;
					}
					else
					{
						if(objects[i]->shouldRedraw)
							redrawCount++;
					}
				}
			}
		}
		int width = surf->getWidth();
		int height = surf->getHeight();
		
		graphicsInterface.setOrthoProjection(width, height);

		if(redrawCount > 0)
		{
			//calculate the bounding render area
			for(GuiInstance* obj : currVisibleObjects)
			{
				if(obj->shouldRedraw)
				{
					minX = MathExt::min((unsigned int)obj->boundingBox.getLeftBound(), minX);
					maxX = MathExt::max((unsigned int)obj->boundingBox.getRightBound(), maxX);
					minY = MathExt::min((unsigned int)obj->boundingBox.getTopBound(), minY);
					maxY = MathExt::max((unsigned int)obj->boundingBox.getBottomBound(), maxY);
					
					preMinX = MathExt::min((unsigned int)obj->previousBoundingBox.getLeftBound(), preMinX);
					preMaxX = MathExt::max((unsigned int)obj->previousBoundingBox.getRightBound(), preMaxX);
					preMinY = MathExt::min((unsigned int)obj->previousBoundingBox.getTopBound(), preMinY);
					preMaxY = MathExt::max((unsigned int)obj->previousBoundingBox.getBottomBound(), preMaxY);
				}
			}

			Box2D newClipBox = Box2D(minX, minY, maxX, maxY);
			Box2D preClipBox = Box2D(preMinX, preMinY, preMaxX, preMaxY);


			graphicsInterface.setColor(backgroundColor);
			graphicsInterface.setBoundSurface(surf);

			if(invalidImage)
			{
				graphicsInterface.clear();
				graphicsInterface.resetClippingPlane();
			}
			else
			{
				if(minX <= 0 && maxX >= surf->getWidth() && minY <= 0 && maxY >= surf->getHeight())
				{
					graphicsInterface.clear(); //Clear everything
				}
				else if(preMinX <= 0 && preMaxX >= surf->getWidth() && preMinY <= 0 && preMaxY >= surf->getHeight())
				{
					graphicsInterface.clear(); //Clear everything
				}
				else
				{
					//Clear the invalid areas
					graphicsInterface.drawRect(minX, minY, maxX, maxY, false);
					graphicsInterface.drawRect(preMinX, preMinY, preMaxX, preMaxY, false);
				}
			}

			for(GuiInstance* obj : currVisibleObjects)
			{
				if(!invalidImage)
				{
					if(CollisionMaster::collisionMethod(&newClipBox, &obj->boundingBox))
					{
						obj->shouldRedraw = true;
						graphicsInterface.setClippingRect(newClipBox);
					}
					else if(CollisionMaster::collisionMethod(&preClipBox, &obj->boundingBox))
					{
						obj->shouldRedraw = true;
						graphicsInterface.setClippingRect(preClipBox);
					}
					else
					{
						obj->shouldRedraw = false;
					}
				}

				if(obj->shouldRedraw)
				{
					obj->baseRender();
					graphicsInterface.setBoundSurface(surf);
					obj->render();

					obj->previousBoundingBox = obj->boundingBox;
				}

				obj->shouldRedraw = false;
			}

			graphicsInterface.setColor(Vec4f(1,0,0,1));
			graphicsInterface.drawRect(minX, minY, maxX, maxY, true);
			graphicsInterface.setColor(Vec4f(0,0,1,1));
			graphicsInterface.drawRect(preMinX, preMinY, preMaxX, preMaxY, true);

		}
		else
		{
			if(invalidImage)
			{
				graphicsInterface.setColor(backgroundColor);
				graphicsInterface.setBoundSurface(surf);
				graphicsInterface.clear();
				redrawCount++;
			}
		}

		graphicsInterface.resetClippingPlane();
		invalidImage = false;

		if(redrawCount != 0)
		{
			graphicsInterface.setColor(Vec4f(1,1,1,1));
			graphicsInterface.drawToScreen();
		}		
		
		return redrawCount != 0;
	}

	std::vector<GuiInstance*>& GuiManager::getElements()
	{
		return objects;
	}
	
	std::vector< HashPair<std::wstring, GuiInstance*>* > GuiManager::getInstancesByName(std::wstring name)
	{
		return objectsByName.getAll(name);
	}

	GuiSurfaceInterface* GuiManager::getSurface()
	{
		return surf;
	}

	size_t GuiManager::getSize()
	{
		return objects.size();
	}

	void GuiManager::resizeImage(int width, int height)
	{
		bool wasBound = surf == graphicsInterface.getBoundSurface();
		if(surf != nullptr)
		{
			delete surf;
		}
		surf = graphicsInterface.createSurface(width, height);
		invalidImage = true;

		if(wasBound)
			graphicsInterface.setBoundSurface(surf);
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

	Color GuiManager::getBackgroundColor()
	{
		return backgroundColor;
	}

	void GuiManager::setBackgroundColor(Color c)
	{
		backgroundColor = c;
	}

	GuiGraphicsInterface* GuiManager::getGraphicsInterface()
	{
		return &graphicsInterface;
	}

	#pragma endregion

} //NAMESPACE glib END