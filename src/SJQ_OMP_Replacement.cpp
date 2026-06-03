#include "SimpleJobQueue.h"

namespace smpl
{
	void SJQ_OMP_Replacement::parallelize(const std::function<void(size_t, size_t, size_t)>& func, size_t start, size_t end, size_t incr, size_t threadsWanted)
	{
		ompReplacement.parallelize(func, start, end, incr, threadsWanted);
	}
}