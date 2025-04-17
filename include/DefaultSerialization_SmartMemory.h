#pragma once
#include "SimpleSerialization.h"
#include "SmartMemory.h"
#include <unordered_map>

namespace smpl
{
    class DeferredSerializedSmartMemory
    {
    public:
        ~DeferredSerializedSmartMemory(){}

        template<typename T>
        static void addPointer(DataFormatter& formatter, SmartMemory<T>& smPointer)
        {
            T* rawP = smPointer.getPointer();
            if(rawP != nullptr)
            {
                std::pair<void*, SerializedStreamableVector> pairing = {nullptr, SerializedStreamableVector(1)};
                for(int i=0; i<smPointer.getNumberOfElements(); i++)
                {
                    staticSerialize(pairing.second, formatter, rawP[i]);
                }
                pairing.second.seek(0);
                singleton.pointerMappings.insert({rawP, pairing});
            }
        }

        template<typename T>
        static T* getPointer(DataFormatter& formatter, T* originalPointer, size_t elements)
        {
            if(originalPointer != nullptr)
            {
                auto it = singleton.pointerMappings.find((void*)originalPointer);
                if(it != singleton.pointerMappings.end())
                {
                    //found the hash. Check if its already been created. If so, return that new pointer.
                    if(it->second.first != nullptr)
                        return (T*)it->second.first;
                    else
                    {
                        //create it from serialized buffer
                        //must be able to create with default constructor
                        T* newPointer;
                        if(elements > 1)
                            newPointer = new T[elements];
                        else
                            newPointer = new T;
                        for(int i=0; i<elements; i++)
                        {
                            staticDeserialize(it->second.second, formatter, newPointer[i]);
                        }
                        it->second.first = newPointer;
                        return newPointer;
                    }
                }
                //TODO: throw exception
            }
            return nullptr;
        }

        static void serialize(SerializedStreamable& output, DataFormatter& formatter);
        static void deserialize(SerializedStreamable& output, DataFormatter& formatter);

        static void clear();

    private:
        DeferredSerializedSmartMemory(){}
        static DeferredSerializedSmartMemory singleton;
        std::unordered_map<void*, std::pair<void*, SerializedStreamableVector>> pointerMappings;
    };

    inline DeferredSerializedSmartMemory DeferredSerializedSmartMemory::singleton = DeferredSerializedSmartMemory();
    
    inline void DeferredSerializedSmartMemory::serialize(SerializedStreamable& output, DataFormatter& formatter)
    {
        //special case. must write end of the object
        formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<DeferredSerializedSmartMemory>(), "", 1);
        staticSerialize(output, formatter, DeferredSerializedSmartMemory::singleton.pointerMappings);
        formatter.writeEnd(output);
    }
    inline void DeferredSerializedSmartMemory::deserialize(SerializedStreamable& output, DataFormatter& formatter)
    {
        //special case. must read end of the object
        int64_t elements = formatter.readStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<DeferredSerializedSmartMemory>(), "");
        if(elements != 1)
            return; //error maybe.
        staticDeserialize(output, formatter, DeferredSerializedSmartMemory::singleton.pointerMappings);
        formatter.readEnd(output);
    }
    
    inline void DeferredSerializedSmartMemory::clear()
    {
        DeferredSerializedSmartMemory::singleton.pointerMappings.clear();
    }

    template<typename T>
    void staticSerializeHandler(SerializedStreamable& output, DataFormatter& formatter, const std::string varName, SmartMemory<T>& var)
    {
        formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<SmartMemory<T>>(), varName, 1);
        
        bool isArray = var.getIsArray(); 
        bool deleteOnLast = var.getWillDeleteOnLast(); 
        bool deleteRights = var.getDeleteRights();
        size_t numElements = var.getNumberOfElements();
        T* pointerStored = var.getPointer();

        staticSerialize(output, formatter, isArray);
        staticSerialize(output, formatter, deleteOnLast);
        staticSerialize(output, formatter, deleteRights);
        staticSerialize(output, formatter, pointerStored);
        staticSerialize(output, formatter, numElements);

        if(deleteRights || deleteOnLast)
            DeferredSerializedSmartMemory::addPointer(formatter, var);
    }

    template<typename T>
    void staticDeserializeHandler(SerializedStreamable& input, DataFormatter& formatter, const std::string varName, SmartMemory<T>& var)
    {
        int64_t s = formatter.readStart(input, DataFormatter::FORMAT_OBJECT, TypeInfo::get<SmartMemory<T>>(), varName);
        if(s != 1)
            return; //couldn't find it. potentially an error.
        
        bool isArray, deleteOnLast, deleteRights;
        size_t numElements;
        T* pointerStored;

        staticDeserialize(input, formatter, isArray);
        staticDeserialize(input, formatter, deleteOnLast);
        staticDeserialize(input, formatter, deleteRights);
        staticDeserialize(input, formatter, pointerStored);
        staticDeserialize(input, formatter, numElements);
        
        if(deleteOnLast || deleteRights)
        {
            T* newPointer = DeferredSerializedSmartMemory::getPointer<T>(formatter, pointerStored, numElements);
            var = SmartMemory<T>(newPointer, numElements, deleteOnLast, deleteRights);
        }
    }
}