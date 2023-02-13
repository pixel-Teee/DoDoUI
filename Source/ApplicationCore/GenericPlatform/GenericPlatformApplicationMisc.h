#pragma once

namespace DoDo {
	struct FGenericPlatformApplicationMisc
	{
		static void platform_pre_init(){}
		/*
		* returns monitor's DPI scale factor at given screen coordinates(expressed in pixels)
		* @return monitor's DPI scale factor at given point
		*/
		static float get_dpi_scale_factor_at_point(float x, float y)
		{
			return 1.0f;
		}
	};
}