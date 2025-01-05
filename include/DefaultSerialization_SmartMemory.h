// #pragma once
// #include "SimpleSerialization.h"
// #include "SmartMemory.h"
// #include <unordered_map>

// //TODO: FIX LATER

// namespace smpl
// {
//     class DeferredSmartPointerInfo
//     {
//     public:
//         /**
//          * @brief Adds to the map of pointers.
//          *      Used for serialization only.
//          *      Stores the ptr reference, the actual ptr (if it exists), and the offset to that data in the Streamable Buffer if needed to deserialize
//          * 
//          * 
//          * @param ptr 
//          * @param sizeInBytes 
//          */
//         static void mapPointer(void* referencePtr, void* actualPtr, size_t offset);

//         /**
//          * @brief Attempts to get a pointer from some reference ptr.
//          *      Used for deserialization only.
//          *          Gets the new pointer and its size in bytes.
//          * 
//          * @param ptr 
//          * @return void* 
//          */
//         static std::pair<void*, size_t> getPointerMapping(void* referencePtr);

//         /**
//          * @brief Clears the mapping of pointers.
//          * 
//          */
//         static void clear();

//         /**
//          * @brief Attempts to serialize at the current index in output
//          * 
//          * @param output 
//          */
//         static void serialize(SerializedStreamable& output, DataFormatter& formatter);

//         /**
//          * @brief Attempts to deserialize at the current index in input.
//          * 
//          * @param output 
//          */
//         static void deserialize(SerializedStreamable& output, DataFormatter& formatter);

//         static SerializedStreamableVector& getStreamableBuffer();
        
//     private:
//         static DeferredSmartPointerInfo& getSingleton();
//         DeferredSmartPointerInfo() {}
//         ~DeferredSmartPointerInfo() {}
//         SerializedStreamableVector buffer;
//         std::unordered_map<void*, std::pair<void*, size_t>> pointerMappings;
//     };

//     inline DeferredSmartPointerInfo& DeferredSmartPointerInfo::getSingleton()
//     {
//         static DeferredSmartPointerInfo singleton;
//         return singleton;
//     }
    
//     inline SerializedStreamableVector& DeferredSmartPointerInfo::getStreamableBuffer()
//     {
//         return getSingleton().buffer;
//     }

//     inline void DeferredSmartPointerInfo::mapPointer(void* referencePtr, void* actualPtr, size_t offset)
//     {
//         DeferredSmartPointerInfo& singleton = getSingleton();
//         singleton.pointerMappings[referencePtr] = std::pair<void*, size_t>(actualPtr, offset);
//     }
//     inline std::pair<void*, size_t> DeferredSmartPointerInfo::getPointerMapping(void* referencePtr)
//     {
//         DeferredSmartPointerInfo& singleton = getSingleton();
//         auto it = singleton.pointerMappings.find(referencePtr);
//         if(it != singleton.pointerMappings.end())
//             return it->second;
//         return std::pair<void*, size_t>(nullptr, 0);
//     }
//     inline void DeferredSmartPointerInfo::clear()
//     {
//         DeferredSmartPointerInfo& singleton = getSingleton();
//         singleton.pointerMappings.clear();
//         // singleton.buffer
//     }
//     inline void DeferredSmartPointerInfo::serialize(SerializedStreamable& output, DataFormatter& formatter)
//     {
//         DeferredSmartPointerInfo& singleton = getSingleton();
//         //write out Streamable data. Already handled
//         //writes out referencePtrKey, sizeInBytes, serialized data (must be deserialized. not read directly)
//         //WRITE OUT TOTAL SIZE OF TABLE
//         formatter.writeStart(output, DataFormatter::FORMAT_OBJECT, TypeInfo::get<DeferredSmartPointerInfo>(), "", 1);
//         ::staticSerialize(output, formatter, singleton.pointerMappings);
//         formatter.writeEnd(output);
//     }
    
//     inline void DeferredSmartPointerInfo::deserialize(SerializedStreamable& input, DataFormatter& formatter)
//     {
//         DeferredSmartPointerInfo& singleton = getSingleton();
//         //read ptr as key, sizeInBytes, data to be deserialized.
//         //READ TOTAL SIZE OF TABLE
//         uint64_t elements = formatter.readStart(input, DataFormatter::FORMAT_OBJECT, TypeInfo::get<DeferredSmartPointerInfo>(), "");
//         ::staticDeserialize(input, formatter, singleton.pointerMappings);
//         formatter.readEnd(input);
//     }

