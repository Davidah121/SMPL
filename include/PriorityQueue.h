#pragma once
#include "StandardTypes.h"
#include "Heap.h"
#include <cstddef>
#include <cstdint>

namespace smpl
{
    template<typename T>
    struct PQData
    {
        double priority;
        T data;
        bool operator<(const PQData& other) const
        {
            return priority < other.priority;
        }
        bool operator<=(const PQData& other) const
        {
            return priority <= other.priority;
        }
        bool operator>(const PQData& other) const
        {
            return priority > other.priority;
        }
        bool operator>=(const PQData& other) const
        {
            return priority >= other.priority;
        }
        bool operator==(const PQData& other) const
        {
            return data == other.data; //ignore priority
        }
    };

    template<typename T>
    class PriorityQueue
    {
    public:
        static const bool TYPE_MAX = false;
        static const bool TYPE_MIN = true;
        
        PriorityQueue();
        PriorityQueue(bool heapType);

		PriorityQueue(const PriorityQueue<T>& other) = default;
		PriorityQueue& operator=(const PriorityQueue<T>& other) = default;
		PriorityQueue(PriorityQueue<T>&& other) = default;
		PriorityQueue& operator=(PriorityQueue<T>&& other) = default;

        ~PriorityQueue();
        void add(const PQData<T>& data);
        void add(PQData<T>&& data);
        void pop();
        bool get(T& data);
        bool peek(T& data);
        bool get(PQData<T>& data);
        bool peek(PQData<T>& data);
        void erase(const T& data);
        void clear();
        size_t size();
        bool empty();
        bool find(const T& data);
        
    private:
        Heap<PQData<T>> buffer = Heap<PQData<T>>(Heap<PQData<T>>::TYPE_MAX);
    };


    template<typename T>
    inline PriorityQueue<T>::PriorityQueue() 
    {

    }
    template<typename T>
    inline PriorityQueue<T>::PriorityQueue(bool heapType)
    {
        buffer = Heap<PQData<T>>(heapType);
    }

    template<typename T>
    inline PriorityQueue<T>::~PriorityQueue()
    {
        
    }
    template<typename T>
    inline void PriorityQueue<T>::add(const PQData<T>& data)
    {
        buffer.insert(data);
    }
    template<typename T>
    inline void PriorityQueue<T>::add(PQData<T>&& data)
    {
        buffer.insert(std::move(data));
    }
    template<typename T>
    inline void PriorityQueue<T>::pop()
    {
        buffer.removeFirst();
    }
    template<typename T>
    inline bool PriorityQueue<T>::get(T& data)
    {
        PQData<T> temp;
        bool retValue = buffer.extract(temp);
        data = temp.data;
        return retValue;
    }
    template<typename T>
    inline bool PriorityQueue<T>::peek(T& data)
    {
        PQData<T> temp;
        bool retValue = buffer.peek(temp);
        data = temp.data;
        return retValue;
    }

    template<typename T>
    inline bool PriorityQueue<T>::get(PQData<T>& data)
    {
        bool retValue = buffer.extract(data);
        return retValue;
    }
    template<typename T>
    inline bool PriorityQueue<T>::peek(PQData<T>& data)
    {
        bool retValue = buffer.peek(data);
        return retValue;
    }
    template<typename T>
    inline void PriorityQueue<T>::erase(const T& data)
    {
        buffer.erase(PQData<T>{0, data});
    }
    template<typename T>
    inline void PriorityQueue<T>::clear()
    {
        buffer.clear();
    }
    template<typename T>
    inline size_t PriorityQueue<T>::size()
    {
        return buffer.size();
    }
    template<typename T>
    inline bool PriorityQueue<T>::empty()
    {
        return buffer.empty();
    }
    template<typename T>
    inline bool PriorityQueue<T>::find(const T& data)
    {
        return buffer.find(PQData<T>{0, data});
    }
    
}