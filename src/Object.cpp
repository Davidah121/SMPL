#include "Object.h"

namespace glib
{
	const Class Object::globalClass = Class("Object", {});

	Object::Object()
	{
		myClass = this->globalClass;
	}
	Object::~Object()
	{

	}

	const Class& Object::getClass()
	{
		return myClass;
	}
} //NAMESPACE glib END