cmake_minimum_required(VERSION 3.15)
project(BioVisualizer)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

add_executable(BioVisualizer
    main.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_glut.cpp
)

target_include_directories(BioVisualizer
    PRIVATE ${CMAKE_SOURCE_DIR}/imgui
    PRIVATE ${CMAKE_SOURCE_DIR}/imgui/backends
)

target_link_libraries(BioVisualizer
    OpenGL::GL
    GLEW::GLEW
    GLUT::GLUT
    GLU
    m
)
