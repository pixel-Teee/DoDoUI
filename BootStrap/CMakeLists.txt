cmake_minimum_required(VERSION 3.20.0)

add_executable(BootStrap
"main.cpp")

# link libraries
target_link_libraries(BootStrap PRIVATE DoDoUI)

target_include_directories(BootStrap PRIVATE "${CMAKE_SOURCE_DIR}/Source")

if(MSVC)
	message("set working directroy success!")
	set_target_properties(BootStrap PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()
