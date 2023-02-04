#pragma once

#include "Core/String/DoDoString.h"

namespace DoDo {
	/*
	* path helpers for retrieving game dir, engine dir, etc
	*/
	class FPaths
	{
	public:
		/*
		* returns the base directroy of the "core" engine that can be shared across
		* several games or across games & mods, shaders and base localization files
		* e.g. reside in the engine directory
		* 
		* @return engine directory
		*/
		static DoDoUtf8String engine_dir();

		/*
		* returns the content directory of the "core" engine that can be shared across
		* several games or across games & mods
		* 
		* @return engine content directory
		*/
		static DoDoUtf8String engine_content_dir();
	};
}