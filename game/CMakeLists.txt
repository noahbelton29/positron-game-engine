cmake_minimum_required(VERSION 4.1)
project(Game)

set(CMAKE_CXX_STANDARD 20)

add_executable(Game src/main.cpp)

target_link_libraries(Game PRIVATE PositronEngine)