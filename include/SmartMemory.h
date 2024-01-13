#pragma once
#include <unordered_map>
#include <mutex>
#include "StringTools.h"

//TESTING FOR SAFE MEMORY MANAGEMENT
template<class T>
class StaticMemManager;

template<class T>
class SmartMemory;

struct MemInfo
{
    bool array = false;
    bool shouldDelete = true;
    int counter = 0;
};

template<class T>
class StaticMemManager
{
public:
    StaticMemManager()
    {
    }

    ~StaticMemManager()
    {
        // glib::StringTools::println("DELETE ALL DATA");
        inDestructor = true;
        std::vector<std::pair<T*, bool>> pArr;
        for(auto it = pointerData.begin(); it != pointerData.end(); it++)
        {
            MemInfo info = it->second;
            if(info.shouldDelete == true)
                pArr.push_back({it->first, info.array});
        }

        for(std::pair<T*, bool>& memory : pArr)
        {
            if(memory.first != nullptr)
            {
                // StringTools::println("LATE DELETE: %p", memory.dataPointer);
                if(memory.second)
                    delete[] memory.first;
                else
                    delete memory.first;
            }
        }

        pointerData.clear();
        pArr.clear();
    }

    void deletePointer(T* p)
    {
        if(inDestructor)
            return;
            
        staticMemMutex.lock();
        //decrement the counter.
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            it->second.counter--;
            if(it->second.counter == 0)
            {
                if(it->second.shouldDelete == true)
                {
                    bool isArray = it->second.array;
                    pointerData.erase(p);
                    
                    staticMemMutex.unlock();
                    if(isArray)
                        delete[] (T*)p;
                    else
                        delete (T*)p;
                }
                else
                {
                    staticMemMutex.unlock();
                }
            }
            else
            {
                staticMemMutex.unlock();
            }
        }
        else
        {
            staticMemMutex.unlock();
        }
    }

    void forceDeletePointer(T* p)
    {
        if(inDestructor)
            return;
        
        staticMemMutex.lock();
        //just remove it regardless of the counter or if it should be deleted.
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            bool isArray = it->second.array;

            pointerData.erase(p);
            staticMemMutex.unlock();

            if(isArray)
                delete[] (T*)p;
            else
                delete (T*)p;
        }
        else
        { 
            staticMemMutex.unlock();
        }
    }

protected:
    friend SmartMemory<T>;
    std::unordered_map<T*, MemInfo> pointerData;
    std::mutex staticMemMutex;
private:
    bool inDestructor = false;
};

template<typename T>
class SmartMemory
{
public:

    /**
     * @brief Construct a new Empty SmartMemory object
     *      Holds a nullptr
     * 
     */
    SmartMemory()
    {
    }

    /**
     * @brief Construct a new SmartMemory object.
     *      Will contain the pointer to some data that is assumed to be valid.
     *      The pointer can not be a nullptr.
     * 
     *      If the pointer is stored by another SmartMemory object, the created SmartMemory object will not
     *      have delete rights.
     * 
     * @param data 
     *      A non zero pointer to some data.
     *      If zero, it is considered invalid.
     * @param array
     *      If set to true, the pointer is considered an array.
     *      This will affect how it is deleted.
     *      By default, it is false.
     * @param deleteOnLast
     *      If set to true, the created SmartMemory will change how data is deleted. It will only delete
     *      the pointer if it is the last known reference to it.
     *      By default, it is set true.
     * @param bypassOwnership 
     *      If set to true, the created SmartMemory object will always has delete rights.
     *      It will delete the pointer when the SmartMemory is destroyed.
     *      By default, it is false.
     */
    SmartMemory(T* data, bool array = false, bool deleteOnLast = true, bool bypassOwnership = false)
    {
        isArray = array;
        hashValue = data;
        this->deleteOnLast = deleteOnLast;

        if(hashValue != nullptr)
        {
            auto it = memoryStuff.pointerData.find(hashValue);
            if(it == memoryStuff.pointerData.end())
            {
                memoryStuff.pointerData[hashValue] = {array, deleteOnLast || bypassOwnership, 1};
            }
            else
            {
                isArray = it->second.array;
                it->second.counter += 1;
            }
        }

        if(bypassOwnership)
            deleteRights = true;
    }

    /**
     * @brief Shortcut function. Creates a SmartMemory such that the data will be deleted when the last
     *      known instance of this pointer exists. This functionality is passed to all copied instances.
     *      This behavior is similar to std::shared_pointer
     *      This is also the default behavior of this class when called with default options.
     * 
     * @param data 
     * @param array 
     * @return SmartMemory<T> 
     */
    static SmartMemory<T> createDeleteOnLast(T* data, bool array = false)
    {
        return SmartMemory(data, array, true, false);
    }

