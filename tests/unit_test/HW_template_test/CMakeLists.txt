cmake_minimum_required(VERSION 3.10)

project(pilot_test C)

add_subdirectory(../unity unity)

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

set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(INCLUDE_DIRS
	../../src
)

set(TEST_INCLUDE_DIRS
	..
	.
)

set(SRCS
	../../src/remote/remote_rx/remote_rx.c
	../../src/remote/remote_rx/internal/remote_rx_start.c
	../../src/remote/remote_rx/internal/remote_rx_data.c
	../../src/remote/remote_rx/internal/remote_rx_footer.c
	../../src/remote/remote_cmd/remote_cmd.c
	../../src/remote/remote.c
	../../src/osal/dummy_os.c
)

set(TEST_SRCS
	remote_rx_test_main.c
	remote_rx_test_runner.c
	remote_rx_start_test.c
	remote_rx_data_test.c
	remote_rx_test.c
	mock_ir_recv.c
)

set(GLOBAL_DEFINES

)

include_directories(${INCLUDE_DIRS} ${TEST_INCLUDE_DIRS})
add_definitions(${GLOBAL_DEFINES})

link_libraries(unity)
add_executable(${PROJECT_NAME} ${SRCS} ${TEST_SRCS})

target_compile_options(${PROJECT_NAME} PRIVATE -fprofile-arcs -ftest-coverage)
target_link_options(${PROJECT_NAME} PRIVATE -fprofile-arcs)




