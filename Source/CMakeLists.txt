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
"Platform/Window/*.h"
"Platform/Window/*.cpp")

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Do_Do_UI_Source})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Renderer_Source})

# add main.cpp to DoDoUI
add_library(DoDoUI STATIC
"PreCompileHeader.h"
${Do_Do_UI_Source}
${Renderer_Source})

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

# link libraries
target_link_libraries(DoDoUI glfw)

target_include_directories(DoDoUI PRIVATE ".")

# pch
target_precompile_headers(DoDoUI PRIVATE "PreCompileHeader.h")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	message("set linux macro success!")
    target_compile_definitions(DoDoUI PRIVATE -DLinux)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	message("set windows macro success!")
	target_compile_definitions(DoDoUI PRIVATE -DWIN32)
elseif(CMAKE_SYSTEN_NAME MATCHES "Android")
	message("set android macro success!")
	target_compile_definitions(DoDoUI PRIVATE -DAndroid)
endif()