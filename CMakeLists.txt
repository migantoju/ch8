cmake_minimum_required(VERSION 3.23)
project(chip8)

set(CMAKE_CXX_STANDARD 20)

# config for Mac OS
# find_package(SDL2 REQUIRED)
# include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})
# end config for Mac OS

# config for windows
set(SDL2_INCLUDE_DIR D:/vclib/include)
set(SDL2_LIB_DIR D:/vclib/lib/x64)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})
# end config for windows

add_executable(chip8 src/main.cpp src/cpu.h src/cpu.cpp src/masks.h src/video.h src/video.cpp src/audio/speaker.h src/audio/speaker.cpp)

# target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} SDL2main SDL2)
