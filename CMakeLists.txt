cmake_minimum_required(VERSION 3.2)
project(MyAnnotator)

add_definitions (-g -Wall)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(CMAKE_BUILD_TYPE Debug)
set(SOURCE_FILES main.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "/home/student/projects/ATestOut")
set(CMAKE_SOURCE_DIR   "/home/student/projects/ATestOut")

add_subdirectory(ConllReader)
add_subdirectory(Visualizer)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
message(WARNING "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")
find_package(UIMA)

add_executable(MyAnnotator ${SOURCE_FILES})
target_link_libraries(MyAnnotator ${UIMA_LIBRARY} ${APR_LIBRARY} ${ICU_LIBRARY})

