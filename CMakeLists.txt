cmake_minimum_required(VERSION 3.28)
project(newtons_fractal)

set(CMAKE_CXX_STANDARD 23)

# Download raylib if it is not present in the system
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    set(FETCHCONTENT_QUIET FALSE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE) # don't build the supplied example games
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY "https://github.com/raysan5/raylib"
            GIT_TAG "master"
            GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(raylib)
endif ()


set(FRACTAL_SOURCE compute/Fractal.cpp)

# add ISPC
option(USE_ISPC "Use ISPC" ON)
if (USE_ISPC)
    find_program(ISPC_EXECUTABLE ispc REQUIRED)

    set(ISPC_SOURCE ${CMAKE_SOURCE_DIR}/compute/Fractal.ispc)
    set(ISPC_OBJECT ${CMAKE_CURRENT_BINARY_DIR}/Fractal.o)

    add_custom_command(
            OUTPUT ${ISPC_OBJECT}
            COMMAND ${ISPC_EXECUTABLE} ${ISPC_SOURCE} -o ${ISPC_OBJECT} -O2
            DEPENDS ${ISPC_SOURCE}
            COMMENT "Compiling ISPC file ${ISPC_SOURCE}"
    )

    set(FRACTAL_SOURCE ${ISPC_OBJECT})
    add_definitions(-DUSE_ISPC)
endif ()


option(BUILD_TESTS "Build Tests" OFF)
if (BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_executable(perf_test tests/PerfTests.cpp
            compute/Fractal.hpp
            ${FRACTAL_SOURCE}
    )

    target_link_libraries(perf_test PRIVATE
            GTest::gtest
            GTest::gtest_main
    )
endif ()


message("Fractal source: " ${FRACTAL_SOURCE})
add_executable(newtons_fractal main.cpp
        display/Display.hpp
        display/Display.cpp
        compute/Fractal.hpp
        export/Image.cpp
        export/Image.hpp
        ${FRACTAL_SOURCE}
)

target_link_libraries(newtons_fractal PRIVATE raylib)