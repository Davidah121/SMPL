#pragma once
#include "BuildOptions.h"
#include "Class.h"

namespace smpl
{
	class DLL_OPTION Object
	{
	public:
		Object();
		~Object();

		virtual const RootClass* getClass() = 0;
		static const RootClass globalClass;
	
	private:
	};

} //NAMESPACE smpl END