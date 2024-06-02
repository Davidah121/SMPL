#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <map>
#include <typeinfo>

namespace smpl
{
	// class Class;
	class ClassMaster;
	class RootClass;

	std::string demangleClassName(std::string name);

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
		RootClass(std::string name, int sizeOfClass, std::unordered_set<const RootClass*> parentClasses);

		/**
		 * @brief Destroy the Root Class object
		 * 		Should only be destroyed at the end of a program when all static objects are destroyed.
		 */
		~RootClass();

		int getID() const;
		std::string getName() const;
		std::unordered_set<const RootClass*> getListOfParents() const;
		
		bool isDerivedFrom(const RootClass* rootClass) const;
		int getSizeOfClass() const;
	private:
		friend ClassMaster;
		int id = -1;
		std::string name = "";
		int sizeOfClass = 0;
		std::unordered_set<const RootClass*> parentClasses; //Stores name of parent classes. May not exist
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

	#ifndef CREATE_ROOT_CLASS
	#define CREATE_ROOT_CLASS(ClassName, ...) RootClass(typeid(ClassName).name(), sizeof(ClassName), {__VA_ARGS__})
	#endif

} //NAMESPACE smpl END