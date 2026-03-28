cmake_minimum_required(VERSION 4.1)
project(PositronEngine)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

cmake_policy(SET CMP0169 OLD)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(SPDLOG_BUILD_PIC ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.17.0
)

FetchContent_Declare(
        backward
        GIT_REPOSITORY https://github.com/bombela/backward-cpp.git
        GIT_TAG v1.6
)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
)
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)
FetchContent_Declare(
        pfr
        GIT_REPOSITORY https://github.com/boostorg/pfr.git
        GIT_TAG 2.2.0
)

set(JPH_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(JPH_ENABLE_ASSERTS OFF CACHE BOOL "" FORCE)
set(JPH_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(JPH_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
        GIT_TAG v5.3.0
        SOURCE_SUBDIR Build
)
FetchContent_MakeAvailable(glfw spdlog glm stb pfr JoltPhysics)
FetchContent_Populate(backward)

add_library(glad STATIC
        ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c)

target_include_directories(glad PUBLIC
        ${CMAKE_SOURCE_DIR}/external/glad/include)

set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(PositronEngine SHARED ${SOURCES} ${HEADERS})

target_include_directories(PositronEngine PUBLIC
        ${CMAKE_SOURCE_DIR}/engine/include
        ${backward_SOURCE_DIR}
        ${stb_SOURCE_DIR}
        ${pfr_SOURCE_DIR}/include
        ${joltphysics_SOURCE_DIR})

target_compile_definitions(PositronEngine PUBLIC BACKWARD_HAS_DW=1)
target_compile_definitions(PositronEngine PRIVATE POSITRON_BUILD_DLL)
target_compile_options(PositronEngine PRIVATE -fno-modules-ts)

target_link_libraries(PositronEngine PUBLIC glfw spdlog glm dw Jolt)
find_package(OpenGL REQUIRED)
target_link_libraries(PositronEngine PRIVATE glad OpenGL::GL)

target_compile_options(PositronEngine PRIVATE
        $<$<CONFIG:Debug>:
        -g
        -fno-omit-frame-pointer
        -fno-inline
        >)

target_compile_options(PositronEngine PRIVATE
        $<$<CONFIG:Release>:
        -O3
        -march=native
        -ffast-math
        -funroll-loops
        -DNDEBUG
        >)

target_link_options(PositronEngine PRIVATE
        $<$<CONFIG:Release>:
        -O3
        -flto
        -s
        >)

target_compile_options(PositronEngine PRIVATE
        $<$<CONFIG:RelWithDebInfo>:
        -O2
        -g
        -fno-omit-frame-pointer
        -DNDEBUG
        >)