//     //NOW THE MEAT AND POTATOES
//     template<typename T>
//     void staticSerialize(SerializedStreamable& output, DataFormatter& formatter, const std::string& varName, SmartMemory<T>& var)
//     {
//         //defer pointer. write out raw value.
//         //IF the pointer isn't deletable, ignore. WRITE OUT NULLPTR
//         T* varPtr = var.getPointer();
//         void* rawPtrValue = (void*)varPtr; //the pointer as void ptr

//         if(!var.getWillDeleteOnLast() && !var.getDeleteRights())
//         {
//             //ptr will not be deleted. Either a stack object or ptr from some unmanaged code.
//             //Will store nullptr due to safety reasons
//             rawPtrValue = nullptr;
//         }

//         //write ptr value
//         ::staticSerialize(output, formatter, rawPtrValue);
        
//         //write delete flags.
//         bool v = var.getDeleteRights();
//         ::staticSerialize(output, formatter, v);
        
//         v = var.getWillDeleteOnLast();
//         ::staticSerialize(output, formatter, v);

//         //write number of elements
//         size_t elements = var.getNumberOfElements();
//         ::staticSerialize(output, formatter, elements);
        
//         //now defer the pointer unless its nullptr
//         if(rawPtrValue != nullptr)
//         {
//             SerializedStreamableVector& buff = DeferredSmartPointerInfo::getStreamableBuffer();
//             //write out to the streamable buffer
//             DeferredSmartPointerInfo::mapPointer(rawPtrValue, rawPtrValue, buff.size());
            
//             size_t sizeInBytes = 0;
//             ::staticSerialize(buff, formatter, rawPtrValue);

//             //write out referencePtr, then data to deserialize (Note that we are writting out an array of values that is potentially 0 elements)
//             formatter.writeStart(buff, DataFormatter::FORMAT_ARRAY, TypeInfo::get<T*>(), elements);
//             for(size_t index=0; index<elements; index++)
//             {
//                 ::staticSerialize(buff, formatter, var[index]);
//             }
//             formatter.writeEnd(buff);
//         }
//     }

//     template<typename T>
//     void staticDeserialize(SerializedStreamable& input, DataFormatter& formatter, const std::string& varName, SmartMemory<T>& var)
//     {
//         //attempt to find the ptr
//         T* referencePtr = nullptr;
//         T* actualPtr = nullptr;
//         bool deleteRights = false;
//         bool deleteOnLast = false;
//         size_t elements = 1;

//         //read ptr value. NOT THE FINAL PTR. DO NOT USE YET
//         ::staticDeserialize(input, formatter, referencePtr);

//         //read delete flags
//         ::staticDeserialize(input, formatter, deleteRights);
//         ::staticDeserialize(input, formatter, deleteOnLast);

//         //read number of elements
//         ::staticDeserialize(input, formatter, elements);

//         //first check if the ptr is in the deferred list.
//         std::pair<void*, size_t> foundPtr = DeferredSmartPointerInfo::getPointerMapping(referencePtr);
//         if(foundPtr.first == nullptr)
//         {
//             //did not find it. Create it and add it to the list
//             //MUST BE ABLE TO CREATE WITH DEFAULT CONSTRUCTOR
            
//             if(elements > 1)
//                 actualPtr = new T[elements]; //IS ARRAY
//             else
//                 actualPtr = new T;
            
//             //now deserialize that/them
//             size_t oldLocation = input.getLocation();

//             input.seek(foundPtr.second); //move to the position with this pointer's data
//             for(size_t i=0; i<elements; i++)
//             {
//                 ::staticDeserialize(input, formatter, actualPtr[i]);
//             }
//             input.seek(oldLocation); //move back to the correct position
            
//             DeferredSmartPointerInfo::mapPointer(referencePtr, actualPtr, foundPtr.second); //offset into the buffer isn't needed anymore but whatever
//         }
//         else
//         {
//             actualPtr = foundPtr.first;
//         }

//         var = SmartMemory<T>(actualPtr, elements, deleteOnLast, deleteRights);
//     }
// }