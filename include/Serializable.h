#pragma once
#include <iostream>
#include <vector>
#include <typeinfo>
#include <functional>
#include "SimpleXml.h"
#include "Object.h"

namespace smpl
{
	class SerializedData;
	class SerializedObject;

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
		static void init();
		
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
		 * @brief Recreates the data stored as a child XmlNode of the parentNode provided.
		 * 		If it is a SerializedObject, it will serialize its variables too.
		 * 
		 * 		The type must be a serializable type to be properly saved.
		 * 			(Meaning a valid conversion must exist. This can be provided via a external function through
		 * 			the function setExternalSaver()).
		 * 
		 * @param parentNode 
		 */
		void serialize(XmlNode* parentNode);

		/**
		 * @brief Attempts to set the data stored by using the provided XmlNode.
		 * 		If it is a SerializedObject, it will deserialize its variables too.
		 * 
		 * 		The type must be a serializable type to be properly loaded.
		 * 			(Meaning a valid conversion must exist. This can be provided via a external function through
		 * 			the function setExternalLoader()).
		 * 
		 * @param node 
		 */
		void deserialize(XmlNode* node);
		
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

		/**
		 * @brief Sets the External Saver function.
		 * 		This is used to save data types that are not the standard primitives typically used. std::string is the exception.
		 * 			i.e. Not an int, long, char, string, etc.
		 * 		An example of this is a std::vector<int>. These are not handled because they are templates.
		 * 		
		 * 		The inputs into the function are the data as a void* and the typename.
		 * 			Note that the typename has not undergone any conversions and a alternate name may be available.
		 * @param extSaver 
		 */
		static void setExternalSaver(std::function<std::string(void*, std::string)> extSaver);

		/**
		 * @brief Sets the External Loader function.
		 * 		This is used to load data types that are not the standard primitives typically used. std::string is the exception.
		 * 			i.e. Not an int, long, char, string, etc.
		 * 		An example of this is a std::vector<int>. These are not handled because they are templates.
		 * 		
		 * 		
		 * 		The inputs into the function are the data to set as a void*, the typename, and the data being loaded as a string.
		 * 			Note that the typename will have the pretty name applied if available.
		 * @param extLoader 
		 */
		static void setExternalLoader(std::function<void(void*, std::string, std::string)> extLoader);

	private:
		friend SerializedObject;
		void init(void* d, std::string name, std::string typeString, size_t size);

		std::string name = "";
		std::string type = "";
		size_t size = 1;
		void* data = nullptr;
		int objType = TYPE_DATA;

		
		static std::unordered_map<std::string, std::string> prettyName;
		static std::function<std::string(void*, std::string)> externalSaver;
		static std::function<void(void*, std::string, std::string)> externalLoader;
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

	private:
		friend SerializedData;
	};

} //Namespace end

#ifndef SERIALIZE
	#define SERIALIZE(var) smpl::SerializedData(&var, #var, typeid(var).name(), 1)
	#define SERIALIZE_TYPENAME(var, name) smpl::SerializedData(&var, #var, name, 1)
	#define SERIALIZE_POINTER(var, size) smpl::SerializedData(&var, #var, typeid(var).name(), size)
	#define SERIALIZE_POINTER_TYPENAME(var, name, size) smpl::SerializedData(&var, #var, typeid(var).name(), size)
	
	#define SERIALIZE_MAP(var) {#var, smpl::SerializedData(&var, #var, typeid(var).name(), 1)}
	#define SERIALIZE_MAP_TYPENAME(var, name) {#var, smpl::SerializedData(&var, #var, name, 1)}
	#define SERIALIZE_MAP_POINTER(var, size) {#var, smpl::SerializedData(&var, #var, typeid(var).name(), size)}
	#define SERIALIZE_MAP_POINTER_TYPENAME(var, name, size) {#var, smpl::SerializedData(&var, #var, name, size)}
#endif