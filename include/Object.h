#pragma once
#include "Class.h"

namespace glib
{

	class Object
	{
	public:
		virtual const Class* getClass();
		static const Class myClass;
	};

} //NAMESPACE glib END