#include "PreCompileHeader.h"

#include "WindowsPlatformMisc.h"

#include "Combaseapi.h"//windows platform depend

namespace DoDo {
	void FWindowsPlatformMisc::create_guid(FGuid& result)
	{
		CoCreateGuid((GUID*)&result);
	}
}