#pragma once

#include <vector>
#include <cmath>

namespace smpl
{
    template<typename K, typename T>
    class SimpleHashMap
    {
    public:
        static const bool MODE_KEEP_ALL = false;
        static const bool MODE_UNIQUE_KEY = true;

        /**
         * @brief Construct a new SimpleHashMap object
         *      SimpleHashMap is designed to be simpler and faster to delete than the std::unordered_multimap.
         *      A mode exists for unique keys but std::unordered_map is faster in every way currently
         * 
         * @param mode 
         *      Valid modes are:
         *          MODE_UNIQUE_KEY
         *          MODE_KEEP_ALL
         *      Unique Key will replace the data with the same key.
         *      Keep All keeps all data added to the hash map.
         *      Default value is MODE_KEEP_ALL
         * @param initBuckets 
         *      The initial amount of buckets
         *      Default value is 0x1000 = 4096
         */
        SimpleHashMap(bool mode = MODE_KEEP_ALL, size_t initBucketCount = 0x1000);

        /**
         * @brief Copy Constructor.
         *      Makes new copy from another SimpleHashMap
         * 
         * @param other 
         */
        SimpleHashMap(const SimpleHashMap<K, T>& other);

        /**
         * @brief Copies a SimpleHashMap object
         * 
         * @param other 
         */
        void operator=(const SimpleHashMap<K, T>& other);

        /**
         * @brief Move Constructor.
         *      Moves a SimpleHashMap to another
         * 
         * @param other 
         */
        SimpleHashMap(SimpleHashMap<K, T>&& other) noexcept;

        /**
         * @brief Moves a SimpleHashMap to another
         * 
         * @param other 
         */
        void operator=(SimpleHashMap<K, T>&& other) noexcept;
        

        /**
         * @brief Destroy the SimpleHashMap object
         * 
         */
        ~SimpleHashMap();

        /**
         * @brief Adds a new data entry into the SimpleHashMap based on the mode of the hashmap
         * 
         * @param key 
         * @param value 
         */
        void add(K key, T data);

        /**
         * @brief Removes the std::pair entry.
         * 
         * @param pair 
         * @return true 
         * @return false 
         */
        bool remove(std::pair<K,T>* pair);

        /**
         * @brief Removes the std::pair entry with the specified key and data.
         * 
         * @param key 
         * @param data 
         * @return true 
         * @return false 
         */
        bool remove(K key, T data);

        /**
         * @brief Removes the first entry found for the key.
         *      Returns if successful.
         * 
         *      All std::pair references to the removed data will be invalid as the reference will be deleted.
         * 
         * @param key 
         * @return bool
         */
        bool remove(K key);

        /**
         * @brief Removes all entries found for the key.
         *      Returns if successful.
         * 
         *      All std::pair references to the removed data will be invalid as the reference will be deleted.
         * 
         * @param key 
         * @return bool
         */
        bool removeAll(K key);
        
        /**
         * @brief Gets the first entry with the specified key.
         * 
         * @param key 
         * @return std::pair<K, T>* 
         */
        std::pair<K,T>* get(K key);

        /**
         * @brief Gets the next entry using the previous key.
         *      get() must have been called before.
         * 
         * @param key 
         * @return std::pair<K, T>* 
         */
        std::pair<K,T>* getNext();

        /**
         * @brief Gets all entries with the specified key.
         * 
         * @param key 
         * @return std::vector<std::pair<K, T>*> 
         */
        std::vector<std::pair<K,T>*> getAll(K key);

        /**
         * @brief Gets a selection of entries with the specified key.
         *      The maximum number of entries grabbed will limited by count.
         * 
         * @param key 
         * @return std::vector<std::pair<K, T>*> 
         */
        std::vector<std::pair<K,T>*> getAllCount(K key, int count);

        /**
         * @brief Gets the things that the hashmap stores.
         *      Not intended for raw modification but reading only.
         *      Modifying of key data may cause failures to find data without doing a rehash.
         * 
         * @return std::vector<std::vector<std::pair<K,T>*>>&
         */
        std::vector<std::vector<std::pair<K,T>*>>& getRawData();

