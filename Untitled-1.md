Notes:
    - SmartMemory system has a few variables that assists Serialization
        - sizeInBytes
        - hashValue
        - deleteOnLast
        - deleteRights
    - Utilizing these variables, its is possible to Serialize SmartMemory<T> but not LockingSmartMemory<T>
    or GenericSmartMemory. They lack in the required information necessary to gauge type.
    - Importantly, the process of Serializing SmartMemory pointers is a two part process.
        - Record current SmartMemory variables
        - Store actual pointer information
    - Because of this two part approach, the way data is written out must be delayed.
        - For each pointer, dereference and serialize but it must be serialized to a temporary location.
        - No seeking is allowed as that would defeat the purpose of allowing any solution for preprocessing.
            - That was the previous solution
        - A better solution is to have separate buffers for each pointer to serialize and at the end, concatentate them all together.
            - A vector of SerializedStreamable objects?
        - This way preprocessing is preserved, no pointers are overlapping with each other (inside of each other), and its easyish to search across those pointers and initialize any individual one in any order.
    - Basic Approach:
        - For each pointer found (each pointer attempted to register), serialize it into a separate buffer stored in an unordered map.
            - If that pointer has already been serialized, it'll be skipped but its SmartPointer attributes will be saved.
        - At the end, store all of these values into a separate buffer that must be read at the start before deserialization.
        - For each pointer found for deserialization, look into the map that was restored at the start and if it was already created,
        use that pointer otherwise recreate using the serialized buffer given.
            - Assuming the hash value was even found. The hash value refers to the pointer value it was initially assigned. Useless
            on new run so it must be changed.
            
