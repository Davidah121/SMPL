#pragma once
#include "Class.h"

namespace smpl
{
	class Object
	{
	public:
		Object();
		~Object();

		Class getClass();
		static const RootClass globalClass;
	
	protected:
		void setClass(Class c);
	private:
		Class myClass;
	};

} //NAMESPACE smpl END