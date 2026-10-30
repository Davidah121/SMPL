#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "SimpleHashTable.h"
#include "SmartMemory.h"

namespace smpl
{
	template<typename T>
	struct CacheDataInfo
	{
		SmartMemory<T> data;
		size_t order;
	};

	template<typename K, typename T>
	class CachedDataList
	{
	public:
		/**
		* @brief Construct a new Cached Data List object
		* 		It maintains a set number of smart pointers for a data type and removes older data when out of allowed allocations.
		* 
		* @param maxSize 
		*/
		CachedDataList(uint32_t maxSize);

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
		void addData(const K& key, const SmartMemory<T>& data);
		void addData(K&& key, SmartMemory<T>&& data);

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
		bool getDataExist(const K& key);

		/**
		* @brief Gets the Data pointer corresponding to key.
		* 		It may return a nullptr.
		* 
		* @param key 
		* @return T* 
		*/
		SmartMemory<T> getData(const K& key);

		/**
		* @brief Removes data from the cache list based on the key.
		* 
		* @param key 
		*/
		void removeData(const K& key);

	private:
		/**
		* @brief Removes data to maintain the cache list size.
		* 
		*/
		void maintainCache();
		uint32_t maxSize = 64;
		size_t currOrder = 0;
		SimpleHashMap<uint32_t, CacheDataInfo<T>> dataList;
	};

	template<typename K, typename T>
	inline CachedDataList<K, T>::CachedDataList(uint32_t maxSize)
	{
		this->maxSize = maxSize;
	}

	template<typename K, typename T>
	inline CachedDataList<K, T>::~CachedDataList()
	{
		dataList.clear();
		currOrder = 0;
	}

	template<typename K, typename T>
	inline void CachedDataList<K, T>::addData(const K& key, const SmartMemory<T>& data)
	{
		CacheDataInfo<T> k;
		k.data = data;
		k.order = currOrder;
		dataList.insertOrReplace({key, k});
		currOrder++;
		maintainCache();
	}
	
	template<typename K, typename T>
	inline void CachedDataList<K, T>::addData(K&& key, SmartMemory<T>&& data)
	{
		std::pair<K, CacheDataInfo<T>> odr = {std::move(key), CacheDataInfo<T>{std::move(data), currOrder}};
		dataList.insertOrReplace(std::move(odr));
		currOrder++;
		maintainCache();
	}

	template<typename K, typename T>
	inline bool CachedDataList<K, T>::getDataExist(const K& key)
	{
		auto value = dataList.find(key);
		return value != dataList.end();
	}

	template<typename K, typename T>
	inline SmartMemory<T> CachedDataList<K, T>::getData(const K& key)
	{
		auto value = dataList.find(key);
		if(value != dataList.end())
		{
			return value->second.data;
		}
		return nullptr;
	}

	template<typename K, typename T>
	inline void CachedDataList<K, T>::removeData(const K& key)
	{
		dataList.erase(key);
	}

	template<typename K, typename T>
	inline void CachedDataList<K, T>::maintainCache()
	{
		if(dataList.size() > maxSize)
		{
			//must remove a value.
			//remove the oldest value.
			auto oldestIT = dataList.begin();
			for(auto it=dataList.begin(); it!=dataList.end(); ++it)
			{
				if(it->second.order < oldestIT->second.order)
					oldestIT = it;
			}

			//remove oldestIT
			dataList.erase(oldestIT);
		}
	}
};