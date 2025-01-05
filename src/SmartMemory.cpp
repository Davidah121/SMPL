#include "SmartMemory.h"

namespace smpl
{
    std::unordered_map<void*, MemInfo> StaticMemManager::pointerData;
    smpl::HybridSpinSemaphore StaticMemManager::staticMemSemaphore;

    MemInfo StaticMemManager::addPointer(void* p, bool isArray, size_t sizeInBytes, bool deleteOnLast, bool bypassOwnership, void (*deleteFunc)(void*, bool))
    {
        if(p == nullptr)
            return {};

        MemInfo m;
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_WRITE);
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            //never change the delete function once set
            it->second.counter++;
            m = it->second;
        }
        else
        {
            m.array = isArray;
            m.sizeInBytes = sizeInBytes;
            m.forceDelete = false;
            m.counter = 1;
            m.shouldDelete = deleteOnLast || bypassOwnership;
            m.deleteFunc = deleteFunc;
            pointerData[p] = m;
        }
        staticMemSemaphore.unlock();
        return m;
    }

    void StaticMemManager::deletePointer(void* p)
    {
        if(p == nullptr)
            return;
            
        MemInfo mInfo = MemInfo(); //default
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_WRITE);
        //decrement the counter.
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            it->second.counter--;
            //a pointer is deleted if its marked as force delete and was delayed, or the known references for the pointer hits 0.
            bool deleteCase1 = it->second.counter == 0;
            bool deleteCase2 = it->second.forceDelete && it->second.lockCount == 0;
            if(deleteCase1 || deleteCase2)
            {
                if(it->second.shouldDelete == true)
                    mInfo = it->second;
                pointerData.erase(p); //remove from the list regardless of whether its on the stack or not.
            }
        }
        staticMemSemaphore.unlock();
        if(mInfo.deleteFunc != nullptr)
            mInfo.deleteFunc(p, mInfo.array);
        
    }

    void StaticMemManager::forceDeletePointer(void* p)
    {
        if(p == nullptr)
            return;
            
        MemInfo mInfo = MemInfo(); //default
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_WRITE);
        //decrement the counter.
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            it->second.counter--;
            it->second.forceDelete = true;
            //a pointer is deleted if its marked as force delete and was delayed, or the known references for the pointer hits 0.
            //even with force delete, prevent crashing while currently in use.
            bool deleteCase1 = it->second.counter == 0;
            bool deleteCase2 = it->second.forceDelete && it->second.lockCount == 0;
            if(deleteCase1 || deleteCase2)
            {
                if(it->second.shouldDelete == true)
                    mInfo = it->second;
                pointerData.erase(p); //remove from the list regardless of whether its on the stack or not.
            }
        }
        staticMemSemaphore.unlock();
        if(mInfo.deleteFunc != nullptr)
            mInfo.deleteFunc(p, mInfo.array);
        
    }

    void* StaticMemManager::duplicateExistingPointer(void* p)
    {
        if(p == nullptr)
            return nullptr;

        void* v = nullptr;
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

    bool StaticMemManager::pointerExists(void* p)
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

    bool StaticMemManager::lockPointer(void* p)
    {
        if(p == nullptr)
            return false;
        
        bool success = false;
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_WRITE);
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            success = true;
            it->second.lockCount += 1;
        }
        staticMemSemaphore.unlock();
        return success;
    }

    bool StaticMemManager::unlockPointer(void* p)
    {
        if(p == nullptr)
            return false;
        
        bool success = false;
        staticMemSemaphore.lock(smpl::HybridSpinSemaphore::TYPE_WRITE);
        auto it = pointerData.find(p);
        if(it != pointerData.end())
        {
            success = true;
            it->second.lockCount -= 1;
        }
        staticMemSemaphore.unlock();
        return success;
    }


    GenericSmartMemory::GenericSmartMemory()
    {
        //nothing
    }

    GenericSmartMemory::~GenericSmartMemory()
    {
        //call delete function
        if(!deleteRights)
            StaticMemManager::deletePointer(hashValue);
        else
            StaticMemManager::forceDeletePointer(hashValue);

        isArray = false;
        deleteOnLast = false;
        deleteRights = false;
        hashValue = nullptr;
    }
    
    GenericSmartMemory::GenericSmartMemory(GenericSmartMemory&& other) noexcept
    {
        this->~GenericSmartMemory();

        isArray = other.isArray;
        deleteOnLast = other.deleteOnLast;
        deleteRights = other.deleteRights;
        hashValue = other.hashValue;

        other.hashValue = nullptr;
    }

    void GenericSmartMemory::operator=(GenericSmartMemory&& other) noexcept
    {
        this->~GenericSmartMemory();

        isArray = other.isArray;
        deleteOnLast = other.deleteOnLast;
        deleteRights = other.deleteRights;
        hashValue = other.hashValue;

        other.hashValue = nullptr;
    }

    GenericSmartMemory::GenericSmartMemory(const GenericSmartMemory& other)
    {
        this->~GenericSmartMemory();
        hashValue = StaticMemManager::duplicateExistingPointer(other.hashValue);
        isArray = other.isArray;
        deleteOnLast = other.deleteOnLast;
        deleteRights = false;
    }

    void GenericSmartMemory::operator=(const GenericSmartMemory& other)
    {
        this->~GenericSmartMemory();
        hashValue = StaticMemManager::duplicateExistingPointer(other.hashValue);
        isArray = other.isArray;
        deleteOnLast = other.deleteOnLast;
        deleteRights = false;
    }

    bool GenericSmartMemory::getDeleteRights()
    {
        return deleteRights;
    }

    bool GenericSmartMemory::getWillDeleteOnLast()
    {
        return deleteOnLast;
    }

    bool GenericSmartMemory::getIsArray()
    {
        return isArray;
    }
    
    size_t GenericSmartMemory::getSizeInBytes()
    {
        return sizeInBytes;
    }

    void* GenericSmartMemory::getPointer()
    {
        if(isValid())
            return hashValue;
        
        hashValue = nullptr;
        return nullptr;
    }

    void* GenericSmartMemory::getRawPointer()
    {
        return hashValue;
    }

    bool const GenericSmartMemory::operator==(const GenericSmartMemory& other)
    {
        //should not matter if they both exist or not.
        return hashValue == other.hashValue;
    }

    bool const GenericSmartMemory::operator==(const void* p)
    {
        //should not matter if they both exist or not.
        return hashValue == p;
    }

    bool GenericSmartMemory::isValid()
    {
        return StaticMemManager::pointerExists(hashValue);
    }
}