        /**
         * @brief Rehashes the hash map.
         *      Can help performance if the data has been reorganized.
         *      Automatically happens as data is added if the max load factor has been exceeded.
         * 
         *      Rehashing is a O(N) operation and requires O(N) space however,
         *          It specifically requires 2-3x more memory. This causes a spike in
         *          memory usage that drops after rehashing is done. This may be undersired.
         * 
         *      Automatic Rehashing can be disabled.
         */
        void rehash();

        /**
         * @brief Clears the SimpleHashMap
         * 
         */
        void clear();

        /**
         * @brief Returns the amount of buckets created in the hash map
         * 
         * @return size_t 
         */
        size_t amountOfBuckets();

        /**
         * @brief Returns the amount of entries
         * 
         * @return size_t 
         */
        size_t size();

        /**
         * @brief Sets the Max Load Factor for the hash map.
         *      If the load factor exceeds the max load factor, it causes a rehash.
         *      load factor = size/buckets
         * 
         *      By default, it is set to 0.75
         * @param f 
         */
        void setMaxLoadFactor(double f);

        /**
         * @brief Gets the Max Load Factor for the hash map.
         * 
         * @return double 
         */
        double getMaxLoadFactor();

        /**
         * @brief Gets the Current Load Factor.
         *      The load factor is the size / buckets*MAX_BUCKET_LOAD
         *      MAX_BUCKET_LOAD = 24
         * 
         * @return double 
         */
        double getCurrentLoadFactor();

        /**
         * @brief Returns whether rehashing is allowed.
         * 
         * @return true 
         * @return false 
         */
        bool getRehashAllowed();
        
        /**
         * @brief Sets whether rehashing is allowed.
         *      This maybe turned off for performance reasons.
         * 
         * @param v 
         */
        void setRehashAllowed(bool v);

    private:
        static const int MAX_BUCKET_LOAD = 24;
        std::vector<std::vector<std::pair<K,T>*>> buckets;
        size_t elements = 0;
        double maxLoadFactor = 0.75;
        bool mode = MODE_KEEP_ALL;
        std::hash<K> hasher;
        size_t initialBucketSize = 512;
        bool rehashAllowed = true;

        size_t offset = -1;
        K lastKey;
    };

    template<typename K, typename T>
    inline SimpleHashMap<K, T>::SimpleHashMap(bool mode, size_t initBuckets)
    {
        initialBucketSize = initBuckets;
        buckets = std::vector<std::vector<std::pair<K,T>*>>(initBuckets);
        this->mode = mode;
    }

    template<class K, class T>
    inline SimpleHashMap<K, T>::SimpleHashMap(const SimpleHashMap<K, T>& other)
    {
        mode = other.mode;
        elements = other.elements;
        maxLoadFactor = other.maxLoadFactor;
        
        buckets = std::vector< std::vector<std::pair<K, T>*> >(other.buckets.size());
        for(size_t bucketLocation=0; bucketLocation<other.buckets.size(); bucketLocation++)
        {
            buckets[bucketLocation].reserve( other.buckets[bucketLocation].size() );
            for(size_t k=0; k<other.buckets[bucketLocation].size(); k++)
            {
                buckets[bucketLocation].push_back(new std::pair<K, T>());
                buckets[bucketLocation].back()->first = other.buckets[bucketLocation][k]->first;
                buckets[bucketLocation].back()->second = other.buckets[bucketLocation][k]->second;
            }
        }
        
        initialBucketSize = other.initialBucketSize;
        offset = other.offset;
        lastKey = other.lastKey;
    }

