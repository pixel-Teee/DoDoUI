cmake_minimum_required(VERSION 3.20.0)

add_executable(unit_tests "main.cpp")

# link libraries
target_link_libraries(unit_tests PRIVATE DoDoUI)

target_include_directories(unit_tests PRIVATE "${CMAKE_SOURCE_DIR}/Source")

if(MSVC)
	message("set working directroy success!")
	set_target_properties(unit_tests PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Test")
endif()

add_test(NAME "1" COMMAND unit_tests 1)
add_test(NAME "2" COMMAND unit_tests 2)
add_test(NAME "3" COMMAND unit_tests 3)

