cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project(Maze)

set(CMAKE_THREAD_LIBS_INIT "-pthread")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall ${CMAKE_THREAD_LIBS_INIT}")
# set(CMAKE_THREAD_PREFER_PTHREAD)

set(MAIN_PROJECT_SRC_LIST main.cpp)
add_executable(main ${MAIN_PROJECT_SRC_LIST})
add_subdirectory(include)
target_link_libraries(main lib)
