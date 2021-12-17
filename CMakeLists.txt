cmake_minimum_required(VERSION 3.10)
project(Vkita)

set(CMAKE_CXX_STANDARD 17)

set(output_dir ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

add_subdirectory(Engine)
add_subdirectory(Sandbox)


# ====================================== Resource ====================================
set(RESOURCE_SRC_DIR ${PROJECT_SOURCE_DIR}/Resource)
set(RESOURCE_DST_DIR ${PROJECT_SOURCE_DIR}/bin/${output_dir}/Resource)

# Compiled shaders and move the outputs to destination directories
set(SHADER_SOURCE_DIR ${RESOURCE_SRC_DIR}/Shaders)
set(SHADER_BINARY_DIR ${RESOURCE_DST_DIR}/Shaders)

file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

file(GLOB SHADERS
        ${SHADER_SOURCE_DIR}/*.vert
        ${SHADER_SOURCE_DIR}/*.frag
        )

foreach(source IN LISTS SHADERS)
    get_filename_component(filename ${source} NAME)
    add_custom_command(
            COMMAND glslc -o ${SHADER_BINARY_DIR}/${filename} ${source}
            OUTPUT ${SHADER_BINARY_DIR}/${filename}
            DEPENDS ${source} ${SHADER_SOURCE_DIR}
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${filename})
endforeach()

add_custom_target(shaders DEPENDS ${SPV_SHADERS})

add_dependencies(${PROJECT_NAME} shaders)


# Copy textures to destination directories
set(TEXTURE_SOURCE_DIR ${RESOURCE_SRC_DIR}/Textures)
set(TEXTURE_DST_PARENT_DIR ${RESOURCE_DST_DIR})

file(COPY ${TEXTURE_SOURCE_DIR} DESTINATION ${TEXTURE_DST_PARENT_DIR})
