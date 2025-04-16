
set(srcs) # set(srcs "file1.c" "file2.c" "file3.c")
set(public_requires)

list(APPEND srcs "src/ws2812b_fx.c") # add new c file to srcs
list(APPEND srcs "src/ws2812b_if_RMT.c") # add new c file to srcs

list(APPEND public_requires "driver")
list(APPEND public_requires "main")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "inc"
                    REQUIRES ${public_requires})