#pragma once
#include <unordered_map>
#include "Concurrency.h"

/*
    Upon first creation:
        Adds the required functionality to delete the pointer correctly.

    If casted to a different pointer (whether valid or not):
        Does not change how the pointer is deleted. Only how its interpreted.

    Use 4 classes for general memory management stuff
        GlobalMemManager -> responsible for maintaining all pointers but not deleting them. Just calls a delete function
        GenericSmartMemory -> the base class that stores a void pointer. Its responsible for most of the front facing functionality for smart pointers.
        SmartMemory -> the template class that adds specific functionality for smart pointers (deleting the pointer, using it, etc.)
        LockingSmartMemory -> a 2nd template class that locks smart memory preventing its deletion until it is done. Must be created from an existing smart memory.
                        and can not be copied. Only moved. Automatically unlocks the memory after being destroyed.
*/

namespace smpl
{
    class DLL_OPTION StaticMemManager;
    class DLL_OPTION GenericSmartMemory;

    template<class T>
    class SmartMemory;
    template<class T>
    class LockingSmartMemory;

    struct MemInfo
    {
        bool array = false;
        bool shouldDelete = true;
        bool forceDelete = false;
        int lockCount = 0; //amount of currently active uses
        int counter = 0; //amount of references (not necessarily in use)
        void (*deleteFunc)(void*, bool) = nullptr; //the delete function. 1st argument is the pointer. 2nd argument specifies if it is an array or not.
        size_t sizeInBytes = 0;
    };

    class DLL_OPTION StaticMemManager
    {
    public:
        static MemInfo addPointer(void* p, bool isArray, size_t sizeInBytes, bool deleteOnLast, bool bypassOwnership, void (*deleteFunc)(void*, bool));
        static void deletePointer(void* p);
        static void forceDeletePointer(void* p);
        static void* duplicateExistingPointer(void* p);
        static bool pointerExists(void* p);
        static bool lockPointer(void* p);
        static bool unlockPointer(void* p);

    protected:
        static std::unordered_map<void*, MemInfo> pointerData;
        static smpl::HybridSpinSemaphore staticMemSemaphore;
    };

    class DLL_OPTION GenericSmartMemory
    {
    public:
        /**
         * @brief Destroy the Generic Smart Memory object
         *      This will delete the pointer if it is supposed to.
         * 
         */
        virtual ~GenericSmartMemory();

        /**
         * @brief Move Constructor
         * 
         * @param other 
         */
        GenericSmartMemory(GenericSmartMemory&& other) noexcept;

        /**
         * @brief Move Assign a new GenericSmartMemory object from another GenericSmartMemory object.
         * 
         * @param other 
         */
        void operator=(GenericSmartMemory&& other) noexcept;

        /**
         * @brief Construct a new GenericSmartMemory object from another GenericSmartMemory object.
         *      It will not have delete rights.
         *      If the object originally stored a pointer and had delete rights, it will delete its pointer.
         * 
         *      Equavelent to a shared_ptr<T>
         * 
         * @param other 
         */
        GenericSmartMemory(const GenericSmartMemory& other);

        /**
         * @brief Copy Assign a new GenericSmartMemory object from another GenericSmartMemory object.
         *      It will not have delete rights.
         *      If the object originally stored a pointer and had delete rights, it will delete its pointer.
         * 
         *      Equavelent to a shared_ptr<T>
         * 
         * @param other 
         */
        void operator=(const GenericSmartMemory& other);

        /**
         * @brief Returns if the object has delete rights.
         *      If it has delete rights, the data pointer will be deleted when the object is destroyed.
         * 
         * @return true 
         * @return false 
         */
        bool getDeleteRights();

        /**
         * @brief Returns in the object will delete the pointer if it is the last known holder of it.
         *      By default, most objects will have this property but if an object is not intended to be
         *          deleted at all, this should be false and delete rights should also be false.
         * 
         * @return true 
         * @return false 
         */
        bool getWillDeleteOnLast();

        /**
         * @brief Returns if the object is an array.
         *      This is mostly for internal use for how to delete the object.
         * 
         * @return true 
         * @return false 
         */
        bool getIsArray();

