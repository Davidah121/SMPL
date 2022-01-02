#include "InternalGuiManagerHeader.h"

namespace glib
{
	#pragma region GUI_MANAGER
	std::unordered_map<std::wstring, std::function<GuiInstance*(std::unordered_map<std::wstring, std::wstring>&)> > GuiManager::elementLoadingFunctions;

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
	}

	void GuiManager::registerLoadFunction(std::wstring className, std::function<GuiInstance*(std::unordered_map<std::wstring, std::wstring>&)> func)
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

				thisIns = it->second(map);
				
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

	GuiManager::GuiManager()
	{
		surf = Image(320,240);
	}

	GuiManager::GuiManager(int width, int height)
	{
		surf = Image(width,height);
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
		shouldDelete.clear();
	}

	void GuiManager::addElement(GuiInstance* k)
	{
		if(k!=nullptr)
		{
			if(k->getManager() == nullptr)
			{
				k->setManager(this);
				objects.push_back(k);

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
			if (objects[i] != nullptr)
			{
				if(objects[i]->getActive())
				{
					currActiveObjects.push_back(objects[i]);
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

	void GuiManager::renderGuiElements()
	{
		std::vector<GuiInstance*> currVisibleObjects = std::vector<GuiInstance*>();
		for(int i=0; i<objects.size(); i++)
		{
			if (objects[i] != nullptr)
			{
				if(objects[i]->getVisible())
				{
					currVisibleObjects.push_back(objects[i]);
				}
			}
		}

		//RENDER
		SimpleGraphics::setColor(backgroundColor);
		surf.clearImage();

		for (GuiInstance* obj : currVisibleObjects)
		{
			obj->baseRender();

			if(obj->canvas != nullptr)
			{
				obj->render(obj->canvas);
			}
			else
			{
				obj->render(&surf);
			}
		}
	}

	std::vector<GuiInstance*>& GuiManager::getElements()
	{
		return objects;
	}

	Image* GuiManager::getImage()
	{
		return &surf;
	}

	size_t GuiManager::getSize()
	{
		return objects.size();
	}

	void GuiManager::resizeImage(int width, int height)
	{
		surf = Image(width, height);
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

	#pragma endregion

} //NAMESPACE glib END