    template<class K, class T>
    inline void SimpleHashMap<K, T>::operator=(const SimpleHashMap<K, T>& other)
    {
        mode = other.mode;
        elements = other.elements;
        maxLoadFactor = other.maxLoadFactor;
        
        buckets = std::vector< std::vector<std::pair<K, T>*> >(other.buckets.size());
        for(size_t bucketLocation=0; bucketLocation<other.buckets.size(); bucketLocation++)
        {
            buckets[bucketLocation].reserve( other.buckets[bucketLocation].size() );
            for(size_t k=0; k<other.buckets[bucketLocation].size(); k++)
            {
                buckets[bucketLocation].push_back(new std::pair<K, T>());
                buckets[bucketLocation].back() = other.buckets[bucketLocation][k];
            }
        }
        
        initialBucketSize = other.initialBucketSize;
        offset = other.offset;
        lastKey = other.lastKey;
    }

    
    template<class K, class T>
    inline SimpleHashMap<K, T>::SimpleHashMap(SimpleHashMap<K, T>&& other) noexcept
    {
        mode = other.mode;
        elements = other.elements;
        maxLoadFactor = other.maxLoadFactor;
        buckets = std::move(other.buckets);

        initialBucketSize = other.initialBucketSize;
        offset = other.offset;
        lastKey = other.lastKey;
    }

    template<class K, class T>
    inline void SimpleHashMap<K, T>::operator=(SimpleHashMap<K, T>&& other) noexcept
    {
        mode = other.mode;
        elements = other.elements;
        maxLoadFactor = other.maxLoadFactor;
        buckets = std::move(other.buckets);

        initialBucketSize = other.initialBucketSize;
        offset = other.offset;
        lastKey = other.lastKey;
    }

    template<typename K, typename T>
    inline SimpleHashMap<K, T>::~SimpleHashMap()
    {
        clear();
    }

    template<typename K, typename T>
    inline void SimpleHashMap<K, T>::add(K key, T data)
    {
        size_t bucketLocation = hasher(key) % buckets.size();
        size_t existingLocation = SIZE_MAX;
        if(mode == MODE_UNIQUE_KEY)
        {
            for(size_t i=0; i<buckets[bucketLocation].size(); i++)
            {
                if(buckets[bucketLocation][i]->first == key)
                {
                    buckets[bucketLocation][i]->second = data;
                    return;
                }
            }
        }

        buckets[bucketLocation].push_back( new std::pair<K, T>{key, data});
        elements++;
        if(rehashAllowed)
        {
            double load = (double)elements / (buckets.size()*MAX_BUCKET_LOAD);
            if(load >= maxLoadFactor)
                rehash();
        }
    }

    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::remove(std::pair<K,T>* pair)
    {
        if(pair != nullptr)
        {
            size_t bucketLocation = hasher(pair->first) % buckets.size();
            std::vector<std::pair<K, T>*>& bucket = buckets[bucketLocation];

            for(size_t i=0; i<bucket.size(); i++)
            {
                if(bucket[i] == pair)
                {
                    //delete
                    delete bucket[i];
                    bucket[i] = bucket[bucket.size() - 1];
                    bucket.pop_back();
                    elements--;
                    return true;
                }
            }
        }
        return false;
    }

    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::remove(K key, T data)
    {
        size_t bucketLocation = hasher(key) % buckets.size();
        std::vector<std::pair<K, T>*>& bucket = buckets[bucketLocation];

        for(size_t i=0; i<bucket.size(); i++)
        {
            if(bucket[i]->first == key && bucket[i]->second == data)
            {
                //delete
                delete bucket[i];
                bucket[i] = bucket[bucket.size() - 1];
                bucket.pop_back();
                elements--;
                return true;
            }
        }
    
        return false;
    }

    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::remove(K key)
    {
        size_t bucketLocation = hasher(key) % buckets.size();
        std::vector<std::pair<K, T>*>& bucket = buckets[bucketLocation];

        for(size_t i=0; i<bucket.size(); i++)
        {
            if(bucket[i]->first == key)
            {
                //delete
                delete bucket[i];
                bucket[i] = bucket[bucket.size() - 1];
                bucket.pop_back();
                elements--;
                return true;
            }
        }
    
        return false;
    }

    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::removeAll(K key)
    {
        size_t bucketLocation = hasher(key) % buckets.size();
        std::vector<std::pair<K, T>*>& bucket = buckets[bucketLocation];
        size_t origSize = bucket.size();

        for(size_t i=bucket.size()-1; i!=-1; i--)
        {
            if(bucket[i]->first == key)
            {
                //delete
                delete bucket[i];
                bucket[i] = bucket[bucket.size() - 1];
                bucket.pop_back();
                elements--;
            }
        }
    
        return elements != origSize;
    }

