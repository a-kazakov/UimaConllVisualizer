
set(TARGET ConllReader)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "/home/student/projects/UimaConllVisualizerOut")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/student/projects/UimaConllVisualizerOut")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
message(WARNING "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

find_package(UIMA)


set(CONLL_HEADERS ConllReader.h)
set(CONLL_SOURCES ConllReader.cpp)
set(CONLL_STATIC ConllReader.xml)

include_directories(
    ${PROJECT_ROOT_DIR}/utils/Tools
    ${PROJECT_ROOT_DIR}/utils/FileManager
)

message(WARNING "${TARGET} SHARED ${CONLL_HEADERS} ${CONLL_SOURCES}")
add_library(${TARGET} SHARED ${CONLL_SOURCES} ${CONLL_HEADERS})
target_link_libraries(${TARGET} ${UIMA_LIBRARY} ${APR_LIBRARY} ${ICU_LIBRARY})

set_target_properties(${TARGET} PROPERTIES PREFIX "")

file(COPY ${CONLL_STATIC} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
