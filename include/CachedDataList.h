#pragma once
#include <unordered_map>

template<typename T>
struct cacheDataInfo
{
	T* data;
	long order;
	bool shouldDelete;
};

template<typename T>
class CachedDataList
{
public:
	/**
	 * @brief Construct a new Cached Data List object
	 * 		It maintains a set number of pointers for a data type
	 * 			and removes older data when out of allowed space.
	 * 
	 * @param maxSize 
	 */
	CachedDataList(int maxSize);

	/**
	 * @brief Destroy the Cached Data List
	 * 		Can delete data in the 
	 * 
	 */
	~CachedDataList();

	/**
	 * @brief Adds new data to the cache list.
	 * 		If adding the data causes the list to grow too large, data will be
	 * 			removed from the cache list.
	 * 
	 * @param key 
	 * @param data 
	 * @param shouldDelete 
	 * 		Whether the pointer should be deleted when removed from the list.
	 * 		By default, it is true.
	 */
	void addData(int key, T* data, bool shouldDelete = true);

	/**
	 * @brief Gets whether the data corresponding to the key exists.
	 * 		It may not exist for different reasons.
	 * 			It may not have been added,
	 * 			It may have been removed by the user,
	 * 			It may have been removed by the list to maintain cache size.
	 * 
	 * @param key 
	 * @return true 
	 * @return false 
	 */
	bool getDataExist(int key);

	/**
	 * @brief Gets the Data pointer corresponding to key.
	 * 		It may return a nullptr.
	 * 
	 * @param key 
	 * @return T* 
	 */
	T* getData(int key);

	/**
	 * @brief Removes data from the cache list based on the key.
	 * 
	 * @param key 
	 */
	void removeData(int key);

private:
	/**
	 * @brief Removes data to maintain the cache list size.
	 * 
	 */
	void maintainCache();
	int maxSize = 64;
	long currOrder = 0;
	std::unordered_map<int, cacheDataInfo<T>> dataList;
};

template<typename T>
inline CachedDataList<T>::CachedDataList(int maxSize)
{
	this->maxSize = maxSize;
}

template<typename T>
inline CachedDataList<T>::~CachedDataList()
{
	for(std::pair<int, cacheDataInfo<T>> pair : dataList)
	{
		if(pair.second.shouldDelete)
		{
			if(pair.second.data != nullptr)
			{
				delete pair.second.data;
				pair.second.data = nullptr;
			}
		}
	}
	dataList.clear();
	currOrder = 0;
}

template<typename T>
inline void CachedDataList<T>::addData(int key, T* data, bool shouldDelete)
{
	cacheDataInfo<T> k;
	k.data = data;
	k.order = currOrder;
	k.shouldDelete = shouldDelete;
	dataList[key] = k;
	currOrder++;
	maintainCache();
}

template<typename T>
inline bool CachedDataList<T>::getDataExist(int key)
{
	auto value = dataList.find(key);
	return value != dataList.end();
}

template<typename T>
inline T* CachedDataList<T>::getData(int key)
{
	auto value = dataList.find(key);
	if(value != dataList.end())
	{
		return value->second.data;
	}
	return nullptr;
}

template<typename T>
inline void CachedDataList<T>::removeData(int key)
{
	auto tVal = dataList.find(key);
	if(tVal != dataList.end())
	{
		if(tVal->second.shouldDelete)
		{
			if(tVal->second.data != nullptr)
				delete tVal->second.data;
		}
	}
	dataList.erase(key);
}

template<typename T>
inline void CachedDataList<T>::maintainCache()
{
	if(dataList.size() > maxSize)
	{
		//must remove a value.
		//remove the oldest value.
		
		int keyToRemove = -1;
		long previousOrder = currOrder;
		cacheDataInfo<T> thingToDel = {nullptr, -1};

		for(const std::pair<int, cacheDataInfo<T>>& pair : dataList)
		{
			if(pair.second.order < previousOrder)
			{
				previousOrder = pair.second.order;
				keyToRemove = pair.first;
				thingToDel = pair.second;
			}
		}

		if(keyToRemove >= 0)
		{
			if(thingToDel.shouldDelete)
			{
				if(thingToDel.data != nullptr)
					delete thingToDel.data;
			}
			dataList.erase(keyToRemove);
		}
	}
}