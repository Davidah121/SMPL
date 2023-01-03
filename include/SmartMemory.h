#pragma once
#include <unordered_map>
#include "StringTools.h"

//TESTING FOR SAFE MEMORY MANAGEMENT

template<class T>
struct MemClassification
{
    T* dataPointer = nullptr;
    bool array = false;
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
        std::vector<MemClassification<T>> pArr;
        for(auto it = pointerData.begin(); it != pointerData.end(); it++)
        {
            pArr.push_back({it->first, it->second});
        }

        for(MemClassification<T>& memory : pArr)
        {
            if(memory.dataPointer != nullptr)
            {
                // StringTools::println("LATE DELETE: %p", memory.dataPointer);
                if(memory.array)
                    delete[] memory.dataPointer;
                else
                    delete memory.dataPointer;
            }
        }

        pointerData.clear();
        pArr.clear();
    }

    std::unordered_map<T*, bool> pointerData;
private:
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
     * @param bypassOwnership 
     *      If set to true, the created SmartMemory object will always has delete rights.
     *      It will delete the pointer when the SmartMemory is destroyed.
     *      By default, it is false.
     */
    SmartMemory(T* data, bool array = false, bool bypassOwnership = false)
    {
        hashValue = data;

        if(hashValue != nullptr)
        {
            auto it = memoryStuff.pointerData.find(hashValue);
            if(it == memoryStuff.pointerData.end())
            {
                deleteRights = true;
                memoryStuff.pointerData.insert( {hashValue, array} );
                // glib::StringTools::println("Added %p", hashValue);
            }
            else
            {
                deleteRights = false;
                it->second = array;
                // glib::StringTools::println("Copied %p", hashValue);
            }
        }

        if(bypassOwnership)
            deleteRights = true;
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
        hashValue = other.hashValue;
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
        hashValue = other.hashValue;
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
     * @brief Removes the pointer from the memory list without deleting it if it has delete rights.
     *      Useful if the data has been deleted outside of the SmartMemory scope.
     *      
     *      An example of such is an object created on the stack and the reference being stored in
     *      a SmartMemory object. 
     * 
     *      Special care should be taken to ensure that data is not deleted twice or not delete at all.
     */
    void unsafeRemove()
    {
        if(hashValue != nullptr && deleteRights)
        {
            memoryStuff.pointerData.erase(hashValue);
            hashValue = nullptr;
        }
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

private:

    /**
     * @brief Deletes the pointer from the global memory list for the specified type.
     * 
     */
    void deletePointer()
    {
        if(deleteRights && hashValue != nullptr)
        {
            auto it = memoryStuff.pointerData.find(hashValue);
            if(it != memoryStuff.pointerData.end())
            {
                // glib::StringTools::println("Delete hashValue %p", hashValue);

                if(it->second == true)
                    delete[] (T*)hashValue;
                else
                    delete (T*)hashValue;
                
                memoryStuff.pointerData.erase(hashValue);
            }
        }
        else
        {
            // glib::StringTools::println("Soft Delete");
        }
        hashValue = nullptr;
    }

    bool deleteRights = false;
    T* hashValue = nullptr;
    static StaticMemManager<T> memoryStuff;
};

template<typename T>
StaticMemManager<T> SmartMemory<T>::memoryStuff;
