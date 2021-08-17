#pragma once

#include <vector>
#include <functional>

namespace glib
{

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
        SimpleHashMap(const SimpleHashMap<K, T>& other);
        void operator=(const SimpleHashMap<K, T>& other);
        ~SimpleHashMap();

        static const unsigned char MODE_UNIQUE_KEY = 0;
        static const unsigned char MODE_REMOVE_LAST_FROM_BUCKET = 1;
        static const unsigned char MODE_KEEP_ALL = 2;

        void clear();
        void add(K key, T value);
        void removeFirst(K key);
        void removeLast(K key);
        void removeAll(K key);
        void remove(HashPair<K, T>* rm);
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
        std::vector< HashPair<K, T>* > data;
    };

    template<class K, class T>
    inline SimpleHashMap<K, T>::SimpleHashMap(unsigned char mode, int initBuckets, int maxSizeOfBuckets)
    {
        type = mode;
        maxBucketSize = maxSizeOfBuckets;
        sizeOfBucket = std::vector<int>(initBuckets);
        data = std::vector< HashPair<K, T>* >(initBuckets*maxBucketSize);
    }

    template<class K, class T>
    inline SimpleHashMap<K, T>::SimpleHashMap(const SimpleHashMap<K, T>& other)
    {
        type = other.type;
        maxBucketSize = other.maxBucketSize;
        maxOfSameKey = other.maxOfSameKey;
        count = other.count;

        sizeOfBucket = other.sizeOfBucket;
        data = std::vector< HashPair<K, T>* >(other.data.size());

        for(int i=0; i<sizeOfBucket.size(); i++)
        {
            int bucketLocation = i*maxBucketSize;
            for(int k=0; k<sizeOfBucket[i]; k++)
            {
                if(other.data[bucketLocation + k] != nullptr)
                {
                    data[bucketLocation + k] = new HashPair<K, T>();
                    data[bucketLocation + k]->key = other.data[bucketLocation + k]->key;
                    data[bucketLocation + k]->data = other.data[bucketLocation + k]->data;
                }
            }
        }
    }

    template<class K, class T>
    inline void SimpleHashMap<K, T>::operator=(const SimpleHashMap<K, T>& other)
    {
        type = other.type;
        maxBucketSize = other.maxBucketSize;
        maxOfSameKey = other.maxOfSameKey;
        count = other.count;

        sizeOfBucket = other.sizeOfBucket;
        data = std::vector< HashPair<K, T>* >(other.data.size());

        for(int i=0; i<sizeOfBucket.size(); i++)
        {
            int bucketLocation = i*maxBucketSize;
            for(int k=0; k<sizeOfBucket[i]; k++)
            {
                if(other.data[bucketLocation + k] != nullptr)
                {
                    data[bucketLocation + k] = new HashPair<K, T>();
                    data[bucketLocation + k]->key = other.data[bucketLocation + k]->key;
                    data[bucketLocation + k]->data = other.data[bucketLocation + k]->data;
                }
            }
        }
    }

    template<class K, class T>
    inline SimpleHashMap<K, T>::~SimpleHashMap()
    {
        clear();
    }

    template<class K, class T>
    inline void SimpleHashMap<K, T>::clear()
    {
        for(int i=0; i<sizeOfBucket.size(); i++)
        {
            int bucketLocation = i*maxBucketSize;
            for(int k=0; k<sizeOfBucket[i]; k++)
            {
                if(data[bucketLocation+k]!=nullptr)
                    delete data[bucketLocation+k];
                
                data[bucketLocation+k] = nullptr;
            }
        }

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
            data = std::vector<HashPair<K,T>*>(nSize*maxBucketSize);
        }

        size_t actualLocation = location % sizeOfBucket.size();
        size_t finalLocation = (actualLocation*maxBucketSize) + sizeOfBucket[actualLocation];

        if(type == MODE_REMOVE_LAST_FROM_BUCKET)
        {
            if(sizeOfBucket[actualLocation] == maxBucketSize - 1)
            {
                bool found = false;
                std::vector<HashPair<K,T>*> tempBucket = std::vector<HashPair<K,T>*>();
                int k = 0;
                int amountFound = 0;

                for(int i=0; i<sizeOfBucket[actualLocation]; i++)
                {
                    if(data[actualLocation*maxBucketSize + i]->key == key)
                    {
                        amountFound++;
                    }
                }

                if(amountFound>=maxOfSameKey)
                {
                    int deletions = amountFound - maxOfSameKey;
                    for(int i=0; i<deletions; i++)
                    {
                        removeFirst(key);
                    }
                }
                
                k = sizeOfBucket[actualLocation];

                data[actualLocation*maxBucketSize + k] = new HashPair<K, T>();
                data[actualLocation*maxBucketSize + k]->key = key;
                data[actualLocation*maxBucketSize + k]->data = value;
                
                sizeOfBucket[actualLocation]++;
            }
            else
            {
                data[finalLocation] = new HashPair<K, T>();
                data[finalLocation]->key = key;
                data[finalLocation]->data = value;

                sizeOfBucket[actualLocation]++;
            }
        }
        else if(type == MODE_UNIQUE_KEY)
        {
            bool found = false;
            for(int i=0; i<sizeOfBucket[actualLocation]; i++)
            {
                if(data[actualLocation*maxBucketSize + i]->key == key)
                {
                    //replace
                    data[actualLocation*maxBucketSize + i]->key = key;
                    data[actualLocation*maxBucketSize + i]->data = value;
                    break;
                }
            }

            if(found==false)
            {
                data[finalLocation] = new HashPair<K, T>();
                data[finalLocation]->key = key;
                data[finalLocation]->data = value;
                sizeOfBucket[actualLocation]++;
            }
            
        }
        else if(type == MODE_KEEP_ALL)
        {
            data[finalLocation] = new HashPair<K, T>();
            data[finalLocation]->key = key;
            data[finalLocation]->data = value;
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
        std::vector< HashPair<K, T>* > nData = std::vector< HashPair<K, T>* >(nBucketsSize*nMaxBucketsSize);

        for(int i=0; i<sizeOfBucket.size(); i++)
        {
            int bucketLocation = i*maxBucketSize;
            for(int k=0; k<sizeOfBucket[i]; k++)
            {
                HashPair<K, T>* pair = data[bucketLocation + k];

                size_t location = hashFunction(pair->key);
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
    inline void SimpleHashMap<K, T>::removeFirst(K key)
    {
        size_t location = hashFunction(key) % sizeOfBucket.size();

        size_t actualLocation = location*maxBucketSize;
        int indexOfFirst = -1;
        for(int i=0; i<sizeOfBucket[location]; i++)
        {
            if(indexOfFirst == -1)
            {
                if(key == data[actualLocation+i]->key)
                {
                    indexOfFirst = i;
                }
            }
            else
            {
                //swap till the end of the bucket
                HashPair<K,T>* temp = data[actualLocation+i-1];
                data[actualLocation+i-1] = data[actualLocation+i];
                data[actualLocation+i] = temp;
            }
        }

        if(indexOfFirst!=-1)
        {
            //found key
            delete data[actualLocation + sizeOfBucket[location] - 1];
            data[actualLocation + sizeOfBucket[location] - 1] = nullptr;
            sizeOfBucket[location]--;
        }
    }

    template<class K, class T>
    inline void SimpleHashMap<K, T>::removeLast(K key)
    {
        size_t location = hashFunction(key) % sizeOfBucket.size();

        size_t actualLocation = location*maxBucketSize;
        int indexOfLast = -1;
        for(int i=sizeOfBucket[location]-1; i>=0; i--)
        {
            if(key == data[actualLocation+i]->key)
            {
                indexOfLast = i;
                break;
            }
        }

        if(indexOfLast != -1)
        {
            //found key
            for(int i=indexOfLast; i<sizeOfBucket[location]; i++)
            {
                //swap till the end of the bucket
                HashPair<K,T>* temp = data[actualLocation+i-1];
                data[actualLocation+i-1] = data[actualLocation+i];
                data[actualLocation+i] = temp;
            }
            
            delete data[actualLocation + sizeOfBucket[location] - 1];
            data[actualLocation + sizeOfBucket[location] - 1] = nullptr;
            sizeOfBucket[location]--;
        }
    }

    template<class K, class T>
    inline void SimpleHashMap<K, T>::removeAll(K key)
    {
        size_t location = hashFunction(key) % sizeOfBucket.size();

        size_t actualLocation = location*maxBucketSize;
        std::vector<HashPair<K, T>*> nBucket;

        for(int i=0; i<sizeOfBucket[location]; i++)
        {
            if(key != data[actualLocation+i]->key)
            {
                nBucket.push_back( data[actualLocation+i] );
            }
            else
            {
                delete data[actualLocation+i];
                data[actualLocation+i] = nullptr;
            }
        }

        for(int i=0; i<nBucket.size(); i++)
        {
            data[actualLocation+i] = nBucket[i];
        }
        sizeOfBucket[location] = nBucket.size();
    }

    template<class K, class T>
    inline void SimpleHashMap<K, T>::remove(HashPair<K, T>* p)
    {
        if(p!=nullptr)
        {
            size_t location = hashFunction(p->key) % sizeOfBucket.size();

            size_t actualLocation = location*maxBucketSize;
            bool found=false;

            for(int i=0; i<sizeOfBucket[location]; i++)
            {
                if(found == false)
                {
                    if(p == data[actualLocation+i])
                    {
                        found = true;
                    }
                }
                else
                {
                    //swap till the end of the bucket
                    HashPair<K,T>* temp = data[actualLocation+i-1];
                    data[actualLocation+i-1] = data[actualLocation+i];
                    data[actualLocation+i] = temp;
                }
            }

            if(found==true)
            {
                delete data[actualLocation + sizeOfBucket[location] - 1];
                data[actualLocation + sizeOfBucket[location] - 1] = nullptr;
                sizeOfBucket[location]--;
            }
        }
    }

    template<class K, class T>
    inline HashPair<K, T>* SimpleHashMap<K, T>::get(K key)
    {
        size_t location = hashFunction(key) % sizeOfBucket.size();
        HashPair<K, T>* value = nullptr;

        size_t actualLocation = location*maxBucketSize;
        for(int i=0; i<sizeOfBucket[location]; i++)
        {
            if(key == data[actualLocation+i]->key)
            {
                value = data[actualLocation+i];
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
            if(key == data[actualLocation+i]->key)
            {
                values.push_back( data[actualLocation+i] );
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
                outputData.push_back(data[bucketLocation+k]);
            }
        }

        return outputData;
    }

} //NAMESPACE glib END