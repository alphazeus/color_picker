cmake_minimum_required(VERSION 3.5.0)
project(color_picker VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(lib/glfw-3.4)

FILE(GLOB_RECURSE imgui_files lib/imgui/*.h lib/imgui/*.cpp) 

add_executable(color_picker 
    src/main.cpp 
    ${imgui_files}
)

find_package(OpenGL REQUIRED)

target_include_directories(color_picker PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    lib/imgui
)

target_link_libraries(color_picker 
    ${OPENGL_LIBRARIES}
    glfw 
    gdi32
)