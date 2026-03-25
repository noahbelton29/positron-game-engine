cmake_minimum_required(VERSION 4.1)
project(Game)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(Game ${SOURCES} ${HEADERS})

target_link_libraries(Game PRIVATE PositronEngine)
target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# debug
target_compile_options(Game PRIVATE
        $<$<CONFIG:Debug>:
        -g
        -fno-omit-frame-pointer
        -fno-inline
        >)

# release
target_compile_options(Game PRIVATE
        $<$<CONFIG:Release>:
        -O3
        -march=native
        -ffast-math
        -funroll-loops
        -fomit-frame-pointer
        -DNDEBUG
        >)

target_link_options(Game PRIVATE
        $<$<CONFIG:Release>:
        -O3
        -flto
        -s
        >)

# relwithdebinfo
target_compile_options(Game PRIVATE
        $<$<CONFIG:RelWithDebInfo>:
        -O2
        -g
        -fno-omit-frame-pointer
        -DNDEBUG
        >)