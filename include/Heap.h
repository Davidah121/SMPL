#pragma once
#include<vector>

namespace smpl
{
    template<typename T>
    class Heap
    {
    public:
        static const bool TYPE_MIN = true;
        static const bool TYPE_MAX = false;
        Heap(bool heapType);
        ~Heap();

        bool peek(T& data);
        bool extract(T& data);
        void removeFirst();
        void insert(T data);
        void erase(T data);

        bool find(T data);

        size_t size();
        bool empty();
        void clear();

        bool getType();

    private:
        size_t parent(size_t index);
        size_t left(size_t index);
        size_t right(size_t index);
        
        void siftUpMinHeap(size_t startIndex);
        void siftUpMaxHeap(size_t startIndex);
        void siftDownMinHeap(size_t startIndex);
        void siftDownMaxHeap(size_t startIndex);

        bool minHeap = false;
        std::vector<T> buffer;
    };

    template<typename T>
    inline Heap<T>::Heap(bool heapType)
    {
        minHeap = heapType;
        buffer = std::vector<T>();
    }

    template<typename T>
    inline Heap<T>::~Heap()
    {
    }

    template<typename T>
    inline size_t Heap<T>::parent(size_t index)
    {
        return (index-1)/2;
    }

    template<typename T>
    inline size_t Heap<T>::left(size_t index)
    {
        return (2*index)+1;
    }

    template<typename T>
    inline size_t Heap<T>::right(size_t index)
    {
        return (2*index)+2;
    }

    template<typename T>
    inline size_t Heap<T>::size()
    {
        return buffer.size();
    }

    template<typename T>
    inline bool Heap<T>::empty()
    {
        return buffer.empty();
    }
    
    template<typename T>
    inline void Heap<T>::clear()
    {
        buffer.clear();
    }

    template<typename T>
    inline bool Heap<T>::getType()
    {
        return minHeap;
    }

    template<typename T>
    inline void Heap<T>::siftUpMinHeap(size_t startIndex)
    {
        size_t i = startIndex;
        while(i != 0 && buffer[parent(i)] > buffer[i])
        {
            std::swap(buffer[parent(i)], buffer[i]);
            i = parent(i);
        }
    }

    template<typename T>
    inline void Heap<T>::siftUpMaxHeap(size_t startIndex)
    {
        size_t i = startIndex;
        while(i != 0 && buffer[parent(i)] < buffer[i])
        {
            std::swap(buffer[parent(i)], buffer[i]);
            i = parent(i);
        }
    }

    template<typename T>
    inline void Heap<T>::siftDownMinHeap(size_t startIndex)
    {
        size_t i = startIndex;
        while(true)
        {
            //check children
            size_t lIndex = left(i);
            size_t rIndex = right(i);
            size_t currMinIndex = i;
            if(rIndex < buffer.size() && buffer[currMinIndex] < buffer[rIndex])
                currMinIndex = rIndex;
            
            if(lIndex < buffer.size()  && buffer[currMinIndex] < buffer[lIndex])
                currMinIndex = lIndex;
            
            if(currMinIndex != i)
            {
                std::swap(buffer[currMinIndex], buffer[i]);
                i = currMinIndex;
            }
            else
                break;
        }
    }

    template<typename T>
    inline void Heap<T>::siftDownMaxHeap(size_t startIndex)
    {
        size_t i = startIndex;
        while(true)
        {
            //check children
            size_t lIndex = left(i);
            size_t rIndex = right(i);
            size_t currMinIndex = i;
            if(rIndex < buffer.size() && buffer[currMinIndex] > buffer[rIndex])
                currMinIndex = rIndex;
            
            if(lIndex < buffer.size()  && buffer[currMinIndex] > buffer[lIndex])
                currMinIndex = lIndex;
            
            if(currMinIndex != i)
            {
                std::swap(buffer[currMinIndex], buffer[i]);
                i = currMinIndex;
            }
            else
                break;
        }
    }

    template<typename T>
    inline void Heap<T>::insert(T data)
    {
        //elements = N. If less (greater) than data, its a child. Insert on left or right.
        buffer.push_back(data);
        
        //fix heap
        if(minHeap)
            siftUpMinHeap(buffer.size()-1);
        else
            siftUpMaxHeap(buffer.size()-1);
    }

    template<typename T>
    inline bool Heap<T>::peek(T& data)
    {
        if(buffer.size() <= 0)
            return false;
        
        data = buffer[0];
        return true;
    }

    template<typename T>
    inline bool Heap<T>::extract(T& data)
    {
        if(buffer.size() <= 0)
            return false;

        data = buffer[0];
        buffer[0] = buffer[buffer.size()-1];
        buffer.pop_back();
        if(buffer.size() == 0)
            return true;
        
        if(minHeap)
            siftDownMinHeap(0);
        else
            siftDownMaxHeap(0);
        
        return true;
    }
    
    template<typename T>
    inline void Heap<T>::removeFirst()
    {
        if(buffer.size() <= 0)
            return;

        buffer[0] = buffer[buffer.size()-1];
        buffer.pop_back();
        if(buffer.size() == 0)
            return;
        
        if(minHeap)
            siftDownMinHeap(0);
        else
            siftDownMaxHeap(0);
    }

    template<typename T>
    inline void Heap<T>::erase(T data)
    {
        //special case.
        //O(1) find + O(LogN) delete if an additional hashmap (multimap) is used.
        //without hashmap, O(N) find + O(LogN) delete
        for(size_t i=0; i < buffer.size(); i++)
        {
            if(data == buffer[i])
            {
                //swap with the last index. pop_back. sift down???
                if(i == buffer.size()-1)
                {
                    buffer.pop_back();
                    break;
                }

                std::swap(buffer[i], buffer[buffer.size()-1]);
                buffer.pop_back();
                if(minHeap)
                    siftDownMinHeap(i);
                else
                    siftDownMaxHeap(i);
                
                break;
            }
        }
    }

    template<typename T>
    inline bool Heap<T>::find(T data)
    {
        for(size_t i=0; i<buffer.size(); i++)
        {
            if(buffer[i] == data)
                return true;
        }
        return false;
    }
}