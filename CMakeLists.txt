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
        -fno-common -fstrict-aliasing -fno-omit-frame-pointer

        -Wno-c++98-compat-extra-semi
    )

    if(NOT BUILD_TESTING)
        add_compile_options(
            -Wglobal-constructors
        )
    endif ()
endif()

if(WIN32)
    set(BOOST_STACKTRACE stacktrace_windbg)
else()
    set(BOOST_STACKTRACE stacktrace_backtrace)
endif()

if(BUILD_TESTING)
    enable_testing()
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/utility")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/core")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/network")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/network-modules")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/scheduling")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/logging")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/logging-modules")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/commands")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/persistence")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/persistence-modules")
add_subdirectory("${CMAKE_SOURCE_DIR}/ebroschin-sdk/chatcore-api")

if(BUILD_SERVER)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/server")
endif()

if(BUILD_CLIENT)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/client")
endif()

if(BUILD_LOAD_TESTER)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/load-tester")
endif()