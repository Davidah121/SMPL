#pragma once
#include <functional>

class Sort
{
public:
    Sort();
    ~Sort();

    template<typename T>
    static void mergeSort(T* list, int size, std::function<bool(T,T)> compareFunc);

    /*
    template<typename T>
    static void quickSort(T* list, int size, std::function<bool(T,T)> compareFunc);
    */
   
private:

    template<typename T>
    static void mergeSortRecursive(T* list, int start, int end, std::function<bool(T,T)> compareFunc);
    
    template<typename T>
    static void merge(T* list, int start, int mid, int end, std::function<bool(T,T)> compareFunc);
};

#pragma region MERGESORT

template<typename T>
inline void Sort::mergeSort(T* list, int size, std::function<bool(T,T)> compareFunc = nullptr)
{
    mergeSortRecursive(list, 0, size-1, compareFunc);
}

template<typename T>
inline void Sort::mergeSortRecursive(T* list, int start, int end, std::function<bool(T,T)> compareFunc)
{
    if(end-start == 0)
    {
        //same element, do nothing
    }
    else if(end-start == 1)
    {
        //two elements, swap manually
        //Assume compare function tells which is less than
        if(compareFunc!=nullptr)
        {
            if(compareFunc(list[end], list[start]))
            {
                T* temp = list[start];
                list[start] = list[end];
                list[end] = temp;
            }
            else
            {
                //In the correct order.
            }
        }
        else
        {
            if(list[end] < list[start])
            {
                T* temp = list[start];
                list[start] = list[end];
                list[end] = temp;
            }
            else
            {
                //In the correct order.
            }
        }
        
        
    }
    else if(end - start > 1)
    {
        //do the merge sorting thing
        int midIndex = (end+start)/2;
        mergeSortRecursive(list, start, midIndex, compareFunc);
        mergeSortRecursive(list, midIndex+1, end, compareFunc);
        merge(list, start, midIndex, end, compareFunc);
    }
}

template<typename T>
inline void Sort::merge(T* list, int start, int mid, int end, std::function<bool(T,T)> compareFunc)
{
    //create two temp arrays
    int lSize = mid-start;
    int rSize = end-mid+1;
    int totalSize = end-start;
    T leftArr[lSize];
    T rightArr[rSize];

    for(int i=0; i<lSize; i++)
    {
        leftArr[i] = list[start+i];
    }
    for(int i=0; i<rSize; i++)
    {
        rightArr[i] = list[mid+i+1];
    }

    //merge
    int lIndex = 0;
    int rIndex = 0;
    if(compareFunc!=nullptr)
    {
        for(int i=0; i<totalSize; i++)
        {
            //compare the index on the left to the right
            if(lIndex < lSize && rIndex < rSize)
            {
                if(compareFunc(leftArr[lIndex], rIndex[rIndex])
                {
                    list[i] = leftArr[lIndex];
                    lIndex++;
                }
                else
                {
                    list[i] = rightArr[rIndex];
                    rIndex++;
                }
            }
            else
            {
                if(lIndex < lSize)
                {
                    list[i] = leftArr[lIndex];
                    lIndex++;
                }
                else
                {
                    list[i] = rightArr[rIndex];
                    rIndex++;
                }
            }
        }
    }
    else
    {
        for(int i=0; i<totalSize; i++)
        {
            //compare the index on the left to the right
            if(lIndex < lSize && rIndex < rSize)
            {
                if(leftArr[lIndex] < rIndex[rIndex])
                {
                    list[i] = leftArr[lIndex];
                    lIndex++;
                }
                else
                {
                    list[i] = rightArr[rIndex];
                    rIndex++;
                }
            }
            else
            {
                if(lIndex < lSize)
                {
                    list[i] = leftArr[lIndex];
                    lIndex++;
                }
                else
                {
                    list[i] = rightArr[rIndex];
                    rIndex++;
                }
            }
        }
    }
}

#pragma endregion
