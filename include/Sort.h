#pragma once
#include <functional>

namespace glib
{

    class Sort
    {
    public:
        Sort();
        ~Sort();

        template<typename T>
        static void mergeSort(T* list, size_t size, std::function<bool(T,T)> compareFunc);

        template<typename T>
        static void insertionSort(T* list, size_t size, std::function<bool(T,T)> compareFunc);

        template<typename T>
        static void quickSort(T* list, size_t size, std::function<bool(T,T)> compareFunc);
    
    private:

        template<typename T>
        static void mergeSortRecursive(T* list, size_t start, size_t end, std::function<bool(T,T)> compareFunc);
        
        template<typename T>
        static void merge(T* list, size_t start, size_t mid, size_t end, std::function<bool(T,T)> compareFunc);

        template<typename T>
        static void quickSortRecursive(T* list, size_t start, size_t end, std::function<bool(T,T)> compareFunc);
        
        template<typename T>
        static size_t quickSortPartition(T* list, size_t start, size_t end, std::function<bool(T,T)> compareFunc);
    };

    #pragma region MERGESORT

    template<typename T>
    inline void Sort::mergeSort(T* list, size_t size, std::function<bool(T,T)> compareFunc)
    {
        Sort::mergeSortRecursive<T>(list, 0, size, compareFunc);
    }

    template<typename T>
    inline void Sort::mergeSortRecursive(T* list, size_t start, size_t end, std::function<bool(T,T)> compareFunc)
    {
        if(end-start>1)
        {
            //do the merge sorting thing
            size_t midIndex = (end+start)/2;
            mergeSortRecursive(list, start, midIndex, compareFunc);
            mergeSortRecursive(list, midIndex, end, compareFunc);
            merge(list, start, midIndex, end, compareFunc);
        }
    }

    template<typename T>
    inline void Sort::merge(T* list, size_t start, size_t mid, size_t end, std::function<bool(T,T)> compareFunc)
    {
        size_t leftSize = mid-start;
        size_t rightSize = end-mid;
        size_t totalSize = end-start;

        T* leftSide = new T[mid-start];
        T* rightSide = new T[end-mid];

        for(size_t i=0; i<leftSize; i++)
        {
            leftSide[i] = list[start+i];
        }
        for(size_t i=0; i<rightSize; i++)
        {
            rightSide[i] = list[mid+i];
        }

        size_t lI = 0;
        size_t rI = 0;
        size_t index = 0;
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
    inline void Sort::insertionSort(T* list, size_t size, std::function<bool(T,T)> compareFunc)
    {
        for(size_t i=1; i<size; i++)
        {
            for(size_t j=i; j>=1; j--)
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

    #pragma region QUICKSORT

    template<typename T>
    void Sort::quickSort(T* list, size_t size, std::function<bool(T,T)> compareFunc)
    {
        Sort::quickSortRecursive<T>(list, 0, size-1, compareFunc);
    }

    template<typename T>
    void Sort::quickSortRecursive(T* list, size_t start, size_t end, std::function<bool(T,T)> compareFunc)
    {
        if(start < end)
        {
            size_t pivot = Sort::quickSortPartition<T>(list, start, end, compareFunc);

            if(pivot > start)
                Sort::quickSortRecursive<T>(list, start, pivot-1, compareFunc);
            
            if(pivot < end)
                Sort::quickSortRecursive<T>(list, pivot+1, end, compareFunc);
        }
    }

    template<typename T>
    size_t Sort::quickSortPartition(T* list, size_t start, size_t end, std::function<bool(T,T)> compareFunc)
    {
        //pick a pivot
        //way to divide and not have an infinite loop
            //size_t pivotLocation = ((end+start) / 3) + ((end+start)%3);
        
        size_t pivotLocation = (end+start)/2;
        T pivotValue = list[pivotLocation];
        size_t j = start-1;

        //in order to go left or right, we swap points around.
        //_#_ is the pivot.     The swap point is where the pivot is. 
        //0,3,_4_,8,2,7
        //result
        //0,3,2   4   8,7
        

        //`0,3,_4_,8,2,7
        //0`,3,_4_,8,2,7     0 less than 4 swap stuff
        //0,3`,_4_,8,2,7     3 less than 4 swap stuff
        //0,3`,_4_,8,2,7     4 equal to 4 so do nothing
        //0,3`,_4_,8,2,7     8 greater than 4 do nothing
        //0,3,_4_`,8,2,7     2 less than 4 so swap with 4
        //0,3,2,8`,_4_,7     7 greater than 4 do nothing

        //final Result with last swap
        //0,3,2     _4_     8,7

        //swap based on pivot
        for(int i=start; i<=end; i++)
        {
            if(compareFunc(list[i], pivotValue))
            {
                //go left
                j++;
                if(j!=i)
                {
                    //swap
                    T temp = list[i];
                    list[i] = list[j];
                    list[j] = temp;

                    if(j == pivotLocation)
                    {
                        pivotLocation = i;
                    }
                }
            }
            else
            {
                //go right
                //do nothing
            }
        }

        T temp = list[pivotLocation];
        list[pivotLocation] = list[j+1];
        list[j+1] = temp;

        return j+1;
    }

    #pragma endregion

} //NAMESPACE glib END