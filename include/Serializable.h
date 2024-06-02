#pragma once
#include <iostream>
#include <vector>
#include <typeinfo>
#include <functional>
#include "Object.h"

namespace smpl
{
	class SerializedData;
	class SerializedObject;

	struct WritableSerialzedData
	{
		static const bool TYPE_PRIMITIVE = false;
		static const bool TYPE_VECTOR = true;
		
		bool type = 0;
		unsigned int size = 0; //size of the data in bytes
		unsigned char* data = nullptr; //the data as bytes
		unsigned int numberOfOffsets = 0;
		unsigned int* offsets = nullptr;
	};

	//so for every object
	//type, name size, name bytes, numberOfOffsets, list of offsets
	//for every primitive
	//type, size, data
	//Total required space for an object is 9 bytes + n bytes for name and k bytes for the offsets. Those can actually be 0 so at least 9 is required.
	//Total required space for a primitive is 5 bytes + n bytes for the data. This too can also be 0 so at least 5 bytes is required.
	//for each function, pass in a vector of unsigned bytes or an array of unsigned bytes with a size
	class SerializedData
	{
	public:
		static const int TYPE_DATA = 0;
		static const int TYPE_OBJECT = 1;

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
		void serialize(std::vector<unsigned char>& data, int offset);

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
		void deserialize(std::vector<unsigned char>& data, int offset);
		
		/**
		 * @brief Sets an alias for the specified class name.
		 * 		Some classes have alternate names that may be prefered where possible.
		 * 			Ex: std::string vs class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
		 * 		This will replace that class name with the pretty name when saving and loading.
		 * 		Good for portability since typeid(typename).name() is not guaranteed to be the same across implementations.
		 * 
		 * 		There can only be one alternate name for a class.
		 * 		New alternate names replace the previous names.
		 * 			Meaning that all pretty names can be the same as one class name but the class name can only have one alternate.
		 * 
		 * @param className 
		 * @param prettyName 
		 */
		static void addPrettyName(std::string className, std::string prettyName);

		/**
		 * @brief Gets an alternate name for the specified class.
		 * 		There can only be one alternate name for a class.
		 * 		New alternate names replace the previous names.
		 * 
		 * @param className 
		 * @return std::string 
		 */
		static std::string getAlternateName(std::string className);

	private:
		friend SerializedObject;
		void init(void* d, std::string name, std::string typeString, size_t size);

		std::string name = "";
		std::string type = "";
		size_t size = 1;
		void* data = nullptr;
		int objType = TYPE_DATA;
		
		static std::unordered_map<std::string, std::string> prettyName;
	};

	class SerializedObject : public Object
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

} //Namespace end

#ifndef SERIALIZE
	#define SERIALIZE(var) smpl::SerializedData(&var, #var, smpl::demangleClassName(typeid(var).name()), 1)
	#define SERIALIZE_TYPENAME(var, name) smpl::SerializedData(&var, #var, name, 1)
	#define SERIALIZE_POINTER(var, size) smpl::SerializedData(&var, #var, smpl::demangleClassName(typeid(var).name()), size)
	#define SERIALIZE_POINTER_TYPENAME(var, name, size) smpl::SerializedData(&var, #var, typeid(var).name(), size)
	
	#define SERIALIZE_MAP(var) {#var, smpl::SerializedData(&var, #var, smpl::demangleClassName(typeid(var).name()), 1)}
	#define SERIALIZE_MAP_TYPENAME(var, name) {#var, smpl::SerializedData(&var, #var, name, 1)}
	#define SERIALIZE_MAP_POINTER(var, size) {#var, smpl::SerializedData(&var, #var, smpl::demangleClassName(typeid(var).name()), size)}
	#define SERIALIZE_MAP_POINTER_TYPENAME(var, name, size) {#var, smpl::SerializedData(&var, #var, name, size)}
#endif