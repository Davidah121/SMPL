#include "Object.h"

namespace glib
{

	const Class Object::myClass = Class("Object", {});
	const Class* Object::getClass()
	{
		return &Object::myClass;
	}

} //NAMESPACE glib END