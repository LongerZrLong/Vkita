cmake_minimum_required(VERSION 3.10)
project(Vkita)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET default)

set(output_dir ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

add_subdirectory(Engine)

enable_testing()
add_subdirectory(Test)
