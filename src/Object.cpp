#include "Object.h"

namespace glib
{
	const RootClass Object::globalClass = RootClass("Object", {""});

	Object::Object()
	{
		myClass = globalClass;
	}
	
	Object::~Object()
	{

	}

	Class Object::getClass()
	{
		return myClass;
	}

	void Object::setClass(Class c)
	{
		myClass = c;
	}
} //NAMESPACE glib END