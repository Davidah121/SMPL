#pragma once

#include <vector>
#include "System.h"

namespace glib
{
    template<typename K, typename T>
    struct HashPair
    {
        K key;
        T data;
    };

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
         *      The amount of buckets must be a power of 2. 
         *          Will be rounded up to a power of 2.
         *      Default value is 16384
         */
        SimpleHashMap(bool mode = MODE_KEEP_ALL, size_t initBucketCount = 16384);

        /**
         * @brief Construct a new SimpleHashMap object from another SimpleHashMap
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
         * @brief Removes the HashPair entry.
         * 
         * @param pair 
         * @return true 
         * @return false 
         */
        bool remove(HashPair<K,T>* pair);

        /**
         * @brief Removes the HashPair entry with the specified key and data.
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
         *      All HashPair references to the removed data will be invalid as the reference will be deleted.
         * 
         * @param key 
         * @return bool
         */
        bool removeFirst(K key);

        /**
         * @brief Removes the last entry found for the key.
         *      Returns if successful.
         * 
         *      All HashPair references to the removed data will be invalid as the reference will be deleted.
         * 
         * @param key 
         * @return bool
         */
        bool removeLast(K key);

        /**
         * @brief Removes all entries found for the key.
         *      Returns if successful.
         * 
         *      All HashPair references to the removed data will be invalid as the reference will be deleted.
         * 
         * @param key 
         * @return bool
         */
        bool removeAll(K key);
        
        /**
         * @brief Gets the first entry with the specified key.
         * 
         * @param key 
         * @return HashPair<K, T>* 
         */
        HashPair<K,T>* get(K key);

        /**
         * @brief Gets the next entry using the previous key.
         *      get() must have been called before.
         * 
         * @param key 
         * @return HashPair<K, T>* 
         */
        HashPair<K,T>* getNext();

        /**
         * @brief Gets all entries with the specified key.
         * 
         * @param key 
         * @return std::vector<HashPair<K, T>*> 
         */
        std::vector<HashPair<K,T>*> getAll(K key);

        /**
         * @brief Gets a selection of entries with the specified key.
         *      The maximum number of entries grabbed will limited by count.
         * 
         * @param key 
         * @return std::vector<HashPair<K, T>*> 
         */
        std::vector<HashPair<K,T>*> getAllCount(K key, int count);

        /**
         * @brief Rehashes the hash map.
         *      Can help performance if the data has been reorganized.
         *      Automatically happens as data is added if the max load factor has been exceeded.
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
        size_t getSize();

        /**
         * @brief Sets the Max Load Factor for the hash map.
         *      If the load factor exceeds the max load factor, it causes a rehash.
         *      load factor = size/buckets
         * 
         *      By default, it is set to 10
         * 
         *      Note that if set to a value less than 0, no rehash will happen unless
         *      explicitly requested
         * @param f 
         */
        void setMaxLoadFactor(double f);

        /**
         * @brief Gets the Max Load Factor for the hash map.
         * 
         * @return double 
         */
        double getMaxLoadFactor();

    private:
        std::vector<std::vector<HashPair<K,T>*>> buckets;
        size_t size = 0;
        double maxLoadFactor = 10;
        bool mode = MODE_KEEP_ALL;
        std::hash<K> hasher;

