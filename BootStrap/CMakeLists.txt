cmake_minimum_required(VERSION 3.20.0)

if(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
	add_executable(BootStrap "main.cpp")
else()
	add_library(BootStrap SHARED "main.cpp")
endif()

# link libraries
target_link_libraries(BootStrap PRIVATE DoDoUI)

# include glm to test for SWidget create
target_include_directories(BootStrap PRIVATE "${CMAKE_SOURCE_DIR}/Source" PRIVATE "${CMAKE_SOURCE_DIR}/ThirdParty/glm")

if(CMAKE_SYSTEM_NAME MATCHES "Android")
	target_include_directories(BootStrap PRIVATE "${ANDROID_NDK}/sources/android/native_app_glue")
endif()

if(MSVC)
	message("set working directroy success!")
	set_target_properties(BootStrap PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/BootStrap")
endif()
