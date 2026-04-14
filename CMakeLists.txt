if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
    set(VCPKG_TARGET_TRIPLET "x64-windows-static")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/third_party/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
else()
    set(VCPKG_TARGET_TRIPLET "x64-linux-clang")
endif()

cmake_minimum_required(VERSION 3.28)
project(chatcore-server)
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

set(TARGET_NAME chat-server)

add_executable(${TARGET_NAME} source/main.cc
        source/application/chat_server_application.cpp
        source/application/chat_server_application.h
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
        source/application/chat_tcp_system.h
        source/application/chat_persistence_system.h
        source/users/user_server_system.cc
        source/users/user_server_system.h
        source/users/adapters/user_persistence_adapter.h
        source/users/adapters/sqlite_user_persistence_adapter.cc
        source/users/adapters/sqlite_user_persistence_adapter.h
        source/chat/chat_channel_store.cc
        source/chat/chat_channel_store.h
        source/chat/chat_message_store.cc
        source/chat/chat_message_store.h
        source/chat/chat_channel_message_log.cc
        source/chat/chat_channel_message_log.h
        source/application/application_system.cc
        source/application/application_system.h
        source/application/chat_server_arguments.cc
        source/application/chat_server_arguments.h
        source/application/connection_event_handler.cc
        source/application/connection_event_handler.h
        source/users/user_store.cc
        source/users/user_store.h
)

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
add_subdirectory("${CMAKE_SOURCE_DIR}/libs/chat-server-api")

find_package(Boost REQUIRED COMPONENTS ${BOOST_STACKTRACE})
find_package(SQLiteCpp CONFIG REQUIRED)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        SQLiteCpp
        Boost::${BOOST_STACKTRACE}
        claw::core
        claw::utility
        claw::network
        claw::network-modules
        claw::chat-api
        claw::scheduling
)