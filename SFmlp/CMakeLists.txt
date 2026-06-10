cmake_minimum_required(VERSION 3.16)
project(DungeonBuilder)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

file(GLOB SOURCES *.cpp)

add_executable(app ${SOURCES})
target_link_libraries(app
    SFML::Graphics
    SFML::Window
    SFML::System
)