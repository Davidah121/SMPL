#pragma once
#include "SmartMemory.h"
#include <string>

template<class T>
class ResourceManager
{
public:
	ResourceManager()
	{
	}

	~ResourceManager()
	{
		resourceList.clear();
	}

	void addResource(T* data, std::string key, bool array, bool localMemory)
	{
		auto it = resourceList.find(key);
		if(it != resourceList.end())
		{
			if(data == it->second.getPointer())
			{
				//ignore
			}
			else
			{
				//replace. It should delete itself since it has delete rights.
				resourceList[key] = SmartMemory(data, array, true, localMemory);
			}
		}
		else
		{
			//add
			resourceList[key] = SmartMemory(data, array, true, localMemory);
		}
		
	}

	void deleteResource(std::string key)
	{
		auto it = resourceList.find(key);
		resourceList.erase(it); //Should call destructor which will delete the pointer
	}

	SmartMemory<T> getResource(std::string key)
	{
		SmartMemory<T> m;
		auto it = resourceList.find(key);
		if(it != resourceList.end())
		{
			m = it->second;
		}
		return m;
	}

	size_t size()
	{
		return resourceList.size();
	}

	void clear()
	{
		resourceList.clear();
	}
	
private:
	std::unordered_map<std::string, SmartMemory<T>> resourceList = std::unordered_map<std::string, SmartMemory<T>>();
};