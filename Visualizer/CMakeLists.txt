
set(TARGET Visualizer)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "/home/student/projects/UimaConllVisualizerOut")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/student/projects/UimaConllVisualizerOut")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
message(WARNING "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

find_package(UIMA)


set(VIS_HEADERS Visualizer.h)
set(VIS_SOURCES Visualizer.cpp)
set(VIS_STATIC Visualizer.xml _visualizer_template.html)

include_directories(
    ${PROJECT_ROOT_DIR}/utils/Tools
    ${PROJECT_ROOT_DIR}/utils/FileManager
)

message(WARNING "${TARGET} SHARED ${VIS_HEADERS} ${VIS_SOURCES}")
add_library(${TARGET} SHARED ${VIS_SOURCES} ${VIS_HEADERS})
target_link_libraries(${TARGET} ${UIMA_LIBRARY} ${APR_LIBRARY} ${ICU_LIBRARY})

set_target_properties(${TARGET} PROPERTIES PREFIX "")

file(COPY ${VIS_STATIC} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})