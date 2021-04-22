#pragma once

#include <vector>
#include <functional>

template<class K, class T>
struct HashPair
{
    K key;
    T data;
};

template<class K, class T>
class SimpleHashMap
{
public:
    SimpleHashMap(unsigned char mode = MODE_KEEP_ALL, int initBuckets = 20, int maxSizeOfBuckets = 20);
    ~SimpleHashMap();

    static const unsigned char MODE_UNIQUE_KEY = 0;
    static const unsigned char MODE_REMOVE_LAST_FROM_BUCKET = 1;
    static const unsigned char MODE_KEEP_ALL = 2;

    void clear();
    void add(K key, T value);
    HashPair<K, T>* get(K key);
    std::vector<HashPair<K, T>*> getAll(K key);

    size_t size();
    size_t totalSize();

    void rehash();
    std::vector< HashPair<K, T>* > getData();

    void setMode(unsigned char v);
    void setMaxOfSameKey(int v);

private:
    std::hash<K> hashFunction;

    unsigned char type = MODE_KEEP_ALL;
    int maxBucketSize = 20;
    int maxOfSameKey = 10;
    size_t count = 0;
    
    
    std::vector< int > sizeOfBucket;
    std::vector< HashPair<K, T> > data;
};

template<class K, class T>
inline SimpleHashMap<K, T>::SimpleHashMap(unsigned char mode, int initBuckets, int maxSizeOfBuckets)
{
    type = mode;
    maxBucketSize = maxSizeOfBuckets;
    sizeOfBucket = std::vector<int>(initBuckets);
    data = std::vector< HashPair<K, T> >(initBuckets*maxBucketSize);
}

template<class K, class T>
inline SimpleHashMap<K, T>::~SimpleHashMap()
{

}

template<class K, class T>
inline void SimpleHashMap<K, T>::clear()
{
    sizeOfBucket.clear();
    data.clear();
    maxBucketSize = 20;
}

template<class K, class T>
inline size_t SimpleHashMap<K, T>::size()
{
    return count;
}

template<class K, class T>
inline size_t SimpleHashMap<K, T>::totalSize()
{
    return data.capacity();
}

template<class K, class T>
inline void SimpleHashMap<K, T>::setMode(unsigned char c)
{
    if(c >= 0 && c <= 2)
    {
        mode = c;
    }
    else
    {
        mode = MODE_KEEP_ALL;
    }
}

template<class K, class T>
inline void SimpleHashMap<K, T>::setMaxOfSameKey(int v)
{
    if(v>0)
        maxOfSameKey = v;
    else
        maxOfSameKey = 1;
}

template<class K, class T>
inline void SimpleHashMap<K, T>::add(K key, T value)
{
    size_t location = hashFunction(key);
    count++;

    if(sizeOfBucket.size() <= 0)
    {
        //init buckets to a size of atleast ceil(count * 1.75)
        size_t nSize = (size_t)MathExt::ceil((double)count * 1.75);
        sizeOfBucket = std::vector<int>(nSize);
        data = std::vector<HashPair<K,T>>(nSize*maxBucketSize);
    }

    size_t actualLocation = location % sizeOfBucket.size();
    size_t finalLocation = (actualLocation*maxBucketSize) + sizeOfBucket[actualLocation];

    if(type == MODE_REMOVE_LAST_FROM_BUCKET)
    {
        if(sizeOfBucket[actualLocation] == maxBucketSize - 1)
        {
            bool found = false;
            std::vector<HashPair<K,T>> tempBucket = std::vector<HashPair<K,T>>();
            int k = 0;
            int amountFound = 0;

            for(int i=0; i<sizeOfBucket[actualLocation]; i++)
            {
                if(data[actualLocation*maxBucketSize + i].key == key)
                {
                    amountFound++;
                }
            }

            if(amountFound>=maxOfSameKey)
            {
                for(int i=0; i<sizeOfBucket[actualLocation]; i++)
                {
                    if(!found)
                    {
                        if(data[actualLocation*maxBucketSize + i].key == key)
                        {
                            found = true;
                        }
                        else
                        {
                            data[actualLocation*maxBucketSize + k] = data[actualLocation*maxBucketSize + i];
                            k++;
                        }
                    }
                    else
                    {
                        data[actualLocation*maxBucketSize + k] = data[actualLocation*maxBucketSize + i];
                        k++;
                    }
                }
            }
            else
            {
                k = sizeOfBucket[actualLocation];
            }

            data[actualLocation*maxBucketSize + k] = {key, value};
            sizeOfBucket[actualLocation] = k+1;
        }
        else
        {
            data[finalLocation] = {key, value};
            sizeOfBucket[actualLocation]++;
        }
    }
    else if(type == MODE_UNIQUE_KEY)
    {
        bool found = false;
        for(int i=0; i<sizeOfBucket[actualLocation]; i++)
        {
            if(data[actualLocation*maxBucketSize + i].key == key)
            {
                //replace
                data[actualLocation*maxBucketSize + i] = {key, value};
                break;
            }
        }

        if(found==false)
        {
            data[finalLocation] = {key, value};
            sizeOfBucket[actualLocation]++;
        }
        
    }
    else if(type == MODE_KEEP_ALL)
    {
        data[finalLocation] = {key, value};
        sizeOfBucket[actualLocation]++;
    }

    if(sizeOfBucket[actualLocation] >= maxBucketSize)
    {
        rehash();
    }
}

