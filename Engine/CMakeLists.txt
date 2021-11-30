file(GLOB_RECURSE SRCS
        Core/*.cpp
        Input/*.cpp
        Window/*.cpp
        )

add_library(${PROJECT_NAME} STATIC ${SRCS})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(${PROJECT_NAME} PRIVATE VKT_ENABLE_ASSERTS)

# GLFW
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(External/GLFW)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

# spdlog
add_subdirectory(External/spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog)

# glm
add_subdirectory(External/glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
