if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

cmake_minimum_required(VERSION 4.0)
project(chat-server)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
    endif()

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

option(BUILD_CLIENT "Build the chat-client instead of chat-server" ON)

if(BUILD_CLIENT)
    set(TARGET_NAME chat-client)
else()
    set(TARGET_NAME chat-server)
endif()

add_executable(${TARGET_NAME} source/main.cc
        source/application/chat_server_application.cpp
        source/application/chat_server_application.h
        source/prototyping/client_test_system.cpp
        source/prototyping/client_test_system.h
        source/prototyping/prototyping_system.h
        source/boost_communication/boost_tcp_server.cpp
        source/boost_communication/boost_tcp_server.h
        source/persistence/persistence_system.h
        source/persistence/persistence_store.h
        source/persistence/persistence_adapter.h
        source/chat/adapters/chat_persistence_adapter.h
        source/sqlite_persistence/sqlite_persistence_store.cpp
        source/sqlite_persistence/sqlite_persistence_store.h
        source/chat/adapters/sqlite_chat_persistence_adapter.cpp
        source/chat/adapters/sqlite_chat_persistence_adapter.h
        source/chat/chat_server_system.cpp
        source/chat/chat_server_system.h
        source/communication/message_handler_registry.h
        source/communication/message_handler.h
        source/prototyping/chat_input_system.cc
        source/prototyping/chat_input_system.h
        source/prototyping/ping_server_system.h
        source/communication/tcp_connection.h
        source/boost_communication/boost_tcp_connection.cc
        source/boost_communication/boost_tcp_connection.h
        source/boost_communication/boost_tcp_client.cc
        source/boost_communication/boost_tcp_client.h
        source/application/chat_tcp_system.h
        source/application/chat_persistence_system.h
        source/utility/concepts.h
        source/prototyping/fallback_message_handler.h
        source/communication/tcp_system_builder.h
        source/utility/variadic.h
        source/communication/tcp_system_concepts.h
        source/codec/test_codec.h
        source/application/messages/api.h
        source/application/messages/json_macros.h
        source/communication/commons.h
        source/cross_platform.h)

# Only define SERVER_SIDE if we are NOT building the client
if(NOT BUILD_CLIENT)
    target_compile_definitions(${TARGET_NAME} PRIVATE SERVER_SIDE)
endif()

if(WIN32)
    find_package(Boost REQUIRED COMPONENTS stacktrace_windbg)
else()
    find_package(Boost REQUIRED COMPONENTS stacktrace_backtrace)
endif()

find_package(SQLiteCpp CONFIG REQUIRED)
find_package(claw-core CONFIG REQUIRED)
find_package(claw-utility CONFIG REQUIRED)

if(WIN32)
    target_link_libraries(${TARGET_NAME} PRIVATE SQLiteCpp Boost::stacktrace_windbg claw::core)
else()
    target_link_libraries(${TARGET_NAME} PRIVATE SQLiteCpp Boost::stacktrace_backtrace claw::core)
endif()
