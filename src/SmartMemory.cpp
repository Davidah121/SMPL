#include "SmartMemory.h"
#include "Concurrency.h"
#include <mutex>

namespace smpl
{
	SimpleHashMap<void*, MemInfo> StaticMemManager::pointerData;
	smpl::ReadWriterLock StaticMemManager::staticMemLock;

	MemInfo StaticMemManager::addPointer(void* p, bool isArray, size_t sizeInBytes, bool deleteOnLast, bool bypassOwnership, void (*deleteFunc)(void*, bool))
	{
		if(p == nullptr)
			return {};

		MemInfo m;
		staticMemLock.lock(smpl::ReadWriterLock::TYPE_WRITE);
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
		staticMemLock.unlock();
		return m;
	}

	void StaticMemManager::deletePointer(void* p)
	{
		if(p == nullptr)
			return;
			
		MemInfo mInfo = MemInfo(); //default
		staticMemLock.lock(smpl::ReadWriterLock::TYPE_WRITE);
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
		staticMemLock.unlock();
		if(mInfo.deleteFunc != nullptr)
			mInfo.deleteFunc(p, mInfo.array);
		
	}

	void StaticMemManager::forceDeletePointer(void* p)
	{
		if(p == nullptr)
			return;
			
		MemInfo mInfo = MemInfo(); //default
		staticMemLock.lock(smpl::ReadWriterLock::TYPE_WRITE);
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
		staticMemLock.unlock();

		if(mInfo.deleteFunc != nullptr)
			mInfo.deleteFunc(p, mInfo.array);
		
	}

	void* StaticMemManager::duplicateExistingPointer(void* p)
	{
		if(p == nullptr)
			return nullptr;

		void* v = nullptr;
		staticMemLock.lock(smpl::ReadWriterLock::TYPE_WRITE);
		auto it = pointerData.find(p);
		if(it != pointerData.end())
		{
			it->second.counter++;
			v = p;
		}
		staticMemLock.unlock();
		return v;
	}

	bool StaticMemManager::pointerExists(void* p)
	{
		if(p == nullptr)
			return false;
		
		bool valid = false;
		staticMemLock.lock(smpl::ReadWriterLock::TYPE_READ);
		auto it = pointerData.find(p);
		if(it != pointerData.end())
			valid = true;
		staticMemLock.unlock();
		return valid;
	}

	bool StaticMemManager::lockPointer(void* p)
	{
		if(p == nullptr)
			return false;
		
		bool success = false;
		staticMemLock.lock(smpl::ReadWriterLock::TYPE_WRITE);
		auto it = pointerData.find(p);
		if(it != pointerData.end())
		{
			success = true;
			it->second.lockCount += 1;
		}
		staticMemLock.unlock();
		return success;
	}

	bool StaticMemManager::unlockPointer(void* p)
	{
		if(p == nullptr)
			return false;
		
		bool success = false;
		staticMemLock.lock(smpl::ReadWriterLock::TYPE_WRITE);
		auto it = pointerData.find(p);
		if(it != pointerData.end())
		{
			success = true;
			it->second.lockCount -= 1;
		}
		staticMemLock.unlock();
		return success;
	}


	GenericSmartMemory::GenericSmartMemory()
	{
		//nothing
	}

	GenericSmartMemory::~GenericSmartMemory()
	{
		//call delete function
		clear();
	}
	
	GenericSmartMemory::GenericSmartMemory(GenericSmartMemory&& other) noexcept
	{
		clear();

		isArray = other.isArray;
		deleteOnLast = other.deleteOnLast;
		deleteRights = other.deleteRights;
		hashValue = other.hashValue;
		sizeInBytes = other.sizeInBytes;

		other.hashValue = nullptr;
	}

	GenericSmartMemory& GenericSmartMemory::operator=(GenericSmartMemory&& other) noexcept
	{
		clear();

		isArray = other.isArray;
		deleteOnLast = other.deleteOnLast;
		deleteRights = other.deleteRights;
		hashValue = other.hashValue;
		sizeInBytes = other.sizeInBytes;

		other.hashValue = nullptr;
		return *this;
	}

	GenericSmartMemory::GenericSmartMemory(const GenericSmartMemory& other)
	{
		clear();

		hashValue = StaticMemManager::duplicateExistingPointer(other.hashValue);
		isArray = other.isArray;
		deleteOnLast = other.deleteOnLast;
		deleteRights = false;
		sizeInBytes = other.sizeInBytes;
	}

	GenericSmartMemory& GenericSmartMemory::operator=(const GenericSmartMemory& other)
	{
		clear();

		hashValue = StaticMemManager::duplicateExistingPointer(other.hashValue);
		isArray = other.isArray;
		deleteOnLast = other.deleteOnLast;
		deleteRights = false;
		sizeInBytes = other.sizeInBytes;
		return *this;
	}

	void GenericSmartMemory::clear()
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

	bool GenericSmartMemory::getDeleteRights() const
	{
		return deleteRights;
	}

	bool GenericSmartMemory::getWillDeleteOnLast() const
	{
		return deleteOnLast;
	}

	bool GenericSmartMemory::getIsArray() const
	{
		return isArray;
	}
	
	size_t GenericSmartMemory::getSizeInBytes() const
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

	void* GenericSmartMemory::getPointer() const
	{
		if(isValid())
			return hashValue;
		
		return nullptr;
	}

	void* GenericSmartMemory::getRawPointer() const
	{
		return hashValue;
	}

	bool GenericSmartMemory::operator==(const GenericSmartMemory& other) const
	{
		//should not matter if they both exist or not.
		return hashValue == other.hashValue;
	}

	bool GenericSmartMemory::operator==(const void* p) const
	{
		//should not matter if they both exist or not.
		return hashValue == p;
	}

	bool GenericSmartMemory::operator!=(const GenericSmartMemory& other) const
	{
		//should not matter if they both exist or not.
		return hashValue != other.hashValue;
	}

	bool GenericSmartMemory::operator!=(const void* p) const
	{
		//should not matter if they both exist or not.
		return hashValue != p;
	}

	bool GenericSmartMemory::isValid() const
	{
		return StaticMemManager::pointerExists(hashValue);
	}
}