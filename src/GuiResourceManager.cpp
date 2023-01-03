#include "GuiManager.h"

namespace glib
{
	GuiResourceManager GuiResourceManager::singleton = GuiResourceManager();

	GuiResourceManager::GuiResourceManager()
	{
		
	}

	GuiResourceManager::~GuiResourceManager()
	{
		
	}

	void GuiResourceManager::addSprite(GuiSpriteInterface* data, std::string key, bool array)
	{
		spriteResources.addResource(data, key, array);
	}
	void GuiResourceManager::addFont(GuiFontInterface* data, std::string key, bool array)
	{
		fontResources.addResource(data, key, array);
	}

	SmartMemory<GuiSpriteInterface> GuiResourceManager::getSprite(std::string key)
	{
		return spriteResources.getResource(key);
	}
	SmartMemory<GuiFontInterface> GuiResourceManager::getFont(std::string key)
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
	
	ResourceManager<GuiSpriteInterface>* GuiResourceManager::getSpriteResourceManager()
	{
		return &spriteResources;
	}
	ResourceManager<GuiFontInterface>* GuiResourceManager::getFontResourceManager()
	{
		return &fontResources;
	}
	
	GuiResourceManager& GuiResourceManager::getResourceManager()
	{
		return singleton;
	}
};