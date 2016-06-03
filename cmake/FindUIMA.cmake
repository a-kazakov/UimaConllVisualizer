set(HAVE_UIMA_HEADER 0)
set(UIMA_HEADER_DIRS PATHS "/usr/local/uimacpp/include")

message(WARNING "UIMA_HEADER_DIRS=${UIMA_HEADER_DIRS}")

find_file(UIMA_HEADER_PATH "api.hpp" ${UIMA_HEADER_DIRS} PATH_SUFFIXES "uima")
message(WARNING "UIMA_HEADER_PATH=${UIMA_HEADER_PATH}")

if (UIMA_HEADER_PATH)
    set(HAVE_UIMA_HEADER 1)
    message(WARNING "HAVE_UIMA_HEADER=${HAVE_UIMA_HEADER}")

    get_filename_component(UIMA_INCLUDE_DIR ${UIMA_HEADER_PATH} PATH)
    message(WARNING "UIMA_INCLUDE_DIR=${UIMA_INCLUDE_DIR}")

    get_filename_component(UIMA_ROOT_DIR ${UIMA_INCLUDE_DIR} PATH)
    get_filename_component(UIMA_ROOT_DIR ${UIMA_ROOT_DIR} PATH)
    message(WARNING "UIMA_ROOT_DIR=${UIMA_ROOT_DIR}")

    set(UIMA_LIBRARY_DIR "${UIMA_ROOT_DIR}/lib")
    message(WARNING "UIMA_LIBRARY_DIR=${UIMA_LIBRARY_DIR}")

    find_library(UIMA_LIBRARY uima HINTS ${UIMA_LIBRARY_DIR})
    message(WARNING "UIMA_LIBRARY=${UIMA_LIBRARY}")

    set(UIMA_HEADER_DIR "${UIMA_ROOT_DIR}/include")
    message(WARNING "UIMA_HEADER_DIR=${UIMA_HEADER_DIR}")

    include_directories(
            ${UIMA_HEADER_DIR}
            /usr/include/apr-1.0
    )

    find_library(APR_LIBRARY apr-1)
    message(WARNING "APR_LIBRARY=${APR_LIBRARY}")
    find_library(ICU_LIBRARY icuuc)
    message(WARNING "ICU_LIBRARY=${ICU_LIBRARY}")

endif(UIMA_HEADER_PATH)

