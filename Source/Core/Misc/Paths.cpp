#include <PreCompileHeader.h>

#include "Paths.h"

#include "Core/GenericPlatform/GenericPlatformMisc.h"

#include <filesystem> //filesystem

namespace DoDo {
	DoDoUtf8String FPaths::engine_dir()
	{
		return DoDoUtf8String(FGenericPlatformMisc::engine_dir());
	}
	DoDoUtf8String FPaths::engine_content_dir()
	{
		return FPaths::engine_dir();//todo:need to fix
	}
	bool FPaths::directory_exists(const DoDoUtf8String& in_path)
	{
		//return false;
		//todo:call file manager function
		std::filesystem::directory_entry entry(in_path.c_str());

		return entry.exists();
	}
}