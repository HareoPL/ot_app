
set(srcs) # set(srcs "file1.c" "file2.c" "file3.c")
set(public_requires)

list(APPEND srcs "src/wifi.c") # add new c file to srcs
list(APPEND srcs "src/web_app.c") 
list(APPEND srcs "src/web_app_parser.c") 


list(APPEND public_requires "main")
list(APPEND public_requires "driver")
list(APPEND public_requires "esp_event")
list(APPEND public_requires "nvs_flash")
list(APPEND public_requires "esp_wifi")
list(APPEND public_requires "esp_http_server")
list(APPEND public_requires "json")
list(APPEND public_requires "spiffs")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "inc"
                    REQUIRES ${public_requires})