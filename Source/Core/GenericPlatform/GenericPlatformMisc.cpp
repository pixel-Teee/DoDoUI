#include <PreCompileHeader.h>

#include "GenericPlatformMisc.h"

#include "Core/String/DoDoString.h"

#include "Core/Misc/LazySingleton.h"//TLazySingleton

namespace DoDo {
	struct FGenericPlatformMisc::FStaticData
	{
		DoDoUtf8String engine_dir;
	};

	static void make_engine_dir(DoDoUtf8String& out_engine_dir)
	{
		//see if we are a root-level project
		//if this is not root-level project, then use peer directory(for binary)
		DoDoUtf8String default_engine_dir = "../../Content/";

		//get current directory
		std::filesystem::path current_working_directory = std::filesystem::current_path();
		DoDoUtf8String cwd_string(current_working_directory.string());

		//DoDoUtf8String dir_to_try = cwd_string / default_engine_dir;//check

		DoDoUtf8String dir_to_try = cwd_string / default_engine_dir;

		if (std::filesystem::exists(dir_to_try.c_str()))
		{
			out_engine_dir = dir_to_try;
		}
		else
		{
			out_engine_dir = cwd_string / "Content/";
		}
	}

	const char* FGenericPlatformMisc::engine_dir()//todo:may be need fix
	{
		//this is a absolute directory

		DoDoUtf8String& engine_directory = TLazySingleton<FStaticData>::Get().engine_dir;

		if (engine_directory.get_length() == 0)
		{
			make_engine_dir(engine_directory);
		}

		return engine_directory.c_str();
		//convert to wchar_t
	}
}