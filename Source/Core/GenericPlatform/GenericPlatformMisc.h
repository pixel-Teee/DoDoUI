#pragma once

namespace DoDo {
	/*
	* generic implementation for most platforms
	*/
	class FGenericPlatformMisc
	{


	public:
		/*get the engine directory*/
		static const char* engine_dir();//todo:may be use TCHAR

	private:
		struct FStaticData;
	};
}