#pragma once
#include "BuildOptions.h"
#include <iostream>
#include <vector>
#include <typeinfo>
#include <functional>
#include "Object.h"
#include "Streamable.h"
#include <unordered_map>

namespace smpl
{
	class DLL_OPTION SerializedData;
	class DLL_OPTION SerializedObject;
	class DLL_OPTION SerializedFactory;

	struct WritableSerialzedData
	{
		static const bool TYPE_PRIMITIVE = false;
		static const bool TYPE_OBJECT = true;
		
		bool type = 0;
		size_t size = 0; //size of the data in bytes
		unsigned char* data = nullptr; //the data as bytes
	};

	//so for every object
	//type, name size, name bytes, numberOfOffsets, list of offsets
	//for every primitive
	//type, size, data
	//Total required space for an object is 9 bytes + n bytes for name and k bytes for the offsets. Those can actually be 0 so at least 9 is required.
	//Total required space for a primitive is 5 bytes + n bytes for the data. This too can also be 0 so at least 5 bytes is required.
	//for each function, pass in a vector of unsigned bytes or an array of unsigned bytes with a size
	class DLL_OPTION SerializedData
	{
	public:
		static const bool TYPE_DATA = false;
		static const bool TYPE_OBJECT = true;

		/**
		 * @brief Adds a few default pretty names.
		 * 		Optional
		 * 
		 */
		// static void init();
		
		/**
		 * @brief Construct a new Serialized Data object
		 * 		Empty Constructor used by SerializedObject.
		 * 
		 */
		SerializedData();

		/**
		 * @brief Construct a new Serialized Data object
		 * 
		 * @param d 
		 * 		Assumes that d is a pointer to the data
		 * @param name 
		 * 		The name of the variable as a string
		 * @param typeString 
		 * 		The type of the variable as a string
		 */
		SerializedData(void* d, std::string name, std::string typeString, size_t size);

		/**
		 * @brief Construct a new Serialized Data object
		 * 
		 * @param d 
		 * 		Assumes that d is a pointer to the data
		 * 		Special case:
		 * 			Will set the name and type of the SerializedObject.
		 * 			The data for the SerializedObject will be itself.
		 * @param name 
		 * 		The name of the variable as a string
		 * @param typeString 
		 * 		The type of the variable as a string
		 */
		SerializedData(SerializedObject* d, std::string name, std::string typeString, size_t size);

		/**
		 * @brief Destroy the Serialized Data object
		 * 
		 */
		~SerializedData();

		/**
		 * @brief Gets the data stored.
		 * 		Should be cast back to the original type specified by the
		 * 		getType() function.
		 * 
		 * @return void* 
		 */
		void* getData();

		/**
		 * @brief Get the Type of the variable as a string
		 * 
		 * @return std::string 
		 */
		std::string getType();

		/**
		 * @brief Get the Name of the variable as a string
		 * 
		 * @return std::string 
		 */
		std::string getName();

		/**
		 * @brief Get the Number Of Elements object
		 * 
		 * @return size_t 
		 */
		size_t getNumberOfElements();

		bool getObjectType();

		/**
		 * @brief Recreates the data stored as a child JObject of the parentNode provided.
		 * 		If it is a SerializedObject, it will serialize its variables too.
		 * 
		 * 		The type must be a serializable type to be properly saved.
		 * 			(Meaning a valid conversion must exist. This can be provided via a external function through
		 * 			the function setExternalSaver()).
		 * 
		 * @param parentNode 
		 */
		void serialize(Streamable<unsigned char>* data);

		/**
		 * @brief Attempts to set the data stored by using the provided JObject.
		 * 		If it is a SerializedObject, it will deserialize its variables too.
		 * 
		 * 		The type must be a serializable type to be properly loaded.
		 * 			(Meaning a valid conversion must exist. This can be provided via a external function through
		 * 			the function setExternalLoader()).
		 * 
		 * @param node 
		 */
		void deserialize(Streamable<unsigned char>* data);

