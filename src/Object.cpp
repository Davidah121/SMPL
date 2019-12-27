#include "Object.h"

const Class* Object::myClass = new Class("Object", {});

const Class* Object::getClass()
{
	return Object::myClass;
}
