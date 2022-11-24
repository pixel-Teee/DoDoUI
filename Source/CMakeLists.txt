cmake_minimum_required(VERSION 3.20.0)

file(GLOB_RECURSE Do_Do_UI_Source
"SlateCore/Layout/*.h"
"SlateCore/Layout/*.cpp"
"SlateCore/Widgets/*.h"
"SlateCore/Widgets/*.cpp"
)

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Do_Do_UI_Source})

# add main.cpp to DoDoUI
add_executable(DoDoUI
"main.cpp"
"PreCompileHeader.h"
${DoDo_UI_Source})

# ------find vulkan------
find_package(Vulkan)

if(${Vulkan_FOUND})
	target_link_libraries(DoDoUI Vulkan::Vulkan)
else()
	message("not find vulkan!")
endif()
# ------find vulkan------

# link libraries
target_link_libraries(DoDoUI glfw)

target_include_directories(DoDoUI PRIVATE ".")

# pch
target_precompile_headers(DoDoUI PRIVATE "PreCompileHeader.h")