        size_t offset = -1;
        K lastKey;
    };

    template<typename K, typename T>
    inline SimpleHashMap<K, T>::SimpleHashMap(bool mode, size_t initBuckets)
    {
        size_t actualBucketCount =  1 << (int)ceil(log2(initBuckets));
        buckets = std::vector<std::vector<HashPair<K,T>*>>(actualBucketCount);
        this->mode = mode;
    }

    template<class K, class T>
    inline SimpleHashMap<K, T>::SimpleHashMap(const SimpleHashMap<K, T>& other)
    {
        mode = other.mode;
        size = other.size;
        maxLoadFactor = other.maxLoadFactor;
        
        buckets = std::vector< std::vector<HashPair<K, T>*> >(other.buckets.size());

        for(int bucketLocation=0; bucketLocation<other.buckets.size(); bucketLocation++)
        {
            buckets[bucketLocation].reserve( other.buckets[bucketLocation].size() );
            for(int k=0; k<other.buckets[bucketLocation].size(); k++)
            {
                buckets[bucketLocation].push_back(new HashPair<K, T>());
                buckets[bucketLocation].back()->key = other.buckets[bucketLocation][k]->key;
                buckets[bucketLocation].back()->data = other.buckets[bucketLocation][k]->data;
            }
        }
    }

    template<class K, class T>
    inline void SimpleHashMap<K, T>::operator=(const SimpleHashMap<K, T>& other)
    {
        mode = other.mode;
        size = other.size;
        maxLoadFactor = other.maxLoadFactor;
        
        buckets = std::vector< std::vector<HashPair<K, T>*> >(other.buckets.size());

        for(int bucketLocation=0; bucketLocation<other.buckets.size(); bucketLocation++)
        {
            buckets[bucketLocation].reserve( other.buckets[bucketLocation].size() );
            for(int k=0; k<other.buckets[bucketLocation].size(); k++)
            {
                buckets[bucketLocation].push_back(new HashPair<K, T>());
                buckets[bucketLocation].back()->key = other.buckets[bucketLocation][k]->key;
                buckets[bucketLocation].back()->data = other.buckets[bucketLocation][k]->data;
            }
        }
    }

    template<typename K, typename T>
    inline SimpleHashMap<K, T>::~SimpleHashMap()
    {
        clear();
    }

    template<typename K, typename T>
    inline void SimpleHashMap<K, T>::add(K key, T data)
    {
        size_t t1 = System::getCurrentTimeMicro();

        size_t bucketLocation = hasher(key) % buckets.size();
        HashPair<K,T>* pair = new HashPair<K,T>();
        pair->key = key;
        pair->data = data;

        if(mode == MODE_KEEP_ALL)
        {
            buckets[bucketLocation].push_back( pair );
            this->size++;
        }
        else if(mode == MODE_UNIQUE_KEY)
        {
            //find if the key already exists
            bool found = false;
            for(int i=0; i<buckets[bucketLocation].size(); i++)
            {
                if(buckets[bucketLocation][i]->key == key)
                {
                    buckets[bucketLocation][i]->data = data;
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                buckets[bucketLocation].push_back( pair );
                this->size++;
            }
        }

        size_t t2 = System::getCurrentTimeMicro();
        // System::dbtime[0] += t2-t1;

        if(maxLoadFactor >= 0)
        {
            double loadFactor = (double)this->size / buckets.size();
            if(loadFactor > maxLoadFactor)
            {
                rehash();
            }
        }
    }

    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::remove(HashPair<K,T>* pair)
    {
        size_t t1 = System::getCurrentTimeMicro();

        int indexOfKey = -1;
        if(pair != nullptr)
        {
            size_t bucketLocation = hasher(pair->key) % buckets.size();

            for(int i=0; i<buckets[bucketLocation].size(); i++)
            {
                if(buckets[bucketLocation][i] == pair)
                {
                    indexOfKey = i;
                    delete pair;
                    break;
                }
            }

            if(indexOfKey != -1)
            {
                for(int i=indexOfKey; i<buckets[bucketLocation].size()-1; i++)
                {
                    HashPair<K,T>* temp = buckets[bucketLocation][i];
                    buckets[bucketLocation][i] = buckets[bucketLocation][i+1];
                    buckets[bucketLocation][i+1] = temp;
                }
                buckets[bucketLocation].pop_back();
                this->size--;
            }
        }

        size_t t2 = System::getCurrentTimeMicro();
        // System::dbtime[3] += t2-t1;

        return indexOfKey != -1;
    }

    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::remove(K key, T data)
    {
        size_t t1 = System::getCurrentTimeMicro();

        size_t bucketLocation = hasher(key) % buckets.size();

        int indexOfKey = -1;
        for(int i=0; i<buckets[bucketLocation].size(); i++)
        {
            if(buckets[bucketLocation][i]->data == data)
            {
                indexOfKey = i;
                delete buckets[bucketLocation][i];
                break;
            }
        }

        if(indexOfKey != -1)
        {
            for(int i=indexOfKey; i<buckets[bucketLocation].size()-1; i++)
            {
                HashPair<K,T>* temp = buckets[bucketLocation][i];
                buckets[bucketLocation][i] = buckets[bucketLocation][i+1];
                buckets[bucketLocation][i+1] = temp;
            }
            buckets[bucketLocation].pop_back();
            this->size--;
        }

        
        size_t t2 = System::getCurrentTimeMicro();
        // System::dbtime[3] += t2-t1;


        return indexOfKey != -1;
    }

    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::removeFirst(K key)
    {
        size_t bucketLocation = hasher(key) % buckets.size();

        int indexOfKey = -1;
        for(int i=buckets[bucketLocation].size()-1; i>=0; i--)
        {
            HashPair<K,T>* pair = buckets[bucketLocation][i];
            if(pair->key == key)
            {
                indexOfKey = i;
                delete pair;
                break;
            }
        }

        if(indexOfKey != -1)
        {
            for(int i=indexOfKey; i<buckets[bucketLocation].size()-1; i++)
            {
                HashPair<K,T>* temp = buckets[bucketLocation][i];
                buckets[bucketLocation][i] = buckets[bucketLocation][i+1];
                buckets[bucketLocation][i+1] = temp;
            }
            buckets[bucketLocation].pop_back();
            this->size--;
            return true;
        }
        return false;
    }

    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::removeLast(K key)
    {
        size_t bucketLocation = hasher(key) % buckets.size();

        int indexOfKey = -1;
        for(int i=0; i<buckets[bucketLocation].size(); i++)
        {
            HashPair<K,T>* pair = buckets[bucketLocation][i];
            if(pair.key == key)
            {
                indexOfKey = i;
                delete pair;
                break;
            }
        }

        if(indexOfKey != -1)
        {
            for(int i=indexOfKey; i<buckets[bucketLocation].size()-1; i++)
            {
                HashPair<K,T>* temp = buckets[bucketLocation][i];
                buckets[bucketLocation][i] = buckets[bucketLocation][i+1];
                buckets[bucketLocation][i+1] = temp;
            }
            buckets[bucketLocation].pop_back();
            this->size--;
            return true;
        }
        return false;
    }

    template<typename K, typename T>
    inline bool SimpleHashMap<K, T>::removeAll(K key)
    {
        size_t bucketLocation = hasher(key) % buckets.size();

        int count = 0;
        for(int i=0; i<buckets[bucketLocation].size(); i++)
        {
            HashPair<K,T>* pair = buckets[bucketLocation][i];
            if(pair.key == key)
            {
                delete pair;
                buckets[bucketLocation][i] = nullptr;
                count++;
            }
        }

        if(count > 0)
        {
            size_t totalSize = buckets[bucketLocation].size();

            for(int i=totalSize-1; i>=0; i--)
            {
                if(buckets[bucketLocation][i] == nullptr)
                {
                    for(int j=i; j<totalSize-1; j++)
                    {
                        HashPair<K,T>* temp = buckets[bucketLocation][j];
                        buckets[bucketLocation][j] = buckets[bucketLocation][j+1];
                        buckets[bucketLocation][j+1] = temp;
                    }
                }
                
                buckets[bucketLocation].pop_back();
                totalSize--;
                this->size--;
            }
            return true;
        }
        return false;
    }

    template<typename K, typename T>
    inline HashPair<K,T>* SimpleHashMap<K, T>::get(K key)
    {
        size_t t1 = System::getCurrentTimeMicro();
        size_t bucketLocation = hasher(key) % buckets.size();
        HashPair<K,T>* collection = nullptr;

        for(int i=buckets[bucketLocation].size()-1; i>=0; i--)
        {
            if(buckets[bucketLocation][i]->key == key)
            {
                collection = buckets[bucketLocation][i];
                offset = i-1;
                lastKey = key;
                break;
            }
        }
        size_t t2 = System::getCurrentTimeMicro();
        // System::dbtime[2] += t2-t1;
        return collection;
    }

    template<typename K, typename T>
    inline HashPair<K,T>* SimpleHashMap<K, T>::getNext()
    {
        size_t t1 = System::getCurrentTimeMicro();
        HashPair<K,T>* collection = nullptr;
        
        if(offset >= 0)
        {
            size_t bucketLocation = hasher(lastKey) % buckets.size();

            for(int i=offset; i>=0; i--)
            {
                if(buckets[bucketLocation][i]->key == lastKey)
                {
                    collection = buckets[bucketLocation][i];
                    offset = i-1;
                    break;
                }
            }
        }

        size_t t2 = System::getCurrentTimeMicro();
        // System::dbtime[2] += t2-t1;
        return collection;
    }

    template<typename K, typename T>
    inline std::vector<HashPair<K,T>*> SimpleHashMap<K, T>::getAll(K key)
    {

        size_t bucketLocation = hasher(key) % buckets.size();
        std::vector<HashPair<K,T>*> collection;

        for(int i=0; i<buckets[bucketLocation].size(); i++)
        {
            if(buckets[bucketLocation][i]->key == key)
                collection.push_back(buckets[bucketLocation][i]);
        }


        return collection;
    }

    template<typename K, typename T>
    inline std::vector<HashPair<K,T>*> SimpleHashMap<K, T>::getAllCount(K key, int count)
    {
        size_t t1 = System::getCurrentTimeMicro();

        size_t bucketLocation = hasher(key) % buckets.size();
        std::vector<HashPair<K,T>*> collection;
        int currCount = 0;

        for(int i=buckets[bucketLocation].size()-1; i>=0; i--)
        {
            if(buckets[bucketLocation][i]->key == key)
            {
                collection.push_back(buckets[bucketLocation][i]);
                currCount++;
                if(currCount >= count)
                    break;
            }
        }

        size_t t2 = System::getCurrentTimeMicro();
        // System::dbtime[2] += t2-t1;

        return collection;
    }

    template<typename K, typename T>
    inline void SimpleHashMap<K, T>::rehash()
    {
        size_t t1 = System::getCurrentTimeMicro();

        size_t oldSize = buckets.size();
        buckets.resize(oldSize*2);

        for(int bucketLocation=0; bucketLocation<oldSize; bucketLocation++)
        {
            if(buckets[bucketLocation].size()>0)
                buckets[bucketLocation+oldSize].reserve( buckets[bucketLocation].size() );

            int count = 0;

            for(int j=0; j<buckets[bucketLocation].size(); j++)
            {
                size_t newLocation = hasher(buckets[bucketLocation][j]->key) % buckets.size();
                
                if(newLocation != bucketLocation)
                {
                    buckets[newLocation].push_back(buckets[bucketLocation][j]);
                    buckets[bucketLocation][j] = nullptr;
                    count++;
                }
            }

            if(count > 0)
            {
                size_t totalSize = buckets[bucketLocation].size();

                for(int i=totalSize-1; i>=0; i--)
                {
                    if(buckets[bucketLocation][i] == nullptr)
                    {
                        for(int j=i; j<totalSize-1; j++)
                        {
                            HashPair<K,T>* temp = buckets[bucketLocation][j];
                            buckets[bucketLocation][j] = buckets[bucketLocation][j+1];
                            buckets[bucketLocation][j+1] = temp;
                        }
                    }
                    
                    buckets[bucketLocation].pop_back();
                    totalSize--;
                }
            }
        }
        size_t t2 = System::getCurrentTimeMicro();
        // System::dbtime[1] += t2-t1;
    }


    template<typename K, typename T>
    inline void SimpleHashMap<K, T>::clear()
    {
        size_t t1 = System::getCurrentTimeMicro();
        for(int i=0; i<buckets.size(); i++)
        {
            for(int j=0; j<buckets[i].size(); j++)
            {
                delete buckets[i][j];
            }
        }
        buckets.clear();
        size_t t2 = System::getCurrentTimeMicro();
        // System::dbtime[4] += t2-t1;
    }

    template<typename K, typename T>
    inline size_t SimpleHashMap<K, T>::amountOfBuckets()
    {
        return buckets.size();
    }

    template<typename K, typename T>
    inline size_t SimpleHashMap<K, T>::getSize()
    {
        return size;
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
} //NAMESPACE glib END