#pragma once
#include "Class.h"

namespace smpl
{
	class Object
	{
	public:
		Object();
		~Object();

		virtual const RootClass* getClass() = 0;
		static const RootClass globalClass;
	
	private:
	};

} //NAMESPACE smpl END