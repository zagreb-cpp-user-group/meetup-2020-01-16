cmake_minimum_required( VERSION 3.15 )
project( emscripten_demo CXX )

if ( NOT DEFINED EMSCRIPTEN )
    message( FATAL_ERROR "This project requires Emscripten toolchain!" )
endif()

set( CMAKE_CXX_STANDARD 17 )

# Download automatically, you can also just copy the conan.cmake file
if( NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake" )
   message( STATUS "Downloading conan.cmake from https://github.com/microblink/cmake-conan" )
   file( DOWNLOAD "https://github.com/microblink/cmake-conan/raw/master/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake" )
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run( CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS BUILD missing PROFILE emscripten )

set( SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/src )

set( SOURCES
    ${SRC_ROOT}/main.cpp
    ${SRC_ROOT}/FaceDetector.cpp
)

add_executable( emscripten_demo ${SOURCES} )
target_link_libraries( emscripten_demo PRIVATE CONAN_PKG::opencv CONAN_PKG::boost )

set_source_files_properties( ${SRC_ROOT}/main.cpp PROPERTIES COMPILE_FLAGS "--bind" )
target_link_options( emscripten_demo PRIVATE
    --bind
    "SHELL:--preload-file ${CMAKE_BINARY_DIR}/haarcascades@/haarcascades"
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s EXPORT_NAME=FaceDetectorModule"
)

add_custom_target( DeployJavaScript ALL
    COMMAND
        ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/web/index.html ${CMAKE_SOURCE_DIR}/web/index.js ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    DEPENDS emscripten_demo
    COMMENT "Deploying JS and HTML"
    VERBATIM
)

set_target_properties( DeployJavaScript PROPERTIES ADDITIONAL_CLEAN_FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )

