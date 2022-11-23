#pragma once
#include "Class.h"

namespace glib
{

	class Object
	{
	public:
		Object();
		~Object();

		const Class& getClass();
		static const Class globalClass;
	
	protected:
		void setClass(Class c)
		{
			myClass = c;
		}
	private:
		Class myClass;
	};

} //NAMESPACE glib END