		/**
		 * @brief adds a new function to save data corresponding to a specific class.
		 * 		
		 * 
		 * @param className 
		 * @param func 
		 */
		static void addSaveFunction(std::string className, std::function<void(std::vector<std::string>, Streamable<unsigned char>*, SerializedData)> func);

		
		/**
		 * @brief adds a new function to load data corresponding to a specific class.
		 * 		
		 * 
		 * @param className 
		 * @param func 
		 */
		static void addLoadFunction(std::string className, std::function<void(std::vector<std::string>, Streamable<unsigned char>*, SerializedData)> func);

	private:
		friend SerializedObject;
		void init(void* d, std::string name, std::string typeString, size_t size);

		std::string name = "";
		std::string type = "";
		size_t size = 1;
		void* data = nullptr;
		bool objType = TYPE_DATA;
		
		static std::unordered_map<std::string, std::function<void(std::vector<std::string>, Streamable<unsigned char>*, SerializedData)>> loadFunctions;
		static std::unordered_map<std::string, std::function<void(std::vector<std::string>, Streamable<unsigned char>*, SerializedData)>> saveFunctions;
	};

	class DLL_OPTION SerializedObject : public Object
	{
	public:

		/**
		 * @brief Construct a new Serialized Object object
		 * 		While it inherits from SerializedData, it does not store any data,
		 * 		type information, or name information by default.
		 * 		Use the preprocessor SERILIZE_NOCLASS for any SerializedObject to give it
		 * 		the name and type info like expected.
		 * 
		 */
		SerializedObject();

		/**
		 * @brief Destroy the Serialized Object
		 * 		The pointer data is assumed to be references to data located on the stack or
		 * 		not intended to be deleted so the references in the variable list are never deleted.
		 * 
		 */
		~SerializedObject();

		/**
		 * @brief Gets a reference to the serialized variables stored by this object.
		 * 		These pointers may not be valid if the object has gone out of scope.
		 * 
		 * @return std::unordered_map<std::string, SerializedData>
		 */
		virtual std::unordered_map<std::string, SerializedData> getSerializedVariables() = 0;

		//class stuff
		static const RootClass globalClass;
		//note that getClass() is still a pure virtual function
		
	private:
		friend SerializedData;
	};
	
	class DLL_OPTION SerializedFactoryMapper
	{
	public:
		static SerializedFactoryMapper& getInstance();

		void registerFactory(const RootClass* c, SerializedFactory* t);
		SerializedFactory* getFactory(const RootClass* c);
		
	private:
		SerializedFactoryMapper() = default;
		SerializedFactoryMapper(const SerializedFactoryMapper&) = delete;
		void operator=(const SerializedFactoryMapper&) = delete;
		
		std::unordered_map<RootClass*, SerializedFactory*> registeredFactories;
	};


	class DLL_OPTION SerializedFactory
	{
	public:
		SerializedFactory(const RootClass* c);
		virtual SerializedData createInstance(Streamable<unsigned char>* data) = 0;
	private:
	};

} //Namespace end


#ifndef SERIALIZE
	#define GET_CLASS_NAME(var) smpl::demangleClassName(typeid(var).name())
	#define SERIALIZE(var) smpl::SerializedData(&var, #var, smpl::demangleClassName(typeid(var).name()), 1)
	#define SERIALIZE_TYPENAME(var, name) smpl::SerializedData(&var, #var, name, 1)
	#define SERIALIZE_POINTER(var, size) smpl::SerializedData(&var, #var, smpl::demangleClassName(typeid(var).name()), size)
	#define SERIALIZE_POINTER_TYPENAME(var, name, size) smpl::SerializedData(&var, #var, typeid(var).name(), size)
	
	#define SERIALIZE_MAP(var) {#var, smpl::SerializedData(&var, #var, smpl::demangleClassName(typeid(var).name()), 1)}
	#define SERIALIZE_MAP_TYPENAME(var, name) {#var, smpl::SerializedData(&var, #var, name, 1)}
	#define SERIALIZE_MAP_POINTER(var, size) {#var, smpl::SerializedData(&var, #var, smpl::demangleClassName(typeid(var).name()), size)}
	#define SERIALIZE_MAP_POINTER_TYPENAME(var, name, size) {#var, smpl::SerializedData(&var, #var, name, size)}
#endif