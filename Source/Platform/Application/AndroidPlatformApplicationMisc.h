#pragma once

#include "ApplicationCore/GenericPlatform/GenericPlatformApplicationMisc.h"

namespace DoDo{
    struct FAndroidPlatformApplicationMisc : public FGenericPlatformApplicationMisc
    {
        static void platform_pre_init();
        /*get glfw content scale*/
        static float get_dpi_scale_factor_at_point(float x, float y);
    };

#ifdef Android
    typedef FAndroidPlatformApplicationMisc FPlatformApplicationMisc;
#endif
}