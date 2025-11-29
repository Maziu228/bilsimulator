cmake_minimum_required(VERSION 4.0)
project(bilsimulator)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

include(FetchContent)

set(THREEPP_BUILD_TESTS OFF)
set(THREEPP_BUILD_EXAMPLES OFF)
FetchContent_Declare(
        threepp
        GIT_REPOSITORY https://github.com/markaren/threepp.git
        GIT_TAG b61aa93bb7fb6f9f4b7e89f237c96efc282bab10
)
FetchContent_MakeAvailable(threepp)
add_subdirectory(external/bullet3)

add_executable(bilsimulator
        src/application.cpp
        src/world.cpp
        src/car.cpp
)
target_link_libraries(bilsimulator PUBLIC
        threepp
        BulletDynamics
        BulletCollision
        LinearMath)

target_include_directories(bilsimulator
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/bullet3/src)


add_custom_command(
        TARGET bilsimulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/assets
        ${CMAKE_BINARY_DIR}/assets
)


