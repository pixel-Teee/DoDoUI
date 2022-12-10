#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <optional>
#include <set>
#include <algorithm>
#include <fstream>
#include <filesystem>

#ifdef Android
    #include <android/log.h>
    static const char* my_tag = "DoDoUI";
    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, my_tag, __VA_ARGS__))
    #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, my_tag, __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, my_tag, __VA_ARGS__))
#else
    //todo:in the future, to use spdlog
    #define LOGI(...) printf(__VA_ARGS__)
    #define LOGW(...) printf(__VA_ARGS__)
    #define LOGE(...) printf(__VA_ARGS__)
#endif
