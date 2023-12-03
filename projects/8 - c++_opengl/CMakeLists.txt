cmake_minimum_required(VERSION 3.26)
project(gltest)

set(CMAKE_CXX_STANDARD 17)

file(GLOB IMGUI_SOURCE
    "lib/imgui/*.h"
    "lib/imgui/*.cpp"
    "lib/imgui/backends/imgui_impl_opengl3.h"
    "lib/imgui/backends/imgui_impl_opengl3.cpp"
    "lib/imgui/backends/imgui_impl_glfw.h"
    "lib/imgui/backends/imgui_impl_glfw.cpp"
)

add_executable(gltest
        lib/glad/src/glad.c
        lib/stb/src/stb_image.cpp
        ${IMGUI_SOURCE}
        src/renderer/Renderer.cpp
        src/shaders/Shader.cpp
        src/buffers/VertexArray.cpp
        src/buffers/VertexBuffer.cpp
        src/buffers/VertexBufferLayout.cpp
        src/buffers/IndexBuffer.cpp
        src/textures/Texture.cpp
        src/renderer/Window.cpp
        src/renderer/Gui.cpp
        src/Application.cpp
        src/renderer/Cube.cpp
)

include_directories(lib/glad/include)
include_directories(lib/stb/include)
include_directories(lib/glm/include)
include_directories(lib/imgui)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(dir ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(lib/glfw)
target_link_libraries(gltest glfw)
