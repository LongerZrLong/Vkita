set(LIB_NAME stb_image)

add_library(${LIB_NAME} stb_image.cpp)
target_include_directories(${LIB_NAME} INTERFACE .)
