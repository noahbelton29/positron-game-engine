cmake_minimum_required(VERSION 4.1)
project(Game)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(Game ${SOURCES} ${HEADERS})

target_link_libraries(Game PRIVATE PositronEngine)

target_include_directories(Game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)