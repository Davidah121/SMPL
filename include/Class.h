#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <map>

namespace glib
{
	class Class;
	class ClassMaster;
	class RootClass;

	class RootClass
	{
	public:

		/**
		 * @brief Construct a new Root Class object
		 * 		There should only exist one per class and it should be static.
		 * 		When created, it adds itself to the master class register getting an ID.
		 * 
		 * 		All classes (normal classes) are created from a root class which just passes the ID along.
		 * 
		 * 		Name must be unique.
		 * 		Parent names must be unique as well and a class that will be defined
		 * 			(static definition is in no particular order.)
		 * 
		 * @param name 
		 * @param parentClassesByName 
		 */
		RootClass(std::string name, std::unordered_set<std::string> parentClassesByName);

		/**
		 * @brief Destroy the Root Class object
		 * 		Should only be destroyed at the end of a program when all static objects are destroyed.
		 */
		~RootClass();

		int getID() const;
		std::string getName() const;
		std::unordered_set<std::string> getListOfParents() const;
	private:
		friend ClassMaster;
		int id = -1;
		std::string name = "";
		std::unordered_set<std::string> parentClassesByName; //Stores name of parent classes. May not exist
	};

	class Class
	{
	public:
		/**
		 * @brief The base Class object.
		 * 		Its purpose is to provide a more object oriented feel similar to Java.
		 * 		It stores a class ID which can be used to get the name and list of parent classes by id.
		 * 			This allows the class to use much less data and therefore less copying giving back performance.
		 * @param name
		 * 		The name of the class. It should be unique.
		 * @param parentClasses
		 * 		The list of parent classes for the current class. It should be a reference to the class object.
		 */
		Class(const RootClass& rootClass);

		/**
		 * @brief Construct a new Class
		 * 		This is an invalid class as it has an ID of -1
		 * 
		 */
		Class();

		/**
		 * @brief Destroys a Class object and removes it from the ClassMaster list of all classes.
		 */
		~Class();

		/**
		 * @brief Returns the name of the class.
		 * @return std::string
		 */
		std::string getClassName() const;

		/**
		 * @brief Returns the class ID given by the ClassMaster class.
		 * 		This must be >= 0 to be valid. 
		 * 		If < 0, it is invalid for some reason or another.
		 * 
		 * @return int 
		 */
		int getClassID();

		const std::unordered_set<std::string> getParentClasses();

		const RootClass* getRootClass();
		
		
		bool operator==(const Class other) const;
		bool operator!=(const Class other) const;
		
		bool operator==(const RootClass& other) const;
		bool operator!=(const RootClass& other) const;

	private:
		int classID = -1;
	};

	class ClassMaster
	{
	public:
		struct InvalidClassName : public std::exception
		{
			const char* what() const noexcept { return "The class name can not be used."; }
		};

		/**
		 * @brief Adds a class to the master list. Should not be used outside of Class object
		 * @param k
		 * 		The class pointer to add.
		 * @return int
		 * 		Returns a positive value if successful.
		 * 		The value returned is the id of the class.
		 */
		static int addClass(const RootClass* k);

		/**
		 * @brief Returns the total number of classes register into the master class list.
		 * @return size_t
		 */
		static size_t getSize();

		static const RootClass* getRootClass(std::string name);
		static const RootClass* getRootClass(int id);
	private:
		ClassMaster();
		~ClassMaster();

		std::map<std::string, int> nameToClassIndex;
		std::vector<const RootClass*> allClasses;

		static ClassMaster* getInstance();
		static ClassMaster* singleton;
	};

} //NAMESPACE glib END