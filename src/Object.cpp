#include "Object.h"

const Class Object::myClass = Class("Object", {});

const Class* Object::getClass()
{
	return &Object::myClass;
}
