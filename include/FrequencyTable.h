#pragma once
#include "StandardTypes.h"
#include "GeneralExceptions.h"
#include "SimpleHashTable.h"

namespace smpl
{
	template<typename T>
	class FrequencyTable
	{
	public:
		/**
		 * @brief Creates a FrequencyTable Object
		 * 		Uses templates to allow counting for any object.
		 */
		FrequencyTable();

		/**
		 * @brief Destroys a FrequencyTable Object.
		 */
		~FrequencyTable();

		/**
		 * @brief Adds a value to the table.
		 * 		If it already exists, its frequency is increased.
		 * @param value
		 * 		The value to add to the table.
		 */
		void add(const T& value);
		void add(T&& value);

		/**
		 * @brief Returns the frequency of the value.
		 * 		Takes O(N) time if unsorted.
		 * 		Takes O(LogN) time if sorted.
		 * @param value
		 * 		The value to add to the table.
		 * @return size_t
		 * 		Returns a value greater than 0 if the value was found.
		 */
		size_t getFrequency(const T& value) const;

		auto begin() const
		{
			return values.begin();
		}

		auto end() const
		{
			return values.end();
		}

		/**
		 * @brief Clears the frequency table.
		 */
		void clear();

		/**
		 * @brief Returns a sum of all frequencies.
		 * 
		 * @return size_t 
		 */
		size_t sumOfFrequencies() const;

		/**
		 * @brief Returns the size of the frequency table.
		 * 
		 * @return size_t 
		 */
		size_t size() const;

	private:
		SimpleHashMap<T, size_t> values;
		bool sorted = false;
	};

	template<typename T>
	inline FrequencyTable<T>::FrequencyTable()
	{
	}

	template<typename T>
	inline FrequencyTable<T>::~FrequencyTable()
	{
	}

	template<typename T>
	inline void FrequencyTable<T>::add(const T& value)
	{
		values[value]++;
	}

	template<typename T>
	inline void FrequencyTable<T>::add(T&& value)
	{
		values[value]++;
	}

	template<typename T>
	inline size_t FrequencyTable<T>::getFrequency(const T& value) const
	{
		auto it = values.find(value);
		if(it != values.end())
			return it->second;
		return 0;
	}

	template<typename T>
	inline void FrequencyTable<T>::clear()
	{
		values.clear();
	}

	template<typename T>
	inline size_t FrequencyTable<T>::sumOfFrequencies() const
	{
		size_t sum = 0;
		for(auto& it : values)
		{
			sum += it.second;
		}

		return sum;
	}

	template<typename T>
	inline size_t FrequencyTable<T>::size() const
	{
		return values.size();
	}

} //NAMESPACE smpl END