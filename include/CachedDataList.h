#pragma once
#include <unordered_map>

template<typename T>
struct cacheDataInfo
{
	T* data;
	int order;
};

template<typename T>
class CachedDataList
{
public:
	CachedDataList(int maxSize);
	~CachedDataList();

	void addData(int key, T* data);
	bool getDataExist(int key);
	T* getData(int key);

	void removeData(int key);

private:
	void maintainCache();
	int maxSize = 64;
	size_t currOrder = 0;
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
		if(pair.second.data != nullptr)
		{
			delete pair.second.data;
			pair.second.data = nullptr;
		}
	}
	dataList.clear();
	currOrder = 0;
}

template<typename T>
inline void CachedDataList<T>::addData(int key, T* data)
{
	cacheDataInfo<T> k;
	k.data = data;
	k.order = currOrder;
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
		int previousOrder = currOrder;
		for(const std::pair<int, cacheDataInfo<T>>& pair : dataList)
		{
			if(pair.second.order < previousOrder)
			{
				previousOrder = pair.second.order;
				keyToRemove = pair.first;
			}
		}

		if(keyToRemove >= 0)
		{
			dataList.erase(keyToRemove);
		}
	}
}