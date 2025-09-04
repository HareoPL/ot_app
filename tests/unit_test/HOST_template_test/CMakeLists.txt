# cmake -DENABLE_ANALYSIS=OFF -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE --no-warn-unused-cli -S. -B./build/template -G Ninja
# cmake --build ./out/ --config Debug --target template_test

# project/target name is as folder name
# automatically finds source files (*.c) in current folder

cmake_minimum_required(VERSION 3.17)

set(SRCS)
set(INCLUDE_DIRS)

list(APPEND INCLUDE_DIRS
	# ADD your include dir here
	# ../../../components/libs/open_thread/ot_app/inc/
	../../../components/utils
	# ../../../main
)

file(GLOB_RECURSE SRCS
    # ../../../components/libs/open_thread/ot_app/src/*.c  
)

list(APPEND SRCS
	# ADD your source file here ex. ../test.c	
	../../../components/utils/hro_utils.c
	# ../../../components/libs/open_thread/ot_app/src/ot_app_pair.c
	# ../../../main/main.c

)


###########################################
############ do not edit below ############

get_filename_component(PROJECT_NAME_AS_DIR ${CMAKE_CURRENT_LIST_DIR} NAME)
project(${PROJECT_NAME_AS_DIR} C)  # project/target name as catalog name

# add target name to global variable
list(APPEND PROJECT_TARGETS_LIST ${PROJECT_NAME_AS_DIR})
set(PROJECT_TARGETS_LIST "${PROJECT_TARGETS_LIST}" CACHE INTERNAL "Target lists")

if(ENABLE_ANALYSIS)
	set(CPPCHECK_CONFIG
		"--enable=warning,style,performance,portability,information,missingInclude"
		"--force" 
		"--inline-suppr"
		"--output-file=cppcheck.out"
	)

	set(CLANG_TIDY_CONFIG
		"-checks=-*,cert-*,clang-analyzer-*,performance-*,portability-*,readability-*,bugprone-*,misc-*"
		"--export-fixes=clang-tidy.out"
	)

	find_program(CMAKE_C_CPPCHECK NAMES cppcheck)
	if (CMAKE_C_CPPCHECK)
		list(APPEND CMAKE_C_CPPCHECK ${CPPCHECK_CONFIG})
	endif()

	find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
	if (CMAKE_CXX_CPPCHECK)
		list(APPEND CMAKE_CXX_CPPCHECK ${CPPCHECK_CONFIG})
	endif()

	find_program(CMAKE_C_CLANG_TIDY NAMES clang-tidy)
	if (CMAKE_C_CLANG_TIDY)
		list(APPEND CMAKE_C_CLANG_TIDY ${CLANG_TIDY_CONFIG})
	endif()

	find_program(CMAKE_CXX_CLANG_TIDY NAMES clang-tidy)
	if (CMAKE_CXX_CLANG_TIDY)
		list(APPEND CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_CONFIG})
	endif()

endif()

set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")


set(TEST_INCLUDE_DIRS
	.
	mocks/
)

file(GLOB_RECURSE SRC_GLOB
	*.c	
	mocks/*.c	
)
list(FILTER SRC_GLOB EXCLUDE REGEX ".*/out/.*")
list(PREPEND SRCS ${SRC_GLOB})

set(GLOBAL_DEFINES

)

add_definitions(${GLOBAL_DEFINES})

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} fff)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIRS}
    ${TEST_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} unity)

target_compile_options(${PROJECT_NAME} PRIVATE -fprofile-arcs -ftest-coverage)
target_link_options(${PROJECT_NAME} PRIVATE -fprofile-arcs)

if(ENABLE_PRINT_SRCS_FILE)
	message(STATUS " ")
	message(STATUS "------------------------------------------------ ${PROJECT_NAME}: ")
	message(STATUS "                  SRCS file list for target: ${PROJECT_NAME}")
	message(STATUS " ")
	foreach(src_file ${SRCS})
	message(STATUS "                  ${src_file}")
	endforeach()

	message(STATUS " ")
endif()
