#include "NewGui.h"

namespace smpl
{
	GuiResourceManager::GuiResourceManager()
	{
		
	}

	GuiResourceManager::~GuiResourceManager()
	{
		
	}

	void GuiResourceManager::addSprite(SpriteInterface* data, std::string key, size_t elements, bool localMemory)
	{
		spriteResources.addResource(data, key, elements, localMemory);
	}
	void GuiResourceManager::addFont(FontInterface* data, std::string key, size_t elements, bool localMemory)
	{
		fontResources.addResource(data, key, elements, localMemory);
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
		static GuiResourceManager singleton;
		return singleton;
	}
};