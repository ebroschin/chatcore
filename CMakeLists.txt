cmake_minimum_required(VERSION 4.0)
project(chat-server)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

add_executable(chat-server source/main.cc)

find_package(Boost REQUIRED COMPONENTS stacktrace_backtrace)

target_link_libraries(chat-server PRIVATE Boost::stacktrace_backtrace)
