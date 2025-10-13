cmake_minimum_required(VERSION 4.0)
project(bilsimulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

include(FetchContent)

set(THREEPP_BUILD_TESTS OFF)
set(THREEPP_BUILD_EXAMPLES OFF)
FetchContent_Declare(
        threepp
        GIT_REPOSITORY https://github.com/markaren/threepp.git
        GIT_TAG b61aa93bb7fb6f9f4b7e89f237c96efc282bab10
)
FetchContent_MakeAvailable(threepp)

add_executable(bilsimulator src/main.cpp
        src/world.cpp
        src/car.cpp
        src/physics.cpp
        src/collision.cpp
        src/pickup.cpp
        src/ui.cpp
)
target_link_libraries(bilsimulator PRIVATE threepp::threepp)


