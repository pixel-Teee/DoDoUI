cmake_minimum_required(VERSION 3.20.0)

file(GLOB_RECURSE Do_Do_UI_Source
"SlateCore/Layout/*.h"
"SlateCore/Layout/*.cpp"
"SlateCore/Widgets/*.h"
"SlateCore/Widgets/*.cpp"
"SlateCore/Rendering/*.h"
"SlateCore/Rendering/*.cpp"
"Core/*.h"
"Core/*.cpp"
"Core/Math/*.h"
"Core/Math/*.cpp"
"Application/*.h"
"Application/*.cpp"
)

file(GLOB_RECURSE Renderer_Source
"Renderer/*.h"
"Renderer/*.cpp"
"Platform/VulkanRenderer/*.h"
"Platform/VulkanRenderer/*.cpp"
)

if(NOT CMAKE_SYSTEN_NAME MATCHES "Android")
	file(GLOB Glfw_Window "Platform/Window/WindowsWindow.h" "Platform/Window/WindowsWindow.cpp")
	# message(${Glfw_Window})
	list(APPEND Renderer_Source ${Glfw_Window})
else()
	file(GLOB Android_Window "Platform/Window/AndroidWindow.h" "Platform/Window/AndroidWindow.cpp")
	# list(APPEND Renderer_Source ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
	list(APPEND Renderer_Source ${Android_Window})
endif()

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Do_Do_UI_Source})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Renderer_Source})

# add main.cpp to DoDoUI
add_library(DoDoUI STATIC
"PreCompileHeader.h"
${Do_Do_UI_Source}
${Renderer_Source})

if(CMAKE_SYSTEM_NAME MATCHES "Android")
	# message(${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
	target_sources(DoDoUI PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
endif()

# ------find vulkan------
find_package(Vulkan)

if(${Vulkan_FOUND})
	target_link_libraries(DoDoUI Vulkan::Vulkan)
else()
	message("not find vulkan!")
endif()
# ------find vulkan------

# ------control debug and release------
if(CMAKE_BUILD_TYPE STREQUAL Debug)
	message("set debug macro success!")
	target_compile_definitions(DoDoUI PRIVATE -DDEBUG)
endif()
# ------control debug and release------

if(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
	# link libraries
	target_link_libraries(DoDoUI glfw)
else()
	find_package(game-activity REQUIRED CONFIG)
	find_library(log-lib log)
	target_link_libraries(DoDoUI android game-activity::game-activity ${log-lib})
endif()

if(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
	target_include_directories(DoDoUI PRIVATE ".")
else()
	# message(${ANDROID_NDK}/sources/android/native_app_glue/)
	target_include_directories(DoDoUI PRIVATE "." ${ANDROID_NDK}/sources/android/native_app_glue)
endif()
# pch
target_precompile_headers(DoDoUI PRIVATE "PreCompileHeader.h")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	message("set linux macro success!")
    target_compile_definitions(DoDoUI PRIVATE -DLinux)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	message("set windows macro success!")
	target_compile_definitions(DoDoUI PRIVATE -DWIN32)
elseif(CMAKE_SYSTEM_NAME MATCHES "Android")
	message("set android macro success!")
	target_compile_definitions(DoDoUI PUBLIC -DAndroid)
endif()