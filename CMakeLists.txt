cmake_minimum_required(VERSION 3.20)
project(bilsimulator)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# =====================================
# threepp
# =====================================
set(THREEPP_BUILD_TESTS OFF)
set(THREEPP_BUILD_EXAMPLES OFF)

FetchContent_Declare(
        threepp
        GIT_REPOSITORY https://github.com/markaren/threepp.git
        GIT_TAG b61aa93bb7fb6f9f4b7e89f237c96efc282bab10
)
FetchContent_MakeAvailable(threepp)

# =====================================
# ImGui (auto-download + OpenGL3/GLFW backend)
# =====================================
FetchContent_Declare(
        imgui_src
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG master
)
FetchContent_MakeAvailable(imgui_src)

set(IMGUI_DIR ${imgui_src_SOURCE_DIR})

# <-- THIS is the path you just found with `find`
set(GLFW_INCLUDE_DIR
        ${CMAKE_BINARY_DIR}/_deps/threepp-src/src/external/glfw/include
)

add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp

        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${GLFW_INCLUDE_DIR}        # <-- makes <GLFW/glfw3.h> work
)

# =====================================
# Your game
# =====================================
add_executable(bilsimulator
        src/application.cpp
        src/world.cpp
        src/car.cpp
)

target_link_libraries(bilsimulator PUBLIC
        threepp
        imgui
)

add_custom_command(
        TARGET bilsimulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/assets
        ${CMAKE_BINARY_DIR}/assets
)
