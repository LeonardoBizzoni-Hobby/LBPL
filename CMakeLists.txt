cmake_minimum_required(VERSION 3.29)
project(lbpl CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set output directories
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.hpp")
add_executable(${PROJECT_NAME} ${SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(${PROJECT_NAME} PRIVATE ROOTDIR="${CMAKE_SOURCE_DIR}")

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
                      ARCHITE_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
                      LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

# Configuration-specific settings
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:OptimizedDebug>:OPTDEBUG>
    $<$<CONFIG:Release>:RELEASE>)

# Configuration-specific optimizations
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:OptimizedDebug>:-O3>
    $<$<CONFIG:Release>:-O3>)
