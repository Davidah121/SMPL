#pragma once
#include <unordered_map>
#include "Concurrency.h"
#include "StringTools.h"

//TESTING FOR SAFE MEMORY MANAGEMENT
template<class T>
class StaticMemManager;

template<class T>
class SmartMemory;

class GenericSmartMemory;

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
        // smpl::StringTools::println("DELETE ALL DATA");
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
        if(p == nullptr)
            return;
        if(inDestructor)
            return;
            
        bool shouldDelete = false;
        bool isArray = false;
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_WRITE);
        //decrement the counter.
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            it->second.counter--;
            if(it->second.counter == 0)
            {
                if(it->second.shouldDelete == true)
                {
                    isArray = it->second.array;
                    pointerData.erase(p);
                    shouldDelete = true;
                }
            }
        }
        staticMemSemaphore.unlock();
        if(isArray)
            delete[] (T*)p;
        else
            delete (T*)p;
    }

    void forceDeletePointer(T* p)
    {
        if(p == nullptr)
            return;
        if(inDestructor)
            return;
        
        bool shouldDelete = false;
        bool isArray = false;
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_WRITE);
        //just remove it regardless of the counter or if it should be deleted.
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            isArray = it->second.array;
            pointerData.erase(p);
        }
        staticMemSemaphore.unlock();

        if(isArray)
            delete[] (T*)p;
        else
            delete (T*)p;
    }

    MemInfo addPointer(T* p, bool array, bool deleteOnLast, bool bypassOwnership)
    {
        if(p == nullptr)
            return {};

        MemInfo m;
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_WRITE);
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            it->second.counter++;
            m = it->second;
        }
        else
        {
            m = {array, deleteOnLast || bypassOwnership, 1};
            pointerData[p] = {array, deleteOnLast || bypassOwnership, 1};
        }
        staticMemSemaphore.unlock();
        return m;
    }

    T* duplicateExistingPointer(T* p)
    {
        if(p == nullptr)
            return nullptr;

        T* v = nullptr;
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_WRITE);
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            it->second.counter++;
            v = p;
        }
        staticMemSemaphore.unlock();
        return v;
    }

    bool pointerExists(T* p)
    {
        if(p == nullptr)
            return false;
        
        bool valid = false;
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_READ);
        auto it = pointerData.find(p);
        if(it != pointerData.end())
            valid = true;
        staticMemSemaphore.unlock();
        return valid;
    }
protected:
    friend SmartMemory<T>;
    std::unordered_map<T*, MemInfo> pointerData;
    smpl::HybridSpinSemaphore staticMemSemaphore;
private:
    bool inDestructor = false;
};

class GenericSmartMemory
{
public:
    GenericSmartMemory() {}
    virtual ~GenericSmartMemory() {}

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
     * @brief Gets the stored pointer.
     *      If it has been deleted, a nullptr will be returned.
     * 
     * @return void* 
     */
    void* getPointer()
    {
        if(isValid(hashValue))
            return hashValue;
        
        hashValue = nullptr;
        return nullptr;
    }

    /**
     * @brief Gets the Raw Pointer that is stored.
     *      Does not check if the pointer is valid.
     * 
     *      If the pointer at a previous point was discovered to be deleted,
     *      this will return a nullptr.
     * 
     * @return void* 
     */
    void* getRawPointer()
    {
        return hashValue;
    }

    bool const operator==(const GenericSmartMemory& other)
    {
        //should not matter if they both exist or not.
        return hashValue == other.hashValue;
    }

    bool const operator==(const void* p)
    {
        //should not matter if they both exist or not.
        return hashValue == p;
    }

    virtual bool isValid(void* p)
    {
        return false;
    };
protected:
    virtual void deletePointer() {};

    bool isArray = false;
    bool deleteRights = false;
    bool deleteOnLast = false;
    void* hashValue = nullptr;
};

template<typename T>
class SmartMemory : public GenericSmartMemory
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
        MemInfo m = memoryStuff.addPointer(data, array, deleteOnLast, bypassOwnership);
        hashValue = data;
        isArray = m.array;
        this->deleteOnLast = deleteOnLast;
        this->deleteRights = bypassOwnership;
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
     * @brief Move Constructor
     * 
     * @param other 
     */
    SmartMemory(SmartMemory<T>&& other) noexcept
    {
        deletePointer();

        isArray = other.isArray;
        deleteOnLast = other.deleteOnLast;
        deleteRights = other.deleteRights;
        hashValue = other.hashValue;

        other.hashValue = nullptr;
    }

    /**
     * @brief Move Assign a new SmartMemory object from another SmartMemory object.
     * 
     * @param other 
     */
    void operator=(SmartMemory<T>&& other) noexcept
    {
        deletePointer();

        isArray = other.isArray;
        deleteOnLast = other.deleteOnLast;
        deleteRights = other.deleteRights;
        hashValue = other.hashValue;

        other.hashValue = nullptr;
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
        hashValue = memoryStuff.duplicateExistingPointer((T*)other.hashValue);
        isArray = other.isArray;
        deleteOnLast = other.deleteOnLast;
        deleteRights = false;
    }

    /**
     * @brief Copy Assign a new SmartMemory object from another SmartMemory object.
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
        hashValue = memoryStuff.duplicateExistingPointer((T*)other.hashValue);
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
        if(memoryStuff.pointerExists((T*)hashValue))
            return (T*)hashValue;
        
        hashValue = nullptr;
        return nullptr;
    }
    
    T* getRawPointer()
    {
        return (T*)hashValue;
    }

    bool isValid(void* p)
    {
        return memoryStuff.pointerExists((T*)hashValue);
    }
    

protected:
    /**
     * @brief Deletes the pointer from the global memory list for the specified type.
     * 
     */
    void deletePointer()
    {
        smpl::StringTools::println("DELETING %p", hashValue);
        if(hashValue != nullptr)
        {
            if(deleteRights)
                memoryStuff.forceDeletePointer((T*)hashValue);
            else
                memoryStuff.deletePointer((T*)hashValue);
        }
        hashValue = nullptr;
    }

private:
    // static std::unordered_map<T*, MemInfo> globalListOfPointers;
    static StaticMemManager<T> memoryStuff;
};

template<typename T>
StaticMemManager<T> SmartMemory<T>::memoryStuff;
