project(Sandbox)

add_executable(${PROJECT_NAME} Source/SandboxApp.cpp)

target_link_libraries(${PROJECT_NAME} Vkita)

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${output_dir})
