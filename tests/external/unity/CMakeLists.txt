cmake_minimum_required(VERSION 3.10)

add_library(unity STATIC
	core/unity.c
	fixture/unity_fixture.c
)

target_include_directories(unity PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/core
    ${CMAKE_CURRENT_LIST_DIR}/fixture
)


target_compile_options(unity PRIVATE "-Wall")
