include(CTest)

enable_testing()

set(TEST_NAME)
set(TEST_PATH_EXE)

function(add_test_to_lists test_name)
    list(APPEND TEST_NAME "${test_name}")
    
    # set(test_path "${CMAKE_BINARY_DIR}/unit_test/${test_name}/${test_name}.exe")
    set(test_path "../../build/unit_test/${test_name}/${test_name}.exe")
    # set(test_path "${CMAKE_BINARY_DIR}/unit_test/${test_name}/${test_name}${CMAKE_EXECUTABLE_SUFFIX}")
    list(APPEND TEST_PATH_EXE "${test_path}")
    
    set(TEST_NAME "${TEST_NAME}" PARENT_SCOPE)
    set(TEST_PATH_EXE "${TEST_PATH_EXE}" PARENT_SCOPE)
endfunction()

# include(tests/unit_test/ctest_add_tests.cmake)
include(ctest_add_tests.cmake)

list(LENGTH TEST_NAME test_name_count)
list(LENGTH TEST_PATH_EXE test_path_count)

set(added_tests "")

message("TEST_NAME length=${test_name_count}")
message("TEST_PATH_EXE length=${test_path_count}")

message("TEST_NAME = ${TEST_NAME}")
message("TEST_PATH_EXE = ${TEST_PATH_EXE}")

math(EXPR last_index "${test_name_count} - 1")
foreach(idx RANGE 0 ${last_index})
    list(GET TEST_NAME ${idx} current_test_name)
    list(GET TEST_PATH_EXE ${idx} current_test_path)

    add_test(NAME ${current_test_name} COMMAND ${current_test_path})
    list(APPEND added_tests ${current_test_name})
endforeach()


message("Tests added list: ${added_tests}")
