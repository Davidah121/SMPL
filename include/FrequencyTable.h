#pragma once
#include<vector>
#include "GeneralExceptions.h"
#include "Sort.h"

namespace smpl
{

	template<typename T>
	struct freqPair
	{
		size_t frequency;
		T data;
	};

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
		void add(T value);

		/**
		 * @brief Returns the frequency of the value.
		 * 		Takes O(N) time if unsorted.
		 * 		Takes O(LogN) time if sorted.
		 * @param value
		 * 		The value to add to the table.
		 * @return size_t
		 * 		Returns a value greater than 0 if the value was found.
		 */
		size_t getFrequency(T value);

		/**
		 * @brief Gets the value at the specified location in the list.
		 * 		Location may change after sorting.
		 * 		If the location is invalid and USE_EXCEPTIONS is defined, an OutOfBounds Error is thrown.
		 * 	
		 * @param location 
		 * 		The location to check.
		 * @return T 
		 * 		The value returned. If the function fails, returns the default constructed value if possible.
		 */
		T getValueAtLocation(size_t location);

		/**
		 * @brief Gets the frequency at the specified location in the list.
		 * 		Location may change after sorting.
		 * 		If the location is invalid and USE_EXCEPTIONS is defined, an OutOfBounds Error is thrown.
		 * 	
		 * @param location 
		 * 		The location to check.
		 * @return size_t
		 * 		If the function fails, returns a frequency value of 0.
		 */
		size_t getFrequencyAtLocation(size_t location);

		/**
		 * @brief Clears the frequency table.
		 */
		void clear();

		/**
		 * @brief Returns a sum of all frequencies.
		 * 
		 * @return size_t 
		 */
		size_t sumOfFrequencies();

		/**
		 * @brief Returns the size of the frequency table.
		 * 
		 * @return size_t 
		 */
		size_t size();

		/**
		 * @brief Sorts the frequency table.
		 * 
		 * @param insertionSort 
		 * 		If set to true, Insertion Sort is used. Otherwise, Merge Sort is used.
		 */
		void sort(bool insertionSort);

	private:
		std::vector<freqPair<T>> values = std::vector<freqPair<T>>();
		bool sorted = false;
	};

	template<typename T>
	inline FrequencyTable<T>::FrequencyTable()
	{
	}

	template<typename T>
	inline FrequencyTable<T>::~FrequencyTable()
	{
		clear();
	}

	template<typename T>
	inline void FrequencyTable<T>::add(T value)
	{
		//check if the value exists
		bool exists = false;
		size_t index = -1;

		if(sorted == false)
		{
			for (size_t i = 0; i < values.size(); i++)
			{
				if (value == values[i].data)
				{
					exists = true;
					index = i;
					break;
				}
			}
		}
		else
		{
			size_t l = 0;
			size_t r = values.size()-1;

			while(l < r)
			{
				size_t m = (l+r) / 2;

				if(values[m].data == value)
				{
					index = m;
					break;
				}
				else if(value < values[m].data)
					r = m;
				else
					l = m;
			}
		}
		

		if (exists == false)
		{
			values.push_back({1, value});
		}
		else
		{
			values[index].frequency++;
		}
	}

	template<typename T>
	inline size_t FrequencyTable<T>::getFrequency(T value)
	{
		//Find the frequency if the value exists
		size_t freq = 0;

		if(sorted == false)
		{
			for (size_t i = 0; i < values.size(); i++)
			{
				if (value == values[i].data)
				{
					freq = values[i].frequency;
					break;
				}
			}
		}
		else
		{
			size_t l = 0;
			size_t r = values.size()-1;

			while(l < r)
			{
				size_t m = (l+r) / 2;

				if(values[m].data == value)
					return values[m].frequency;
				else if(value < values[m].data)
					r = m;
				else
					l = m;
			}
		}

		return freq;
	}

	template<typename T>
	inline T FrequencyTable<T>::getValueAtLocation(size_t location)
	{
		if (location >= 0 && location < values.size())
			return values[location].data;
		else
		{
			#ifdef USE_EXCEPTIONS
			throw OutOfBoundsError();
			#endif
		}
		
		return T();
	}

	template<typename T>
	inline size_t FrequencyTable<T>::getFrequencyAtLocation(size_t location)
	{
		if (location >= 0 && location < values.size())
			return values[location].frequency;
		else
		{
			#ifdef USE_EXCEPTIONS
			throw OutOfBoundsError();
			#endif
		}

		return 0;
	}

	template<typename T>
	inline void FrequencyTable<T>::clear()
	{
		values.clear();
	}

	template<typename T>
	inline size_t FrequencyTable<T>::sumOfFrequencies()
	{
		size_t sum = 0;
		for (size_t i = 0; i < values.size(); i++)
		{
			sum += values[i].frequency;
		}

		return sum;
	}

	template<typename T>
	inline size_t FrequencyTable<T>::size()
	{
		return values.size();
	}

	template<typename T>
	inline void FrequencyTable<T>::sort(bool insertionSort)
	{
		if(insertionSort)
		{
			Sort::insertionSort<freqPair<T>>(values.data(), values.size(), [](freqPair<T> a, freqPair<T> b) ->bool{
				return a.data < b.data;
			});
		}
		else
		{
			//MergeSort
			Sort::mergeSort<freqPair<T>>(values.data(), values.size(), [](freqPair<T> a, freqPair<T> b) ->bool{
				return a.data < b.data;
			});
		}
		
	}

} //NAMESPACE smpl END