    /**
     * @brief Shortcut function. Creates a SmartMemory such that the data will be never be deleted.
     *      Good for local objects being passed around via pointers without worrying if it will be deleted improperly.
     *      This functionality is passed to all copied instances.
     * 
     * @param data 
     * @param array 
     * @return SmartMemory<T> 
     */
    static SmartMemory<T> createNoDelete(T* data, bool array = false)
    {
        return SmartMemory(data, array, false, false);
    }

    /**
     * @brief Shortcut function. Creates a SmartMemory such that the data will be whenever this object is deleted.
     *      This establishes a sort of ownership relationship with the object and the data.
     *      All other SmartMemory instances created from this one will not shared the same functionality but will not 
     *          be able to delete the pointer when they are destroyed.
     * 
     * @param data 
     * @param array 
     * @return SmartMemory<T> 
     */
    static SmartMemory<T> createDeleteRights(T* data, bool array = false)
    {
        return SmartMemory(data, array, false, true);
    }
    
    /**
     * @brief Construct a new SmartMemory object from another SmartMemory object.
     *      It will not have delete rights.
     *      If the object originally stored a pointer and had delete rights, it will delete its pointer.
     * 
     *      Equavelent to a shared_ptr<T>
     * 
     * @param other 
     */
    SmartMemory(const SmartMemory<T>& other)
    {
        deletePointer();
        if(other.hashValue != nullptr)
        {
            auto it = memoryStuff.pointerData.find(other.hashValue);
            if(it != memoryStuff.pointerData.end())
            {
                hashValue = other.hashValue;
                it->second.counter += 1;
            }
            else
                hashValue = nullptr;
        }
        isArray = other.isArray;
        deleteOnLast = other.deleteOnLast;
        deleteRights = false;
    }

    /**
     * @brief Construct a new SmartMemory object from another SmartMemory object.
     *      It will not have delete rights.
     *      If the object originally stored a pointer and had delete rights, it will delete its pointer.
     * 
     *      Equavelent to a shared_ptr<T>
     * 
     * @param other 
     */
    void operator=(const SmartMemory<T>& other)
    {
        deletePointer();
        if(other.hashValue != nullptr)
        {
            auto it = memoryStuff.pointerData.find(other.hashValue);
            if(it != memoryStuff.pointerData.end())
            {
                hashValue = other.hashValue;
                it->second.counter += 1;
            }
            else
                hashValue = nullptr;
        }
        isArray = other.isArray;
        deleteOnLast = other.deleteOnLast;
        deleteRights = false;
    }

    /**
     * @brief Destroy the SmartMemory object.
     *      If the object has delete rights, it will delete the pointer.
     */
    ~SmartMemory()
    {
        deletePointer();
    }

    /**
     * @brief Gets the stored pointer.
     *      If it has been deleted, a nullptr will be returned.
     * 
     * @return T* 
     */
    T* getPointer()
    {
        if(hashValue != nullptr)
        {
            auto it = memoryStuff.pointerData.find(hashValue);
            if(it != memoryStuff.pointerData.end())
            {
                return hashValue;
            }
            else
            {
                hashValue = nullptr;
            }
        }

        return nullptr;
    }

    /**
     * @brief Returns if the object has delete rights.
     *      If it has delete rights, the data pointer will be deleted when the object is destroyed.
     * 
     * @return true 
     * @return false 
     */
    bool getDeleteRights()
    {
        return deleteRights;
    }

    /**
     * @brief Returns in the object will delete the pointer if it is the last known holder of it.
     *      By default, most objects will have this property but if an object is not intended to be
     *          deleted at all, this should be false and delete rights should also be false.
     * 
     * @return true 
     * @return false 
     */
    bool getWillDeleteOnLast()
    {
        return deleteOnLast;
    }

    /**
     * @brief Returns if the object is an array.
     *      This is mostly for internal use for how to delete the object.
     * 
     * @return true 
     * @return false 
     */
    bool getIsArray()
    {
        return isArray;
    }

    /**
     * @brief Gets the Raw Pointer that is stored.
     *      Does not check if the pointer is valid.
     * 
     *      If the pointer at a previous point was discovered to be deleted,
     *      this will return a nullptr.
     * 
     * @return T* 
     */
    T* getRawPointer()
    {
        return hashValue;
    }

    bool const operator==(const SmartMemory<T>& other)
    {
        //should not matter if they both exist or not.
        return hashValue == other.hashValue;
    }

private:

    /**
     * @brief Deletes the pointer from the global memory list for the specified type.
     * 
     */
    void deletePointer()
    {
        if(hashValue != nullptr)
        {
            if(deleteRights)
            {
                memoryStuff.forceDeletePointer(hashValue);
            }
            else
            {
                memoryStuff.deletePointer(hashValue);
            }
        }
        hashValue = nullptr;
    }

    bool isArray = false;
    bool deleteRights = false;
    bool deleteOnLast = false;
    T* hashValue = nullptr;
    static StaticMemManager<T> memoryStuff;
};

template<typename T>
StaticMemManager<T> SmartMemory<T>::memoryStuff;
