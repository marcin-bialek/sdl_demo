cmake_minimum_required(VERSION 3.26)
project(sdl_demo)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/SDL)

include_directories(external/SDL/include)

file(GLOB_RECURSE SRCS src/*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
