#include "Core/GenericPlatform/GenericPlatformMisc.h"

namespace DoDo {
	/*
	* windows implementation of the misc os functions
	*/
	struct FWindowsPlatformMisc : public FGenericPlatformMisc
	{
		static void create_guid(struct FGuid& result);
	};

#ifdef WIN32
	typedef FWindowsPlatformMisc FPlatformMisc;
#endif
}