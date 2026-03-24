cmake_minimum_required(VERSION 4.1)
project(PositronEngine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(PositronEngine SHARED ${SOURCES} ${HEADERS})

target_include_directories(PositronEngine PUBLIC ${CMAKE_SOURCE_DIR}/engine/include)