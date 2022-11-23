#include "Class.h"
#include <iostream>

#pragma region CLASS

namespace glib
{

	Class::Class(std::string name, std::vector<const Class*> parentClasses)
	{
		this->className = name;
		this->parentClasses = parentClasses;
		if (name != "")
		{
			classID = ClassMaster::addClass(this);
		}
	}

	Class::~Class()
	{
		ClassMaster::removeClass(this);
		className.clear();
		parentClasses.clear();
		classID = -1;
	}

	bool Class::isParentClass(const Class* k) const
	{
		bool is = false;
		if(k != nullptr)
		{
			for (int i = 0; i < parentClasses.size(); i++)
			{
				if (k == parentClasses[i])
				{
					is = true;
					break;
				}
			}
		}
		return is;
	}

	std::string Class::getClassName() const
	{
		return className;
	}

	bool Class::operator==(const Class other) const
	{
		return classID == other.classID;
	}
	
	bool Class::operator!=(const Class other) const
	{
		return classID != other.classID;
	}

	#pragma endregion

	#pragma region CLASS_MASTER

	std::vector<const Class*> ClassMaster::allClasses = std::vector<const Class*>();
	int ClassMaster::maxID = 0;

	int ClassMaster::addClass(const Class* k)
	{
		//Check if the class already exists or if the name already exists
		bool can = true;
		for (int i = 0; i < allClasses.size(); i++)
		{
			if (k->getClassName() == allClasses[i]->getClassName())
			{
				//Can't have more than one class with the same name.
				can = false;
				break;
			}
		}

		if (can)
		{
			allClasses.push_back(k);
		}
		else
		{
			#ifdef USE_EXCEPTIONS
			throw ClassMaster::InvalidClassName;
			#endif
			//No exit of fail
		}

		if(can)
		{
			maxID++;
			return maxID;
		}
		return -1;
	}

	void ClassMaster::removeClass(const Class* k)
	{
		int index = -1;
		for (int i = 0; i < allClasses.size(); i++)
		{
			if (allClasses[i] == k)
			{
				index = i;
				break;
			}
		}

		if (index != -1)
		{
			for (int i = index; i < allClasses.size() - 1; i++)
			{
				allClasses[i] = allClasses[i + 1];
			}

			allClasses.pop_back();
		}
	}

	int ClassMaster::getSize()
	{
		return allClasses.size();
	}

	const Class* ClassMaster::findClass(int i)
	{
		if (i >= 0 && i < allClasses.size())
		{
			return allClasses[i];
		}
		return nullptr;
	}

	const Class* ClassMaster::findClass(std::string className)
	{
		for(int i=0; i<allClasses.size(); i++)
		{
			if(allClasses[i]->getClassName() == className)
				return allClasses[i];
		}
		return nullptr;
	}

	#pragma endregion

} //NAMESPACE glib END