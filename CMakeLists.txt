if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/third_party/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
else()
    set(VCPKG_TARGET_TRIPLET "x64-linux-clang")
endif()

cmake_minimum_required(VERSION 3.28)
project(chatcore)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(
        -Wall -Wextra -Werror -Wpedantic
        -Wshadow -Wconversion -Wsign-conversion
        -Wold-style-cast -Wnon-virtual-dtor
        -Woverloaded-virtual -Wnull-dereference
        -Wdouble-promotion -Wformat=2
        -Wundef -Wextra-semi -Wimplicit-fallthrough
        -Wglobal-constructors
        -fno-common -fstrict-aliasing -fno-omit-frame-pointer

        -Wno-c++98-compat-extra-semi
    )
endif()

if(WIN32)
    set(BOOST_STACKTRACE stacktrace_windbg)
else()
    set(BOOST_STACKTRACE stacktrace_backtrace)
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/libs/ebroschin-utility")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/ebroschin-core")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/ebroschin-network")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/ebroschin-network-modules")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/ebroschin-scheduling")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/ebroschin-logging")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/ebroschin-logging-modules")
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/chat-server-api")

if(BUILD_SERVER)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/server")
endif()

if(BUILD_CLIENT)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/client")
endif()

if(BUILD_LOAD_TESTER)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/load-tester")
endif()