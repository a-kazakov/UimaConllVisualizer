cmake_minimum_required(VERSION 3.2)
project(ConllVisualizer)

add_definitions (-g -Wall)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(CMAKE_BUILD_TYPE Debug)
set(SOURCE_FILES main.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/student/projects/ConllVisualizerOut")
set(CMAKE_SOURCE_DIR "/home/student/projects/ConllVisualizerOut")

add_subdirectory(ConllReader)
add_subdirectory(Visualizer)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
message(WARNING "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")
find_package(UIMA)

add_executable(ConllVisualizer ${SOURCE_FILES})
target_link_libraries(ConllVisualizer ${UIMA_LIBRARY} ${APR_LIBRARY} ${ICU_LIBRARY})

