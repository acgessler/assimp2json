cmake_minimum_required (VERSION 2.6)
project (assimp2json)

# The version number.
set (ASSIMP2JSON_VERSION_MAJOR 1)
set (ASSIMP2JSON_VERSION_MINOR 0)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/assimp2json/version.h.in"
  "${PROJECT_SOURCE_DIR}/assimp2json/version.h"
  )

add_subdirectory (assimp)
set (EXTRA_LIBS ${EXTRA_LIBS} assimp)

include_directories("assimp/include")

add_executable(assimp2json assimp2json/main.cpp assimp2json/json_exporter.cpp)
target_link_libraries (assimp2json  ${EXTRA_LIBS})