        /**
         * @brief Gets the size in terms of bytes.
         * 
         * @return size_t 
         */
        size_t getSizeInBytes();

        /**
         * @brief Gets the stored pointer.
         *      If it has been deleted, a nullptr will be returned.
         * 
         * @return void* 
         */
        void* getPointer();

        /**
         * @brief Gets the Raw Pointer that is stored.
         *      Does not check if the pointer is valid.
         * 
         *      If the pointer at a previous point was discovered to be deleted,
         *      this will return a nullptr.
         * 
         * @return void* 
         */
        void* getRawPointer();

        /**
         * @brief Compares 2 GenericSmartMemory objects.
         *      Does not check if either pointer is valid but just compares the pointers that they currently store.
         * 
         * @param other 
         * @return true 
         * @return false 
         */
        bool const operator==(const GenericSmartMemory& other);

        /**
         * @brief Compares the pointer stored to the specified pointer.
         *      Does not check if either pointer is valid.
         * 
         * @param p 
         * @return true 
         * @return false 
         */
        bool const operator==(const void* p);

        /**
         * @brief Checks if the pointer is valid.
         * 
         * @return true 
         * @return false 
         */
        bool isValid();
    protected:
        GenericSmartMemory();

        bool isArray = false;
        bool deleteOnLast = false;
        bool deleteRights = false; //Not necessarily the same between GenericSmartMemory. 1 may have delete rights but not another.
        void* hashValue = nullptr;
        size_t sizeInBytes = 0;
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
        SmartMemory(T* data, size_t elements = 1, bool deleteOnLast = true, bool bypassOwnership = false)
        {
            if(elements == 0)
                elements = 1;
            sizeInBytes = elements * sizeof(T);
            MemInfo m = StaticMemManager::addPointer(data, elements>1, sizeInBytes, deleteOnLast, bypassOwnership, SmartMemory<T>::deletePointer);
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
         * @param elements 
         * @return SmartMemory<T> 
         */
        static SmartMemory<T> createDeleteOnLast(T* data, size_t elements = 1)
        {
            return SmartMemory(data, elements, true, false);
        }

        /**
         * @brief Shortcut function. Creates a SmartMemory such that the data will be never be deleted.
         *      Good for local objects being passed around via pointers without worrying if it will be deleted improperly.
         *      This functionality is passed to all copied instances.
         * 
         * @param data 
         * @param elements 
         * @return SmartMemory<T> 
         */
        static SmartMemory<T> createNoDelete(T* data, size_t elements = 1)
        {
            return SmartMemory(data, elements, false, false);
        }

        /**
         * @brief Shortcut function. Creates a SmartMemory such that the data will be whenever this object is deleted.
         *      This establishes a sort of ownership relationship with the object and the data.
         *      All other SmartMemory instances created from this one will not shared the same functionality but will not 
         *          be able to delete the pointer when they are destroyed.
         * 
         * @param data 
         * @param elements 
         * @return SmartMemory<T> 
         */
        static SmartMemory<T> createDeleteRights(T* data, size_t elements = 1)
        {
            return SmartMemory(data, elements, false, true);
        }

        /**
         * @brief Destroy the SmartMemory object.
         *      If the object has delete rights, it will delete the pointer.
         */
        virtual ~SmartMemory()
        {
        }

        
        /**
         * @brief Move Constructor
         * 
         * @param other 
         */
        SmartMemory(SmartMemory&& other) noexcept : GenericSmartMemory(std::move(other)) {}

        /**
         * @brief Move Assign a new SmartMemory object from another SmartMemory object.
         * 
         * @param other 
         */
        void operator=(SmartMemory&& other) noexcept {GenericSmartMemory::operator=(std::move(other));}

        
        /**
         * @brief Copy Constructor
         * 
         * @param other 
         */
        SmartMemory(const SmartMemory& other) : GenericSmartMemory(other) {}

        /**
         * @brief Copy Assign a new SmartMemory object from another SmartMemory object.
         * 
         * @param other 
         */
        void operator=(const SmartMemory& other) {GenericSmartMemory::operator=(other);}

        /**
         * @brief Gets the stored pointer.
         *      If it has been deleted, a nullptr will be returned.
         * 
         * @return T* 
         */
        T* getPointer()
        {
            return (T*)GenericSmartMemory::getPointer();
        }
        
        /**
         * @brief Get the Raw Pointer stored.
         *      Does not check if the pointer is valid or not.
         * 
         * @return T* 
         */
        T* getRawPointer()
        {
            return (T*)GenericSmartMemory::getRawPointer();
        }

        /**
         * @brief Attempts to use the pointer as is. Does not check if it is valid or not.
         * 
         * @return T* 
         */
        T* operator->()
        {
            return (T*)hashValue;
        }

        /**
         * @brief Attempts to use the pointer as an array as is. Does not check if it is valid or not.
         * 
         * @return T* 
         */
        T& operator[](size_t index)
        {
            return ((T*)hashValue)[index];
        }

        /**
         * @brief Attempts to dereference the pointer. Does not check if it is valid or not.
         * 
         * @return T&* 
         */
        T& operator*()
        {
            return *((T*)hashValue);
        }

        /**
         * @brief Compares the pointer stored to another pointer to determine
         *      if they are the same.
         * 
         *      Calls getPointer() before checking if the 2 pointers are the same.
         * 
         * @param pointer 
         * @return true 
         * @return false 
         */
        bool operator==(const T* pointer)
        {
            return getPointer() == pointer;
        }

        /**
         * @brief Compares the pointer stored to another pointer to determine
         *      if they are the same.
         * 
         *      Calls getPointer() before checking if the 2 pointers are the same.
         * 
         * @param pointer 
         * @return true 
         * @return false 
         */
        bool operator==(SmartMemory<T>& other)
        {
            return getPointer() == other.getPointer();
        }

        /**
         * @brief Compares the pointer stored to another pointer to determine
         *      if they are not the same.
         * 
         *      Calls getPointer() before checking if the 2 pointers are not the same.
         * 
         * @param pointer 
         * @return true 
         * @return false 
         */
        bool operator!=(const T* pointer)
        {
            return getPointer() != pointer;
        }
        
        /**
         * @brief Compares the pointer stored to another pointer to determine
         *      if they are not the same.
         * 
         *      Calls getPointer() before checking if the 2 pointers are not the same.
         * 
         * @param pointer 
         * @return true 
         * @return false 
         */
        bool operator!=(SmartMemory<T>& other)
        {
            return getPointer() != other.getPointer();
        }

        /**
         * @brief Get the Number Of Elements for this pointer.
         *      The number of elements are calculated by dividing the total bytes by the size of T (the type)
         * 
         * @return size_t 
         */
        size_t getNumberOfElements()
        {
            return sizeInBytes / sizeof(T);
        }

        /**
         * @brief Creates a locking pointer from this object.
         *      The locking pointer insures that the pointer can not be deleted while it is in use.
         *      Note that this is different from normal use where a SmartMemory object only holds onto a pointer
         *          to use later and can check if it is valid or not before using it.
         * 
         * @return LockingSmartMemory<T> 
         */
        LockingSmartMemory<T> getLockingPointer()
        {
            return LockingSmartMemory<T>(*this); //fix later
        }

        template<typename K>
        operator SmartMemory<K>() const
        {
            return SmartMemory<K>((K*)hashValue, sizeInBytes / sizeof(K), deleteOnLast, false);
        }

    private:
        friend LockingSmartMemory<T>;

        /**
         * @brief Deletes the pointer properly. Is passed as a pointer to the StaticMemMananger
         * 
         */
        static void deletePointer(void* p, bool arr)
        {
            //assume p has been verified to not be nullptr before
            if(arr)
                delete[] (T*)p;
            else
                delete (T*)p;
        }
    };

