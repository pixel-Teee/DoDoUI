cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_GENERATOR_TOOLSET "ClangCL")

if(${ENABLE_TEST})
    enable_testing()
endif()
# set c++ standard
set(CMAKE_CXX_STANDARD 17)

project(DoDoUI VERSION 1.0.0 LANGUAGES CXX)

# need to place to add_executable before
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message("set msvc utf8 successful!")
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

# ------set vulkan------
set(vulkan_include ${CMAKE_SOURCE_DIR}/ThirdParty/vulkan/include)
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(vulkan_lib ${CMAKE_SOURCE_DIR}/ThirdParty/vulkan/lib/Win32/vulkan-1.lib)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(vulkan_lib ${CMAKE_SOURCE_DIR}/ThirdParty/vulkan/lib/Linux/libvulkan.so.1)
endif()
# ------set vulkan------

# source dir and binary dir
add_subdirectory(Source Do_Do_UI)

if(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    # add third party
    add_subdirectory(ThirdParty/glfw GLFW)
endif()

if(${ENABLE_TEST})
    add_subdirectory(Test unit_tests)
else()
    add_subdirectory(BootStrap Boot_Strap)
endif()