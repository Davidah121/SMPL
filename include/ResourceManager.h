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
		for(auto it=resourceList.begin(); it!=resourceList.end(); it++)
		{
			delete (SmartMemory<T>*)it->second;
		}
		resourceList.clear();
	}

	void addResource(T* data, std::string key, bool array)
	{
		auto it = resourceList.find(key);
		if(it != resourceList.end())
		{
			if(data == it->second->getPointer())
			{
				//ignore
			}
			else
			{
				//delete and replace
				delete it->second;
				resourceList[key] = new SmartMemory(data, array);
			}
		}
		else
		{
			//add
			resourceList[key] = new SmartMemory(data, array);
		}
		
	}

	void deleteResource(std::string key)
	{
		auto it = resourceList.find(key);
		if(it!=resourceList.end())
		{
			delete (SmartMemory<T>*)it->second;
		}
		resourceList.erase(it); //Should call destructor
	}

	SmartMemory<T> getResource(std::string key)
	{
		SmartMemory<T> m;
		auto it = resourceList.find(key);
		if(it != resourceList.end())
		{
			m = *it->second;
		}
		return m;
	}

	size_t size()
	{
		return resourceList.size();
	}

	void clear()
	{
		for(auto it=resourceList.begin(); it!=resourceList.end(); it++)
		{
			delete (SmartMemory<T>*)it->second;
		}
		resourceList.clear();
	}
	
private:
	std::unordered_map<std::string, SmartMemory<T>*> resourceList = std::unordered_map<std::string, SmartMemory<T>*>();
};