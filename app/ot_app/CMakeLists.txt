
# ==============================================================================
# VERSIONING SECTION (Automatic generation from Git)
# ==============================================================================
# 1. Pobranie wersji subrepozytorium (ot_app)
execute_process(
    COMMAND git describe --tags --always --dirty
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE OT_APP_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT OT_APP_VERSION)
    set(OT_APP_VERSION "unknown")
endif()

# 2. Pobranie wersji głównego repozytorium (Platformy ESP32)
execute_process(
    COMMAND git describe --tags --always --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE PLATFORM_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT PLATFORM_VERSION)
    set(PLATFORM_VERSION "unknown")
endif()

# 3. Wygenerowanie pliku z dwoma zmiennymi
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/inc/ot_app_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/ot_app_version.h
    @ONLY
)
# ==============================================================================

set(srcs) 
set(public_requires)

file(GLOB_RECURSE srcs
    src/*.c  
    port/*.c  
)

list(APPEND public_requires "main")
list(APPEND public_requires "driver")
list(APPEND public_requires "openthread")
# list(APPEND public_requires "json")
# list(APPEND public_requires "spiffs")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "inc" "port" "${CMAKE_CURRENT_BINARY_DIR}"
                    REQUIRES ${public_requires})