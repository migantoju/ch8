cmake_minimum_required(VERSION 3.23)
project(chip8)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

add_executable(chip8 src/main.cpp src/cpu.h src/cpu.cpp src/masks.h src/fontset.h src/video.h src/video.cpp)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
