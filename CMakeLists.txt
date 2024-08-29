cmake_minimum_required(VERSION 3.5.0)
project(color_picker VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(lib/glfw-3.4)

add_executable(color_picker src/main.cpp)

target_link_libraries(color_picker 
    glfw 
    opengl32 
    gdi32
)