#pragma once
#include <iostream>
#include <vector>
#include <typeinfo>
#include "SimpleXml.h"
#include <functional>

namespace glib
{
	class SerializedData;
	class SerializedObject;

	class SerializedData
	{
	public:
		static const int TYPE_DATA = 0;
		static const int TYPE_OBJECT = 1;

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
		SerializedData(void* d, std::string name, std::string typeString);

		/**
		 * @brief Construct a new Serialized Data object
		 * 
		 * @param d 
		 * 		Assumes that d is a pointer to the data
		 * @param name 
		 * 		The name of the variable as a string
		 * @param typeString 
		 * 		The type of the variable as a string
		 * @param SerializedObject*
		 * 		The parent object for this SerializedData variable
		 * 		Gets added to the parent's list of variables.
		 */
		SerializedData(void* d, std::string name, std::string typeString, SerializedObject* parent);

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
		SerializedData(SerializedObject* d, std::string name, std::string typeString);

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
		 * @param SerializedObject*
		 * 		The parent object for this SerializedData variable
		 * 		Gets added to the parent's list of variables.
		 */
		SerializedData(SerializedObject* d, std::string name, std::string typeString, SerializedObject* parent);

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

	private:
		friend SerializedObject;
		/**
		 * @brief Construct a new Serialized Data object
		 * 		Empty Constructor used by SerializedObject.
		 * 
		 */
		SerializedData();
		std::string name = "";
		std::string type = "";
		void* data = nullptr;
		SerializedObject* parent = nullptr;
		int objType = TYPE_DATA;
	};

	class SerializedObject : public SerializedData
	{
	public:

		/**
		 * @brief Adds a few default pretty names.
		 * 		Optional
		 * 
		 */
		static void init();

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
		 * @return std::vector<SerializedData*>& 
		 */
		std::vector<SerializedData*>& getVariables();

		/**
		 * @brief Serializes the object into an XML file specified.
		 * 		Note that the type and name may be empty if SERIALIZE or SERIALIZE_NOCLASS
		 * 		is not used on this object. This is normal behavior and will not affect the
		 * 		deserialization function.
		 * 
		 * @param s 
		 */
		void serialize(std::string s);

		/**
		 * @brief Deserializes the object from an XML file specified.
		 * 		Assumes that this is the main object in the XML file meaning that it is the
		 * 		first child node off of the SERIALIZER title.
		 * 		Assumes that the order the variables are stored in the file is the same in memory.
		 * 		An error can occur otherwise.
		 * 
		 * @param s 
		 */
		void deserialize(std::string s);
		
		/**
		 * @brief Serializes the object as an XmlNode under the parent node.
		 * 		Useful for more custom file formatting and better loading conditions.
		 * 
		 * @param parent 
		 */
		void serialize(XmlNode* parent);

		/**
		 * @brief Attempts to deserialize the object from the specified XmlNode.
		 * 		Assumes that the order the variables are stored in the XmlNode is the same in memory.
		 * 
		 * @param n 
		 */
		void deserialize(XmlNode* n);

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
		friend SerializedData;
		static std::unordered_map<std::string, std::string> prettyName;
		static std::function<std::string(void*, std::string)> externalSaver;
		static std::function<void(void*, std::string, std::string)> externalLoader;

		std::vector<SerializedData*> variables;
	};

} //Namespace end

#ifndef SERIALIZE
	#define SERIALIZE(var) SerializedData SERIALIZED__##var##__ = glib::SerializedData(&var, #var, typeid(var).name(), this)
	#define SERIALIZE_TYPENAME(var, name) SerializedData SERIALIZED__##var##__ = glib::SerializedData(&var, #var, name, this)
	#define SERIALIZE_NOCLASS(var) SerializedData SERIALIZED__##var##__ = glib::SerializedData(&var, #var, typeid(var).name())
	#define SERIALIZE_TYPENAME_NOCLASS(var, name) SerializedData SERIALIZED__##var##__ = glib::SerializedData(&var, #var, name)
#endif