### cmake for tests

```
/project_root
├── .vscode/                    
|
├── project
│	├── ...                 
│   └── ...   
├── tests/                      
│   ├── external/
│	│   └── unity/              
│   ├── integration/
│   └── unit_test/
│	    ├── HW_template_test/      # for hardware test
│		│   ├── CMakeLists.txt
│		│   ├── template_test.c
│		│   └── test_runner.c        
│		├── HOST_template_test/    # for development machine  
│		|   ├── CMakeLists.txt
│		|   ├── mock_template.c
│		|   ├── test_runner.c
│		|   └── ... 
│		└── CMakeLists.txt     
└── ...   
```

There are two diffrent unit test case:
- for HW - hardware (compiler: arm-none-eabi)
- for HOST - development machine (compiler: gcc)

#### HW test

> [!note] HW TEST IS NOT FINISHED YET

#### HOST test
```
/project_root       <----- run build here
├── ...                    
|
├── project      
│   └── ...   
├── tests/                      
│   ├── ...           
│   └── unit_test/
│	    ├── .. 
│		├── HOST_template_test/    # for development machine  
│		|   ├── CMakeLists.txt   
│		|   ├── mock_template.c
│		|   ├── mock_template.h
│		|   ├── test_runner.c
│		|   └── ...
│		├── HOST_my_new_test/       <--- new test  
│		|   ├── CMakeLists.txt      <--- you can add INCLUDE_DIRS and SRCS   
│		|   ├── mock_template.c
│		|   ├── mock_template.h
│		|   ├── test_runner.c
│		|   └── ...
│		└── CMakeLists.txt         <--- add subdirectory of your new test
└── ...   
```


To create your new test:
- copy HOST_template_test folder and paste in the same folder (unit_test)
- change foldar name 
- add subdirectory to CMakeLists.txt  see above (name subdirectory has to be like your folder name)
- check build of your new test
- implement your new test 
	- you can add INCLUDE_DIRS and  SRCS files to your test at CMakeLists.txt of your catalog


***content of  main CMakeLists.txt file ( unit_test/CMakeLists.txt):***
```bash
cmake_minimum_required(VERSION 3.17)
project(unit_test C)
  
set(PROJECT_TARGETS_LIST "" CACHE INTERNAL "Target list")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  
add_subdirectory(../external/unity unity)
  
  
# add tests subdirectory:
add_subdirectory(HOST_template_test)
# add_subdirectory(HOST_template_test2)
# add_subdirectory(HW_template_test)
# add_subdirectory(testX)
  
message(STATUS "------------------------------------------------ Project targets list: ")
foreach(list ${PROJECT_TARGETS_LIST})
    message(STATUS "                 ${list}")
endforeach()
message(STATUS " ")
```

##### how to build a test:
every command run in the root project path. 

```bash
cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE --no-warn-unused-cli -S./tests/unit_test -B./build/unit_test -G Ninja
```

output
```bash
-- The C compiler identification is GNU 14.1.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/.Programy/msys64/ucrt64/bin/cc.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- ------------------------------------------------ Project targets list: 
--                  HOST_template_test
--
-- Configuring done (1.1s)
-- Generating done (0.0s)
-- Build files have been written to: C:/.Code/STM/MyProjects/template/build/unit_test
```

As you see the target list has been printed there. It will be useful for run specific target build (see below)

###### ***Available options:***
 - enable CPPCHECK and CLANG_TIDY
```bash
	-DENABLE_ANALYSIS=ON
	```
 - enable log of file source
```bash
	-DENABLE_PRINT_SRCS_FILE=ON
```

###### ***full command:***
```BASH
cmake -DENABLE_ANALYSIS=ON -DENABLE_PRINT_SRCS_FILE=ON -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE --no-warn-unused-cli -S./tests/unit_test -B./build/unit_test -G Ninja
```

output of full command:
```
...
...
-- Detecting C compile features - done
--  
-- ------------------------------------------------ HOST_template_test: 
--                   SRCS file list for target: HOST_template_test
--
--                   C:/.Code/STM/MyProjects/template/tests/unit_test/HOST_template_test/mock_template.c
--                   C:/.Code/STM/MyProjects/template/tests/unit_test/HOST_template_test/template_test.c
--                   C:/.Code/STM/MyProjects/template/tests/unit_test/HOST_template_test/template_test_main.c
--                   C:/.Code/STM/MyProjects/template/tests/unit_test/HOST_template_test/template_test_runner.c
--
-- ------------------------------------------------ Project targets list:
--                  HOST_template_test
--
-- Configuring done (1.1s)
-- Generating done (0.0s)
-- Build files have been written to: C:/.Code/STM/MyProjects/template/build/unit_test
```
The SRC file list has been printed there. There also will be printed your all source file of your new test case.
(if you use: -DENABLE_PRINT_SRCS_FILE=ON)


###### ***run build:***
you can run build of all test:
```bash
cmake --build ./build/unit_test --config Debug 
```

or specyfic test (specyfic target of your test from Project targets list)
```bash
cmake --build ./build/unit_test --config Debug --target HOST_template_test
```

output:
```bash
[8/8] Linking C executable HOST_template_test\HOST_template_test.exe
```

###### ***run test:***
```
/project_root
  ├── .vscode/                    # VSC configuration directory
  ├── doc/
  ├── build/                      # Output of app, boot_app, test
  │   ├── app/
  │   ├── boot_app/
  │   └── unit_test/
  │       ├── HOST_template_test/
  │       |   ├── ...
  │       |   └── HOST_template_test.exe
  │       └── ...
  └── ...
   ```

In the command line in project root folder, enter:
```bash 
build/unit_test/HOST_template_test/HOST_template_test.exe
```

or your specyfic path of your new unit test
