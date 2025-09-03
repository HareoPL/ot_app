cmake_minimum_required(VERSION 3.17)
project(unit_test C)

set(PROJECT_TARGETS_LIST "" CACHE INTERNAL "Target list")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(UNIT_TEST)

add_subdirectory(../external/unity unity)


# add tests subdirectory:
# add_subdirectory(HOST_template_test)
add_subdirectory(HOST_ot_app_pair_test)
add_subdirectory(HOST_ot_app_deviceName_test)


message(STATUS "------------------------------------------------ Project targets list: ")
foreach(list ${PROJECT_TARGETS_LIST})
    message(STATUS "                 ${list}")
endforeach()
message(STATUS " ")


