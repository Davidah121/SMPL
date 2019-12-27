#pragma once
#include <vector>
#include <string>

//Every class should implement this class if you want to
//track it or determine it's class and parents
class Class
{
public:
	Class(std::string name = "", std::vector<const Class*> parentClasses = std::vector<const Class*>());
	~Class();
	//Determines if a class is the parent of this class
	bool isParentClass(const Class* k) const;
	std::string getClassName() const;

private:
	std::string className = "";
	std::vector<const Class*> parentClasses = std::vector<const Class*>();
};

class ClassMaster
{
public:
	static bool addClass(const Class* k);
	static void removeClass(const Class* k);
	static int getSize();
	static const Class* findClass(int i);
	static const Class* findClass(std::string className);
private:
	static std::vector<const Class*> allClasses;
};