    template<typename T>
    class LockingSmartMemory : public GenericSmartMemory
    {
    public:
        /**
         * @brief Constructs a new Locking Smart Memory object
         *      Must be created from an existing SmartMemory object.
         *      Guarantees that the pointer has not been delete nor will be deleted by
         *          any of the smart pointer system functions. If it is deleted from outside
         *          the smart pointer system, these operations may be invalid.
         * 
         *      Due to the nature of the locking system, this class does not check if the pointer is valid on each access.
         *          The pointer is guaranteed to be valid, as far as the SmartMemory System is concerned, for its entire lifespan.
         *          This means that this should be faster in situations where peformance matters while maintaining safety.
         * 
         * @param parent 
         */
        LockingSmartMemory(const SmartMemory<T>& parent)
        {
            hashValue = StaticMemManager::duplicateExistingPointer((T*)parent.hashValue);
            isArray = parent.isArray;
            deleteOnLast = parent.deleteOnLast;
            deleteRights = false;
            StaticMemManager::lockPointer(hashValue);
        }

        virtual ~LockingSmartMemory()
        {
            StaticMemManager::unlockPointer(hashValue);
        }

        LockingSmartMemory(const LockingSmartMemory& other) = delete;
        void operator=(const LockingSmartMemory& other) = delete;
        
