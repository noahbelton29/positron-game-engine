cmake_minimum_required(VERSION 4.1)
project(PositronEngine)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(PositronEngine SHARED ${SOURCES} ${HEADERS})

target_include_directories(PositronEngine PUBLIC ${CMAKE_SOURCE_DIR}/engine/include)
target_link_libraries(PositronEngine PUBLIC glfw)