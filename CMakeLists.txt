cmake_minimum_required(VERSION 3.14)
project(GridTactics CXX)

set(CMAKE_CXX_STANDARD 20)


include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/master.tar.gz
)
FetchContent_MakeAvailable(raylib)


add_executable(${PROJECT_NAME} main.cpp Components.h Registry.h Systems.h)


target_link_libraries(${PROJECT_NAME} PRIVATE raylib)