file(GLOB_RECURSE SRCS
        Application/*.cpp
        Core/*.cpp
        Debug/*.cpp
        Input/*.cpp
        Memory/*.cpp
        Parser/*.cpp
        Rendering/*.cpp
        Scene/*.cpp
        Utils/*.cpp
        Vulkan/*.cpp
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

# Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

# Compile shaders
set(SHADER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Resource/Shaders)

file(GLOB SHADERS
        ${SHADER_SOURCE_DIR}/*.vert
        ${SHADER_SOURCE_DIR}/*.frag
        )

foreach(source IN LISTS SHADERS)
    get_filename_component(filename ${source} NAME)
    set(binary_name ${filename}.spv)
    add_custom_command(
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${source} -o ${SHADER_SOURCE_DIR}/${binary_name}
            OUTPUT ${SHADER_SOURCE_DIR}/${binary_name}
            DEPENDS ${source} ${SHADER_SOURCE_DIR}
    )
    list(APPEND SPV_SHADERS ${SHADER_SOURCE_DIR}/${binary_name})
endforeach()

add_custom_target(Shaders DEPENDS ${SPV_SHADERS})

add_dependencies(${PROJECT_NAME} Shaders)


# stb_image
add_subdirectory(External/stb_image)
target_link_libraries(${PROJECT_NAME} PUBLIC stb_image)

# Assimp
add_subdirectory(External/Assimp)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)

# ktx
add_subdirectory(External/ktx)
target_link_libraries(${PROJECT_NAME} PUBLIC ktx)
