#pragma once
#include <functional>

class Sort
{
public:
    Sort();
    ~Sort();

    template<typename T>
    static void mergeSort(T* list, int size, std::function<bool(T,T)> compareFunc);

    template<typename T>
    static void insertionSort(T* list, int size, std::function<bool(T,T)> compareFunc);

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
inline void Sort::mergeSort(T* list, int size, std::function<bool(T,T)> compareFunc)
{
    Sort::mergeSortRecursive<T>(list, 0, size, compareFunc);
}

template<typename T>
inline void Sort::mergeSortRecursive(T* list, int start, int end, std::function<bool(T,T)> compareFunc)
{
    if(end-start>1)
    {
        //do the merge sorting thing
        int midIndex = (end+start)/2;
        mergeSortRecursive(list, start, midIndex, compareFunc);
        mergeSortRecursive(list, midIndex, end, compareFunc);
        merge(list, start, midIndex, end, compareFunc);
    }
}

template<typename T>
inline void Sort::merge(T* list, int start, int mid, int end, std::function<bool(T,T)> compareFunc)
{
    int leftSize = mid-start;
    int rightSize = end-mid;
    int totalSize = end-start;

    T* leftSide = new T[mid-start];
    T* rightSide = new T[end-mid];

    for(int i=0; i<leftSize; i++)
    {
        leftSide[i] = list[start+i];
    }
    for(int i=0; i<rightSize; i++)
    {
        rightSide[i] = list[mid+i];
    }

    int lI = 0;
    int rI = 0;
    int index = 0;
    while(true)
    {
        if(lI < leftSize && rI < rightSize)
        {
            if(compareFunc(leftSide[lI], rightSide[rI]))
            {
                list[start+index] = leftSide[lI];
                lI++;
            }
            else
            {
                list[start+index] = rightSide[rI];
                rI++;
            }
        }
        else if(lI < leftSize)
        {
            list[start+index] = leftSide[lI];
            lI++;
        }
        else if(rI < rightSize)
        {
            list[start+index] = rightSide[rI];
            rI++;
        }
        else
        {
            break;
        }
        index++;

        if(index>totalSize)
            break;
    }

    if(leftSide!=nullptr)
        delete[] leftSide;
    if(rightSide!=nullptr)
        delete[] rightSide;
}

#pragma endregion

#pragma region INSERTIONSORT
template<typename T>
inline void Sort::insertionSort(T* list, int size, std::function<bool(T,T)> compareFunc)
{
    for(int i=1; i<size; i++)
    {
        for(int j=i; j>=1; j--)
        {
            //if (list[j] < list[i])
            if(compareFunc(list[j], list[j-1]))
            {
                //swap
                T temp = list[j-1];
                list[j-1] = list[j];
                list[j] = temp;
            }
            else
            {
                break;
            }
        }
    }
}
#pragma endregion