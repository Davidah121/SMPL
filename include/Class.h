#pragma once
#include <vector>
#include <string>

namespace glib
{
	class Class
	{
	public:
		/**
		 * @brief The base Class object.
		 * 		Its purpose is to provide a more object oriented feel similar to Java.
		 * 		It stores a class name, and parent classes and can be accessed from individual objects
		 * 		during run time.
		 * @param name
		 * 		The name of the class. It should be unique.
		 * @param parentClasses
		 * 		The list of parent classes for the current class. It should be a reference to the class object.
		 */
		Class(std::string name = "", std::vector<const Class*> parentClasses = std::vector<const Class*>());

		/**
		 * @brief Destroys a Class object and removes it from the ClassMaster list of all classes.
		 */
		~Class();

		/**
		 * @brief Determines if a class is the parent of this class.
		 * @param k
		 * 		The parent class pointer.
		 * @return bool
		 * 		Returns true if the class was found in its parent list.
		 */
		bool isParentClass(const Class* k) const;

		/**
		 * @brief Returns the name of the class.
		 * @return std::string
		 */
		std::string getClassName() const;
		
		bool operator==(const Class other) const;
		bool operator!=(const Class other) const;

	private:
		std::string className = "";
		std::vector<const Class*> parentClasses = std::vector<const Class*>();
		int classID = 0;
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
		static int addClass(const Class* k);

		/**
		 * @brief Removes a class from the master list. Should not be used outside of Class object
		 * @param k
		 * 		The class pointer to remove.
		 */
		static void removeClass(const Class* k);

		/**
		 * @brief Returns the total number of classes register into the master class list.
		 * @return size_t
		 */
		static size_t getSize();

		/**
		 * @brief Returns the class with the specified index in the master class list.
		 * @param i
		 * 		The index of the list to check.
		 * @return Class*
		 * 		returns a valid class pointer if index was valid. Otherwise, it returns a nullptr.
		 */
		static const Class* findClass(size_t i);

		/**
		 * @brief Returns the class with the specified name in the master class list.
		 * @param className
		 * 		The name of the class to search for.
		 * @return Class*
		 * 		returns a valid class pointer if successful. Otherwise, it returns a nullptr.
		 */
		static const Class* findClass(std::string className);
	private:
		static std::vector<const Class*> allClasses;
		static int maxID;
	};

} //NAMESPACE glib END