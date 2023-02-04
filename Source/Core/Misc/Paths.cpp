#include <PreCompileHeader.h>

#include "Paths.h"

#include "Core/GenericPlatform/GenericPlatformMisc.h"

namespace DoDo {
	DoDoUtf8String FPaths::engine_dir()
	{
		return DoDoUtf8String(FGenericPlatformMisc::engine_dir());
	}
	DoDoUtf8String FPaths::engine_content_dir()
	{
		return FPaths::engine_dir() / "Content/";
	}
}