template<class K, class T>
inline void SimpleHashMap<K, T>::rehash()
{
    size_t nBucketsSize = (size_t)MathExt::ceil((double)count * 1.5);
    size_t nMaxBucketsSize = (size_t)MathExt::ceil( (double)maxBucketSize * 1.25 );

    std::vector<int> nBuckets = std::vector<int>(nBucketsSize);
    std::vector< HashPair<K, T> > nData = std::vector< HashPair<K, T> >(nBucketsSize*nMaxBucketsSize);

    for(int i=0; i<sizeOfBucket.size(); i++)
    {
        int bucketLocation = i*maxBucketSize;
        for(int k=0; k<sizeOfBucket[i]; k++)
        {
            HashPair<K, T> pair = data[bucketLocation + k];

            size_t location = hashFunction(pair.key);
            location = location % nBucketsSize;
            size_t finalLocation = (location*nMaxBucketsSize) + nBuckets[location];

            nData[finalLocation] = pair;
            nBuckets[location]++;
        }
    }

    maxBucketSize = nMaxBucketsSize;
    sizeOfBucket = nBuckets;
    data = nData;
}

template<class K, class T>
inline HashPair<K, T>* SimpleHashMap<K, T>::get(K key)
{
    size_t location = hashFunction(key) % sizeOfBucket.size();
    HashPair<K, T>* value = nullptr;

    size_t actualLocation = location*maxBucketSize;
    for(int i=0; i<sizeOfBucket[location]; i++)
    {
        if(key == data[actualLocation+i].key)
        {
            value = &data[actualLocation+i];
            break;
        }
    }

    return value;
}

template<class K, class T>
inline std::vector<HashPair<K, T>*> SimpleHashMap<K, T>::getAll(K key)
{
    size_t location = hashFunction(key) % sizeOfBucket.size();
    std::vector<HashPair<K, T>*> values;

    size_t actualLocation = location*maxBucketSize;
    for(int i=0; i<sizeOfBucket[location]; i++)
    {
        if(key == data[actualLocation+i].key)
        {
            values.push_back( &data[actualLocation+i] );
        }
    }

    return values;
}

template<class K, class T>
inline std::vector<HashPair<K, T>*> SimpleHashMap<K,T>::getData()
{
    std::vector<HashPair<K, T>*> outputData = std::vector<HashPair<K, T>*>();

    for(int i=0; i<sizeOfBucket.size(); i++)
    {
        int bucketLocation = i*maxBucketSize;
        for(int k=0; k<sizeOfBucket[i]; k++)
        {
            outputData.push_back(&data[bucketLocation+k]);
        }
    }

    return outputData;
}