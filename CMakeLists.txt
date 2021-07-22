cmake_minimum_required(VERSION 3.10)
# name of the folder
project(logReader)

include(GNUInstallDirs)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)
  #release comes with -O3 by default
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)

set(CMAKE_CXX_STANDARD 11)

option(TEST "test example" ON)
option(DEBUG "dev debug" OFF)

file(GLOB PythonSOURCES
    src/logReader.cpp
    src/python.cpp
    )

set(SOURCES "src/logReader.cpp")


set(HOME_INC_DIR "./src") 
include_directories(/src)



if(NOT DEFINED VERSION_INFO)
set(VERSION_INFO "\"0.1.1\"")
endif(NOT DEFINED VERSION_INFO)


add_subdirectory(pybind11)
pybind11_add_module(_logReader ${PythonSOURCES})
target_compile_definitions(_logReader PRIVATE VERSION_INFO=${VERSION_INFO})

add_library(logReader ${SOURCES})
if(DEBUG)
  target_compile_definitions(logReader PRIVATE debug)
endif(DEBUG)

IF(TEST)
  enable_testing()
  file(GLOB TestFile test/*.cpp)
  # remove some test that not modified

  list(REMOVE_ITEM TestFile ${NOT_test})
  foreach(testSource ${TestFile})  
    string(REGEX REPLACE "^.*/([^/]*)\\.cpp$" "\\1" PROGNAME "${testSource}")
    # message(${PROGNAME})
    add_executable(${PROGNAME} ${testSource})
    include_directories(src/)
    target_link_libraries(${PROGNAME} PRIVATE logReader)
    add_test(NAME ${PROGNAME} COMMAND ${PROGNAME})
    # add_test(NAME ${PROGNAME} COMMAND qw46 ${CMAKE_CURRENT_SOURCE_DIR}/test/fcc.xyz)
endforeach()

  # file(GLOB EXAMPLES LIST_DIRECTORIES true test/*)
endif(TEST)
