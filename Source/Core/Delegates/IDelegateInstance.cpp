#include <PreCompileHeader.h>

#include "IDelegateInstance.h"

namespace Delegates_Private
{
	std::atomic<uint64_t> global_next_id(1);
}


uint64_t FDelegateHandle::Generate_New_Id()
{
	//just increment a counter to generate an id
	uint64_t result = ++Delegates_Private::global_next_id;

	//check for the next-to-impossible event that we wrap round to 0, because we reserve 0 for null delegates
	if (result == 0)
	{
		//increment it again - it might not be zero, so don't just assign it to 1
		result = ++Delegates_Private::global_next_id;
	}

	return result;
}