    template<typename K, typename T>
    inline std::pair<K,T>* SimpleHashMap<K, T>::get(K key)
    {
        size_t bucketLocation = hasher(key) % buckets.size();
        std::vector<std::pair<K, T>*>& bucket = buckets[bucketLocation];
        
        for(size_t i=bucket.size()-1; i!=-1; i--)
        {
            if(bucket[i]->first == key)
            {
                offset = i-1;
                lastKey = key;
                return bucket[i];
            }
        }
        offset = SIZE_MAX;
        return nullptr;
    }

    template<typename K, typename T>
    inline std::pair<K,T>* SimpleHashMap<K, T>::getNext()
    {
        if(offset != SIZE_MAX)
        {
            size_t bucketLocation = hasher(lastKey) % buckets.size();
            std::vector<std::pair<K, T>*>& bucket = buckets[bucketLocation];

            for(size_t i=offset; i!=-1; i--)
            {
                if(bucket[i]->first == lastKey)
                {
                    offset = i-1;
                    return bucket[i];
                }
            }
            offset = SIZE_MAX;
        }
        return nullptr;
    }
        
    template<typename K, typename T>
    inline std::vector<std::vector<std::pair<K,T>*>>& SimpleHashMap<K, T>::getRawData()
    {
        return buckets;
    }

    template<typename K, typename T>
    inline std::vector<std::pair<K,T>*> SimpleHashMap<K, T>::getAll(K key)
    {
        if(buckets.size() == 0)
            return {};
        
        size_t bucketLocation = hasher(key) % buckets.size();
        std::vector<std::pair<K,T>*> collection;

        for(size_t i=0; i<buckets[bucketLocation].size(); i++)
        {
            if(buckets[bucketLocation][i]->first == key)
                collection.push_back(buckets[bucketLocation][i]);
        }

        return collection;
    }

    template<typename K, typename T>
    inline void SimpleHashMap<K, T>::rehash()
    {
        std::vector<std::vector<std::pair<K, T>*>> newBuckets = std::vector<std::vector<std::pair<K, T>*>>(buckets.size()*2);
        for(size_t i=0; i<newBuckets.size(); i++)
        {
            newBuckets[i].reserve(MAX_BUCKET_LOAD/2);
        }

        for(size_t bucketLocation=0; bucketLocation<buckets.size(); bucketLocation++)
        {
            std::vector<std::pair<K, T>*>& currBucket = buckets[bucketLocation];
            for(size_t j=0; j<currBucket.size(); j++)
            {
                size_t newLocation = hasher(currBucket[j]->first) % newBuckets.size();
                newBuckets[newLocation].push_back(currBucket[j]);
            }
        }
        buckets = std::move(newBuckets);
    }


    template<typename K, typename T>
    inline void SimpleHashMap<K, T>::clear()
    {
        for(size_t i=0; i<buckets.size(); i++)
        {
            for(size_t j=0; j<buckets[i].size(); j++)
            {
                if(buckets[i][j] == nullptr)
                {
                    delete buckets[i][j];
                }
            }
            buckets[i].clear();
        }
        elements = 0;
    }

    template<typename K, typename T>
    inline size_t SimpleHashMap<K, T>::amountOfBuckets()
    {
        return buckets.size();
    }

    template<typename K, typename T>
    inline size_t SimpleHashMap<K, T>::size()
    {
        return elements;
    }

    template<typename K, typename T>
    inline void SimpleHashMap<K, T>::setMaxLoadFactor(double f)
    {
        maxLoadFactor = f;
    }

    template<typename K, typename T>
    inline double SimpleHashMap<K, T>::getMaxLoadFactor()
    {
        return maxLoadFactor;
    }

    template<typename K, typename T>
    inline double SimpleHashMap<K, T>::getCurrentLoadFactor()
    {
        if(buckets.size() == 0)
            return 0;
        return (double)elements / (buckets.size()*MAX_BUCKET_LOAD);
    }
    
    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::getRehashAllowed()
    {
        return rehashAllowed;
    }

    template<typename K, typename T>
    inline void SimpleHashMap<K, T>::setRehashAllowed(bool v)
    {
        rehashAllowed = v;
    }
} //NAMESPACE glib END