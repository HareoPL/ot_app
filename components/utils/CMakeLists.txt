
set(srcs) # set(srcs "file1.c" "file2.c" "file3.c")
set(public_requires)

list(APPEND srcs "utils.c") # add new c file to srcs

list(APPEND public_requires "driver")
list(APPEND public_requires "main")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    REQUIRES ${public_requires})