        LockingSmartMemory(const LockingSmartMemory&& other) noexcept
        {
            hashValue = other.hashValue;
            isArray = other.isArray;
            deleteOnLast = other.deleteOnLast;
            other.hashValue = nullptr;
        }
        void operator=(const LockingSmartMemory&& other) noexcept
        {
            hashValue = other.hashValue;
            isArray = other.isArray;
            deleteOnLast = other.deleteOnLast;
            other.hashValue = nullptr;
        }

        /**
         * @brief Gets the stored pointer.
         *      If it has been deleted, a nullptr will be returned.
         *      Note that for locking pointers, the pointer will not be deleted
         *          before this object has been deleted. Therefore, this just
         *          returns the raw pointer.
         * 
         * @return T* 
         */
        T* getPointer()
        {
            return (T*)GenericSmartMemory::getRawPointer();
        }
        
        /**
         * @brief Get the Raw Pointer stored.
         *      Does not check if the pointer is valid or not.
         * 
         * @return T* 
         */
        T* getRawPointer()
        {
            return (T*)GenericSmartMemory::getRawPointer();
        }

        /**
         * @brief Attempts to use the pointer as is. Does not check if it is valid or not.
         *     Pointer is invalid if it was deleted outside of the SmartMemory System or it is nullptr.
         * 
         * @return T* 
         */
        T* operator->()
        {
            return (T*)hashValue;
        }

        /**
         * @brief Attempts to use the pointer as an array as is. Does not check if it is valid or not.
         *      Pointer is invalid if it was deleted outside of the SmartMemory System or it is nullptr.
         * 
         * @return T* 
         */
        T& operator[](size_t index)
        {
            return ((T*)hashValue)[index];
        }

        /**
         * @brief Attempts to dereference the pointer. Does not check if it is valid or not.
         *      Pointer is invalid if it was deleted outside of the SmartMemory System or it is nullptr.
         * 
         * @return T&* 
         */
        T& operator*()
        {
            return *((T*)hashValue);
        }

        /**
         * @brief Compares the pointer stored to another pointer to determine
         *      if they are the same.
         * 
         *      Unlike with SmartMemory, this class avoids the need to call getPointer as
         *          the pointer it stores must be valid for this objects lifetime unless it is nullptr which is not valid at all.
         *          Assuming no deletion happens outside of the SmartMemory system.
         * 
         * @param pointer 
         * @return true 
         * @return false 
         */
        bool operator==(T* pointer)
        {
            return ((T*)hashValue) == pointer;
        }

        /**
         * @brief Compares the pointer stored to another pointer to determine
         *      if they are not the same.
         * 
         *      Unlike with SmartMemory, this class avoids the need to call getPointer as
         *          the pointer it stores must be valid for this objects lifetime unless it is nullptr which is not valid at all.
         *          Assuming no deletion happens outside of the SmartMemory system.
         * 
         * @param pointer 
         * @return true 
         * @return false 
         */
        bool operator!=(T* pointer)
        {
            return ((T*)hashValue) != pointer;
        }

        /**
         * @brief Get the Number Of Elements for this pointer.
         *      The number of elements are calculated by dividing the total bytes by the size of T (the type)
         * 
         * @return size_t 
         */
        size_t getNumberOfElements()
        {
            return sizeInBytes / sizeof(T);
        }
    private:
        friend SmartMemory<T>;
    };
}