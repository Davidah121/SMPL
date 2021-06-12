#pragma once
#include "Class.h"

class Object
{
public:
	virtual const Class* getClass();
	static const Class myClass;
};

