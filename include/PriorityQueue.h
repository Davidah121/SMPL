#pragma once
#include "Heap.h"

namespace smpl
{
    template<typename T>
    struct PQData
    {
        double priority;
        T data;
        bool operator<(PQData& other)
        {
            return priority < other.priority;
        }
        bool operator<=(PQData& other)
        {
            return priority <= other.priority;
        }
        bool operator>(PQData& other)
        {
            return priority > other.priority;
        }
        bool operator>=(PQData& other)
        {
            return priority >= other.priority;
        }
        bool operator==(PQData& other)
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
        ~PriorityQueue();
        void add(double priority, T data);
        void pop();
        bool get(T& data);
        bool peek(T& data);
        bool get(T& data, double& priority);
        bool peek(T& data, double& priority);
        void erase(T data);
        void clear();
        size_t size();
        bool empty();
        bool find(T data);
        
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
    inline void PriorityQueue<T>::add(double priority, T data)
    {
        buffer.insert(PQData<T>{priority, data});
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
    inline bool PriorityQueue<T>::get(T& data, double& priority)
    {
        PQData<T> temp;
        bool retValue = buffer.extract(temp);
        data = temp.data;
        priority = temp.priority;
        return retValue;
    }
    template<typename T>
    inline bool PriorityQueue<T>::peek(T& data, double& priority)
    {
        PQData<T> temp;
        bool retValue = buffer.peek(temp);
        data = temp.data;
        priority = temp.priority;
        return retValue;
    }
    template<typename T>
    inline void PriorityQueue<T>::erase(T data)
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
    inline bool PriorityQueue<T>::find(T data)
    {
        return buffer.find(PQData<T>{0, data});
    }
    
}