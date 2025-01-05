#pragma once
#include "SmartMemory.h"
#include <string>

namespace smpl
{
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

		void addResource(T* data, std::string key, size_t elements, bool localMemory)
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
					if(!localMemory)
						resourceList[key] = SmartMemory<T>::createDeleteRights(data, 1);
					else
						resourceList[key] = SmartMemory<T>::createNoDelete(data, 1);
				}
			}
			else
			{
				//add
				if(!localMemory)
					resourceList[key] = SmartMemory<T>::createDeleteRights(data, 1);
				else
					resourceList[key] = SmartMemory<T>::createNoDelete(data, 1);
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
}