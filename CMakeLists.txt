cmake_minimum_required (VERSION 3.26.4)

add_compile_options(-O2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project (lbpl VERSION 0.1.0)

include_directories(lib)

file(GLOB SRCFILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SRCFILES})
