#include "NewGui.h"

namespace smpl
{
	GuiResourceManager::GuiResourceManager()
	{
		
	}

	GuiResourceManager::~GuiResourceManager()
	{
		
	}

	void GuiResourceManager::addSprite(const SpriteInterface& data, const std::string& key, size_t elements, bool localMemory)
	{
		//insert or replace? Could ignore anything with same key
		spriteResources.insert({key, data});
	}
	void GuiResourceManager::addFont(const FontInterface& data, const std::string& key, size_t elements, bool localMemory)
	{
		//insert or replace? Could ignore anything with same key
		fontResources.insert({key, data});
	}

	SpriteInterface GuiResourceManager::getSprite(const std::string& key)
	{
		auto it = spriteResources.find(key);
		if(it != spriteResources.end())
			return it->second;
		return SpriteInterface();
	}
	FontInterface GuiResourceManager::getFont(const std::string& key)
	{
		auto it = fontResources.find(key);
		if(it != fontResources.end())
			return it->second;
		return FontInterface();
	}
	
	void GuiResourceManager::deleteSprite(const std::string& key)
	{
		spriteResources.erase(key);
	}
	void GuiResourceManager::deleteFont(const std::string& key)
	{
		fontResources.erase(key);
	}
	
	SimpleHashMap<std::string, SpriteInterface>& GuiResourceManager::getAllSpriteResources()
	{
		return spriteResources;
	}
	SimpleHashMap<std::string, FontInterface>& GuiResourceManager::getAllFontResources()
	{
		return fontResources;
	}
	
	GuiResourceManager& GuiResourceManager::getResourceManager()
	{
		static GuiResourceManager singleton;
		return singleton;
	}
};