#include "NewGui.h"

namespace smpl
{
	GuiResourceManager GuiResourceManager::singleton = GuiResourceManager();

	GuiResourceManager::GuiResourceManager()
	{
		
	}

	GuiResourceManager::~GuiResourceManager()
	{
		
	}

	void GuiResourceManager::addSprite(SpriteInterface* data, std::string key, bool array, bool localMemory)
	{
		spriteResources.addResource(data, key, array, localMemory);
	}
	void GuiResourceManager::addFont(FontInterface* data, std::string key, bool array, bool localMemory)
	{
		fontResources.addResource(data, key, array, localMemory);
	}

	SmartMemory<SpriteInterface> GuiResourceManager::getSprite(std::string key)
	{
		return spriteResources.getResource(key);
	}
	SmartMemory<FontInterface> GuiResourceManager::getFont(std::string key)
	{
		return fontResources.getResource(key);
	}
	
	void GuiResourceManager::deleteSprite(std::string key)
	{
		spriteResources.deleteResource(key);
	}
	void GuiResourceManager::deleteFont(std::string key)
	{
		fontResources.deleteResource(key);
	}
	
	ResourceManager<SpriteInterface>* GuiResourceManager::getSpriteResourceManager()
	{
		return &spriteResources;
	}
	ResourceManager<FontInterface>* GuiResourceManager::getFontResourceManager()
	{
		return &fontResources;
	}
	
	GuiResourceManager& GuiResourceManager::getResourceManager()
	{
		return singleton;
	}
};