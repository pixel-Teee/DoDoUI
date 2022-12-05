cmake_minimum_required(VERSION 3.20.0)

# set c++ standard
set(CMAKE_CXX_STANDARD 17)

project(DoDoUI VERSION 1.0.0 LANGUAGES CXX)

# source dir and binary dir
add_subdirectory(Source Do_Do_UI)

if(NOT CMAKE_SYSTEM_NAME MATCHES "Android")
    # add third party
    add_subdirectory(ThirdParty/glfw GLFW)
endif()

add_subdirectory(BootStrap Boot_Strap)