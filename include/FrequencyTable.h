#pragma once
#include<vector>
#include "GeneralExceptions.h"

template<typename T>
class FrequencyTable
{
public:
	FrequencyTable();
	~FrequencyTable();

	void add(T value);
	int getFrequency(T value);

	T getValueAtLocation(int location);
	int getFrequencyAtLocation(int location);

	void clear();
	int sumOfFrequencies();

	int size();

	void sort();

private:
	std::vector<T> values = std::vector<T>();
	std::vector<int> frequency = std::vector<int>();
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
	for (int i = 0; i < values.size(); i++)
	{
		if (value == values[i])
		{
			frequency[i] += 1;
			exists = true;
			break;
		}
	}

	if (exists == false)
	{
		values.push_back(value);
		frequency.push_back(1);
	}
}

template<typename T>
inline int FrequencyTable<T>::getFrequency(T value)
{
	//Find the frequency if the value exists
	int freq = 0;
	for (int i = 0; i < values.size(); i++)
	{
		if (value == values[i])
		{
			freq = frequency[i];
			break;
		}
	}

	return freq;
}

template<typename T>
inline T FrequencyTable<T>::getValueAtLocation(int location)
{
	if (location >= 0 && location < values.size())
		return values[location];
	else
	{
		#ifdef USE_EXCEPTIONS
		throw OutOfBoundsError();
		#endif
	}
	
	return T();
}

template<typename T>
inline int FrequencyTable<T>::getFrequencyAtLocation(int location)
{
	if (location >= 0 && location < frequency.size())
		return frequency[location];
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
	frequency.clear();
}

template<typename T>
inline int FrequencyTable<T>::sumOfFrequencies()
{
	int sum = 0;
	for (int i = 0; i < frequency.size(); i++)
	{
		sum += frequency[i];
	}

	return sum;
}

template<typename T>
inline int FrequencyTable<T>::size()
{
	return values.size();
}

template<typename T>
inline void FrequencyTable<T>::sort()
{
	//selection sort
	for (int i = 0; i < values.size(); i++)
	{
		for (int j = i; j > 0; j--)
		{
			if (frequency[j] > frequency[j - 1])
			{
				//swap

				T oldValue = values[j];
				int oldFrequency = frequency[j];

				values[j] = values[j - 1];
				frequency[j] = frequency[j - 1];

				values[j - 1] = oldValue;
				frequency[j - 1] = oldFrequency;
			}
		}
	}
}
