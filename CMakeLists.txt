cmake_minimum_required (VERSION 2.6)
project (assimp2json)

# The version number.
set (ASSIMP2JSON_VERSION_MAJOR 1)
set (ASSIMP2JSON_VERSION_MINOR 0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/assimp2json/version.h.in"
  "${PROJECT_SOURCE_DIR}/assimp2json/version.h"
  )

add_subdirectory (assimp)
set (EXTRA_LIBS ${EXTRA_LIBS} assimp)

include_directories("assimp/include")

add_executable(assimp2json assimp2json/main.cpp assimp2json/json_exporter.cpp assimp2json/cencode.c)
target_link_libraries (assimp2json  ${EXTRA_LIBS})

if( MSVC_IDE )
   add_custom_command(
      TARGET assimp2json
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/assimp/code/$(ConfigurationName)/assimp.dll ${CMAKE_BINARY_DIR}/$(ConfigurationName)/assimp.dll
   )
endif( MSVC_IDE )
